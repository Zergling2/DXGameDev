#include "Channel.h"
#include "Player.h"
#include "Session.h"
#include "Protocol.h"
#include <cassert>
#include <winppy/Network/TCPServer.h>

Channel::Channel(uint8_t id, size_t maxPlayer)
	: m_id(id)
	, m_maxPlayer(maxPlayer)
	, m_players()
	, m_nextGameRoomId(0)
	, m_gameRoomNoStk()
	, m_gameRooms()
{
	for (uint16_t i = 500; i > 0; --i)
		m_gameRoomNoStk.push(i);
}

bool Channel::AddPlayer(Player* pPlayer)
{
	if (m_players.size() == m_maxPlayer)
		return false;

	m_players.insert(pPlayer);
	pPlayer->MarkEnterChannel(m_id);

	return true;
}

void Channel::RemovePlayer(Player* pPlayer)
{
	auto iter = m_players.find(pPlayer);
	if (iter != m_players.end())
	{
		(*iter)->MarkExitChannel();
		m_players.erase(iter);
	}
}

void Channel::BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const
{
	for (const auto& pPlayer : m_players)
	{
		uint64_t netId = pPlayer->GetSession()->GetNetId();
		server.Send(netId, packet);
	}
}

GameRoom* Channel::CreateGameRoom(GameRoomTeamFormat tf, const wchar_t* roomName)
{
	if (m_gameRoomNoStk.empty())
		return nullptr;

	const uint16_t roomNo = m_gameRoomNoStk.top();
	m_gameRoomNoStk.pop();

	constexpr GameMap DEFAULT_GAME_MAP = GameMap::tdm_warehouse;
	std::unique_ptr<GameRoom> upNewGameRoom = std::make_unique<GameRoom>(this->CreateGameRoomId(), roomNo, tf, DEFAULT_GAME_MAP, roomName);
	GameRoom* pGameRoom = upNewGameRoom.get();

	m_gameRooms.insert(std::make_pair(upNewGameRoom->GetId(), std::move(upNewGameRoom)));
	return pGameRoom;
}

void Channel::RemoveGameRoom(uint64_t roomId)
{
	const auto iter = m_gameRooms.find(roomId);
	if (iter == m_gameRooms.end())
		return;

	const GameRoom* pGameRoom = iter->second.get();
	if (pGameRoom->GetNumOfPlayers() > 0)	// 사람이 있는데 방을 제거하는 경우
		*reinterpret_cast<int*>(0) = 0;

	m_gameRoomNoStk.push(pGameRoom->GetNo());	// 방 번호 재활용

	// 방 제거
	m_gameRooms.erase(iter);
}

GameRoom* Channel::FindRoom(uint64_t roomId) const
{
	const auto iter = m_gameRooms.find(roomId);
	if (iter == m_gameRooms.end())
		return nullptr;
	else
		return iter->second.get();
}

std::vector<winppy::Packet> Channel::CreateGameRoomListPackets(uint32_t reqContextNo) const
{
	std::vector<winppy::Packet> pkts;

	SCResGameRoomList res;
	res.m_reqContextNo = reqContextNo;

	auto iter = m_gameRooms.cbegin();
	do
	{
		winppy::Packet outPkt;
		outPkt->Write(static_cast<protocol_type>(Protocol::SC_RES_GAME_ROOM_LIST));
		outPkt->WriteBytes(&res, sizeof(res));

		while (iter != m_gameRooms.cend())
		{
			assert(iter->first == iter->second->GetId());	// 키(GameRoom ID), 밸류->GameRoom ID 일치 체크

			SCResGameRoomListItem resItem;

			if (outPkt->WriteableSize() < sizeof(resItem))
				break;

			const GameRoom* pGameRoom = iter->second.get();

			resItem.m_id = pGameRoom->GetId();
			resItem.m_no = pGameRoom->GetNo();
			resItem.m_tf = pGameRoom->GetTeamFormat();
			resItem.m_numOfPlayers = static_cast<uint8_t>(pGameRoom->GetNumOfPlayers());
			resItem.m_map = pGameRoom->GetMap();
			resItem.m_state = pGameRoom->GetState();
			resItem.m_nameLen = static_cast<uint16_t>(pGameRoom->GetName().length());
			assert(resItem.m_nameLen <= MAX_GAME_ROOM_NAME_LEN);
			wmemcpy(resItem.m_name, pGameRoom->GetName().c_str(), resItem.m_nameLen);

			if (!outPkt->WriteBytes(&resItem, sizeof(resItem)))
				break;

			++iter;
		}

		pkts.push_back(std::move(outPkt));
	} while (iter != m_gameRooms.cend());

	return pkts;
}

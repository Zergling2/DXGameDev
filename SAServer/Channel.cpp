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

void Channel::AddPlayer(winppy::TCPServer& server, Player* pPlayer)
{
	bool result;

	if (m_players.size() == m_maxPlayer)
	{
		result = false;
	}
	else
	{
		m_players.insert(pPlayer);
		pPlayer->MarkEnterChannel(m_id);

		result = true;
	}

	// 패킷 전송
	SCResJoinChannel res;
	res.m_result = result;

	winppy::Packet pktResJoinChannel;
	pktResJoinChannel->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_CHANNEL));
	pktResJoinChannel->WriteBytes(&res, sizeof(res));

	server.Send(pPlayer->GetSession()->GetNetId(), std::move(pktResJoinChannel));
}

void Channel::RemovePlayer(winppy::TCPServer& server, Player* pPlayer)
{
	auto iter = m_players.find(pPlayer);
	if (iter == m_players.end())
		return;


	(*iter)->MarkExitChannel();
	m_players.erase(iter);


	// 패킷 전송
	SCResExitGameChannel res;
	res.m_result = true;

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_CHANNEL));
	pkt->WriteBytes(&res, sizeof(res));

	server.Send(pPlayer->GetSession()->GetNetId(), std::move(pkt));
}

bool Channel::CreateGameRoom(winppy::TCPServer& server, GameRoomTeamFormat tf, const wchar_t* roomName, Player* pHost)
{
	if (m_gameRoomNoStk.empty())
		return false;

	// 게임 방 번호 발급
	const uint16_t roomNo = m_gameRoomNoStk.top();
	m_gameRoomNoStk.pop();




	constexpr GameMap DEFAULT_GAME_MAP = GameMap::tdm_warehouse;
	std::unique_ptr<GameRoom> upNewGameRoom = std::make_unique<GameRoom>(this->CreateGameRoomId(), roomNo, tf, DEFAULT_GAME_MAP, roomName);
	upNewGameRoom->AddPlayerAsHost(server, pHost);

	m_gameRooms.insert(std::make_pair(upNewGameRoom->GetId(), std::move(upNewGameRoom)));
	return true;
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

void Channel::SendGameRoomLists(winppy::TCPServer& server, const Player* pReceiver, uint32_t reqContextNo) const
{
	const uint64_t receiverNetId = pReceiver->GetSession()->GetNetId();

	SCResGameRoomList res;
	res.m_reqContextNo = reqContextNo;

	auto iter = m_gameRooms.cbegin();
	do
	{
		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_GAME_ROOM_LIST));
		pkt->WriteBytes(&res, sizeof(res));

		while (iter != m_gameRooms.cend())
		{
			assert(iter->first == iter->second->GetId());	// 키(GameRoom ID), 밸류->GameRoom ID 일치 체크

			SCResGameRoomListItem resItem;

			if (pkt->WriteableSize() < sizeof(resItem))
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

			assert(pkt->WriteBytes(&resItem, sizeof(resItem)));

			++iter;
		}

		server.Send(receiverNetId, std::move(pkt));
	} while (iter != m_gameRooms.cend());
}

void Channel::BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const
{
	for (const auto& pPlayer : m_players)
		server.Send(pPlayer->GetSession()->GetNetId(), packet);
}

void Channel::BroadcastPacketExceptInRoomPlayers(winppy::TCPServer& server, winppy::Packet packet) const
{
	for (const auto& pPlayer : m_players)
	{
		if (pPlayer->IsInRoom())
			continue;

		server.Send(pPlayer->GetSession()->GetNetId(), packet);
	}
}
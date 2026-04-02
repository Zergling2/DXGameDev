#include "ChJobReqGameRoomList.h"
#include "GameServer.h"
#include "GameChannel.h"
#include "GameRoom.h"
#include "Protocol.h"
#include <cassert>

void ChJobReqGameRoomList::Execute(GameChannel& channel)
{
	auto& gameRooms = channel.GameRooms();

	CSReqGameRoomList req;
	if (!m_packet->ReadBytes(&req, sizeof(req)))
	{
		m_server.Disconnect(m_netId);
		return;
	}

	const uint32_t gameRoomCount = static_cast<uint32_t>(gameRooms.size());
	auto iter = gameRooms.cbegin();

	SCResGameList res;
	res.m_reqContextNo = req.m_reqContextNo;
	do
	{
		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_GAME_LIST));
		outPacket->WriteBytes(&res, sizeof(res));

		while (iter != gameRooms.cend())
		{
			assert(iter->first == iter->second->GetRoomId());

			SCResGameListItem resItem;

			if (outPacket->WriteableSize() < sizeof(resItem))
				break;

			resItem.m_gameRoomId = iter->second->GetRoomId();
			resItem.m_gameRoomNo = iter->second->GetRoomNo();
			resItem.m_maxPlayer = iter->second->GetMaxPlayer();
			resItem.m_currPlayer = iter->second->GetRedTeamSessionsCount() + iter->second->GetBlueTeamSessionsCount();
			resItem.m_gameMap = iter->second->GetGameMap();
			resItem.m_gameMode = iter->second->GetGameMode();
			resItem.m_gameRoomState = iter->second->GetGameRoomState();
			resItem.m_gameRoomNameLen = iter->second->GetNameLen();
			wmemcpy(resItem.m_gameName, iter->second->GetName(), iter->second->GetNameLen());
			if (!outPacket->WriteBytes(&resItem, sizeof(resItem)))
				break;

			++iter;
		}

		m_server.Send(m_netId, std::move(outPacket));
	} while (iter != gameRooms.cend());
}

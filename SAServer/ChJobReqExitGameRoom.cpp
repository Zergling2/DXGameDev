#include "ChJobReqExitGameRoom.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "GameServer.h"
#include "Protocol.h"
#include <winppy/Network/Packet.h>
#include <cassert>

void ChJobReqExitGameRoom::Execute(GameChannel& channel)
{
	std::shared_ptr<GameRoom> spGameRoom = m_spSession->GetJoiningGameRoom();
	if (!spGameRoom)
		return;

	bool found = false;
	do
	{
		for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
		{
			if (spGameRoom->m_redTeamSessions[i] == m_spSession)
			{
				found = true;

				// 한 칸씩 당기기
				for (uint8_t j = i + 1; j < spGameRoom->m_redTeamSessionsCount; ++j)
					spGameRoom->m_redTeamSessions[j - 1] = spGameRoom->m_redTeamSessions[j];

				spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount - 1].reset();

				--spGameRoom->m_redTeamSessionsCount;
				break;
			}
		}

		if (found)
			break;

		for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
		{
			if (spGameRoom->m_blueTeamSessions[i] == m_spSession)
			{
				found = true;

				// 한 칸씩 당기기
				for (uint8_t j = i + 1; j < spGameRoom->m_blueTeamSessionsCount; ++j)
					spGameRoom->m_blueTeamSessions[j - 1] = spGameRoom->m_blueTeamSessions[j];

				spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount - 1].reset();

				--spGameRoom->m_blueTeamSessionsCount;
				break;
			}
		}
	} while (false);
	m_spSession->SetJoiningGameRoom(nullptr);

	SCResExitGameRoom res;
	res.m_result = true;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_ROOM));
	outPacket->WriteBytes(&res, sizeof(res));
	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));

	// 방에 혼자 남아있던 경우
	if (spGameRoom->m_redTeamSessionsCount + spGameRoom->m_blueTeamSessionsCount == 0)
	{
		channel.GameRooms().erase(spGameRoom->GetRoomId());
		// spGameRoom.reset();	// 암시적으로 일어나므로 생략
		return;
	}

	
	if (spGameRoom->m_spHost == m_spSession)
	{
		// 방장인데 나가는 경우
		spGameRoom->m_spHost.reset();

		// 새로운 방장
		if (spGameRoom->m_redTeamSessionsCount > 0)
			spGameRoom->m_spHost = spGameRoom->m_redTeamSessions[0];
		else
			spGameRoom->m_spHost = spGameRoom->m_blueTeamSessions[0];
	
		// 방에 있는 다른 플레이어들에게 방장이 변경되었다는 패킷 전송
		winppy::Packet notifyHostChangedPacket;
		notifyHostChangedPacket->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_HOST_CHANGED));
		notifyHostChangedPacket->Write(spGameRoom->GetRoomId());
		notifyHostChangedPacket->Write(spGameRoom->m_spHost->GetNetId());	// 새 방장의 net id 전달

		for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
			m_server.Send(spGameRoom->m_redTeamSessions[i]->GetNetId(), notifyHostChangedPacket);
		for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
			m_server.Send(spGameRoom->m_blueTeamSessions[i]->GetNetId(), notifyHostChangedPacket);

		assert(spGameRoom->m_spHost);
	}

	// 방 나가기에 성공했다는 패킷 전송
	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));

	
	// 방에 있던 다른 플레이어들에게 플레이어가 나갔다는 패킷 전송
	winppy::Packet notifyPlayerExitGameRoom;
	notifyPlayerExitGameRoom->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_EXIT_GAME_ROOM));
	notifyPlayerExitGameRoom->Write(spGameRoom->GetRoomId());
	notifyPlayerExitGameRoom->Write(m_spSession->GetNetId());
	for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
		m_server.Send(spGameRoom->m_redTeamSessions[i]->GetNetId(), notifyPlayerExitGameRoom);
	for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
		m_server.Send(spGameRoom->m_blueTeamSessions[i]->GetNetId(), notifyPlayerExitGameRoom);
}

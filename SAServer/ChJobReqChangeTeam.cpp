#include "ChJobReqChangeTeam.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "GameServer.h"
#include "Protocol.h"

void ChJobReqChangeTeam::Execute(GameChannel& channel)
{
	CSReqChangeTeam req;

	// 패킷 유효성 검사
	if (!m_packet->ReadBytes(&req, sizeof(req)))
	{
		m_server.Disconnect(m_netId);
		return;
	}

	if (static_cast<uint8_t>(req.m_newTeam) >= static_cast<uint8_t>(GameTeam::Unknown))
	{
		m_server.Disconnect(m_netId);
		return;
	}

	// 팀 변경 가능 여부 검사
	bool success = false;
	do
	{
		std::shared_ptr<GameRoom> spGameRoom = m_spSession->GetJoiningGameRoom();
		if (!spGameRoom)
			break;

		const size_t maxPlayerPerTeam = static_cast<size_t>(spGameRoom->GetMaxPlayer()) + 1;
		if (req.m_newTeam == GameTeam::BlueTeam)
		{
			if (spGameRoom->m_blueTeamSessionsCount == maxPlayerPerTeam)
				break;

			bool found = false;
			uint8_t sessionIndex;
			for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_redTeamSessions[i]->GetNetId() == m_netId)
				{
					sessionIndex = i;
					found = true;
					break;
				}
			}

			if (!found)
				break;

			std::shared_ptr<GameSession> spSession = std::move(spGameRoom->m_redTeamSessions[sessionIndex]);
			for (uint8_t i = sessionIndex + 1; i < spGameRoom->m_redTeamSessionsCount; ++i)
				spGameRoom->m_redTeamSessions[i - 1] = std::move(spGameRoom->m_redTeamSessions[i]);

			--spGameRoom->m_redTeamSessionsCount;

			spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount] = std::move(spSession);
			++spGameRoom->m_blueTeamSessionsCount;

			success = true;
		}
		else
		{
			if (spGameRoom->m_redTeamSessionsCount == maxPlayerPerTeam)
				break;

			bool found = false;
			uint8_t sessionIndex;
			for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_blueTeamSessions[i]->GetNetId() == m_netId)
				{
					sessionIndex = i;
					found = true;
					break;
				}
			}

			if (!found)
				break;

			std::shared_ptr<GameSession> spSession = std::move(spGameRoom->m_blueTeamSessions[sessionIndex]);
			for (uint8_t i = sessionIndex + 1; i < spGameRoom->m_blueTeamSessionsCount; ++i)
				spGameRoom->m_blueTeamSessions[i - 1] = std::move(spGameRoom->m_blueTeamSessions[i]);

			--spGameRoom->m_blueTeamSessionsCount;

			spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount] = std::move(spSession);
			++spGameRoom->m_redTeamSessionsCount;

			success = true;
		}


		SCResChangeTeam res;
		if (success)
		{
			res.m_result = true;
			res.m_newTeam = req.m_newTeam;

			SCNotifyPlayerTeamChanged notify;
			notify.m_netId = m_netId;
			notify.m_gameRoomId = spGameRoom->GetRoomId();
			notify.m_newTeam = req.m_newTeam;

			winppy::Packet notifyPacket;
			notifyPacket->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_TEAM_CHANGED));
			notifyPacket->WriteBytes(&notify, sizeof(notify));
			// 방의 모든 사람들에게 m_netId에 대한 팀 변경을 알림.

			for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_redTeamSessions[i]->GetNetId() == m_netId)
					continue;

				m_server.Send(spGameRoom->m_redTeamSessions[i]->GetNetId(), notifyPacket);
			}
			for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_blueTeamSessions[i]->GetNetId() == m_netId)
					continue;

				m_server.Send(spGameRoom->m_blueTeamSessions[i]->GetNetId(), notifyPacket);
			}
		}
		else
		{
			res.m_result = false;
		}

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_CHANGE_TEAM));
		outPacket->WriteBytes(&res, sizeof(res));
		m_server.Send(m_netId, std::move(outPacket));
	} while (false);
}

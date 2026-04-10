#include "ChJobReqChangeTeam.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "GameServer.h"
#include "Protocol.h"
#include <cassert>

void ChJobReqChangeTeam::Execute(GameChannel& channel)
{
	CSReqChangeTeam req;

	// 패킷 유효성 검사
	if (!m_packet->ReadBytes(&req, sizeof(req)))
	{
		m_server.Disconnect(m_spSession->GetNetId());
		return;
	}

	if (static_cast<uint8_t>(req.m_newTeam) >= static_cast<uint8_t>(GameTeam::Unknown))
	{
		m_server.Disconnect(m_spSession->GetNetId());
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
			if (m_spSession->GetCurrTeam() == GameTeam::BlueTeam)
				break;

			if (spGameRoom->m_blueTeamSessionsCount == maxPlayerPerTeam)
				break;

			assert(spGameRoom->m_redTeamSessions[m_spSession->GetCurrTeamIndex()] == m_spSession);

			const uint8_t removalIndex = m_spSession->GetCurrTeamIndex();
			const uint8_t lastIndex = spGameRoom->m_redTeamSessionsCount - 1;
			if (removalIndex != lastIndex)
			{
				std::swap(spGameRoom->m_redTeamSessions[removalIndex], spGameRoom->m_redTeamSessions[lastIndex]);
				spGameRoom->m_redTeamSessions[removalIndex]->SetCurrTeamIndex(removalIndex);
			}

			std::shared_ptr<GameSession> spSession = std::move(spGameRoom->m_redTeamSessions[lastIndex]);
			--spGameRoom->m_redTeamSessionsCount;

			
			spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount] = std::move(spSession);
			spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount]->SetCurrTeam(GameTeam::BlueTeam);
			spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount]->SetCurrTeamIndex(spGameRoom->m_blueTeamSessionsCount);
			++spGameRoom->m_blueTeamSessionsCount;

			success = true;
		}
		else
		{
			if (m_spSession->GetCurrTeam() == GameTeam::RedTeam)
				break;

			if (spGameRoom->m_redTeamSessionsCount == maxPlayerPerTeam)
				break;

			assert(spGameRoom->m_blueTeamSessions[m_spSession->GetCurrTeamIndex()] == m_spSession);

			const uint8_t removalIndex = m_spSession->GetCurrTeamIndex();
			const uint8_t lastIndex = spGameRoom->m_blueTeamSessionsCount - 1;
			if (removalIndex != lastIndex)
			{
				std::swap(spGameRoom->m_blueTeamSessions[removalIndex], spGameRoom->m_blueTeamSessions[lastIndex]);
				spGameRoom->m_blueTeamSessions[removalIndex]->SetCurrTeamIndex(removalIndex);
			}

			std::shared_ptr<GameSession> spSession = std::move(spGameRoom->m_blueTeamSessions[lastIndex]);
			--spGameRoom->m_blueTeamSessionsCount;


			spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount] = std::move(spSession);
			spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount]->SetCurrTeam(GameTeam::RedTeam);
			spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount]->SetCurrTeamIndex(spGameRoom->m_redTeamSessionsCount);
			++spGameRoom->m_redTeamSessionsCount;

			success = true;
		}


		SCResChangeTeam res;
		if (success)
		{
			res.m_result = true;
			res.m_newTeam = m_spSession->GetCurrTeam();

			SCNotifyPlayerTeamChanged notify;
			notify.m_netId = m_spSession->GetNetId();
			notify.m_gameRoomId = spGameRoom->GetRoomId();
			notify.m_newTeam = m_spSession->GetCurrTeam();

			winppy::Packet notifyPacket;
			notifyPacket->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_TEAM_CHANGED));
			notifyPacket->WriteBytes(&notify, sizeof(notify));
			// 방의 모든 사람들에게 m_netId에 대한 팀 변경을 알림.

			for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_redTeamSessions[i]->GetNetId() == m_spSession->GetNetId())
					continue;

				m_server.Send(spGameRoom->m_redTeamSessions[i]->GetNetId(), notifyPacket);
			}
			for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
			{
				if (spGameRoom->m_blueTeamSessions[i]->GetNetId() == m_spSession->GetNetId())
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
		m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
	} while (false);
}

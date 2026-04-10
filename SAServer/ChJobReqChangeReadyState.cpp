#include "ChJobReqChangeReadyState.h"
#include "GameSession.h"
#include "GameServer.h"
#include "GameRoom.h"
#include "Protocol.h"

void ChJobReqChangeReadyState::Execute(GameChannel& channel)
{
	uint8_t req;
	if (!m_packet->Read(&req))
	{
		m_server.Disconnect(m_spSession->GetNetId());
		return;
	}

	bool ready = req != 0;

	std::shared_ptr<GameRoom> spGameRoom = m_spSession->GetJoiningGameRoom();
	if (!spGameRoom)
	{
		m_server.Disconnect(m_spSession->GetNetId());
		return;
	}

	if (m_spSession->GetReadyState() == ready)
		return;

	m_spSession->SetReadyState(ready);

	// 패킷 전송
	winppy::Packet outPacket;
	if (ready)
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_GAME_READY));
	else
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_GAME_UNREADY));

	outPacket->Write(spGameRoom->GetRoomId());
	outPacket->Write(m_spSession->GetNetId());

	for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
		m_server.Send(spGameRoom->m_redTeamSessions[i]->GetNetId(), outPacket);

	for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
		m_server.Send(spGameRoom->m_blueTeamSessions[i]->GetNetId(), outPacket);
}

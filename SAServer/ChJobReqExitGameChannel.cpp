#include "ChJobReqExitGameChannel.h"
#include "GameSession.h"
#include "GameServer.h"
#include "ChJobReqExitGameRoom.h"
#include "Protocol.h"

void ChJobReqExitGameChannel::Execute(GameChannel& channel)
{
	if (m_spSession->GetJoiningGameRoom())
	{
		ChJobReqExitGameRoom job(m_server, m_spSession);
		job.Execute(channel);
	}

	m_spSession->SetJoiningGameChannel(nullptr);
	channel.RemoveSession(m_spSession->GetNetId());
	
	SCResExitGameChannel res;
	res.m_result = true;
	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_CHANNEL));
	outPacket->WriteBytes(&res, sizeof(res));
	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
}

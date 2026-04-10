#include "ChJobReqJoinChannel.h"
#include "GameSession.h"
#include "GameServer.h"
#include "Protocol.h"

void ChJobReqJoinChannel::Execute(GameChannel& channel)
{
	SCResJoinChannel res;

	if (channel.GetSessionCount() < MAX_SESSIONS_PER_CHANNEL)
	{
		res.m_result = true;

		channel.AddSession(m_spSession->GetNetId(), m_spSession);
		m_spSession->SetJoiningGameChannel(&channel);

		if (m_spSession->GetDisconnectedFlag())
		{
			channel.RemoveSession(m_spSession->GetNetId());
			m_spSession->SetJoiningGameChannel(nullptr);
			return;
		}
	}
	else
	{
		res.m_result = false;
	}

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_CHANNEL));
	outPacket->WriteBytes(&res, sizeof(res));

	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
}

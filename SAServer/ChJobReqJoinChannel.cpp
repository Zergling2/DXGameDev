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

		channel.AddSession(m_netId, m_spSession);
		if (!m_spSession->SetJoiningGameChannel(&channel))	// 접속한 채널의 포인터를 세션 구조체에 저장
		{
			channel.RemoveSession(m_netId);
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

	m_server.Send(m_netId, std::move(outPacket));
}

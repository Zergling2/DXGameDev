#include "ChJobReqExitGameChannel.h"
#include "GameSession.h"
#include "GameServer.h"
#include "ChJobReqExitGameRoom.h"

void ChJobReqExitGameChannel::Execute(GameChannel& channel)
{
	if (m_spSession->GetJoiningGameRoom())
	{
		ChJobReqExitGameRoom job(m_server, m_spSession);
		job.Execute(channel);
	}

	// 123
}

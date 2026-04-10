#include "ChJobReqHostGameStart.h"
#include "GameSession.h"
#include "GameServer.h"
#include "Protocol.h"

void ChJobReqHostGameStart::Execute(GameChannel& channel)
{
	// 1. m_spSession이 방에 참여중인지 검사.
	// 2. m_spSession이 방장인지 검사.
	// 3. m_spSession의 상대편 플레이어 중 적어도 한 명 이상이 준비완료 상태인지 검사.

	SCResHostGameStart res;
	res.m_result = false;
	res.m_reason = FailReason::NotReady;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_HOST_GAME_START));
	outPacket->WriteBytes(&res, sizeof(res));

	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
}

#include "ChJobReqSendChatMsg.h"
#include "GameServer.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "Protocol.h"

void ChJobReqSendChatMsg::Execute(GameChannel& channel)
{
	bool disconnect = true;

	do
	{
		CSReqSendChatMsg req;
		if (!m_packet->ReadBytes(&req, sizeof(req)))
			break;

		if (req.m_chatMsgLen > MAX_CHAT_MSG_LEN)
			break;

		wchar_t chatMsg[MAX_CHAT_MSG_LEN];
		if (!m_packet->ReadBytes(chatMsg, sizeof(wchar_t) * req.m_chatMsgLen))
			break;

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_SEND_CHAT_MSG));
		outPacket->Write(m_spSession->GetNicknameLen());
		outPacket->Write(req.m_chatMsgLen);
		outPacket->WriteBytes(m_spSession->GetNickname(), sizeof(wchar_t) * m_spSession->GetNicknameLen());
		outPacket->WriteBytes(chatMsg, sizeof(wchar_t) * req.m_chatMsgLen);

		std::shared_ptr<GameRoom> spJoiningGameRoom = m_spSession->GetJoiningGameRoom();
		if (spJoiningGameRoom)
		{
			// 방에 브로드캐스트
			for (size_t i = 0; i < spJoiningGameRoom->GetRedTeamSessionsCount(); ++i)
				m_server.Send(spJoiningGameRoom->GetRedTeamSessionPtr(i)->GetNetId(), outPacket);

			for (size_t i = 0; i < spJoiningGameRoom->GetBlueTeamSessionsCount(); ++i)
				m_server.Send(spJoiningGameRoom->GetBlueTeamSessionPtr(i)->GetNetId(), outPacket);
		}
		else
		{
			// 채널에 브로드캐스트
			auto iter = channel.Sessions().cbegin();
			while (iter != channel.Sessions().cend())
			{
				m_server.Send(iter->second->GetNetId(), outPacket);
				++iter;
			}
		}

		disconnect = false;
	} while (false);

	if (disconnect)
		m_server.Disconnect(m_netId);
}

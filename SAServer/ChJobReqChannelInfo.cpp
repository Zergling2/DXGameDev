#include "ChJobReqChannelInfo.h"
#include "GameChannel.h"
#include "Protocol.h"
#include "GameServer.h"

void ChJobReqChannelInfo::Execute(GameChannel& channel)
{
	SCResChannelInfo res;
	res.m_serverId = 0;
	res.m_channelId = channel.GetChannelId();
	res.m_sessionCount = static_cast<uint16_t>(channel.GetSessionCount());;
	res.m_maxSession = MAX_SESSIONS_PER_CHANNEL;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_CHANNEL_INFO));
	outPacket->WriteBytes(&res, sizeof(res));

	m_server.Send(m_netId, std::move(outPacket));
}

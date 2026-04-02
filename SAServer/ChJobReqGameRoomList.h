#pragma once

#include <winppy/Network/Packet.h>
#include <memory>
#include <utility>
#include "ChannelJob.h"

class GameServer;
class GameChannel;

class ChJobReqGameRoomList : public IChannelJob
{
public:
	ChJobReqGameRoomList(GameServer& server, uint64_t netId, winppy::Packet packet)
		: m_server(server)
		, m_netId(netId)
		, m_packet(std::move(packet))
	{
	}
	virtual ~ChJobReqGameRoomList() = default;

	virtual void Execute(GameChannel& channel) override;
private:
	GameServer& m_server;
	uint64_t m_netId;	// 요청 세션의 net id
	winppy::Packet m_packet;
};

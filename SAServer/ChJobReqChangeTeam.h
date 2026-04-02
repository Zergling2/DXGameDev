#pragma once

#include <winppy/Network/Packet.h>
#include <memory>
#include <utility>
#include "ChannelJob.h"

class GameServer;
class GameChannel;
class GameSession;

class ChJobReqChangeTeam : public IChannelJob
{
public:
	ChJobReqChangeTeam(GameServer& server, uint64_t netId, winppy::Packet packet, std::shared_ptr<GameSession> spSession)
		: m_server(server)
		, m_netId(netId)
		, m_packet(std::move(packet))
		, m_spSession(std::move(spSession))
	{
	}
	virtual ~ChJobReqChangeTeam() = default;

	virtual void Execute(GameChannel& channel) override;
private:
	GameServer& m_server;
	uint64_t m_netId;	// 요청 세션 net id
	winppy::Packet m_packet;
	std::shared_ptr<GameSession> m_spSession;
};

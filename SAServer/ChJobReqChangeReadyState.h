#pragma once

#include <winppy/Network/Packet.h>
#include <memory>
#include <utility>
#include "ChannelJob.h"

class GameServer;
class GameSession;
class GameChannel;

class ChJobReqChangeReadyState : public IChannelJob
{
public:
	ChJobReqChangeReadyState(GameServer& server, winppy::Packet packet, std::shared_ptr<GameSession> spSession)
		: m_server(server)
		, m_packet(std::move(packet))
		, m_spSession(std::move(spSession))
	{
	}
	virtual ~ChJobReqChangeReadyState() = default;

	virtual void Execute(GameChannel& channel) override;
private:
	GameServer& m_server;
	winppy::Packet m_packet;
	std::shared_ptr<GameSession> m_spSession;
};

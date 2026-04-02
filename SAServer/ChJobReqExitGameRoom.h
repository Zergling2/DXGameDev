#pragma once

#include <memory>
#include <utility>
#include "ChannelJob.h"

class GameServer;
class GameSession;
class GameChannel;

class ChJobReqExitGameRoom : public IChannelJob
{
public:
	ChJobReqExitGameRoom(GameServer& server, uint64_t netId, std::shared_ptr<GameSession> spSession)
		: m_server(server)
		, m_netId(netId)
		, m_spSession(std::move(spSession))
	{
	}
	virtual ~ChJobReqExitGameRoom() = default;

	virtual void Execute(GameChannel& channel) override;
private:
	GameServer& m_server;
	uint64_t m_netId;	// 요청 세션 net id
	std::shared_ptr<GameSession> m_spSession;
};

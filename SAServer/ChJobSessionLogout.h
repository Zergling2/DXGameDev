#pragma once

#include <memory>
#include <utility>
#include "ChannelJob.h"

class GameServer;
class GameSession;
class GameChannel;

class ChJobSessionLogout : public IChannelJob
{
public:
	ChJobSessionLogout(GameServer& server, std::shared_ptr<GameSession> spSession)
		: m_server(server)
		, m_spSession(std::move(spSession))
	{
	}
	virtual ~ChJobSessionLogout() = default;

	virtual void Execute(GameChannel& channel) override;
private:
	GameServer& m_server;
	std::shared_ptr<GameSession> m_spSession;	// 요청 세션
};

#pragma once

#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include <unordered_map>
#include <memory>
#include "SlimRWLock.h"
#include "GameChannel.h"
#include "Constants.h"

class GameSession;

class GameServer : public winppy::TCPServer
{
public:
	GameServer();
	virtual ~GameServer() = default;
	virtual bool OnConnect(const wchar_t* ip, uint16_t port, uint64_t id) override;
	virtual void OnReceive(uint64_t id, winppy::Packet packet) override;
	virtual void OnDisconnect(uint64_t id) override;

	bool PktProcCSReqLogin(uint64_t id, winppy::Packet packet);
	bool PktProcCSReqChannelInfo(uint64_t id, winppy::Packet packet);
	bool PktProcCSReqJoinChannel(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqSendChatMsg(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqGameList(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqCreateGameRoom(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqJoinGameRoom(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqExitGameRoom(uint64_t id, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqChangeTeam(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
private:
	std::shared_ptr<GameSession> FindSession(uint64_t netId) const;
private:
	GameChannel m_channel[CHANNEL_COUNT];
	mutable SlimRWLock m_sessionMapLock;
	std::unordered_map<uint64_t, std::shared_ptr<GameSession>> m_sessions;
};

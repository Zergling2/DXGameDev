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
	bool PktProcCSReqJoinChannel(winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqSendChatMsg(winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqGameList(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqCreateGameRoom(winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqJoinGameRoom(winppy::Packet packet, std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqExitGameRoom(std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqExitGameChannel(std::shared_ptr<GameSession> spSession);
	bool PktProcCSReqChangeTeam(winppy::Packet packet, std::shared_ptr<GameSession> spSession);
private:
	std::shared_ptr<GameSession> FindSession(uint64_t netId) const;
private:
	GameChannel m_channel[CHANNEL_COUNT];
	mutable SlimRWLock m_sessionMapLock;
	std::unordered_map<uint64_t, std::shared_ptr<GameSession>> m_sessions;
};

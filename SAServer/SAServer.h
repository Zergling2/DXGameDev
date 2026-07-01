#pragma once

#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include "Constants.h"
#include "DBThread.h"
#include "LogicThread.h"

class SAServer : public winppy::TCPServer
{
public:
	SAServer();
	virtual ~SAServer() = default;
	virtual bool OnConnect(const wchar_t* ip, uint16_t port, uint64_t id) override;
	virtual void OnReceive(uint64_t id, winppy::Packet packet) override;
	virtual void OnDisconnect(uint64_t id) override;
	
	void OnCSReqLogin(uint64_t netId, winppy::Packet packet);
	void OnCSReqIdDuplicateCheck(uint64_t netId, winppy::Packet packet);
	void OnCSReqNicknameDuplicateCheck(uint64_t netId, winppy::Packet packet);
	void OnCSReqCreateAccount(uint64_t netId, winppy::Packet packet);
	void OnCSReqChannelInfo(uint64_t netId, winppy::Packet packet);
	void OnCSReqJoinChannel(uint64_t netId, winppy::Packet packet);
	void OnCSReqLobbyChat(uint64_t netId, winppy::Packet packet);
	void OnCSReqGameRoomList(uint64_t netId, winppy::Packet packet);
	void OnCSReqCreateGameRoom(uint64_t netId, winppy::Packet packet);
	void OnCSReqJoinGameRoom(uint64_t netId, winppy::Packet packet);
	void OnCSReqChangeTeam(uint64_t netId, winppy::Packet packet);
	void OnCSReqExitGameRoom(uint64_t netId, winppy::Packet packet);
	void OnCSReqExitGameChannel(uint64_t netId, winppy::Packet packet);
	void OnCSReqHostGameStart(uint64_t netId, winppy::Packet packet);
	void OnCSReqGameReady(uint64_t netId, winppy::Packet packet);
	void OnCSReqGameUnready(uint64_t netId, winppy::Packet packet);
private:
	DBThread m_dbThread[DB_THREAD_COUNT];
	LogicThread m_logic;
};

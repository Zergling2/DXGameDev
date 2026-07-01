#pragma once

#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include <memory>
#include "Constants.h"

class DBThread;
class LogicThread;

class SAServer : public winppy::TCPServer
{
public:
	SAServer();
	virtual ~SAServer();
	virtual bool OnConnect(const wchar_t* ip, uint16_t port, uint64_t id) override;
	virtual void OnReceive(uint64_t id, winppy::Packet packet) override;
	virtual void OnDisconnect(uint64_t id) override;

	DBThread& GetDBThread(size_t index) { return *m_dbThreads[index].get(); }
	
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
	std::vector<std::unique_ptr<DBThread>> m_dbThreads;
	std::unique_ptr<LogicThread> m_logicThread;
};

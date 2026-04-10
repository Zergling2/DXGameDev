#pragma once

#include <ZergEngine\ZergEngine.h>
#include "winppy/Network/TCPServer.h"
#include "winppy/Network/TCPClientEngine.h"
#include "winppy/Network/TCPClient.h"
#include "winppy/Network/Packet.h"
#include <queue>

class Network;
class LobbyHandler;
class Account;

class ListenServer : public winppy::TCPServer
{
public:
	ListenServer(Network& network)
		: m_network(network)
	{
	}
	virtual ~ListenServer() = default;

	virtual bool OnConnect(const wchar_t* ip, uint16_t port, uint64_t id) override { return true; }
	virtual void OnReceive(uint64_t id, winppy::Packet packet) override {}
	virtual void OnDisconnect(uint64_t id) override {}
private:
	Network& m_network;
};

class Client : public winppy::TCPClient
{
public:
	Client(Network& network)
		: m_network(network)
	{
	}
	virtual void OnConnect() override;
	virtual void OnReceive(winppy::Packet packet) override;
	virtual void OnDisconnect() override;
private:
	Network& m_network;
};

class Network : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
	friend class Client;
public:
	Network(ze::GameObject& owner);
	virtual ~Network() = default;

	Client& GetClient() { return m_client; }

	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	uint64_t GetNetId() const { return m_netId; }
private:
	void PktProcSCResLogin(winppy::Packet packet);
	void PktProcSCResChannelInfo(winppy::Packet packet);
	void PktProcSCResJoinChannel(winppy::Packet packet);
	void PktProcSCResSendChatMsg(winppy::Packet packet);
	void PktProcSCResGameList(winppy::Packet packet);
	void PktProcSCResCreateGameRoom(winppy::Packet packet);
	void PktProcSCResJoinGameRoom(winppy::Packet packet);
	void PktProcSCResChangeTeam(winppy::Packet packet);
	void PktProcSCResHostGameStart(winppy::Packet packet);
	void PktProcSCResExitGameRoom(winppy::Packet packet);
	void PktProcSCResExitGameChannel(winppy::Packet packet);
	void PktProcSCNotifyPlayerTeamChanged(winppy::Packet packet);
	void PktProcSCNotifyPlayerJoinedGameRoom(winppy::Packet packet);
	void PktProcSCNotifyPlayerExitGameRoom(winppy::Packet packet);
	void PktProcSCNotifyGameRoomPlayer(winppy::Packet packet);
	void PktProcSCNotifyHostChanged(winppy::Packet packet);
	//void PktProcSCNotifyHostGameStart(winppy::Packet packet);
	void PktProcSCNotifyPlayerGameReady(winppy::Packet packet);
	void PktProcSCNotifyPlayerGameUnready(winppy::Packet packet);
private:
	ListenServer m_listenServer;
	winppy::TCPClientEngine m_ce;
	Client m_client;
	SRWLOCK m_lock;	// m_packetQueue¿ë ¶ô
	std::queue<winppy::Packet> m_packetQueue;
	bool m_connected;
	bool m_disconnectJobDone;
	uint64_t m_netId;
public:
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<LobbyHandler> m_hScriptLobbyHandler;
};

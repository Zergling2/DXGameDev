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
private:
	void PktProcSCResLogin(winppy::Packet packet);
	void PktProcSCResBroadcastChatMsg(winppy::Packet packet);
private:
	ListenServer m_listenServer;
	winppy::TCPClientEngine m_ce;
	Client m_client;
	SRWLOCK m_lock;	// m_packetQueue¿ë ¶ô
	std::queue<winppy::Packet> m_packetQueue;
public:
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<LobbyHandler> m_hScriptLobbyHandler;
};

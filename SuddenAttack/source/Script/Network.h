#pragma once

#include <ZergEngine\ZergEngine.h>
#include "winppy/Network/TCPClientEngine.h"
#include "winppy/Network/TCPClient.h"
#include "winppy/Network/Packet.h"
#include <queue>

class Network;

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

	virtual void Start() override;
	virtual void OnDestroy() override;
private:
	winppy::TCPClientEngine m_ce;
	Client m_client;
	SRWLOCK m_lock;
	std::queue<winppy::Packet> m_packetQueue;
};

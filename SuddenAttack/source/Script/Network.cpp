#include "Network.h"
#include "../Common/Constants.h"

using namespace ze;

void Client::OnConnect()
{
	printf("Connected.\n");
}

void Client::OnReceive(winppy::Packet packet)
{
	AcquireSRWLockExclusive(&m_network.m_lock);
	m_network.m_packetQueue.push(std::move(packet));
	ReleaseSRWLockExclusive(&m_network.m_lock);
}

void Client::OnDisconnect()
{
	printf("Disconnected.\n");
	this->Release();
}

Network::Network(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_client(*this)
{
	InitializeSRWLock(&m_lock);
}

void Network::Start()
{
	winppy::TCPClientEngineConfig config;
	config.m_headerCode = HEADER_CODE;
	config.m_numOfConcurrentThreads = 1;
	config.m_numOfWorkerThreads = 1;
	m_ce.Init(config);

	winppy::TCPClientInitDesc desc;
	desc.m_pEngine = &m_ce;
	desc.m_tcpNoDelay = true;
	m_client.Init(desc);

	m_client.Connect(L"127.0.0.1", 37015);
}

void Network::OnDestroy()
{
	m_client.Disconnect();
	m_client.Release();

	m_ce.Release();
}

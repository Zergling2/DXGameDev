#include "ListenServerClient.h"
#include "Constants.h"
#include <WS2tcpip.h>

using namespace ze;

ListenServerClient::ListenServerClient(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_pClient(nullptr)
	, m_pPeer(nullptr)
	, m_hScriptNetwork()
{
}

void ListenServerClient::Update()
{
	if (!m_pClient)
		return;

	ENetEvent event;
	IN_ADDR peerAddr;
	wchar_t ipStr[22];
	while (enet_host_service(m_pClient, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_NONE:
			break;
		case ENET_EVENT_TYPE_CONNECT:
			peerAddr.s_addr = event.peer->address.host;
			if (!InetNtopW(AF_INET, &peerAddr.s_addr, ipStr, _countof(ipStr)))
				ipStr[0] = L'\0';
			wprintf(L"ENET_EVENT_TYPE_CONNECT %s:%u.\n", ipStr, static_cast<uint32_t>(event.peer->address.port));
			// /* Store any relevant client information here. */
			// event.peer->data = "Peer information";
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("ENET_EVENT_TYPE_RECEIVE\n");
			// printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
			// 	event.packet->dataLength,
			// 	event.packet->data,
			// 	event.peer->data,
			// 	event.channelID);
			// 
			// /* Clean up the packet now that we're done using it. */
			// enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			printf("ENET_EVENT_TYPE_DISCONNECT\n");
			// printf("%s disconnected.\n", event.peer->data);
			// 
			// /* Reset the peer's client information. */
			// 
			// event.peer->data = NULL;
			break;
		}
	}
}

void ListenServerClient::OnDestroy()
{
	this->CloseClient();
}

void ListenServerClient::StartClient(uint32_t serverIP, uint16_t serverPort)
{
	assert(m_pClient == nullptr);

	if (m_pClient)
		*reinterpret_cast<int*>(0) = 0;

	m_pClient = enet_host_create(
		nullptr,
		1,	// 1 outgoint connection
		UDP_CHANNEL_COUNT,
		0,
		0
	);

	if (m_pClient == nullptr)
	{
		// SAServer로 리슨서버 클라이언트 생성 실패 패킷 전송
		// SAServer는 이 패킷 수신 시 리슨서버 방장에게 알린다. (알리지 않아도 될지도?)
		// 그리고 해당 플레이어의 PlayerState도 None으로 변경한다. (게임중 -> 상태 없음) 으로 전환해서 플레이어가 방을 나가거나 할 수 있게 유도한다.

		return;
	}


	// The host must be specified in network byte - order, and the port must be in host byte - order.
	ENetAddress listenServerAddr;
	listenServerAddr.host = htonl(serverIP);
	listenServerAddr.port = serverPort;

	m_pPeer = enet_host_connect(m_pClient, &listenServerAddr, UDP_CHANNEL_COUNT, 0);
	if (!m_pPeer)
	{
		printf("No available peers for initializing an ENet connection.\n");
		return;
	}

	ENetEvent event;
	IN_ADDR peerAddr;
	wchar_t ipStr[22];
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_pClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		peerAddr.s_addr = event.peer->address.host;
		if (!InetNtopW(AF_INET, &peerAddr.s_addr, ipStr, _countof(ipStr)))
			ipStr[0] = L'\0';
		wprintf(L"ENET_EVENT_TYPE_CONNECT %s:%u.\n", ipStr, static_cast<uint32_t>(event.peer->address.port));

		puts("Connection to listen server succeeded.");
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(m_pPeer);

		puts("Connection to listen server failed.");
	}
}

void ListenServerClient::CloseClient()
{
	if (m_pPeer)
	{
		enet_peer_reset(m_pPeer);
		m_pPeer = nullptr;
	}

	if (m_pClient)
	{
		enet_host_destroy(m_pClient);
		m_pClient = nullptr;
	}
}

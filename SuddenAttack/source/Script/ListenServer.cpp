#include "ListenServer.h"
#include "Network.h"
#include "Protocol.h"
#include "..\Script\ThirdPersonCharacter.h"

using namespace ze;

ListenServer::ListenServer(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_hScriptNetwork()
	, m_hScriptGameUIManager()
	, m_pHost(nullptr)
	, m_startingTeam(GameTeam::Unknown)
	, m_startingMap(GameMap::Unknown)
{
}

void ListenServer::Awake()
{
	// ENet ÃÊ±âÈ­
	if (enet_initialize() != 0)
	{
		ze::Runtime::GetInstance()->GetSyncFileLogger().Write(L"Failed to initialize enet!\n");
	}
	else
	{
		atexit(enet_deinitialize);
	}
}

void ListenServer::Update()
{
	if (!m_pHost)
		return;

	ENetEvent event;

	while (enet_host_service(m_pHost, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("ENET_EVENT_TYPE_CONNECT\n");
			// printf("A new client connected from %x:%u.\n",
			// 	event.peer->address.host,
			// 	event.peer->address.port);
			// 
			// /* Store any relevant client information here. */
			// event.peer->data = "Client information";
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

void ListenServer::SetStartInfo(GameTeam startingTeam, GameMap startingMap)
{
	m_startingTeam = startingTeam;
	m_startingMap = startingMap;
}

void ListenServer::StartServer()
{
	printf("StartServer!\n");

	if (m_pHost)
		*reinterpret_cast<int*>(0) = 0;

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	SAClient& client = pScriptNetwork->GetClient();

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::CS_NOTIFY_LISTEN_SERVER_START));
	client.Send(std::move(pkt));


	GameObjectHandle hTPCTest = Runtime::GetInstance()->CreateGameObject(L"TPC");
	GameObject* pTPCTest = hTPCTest.ToPtr();
	pTPCTest->m_transform.SetPosition(0.0f, 3.0f, 0.0f);
	pTPCTest->AddComponent<ThirdPersonCharacter>();
}

void ListenServer::CloseServer()
{
	ReleaseENetHost();
}

void ListenServer::ReleaseENetHost()
{
	if (m_pHost)
	{
		enet_host_destroy(m_pHost);
		m_pHost = nullptr;
	}
}

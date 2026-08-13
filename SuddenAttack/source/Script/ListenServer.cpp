#include "ListenServer.h"
#include "Network.h"
#include "Protocol.h"
#include "..\Script\Player.h"
#include "..\Script\ThirdPersonCharacter.h"
#include "..\Script\GameUIManager.h"

using namespace ze;

ListenServer::ListenServer(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_hScriptNetwork()
	, m_hScriptGameUIManager()
	, m_pHost(nullptr)
	, m_map(GameMap::Unknown)
	, m_playersTeam()
	, m_redTeamPlayers()
	, m_blueTeamPlayers()
	, m_ready(false)
{
}

void ListenServer::Awake()
{
	// ENet 초기화
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
	IN_ADDR peerAddr;
	wchar_t ipStr[22];
	while (enet_host_service(m_pHost, &event, 0) > 0)
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

void ListenServer::OnDestroy()
{
	this->CloseServer();
}

void ListenServer::SetStartupInfo(GameMap startingMap, const uint32_t* pStartingPlayersAccountIds, const GameTeam* pStartingPlayersTeam, size_t count)
{
	assert(count > 0);
	assert(startingMap != GameMap::Unknown);

	m_map = startingMap;

	m_playersTeam.clear();
	for (size_t i = 0; i < count; ++i)
		m_playersTeam[pStartingPlayersAccountIds[i]] = pStartingPlayersTeam[i];

	m_ready = true;
}

void ListenServer::StartServer()
{
	assert(m_pHost == nullptr);

	if (m_pHost)
		*reinterpret_cast<int*>(0) = 0;

	// Create ENet Host

	// The host must be specified in network byte - order, and the port must be in host byte - order.
	ENetAddress addr;
	addr.host = ENET_HOST_ANY;
	addr.port = LISTEN_SERVER_PORT;
	m_pHost = enet_host_create(
		&addr,
		32,
		UDP_CHANNEL_COUNT,
		0,
		0
	);

	if (m_pHost == nullptr)
	{
		// SAServer로 방 시작 실패 패킷 전송.
		// 서버는 클라이언트들에게 방 입장 돌입 상태에서 빠져나오도록 알려줘야 함.
		return;
	}


	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	SAClient& client = pScriptNetwork->GetClient();

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::CS_NOTIFY_LISTEN_SERVER_START));
	pkt->Write(LISTEN_SERVER_PORT);
	client.Send(std::move(pkt));

	
	this->CreateMainPlayer();
	m_hScriptGameUIManager.ToPtr()->SetState(GameUIStatePlaying::GetState());


	GameObjectHandle hTPCTest = Runtime::GetInstance()->CreateGameObject(L"TPC");
	GameObject* pTPCTest = hTPCTest.ToPtr();
	pTPCTest->m_transform.SetPosition(2.0f, 0.0f, 2.0f);
	pTPCTest->AddComponent<ThirdPersonCharacter>();
}

void ListenServer::CloseServer()
{
	if (m_pHost)
	{
		// 모든 클라이언트 접속 끊기
		



		// 기타 작업...
		


		
		// ENet 호스트 제거
		enet_host_destroy(m_pHost);
		m_pHost = nullptr;
	}
}

void ListenServer::InitState()
{
	m_map = GameMap::Unknown;
	m_playersTeam.clear();

	m_ready = false;
}

void ListenServer::CreateMainPlayer()
{
	GameObjectHandle hGameObjectPlayer = Runtime::GetInstance()->CreateGameObject(L"Player");
	GameObject* pGameObjectPlayer = hGameObjectPlayer.ToPtr();
	pGameObjectPlayer->m_transform.SetPosition(-7.0f, 0.0f, -5.0f);
	ComponentHandle<Player> hScriptPlayer = pGameObjectPlayer->AddComponent<Player>();
}

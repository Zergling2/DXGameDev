#include "ListenServerClient.h"
#include "Constants.h"
#include "..\Resource\GameInfo.h"
#include "GameResources.h"
#include "Account.h"
#include "Player.h"
#include "ThirdPersonCharacter.h"
#include "GameUIManager.h"
#include <WS2tcpip.h>

using namespace ze;

ListenServerClient::ListenServerClient(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_serverIP(0)
	, m_serverPort(0)
	, m_pClient(nullptr)
	, m_pPeer(nullptr)
	, m_hScriptGameResources()
	, m_hScriptNetwork()
	, m_hScriptAccount()
	, m_hScriptGameUIManager()
	, m_hScriptPlayer()
	, m_players()
	, m_gameRemainingTime(0.0f)
{
	m_players.reserve(MAX_PLAYERS_PER_TEAM * static_cast<size_t>(GameTeam::Count));
}

void ListenServerClient::FixedUpdate()
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
			/* Store any relevant client information here. */
			// event.peer->data = "Peer information";

			// 방장의 리슨서버와 연결되면 가장 먼저
			OnConnect(event.peer);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			wprintf(L"ENET_EVENT_TYPE_DISCONNECT\n");
			if (m_pPeer == event.peer)
			{
				m_pPeer = nullptr;
			}
			else
			{
				wprintf(L"[WARNING] m_pPeer != event.peer\n");
			}
			// printf("%s disconnected.\n", event.peer->data);
			// 
			/* Reset the peer's client information. */
			// 
			// event.peer->data = NULL;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			// printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
			// 	event.packet->dataLength,
			// 	event.packet->data,
			// 	event.peer->data,
			// 	event.channelID);

			OnReceive(event.peer, event.channelID, event.packet);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			break;
		}
	}



	const float dt = Time::GetInstance()->GetFixedDeltaTime();
	
	// 남은 시간 UI 업데이트
	m_gameRemainingTime = (std::max)(m_gameRemainingTime - dt, 0.0f);
	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->SetTextGameRemainingTime(m_gameRemainingTime);
}

void ListenServerClient::OnDestroy()
{
	this->CloseClient();
}

void ListenServerClient::OnConnect(ENetPeer* pPeer)
{
	m_pPeer = pPeer;

	Account* pScriptAccount = m_hScriptAccount.ToPtr();

	// 연결되면 가장 먼저 할 일
	// 유효 클라이언트임을 인증
	LSCSReqAuth req;
	req.m_protocol = LSProtocol::CS_REQ_AUTH;
	req.m_accountId = pScriptAccount->GetAccountId();
	req.m_key = 0xdeadbeef;
	req.m_nicknameLen = pScriptAccount->GetNicknameLen();
	wmemcpy(req.m_nickname, pScriptAccount->GetNickname(), req.m_nicknameLen);
	req.m_level = pScriptAccount->GetLevel();

	ENetPacket* pReqPkt = enet_packet_create(&req, sizeof(req), ENET_PACKET_FLAG_RELIABLE);
	if (!SendPacket(pReqPkt))
	{
		enet_packet_destroy(pReqPkt);
		pReqPkt = nullptr;
	}
}

void ListenServerClient::OnReceive(ENetPeer* pPeer, uint8_t channelId, const ENetPacket* pPacket)
{
	const size_t packetSize = pPacket->dataLength;

	if (packetSize < sizeof(LSPacketBase))
	{
		wprintf(L"Invalid data length %zu bytes!\n", packetSize);
		enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		return;
	}

	const LSProtocol protocol = reinterpret_cast<const LSPacketBase*>(pPacket->data)->m_protocol;
	switch (protocol)
	{
	case LSProtocol::SC_RES_AUTH_RESULT:
		if (packetSize != sizeof(LSSCResAuthResult))
			enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		else
			OnSCResAuthResult(reinterpret_cast<const LSSCResAuthResult*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_STATUS:
		if (packetSize != sizeof(LSSCNotifyGameStatus))
			enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		else
			OnSCNotifyGameStatus(reinterpret_cast<const LSSCNotifyGameStatus*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_CHAT:
		if (packetSize != sizeof(LSSCNotifyChat))
			enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		else
			OnSCNotifyChat(reinterpret_cast<const LSSCNotifyChat*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_JOINED:
		if (packetSize != sizeof(LSSCNotifyGamePlayerJoined))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerJoined(reinterpret_cast<const LSSCNotifyGamePlayerJoined*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_EXIT:
		if (packetSize != sizeof(LSSCNotifyGamePlayerExit))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerExit(reinterpret_cast<const LSSCNotifyGamePlayerExit*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_INFO:
		if (packetSize != sizeof(LSSCNotifyGamePlayerInfo))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerInfo(reinterpret_cast<const LSSCNotifyGamePlayerInfo*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_WEAPON_EVENT:
		if (packetSize != sizeof(LSSCNotifyGamePlayerWeaponEvent))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerWeaponEvent(reinterpret_cast<const LSSCNotifyGamePlayerWeaponEvent*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_TRANSFORM:
		if (packetSize != sizeof(LSSCNotifyGamePlayerTransform))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerTransform(reinterpret_cast<const LSSCNotifyGamePlayerTransform*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_RESPAWN:
		if (packetSize != sizeof(LSSCNotifyGamePlayerRespawn))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerRespawn(reinterpret_cast<const LSSCNotifyGamePlayerRespawn*>(pPacket->data));
		break;
	default:
		// ...
		break;
	}
}

void ListenServerClient::SetStartupInfo(uint32_t serverIP, uint16_t serverPort)
{
	m_serverIP = serverIP;
	m_serverPort = serverPort;
}

void ListenServerClient::StartClient()
{
	assert(m_pClient == nullptr);
	assert(m_pPeer == nullptr);

	if (m_pClient)
		*reinterpret_cast<int*>(0) = 0;

	m_pClient = enet_host_create(
		nullptr,
		1,	// 1 outgoing connection
		UDP_CHANNEL_COUNT,
		0,
		0
	);

	if (m_pClient == nullptr)
	{
		// SAServer로 리슨서버 클라이언트 생성 실패 패킷 전송
		// SAServer는 이 패킷 수신 시 리슨서버 방장에게 알린다. (알리지 않아도 될지도?)
		// 그리고 해당 플레이어의 PlayerState도 None으로 변경한다. (게임중 -> 상태 없음) 으로 전환해서 플레이어가 방을 나가거나 할 수 있게 유도한다.

		wprintf(L"enet_host_create failed.\n");
		return;
	}

	wprintf(L"### Listen Server Client Start ###\n");

	// The host must be specified in network byte - order, and the port must be in host byte - order.
	ENetAddress listenServerAddr;
	listenServerAddr.host = htonl(m_serverIP);
	listenServerAddr.port = m_serverPort;

	m_pPeer = enet_host_connect(m_pClient, &listenServerAddr, UDP_CHANNEL_COUNT, 0);
	if (!m_pPeer)
	{
		wprintf(L"No available peers for initializing an ENet connection.\n");
		return;
	}

	// ENetEvent event;
	// IN_ADDR peerAddr;
	// wchar_t ipStr[22];
	// /* Wait up to 5 seconds for the connection attempt to succeed. */
	// if (enet_host_service(m_pClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	// {
	// 	peerAddr.s_addr = event.peer->address.host;
	// 	if (!InetNtopW(AF_INET, &peerAddr.s_addr, ipStr, _countof(ipStr)))
	// 		ipStr[0] = L'\0';
	// 	wprintf(L"ENET_EVENT_TYPE_CONNECT %s:%u.\n", ipStr, static_cast<uint32_t>(event.peer->address.port));
	// 
	// 	puts("Connection to listen server succeeded.");
	// }
	// else
	// {
	// 	/* Either the 5 seconds are up or a disconnect event was */
	// 	/* received. Reset the peer in the event the 5 seconds   */
	// 	/* had run out without any significant event.            */
	// 	enet_peer_reset(m_pPeer);		// DISCONNECT 이벤트 발생 X
	// 	m_pPeer = nullptr;
	// 
	// 	puts("Connection to listen server failed.");
	// }
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

	m_serverIP = 0;
	m_serverPort = 0;
}

bool ListenServerClient::SendPacket(ENetPacket* pPacket) const
{
	if (!m_pPeer)
		return false;

	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;
	return enet_peer_send(m_pPeer, channelId, pPacket) == 0;
}

void ListenServerClient::OnSCResAuthResult(const LSSCResAuthResult* pPacket)
{
	// SAServer로 리슨서버 퇴장 알림->은 방장의 역할 (근데 이 동작은 필요가 없을것 같기도 하다.)

	if (!pPacket->m_result)
		this->CloseClient();
}

void ListenServerClient::OnSCNotifyGameStatus(const LSSCNotifyGameStatus* pPacket)
{
	m_gameRemainingTime = pPacket->m_gameRemainingTime;

	// 플레이어 캐릭터 오브젝트 생성
	GameObjectHandle hGameObjectPlayer = Runtime::GetInstance()->CreateGameObject(L"MyPlayer");
	GameObject* pGameObjectPlayer = hGameObjectPlayer.ToPtr();
	pGameObjectPlayer->m_transform.SetPosition(-7.0f, 0.0f, -5.0f);
	ComponentHandle<Player> hScriptPlayer = pGameObjectPlayer->AddComponent<Player>();
	m_hScriptPlayer = hScriptPlayer;
	Player* pScriptPlayer = hScriptPlayer.ToPtr();
	pScriptPlayer->m_hScriptListenServerClient = this->ToHandle();
	pScriptPlayer->SetProcessingInput(false);

	// GameUIManager 상태 설정
	m_hScriptGameUIManager.ToPtr()->SetState(GameUIStatePlaying::GetState());
}

void ListenServerClient::OnSCNotifyChat(const LSSCNotifyChat* pPacket)
{
	if (pPacket->m_chatMsgLen > MAX_CHAT_MSG_LEN)
		return;

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();

	const auto iter = m_players.find(pPacket->m_accountId);
	const GamePlayerInfo* const pGamePlayerInfo = iter != m_players.cend() ? iter->second.first.get() : nullptr;

	wchar_t msg[MAX_CHAT_MSG_LEN + MAX_NICKNAME_LEN + 8];

	if (pGamePlayerInfo)
	{
		msg[0] = L'[';
		wmemcpy(&msg[1], pGamePlayerInfo->m_nickname, pGamePlayerInfo->m_nicknameLen);
		msg[1 + pGamePlayerInfo->m_nicknameLen] = L']';
		msg[1 + pGamePlayerInfo->m_nicknameLen + 1] = L' ';
		wmemcpy(&msg[1 + pGamePlayerInfo->m_nicknameLen + 2], pPacket->m_chatMsg, pPacket->m_chatMsgLen);
		msg[1 + pGamePlayerInfo->m_nicknameLen + 2 + pPacket->m_chatMsgLen] = L'\0';
	}
	else
	{
		msg[0] = L'[';
		msg[1] = L'?';
		msg[2] = L'?';
		msg[3] = L'?';
		msg[4] = L']';
		msg[5] = L' ';
		wmemcpy(&msg[6], pPacket->m_chatMsg, pPacket->m_chatMsgLen);
		msg[6 + pPacket->m_chatMsgLen] = L'\0';
	}

	pScriptGameUIManager->AddChatMsg(msg);
}

void ListenServerClient::OnSCNotifyGamePlayerJoined(const LSSCNotifyGamePlayerJoined* pPacket)
{
	wprintf(L"OnSCNotifyGamePlayerJoined\n");

	std::unique_ptr<GamePlayerInfo> upNewPlayer = std::make_unique<GamePlayerInfo>(pPacket->m_accountId);

	upNewPlayer->m_nicknameLen = pPacket->m_nicknameLen;
	wmemcpy(upNewPlayer->m_nickname, pPacket->m_nickname, upNewPlayer->m_nicknameLen);
	upNewPlayer->m_nickname[upNewPlayer->m_nicknameLen] = L'\0';	// null termination
	upNewPlayer->m_team = pPacket->m_team;
	upNewPlayer->m_level = pPacket->m_level;
	upNewPlayer->m_kill = pPacket->m_kill;
	upNewPlayer->m_death = pPacket->m_death;
	upNewPlayer->m_ping = pPacket->m_ping;
	upNewPlayer->m_state = pPacket->m_state;
	upNewPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
	upNewPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
	upNewPlayer->m_currWeapon = pPacket->m_currWeapon;

	GameObjectHandle hPlayerGameObject = Runtime::GetInstance()->CreateGameObject(L"Player");
	GameObject* pPlayerGameObject = hPlayerGameObject.ToPtr();
	ComponentHandle<ThirdPersonCharacter> hScriptThirdPersonCharacter = pPlayerGameObject->AddComponent<ThirdPersonCharacter>();
	ThirdPersonCharacter* pScriptThirdPersonCharacter = hScriptThirdPersonCharacter.ToPtr();


	pScriptThirdPersonCharacter->OnInit(
		upNewPlayer->m_team,
		upNewPlayer->m_weaponCodes[0],
		upNewPlayer->m_weaponCodes[1],
		upNewPlayer->m_currWeapon,
		upNewPlayer->m_state,
		XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
		XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
		pPacket->m_camRotX
	);

	auto ret = m_players.insert(std::make_pair(pPacket->m_accountId, std::make_pair(std::move(upNewPlayer), hScriptThirdPersonCharacter)));
	assert(ret.second);
}

void ListenServerClient::OnSCNotifyGamePlayerExit(const LSSCNotifyGamePlayerExit* pPacket)
{
	wprintf(L"OnSCNotifyGamePlayerExit\n");
}

void ListenServerClient::OnSCNotifyGamePlayerInfo(const LSSCNotifyGamePlayerInfo* pPacket)
{
	wprintf(L"OnSCNotifyGamePlayerInfo\n");

	std::unique_ptr<GamePlayerInfo> upNewPlayer = std::make_unique<GamePlayerInfo>(pPacket->m_accountId);

	upNewPlayer->m_nicknameLen = pPacket->m_nicknameLen;
	wmemcpy(upNewPlayer->m_nickname, pPacket->m_nickname, upNewPlayer->m_nicknameLen);
	upNewPlayer->m_nickname[upNewPlayer->m_nicknameLen] = L'\0';	// null termination
	upNewPlayer->m_team = pPacket->m_team;
	upNewPlayer->m_level = pPacket->m_level;
	upNewPlayer->m_kill = pPacket->m_kill;
	upNewPlayer->m_death = pPacket->m_death;
	upNewPlayer->m_ping = pPacket->m_ping;
	upNewPlayer->m_state = pPacket->m_state;
	upNewPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
	upNewPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
	upNewPlayer->m_currWeapon = pPacket->m_currWeapon;

	GameObjectHandle hPlayerGameObject = Runtime::GetInstance()->CreateGameObject(L"Player");
	GameObject* pPlayerGameObject = hPlayerGameObject.ToPtr();
	ComponentHandle<ThirdPersonCharacter> hScriptThirdPersonCharacter = pPlayerGameObject->AddComponent<ThirdPersonCharacter>();
	ThirdPersonCharacter* pScriptThirdPersonCharacter = hScriptThirdPersonCharacter.ToPtr();


	pScriptThirdPersonCharacter->OnInit(
		upNewPlayer->m_team,
		upNewPlayer->m_weaponCodes[0],
		upNewPlayer->m_weaponCodes[1],
		upNewPlayer->m_currWeapon,
		upNewPlayer->m_state,
		XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
		XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
		pPacket->m_camRotX
	);

	auto ret = m_players.insert(std::make_pair(pPacket->m_accountId, std::make_pair(std::move(upNewPlayer), hScriptThirdPersonCharacter)));
	assert(ret.second);
}

void ListenServerClient::OnSCNotifyGamePlayerWeaponEvent(const LSSCNotifyGamePlayerWeaponEvent* pPacket)
{
	const auto iter = m_players.find(pPacket->m_accountId);
	if (iter == m_players.cend())
		return;

	ThirdPersonCharacter* pScriptThirdPersonCharacter = iter->second.second.ToPtr();
	switch (pPacket->m_action)
	{
	case WeaponAction::Draw:
		pScriptThirdPersonCharacter->OnDraw(pPacket->m_slot);
		break;
	case WeaponAction::Fire:
		pScriptThirdPersonCharacter->OnFire();
		break;
	case WeaponAction::Reload:
		pScriptThirdPersonCharacter->OnReload();
		break;
	default:
		break;
	}
}

void ListenServerClient::OnSCNotifyGamePlayerTransform(const LSSCNotifyGamePlayerTransform* pPacket)
{
	const auto iter = m_players.find(pPacket->m_accountId);
	if (iter == m_players.cend())
		return;

	ThirdPersonCharacter* pScriptThirdPersonCharacter = iter->second.second.ToPtr();
	pScriptThirdPersonCharacter->OnTransform(
		XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
		XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
		pPacket->m_camRotX
	);
}

void ListenServerClient::OnSCNotifyGamePlayerRespawn(const LSSCNotifyGamePlayerRespawn* pPacket)
{
	const Account* pScriptAccount = m_hScriptAccount.ToPtr();

	if (pPacket->m_accountId == pScriptAccount->GetAccountId())		// 나의 리스폰 소식인 경우
	{
		Player* pScriptMyPlayer = m_hScriptPlayer.ToPtr();
		pScriptMyPlayer->OnRespawn(
			XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
			XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
			pPacket->m_camRotX
		);
	}
	else	// 다른 플레이어의 리스폰 소식인 경우
	{
		const auto iter = m_players.find(pPacket->m_accountId);
		if (iter == m_players.cend())
			return;

		ThirdPersonCharacter* pScriptThirdPersonCharacter = iter->second.second.ToPtr();
		pScriptThirdPersonCharacter->OnRespawn(
			XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
			XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
			pPacket->m_camRotX
		);
	}
}

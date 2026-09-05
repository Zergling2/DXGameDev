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
	, m_team(GameTeam::Unknown)
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
			m_pPeer = nullptr;
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
	req.m_team = m_team;	// 시작 팀 힌트

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
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_KILL:
		if (packetSize != sizeof(LSSCNotifyGamePlayerKill))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerKill(reinterpret_cast<const LSSCNotifyGamePlayerKill*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_DEAD:
		if (packetSize != sizeof(LSSCNotifyGamePlayerDead))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerDead(reinterpret_cast<const LSSCNotifyGamePlayerDead*>(pPacket->data));
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
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_HIT:
		if (packetSize != sizeof(LSSCNotifyGamePlayerHit))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerHit(reinterpret_cast<const LSSCNotifyGamePlayerHit*>(pPacket->data));
		break;
	case LSProtocol::SC_NOTIFY_GAME_PLAYER_START_RESPAWN:
		if (packetSize != sizeof(LSSCNotifyGamePlayerStartRespawn))
			enet_peer_disconnect_now(pPeer, 0);
		else
			OnSCNotifyGamePlayerStartRespawn(reinterpret_cast<const LSSCNotifyGamePlayerStartRespawn*>(pPacket->data));
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

void ListenServerClient::SetStartupInfo(uint32_t serverIP, uint16_t serverPort, GameTeam team)
{
	m_serverIP = serverIP;
	m_serverPort = serverPort;
	m_team = team;
}

void ListenServerClient::StartClient()
{
	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	if (pScriptGameUIManager)
		pScriptGameUIManager->Init();


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
	wprintf(L"ListenServerClient::CloseClient()\n");

	this->Disconnect();

	m_players.clear();

	m_serverIP = 0;
	m_serverPort = 0;
	m_team = GameTeam::Unknown;


	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	if (pScriptGameUIManager)
		pScriptGameUIManager->Init();
}

void ListenServerClient::Disconnect()
{
	if (m_pPeer)
	{
		LSCSNotifyGamePlayerExit notify;
		notify.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_EXIT;

		ENetPacket* pPkt = enet_packet_create(&notify, sizeof(notify), ENET_PACKET_FLAG_RELIABLE);
		if (!this->SendPacketFlush(pPkt))
		{
			enet_packet_destroy(pPkt);
			pPkt = nullptr;
		}

		enet_peer_reset(m_pPeer);	// CloseClient 이후로는 이벤트 루프가 실행되지 않을것이므로(enet 클라이언트 객체도 밑에서 파괴할 것이므로)
		// enet_peer_disconnect류 대신 reset을 한다.

		m_pPeer = nullptr;
	}

	if (m_pClient)
	{
		enet_host_destroy(m_pClient);
		m_pClient = nullptr;
	}
}

bool ListenServerClient::SendPacket(ENetPacket* pPacket) const
{
	if (!m_pPeer)
		return false;

	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;
	return enet_peer_send(m_pPeer, channelId, pPacket) == 0;
}

bool ListenServerClient::SendPacketFlush(ENetPacket* pPacket) const
{
	if (m_pPeer == nullptr || m_pClient == nullptr)
		return false;

	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;
	bool ret = enet_peer_send(m_pPeer, channelId, pPacket) == 0;

	enet_host_flush(m_pClient);

	return ret;
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
	ComponentHandle<Player> hScriptPlayer = pGameObjectPlayer->AddComponent<Player>();
	m_hScriptPlayer = hScriptPlayer;
	Player* pScriptPlayer = hScriptPlayer.ToPtr();
	pScriptPlayer->SetListenServerClientScriptHandle(this->ToHandle());

	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	std::unique_ptr<GamePlayerInfo> upNewMyPlayer = std::make_unique<GamePlayerInfo>(pScriptAccount->GetAccountId());
	const GamePlayerInfo* const pNewMyPlayer = upNewMyPlayer.get();	// move 대비

	upNewMyPlayer->m_nicknameLen = pScriptAccount->GetNicknameLen();
	wmemcpy(upNewMyPlayer->m_nickname, pScriptAccount->GetNickname(), pScriptAccount->GetNicknameLen());
	upNewMyPlayer->m_nickname[pScriptAccount->GetNicknameLen()] = L'\0';	// null termination
	upNewMyPlayer->m_team = pPacket->m_team;
	upNewMyPlayer->m_level = pScriptAccount->GetLevel();
	upNewMyPlayer->m_kill = pPacket->m_kill;
	upNewMyPlayer->m_death = pPacket->m_death;
	upNewMyPlayer->m_ping = pPacket->m_ping;
	upNewMyPlayer->m_state = pPacket->m_state;
	upNewMyPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
	upNewMyPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = pPacket->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
	upNewMyPlayer->m_currWeapon = pPacket->m_currWeapon;

	// 나의 플레이어 객체 생성 (ThirdPersonCharacter는 생성하지 않음.
	ComponentHandle<ThirdPersonCharacter> hEmptyThirdPersonCharacter;
	auto ret = m_players.insert(std::make_pair(pNewMyPlayer->m_accountId, std::make_pair(std::move(upNewMyPlayer), hEmptyThirdPersonCharacter)));
	assert(ret.second);

	// 이제 게임 플레이 로직에서 나의 pNewMyPlayer에도 접근해서 수정해야 한다.(클라이언트 단에서 자체적으로 판단하는 것들에 한해서.)

	pScriptPlayer->OnInit(
		pNewMyPlayer->m_team,
		pNewMyPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)],
		pNewMyPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)],
		pNewMyPlayer->m_currWeapon,
		pNewMyPlayer->m_state
	);

	// GameUIManager 상태 설정
	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->SetState(GameUIStatePlaying::GetState());

	pScriptGameUIManager->AddPlayer(
		pNewMyPlayer->m_accountId,
		pNewMyPlayer->m_team,
		pNewMyPlayer->m_level,
		pNewMyPlayer->m_nickname,
		pNewMyPlayer->m_kill,
		pNewMyPlayer->m_death,
		pNewMyPlayer->m_ping
	);
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
	std::unique_ptr<GamePlayerInfo> upNewPlayer = std::make_unique<GamePlayerInfo>(pPacket->m_accountId);
	const GamePlayerInfo* const pNewPlayer = upNewPlayer.get();	// move 대비

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
		upNewPlayer->m_accountId,
		upNewPlayer->m_team,
		upNewPlayer->m_weaponCodes[0],
		upNewPlayer->m_weaponCodes[1],
		upNewPlayer->m_currWeapon,
		upNewPlayer->m_state,
		XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
		XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
		pPacket->m_camRotX
	);

	auto ret = m_players.insert(std::make_pair(pNewPlayer->m_accountId, std::make_pair(std::move(upNewPlayer), hScriptThirdPersonCharacter)));
	assert(ret.second);

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	wchar_t joinMsgBuf[64];
	StringCchPrintfW(joinMsgBuf, _countof(joinMsgBuf), L"[%s]님이 입장하였습니다.", pNewPlayer->m_nickname);
	pScriptGameUIManager->AddChatMsg(joinMsgBuf, ColorsLinear::LimeGreen);

	pScriptGameUIManager->AddPlayer(
		pNewPlayer->m_accountId,
		pNewPlayer->m_team,
		pNewPlayer->m_level,
		pNewPlayer->m_nickname,
		pNewPlayer->m_kill,
		pNewPlayer->m_death,
		pNewPlayer->m_ping
	);
}

void ListenServerClient::OnSCNotifyGamePlayerExit(const LSSCNotifyGamePlayerExit* pPacket)
{
	wprintf(L"OnSCNotifyGamePlayerExit\n");

	auto iter = m_players.find(pPacket->m_accountId);
	if (iter != m_players.cend())
	{
		GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
		if (pScriptGameUIManager)
		{
			wchar_t exitMsgBuf[64];
			StringCchPrintfW(exitMsgBuf, _countof(exitMsgBuf), L"[%s]님이 퇴장하였습니다.", iter->second.first->m_nickname);
			pScriptGameUIManager->AddChatMsg(exitMsgBuf, ColorsLinear::LimeGreen);

			// 점수판 등에서 제거
			pScriptGameUIManager->RemovePlayer(iter->second.first->m_accountId);
		}

		ThirdPersonCharacter* pScriptThirdPersonCharacter = iter->second.second.ToPtr();
		GameObjectHandle hGameObjExitPlayer = pScriptThirdPersonCharacter->GetGameObjectHandle();
		GameObject* pGameObjExitPlayer = hGameObjExitPlayer.ToPtr();
		pGameObjExitPlayer->Destroy();

		m_players.erase(iter);
	}
}

void ListenServerClient::OnSCNotifyGamePlayerInfo(const LSSCNotifyGamePlayerInfo* pPacket)
{
	std::unique_ptr<GamePlayerInfo> upNewPlayer = std::make_unique<GamePlayerInfo>(pPacket->m_accountId);
	const GamePlayerInfo* const pNewPlayer = upNewPlayer.get();	// move 대비

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
		upNewPlayer->m_accountId,
		upNewPlayer->m_team,
		upNewPlayer->m_weaponCodes[0],
		upNewPlayer->m_weaponCodes[1],
		upNewPlayer->m_currWeapon,
		upNewPlayer->m_state,
		XMFLOAT3(pPacket->m_x, pPacket->m_y, pPacket->m_z),
		XMFLOAT4(pPacket->m_rx, pPacket->m_ry, pPacket->m_rz, pPacket->m_rw),
		pPacket->m_camRotX
	);

	auto ret = m_players.insert(std::make_pair(pNewPlayer->m_accountId, std::make_pair(std::move(upNewPlayer), hScriptThirdPersonCharacter)));
	assert(ret.second);

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->AddPlayer(
		pNewPlayer->m_accountId,
		pNewPlayer->m_team,
		pNewPlayer->m_level,
		pNewPlayer->m_nickname,
		pNewPlayer->m_kill,
		pNewPlayer->m_death,
		pNewPlayer->m_ping
	);
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
		pPacket->m_camRotX,
		pPacket->m_moveType
	);
}

void ListenServerClient::OnSCNotifyGamePlayerHit(const LSSCNotifyGamePlayerHit* pPacket)
{
	const Account* pScriptAccount = m_hScriptAccount.ToPtr();

	if (pPacket->m_accountIdWhoWasShot == pScriptAccount->GetAccountId())
	{
		// 나의 체력 뷰 업데이트
		GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
		pScriptGameUIManager->SetTextHP(pPacket->m_hp);
		pScriptGameUIManager->SetTextAP(pPacket->m_ap);
	}
	else
	{
		// 다른 플레이어의 TPC에 피격 이펙트 등 재생
		// ...
	}
}

void ListenServerClient::OnSCNotifyGamePlayerStartRespawn(const LSSCNotifyGamePlayerStartRespawn* pPacket)
{
	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->StartRespawnUI(pPacket->m_remainingTime);
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
			pPacket->m_camRotX,
			pPacket->m_hp,
			pPacket->m_ap
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

void ListenServerClient::OnSCNotifyGamePlayerKill(const LSSCNotifyGamePlayerKill* pPacket)
{
	// Kill 이벤트가 암시적으로 Dead 이벤트를 포함.

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();

	const auto iterKiller = m_players.find(pPacket->m_killerAccountId);
	const auto iterDeader = m_players.find(pPacket->m_deaderAccountId);
	const wchar_t* killerNickname = nullptr;
	const wchar_t* deaderNickname = nullptr;;
	GameTeam killerTeam = GameTeam::Unknown;
	GameTeam deaderTeam = GameTeam::Unknown;

	if (iterKiller != m_players.cend())
	{
		killerNickname = iterKiller->second.first->m_nickname;
		++iterKiller->second.first->m_kill;
		
		killerTeam = iterKiller->second.first->m_team;

		pScriptGameUIManager->SetPlayerKillDeath(iterKiller->second.first->m_accountId, iterKiller->second.first->m_kill, iterKiller->second.first->m_death);
	}
	else
	{
		killerNickname = L"";
	}

	if (iterDeader != m_players.cend())
	{
		deaderNickname = iterDeader->second.first->m_nickname;
		++iterDeader->second.first->m_death;

		deaderTeam = iterDeader->second.first->m_team;

		pScriptGameUIManager->SetPlayerKillDeath(iterDeader->second.first->m_accountId, iterDeader->second.first->m_kill, iterDeader->second.first->m_death);
	}
	else
	{
		deaderNickname = L"";
	}

	// 킬 로그 항목 추가
	pScriptGameUIManager->AddKillLog(pPacket->m_headshot, killerTeam, killerNickname, pPacket->m_weaponCode, deaderTeam, deaderNickname);
	

	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	if (pPacket->m_deaderAccountId == pScriptAccount->GetAccountId())
	{
		Player* pScriptPlayer = m_hScriptPlayer.ToPtr();
		pScriptPlayer->OnDead();
	}
	else
	{
		ThirdPersonCharacter* pScriptThirdPersonCharacter = iterDeader->second.second.ToPtr();
		pScriptThirdPersonCharacter->OnDead(WeaponAction::Death1);
	}
}

void ListenServerClient::OnSCNotifyGamePlayerDead(const LSSCNotifyGamePlayerDead* pPacket)
{
	// Dead 이벤트는 사망 요인이 다른 플레이어에 의한 kill이 아닌 경우 발생.

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();

	const auto iterDeader = m_players.find(pPacket->m_deaderAccountId);
	const wchar_t* deaderNickname = nullptr;
	GameTeam deaderTeam = GameTeam::Unknown;

	if (iterDeader != m_players.cend())
	{
		deaderNickname = iterDeader->second.first->m_nickname;
		++iterDeader->second.first->m_death;

		deaderTeam = iterDeader->second.first->m_team;

		pScriptGameUIManager->SetPlayerKillDeath(iterDeader->second.first->m_accountId, iterDeader->second.first->m_kill, iterDeader->second.first->m_death);
	}
	else
	{
		deaderNickname = L"";
	}

	// 킬 로그 항목 추가
	pScriptGameUIManager->AddKillLog(false, GameTeam::Unknown, L"", WeaponCode::Unknown, deaderTeam, deaderNickname);


	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	if (pPacket->m_deaderAccountId == pScriptAccount->GetAccountId())
	{
		Player* pScriptPlayer = m_hScriptPlayer.ToPtr();
		pScriptPlayer->OnDead();
	}
	else
	{
		ThirdPersonCharacter* pScriptThirdPersonCharacter = iterDeader->second.second.ToPtr();
		pScriptThirdPersonCharacter->OnDead(WeaponAction::Death1);
	}
}

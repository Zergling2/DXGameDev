#include "ListenServer.h"
#include "Network.h"
#include "Protocol.h"
#include "..\Resource\LSGamePlayerInfo.h"
#include "..\Resource\GameInfo.h"

using namespace ze;

ListenServer::ListenServer(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_hScriptNetwork()
	, m_pHost(nullptr)
	, m_map(GameMap::Unknown)
	, m_gameRemainingTime(0.0f)
{
}

void ListenServer::Awake()
{
	srand(static_cast<unsigned int>(time(nullptr)));

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

	// 네트워크 이벤트 루프 수행
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

			OnConnect(event.peer);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			// 유효 필드는 event.peer, event.peer->data 뿐이다.
			wprintf(L"ENET_EVENT_TYPE_DISCONNECT\n");
			OnDisconnect(event.peer);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			// printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
			// 	event.packet->dataLength,
			// 	event.packet->data,
			// 	event.peer->data,
			// 	event.channelID);
			// 
			OnReceive(event.peer, event.channelID, event.packet);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			break;
		}
	}
}

void ListenServer::FixedUpdate()
{
	const float dt = Time::GetInstance()->GetFixedDeltaTime();

	// 리슨서버 로직 수행

	// 게임 남은시간 업데이트
	m_gameRemainingTime = (std::max)(m_gameRemainingTime - dt, 0.0f);



	for (const auto& item : m_peers)
	{
		if (!item.second)
			continue;

		LSGamePlayerInfo& player = *item.second.get();

		// 1. 리스폰 업데이트
		if (player.m_state == InGamePlayerState::Dead && player.m_respawnRemainingTime >= 0.0f)
			UpdateRespawn(dt, player);
	}
}

void ListenServer::OnDestroy()
{
	this->CloseServer();
}

void ListenServer::OnConnect(ENetPeer* pPeer)
{
	m_peers.insert(std::make_pair(pPeer, nullptr));		// 피어 정보 추가(아직 미인증된 피어이므로 GamePlayerInfo는 생성하지 않는다.
}

void ListenServer::OnReceive(ENetPeer* pPeer, uint8_t channelId, const ENetPacket* pPacket)
{
	const size_t packetSize = pPacket->dataLength;

	if (packetSize < sizeof(LSPacketBase))
	{
		wprintf(L"Invalid data length %zu bytes!\n", packetSize);
		enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		return;
	}

	const LSProtocol protocol = reinterpret_cast<const LSPacketBase*>(pPacket->data)->m_protocol;
	if (protocol == LSProtocol::CS_REQ_AUTH)
	{
		if (packetSize != sizeof(LSCSReqAuth))
			enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
		else
			OnCSReqAuth(reinterpret_cast<const LSCSReqAuth*>(pPacket->data), pPeer);
	}
	else
	{
		if (!m_peers.find(pPeer)->second.get())
		{
			wprintf(L"Invalid peer's(0x%p) request has been rejected. (Disconnect)\n", pPeer);
			enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			return;
		}

		switch (protocol)
		{
		case LSProtocol::CS_REQ_CHAT:
			if (packetSize != sizeof(LSCSReqChat))
				enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			else
				OnCSReqChat(reinterpret_cast<const LSCSReqChat*>(pPacket->data), pPeer);
			break;
		case LSProtocol::CS_NOTIFY_GAME_PLAYER_WEAPON_EVENT:
			if (packetSize != sizeof(LSCSNotifyGamePlayerWeaponEvent))
				enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			else
				OnCSNotifyGamePlayerWeaponEvent(reinterpret_cast<const LSCSNotifyGamePlayerWeaponEvent*>(pPacket->data), pPeer);
			break;
		case LSProtocol::CS_NOTIFY_GAME_PLAYER_TRANSFORM:
			if (packetSize != sizeof(LSCSNotifyGamePlayerTransform))
				enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			else
				OnCSNotifyGamePlayerTransform(reinterpret_cast<const LSCSNotifyGamePlayerTransform*>(pPacket->data), pPeer);
			break;
		case LSProtocol::CS_NOTIFY_GAME_PLAYER_HIT:
			if (packetSize != sizeof(LSCSNotifyGamePlayerHit))
				enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			else
				OnCSNotifyGamePlayerHit(reinterpret_cast<const LSCSNotifyGamePlayerHit*>(pPacket->data), pPeer);
			break;
		case LSProtocol::CS_NOTIFY_GAME_PLAYER_EXIT:
			if (packetSize != sizeof(LSCSNotifyGamePlayerExit))
				enet_peer_disconnect_now(pPeer, 0);		// 즉시 연결 종료
			else
				OnCSNotifyGamePlayerExit(reinterpret_cast<const LSCSNotifyGamePlayerExit*>(pPacket->data), pPeer);
			break;
		default:
			wprintf(L"Invalid protocol type packet has been arrived: %u.\n", static_cast<uint32_t>(protocol));
			break;
		}
	}
}

void ListenServer::OnDisconnect(ENetPeer* pPeer)
{
	wprintf(L"ListenServer::OnDisconnect()\n");
	// pPeer->data;	// 유효 필드
	const auto iter = m_peers.find(pPeer);
	if (iter == m_peers.cend())
		return;

	if (iter->second != nullptr)		// 인증된 피어여서 LSGamePlayerInfo 구조체가 할당된 피어인 경우
	{
		{
			LSSCNotifyGamePlayerExit notify;
			notify.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_EXIT;
			notify.m_accountId = iter->second->m_accountId;

			ENetPacket* pPkt = enet_packet_create(&notify, sizeof(notify), ENET_PACKET_FLAG_RELIABLE);
			if (BroadcastPacketExcept(pPkt, iter->first) == 0)
			{
				enet_packet_destroy(pPkt);
				pPkt = nullptr;
			};
		}


		m_peersWithAccountId.erase(iter->second->m_accountId);	// account id 맵에서 항목을 지운다.


		// 리슨서버에서 피어&플레이어 정보를 모두 제거 후에 재접속이 가능하도록 SAServer로 플레이어 나감을 알림.
		Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
		SAClient& client = pScriptNetwork->GetClient();

		CSNotifyGamePlayerExitListenServer notify;
		notify.m_accountId = iter->second->m_accountId;

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::CS_NOTIFY_GAME_PLAYER_EXIT_LISTEN_SERVER));
		pkt->WriteBytes(&notify, sizeof(notify));
		client.Send(std::move(pkt));
	}

	m_peers.erase(iter);	// 피어(플레이어) 정보 제거
}

void ListenServer::StartServer(GameMap map, float gameDuration)
{
	assert(m_pHost == nullptr);
	assert(m_peers.size() == 0);

	assert(map != GameMap::Unknown);

	m_map = map;
	m_gameRemainingTime = gameDuration;
	for (size_t i = 0; i < _countof(m_score); ++i)
		m_score[i] = 0;

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
		wprintf(L"enet_host_create failed.\n");
		// ...
		return;
	}

	wprintf(L"### Listen Server Start ###\n");

	// SAServer로 리슨서버 시작 알림
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	SAClient& client = pScriptNetwork->GetClient();

	
	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::CS_NOTIFY_LISTEN_SERVER_START));
	pkt->Write(static_cast<uint16_t>(LISTEN_SERVER_PORT));
	client.Send(std::move(pkt));
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

	// m_redTeamPlayers.clear();
	// m_blueTeamPlayers.clear();
	m_peers.clear();
	m_peersWithAccountId.clear();
	m_map = GameMap::Unknown;
}

bool ListenServer::SendPacket(ENetPeer* pPeer, ENetPacket* pPacket) const
{
	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;
	return enet_peer_send(pPeer, channelId, pPacket) == 0;
}

size_t ListenServer::BroadcastPacket(ENetPacket* pPacket) const
{
	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;

	size_t count = 0;
	for (const auto& item : m_peers)
	{
		if (!item.second)	// 아직 유효성 입증되지 않은 피어에게는 전송 X
			continue;

		if (enet_peer_send(item.first, channelId, pPacket) == 0)
			++count;
	}

	return count;
}

size_t ListenServer::BroadcastPacketExcept(ENetPacket* pPacket, const ENetPeer* pExceptor) const
{
	const uint8_t channelId = pPacket->flags & ENET_PACKET_FLAG_RELIABLE ? UDP_RELIABLE_CHANNEL_ID : UDP_UNRELIABLE_CHANNEL_ID;

	size_t count = 0;
	for (const auto& item : m_peers)
	{
		if (!item.second)	// 아직 유효성 입증되지 않은 피어에게는 전송 X
			continue;

		if (item.first == pExceptor)
			continue;

		if (enet_peer_send(item.first, channelId, pPacket) == 0)
			++count;
	}

	return count;
}

void ListenServer::UpdateRespawn(float dt, LSGamePlayerInfo& player)
{
	// 리스폰 남은 시간 업데이트
	player.m_respawnRemainingTime = (std::max)(player.m_respawnRemainingTime - dt, 0.0f);

	// 리스폰 남은 시간이 0초인 경우
	if (player.m_respawnRemainingTime == 0.0f)
	{
		// 리스폰 전파 (RELIABLE)
		player.m_state = InGamePlayerState::Alive;
		player.m_hp = 100;
		player.m_ap = 100;

		LSSCNotifyGamePlayerRespawn ntfyRespawn;
		ntfyRespawn.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_RESPAWN;
		ntfyRespawn.m_accountId = player.m_accountId;

		int r[3] = { rand() & 1, rand() & 1, rand() & 1 };
		float x = 1.0f + static_cast<float>(rand() % 31) * 0.1f * (r[0] ? -1.0f : +1.0f);
		// float y = static_cast<float>(rand() % 31) * 0.1f * (r[1] ? -1.0f : +1.0f);
		float z = 2.5f + static_cast<float>(rand() % 31) * 0.1f * (r[2] ? -1.0f : +1.0f);

		ntfyRespawn.m_x = x;
		ntfyRespawn.m_y = 0.0f;
		ntfyRespawn.m_z = z;
		ntfyRespawn.m_rx = 0.0f;
		ntfyRespawn.m_ry = 0.0f;
		ntfyRespawn.m_rz = 0.0f;
		ntfyRespawn.m_rw = 1.0f;
		ntfyRespawn.m_camRotX = 0.0f;
		ntfyRespawn.m_hp = player.m_hp;
		ntfyRespawn.m_ap = player.m_ap;

		ENetPacket* pNtfyPktRespawn = enet_packet_create(&ntfyRespawn, sizeof(ntfyRespawn), ENET_PACKET_FLAG_RELIABLE);
		BroadcastPacket(pNtfyPktRespawn);
	}
}

void ListenServer::OnCSReqAuth(const LSCSReqAuth* pPacket, ENetPeer* pRequester)
{
	if (pPacket->m_key != 0xdeadbeef)
	{
		wprintf(L"Invalid auth key: %u (Disconnect)\n", pPacket->m_key);
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	if (pPacket->m_team >= GameTeam::Unknown)
	{
		wprintf(L"Invalid starting team: %u (Disconnect)\n", static_cast<uint32_t>(pPacket->m_team));
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	const auto peerIter = m_peers.find(pRequester);
	if (peerIter != m_peers.cend() && peerIter->second.get() != nullptr)
	{
		wprintf(L"Duplicated auth request! Account Id: %u (Disconnect)\n", pPacket->m_accountId);
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	if (pPacket->m_nicknameLen > MAX_NICKNAME_LEN)
	{
		wprintf(L"Invalid nickname len! Account Id: %u (Disconnect)\n", pPacket->m_accountId);
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	// 0. GamePlayerInfo 생성
	constexpr std::underlying_type_t<WeaponCode> PRIMARY_WEAPON_CODE_BEGIN = static_cast<int>(WeaponCode::M16);
	constexpr std::underlying_type_t<WeaponCode> PRIMARY_WEAPON_CODE_END = static_cast<int>(WeaponCode::M4A1);
	constexpr std::underlying_type_t<WeaponCode> SECONDARY_WEAPON_CODE_BEGIN = static_cast<int>(WeaponCode::USP);
	constexpr std::underlying_type_t<WeaponCode> SECONDARY_WEAPON_CODE_END = static_cast<int>(WeaponCode::B92FSBlack);

	std::underlying_type_t<WeaponCode> primaryWeapon = (rand() % (PRIMARY_WEAPON_CODE_END - PRIMARY_WEAPON_CODE_BEGIN + 1)) + PRIMARY_WEAPON_CODE_BEGIN;
	std::underlying_type_t<WeaponCode> secondaryWeapon = (rand() % (SECONDARY_WEAPON_CODE_END - SECONDARY_WEAPON_CODE_BEGIN + 1)) + SECONDARY_WEAPON_CODE_BEGIN;

	std::shared_ptr<LSGamePlayerInfo> spNewLSGamePlayerInfo = std::make_unique<LSGamePlayerInfo>(pPacket->m_accountId);
	spNewLSGamePlayerInfo->m_nicknameLen = pPacket->m_nicknameLen;
	wmemcpy(spNewLSGamePlayerInfo->m_nickname, pPacket->m_nickname, pPacket->m_nicknameLen);
	spNewLSGamePlayerInfo->m_nickname[pPacket->m_nicknameLen] = L'\0';
	spNewLSGamePlayerInfo->m_team = pPacket->m_team;
	spNewLSGamePlayerInfo->m_level = pPacket->m_level;
	spNewLSGamePlayerInfo->m_kill = 0;
	spNewLSGamePlayerInfo->m_death = 0;
	spNewLSGamePlayerInfo->m_ping = 0;
	spNewLSGamePlayerInfo->m_state = InGamePlayerState::Dead;	// 최초 상태는 Dead
	spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = static_cast<WeaponCode>(primaryWeapon);
	spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = static_cast<WeaponCode>(secondaryWeapon);
	spNewLSGamePlayerInfo->m_currWeapon = WeaponSlot::Secondary;
	spNewLSGamePlayerInfo->m_respawnRemainingTime = 0.0f;

	m_peers[pRequester] = spNewLSGamePlayerInfo;	// OnConnect에서 버킷만 만들어두고 nullptr로 지정해둔 GamePlayer 정보를 지금 바인딩한다.
	m_peersWithAccountId[spNewLSGamePlayerInfo->m_accountId] = std::make_pair(spNewLSGamePlayerInfo, pRequester);

	// 1. 게임 상태 전송
	LSSCNotifyGameStatus ntfyGameStatus;
	ntfyGameStatus.m_protocol = LSProtocol::SC_NOTIFY_GAME_STATUS;
	ntfyGameStatus.m_gameRemainingTime = m_gameRemainingTime;
	ntfyGameStatus.m_score[static_cast<size_t>(GameTeam::RedTeam)] = m_score[static_cast<size_t>(GameTeam::RedTeam)];
	ntfyGameStatus.m_score[static_cast<size_t>(GameTeam::BlueTeam)] = m_score[static_cast<size_t>(GameTeam::BlueTeam)];
	ntfyGameStatus.m_team = pPacket->m_team;
	ntfyGameStatus.m_kill = spNewLSGamePlayerInfo->m_kill;
	ntfyGameStatus.m_death = spNewLSGamePlayerInfo->m_death;
	ntfyGameStatus.m_ping = spNewLSGamePlayerInfo->m_ping;
	ntfyGameStatus.m_state = spNewLSGamePlayerInfo->m_state;
	ntfyGameStatus.m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
	ntfyGameStatus.m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
	ntfyGameStatus.m_currWeapon = WeaponSlot::Secondary;

	ENetPacket* pNtfyPktGameStatus = enet_packet_create(&ntfyGameStatus, sizeof(ntfyGameStatus), ENET_PACKET_FLAG_RELIABLE);
	assert(pNtfyPktGameStatus);

	if (!SendPacket(pRequester, pNtfyPktGameStatus))
	{
		enet_packet_destroy(pNtfyPktGameStatus);
		pNtfyPktGameStatus = nullptr;
		return;
	}

	// 2. 먼저 들어와 플레이 하고있던 클라이언트들에게 새로운 클라이언트의 입장을 알리는 패킷 브로드캐스트
	LSSCNotifyGamePlayerJoined ntfyPlayerJoined;
	ntfyPlayerJoined.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_JOINED;
	ntfyPlayerJoined.m_accountId = pPacket->m_accountId;
	ntfyPlayerJoined.m_nicknameLen = pPacket->m_nicknameLen;
	wmemcpy(ntfyPlayerJoined.m_nickname, pPacket->m_nickname, pPacket->m_nicknameLen);
	ntfyPlayerJoined.m_team = pPacket->m_team;
	ntfyPlayerJoined.m_level = pPacket->m_level;
	ntfyPlayerJoined.m_kill = 0;
	ntfyPlayerJoined.m_death = 0;
	ntfyPlayerJoined.m_ping = 0;
	ntfyPlayerJoined.m_state = spNewLSGamePlayerInfo->m_state;
	ntfyPlayerJoined.m_x = 0.0f;
	ntfyPlayerJoined.m_y = 0.0f;
	ntfyPlayerJoined.m_z = 0.0f;
	ntfyPlayerJoined.m_rx = 0.0f;
	ntfyPlayerJoined.m_ry = 0.0f;
	ntfyPlayerJoined.m_rz = 0.0f;
	ntfyPlayerJoined.m_rw = 1.0f;
	ntfyPlayerJoined.m_camRotX = 0.0f;
	ntfyPlayerJoined.m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
	ntfyPlayerJoined.m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = spNewLSGamePlayerInfo->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
	ntfyPlayerJoined.m_currWeapon = spNewLSGamePlayerInfo->m_currWeapon;

	ENetPacket* pNtfyPktPlayerJoined = enet_packet_create(&ntfyPlayerJoined, sizeof(ntfyPlayerJoined), ENET_PACKET_FLAG_RELIABLE);
	if (BroadcastPacketExcept(pNtfyPktPlayerJoined, pRequester) == 0)
	{
		enet_packet_destroy(pNtfyPktPlayerJoined);
		pNtfyPktPlayerJoined = nullptr;
	}

	// 3. 새로 입장하는 클라이언트에게 기존 플레이어들의 정보 전송
	size_t numOfOtherPlayers = 0;
	const GamePlayerInfo* otherPlayers[MAX_PLAYERS_PER_TEAM * 2];
	for (const auto& item : m_peers)
	{
		if (!item.second.get())
			continue;

		if (item.first == pRequester)
			continue;

		if (numOfOtherPlayers >= _countof(otherPlayers))
			*reinterpret_cast<int*>(0) = 0;

		otherPlayers[numOfOtherPlayers++] = item.second.get();
	}

	for (size_t i = 0; i < numOfOtherPlayers; ++i)
	{
		const GamePlayerInfo* const pOtherPlayer = otherPlayers[i];
		LSSCNotifyGamePlayerInfo ntfyPlayerInfo;
		ntfyPlayerInfo.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_INFO;
		assert(pOtherPlayer->m_nicknameLen <= MAX_NICKNAME_LEN);
		ntfyPlayerInfo.m_accountId = pOtherPlayer->m_accountId;
		ntfyPlayerInfo.m_nicknameLen = pOtherPlayer->m_nicknameLen;
		wmemcpy(ntfyPlayerInfo.m_nickname, pOtherPlayer->m_nickname, pOtherPlayer->m_nicknameLen);
		ntfyPlayerInfo.m_team = pOtherPlayer->m_team;
		ntfyPlayerInfo.m_level = pOtherPlayer->m_level;
		ntfyPlayerInfo.m_kill = pOtherPlayer->m_kill;
		ntfyPlayerInfo.m_death = pOtherPlayer->m_death;
		ntfyPlayerInfo.m_ping = pOtherPlayer->m_ping;
		ntfyPlayerInfo.m_state = pOtherPlayer->m_state;
		ntfyPlayerInfo.m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)] = pOtherPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Primary)];
		ntfyPlayerInfo.m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)] = pOtherPlayer->m_weaponCodes[static_cast<size_t>(WeaponSlot::Secondary)];
		ntfyPlayerInfo.m_currWeapon = pOtherPlayer->m_currWeapon;
		// 테스트 코드(랜덤 위치에 있다고 알려주기) -> 근데 나중에는 서버에서 위치 추적하고 있어야 할듯. 그래야 정보를 보내준다.
		if (pOtherPlayer->m_state == InGamePlayerState::Alive)
		{
			int r[3] = { rand() & 1, rand() & 1, rand() & 1 };
			float x = static_cast<float>(rand() % 31) * 0.1f * (r[0] ? -1.0f : +1.0f);
			// float y = static_cast<float>(rand() % 31) * 0.1f * (r[1] ? -1.0f : +1.0f);
			float z = static_cast<float>(rand() % 31) * 0.1f * (r[2] ? -1.0f : +1.0f);

			ntfyPlayerInfo.m_x = x;
			ntfyPlayerInfo.m_y = 0.0f;
			ntfyPlayerInfo.m_z = z;
			ntfyPlayerInfo.m_rx = 0.0f;
			ntfyPlayerInfo.m_ry = 0.0f;
			ntfyPlayerInfo.m_rz = 0.0f;
			ntfyPlayerInfo.m_rw = 1.0f;
			ntfyPlayerInfo.m_camRotX = 0.0f;
		}

		ENetPacket* pNtfyPktPlayerInfo = enet_packet_create(&ntfyPlayerInfo, sizeof(ntfyPlayerInfo), ENET_PACKET_FLAG_RELIABLE);
		if (!SendPacket(pRequester, pNtfyPktPlayerInfo))
		{
			enet_packet_destroy(pNtfyPktPlayerInfo);
			pNtfyPktPlayerInfo = nullptr;
			return;
		}
	}
}

void ListenServer::OnCSReqChat(const LSCSReqChat* pPacket, ENetPeer* pRequester)
{
	const auto iter = m_peers.find(pRequester);
	if (iter == m_peers.cend())
	{
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	if (pPacket->m_chatMsgLen > MAX_CHAT_MSG_LEN)
	{
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	LSSCNotifyChat notify;
	notify.m_protocol = LSProtocol::SC_NOTIFY_CHAT;
	notify.m_accountId = pPacket->m_accountId;
	notify.m_chatMsgLen = pPacket->m_chatMsgLen;
	wmemcpy(notify.m_chatMsg, pPacket->m_chatMsg, pPacket->m_chatMsgLen);

	ENetPacket* pResPkt = enet_packet_create(&notify, sizeof(notify), ENET_PACKET_FLAG_RELIABLE);
	if (BroadcastPacket(pResPkt) == 0)
	{
		enet_packet_destroy(pResPkt);
		pResPkt = nullptr;
		return;
	}
}

void ListenServer::OnCSNotifyGamePlayerWeaponEvent(const LSCSNotifyGamePlayerWeaponEvent* pPacket, ENetPeer* pRequester)
{
	const auto iter = m_peers.find(pRequester);
	if (iter == m_peers.cend())
	{
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	switch (pPacket->m_action)
	{
	case WeaponAction::Draw:
		iter->second->m_currWeapon = pPacket->m_slot;
		break;
	default:
		break;
	}

	LSSCNotifyGamePlayerWeaponEvent ntfyWeaponEvent;
	ntfyWeaponEvent.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_WEAPON_EVENT;
	ntfyWeaponEvent.m_accountId = iter->second->m_accountId;	// Requester의 account id를 획득해서 전달.
	ntfyWeaponEvent.m_action = pPacket->m_action;
	ntfyWeaponEvent.m_slot = pPacket->m_slot;

	ENetPacket* pNtfyPktWeaponEvent = enet_packet_create(&ntfyWeaponEvent, sizeof(ntfyWeaponEvent), 0);
	if (BroadcastPacketExcept(pNtfyPktWeaponEvent, pRequester) == 0)
	{
		enet_packet_destroy(pNtfyPktWeaponEvent);
		pNtfyPktWeaponEvent = nullptr;
		return;
	}
}

void ListenServer::OnCSNotifyGamePlayerTransform(const LSCSNotifyGamePlayerTransform* pPacket, ENetPeer* pRequester)
{
	const auto iter = m_peers.find(pRequester);
	if (iter == m_peers.cend())
	{
		enet_peer_disconnect(pRequester, 0);
		return;
	}

	LSSCNotifyGamePlayerTransform ntfyTransform;
	ntfyTransform.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_TRANSFORM;
	ntfyTransform.m_accountId = iter->second->m_accountId;	// Requester의 account id를 획득해서 전달.
	ntfyTransform.m_x = pPacket->m_x;
	ntfyTransform.m_y = pPacket->m_y;
	ntfyTransform.m_z = pPacket->m_z;
	ntfyTransform.m_rx = pPacket->m_rx;
	ntfyTransform.m_ry = pPacket->m_ry;
	ntfyTransform.m_rz = pPacket->m_rz;
	ntfyTransform.m_rw = pPacket->m_rw;
	ntfyTransform.m_camRotX = pPacket->m_camRotX;
	ntfyTransform.m_moveType = pPacket->m_moveType;

	ENetPacket* pNtfyPktTransform = enet_packet_create(&ntfyTransform, sizeof(ntfyTransform), 0);
	if (BroadcastPacketExcept(pNtfyPktTransform, pRequester) == 0)
	{
		enet_packet_destroy(pNtfyPktTransform);
		pNtfyPktTransform = nullptr;
		return;
	}
}

void ListenServer::OnCSNotifyGamePlayerHit(const LSCSNotifyGamePlayerHit* pPacket, ENetPeer* pRequester)
{
	const auto iter = m_peers.find(pRequester);
	if (iter == m_peers.cend())
	{
		enet_peer_disconnect(pRequester, 0);
		return;
	}
	
	const auto iterWhoWasShot = m_peersWithAccountId.find(pPacket->m_accountIdWhoWasShot);

	if (iterWhoWasShot->second.first->m_state != InGamePlayerState::Alive)
		return;

	const uint16_t damage = WeaponInfo::GetWeaponDamage(pPacket->m_weaponCode, pPacket->m_hitPart);
	if (iterWhoWasShot->second.first->m_hp <= damage)
		iterWhoWasShot->second.first->m_hp = 0;
	else
		iterWhoWasShot->second.first->m_hp -= damage;
	
	{
		// 피격 이벤트 브로드캐스팅 (피격 애니메이션 재생, 피격 이펙트 표시, 피격자는 이 패킷 수신 시 자신의 체력 UI 업데이트 등을 위해)
		// 히트를 주장한 플레이어는 자체적으로 피격 애니메이션, 피격 이펙트를 표시한다.
		LSSCNotifyGamePlayerHit ntfy;
		ntfy.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_HIT;
		ntfy.m_accountIdWhoWasShot = pPacket->m_accountIdWhoWasShot;
		ntfy.m_hp = iterWhoWasShot->second.first->m_hp;
		ntfy.m_ap = iterWhoWasShot->second.first->m_ap;

		ENetPacket* pPkt = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
		if (BroadcastPacketExcept(pPkt, pRequester) == 0)
		{
			enet_packet_destroy(pPkt);
			pPkt = nullptr;
		}
	}

	// 사망 시
	if (iterWhoWasShot->second.first->m_hp == 0)
	{
		iterWhoWasShot->second.first->m_state = InGamePlayerState::Dead;
		iterWhoWasShot->second.first->m_respawnRemainingTime = GameSettings::GetRespawnTime();

		assert(iter->second->m_team < GameTeam::Count);
		++m_score[static_cast<size_t>(iter->second->m_team)];	// 스코어 증가

		{
			// 사망 이벤트 알림
			LSSCNotifyGamePlayerKill ntfy;
			ntfy.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_KILL;
			ntfy.m_killerAccountId = iter->second->m_accountId;
			ntfy.m_deaderAccountId = iterWhoWasShot->second.first->m_accountId;
			ntfy.m_weaponCode = pPacket->m_weaponCode;
			ntfy.m_headshot = pPacket->m_hitPart == HitboxPart::Head;

			ENetPacket* pPkt = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
			if (BroadcastPacket(pPkt) == 0)
			{
				enet_packet_destroy(pPkt);
				pPkt = nullptr;
			}
		}

		{
			// 사망한 플레이어에게 리스폰 시작을 알림
			LSSCNotifyGamePlayerStartRespawn ntfy;
			ntfy.m_protocol = LSProtocol::SC_NOTIFY_GAME_PLAYER_START_RESPAWN;
			ntfy.m_remainingTime = iterWhoWasShot->second.first->m_respawnRemainingTime;

			ENetPacket* pPkt = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
			if (!SendPacket(iterWhoWasShot->second.second, pPkt))
			{
				enet_packet_destroy(pPkt);
				pPkt = nullptr;
			}
		}
	}
}

void ListenServer::OnCSNotifyGamePlayerExit(const LSCSNotifyGamePlayerExit* pPacket, ENetPeer* pRequester)
{
	enet_peer_disconnect_now(pRequester, 0);
	this->OnDisconnect(pRequester);
}

#include "Network.h"
#include "Constants.h"
#include "Protocol.h"
#include "LobbyHandler.h"
#include "Account.h"

using namespace ze;

void Client::OnConnect()
{
	printf("Connected to the GameServer.\n");
	m_network.m_connected = true;
}

void Client::OnReceive(winppy::Packet packet)
{
	AcquireSRWLockExclusive(&m_network.m_lock);
	m_network.m_packetQueue.push(std::move(packet));
	ReleaseSRWLockExclusive(&m_network.m_lock);
}

void Client::OnDisconnect()
{
	printf("Disconnected from the GameServer.\n");
	m_network.m_disconnectJobDone = true;
}

Network::Network(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_listenServer(*this)
	, m_ce()
	, m_client(*this)
	, m_packetQueue()
	, m_connected(false)
	, m_disconnectJobDone(false)
	, m_netId(0)
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

	m_client.Connect(L"127.0.0.1", SASERVER_PORT);
}

void Network::Update()
{
	AcquireSRWLockExclusive(&m_lock);
	while (!m_packetQueue.empty())
	{
		winppy::Packet packet = std::move(m_packetQueue.front());
		m_packetQueue.pop();

		if (packet->ReadableSize() < sizeof(Protocol))
			continue;

		Protocol protocol;
		packet->Read(reinterpret_cast<protocol_type*>(&protocol));

		switch (protocol)
		{
		case Protocol::SC_RES_LOGIN:
			PktProcSCResLogin(std::move(packet));
			break;
		case Protocol::SC_RES_CHANNEL_INFO:
			PktProcSCResChannelInfo(std::move(packet));
			break;
		case Protocol::SC_RES_JOIN_CHANNEL:
			PktProcSCResJoinChannel(std::move(packet));
			break;
		case Protocol::SC_RES_SEND_CHAT_MSG:
			PktProcSCResSendChatMsg(std::move(packet));
			break;
		case Protocol::SC_RES_GAME_LIST:
			PktProcSCResGameList(std::move(packet));
			break;
		case Protocol::SC_RES_CREATE_GAME_ROOM:
			PktProcSCResCreateGameRoom(std::move(packet));
			break;
		case Protocol::SC_RES_JOIN_GAME_ROOM:
			PktProcSCResJoinGameRoom(std::move(packet));
			break;
		case Protocol::SC_RES_CHANGE_TEAM:
			PktProcSCResChangeTeam(std::move(packet));
			break;
		case Protocol::SC_RES_EXIT_GAME_ROOM:
			PktProcSCResExitGameRoom(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_PLAYER_TEAM_CHANGED:
			PktProcSCNotifyPlayerTeamChanged(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_PLAYER_JOINED_GAME_ROOM:
			PktProcSCNotifyPlayerJoinedGameRoom(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_GAME_ROOM_PLAYER:
			PktProcSCNotifyGameRoomPlayer(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_PLAYER_EXIT_GAME_ROOM:
			PktProcSCNotifyPlayerExitGameRoom(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_HOST_CHANGED:
			PktProcSCNotifyHostChanged(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_HOST_GAME_START:
			//PktProcSCNotifyHostGameStart(std::move(packet));
			break;
		default:
			break;
		}
	}

	ReleaseSRWLockExclusive(&m_lock);
}

void Network::OnDestroy()
{
	m_listenServer.Shutdown();

	m_client.Disconnect();

	while (m_connected && !m_disconnectJobDone)
	{
		printf("Waiting disconnect job done!\n");
		Sleep(5);
	}

	m_ce.Release();
}

void Network::PktProcSCResLogin(winppy::Packet packet)
{
	SCResLogin res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (!res.m_result)
	{
		// 로그인 실패
		// pScriptLobbyHandler->m_hPanelLoginFailedRoot.ToPtr()->SetActive(true);
		return;
	}

	if (res.m_nicknameLen > MAX_NICKNAME_LEN)
		return;

	Account* pScriptAccount = m_hScriptAccount.ToPtr();

	// 로그인 성공
	m_netId = res.m_netId;
	pScriptAccount->SetAccountId(res.m_accountId);
	pScriptAccount->SetLevel(res.m_level);
	pScriptAccount->SetExp(res.m_exp);
	pScriptAccount->SetPoint(res.m_point);
	pScriptAccount->SetNickname(res.m_nickname, res.m_nicknameLen);

	pScriptLobbyHandler->SetLobbyState(LobbyState::ChannelListBrowser);
	return;
}

void Network::PktProcSCResChannelInfo(winppy::Packet packet)
{
	SCResChannelInfo res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	if (res.m_channelId >= CHANNEL_COUNT)
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	wchar_t buf[32];
	StringCchPrintfW(buf, _countof(buf), L"%u / %u", static_cast<uint32_t>(res.m_sessionCount), static_cast<uint32_t>(res.m_maxSession));
	static_cast<Text*>(pScriptLobbyHandler->m_hTextChannelHeadcount[res.m_channelId].ToPtr())->SetText(buf);
}

void Network::PktProcSCResJoinChannel(winppy::Packet packet)
{
	SCResJoinChannel res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (res.m_result)
	{
		pScriptLobbyHandler->SetLobbyState(LobbyState::GameListBrowser);
	}
	else
	{
		Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
		pTextOkMsgBoxMsg->SetText(L"현재 채널에 접속할 수 없습니다. 잠시 후 다시 시도해주세요.");

		IUIObject* pPanelOkMsgBoxRoot = pScriptLobbyHandler->m_hPanelOkMsgBoxRoot.ToPtr();
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelChannelBrowserRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
	}
}

void Network::PktProcSCResSendChatMsg(winppy::Packet packet)
{
	uint16_t nicknameLen;
	if (!packet->Read(&nicknameLen))
		return;
	if (nicknameLen > MAX_NICKNAME_LEN)
		return;

	uint16_t chatMsgLen;
	if (!packet->Read(&chatMsgLen))
		return;
	if (chatMsgLen > MAX_CHAT_MSG_LEN)
		return;

	wchar_t finalString[MAX_CHAT_MSG_LEN + MAX_NICKNAME_LEN + 4];
	if (!packet->ReadBytes(finalString, sizeof(wchar_t) * nicknameLen))
		return;

	finalString[nicknameLen] = L':';
	finalString[nicknameLen + 1] = L' ';
	finalString[nicknameLen + 2] = L'\0';

	if (!packet->ReadBytes(finalString + nicknameLen + 2, sizeof(wchar_t) * chatMsgLen))
		return;
	finalString[nicknameLen + 2 + chatMsgLen] = L'\0';

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->AddChatMsg(finalString);
}

void Network::PktProcSCResGameList(winppy::Packet packet)
{
	SCResGameList res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (pScriptLobbyHandler->m_currGameListContextNo != res.m_reqContextNo)	// 이전 방 목록 컨텍스트인 경우 무시
		return;

	auto& gameRoomList = pScriptLobbyHandler->m_gameRoomList;

	SCResGameListItem resItem;
	while (packet->ReadBytes(&resItem, sizeof(resItem)))
	{
		gameRoomList.emplace_back();
		GameRoomItem& item = gameRoomList.back();

		item.m_gameRoomId = resItem.m_gameRoomId;
		item.m_gameRoomNo = resItem.m_gameRoomNo;
		item.m_maxPlayer = resItem.m_maxPlayer;
		item.m_currPlayer = resItem.m_currPlayer;
		item.m_gameMap = resItem.m_gameMap;
		item.m_gameMode = resItem.m_gameMode;
		item.m_gameRoomState = resItem.m_gameRoomState;
		if (resItem.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)
			resItem.m_gameRoomNameLen = MAX_GAME_ROOM_NAME_LEN;
		wmemcpy_s(item.m_gameName, _countof(item.m_gameName), resItem.m_gameName, resItem.m_gameRoomNameLen);
		item.m_gameName[resItem.m_gameRoomNameLen] = L'\0';
	}

	pScriptLobbyHandler->UpdateGameListBrowserUI();
}

void Network::PktProcSCResCreateGameRoom(winppy::Packet packet)
{
	SCResCreateGameRoom res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	if (res.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)
		return;

	wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
	if (!packet->ReadBytes(gameRoomName, sizeof(wchar_t) * res.m_gameRoomNameLen))
		return;
	gameRoomName[res.m_gameRoomNameLen] = L'\0';

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	Account* pScriptAccount = m_hScriptAccount.ToPtr();

	if (res.m_result)
	{
		pScriptLobbyHandler->m_gameRoomId = res.m_gameRoomId;

		wchar_t gameRoomHeadText[_countof(gameRoomName) + 12];
		StringCchPrintfW(gameRoomHeadText, _countof(gameRoomHeadText), L"[%u] %s", static_cast<uint32_t>(res.m_gameRoomNo), gameRoomName);
		
		pScriptLobbyHandler->ClearGameRoomInfo();

		pScriptLobbyHandler->SetGameRoomInfo(res.m_gameRoomId, res.m_gameRoomHostNetId, res.m_maxPlayer, res.m_gameMap, res.m_gameMode, gameRoomHeadText, res.m_joinedTeam);
		pScriptLobbyHandler->AddGameRoomPlayerInfo(res.m_joinedTeam, m_netId, pScriptAccount->GetLevel(), pScriptAccount->GetNickname());

		pScriptLobbyHandler->SetLobbyState(LobbyState::GameRoom);
	}
	else
	{
		Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
		pTextOkMsgBoxMsg->SetText(L"현재 방을 생성할 수 없습니다. 잠시 후 다시 시도해주세요.");

		IUIObject* pPanelOkMsgBoxRoot = pScriptLobbyHandler->m_hPanelOkMsgBoxRoot.ToPtr();
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelCreateGameRoomRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
	}
}

void Network::PktProcSCResJoinGameRoom(winppy::Packet packet)
{
	SCResJoinGameRoom res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (!res.m_result)
	{
		Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
		switch (res.m_reason)
		{
		case FailReason::InvalidGame:
			pTextOkMsgBoxMsg->SetText(L"유효하지 않은 방입니다. 방 목록을 새로고침 후 다시 시도해주세요.");
			break;
		case FailReason::Full:
			pTextOkMsgBoxMsg->SetText(L"방 인원이 가득 차서 입장할 수 없습니다.");
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;
			break;
		}

		IUIObject* pPanelOkMsgBoxRoot = pScriptLobbyHandler->m_hPanelOkMsgBoxRoot.ToPtr();
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelGameListBrowserRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
	}
	else
	{
		if (res.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)
			return;

		Account* pScriptAccount = m_hScriptAccount.ToPtr();

		wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
		wmemcpy_s(gameRoomName, _countof(gameRoomName), res.m_gameRoomName, res.m_gameRoomNameLen);
		gameRoomName[res.m_gameRoomNameLen] = L'\0';

		wchar_t gameRoomHeadText[_countof(gameRoomName) + 12];
		StringCchPrintfW(gameRoomHeadText, _countof(gameRoomHeadText), L"[%u] %s", static_cast<uint32_t>(res.m_gameRoomNo), gameRoomName);

		pScriptLobbyHandler->ClearGameRoomInfo();
		pScriptLobbyHandler->SetGameRoomInfo(res.m_gameRoomId, res.m_gameRoomHostNetId, res.m_maxPlayer, res.m_gameMap, res.m_gameMode, gameRoomHeadText, res.m_joinedTeam);
		pScriptLobbyHandler->AddGameRoomPlayerInfo(res.m_joinedTeam, m_netId, pScriptAccount->GetLevel(), pScriptAccount->GetNickname());

		pScriptLobbyHandler->SetLobbyState(LobbyState::GameRoom);
	}
}

void Network::PktProcSCResChangeTeam(winppy::Packet packet)
{
	SCResChangeTeam res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	if (!res.m_result)
		return;

	Account* pScriptAccount = m_hScriptAccount.ToPtr();
	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->m_gameRoomMyTeam = res.m_newTeam;
	pScriptLobbyHandler->MoveGameRoomPlayerInfo(m_netId, res.m_newTeam);
}

void Network::PktProcSCResExitGameRoom(winppy::Packet packet)
{
	// TEST CODE
	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->ClearGameRoomInfo();

	pScriptLobbyHandler->SetLobbyState(LobbyState::GameListBrowser);
}

void Network::PktProcSCNotifyPlayerTeamChanged(winppy::Packet packet)
{
	SCNotifyPlayerTeamChanged notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	if (pScriptLobbyHandler->m_gameRoomId != notify.m_gameRoomId)
		return;

	pScriptLobbyHandler->MoveGameRoomPlayerInfo(notify.m_netId, notify.m_newTeam);
}

void Network::PktProcSCNotifyPlayerJoinedGameRoom(winppy::Packet packet)
{
	SCNotifyPlayerJoined notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		return;

	if (notify.m_nicknameLen > MAX_NICKNAME_LEN)
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	if (pScriptLobbyHandler->m_gameRoomId != notify.m_gameRoomId)
		return;

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), notify.m_nickname, notify.m_nicknameLen);
	nickname[notify.m_nicknameLen] = L'\0';

	pScriptLobbyHandler->AddGameRoomPlayerInfo(notify.m_joinedTeam, notify.m_netId, notify.m_level, nickname);
}

void Network::PktProcSCNotifyPlayerExitGameRoom(winppy::Packet packet)
{
	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	uint64_t gameRoomId;
	uint64_t exitPlayerNetId;
	if (!packet->Read(&gameRoomId))
		return;

	if (!packet->Read(&exitPlayerNetId))
		return;

	if (pScriptLobbyHandler->m_gameRoomId != gameRoomId)
		return;

	pScriptLobbyHandler->RemoveGameRoomPlayerInfo(exitPlayerNetId);
}

void Network::PktProcSCNotifyGameRoomPlayer(winppy::Packet packet)
{
	SCNotifyGameRoomPlayer notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		return;

	if (notify.m_nicknameLen > MAX_NICKNAME_LEN)
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	if (pScriptLobbyHandler->m_gameRoomId != notify.m_gameRoomId)
		return;

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), notify.m_nickname, notify.m_nicknameLen);
	nickname[notify.m_nicknameLen] = L'\0';

	pScriptLobbyHandler->AddGameRoomPlayerInfo(notify.m_team, notify.m_netId, notify.m_level, nickname);
}

void Network::PktProcSCNotifyHostChanged(winppy::Packet packet)
{
	uint64_t gameRoomId;
	uint64_t newHostNetId;
	if (!packet->Read(&gameRoomId))
		return;

	if (!packet->Read(&newHostNetId))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	if (pScriptLobbyHandler->m_gameRoomId != gameRoomId)
		return;

	pScriptLobbyHandler->OnGameRoomHostChanged(newHostNetId);
}

#include "Network.h"
#include "Constants.h"
#include "Protocol.h"
#include "LobbyHandler.h"
#include "Account.h"

using namespace ze;

void Client::OnConnect()
{
	printf("Connected to the SAServer.\n");
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
	printf("Disconnected from the SAServer.\n");
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
		case Protocol::SC_RES_ID_DUPLICATE_CHECK:
			PktProcSCResIdDuplicateCheck(std::move(packet));
			break;
		case Protocol::SC_RES_NICKNAME_DUPLICATE_CHECK:
			PktProcSCResNicknameDuplicateCheck(std::move(packet));
			break;
		case Protocol::SC_RES_CREATE_ACCOUNT:
			PktProcSCResCreateAccount(std::move(packet));
			break;
		case Protocol::SC_RES_CHANNEL_INFO:
			PktProcSCResChannelInfo(std::move(packet));
			break;
		case Protocol::SC_RES_JOIN_CHANNEL:
			PktProcSCResJoinChannel(std::move(packet));
			break;
		case Protocol::SC_RES_GAME_ROOM_LIST:
			PktProcSCResGameList(std::move(packet));
			break;
		case Protocol::SC_RES_CREATE_GAME_ROOM:
			PktProcSCResCreateGameRoom(std::move(packet));
			break;
		case Protocol::SC_RES_JOIN_GAME_ROOM:
			PktProcSCResJoinGameRoom(std::move(packet));
			break;
		case Protocol::SC_RES_HOST_GAME_START:
			PktProcSCResHostGameStart(std::move(packet));
			break;
		case Protocol::SC_RES_EXIT_GAME_ROOM:
			PktProcSCResExitGameRoom(std::move(packet));
			break;
		case Protocol::SC_RES_EXIT_GAME_CHANNEL:
			PktProcSCResExitGameChannel(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_LOBBY_CHAT:
			PktProcSCNotifyLobbyChat(std::move(packet));
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
		case Protocol::SC_NOTIFY_PLAYER_GAME_READY:
			PktProcSCNotifyPlayerGameReady(std::move(packet));
			break;
		case Protocol::SC_NOTIFY_PLAYER_GAME_UNREADY:
			PktProcSCNotifyPlayerGameUnready(std::move(packet));
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
	pScriptAccount->SetAccountId(res.m_accountId);
	pScriptAccount->SetLevel(res.m_level);
	pScriptAccount->SetExp(res.m_exp);
	pScriptAccount->SetPoint(res.m_point);
	pScriptAccount->SetNickname(res.m_nickname, res.m_nicknameLen);

	pScriptLobbyHandler->SetLobbyState(LobbyState::ChannelListBrowser);
	return;
}

void Network::PktProcSCResIdDuplicateCheck(winppy::Packet packet)
{
	SCResIdDuplicateCheck res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	Text* pTextCreateAccountIdDuplicateCheckMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextCreateAccountIdDuplicateCheckMsg.ToPtr());

	switch (res.m_result)
	{
	case IdDuplicateCheckResult::Valid:
		pTextCreateAccountIdDuplicateCheckMsg->SetText(L"사용 가능한 아이디입니다.");
		break;
	case IdDuplicateCheckResult::AlreadyExist:
		pTextCreateAccountIdDuplicateCheckMsg->SetText(L"이미 존재하는 아이디입니다.");
		break;
	case IdDuplicateCheckResult::Invalid:
		pTextCreateAccountIdDuplicateCheckMsg->SetText(L"사용 불가능한 아이디입니다.");
		break;
	default:
		break;
	}
}

void Network::PktProcSCResNicknameDuplicateCheck(winppy::Packet packet)
{
	SCResNicknameDuplicateCheck res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	Text* pTextCreateAccountNicknameDuplicateCheckMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextCreateAccountNicknameDuplicateCheckMsg.ToPtr());

	switch (res.m_result)
	{
	case NicknameDuplicateCheckResult::Valid:
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(L"사용 가능한 닉네임입니다.");
		break;
	case NicknameDuplicateCheckResult::AlreadyExist:
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(L"이미 존재하는 닉네임입니다.");
		break;
	case NicknameDuplicateCheckResult::Invalid:
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(L"사용 불가능한 단어가 포함되어 있습니다.");
		break;
	default:
		break;
	}
}

void Network::PktProcSCResCreateAccount(winppy::Packet packet)
{
	SCResCreateAccount res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
	IUIObject* pPanelOkMsgBoxRoot = pScriptLobbyHandler->m_hPanelOkMsgBoxRoot.ToPtr();

	switch (res.m_result)
	{
	case CreateAccountResult::Success:
		pTextOkMsgBoxMsg->SetText(L"계정 생성에 성공하였습니다. 새로운 계정으로 로그인해 주세요.");
		pScriptLobbyHandler->m_hPanelCreateAccountWindowRoot.ToPtr()->SetActive(false);
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelLoginWindowRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
		break;
	case CreateAccountResult::FailedInvalidId:
		pTextOkMsgBoxMsg->SetText(L"계정 생성에 실패하였습니다. 아이디가 이미 존재하거나 사용할 수 없는 아이디입니다.");
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelCreateAccountWindowRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
		break;
	case CreateAccountResult::FailedInvalidNickname:
		pTextOkMsgBoxMsg->SetText(L"계정 생성에 실패하였습니다. 닉네임이 이미 존재하거나 사용할 수 없는 닉네임입니다.");
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelCreateAccountWindowRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
		break;
	case CreateAccountResult::FailedInvalidPw:
		pTextOkMsgBoxMsg->SetText(L"계정 생성에 실패하였습니다. 비밀번호 형식이 올바르지 않습니다.");
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelCreateAccountWindowRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
		break;
	case CreateAccountResult::FailedUnknown:
		pTextOkMsgBoxMsg->SetText(L"계정 생성에 실패하였습니다. 잠시 후 다시 시도해 주세요.");
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelCreateAccountWindowRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
		break;
	}
}

void Network::PktProcSCResChannelInfo(winppy::Packet packet)
{
	SCResChannelInfo res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnReceiveChannelInfo(res.m_channelInfo, _countof(res.m_channelInfo));
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

void Network::PktProcSCNotifyLobbyChat(winppy::Packet packet)
{
	SCNotifyLobbyChat notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	if (notify.m_nicknameLen > MAX_NICKNAME_LEN)
		*reinterpret_cast<int*>(0) = 0;

	if (notify.m_msgLen > MAX_CHAT_MSG_LEN)
		*reinterpret_cast<int*>(0) = 0;

	wchar_t finalString[MAX_CHAT_MSG_LEN + MAX_NICKNAME_LEN + 4];
	if (!packet->ReadBytes(finalString, sizeof(wchar_t) * notify.m_nicknameLen))
		return;

	finalString[notify.m_nicknameLen] = L':';
	finalString[notify.m_nicknameLen + 1] = L' ';
	finalString[notify.m_nicknameLen + 2] = L'\0';

	if (!packet->ReadBytes(finalString + notify.m_nicknameLen + 2, sizeof(wchar_t) * notify.m_msgLen))
		return;
	finalString[notify.m_nicknameLen + 2 + notify.m_msgLen] = L'\0';

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->AddChatMsg(finalString);
}

void Network::PktProcSCResGameList(winppy::Packet packet)
{
	SCResGameRoomList res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	std::vector<GameRoomItem> list;
	
	SCResGameRoomListItem resItem;
	while (packet->ReadBytes(&resItem, sizeof(resItem)))
	{
		list.emplace_back();
		GameRoomItem& item = list.back();

		item.m_id = resItem.m_id;
		item.m_no = resItem.m_no;
		item.m_tf = resItem.m_tf;
		item.m_numOfPlayers = resItem.m_numOfPlayers;
		item.m_map = resItem.m_map;
		item.m_state = resItem.m_state;
		if (resItem.m_nameLen > MAX_GAME_ROOM_NAME_LEN)
			resItem.m_nameLen = MAX_GAME_ROOM_NAME_LEN;
		wmemcpy_s(item.m_name, _countof(item.m_name), resItem.m_name, resItem.m_nameLen);
		item.m_name[resItem.m_nameLen] = L'\0';
	}

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnReceiveGameList(res.m_reqContextNo, list);
}

void Network::PktProcSCResCreateGameRoom(winppy::Packet packet)
{
	SCResCreateGameRoom res;
	
	if (!packet->ReadBytes(&res, sizeof(res)))
		return;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	Account* pScriptAccount = m_hScriptAccount.ToPtr();

	if (res.m_result)
	{
		if (res.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)
			res.m_gameRoomNameLen = MAX_GAME_ROOM_NAME_LEN;

		wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
		wmemcpy_s(gameRoomName, _countof(gameRoomName), res.m_gameRoomName, res.m_gameRoomNameLen);
		gameRoomName[res.m_gameRoomNameLen] = L'\0';

		pScriptLobbyHandler->OnJoinGameRoom(res.m_gameRoomHostAccountId, res.m_gameRoomNo, res.m_gameRoomTeamFormat, res.m_gameMap, res.m_joinedTeam, gameRoomName);
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
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (res.m_result != JoinGameRoomResult::Success)
	{
		Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
		switch (res.m_result)
		{
		case JoinGameRoomResult::InvalidGame:
			pTextOkMsgBoxMsg->SetText(L"유효하지 않은 방입니다. 방 목록을 새로고침 후 다시 시도해주세요.");
			break;
		case JoinGameRoomResult::Full:
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
			res.m_gameRoomNameLen = MAX_GAME_ROOM_NAME_LEN;

		wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
		wmemcpy_s(gameRoomName, _countof(gameRoomName), res.m_gameRoomName, res.m_gameRoomNameLen);
		gameRoomName[res.m_gameRoomNameLen] = L'\0';

		pScriptLobbyHandler->OnJoinGameRoom(res.m_gameRoomHostAccountId, res.m_gameRoomNo, res.m_gameRoomTeamFormat, res.m_gameMap, res.m_joinedTeam, gameRoomName);
	}
}

void Network::PktProcSCResHostGameStart(winppy::Packet packet)
{
	SCResHostGameStart res;
	if (!packet->ReadBytes(&res, sizeof(res)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();

	if (res.m_result == HostGameStartResult::Success)
	{
		// m_listenServer.Run();
	}
	else if (res.m_result == HostGameStartResult::NotReady)
	{
		Text* pTextOkMsgBoxMsg = static_cast<Text*>(pScriptLobbyHandler->m_hTextOkMsgBoxMsg.ToPtr());
		pTextOkMsgBoxMsg->SetText(L"상대팀 플레이어중 적어도 한 명 이상이 준비 완료 상태이어야 합니다.");

		IUIObject* pPanelOkMsgBoxRoot = pScriptLobbyHandler->m_hPanelOkMsgBoxRoot.ToPtr();
		pPanelOkMsgBoxRoot->m_transform.SetParent(&pScriptLobbyHandler->m_hPanelGameRoomRoot.ToPtr()->m_transform);
		pPanelOkMsgBoxRoot->SetActive(true);
	}
}

void Network::PktProcSCResExitGameRoom(winppy::Packet packet)
{
	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnExitGameRoom();
}

void Network::PktProcSCResExitGameChannel(winppy::Packet packet)
{
	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->ClearChatMsgs();

	pScriptLobbyHandler->SetLobbyState(LobbyState::ChannelListBrowser);
}

void Network::PktProcSCNotifyPlayerTeamChanged(winppy::Packet packet)
{
	SCNotifyPlayerTeamChanged notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnPlayerTeamChanged(notify.m_accountId, notify.m_newTeam);
}

void Network::PktProcSCNotifyPlayerJoinedGameRoom(winppy::Packet packet)
{
	SCNotifyPlayerJoined notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	if (notify.m_nicknameLen > MAX_NICKNAME_LEN)
		*reinterpret_cast<int*>(0) = 0;

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), notify.m_nickname, notify.m_nicknameLen);
	nickname[notify.m_nicknameLen] = L'\0';

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnPlayerJoinGameRoom(notify.m_accountId, notify.m_level, nickname, notify.m_state, notify.m_team);
}

void Network::PktProcSCNotifyPlayerExitGameRoom(winppy::Packet packet)
{
	SCNotifyPlayerExitGameRoom notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnPlayerExitGameRoom(notify.m_accountId);
}

void Network::PktProcSCNotifyGameRoomPlayer(winppy::Packet packet)
{
	SCNotifyGameRoomPlayer notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	if (notify.m_nicknameLen > MAX_NICKNAME_LEN)
		notify.m_nicknameLen = MAX_NICKNAME_LEN;

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), notify.m_nickname, notify.m_nicknameLen);
	nickname[notify.m_nicknameLen] = L'\0';

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnPlayerJoinGameRoom(notify.m_accountId, notify.m_level, nickname, notify.m_state, notify.m_team);
}

void Network::PktProcSCNotifyHostChanged(winppy::Packet packet)
{
	SCNotifyHostChanged notify;
	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnGameRoomHostChanged(notify.m_newHostAccountId);
}

void Network::PktProcSCNotifyPlayerGameReady(winppy::Packet packet)
{
	SCNotifyPlayerGameReady notify;

	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnGameRoomPlayerStateChanged(notify.m_accountId, PlayerState::Ready);
}

void Network::PktProcSCNotifyPlayerGameUnready(winppy::Packet packet)
{
	SCNotifyPlayerGameUnready notify;

	if (!packet->ReadBytes(&notify, sizeof(notify)))
		*reinterpret_cast<int*>(0) = 0;

	LobbyHandler* pScriptLobbyHandler = m_hScriptLobbyHandler.ToPtr();
	pScriptLobbyHandler->OnGameRoomPlayerStateChanged(notify.m_accountId, PlayerState::None);
}

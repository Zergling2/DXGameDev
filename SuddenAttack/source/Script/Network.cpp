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
		case Protocol::SC_RES_BROADCAST_CHAT_MSG:
			PktProcSCResBroadcastChatMsg(std::move(packet));
			break;
		case Protocol::SC_RES_GAME_LIST:
			PktProcSCResGameList(std::move(packet));
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

	// 로그인 성공
	Account* pScriptAccount = m_hScriptAccount.ToPtr();
	pScriptAccount->SetLevel(res.m_level);
	pScriptAccount->SetExp(res.m_exp);
	pScriptAccount->SetPoint(res.m_point);
	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), res.m_nickname, _countof(res.m_nickname));
	nickname[MAX_NICKNAME_LEN] = L'\0';	//null termination 강제.
	pScriptAccount->SetNickname(nickname);

	pScriptLobbyHandler->SetLobbyState(LobbyState::GameListBrowser);
	return;
}

void Network::PktProcSCResBroadcastChatMsg(winppy::Packet packet)
{
	SCResBroadcastChatMsg req;
	if (!packet->ReadBytes(&req, sizeof(req)))
		return;

	if (req.m_chatMsgLen > MAX_CHAT_MSG_LEN)
		return;

	wchar_t finalString[MAX_CHAT_MSG_LEN + MAX_NICKNAME_LEN + 4];

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy_s(nickname, _countof(nickname), req.m_nickname, _countof(req.m_nickname));
	nickname[MAX_NICKNAME_LEN] = L'\0';
	
	wchar_t chatMsg[MAX_CHAT_MSG_LEN + 1];
	packet->ReadBytes(chatMsg, sizeof(wchar_t) * req.m_chatMsgLen);
	chatMsg[req.m_chatMsgLen] = L'\0';

	StringCchPrintfW(finalString, _countof(finalString), L"%ls: %ls", nickname, chatMsg);

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
	for (uint32_t i = 0; i < res.m_itemCount; ++i)
	{
		SCResGameListItem resItem;
		packet->ReadBytes(&resItem, sizeof(resItem));

		gameRoomList.emplace_back();
		GameRoomItem& item = gameRoomList.back();

		item.m_roomId = resItem.m_roomId;
		item.m_gameNo = resItem.m_gameNo;
		item.m_maxPlayer = resItem.m_maxPlayer;
		item.m_currPlayer = resItem.m_currPlayer;
		item.m_gameMap = resItem.m_gameMap;
		item.m_gameMode = resItem.m_gameMode;
		item.m_gameRoomState = resItem.m_gameRoomState;
		if (resItem.m_gameNameLen > MAX_GAME_NAME_LEN)
			resItem.m_gameNameLen = MAX_GAME_NAME_LEN;
		wmemcpy_s(item.m_gameName, _countof(item.m_gameName), resItem.m_gameName, resItem.m_gameNameLen);
		item.m_gameName[resItem.m_gameNameLen] = L'\0';
	}

	pScriptLobbyHandler->UpdateGameListBrowserUI();
}

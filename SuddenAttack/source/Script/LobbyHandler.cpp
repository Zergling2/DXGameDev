#include "LobbyHandler.h"
#include "Protocol.h"
#include "GameResources.h"
#include "Network.h"
#include "../Resource/GameInfo.h"
#include <openssl/sha.h>

using namespace ze;

LobbyHandler::LobbyHandler(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_needUIUpdate(false)
	, m_lobbyState(LobbyState::None)
	, m_textLobbyChatMsgCount(0)
	, m_hScriptGameResources()
	, m_hScriptNetwork()
	, m_hImageLobbyBgr()
	, m_hPanelLoginUIRoot()
	, m_hInputFieldId()
	, m_hInputFieldPw()
	, m_hTextIdPwInputFieldHelpMsg()
	, m_hButtonOpenShop()
	, m_hButtonUserInfo()
	, m_hPanelOkMsgBoxRoot()
	, m_hTextOkMsgBoxMsg()
	, m_hPanelChannelBrowserRoot()
	, m_hTextChannelHeadcount{}
	, m_hPanelChatRoot()
	, m_hInputFieldChatMsg()
	, m_hTextLobbyChatMsg()
	, m_hPanelGameListBrowserRoot()
	, m_hPanelGameSelectedIndicator()
	, m_hTextGameNo{}
	, m_hTextGameName{}
	, m_hTextGameMap{}
	, m_hTextGameHeadcount{}
	, m_hTextGameMode{}
	, m_hTextGameState{}
	, m_hButtonJoinGameRoom{}
	, m_hTextGameListPage()
	, m_hPanelCreateGameRoomRoot()
	, m_hInputFieldCreateGameRoomName()
	, m_hRadioButtonGameRoomMaxPlayer{}
	, m_createGameRoomMaxPlayerSelected(GameRoomMaxPlayer::Game8vs8)
	, m_gameRoomId(0)
	, m_gameRoomMaxPlayer(GameRoomMaxPlayer::Unknown)
	, m_gameRoomGameMap(GameMap::Unknown)
	, m_gameRoomGameMode(GameMode::Unknown)
	, m_gameRoomMyTeam(GameTeam::Unknown)
	, m_gameRoomRedTeamPlayers{}
	, m_gameRoomBlueTeamPlayers{}
	, m_gameRoomRedTeamPlayersCount(0)
	, m_gameRoomBlueTeamPlayersCount(0)
	, m_hPanelGameRoomRoot()
	, m_hTextGameRoomNamePanel()
	, m_hTextGameRoomRedTeamPlayers{}
	, m_hTextGameRoomBlueTeamPlayers{}
	, m_gameRoomList()
	, m_currGameListContextNo(0)
	, m_gameListReqContextNo(0)
	, m_currGameListPage(1)
{
	m_gameRoomList.reserve(1 << 8);
}

void LobbyHandler::Awake()
{
	// SetResolution(1600, 900, DisplayMode::Windowed);
	SetResolution(0, 0, DisplayMode::BorderlessWindowed);
}

void LobbyHandler::Start()
{
	this->SetLobbyState(LobbyState::Login);
	// this->SetLobbyState(LobbyState::GameRoom);
}

void LobbyHandler::Update()
{
	if (m_needUIUpdate)
	{
		this->UpdateUI();
		m_needUIUpdate = false;
	}

	if (m_lobbyState == LobbyState::GameListBrowser && !m_hPanelCreateGameRoomRoot.ToPtr()->IsActiveSelf())	// 방 목록 탐색중인 경우 선택 게임 행 강조 표시
	{
		bool selected = false;
		for (size_t i = 0; i < MAX_GAME_PER_LIST_PAGE; ++i)
		{
			const POINT mp = Input::GetInstance()->GetMousePosition();
			const IUIObject* pUIObject = m_hButtonJoinGameRoom[i].ToPtr();

			if (!pUIObject->IsActiveSelf())
				break;

			if (pUIObject->HitTest(mp))
			{
				if (!m_hPanelGameSelectedIndicator.ToPtr()->IsActiveSelf())
					m_hPanelGameSelectedIndicator.ToPtr()->SetActive(true);
				m_hPanelGameSelectedIndicator.ToPtr()->m_transform.SetPositionY(pUIObject->m_transform.GetPositionY());
				selected = true;
				break;
			}
		}

		if (!selected)
		{
			if (m_hPanelGameSelectedIndicator.ToPtr()->IsActiveSelf())
				m_hPanelGameSelectedIndicator.ToPtr()->SetActive(false);
		}
	}

	if (UIObjectManager::GetInstance()->GetFocusedUI() == m_hInputFieldChatMsg.ToPtr())
	{
		if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_RETURN))
			this->OnClickSendChatMsg();
	}
}

void LobbyHandler::OnClickLogin()
{
	InputField* pInputFieldId = static_cast<InputField*>(m_hInputFieldId.ToPtr());
	InputField* pInputFieldPw = static_cast<InputField*>(m_hInputFieldPw.ToPtr());

	// 서버로 로그인 요청 전송
	Text* pTextIdPwInputFieldHelpMsg = static_cast<Text*>(m_hTextIdPwInputFieldHelpMsg.ToPtr());
	if (pInputFieldId->GetText().length() < MIN_ID_LEN || pInputFieldPw->GetText().length() < MIN_PW_LEN)
	{
		pTextIdPwInputFieldHelpMsg->SetActive(true);
		return;
	}
	else
	{
		pTextIdPwInputFieldHelpMsg->SetActive(false);
	}

	assert(pInputFieldId->GetText().length() <= MAX_ID_LEN);
	assert(pInputFieldPw->GetText().length() <= MAX_PW_LEN);

	char buf[(MAX_PW_LEN + 1) * 4];		// UTF-8 변환 결과 버퍼
	int len = WideCharToMultiByte(
		CP_UTF8,
		0,
		pInputFieldPw->GetText().c_str(),
		-1,		// NULL 종료까지 변환
		buf,
		sizeof(buf),
		nullptr,
		nullptr
	);

	if (len == 0)
	{
		pTextIdPwInputFieldHelpMsg->SetActive(true);
		return;
	}

	unsigned char hpw[SHA256_DIGEST_LENGTH];
	ZeroMemory(hpw, sizeof(hpw));

	SHA256(reinterpret_cast<unsigned char*>(buf), len - 1, hpw);	// NULL 문자 제외하고 해싱

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
		return;

	winppy::Packet outPacket;
	CSReqLogin req;
	req.m_idLen = static_cast<uint16_t>(pInputFieldId->GetText().length());
	wmemcpy_s(req.m_id, _countof(req.m_id), pInputFieldId->GetText().c_str(), pInputFieldId->GetText().length());
	memcpy(req.m_hpw, hpw, sizeof(req.m_hpw));
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_LOGIN));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickExitGame()
{
	Runtime::GetInstance()->Exit();
}

void LobbyHandler::OnClickExitGameListBrowser()
{
	// TEST CODE
}

void LobbyHandler::SendJoinChannelPacket(uint16_t channelId)
{
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
		return;

	CSReqJoinChannel req;
	req.m_channelId = channelId;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_JOIN_CHANNEL));
	outPacket->WriteBytes(&req, sizeof(req));

	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickJoinGameRoomImpl(size_t index)
{
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
		return;

	if (m_gameRoomList.size() > 0)
	{
		uint32_t firstItemIndex = (m_currGameListPage - 1) * MAX_GAME_PER_LIST_PAGE;
		uint32_t lastItemIndex = firstItemIndex + (MAX_GAME_PER_LIST_PAGE - 1);
		if (lastItemIndex > m_gameRoomList.size() - 1)
			lastItemIndex = static_cast<uint32_t>(m_gameRoomList.size() - 1);

		uint32_t currPageItemCount = lastItemIndex - firstItemIndex + 1;
		assert(currPageItemCount <= MAX_GAME_PER_LIST_PAGE);

		if (index >= currPageItemCount)
			return;


		CSReqJoinGameRoom req;
		req.m_gameRoomId = m_gameRoomList[firstItemIndex + index].m_gameRoomId;

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_JOIN_GAME_ROOM));
		outPacket->WriteBytes(&req, sizeof(req));

		pScriptNetwork->GetClient().Send(std::move(outPacket));
	}
}

void LobbyHandler::OnClickJoinChannel0()
{
	this->SendJoinChannelPacket(0);
}

void LobbyHandler::OnClickJoinChannel1()
{
	this->SendJoinChannelPacket(1);
}

void LobbyHandler::OnClickJoinChannel2()
{
	this->SendJoinChannelPacket(2);
}

void LobbyHandler::OnClickJoinChannel3()
{
	this->SendJoinChannelPacket(3);
}

void LobbyHandler::OnClickJoinChannel4()
{
	this->SendJoinChannelPacket(4);
}

void LobbyHandler::OnClickJoinChannel5()
{
	this->SendJoinChannelPacket(5);
}

void LobbyHandler::OnClickRefreshChannelList()
{
	CSReqChannelInfo req;
	req.m_serverId = 0;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CHANNEL_INFO));
	outPacket->WriteBytes(&req, sizeof(req));

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickSendChatMsg()
{
	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	if (pInputFieldChatMsg->GetText().length() == 0)
		return;
	
	CSReqSendChatMsg req;
	req.m_chatMsgLen = static_cast<uint16_t>(pInputFieldChatMsg->GetText().length());
	assert(req.m_chatMsgLen <= MAX_CHAT_MSG_LEN);

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_SEND_CHAT_MSG));
	outPacket->WriteBytes(&req, sizeof(req));
	outPacket->WriteBytes(pInputFieldChatMsg->GetText().c_str(), sizeof(wchar_t) * req.m_chatMsgLen);

	pInputFieldChatMsg->GetText().clear();	// 채팅 입력 클리어

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickRefreshGameList()
{
	ClearAllGameListItem();

	m_gameRoomList.clear();
	m_currGameListPage = 1;

	CSReqGameRoomList req;
	m_currGameListContextNo = m_gameListReqContextNo;
	req.m_reqContextNo = m_currGameListContextNo;
	++m_gameListReqContextNo;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_GAME_LIST));
	outPacket->WriteBytes(&req, sizeof(req));
	
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickGameListPrev()
{
	if (m_currGameListPage <= 1)
		return;

	m_currGameListPage -= 1;
	UpdateGameListBrowserUI();
}

void LobbyHandler::OnClickGameListNext()
{
	uint32_t gameListPageCount = GetGameListPageCount();
	if (m_currGameListPage >= gameListPageCount)
		return;

	m_currGameListPage += 1;
	UpdateGameListBrowserUI();
}

void LobbyHandler::OnClickCreateGameRoom()
{
	m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);

	static_cast<RadioButton*>(m_hRadioButtonGameRoomMaxPlayer[static_cast<size_t>(m_createGameRoomMaxPlayerSelected)].ToPtr())->SetCheck();

	m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(true);
}

void LobbyHandler::OnClickCreateGameRoomReq()
{
	InputField* pInputFieldCreateGameRoomName = static_cast<InputField*>(m_hInputFieldCreateGameRoomName.ToPtr());

	CSReqCreateGameRoom req;
	req.m_maxPlayer = m_createGameRoomMaxPlayerSelected;
	req.m_gameRoomNameLen = static_cast<uint16_t>(pInputFieldCreateGameRoomName->GetText().length());

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CREATE_GAME_ROOM));
	outPacket->WriteBytes(&req, sizeof(req));
	outPacket->WriteBytes(pInputFieldCreateGameRoomName->GetText().c_str(), sizeof(wchar_t) * pInputFieldCreateGameRoomName->GetText().length());

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickExitGameRoom()
{
	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_EXIT_GAME_ROOM));

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickCreateGameRoomCancel()
{
	static_cast<InputField*>(m_hInputFieldCreateGameRoomName.ToPtr())->GetText().clear();
	m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);
	m_hPanelGameListBrowserRoot.ToPtr()->SetActive(true);
}

void LobbyHandler::OnClickRadioButtonGameRoom1vs1()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game1vs1;
}

void LobbyHandler::OnClickRadioButtonGameRoom2vs2()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game2vs2;
}

void LobbyHandler::OnClickRadioButtonGameRoom3vs3()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game3vs3;
}

void LobbyHandler::OnClickRadioButtonGameRoom4vs4()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game4vs4;
}

void LobbyHandler::OnClickRadioButtonGameRoom5vs5()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game5vs5;
}

void LobbyHandler::OnClickRadioButtonGameRoom6vs6()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game6vs6;
}

void LobbyHandler::OnClickRadioButtonGameRoom7vs7()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game7vs7;
}

void LobbyHandler::OnClickRadioButtonGameRoom8vs8()
{
	m_createGameRoomMaxPlayerSelected = GameRoomMaxPlayer::Game8vs8;
}

void LobbyHandler::OnClickJoinGameRoom0()
{
	OnClickJoinGameRoomImpl(0);
}

void LobbyHandler::OnClickJoinGameRoom1()
{
	OnClickJoinGameRoomImpl(1);
}

void LobbyHandler::OnClickJoinGameRoom2()
{
	OnClickJoinGameRoomImpl(2);
}

void LobbyHandler::OnClickJoinGameRoom3()
{
	OnClickJoinGameRoomImpl(3);
}

void LobbyHandler::OnClickJoinGameRoom4()
{
	OnClickJoinGameRoomImpl(4);
}

void LobbyHandler::OnClickJoinGameRoom5()
{
	OnClickJoinGameRoomImpl(5);
}

void LobbyHandler::OnClickJoinGameRoom6()
{
	OnClickJoinGameRoomImpl(6);
}

void LobbyHandler::OnClickJoinGameRoom7()
{
	OnClickJoinGameRoomImpl(7);
}

void LobbyHandler::OnClickJoinGameRoom8()
{
	OnClickJoinGameRoomImpl(8);
}

void LobbyHandler::OnClickJoinGameRoom9()
{
	OnClickJoinGameRoomImpl(9);
}

void LobbyHandler::OnClickJoinGameRoom10()
{
	OnClickJoinGameRoomImpl(10);
}

void LobbyHandler::OnClickJoinGameRoom11()
{
	OnClickJoinGameRoomImpl(11);
}

void LobbyHandler::OnClickMoveToBlueTeam()
{
	if (m_gameRoomMyTeam == GameTeam::BlueTeam)
		return;

	CSReqChangeTeam req;
	req.m_newTeam = GameTeam::BlueTeam;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CHANGE_TEAM));
	outPacket->WriteBytes(&req, sizeof(req));

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickMoveToRedTeam()
{
	if (m_gameRoomMyTeam == GameTeam::RedTeam)
		return;

	CSReqChangeTeam req;
	req.m_newTeam = GameTeam::RedTeam;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CHANGE_TEAM));
	outPacket->WriteBytes(&req, sizeof(req));

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickOkMsgBoxOk()
{
	IUIObject* pPanelOkMsgBoxRoot = m_hPanelOkMsgBoxRoot.ToPtr();
	pPanelOkMsgBoxRoot->SetActive(false);
	pPanelOkMsgBoxRoot->m_transform.SetParent(&m_hImageLobbyBgr.ToPtr()->m_transform);
}

void LobbyHandler::SetLobbyState(LobbyState state)
{
	LobbyState old = m_lobbyState;

	m_lobbyState = state;
	if (old != m_lobbyState)
		m_needUIUpdate = true;
}

void LobbyHandler::ClearChatMsgs()
{
	for (size_t i = 0; i < _countof(m_hTextLobbyChatMsg); ++i)
		static_cast<Text*>(m_hTextLobbyChatMsg[i].ToPtr())->GetText().clear();

	m_textLobbyChatMsgCount = 0;
}

void LobbyHandler::ClearGameRoomInfo()
{
	m_gameRoomId = 0;
	m_gameRoomHostNetId = 0;
	m_gameRoomMaxPlayer = GameRoomMaxPlayer::Unknown;
	m_gameRoomGameMap = GameMap::Unknown;
	m_gameRoomGameMode = GameMode::Unknown;
	m_gameRoomMyTeam = GameTeam::Unknown;
	static_cast<Text*>(m_hTextGameRoomNamePanel.ToPtr())->GetText().clear();

	for (size_t i = 0; i < _countof(m_hTextGameRoomRedTeamPlayers); ++i)
		static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr())->GetText().clear();

	for (size_t i = 0; i < _countof(m_hTextGameRoomBlueTeamPlayers); ++i)
		static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr())->GetText().clear();

	m_gameRoomRedTeamPlayersCount = 0;
	m_gameRoomBlueTeamPlayersCount = 0;
}

void LobbyHandler::SetGameRoomInfo(uint64_t gameRoomId, uint64_t hostNetId, GameRoomMaxPlayer maxPlayer, GameMap gameMap, GameMode gameMode, const wchar_t* gameRoomHeadText, GameTeam myTeam)
{
	m_gameRoomId = gameRoomId;
	m_gameRoomHostNetId = hostNetId;
	m_gameRoomMaxPlayer = maxPlayer;
	m_gameRoomGameMap = gameMap;
	m_gameRoomGameMode = gameMode;
	static_cast<Text*>(m_hTextGameRoomNamePanel.ToPtr())->SetText(gameRoomHeadText);

	wchar_t gameInfoBuf[128];
	const uint32_t maxPlayerPerTeam = static_cast<uint32_t>(m_gameRoomMaxPlayer) + 1;
	StringCchPrintfW(gameInfoBuf, _countof(gameInfoBuf), L"맵\t\t%s\n게임 모드\t%s\n최대 인원\t%u vs %u\n게임 시간\t10분",
		GameMapInfo::GetGameMapString(m_gameRoomGameMap),
		GameModeInfo::GetGameModeString(m_gameRoomGameMode),
		maxPlayerPerTeam, maxPlayerPerTeam
	);
	static_cast<Text*>(m_hTextGameRoomInfo.ToPtr())->SetText(gameInfoBuf);

	m_gameRoomMyTeam = myTeam;	// 현재 내 팀 정보 설정
}

void LobbyHandler::AddGameRoomPlayerInfo(GameTeam team, uint64_t netId, uint16_t level, const wchar_t* nickname)
{
	assert(team != GameTeam::Unknown);
	assert(m_gameRoomMaxPlayer != GameRoomMaxPlayer::Unknown);

	const size_t maxPlayerPerTeam = static_cast<size_t>(m_gameRoomMaxPlayer) + 1;

	wchar_t textBuf[MAX_NICKNAME_LEN + 32];
	if (m_gameRoomHostNetId == netId)
		StringCchPrintfW(textBuf, _countof(textBuf), L"[HOST] Lv.%u\t%s", static_cast<uint32_t>(level), nickname);
	else
		StringCchPrintfW(textBuf, _countof(textBuf), L"Lv.%u\t%s", static_cast<uint32_t>(level), nickname);

	switch (team)
	{
	case GameTeam::RedTeam:
		assert(m_gameRoomRedTeamPlayersCount < maxPlayerPerTeam);
		m_gameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount].m_netId = netId;
		m_gameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount].m_level = level;
		wcscpy_s(m_gameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount].m_nickname, nickname);

		static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount].ToPtr())->SetText(textBuf);
		++m_gameRoomRedTeamPlayersCount;
		break;
	case GameTeam::BlueTeam:
		assert(m_gameRoomBlueTeamPlayersCount < maxPlayerPerTeam);
		m_gameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount].m_netId = netId;
		m_gameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount].m_level = level;
		wcscpy_s(m_gameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount].m_nickname, nickname);

		static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount].ToPtr())->SetText(textBuf);
		++m_gameRoomBlueTeamPlayersCount;
		break;
	default:
		*reinterpret_cast<int*>(0) = 0;
		break;
	}
}

void LobbyHandler::MoveGameRoomPlayerInfo(uint64_t netId, GameTeam newTeam)
{
	uint16_t level = (std::numeric_limits<uint16_t>::max)();
	wchar_t nickname[MAX_NICKNAME_LEN + 1];

	bool found = false;
	do
	{
		for (uint8_t i = 0; i < m_gameRoomRedTeamPlayersCount; ++i)
		{
			const GameRoomPlayer& player = m_gameRoomRedTeamPlayers[i];
			if (player.m_netId == netId)
			{
				found = true;

				// 정보 백업
				level = player.m_level;
				wcscpy_s(nickname, player.m_nickname);

				for (uint8_t j = i + 1; j < m_gameRoomRedTeamPlayersCount; ++j)
				{
					m_gameRoomRedTeamPlayers[j - 1] = std::move(m_gameRoomRedTeamPlayers[j]);
					static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[j - 1].ToPtr())->GetText() = std::move(static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[j].ToPtr())->GetText());
				}
				static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount - 1].ToPtr())->GetText().clear();

				--m_gameRoomRedTeamPlayersCount;
				break;
			}
		}

		if (found)
			break;

		for (uint8_t i = 0; i < m_gameRoomBlueTeamPlayersCount; ++i)
		{
			const GameRoomPlayer& player = m_gameRoomBlueTeamPlayers[i];
			if (player.m_netId == netId)
			{
				found = true;

				// 정보 백업
				level = player.m_level;
				wcscpy_s(nickname, player.m_nickname);

				for (uint8_t j = i + 1; j < m_gameRoomBlueTeamPlayersCount; ++j)
				{
					m_gameRoomBlueTeamPlayers[j - 1] = std::move(m_gameRoomBlueTeamPlayers[j]);
					static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[j - 1].ToPtr())->GetText() = std::move(static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[j].ToPtr())->GetText());
				}
				static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount - 1].ToPtr())->GetText().clear();

				--m_gameRoomBlueTeamPlayersCount;
				break;
			}
		}
	} while (false);

	assert(found);

	AddGameRoomPlayerInfo(newTeam, netId, level, nickname);
}

void LobbyHandler::RemoveGameRoomPlayerInfo(uint64_t netId)
{
	bool found = false;

	do
	{
		for (uint8_t i = 0; i < m_gameRoomRedTeamPlayersCount; ++i)
		{
			if (m_gameRoomRedTeamPlayers[i].m_netId == netId)
			{
				found = true;

				for (uint8_t j = i + 1; j < m_gameRoomRedTeamPlayersCount; ++j)
				{
					m_gameRoomRedTeamPlayers[j - 1] = std::move(m_gameRoomRedTeamPlayers[j]);
					static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[j - 1].ToPtr())->GetText() = std::move(static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[j].ToPtr())->GetText());
				}
				static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[m_gameRoomRedTeamPlayersCount - 1].ToPtr())->GetText().clear();

				--m_gameRoomRedTeamPlayersCount;
				break;
			}
		}

		if (found)
			break;

		for (uint8_t i = 0; i < m_gameRoomBlueTeamPlayersCount; ++i)
		{
			if (m_gameRoomBlueTeamPlayers[i].m_netId == netId)
			{
				found = true;

				for (uint8_t j = i + 1; j < m_gameRoomBlueTeamPlayersCount; ++j)
				{
					m_gameRoomBlueTeamPlayers[j - 1] = std::move(m_gameRoomBlueTeamPlayers[j]);
					static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[j - 1].ToPtr())->GetText() = std::move(static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[j].ToPtr())->GetText());
				}
				static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[m_gameRoomBlueTeamPlayersCount - 1].ToPtr())->GetText().clear();

				--m_gameRoomBlueTeamPlayersCount;
				break;
			}
		}
	} while (false);
	
}

void LobbyHandler::OnGameRoomHostChanged(uint64_t newHostNetId)
{
	const uint64_t prevHostNetId = m_gameRoomHostNetId;
	m_gameRoomHostNetId = newHostNetId;

	wchar_t textBuf[MAX_NICKNAME_LEN + 32];
	do
	{
		for (uint8_t i = 0; i < m_gameRoomRedTeamPlayersCount; ++i)
		{
			const GameRoomPlayer& player = m_gameRoomRedTeamPlayers[i];
			if (player.m_netId == prevHostNetId)
			{
				StringCchPrintfW(textBuf, _countof(textBuf), L"Lv.%u\t%s", static_cast<uint32_t>(player.m_level), player.m_nickname);
				static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr())->SetText(textBuf);
			}

			if (player.m_netId == newHostNetId)
			{
				StringCchPrintfW(textBuf, _countof(textBuf), L"[HOST] Lv.%u\t%s", static_cast<uint32_t>(player.m_level), player.m_nickname);
				static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr())->SetText(textBuf);
			}
		}

		for (uint8_t i = 0; i < m_gameRoomBlueTeamPlayersCount; ++i)
		{
			const GameRoomPlayer& player = m_gameRoomBlueTeamPlayers[i];
			if (player.m_netId == prevHostNetId)
			{
				StringCchPrintfW(textBuf, _countof(textBuf), L"Lv.%u\t%s", static_cast<uint32_t>(player.m_level), player.m_nickname);
				static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr())->SetText(textBuf);
			}

			if (player.m_netId == newHostNetId)
			{
				StringCchPrintfW(textBuf, _countof(textBuf), L"[HOST] Lv.%u\t%s", static_cast<uint32_t>(player.m_level), player.m_nickname);
				static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr())->SetText(textBuf);
			}
		}
	} while (false);
}

void LobbyHandler::AddChatMsg(const wchar_t* msg)
{
	if (m_lobbyState != LobbyState::GameListBrowser && m_lobbyState != LobbyState::GameRoom)
		return;

	if (m_textLobbyChatMsgCount < CHAT_MSG_ITEM_ROW_COUNT)
	{
		Text* pTextLobbyChatMsg = static_cast<Text*>(m_hTextLobbyChatMsg[m_textLobbyChatMsgCount].ToPtr());
		pTextLobbyChatMsg->SetText(msg);
		++m_textLobbyChatMsgCount;
	}
	else
	{
		Text* pTextLobbyChatMsgs[CHAT_MSG_ITEM_ROW_COUNT];
		for (size_t i = 0; i < _countof(m_hTextLobbyChatMsg); ++i)
			pTextLobbyChatMsgs[i] = static_cast<Text*>(m_hTextLobbyChatMsg[i].ToPtr());
		
		std::wstring first = std::move(pTextLobbyChatMsgs[0]->GetText());
		for (size_t i = 0; i < _countof(m_hTextLobbyChatMsg) - 1; ++i)
			pTextLobbyChatMsgs[i]->GetText() = std::move(pTextLobbyChatMsgs[i + 1]->GetText());

		pTextLobbyChatMsgs[CHAT_MSG_ITEM_ROW_COUNT - 1]->GetText() = std::move(first);
		pTextLobbyChatMsgs[CHAT_MSG_ITEM_ROW_COUNT - 1]->GetText() = msg;
	}
}

void LobbyHandler::UpdateGameListBrowserUI()
{
	assert(m_currGameListPage > 0);

	const uint32_t pageCount = GetGameListPageCount();

	wchar_t buf[16];
	StringCchPrintfW(buf, _countof(buf), L"%u / %u", static_cast<uint32_t>(m_currGameListPage), pageCount);
	static_cast<Text*>(m_hTextGameListPage.ToPtr())->SetText(buf);

	ClearAllGameListItem();

	if (m_gameRoomList.size() > 0)
	{
		uint32_t firstItemIndex = (m_currGameListPage - 1) * MAX_GAME_PER_LIST_PAGE;
		uint32_t lastItemIndex = firstItemIndex + (MAX_GAME_PER_LIST_PAGE - 1);
		if (lastItemIndex > m_gameRoomList.size() - 1)
			lastItemIndex = static_cast<uint32_t>(m_gameRoomList.size() - 1);

		uint32_t currPageItemCount = lastItemIndex - firstItemIndex + 1;
		assert(currPageItemCount <= MAX_GAME_PER_LIST_PAGE);

		for (size_t i = 0; i < currPageItemCount; ++i)
		{
			const size_t index = firstItemIndex + i;
			const GameRoomItem& item = m_gameRoomList[index];
			wchar_t buf[128];

			StringCchPrintfW(buf, _countof(buf), L"%u", static_cast<unsigned int>(item.m_gameRoomNo));
			static_cast<Text*>(m_hTextGameNo[i].ToPtr())->SetText(buf);

			static_cast<Text*>(m_hTextGameName[i].ToPtr())->SetText(item.m_gameName);

			static_cast<Text*>(m_hTextGameMap[i].ToPtr())->SetText(GameMapInfo::GetGameMapString(item.m_gameMap));

			StringCchPrintfW(buf, _countof(buf), L"%u/%u", static_cast<unsigned int>(item.m_currPlayer), (static_cast<unsigned int>(item.m_maxPlayer) + 1) * 2);
			static_cast<Text*>(m_hTextGameHeadcount[i].ToPtr())->SetText(buf);

			static_cast<Text*>(m_hTextGameMode[i].ToPtr())->SetText(GameModeInfo::GetGameModeString(item.m_gameMode));

			Text* pTextGameState = static_cast<Text*>(m_hTextGameState[i].ToPtr());
			pTextGameState->SetText(GameRoomStateInfo::GetGameRoomStateString(item.m_gameRoomState));
			switch (item.m_gameRoomState)
			{
			case GameRoomState::InWaiting:
				pTextGameState->SetColor(Colors::GreenYellow);
				break;
			case GameRoomState::InPlay:
				pTextGameState->SetColor(Colors::OrangeRed);
				break;
			default:
				break;
			}

			static_cast<Button*>(m_hButtonJoinGameRoom[i].ToPtr())->SetActive(true);
		}
	}
}

void LobbyHandler::UpdateUI()
{
	switch (m_lobbyState)
	{
	case LobbyState::Login:
		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelOkMsgBoxRoot.ToPtr()->SetActive(false);
		m_hPanelChannelBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameRoomRoot.ToPtr()->SetActive(false);
		m_hPanelChatRoot.ToPtr()->SetActive(false);
		m_hButtonOpenShop.ToPtr()->SetActive(false);
		m_hButtonUserInfo.ToPtr()->SetActive(false);
		m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);
		// #########################################################
		
		// 현재 상태에 대응하는 UI 보이기
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texLoginBgr);
		m_hImageLobbyBgr.ToPtr()->SetActive(true);

		// 로그인 UI 표시
		m_hPanelLoginUIRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::ChannelListBrowser:
		m_hPanelOkMsgBoxRoot.ToPtr()->SetActive(false);
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameRoomRoot.ToPtr()->SetActive(false);
		m_hPanelChatRoot.ToPtr()->SetActive(false);
		m_hButtonOpenShop.ToPtr()->SetActive(false);
		m_hButtonUserInfo.ToPtr()->SetActive(false);
		m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);

		// 게임 리스트 브라우저 UI 표시
		m_hPanelChannelBrowserRoot.ToPtr()->SetActive(true);
		
		OnClickRefreshChannelList();
		break;
	case LobbyState::GameListBrowser:
		// 채팅 목록 클리어
		ClearChatMsgs();

		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelOkMsgBoxRoot.ToPtr()->SetActive(false);
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);
		m_hPanelChannelBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameRoomRoot.ToPtr()->SetActive(false);
		m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);

		// #########################################################
		// 현재 상태에 대응하는 UI 보이기
		
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texGameListBgr);
		m_hImageLobbyBgr.ToPtr()->SetActive(true);

		m_hPanelChatRoot.ToPtr()->SetActive(true);
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);

		// 게임 리스트 브라우저 UI 표시
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(true);
		// #########################################################

		OnClickRefreshGameList();
		break;
	case LobbyState::GameRoom:
		// 채팅 목록 클리어
		ClearChatMsgs();

		m_hPanelOkMsgBoxRoot.ToPtr()->SetActive(false);
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);
		m_hPanelChannelBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);

		// 상점/정비 버튼 표시
		m_hPanelGameRoomRoot.ToPtr()->SetActive(true);
		m_hPanelChatRoot.ToPtr()->SetActive(true);
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);
		break;
	case LobbyState::Scoreboard:
		break;
	default:
		break;
	}
}

uint32_t LobbyHandler::GetGameListPageCount() const
{
	const uint32_t itemCount = static_cast<uint32_t>(m_gameRoomList.size());

	if (itemCount == 0)
		return 1;
	else
		return ((itemCount - 1) / MAX_GAME_PER_LIST_PAGE) + 1;
}

void LobbyHandler::ClearAllGameListItem()
{
	for (size_t i = 0; i < MAX_GAME_PER_LIST_PAGE; ++i)
	{
		static_cast<Text*>(m_hTextGameNo[i].ToPtr())->GetText().clear();
		static_cast<Text*>(m_hTextGameName[i].ToPtr())->GetText().clear();
		static_cast<Text*>(m_hTextGameMap[i].ToPtr())->GetText().clear();
		static_cast<Text*>(m_hTextGameHeadcount[i].ToPtr())->GetText().clear();
		static_cast<Text*>(m_hTextGameMode[i].ToPtr())->GetText().clear();
		static_cast<Text*>(m_hTextGameState[i].ToPtr())->GetText().clear();
		static_cast<Button*>(m_hButtonJoinGameRoom[i].ToPtr())->SetActive(false);
	}
}

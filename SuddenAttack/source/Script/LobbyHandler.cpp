#include "LobbyHandler.h"
#include "Protocol.h"
#include "GameResources.h"
#include "Network.h"
#include "Account.h"
#include "../Resource/GameInfo.h"

using namespace ze;

static const wchar_t* SERVER_CONNECTION_FAIL_MESSAGE = L"서버에 연결하지 못했습니다.";
static const wchar_t* WRONG_TYPE_PASSWORD_MESSAGE = L"비밀번호가 올바른 형식이 아닙니다.";

static bool IsAlpha(wchar_t ch)
{
	return (L'A' <= ch && ch <= L'Z') || (L'a' <= ch && ch <= L'z');
}

static bool IsNum(wchar_t ch)
{
	return L'0' <= ch && ch <= '9';
}

static bool IsSpecialChar(wchar_t ch)
{
	switch (ch)
	{
	case L'!':
	case L'@':
	case L'#':
	case L'$':
	case L'%':
	case L'^':
	case L'&':
	case L'*':
	case L'(':
	case L')':
	case L'-':
	case L'_':
	case L'=':
	case L'+':
		return true;
	default:
		return false;
	}
}

static bool IsAlphaNumOnly(const wchar_t* str)
{
	if (str == nullptr)
		return false;

	while (*str != L'\0')
	{
		const wchar_t ch = *str;

		if (!IsAlpha(ch) && !IsNum(ch))
			return false;

		++str;
	}

	return true;
}

static bool IsAlphaNumSpecialOnly(const wchar_t* str)
{
	if (str == nullptr)
		return false;

	while (*str != L'\0')
	{
		const wchar_t ch = *str;

		if (!IsAlpha(ch) && !IsNum(ch) && !IsSpecialChar(ch))
			return false;

		++str;
	}

	return true;
}

LobbyHandler::LobbyHandler(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_needUIUpdate(false)
	, m_showSelectedGameRoomIndicator(true)
	, m_lobbyState(LobbyState::None)
	, m_textLobbyChatMsgCount(0)
	, m_hAudioBGM()
	, m_hScriptGameResources()
	, m_hScriptNetwork()
	, m_hImageLobbyBgr()
	, m_hPanelLoginWindowRoot()
	, m_hInputFieldLoginId()
	, m_hInputFieldLoginPw()
	, m_hTextLoginHelpMsg()
	, m_hPanelCreateAccountWindowRoot()
	, m_hInputFieldCreateAccountId()
	, m_hTextCreateAccountIdDuplicateCheckMsg()
	, m_hInputFieldCreateAccountPw()
	, m_hInputFieldCreateAccountPwDoubleCheck()
	, m_hTextCreateAccountPwCheckMsg()
	, m_hInputFieldCreateAccountNickname()
	, m_hTextCreateAccountNicknameDuplicateCheckMsg()
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
	, m_hRadioButtonGameRoomTeamFormat{}
	, m_createGameRoomTeamFormatSelected(GameRoomTeamFormat::Team8vs8)
	, m_gameRoomHostAccountId(0)
	, m_gameRoomTeamFormat(GameRoomTeamFormat::Unknown)
	, m_gameRoomGameMap(GameMap::Unknown)
	, m_gameRoomMyTeam(GameTeam::Unknown)
	, m_gameRoomRedTeamPlayers()
	, m_gameRoomBlueTeamPlayers()
	, m_hPanelGameRoomRoot()
	, m_hTextGameRoomNamePanel()
	, m_hTextGameRoomRedTeamPlayers{}
	, m_hTextGameRoomBlueTeamPlayers{}
	, m_hButtonHostGameStart()
	, m_hButtonGameReady()
	, m_hButtonGameUnready()
	, m_gameRoomList()
	, m_currGameListContextNo(0)
	, m_gameListReqContextNo(0)
	, m_currGameListPage(0)
{
	m_gameRoomList.reserve(256);
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
	if (Input::GetInstance()->GetKeyDown(Keycode::KEY_F5))
	{
		SetResolution(1366, 768, DisplayMode::Windowed);
		SetResolution(0, 0, DisplayMode::BorderlessWindowed);
	}

	if (m_needUIUpdate)
	{
		this->UpdateUI();
		m_needUIUpdate = false;
	}

	if (m_lobbyState == LobbyState::GameListBrowser && m_showSelectedGameRoomIndicator)	// 방 목록 탐색중인 경우 선택 게임 행 강조 표시
	{
		bool selected = false;
		for (size_t i = 0; i < MAX_GAMES_PER_LIST_PAGE; ++i)
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
		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_RETURN))
			this->OnClickSendChatMsg();
	}
}

void LobbyHandler::OnClickLogin()
{
	InputField* pInputFieldId = static_cast<InputField*>(m_hInputFieldLoginId.ToPtr());
	InputField* pInputFieldPw = static_cast<InputField*>(m_hInputFieldLoginPw.ToPtr());

	// 서버로 로그인 요청 전송
	Text* pTextLoginHelpMsg = static_cast<Text*>(m_hTextLoginHelpMsg.ToPtr());
	const size_t idLen = pInputFieldId->GetText().length();
	const size_t pwLen = pInputFieldPw->GetText().length();
	if (idLen < MIN_ID_LEN || pwLen < MIN_PW_LEN)
	{
		pTextLoginHelpMsg->SetColor(Colors::Orange);
		pTextLoginHelpMsg->SetText(L"아이디 또는 비밀번호가 올바른 형식이 아닙니다.");
		return;
	}

	if (!IsAlphaNumOnly(pInputFieldId->GetText().c_str()) || !IsAlphaNumSpecialOnly(pInputFieldPw->GetText().c_str()))
	{
		pTextLoginHelpMsg->SetColor(Colors::Orange);
		pTextLoginHelpMsg->SetText(L"아이디 또는 비밀번호에 유효하지 않은 문자가 포함되어 있습니다.");
		return;
	}

	assert(idLen <= MAX_ID_LEN);
	assert(pwLen <= MAX_PW_LEN);

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
	{
		pTextLoginHelpMsg->SetColor(Colors::Red);
		pTextLoginHelpMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		return;
	}

	pTextLoginHelpMsg->SetColor(Colors::Green);
	pTextLoginHelpMsg->SetText(L"로그인 대기중...");

	winppy::Packet outPacket;
	CSReqLogin req;
	req.m_idLen = static_cast<uint16_t>(idLen);
	wmemcpy_s(req.m_id, _countof(req.m_id), pInputFieldId->GetText().c_str(), req.m_idLen);
	req.m_pwLen = static_cast<uint16_t>(pwLen);
	wmemcpy_s(req.m_pw, _countof(req.m_pw), pInputFieldPw->GetText().c_str(), req.m_pwLen);
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_LOGIN));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickCreateAccount()
{
	static_cast<InputField*>(m_hInputFieldLoginId.ToPtr())->GetText().clear();
	static_cast<InputField*>(m_hInputFieldLoginPw.ToPtr())->GetText().clear();
	static_cast<Text*>(m_hTextLoginHelpMsg.ToPtr())->GetText().clear();

	m_hPanelLoginWindowRoot.ToPtr()->SetActive(false);
	m_hPanelCreateAccountWindowRoot.ToPtr()->SetActive(true);
}

void LobbyHandler::OnClickIdDuplicateCheck()
{
	InputField* pInputFieldCreateAccountId = static_cast<InputField*>(m_hInputFieldCreateAccountId.ToPtr());

	// 서버로 ID 중복 검사 요청
	Text* pTextCreateAccountIdDuplicateCheckMsg = static_cast<Text*>(m_hTextCreateAccountIdDuplicateCheckMsg.ToPtr());
	if (pInputFieldCreateAccountId->GetText().length() < MIN_ID_LEN)
	{
		wchar_t msgBuf[40];
		StringCchPrintfW(msgBuf, _countof(msgBuf), L"아이디는 최소 %u글자 이상이어야 합니다.", static_cast<uint32_t>(MIN_ID_LEN));
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(msgBuf);
		return;
	}

	if (!IsAlphaNumOnly(pInputFieldCreateAccountId->GetText().c_str()))
	{
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(L"아이디에는 영문 및 숫자만 사용할 수 있습니다.");
		return;
	}

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
	{
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Red);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		return;
	}

	pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Green);
	pTextCreateAccountIdDuplicateCheckMsg->SetText(L"중복 검사 요청중...");

	winppy::Packet outPacket;
	CSReqIdDuplicateCheck req;
	req.m_idLen = static_cast<uint16_t>(pInputFieldCreateAccountId->GetText().length());
	wmemcpy_s(req.m_id, _countof(req.m_id), pInputFieldCreateAccountId->GetText().c_str(), req.m_idLen);
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_ID_DUPLICATE_CHECK));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickNicknameDuplicateCheck()
{
	InputField* pInputFieldCreateAccountNickname = static_cast<InputField*>(m_hInputFieldCreateAccountNickname.ToPtr());

	// 서버로 ID 중복 검사 요청
	Text* pTextCreateAccountNicknameDuplicateCheckMsg = static_cast<Text*>(m_hTextCreateAccountNicknameDuplicateCheckMsg.ToPtr());
	if (pInputFieldCreateAccountNickname->GetText().length() < MIN_NICKNAME_LEN)
	{
		wchar_t msgBuf[40];
		StringCchPrintfW(msgBuf, _countof(msgBuf), L"닉네임은 최소 %u글자 이상이어야 합니다.", static_cast<uint32_t>(MIN_NICKNAME_LEN));
		pTextCreateAccountNicknameDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(msgBuf);
		return;
	}

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
	{
		pTextCreateAccountNicknameDuplicateCheckMsg->SetColor(Colors::Red);
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		return;
	}

	pTextCreateAccountNicknameDuplicateCheckMsg->SetColor(Colors::Green);
	pTextCreateAccountNicknameDuplicateCheckMsg->SetText(L"중복 검사 요청중...");

	winppy::Packet outPacket;
	CSReqNicknameDuplicateCheck req;
	req.m_nicknameLen = static_cast<uint16_t>(pInputFieldCreateAccountNickname->GetText().length());
	wmemcpy_s(req.m_nickname, _countof(req.m_nickname), pInputFieldCreateAccountNickname->GetText().c_str(), req.m_nicknameLen);
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_NICKNAME_DUPLICATE_CHECK));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickRequestCreateAccount()
{
	InputField* pInputFieldCreateAccountId = static_cast<InputField*>(m_hInputFieldCreateAccountId.ToPtr());
	InputField* pInputFieldCreateAccountNickname = static_cast<InputField*>(m_hInputFieldCreateAccountNickname.ToPtr());
	InputField* pInputFieldCreateAccountPw = static_cast<InputField*>(m_hInputFieldCreateAccountPw.ToPtr());
	InputField* pInputFieldCreateAccountPwDoubleCheck = static_cast<InputField*>(m_hInputFieldCreateAccountPwDoubleCheck.ToPtr());

	Text* pTextCreateAccountIdDuplicateCheckMsg = static_cast<Text*>(m_hTextCreateAccountIdDuplicateCheckMsg.ToPtr());
	Text* pTextCreateAccountNicknameDuplicateCheckMsg = static_cast<Text*>(m_hTextCreateAccountNicknameDuplicateCheckMsg.ToPtr());
	Text* pTextCreateAccountPwCheckMsg = static_cast<Text*>(m_hTextCreateAccountPwCheckMsg.ToPtr());
	
	const size_t idLen = pInputFieldCreateAccountId->GetText().length();
	if (idLen < MIN_ID_LEN || idLen > MAX_ID_LEN)
	{
		wchar_t msgBuf[40];
		StringCchPrintfW(msgBuf, _countof(msgBuf), L"아이디는 %u자에서 %u자 사이이어야 합니다.", static_cast<uint32_t>(MIN_ID_LEN), static_cast<uint32_t>(MAX_ID_LEN));
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(msgBuf);
		return;
	}

	if (!IsAlphaNumOnly(pInputFieldCreateAccountId->GetText().c_str()))
	{
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(L"아이디에는 영문 및 숫자만 사용할 수 있습니다.");
		return;
	}

	const size_t nicknameLen = pInputFieldCreateAccountNickname->GetText().length();
	if (nicknameLen < MIN_NICKNAME_LEN || nicknameLen > MAX_NICKNAME_LEN)
	{
		wchar_t msgBuf[40];
		StringCchPrintfW(msgBuf, _countof(msgBuf), L"닉네임은 %u자에서 %u자 사이이어야 합니다.", static_cast<uint32_t>(MIN_NICKNAME_LEN), static_cast<uint32_t>(MAX_NICKNAME_LEN));
		pTextCreateAccountNicknameDuplicateCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(msgBuf);
		return;
	}

	const size_t pwLen = pInputFieldCreateAccountPw->GetText().length();
	if (pwLen < MIN_PW_LEN || pwLen > MAX_PW_LEN)
	{
		wchar_t msgBuf[40];
		StringCchPrintfW(msgBuf, _countof(msgBuf), L"비밀번호는 %u자에서 %u자 사이이어야 합니다.", static_cast<uint32_t>(MIN_PW_LEN), static_cast<uint32_t>(MAX_PW_LEN));
		pTextCreateAccountPwCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountPwCheckMsg->SetText(msgBuf);
		return;
	}

	if (!IsAlphaNumSpecialOnly(pInputFieldCreateAccountPw->GetText().c_str()))
	{
		pTextCreateAccountPwCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountPwCheckMsg->SetText(L"비밀번호에는 영문, 숫자, 특수문자만 사용 가능합니다.");
		return;
	}

	if (pInputFieldCreateAccountPw->GetText() != pInputFieldCreateAccountPwDoubleCheck->GetText())
	{
		pTextCreateAccountPwCheckMsg->SetColor(Colors::Orange);
		pTextCreateAccountPwCheckMsg->SetText(L"비밀번호가 일치하지 않습니다.");
		return;
	}

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
	{
		pTextCreateAccountIdDuplicateCheckMsg->SetColor(Colors::Red);
		pTextCreateAccountIdDuplicateCheckMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		pTextCreateAccountNicknameDuplicateCheckMsg->SetColor(Colors::Red);
		pTextCreateAccountNicknameDuplicateCheckMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		pTextCreateAccountPwCheckMsg->SetColor(Colors::Red);
		pTextCreateAccountPwCheckMsg->SetText(SERVER_CONNECTION_FAIL_MESSAGE);
		return;
	}

	
	CSReqCreateAccount req;
	req.m_idLen = static_cast<uint16_t>(idLen);
	wmemcpy_s(req.m_id, _countof(req.m_id), pInputFieldCreateAccountId->GetText().c_str(), req.m_idLen);
	req.m_nicknameLen = static_cast<uint16_t>(nicknameLen);
	wmemcpy_s(req.m_nickname, _countof(req.m_nickname), pInputFieldCreateAccountNickname->GetText().c_str(), req.m_nicknameLen);
	req.m_pwLen = static_cast<uint16_t>(pwLen);
	wmemcpy_s(req.m_pw, _countof(req.m_pw), pInputFieldCreateAccountPw->GetText().c_str(), req.m_pwLen);

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CREATE_ACCOUNT));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickCancelCreateAccount()
{
	static_cast<InputField*>(m_hInputFieldCreateAccountId.ToPtr())->GetText().clear();
	static_cast<InputField*>(m_hInputFieldCreateAccountNickname.ToPtr())->GetText().clear();
	static_cast<InputField*>(m_hInputFieldCreateAccountPw.ToPtr())->GetText().clear();
	static_cast<InputField*>(m_hInputFieldCreateAccountPwDoubleCheck.ToPtr())->GetText().clear();
	static_cast<Text*>(m_hTextCreateAccountIdDuplicateCheckMsg.ToPtr())->GetText().clear();
	static_cast<Text*>(m_hTextCreateAccountNicknameDuplicateCheckMsg.ToPtr())->GetText().clear();
	static_cast<Text*>(m_hTextCreateAccountPwCheckMsg.ToPtr())->GetText().clear();

	m_hPanelCreateAccountWindowRoot.ToPtr()->SetActive(false);
	m_hPanelLoginWindowRoot.ToPtr()->SetActive(true);
}

void LobbyHandler::OnClickExitGame()
{
	Runtime::GetInstance()->Exit();
}

void LobbyHandler::OnClickExitGameChannel()
{
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
		return;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_EXIT_GAME_CHANNEL));

	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::SendJoinChannelPacket(uint8_t channelId)
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
		size_t firstItemIndex = m_currGameListPage * MAX_GAMES_PER_LIST_PAGE;
		size_t lastItemIndex = (std::min)(firstItemIndex + MAX_GAMES_PER_LIST_PAGE - 1, m_gameRoomList.size() - 1);

		size_t currPageItemCount = lastItemIndex - firstItemIndex + 1;
		assert(currPageItemCount <= MAX_GAMES_PER_LIST_PAGE);

		if (index >= currPageItemCount)
			return;

		CSReqJoinGameRoom req;
		req.m_gameRoomId = m_gameRoomList[firstItemIndex + index].m_id;

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
	req.m_worldId = 0;

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
	
	CSReqLobbyChat req;
	req.m_msgLen = static_cast<uint16_t>(pInputFieldChatMsg->GetText().length());
	assert(req.m_msgLen <= MAX_CHAT_MSG_LEN);

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_LOBBY_CHAT));
	outPacket->WriteBytes(&req, sizeof(req));
	outPacket->WriteBytes(pInputFieldChatMsg->GetText().c_str(), sizeof(wchar_t) * req.m_msgLen);

	pInputFieldChatMsg->GetText().clear();	// 채팅 입력 클리어

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickRefreshGameList()
{
	ClearAllGameListItem();

	m_gameRoomList.clear();
	m_currGameListPage = 0;

	CSReqGameRoomList req;
	m_currGameListContextNo = m_gameListReqContextNo;
	req.m_reqContextNo = m_currGameListContextNo;
	++m_gameListReqContextNo;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_GAME_ROOM_LIST));
	outPacket->WriteBytes(&req, sizeof(req));
	
	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickGameListPrev()
{
	if (m_currGameListPage == 0)
		return;

	m_currGameListPage -= 1;
	UpdateGameListBrowserUI();
}

void LobbyHandler::OnClickGameListNext()
{
	if (m_currGameListPage + 1 >= GetNumOfGameListPages())
		return;

	m_currGameListPage += 1;
	UpdateGameListBrowserUI();
}

void LobbyHandler::OnClickCreateGameRoom()
{
	m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);

	static_cast<RadioButton*>(m_hRadioButtonGameRoomTeamFormat[static_cast<size_t>(m_createGameRoomTeamFormatSelected)].ToPtr())->SetCheck();

	m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(true);

	ShowSelectedGameRoomIndicator(false);
}

void LobbyHandler::OnClickCreateGameRoomReq()
{
	InputField* pInputFieldCreateGameRoomName = static_cast<InputField*>(m_hInputFieldCreateGameRoomName.ToPtr());

	CSReqCreateGameRoom req;
	req.m_gameRoomTeamFormat = m_createGameRoomTeamFormatSelected;
	req.m_gameRoomNameLen = static_cast<uint16_t>(pInputFieldCreateGameRoomName->GetText().length());

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_CREATE_GAME_ROOM));
	outPacket->WriteBytes(&req, sizeof(req));
	outPacket->WriteBytes(pInputFieldCreateGameRoomName->GetText().c_str(), sizeof(wchar_t) * pInputFieldCreateGameRoomName->GetText().length());

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickHostGameStart()
{
	SceneManager::GetInstance()->LoadScene(L"Warehouse");



	// Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	// 
	// if (pScriptNetwork->GetNetId() != m_gameRoomHostNetId)
	// 	return;
	// 
	// winppy::Packet outPacket;
	// outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_HOST_GAME_START));
	// 
	// pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickGameReady()
{
	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_GAME_READY));

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickGameUnready()
{
	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_GAME_UNREADY));

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

	ShowSelectedGameRoomIndicator(true);
}

void LobbyHandler::OnClickRadioButtonGameRoom1vs1()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team1vs1;
}

void LobbyHandler::OnClickRadioButtonGameRoom2vs2()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team2vs2;
}

void LobbyHandler::OnClickRadioButtonGameRoom3vs3()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team3vs3;
}

void LobbyHandler::OnClickRadioButtonGameRoom4vs4()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team4vs4;
}

void LobbyHandler::OnClickRadioButtonGameRoom5vs5()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team5vs5;
}

void LobbyHandler::OnClickRadioButtonGameRoom6vs6()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team6vs6;
}

void LobbyHandler::OnClickRadioButtonGameRoom7vs7()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team7vs7;
}

void LobbyHandler::OnClickRadioButtonGameRoom8vs8()
{
	m_createGameRoomTeamFormatSelected = GameRoomTeamFormat::Team8vs8;
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
	Panel* pPanelOkMsgBoxRoot = static_cast<Panel*>(m_hPanelOkMsgBoxRoot.ToPtr());
	pPanelOkMsgBoxRoot->SetActive(false);
	pPanelOkMsgBoxRoot->m_transform.SetParent(&m_hImageLobbyBgr.ToPtr()->m_transform);
}

bool LobbyHandler::FindGameRoomPlayer(uint32_t accountId, GameTeam& team, size_t& index) const
{
	for (size_t i = 0; i < m_gameRoomRedTeamPlayers.size(); ++i)
	{
		if (m_gameRoomRedTeamPlayers[i].m_accountId == accountId)
		{
			team = GameTeam::RedTeam;
			index = i;
			return true;
		}
	}

	for (size_t i = 0; i < m_gameRoomBlueTeamPlayers.size(); ++i)
	{
		if (m_gameRoomBlueTeamPlayers[i].m_accountId == accountId)
		{
			team = GameTeam::BlueTeam;
			index = i;
			return true;
		}
	}

	return false;
}

const GameRoomPlayer* LobbyHandler::FindMyGameRoomPlayer() const
{
	const Account* pScriptAccount = m_hScriptAccount.ToPtr();

	for (size_t i = 0; i < m_gameRoomRedTeamPlayers.size(); ++i)
	{
		if (pScriptAccount->GetAccountId() == m_gameRoomRedTeamPlayers[i].m_accountId)
			return &m_gameRoomRedTeamPlayers[i];
	}

	for (size_t i = 0; i < m_gameRoomBlueTeamPlayers.size(); ++i)
	{
		if (pScriptAccount->GetAccountId() == m_gameRoomBlueTeamPlayers[i].m_accountId)
			return &m_gameRoomBlueTeamPlayers[i];
	}

	return nullptr;
}

void LobbyHandler::SetLobbyState(LobbyState state)
{
	if (m_lobbyState == state)
		return;

	m_lobbyState = state;
	m_needUIUpdate = true;
}

void LobbyHandler::ClearChatMsgs()
{
	for (size_t i = 0; i < _countof(m_hTextLobbyChatMsg); ++i)
		static_cast<Text*>(m_hTextLobbyChatMsg[i].ToPtr())->GetText().clear();

	m_textLobbyChatMsgCount = 0;
}

void LobbyHandler::UpdateGameRoomUI()
{
	wchar_t textBuf[128];

	// 게임 방 타이틀 바 텍스트 ( [방 번호] 방 제목 )
	StringCchPrintfW(textBuf, _countof(textBuf), L"[%u] %s", static_cast<uint32_t>(m_gameRoomNo), m_gameRoomName.c_str());
	static_cast<Text*>(m_hTextGameRoomNamePanel.ToPtr())->SetText(textBuf);

	auto UpdatePlayerText = [this](const GameRoomPlayer& player, Text* pTextGameRoomPlayer)
		{
			wchar_t textBuf[64];

			const wchar_t* prefixStr = m_gameRoomHostAccountId == player.m_accountId ? L"[방장]" : L"";
			const wchar_t* suffixStr = L"";
			switch (player.m_state)
			{
			case PlayerState::None:
				pTextGameRoomPlayer->SetColor(ColorsLinear::LightGray);
				suffixStr = L"";
				break;
			case PlayerState::Ready:
				pTextGameRoomPlayer->SetColor(ColorsLinear::Green);
				suffixStr = L"[준비완료]";
				break;
			case PlayerState::Maintenance:
				pTextGameRoomPlayer->SetColor(ColorsLinear::Orange);
				suffixStr = L"[정비중]";
				break;
			default:
				pTextGameRoomPlayer->SetColor(ColorsLinear::Black);
				suffixStr = L"";
				break;
			}
			StringCchPrintfW(textBuf, _countof(textBuf), L"%s %s %s", prefixStr, player.m_nickname.c_str(), suffixStr);
			pTextGameRoomPlayer->SetText(textBuf);
		};

	assert(m_gameRoomRedTeamPlayers.size() <= MAX_PLAYERS_PER_TEAM);
	assert(m_gameRoomBlueTeamPlayers.size() <= MAX_PLAYERS_PER_TEAM);

	// 플레이어 목록 클리어
	for (size_t i = 0; i < _countof(m_hTextGameRoomRedTeamPlayers); ++i)
	{
		Text* pTextGameRoomPlayer = static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr());
		pTextGameRoomPlayer->GetText().clear();
	}
	for (size_t i = 0; i < _countof(m_hTextGameRoomBlueTeamPlayers); ++i)
	{
		Text* pTextGameRoomPlayer = static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr());
		pTextGameRoomPlayer->GetText().clear();
	}


	for (size_t i = 0; i < m_gameRoomRedTeamPlayers.size() && i < _countof(m_hTextGameRoomRedTeamPlayers); ++i)
	{
		const GameRoomPlayer& player = m_gameRoomRedTeamPlayers[i];
		Text* pTextGameRoomPlayer = static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr());

		UpdatePlayerText(player, pTextGameRoomPlayer);
	}

	for (size_t i = 0; i < m_gameRoomBlueTeamPlayers.size() && i < _countof(m_hTextGameRoomBlueTeamPlayers); ++i)
	{
		const GameRoomPlayer& player = m_gameRoomBlueTeamPlayers[i];
		Text* pTextGameRoomPlayer = static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr());

		UpdatePlayerText(player, pTextGameRoomPlayer);
	}

	// 게임 정보 UI 업데이트
	wchar_t gameInfoBuf[128];
	const uint32_t maxPlayerPerTeam = static_cast<uint32_t>(GameRoomTeamFormatToMaxPlayerCount(m_gameRoomTeamFormat) / 2);
	StringCchPrintfW(gameInfoBuf, _countof(gameInfoBuf), L"맵\t\t%s\n게임 모드\t%s\n최대 인원\t%u vs %u\n게임 시간\t10분",
		GameMapInfo::GetMapNameString(m_gameRoomGameMap),
		GameMapInfo::GetModeNameString(m_gameRoomGameMap),
		maxPlayerPerTeam, maxPlayerPerTeam
	);
	static_cast<Text*>(m_hTextGameRoomInfo.ToPtr())->SetText(gameInfoBuf);

	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	if (m_gameRoomHostAccountId == pScriptAccount->GetAccountId())
	{
		m_hButtonGameReady.ToPtr()->SetActive(false);
		m_hButtonGameUnready.ToPtr()->SetActive(false);
		m_hButtonHostGameStart.ToPtr()->SetActive(true);
	}
	else
	{
		const GameRoomPlayer* pMyPlayer = FindMyGameRoomPlayer();

		if (pMyPlayer->m_state == PlayerState::None)
		{
			m_hButtonGameReady.ToPtr()->SetActive(true);
			m_hButtonGameUnready.ToPtr()->SetActive(false);
		}
		else
		{
			m_hButtonGameReady.ToPtr()->SetActive(false);
			m_hButtonGameUnready.ToPtr()->SetActive(true);
		}

		m_hButtonHostGameStart.ToPtr()->SetActive(false);
	}
}

void LobbyHandler::OnReceiveChannelInfo(const ChannelInfo* pInfos, size_t count)
{
	for (uint8_t channelId = 0; channelId < _countof(m_hTextChannelHeadcount) && channelId < count; ++channelId)
	{
		wchar_t textBuf[32];
		StringCchPrintfW(
			textBuf,
			_countof(textBuf),
			L"%u / %u",
			static_cast<uint32_t>(pInfos[channelId].m_numOfPlayers),
			static_cast<uint32_t>(pInfos[channelId].m_maxPlayerCount)
		);
		static_cast<Text*>(m_hTextChannelHeadcount[channelId].ToPtr())->SetText(textBuf);
	}
}

void LobbyHandler::OnReceiveGameList(uint32_t listContextNo, const std::vector<GameRoomItem>& list)
{
	if (m_currGameListContextNo != listContextNo)	// 이전 방 목록 컨텍스트인 경우 무시
		return;

	for (size_t i = 0; i < list.size(); ++i)
		m_gameRoomList.push_back(list[i]);

	this->UpdateGameListBrowserUI();
}

void LobbyHandler::OnJoinGameRoom(uint32_t hostAccountId, uint16_t roomNo, GameRoomTeamFormat tf, GameMap map, GameTeam team, const wchar_t* gameRoomName)
{
	const Account* pScriptAccount = m_hScriptAccount.ToPtr();

	m_gameRoomHostAccountId = hostAccountId;
	m_gameRoomNo = roomNo;
	m_gameRoomName = gameRoomName;
	m_gameRoomTeamFormat = tf;
	m_gameRoomGameMap = map;
	m_gameRoomMyTeam = team;

	GameRoomPlayer player;
	player.m_accountId = pScriptAccount->GetAccountId();
	player.m_level = pScriptAccount->GetLevel();
	player.m_nickname = pScriptAccount->GetNickname();
	player.m_state = PlayerState::None;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers.push_back(player);
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers.push_back(player);
		break;
	default:
		assert(false);
		break;
	}

	UpdateGameRoomUI();

	SetLobbyState(LobbyState::GameRoom);
}

void LobbyHandler::OnPlayerJoinGameRoom(uint32_t accountId, uint16_t level, wchar_t* nickname, PlayerState state, GameTeam team)
{
	GameRoomPlayer player;
	player.m_accountId = accountId;
	player.m_level = level;
	player.m_nickname = nickname;
	player.m_state = state;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers.push_back(player);
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers.push_back(player);
		break;
	default:
		assert(false);
		break;
	}

	UpdateGameRoomUI();
}

void LobbyHandler::OnPlayerTeamChanged(uint32_t accountId, GameTeam newTeam)
{
	GameTeam team;
	size_t index;
	if (!FindGameRoomPlayer(accountId, team, index))
		return;

	GameRoomPlayer backup;
	if (team == GameTeam::RedTeam)
		backup = m_gameRoomRedTeamPlayers[index];
	else
		backup = m_gameRoomBlueTeamPlayers[index];

	if (team == GameTeam::RedTeam)
		m_gameRoomRedTeamPlayers.erase(m_gameRoomRedTeamPlayers.begin() + index);
	else
		m_gameRoomBlueTeamPlayers.erase(m_gameRoomBlueTeamPlayers.begin() + index);

	switch (newTeam)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers.push_back(backup);
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers.push_back(backup);
		break;
	case GameTeam::Unknown:
		*reinterpret_cast<int*>(0) = 0;
		break;
	}

	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	if (pScriptAccount->GetAccountId() == accountId)
		m_gameRoomMyTeam = newTeam;

	UpdateGameRoomUI();
}

void LobbyHandler::OnExitGameRoom()
{
	// 게임 방 정보 & UI 클리어
	m_gameRoomHostAccountId = 0;
	m_gameRoomNo = 0;
	m_gameRoomName.clear();
	m_gameRoomTeamFormat = GameRoomTeamFormat::Unknown;
	m_gameRoomGameMap = GameMap::Unknown;
	m_gameRoomMyTeam = GameTeam::Unknown;
	m_gameRoomRedTeamPlayers.clear();
	m_gameRoomBlueTeamPlayers.clear();

	static_cast<Text*>(m_hTextGameRoomNamePanel.ToPtr())->GetText().clear();

	for (size_t i = 0; i < _countof(m_hTextGameRoomRedTeamPlayers); ++i)
		static_cast<Text*>(m_hTextGameRoomRedTeamPlayers[i].ToPtr())->GetText().clear();

	for (size_t i = 0; i < _countof(m_hTextGameRoomBlueTeamPlayers); ++i)
		static_cast<Text*>(m_hTextGameRoomBlueTeamPlayers[i].ToPtr())->GetText().clear();

	// 게임 리스트 UI로 전환
	this->SetLobbyState(LobbyState::GameListBrowser);
	this->ShowSelectedGameRoomIndicator(true);
}

void LobbyHandler::OnPlayerExitGameRoom(uint32_t accountId)
{
	GameTeam team;
	size_t index;

	if (!FindGameRoomPlayer(accountId, team, index))
		return;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers.erase(m_gameRoomRedTeamPlayers.begin() + index);
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers.erase(m_gameRoomBlueTeamPlayers.begin() + index);
		break;
	default:
		break;
	}

	UpdateGameRoomUI();
}

void LobbyHandler::OnGameRoomHostChanged(uint32_t newHostAccountId)
{
	const uint32_t oldHostAccountId = m_gameRoomHostAccountId;
	m_gameRoomHostAccountId = newHostAccountId;

	const Account* pScriptAccount = m_hScriptAccount.ToPtr();
	if (oldHostAccountId == pScriptAccount->GetAccountId() && newHostAccountId != pScriptAccount->GetAccountId())
	{
		m_hButtonGameReady.ToPtr()->SetActive(true);
		m_hButtonGameUnready.ToPtr()->SetActive(false);
		m_hButtonHostGameStart.ToPtr()->SetActive(false);
	}

	if (newHostAccountId == pScriptAccount->GetAccountId())
	{
		m_hButtonGameReady.ToPtr()->SetActive(false);
		m_hButtonGameUnready.ToPtr()->SetActive(false);
		m_hButtonHostGameStart.ToPtr()->SetActive(true);
	}

	GameTeam oldHostTeam;
	size_t oldHostIndex;
	if (!FindGameRoomPlayer(oldHostAccountId, oldHostTeam, oldHostIndex))
		return;

	switch (oldHostTeam)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers[oldHostIndex].m_state = PlayerState::None;
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers[oldHostIndex].m_state = PlayerState::None;
		break;
	default:
		break;
	}

	GameTeam newHostTeam;
	size_t newHostIndex;
	if (!FindGameRoomPlayer(newHostAccountId, newHostTeam, newHostIndex))
		return;

	switch (newHostTeam)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers[newHostIndex].m_state = PlayerState::None;
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers[newHostIndex].m_state = PlayerState::None;
		break;
	default:
		break;
	}

	UpdateGameRoomUI();
}

void LobbyHandler::OnGameRoomPlayerStateChanged(uint32_t accountId, PlayerState state)
{
	const Account* pScriptAccount = m_hScriptAccount.ToPtr();

	if (accountId == pScriptAccount->GetAccountId())
	{
		if (state == PlayerState::Ready)
		{
			m_hButtonGameReady.ToPtr()->SetActive(false);
			m_hButtonGameUnready.ToPtr()->SetActive(true);
		}
		else if (state == PlayerState::None)
		{
			m_hButtonGameReady.ToPtr()->SetActive(true);
			m_hButtonGameUnready.ToPtr()->SetActive(false);
		}
	}

	GameTeam team;
	size_t index;
	if (!FindGameRoomPlayer(accountId, team, index))
		return;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_gameRoomRedTeamPlayers[index].m_state = state;
		break;
	case GameTeam::BlueTeam:
		m_gameRoomBlueTeamPlayers[index].m_state = state;
		break;
	default:
		break;
	}

	UpdateGameRoomUI();
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
	const size_t pageCount = GetNumOfGameListPages();

	wchar_t buf[16];
	StringCchPrintfW(buf, _countof(buf), L"%u / %u", static_cast<uint32_t>(m_currGameListPage + 1), static_cast<uint32_t>(pageCount));
	static_cast<Text*>(m_hTextGameListPage.ToPtr())->SetText(buf);

	ClearAllGameListItem();

	if (m_gameRoomList.size() > 0)
	{
		size_t firstItemIndex = m_currGameListPage * MAX_GAMES_PER_LIST_PAGE;
		size_t lastItemIndex = (std::min)(firstItemIndex + MAX_GAMES_PER_LIST_PAGE - 1, m_gameRoomList.size() - 1);

		size_t currPageItemCount = lastItemIndex - firstItemIndex + 1;
		assert(currPageItemCount <= MAX_GAMES_PER_LIST_PAGE);

		for (size_t i = 0; i < currPageItemCount; ++i)
		{
			const size_t index = firstItemIndex + i;
			const GameRoomItem& item = m_gameRoomList[index];
			wchar_t buf[128];

			StringCchPrintfW(buf, _countof(buf), L"%u", static_cast<unsigned int>(item.m_no));
			static_cast<Text*>(m_hTextGameNo[i].ToPtr())->SetText(buf);

			static_cast<Text*>(m_hTextGameName[i].ToPtr())->SetText(item.m_name);

			static_cast<Text*>(m_hTextGameMap[i].ToPtr())->SetText(GameMapInfo::GetMapNameString(item.m_map));

			StringCchPrintfW(buf, _countof(buf), L"%u/%u", static_cast<uint32_t>(item.m_numOfPlayers), static_cast<uint32_t>(GameRoomTeamFormatToMaxPlayerCount(item.m_tf)));
			static_cast<Text*>(m_hTextGameHeadcount[i].ToPtr())->SetText(buf);

			static_cast<Text*>(m_hTextGameMode[i].ToPtr())->SetText(GameMapInfo::GetModeNameString(item.m_map));

			Text* pTextGameState = static_cast<Text*>(m_hTextGameState[i].ToPtr());
			pTextGameState->SetText(GameRoomStateInfo::GetGameRoomStateString(item.m_state));
			switch (item.m_state)
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
		m_hPanelCreateAccountWindowRoot.ToPtr()->SetActive(false);
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
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->GetTexture2D(L"login_bgr"));
		m_hImageLobbyBgr.ToPtr()->SetActive(true);

		// 로그인 UI 표시
		m_hPanelLoginWindowRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::ChannelListBrowser:
		m_hPanelOkMsgBoxRoot.ToPtr()->SetActive(false);
		m_hPanelLoginWindowRoot.ToPtr()->SetActive(false);
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
		m_hPanelLoginWindowRoot.ToPtr()->SetActive(false);
		m_hPanelChannelBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelGameRoomRoot.ToPtr()->SetActive(false);
		m_hPanelCreateGameRoomRoot.ToPtr()->SetActive(false);

		// #########################################################
		// 현재 상태에 대응하는 UI 보이기
		
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->GetTexture2D(L"gamelist_bgr"));
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
		m_hPanelLoginWindowRoot.ToPtr()->SetActive(false);
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

size_t LobbyHandler::GetNumOfGameListPages() const
{
	const size_t itemCount = m_gameRoomList.size();

	if (itemCount > 0)
		return (itemCount - 1) / MAX_GAMES_PER_LIST_PAGE + 1;
	else
		return 1;
}

void LobbyHandler::ClearAllGameListItem()
{
	for (size_t i = 0; i < MAX_GAMES_PER_LIST_PAGE; ++i)
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

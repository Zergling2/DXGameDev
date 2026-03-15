#include "LobbyHandler.h"
#include "Protocol.h"
#include "GameResources.h"
#include "Network.h"
#include <openssl/sha.h>

using namespace ze;

LobbyHandler::LobbyHandler(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_needUIUpdate(false)
	, m_lobbyState(LobbyState::None)
	, m_textLobbyChatMsgCount(0)
{
}

void LobbyHandler::Awake()
{
	// SetResolution(1600, 900, DisplayMode::Windowed);
	SetResolution(0, 0, DisplayMode::BorderlessWindowed);
}

void LobbyHandler::Start()
{
	this->SetLobbyState(LobbyState::Login);
}

void LobbyHandler::Update()
{
	if (m_needUIUpdate)
	{
		this->UpdateUI();
		m_needUIUpdate = false;
	}

	if (m_lobbyState == LobbyState::GameListBrowser)	// 방 목록 탐색중인 경우 선택 게임 행 강조 표시
	{
		bool selected = false;
		for (size_t i = 0; i < MAX_GAME_PER_LIST_PAGE; ++i)
		{
			const POINT mp = Input::GetInstance()->GetMousePosition();
			const IUIObject* pUIObject = m_hButtonEnterGame[i].ToPtr();
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
	// if (pScriptNetwork->GetClient().GetState() != winppy::ClientState::Connected)
	// 	return;

	winppy::Packet outPacket;
	CSReqLogin req;
	wmemcpy_s(req.m_id, _countof(req.m_id), pInputFieldId->GetText().c_str(), pInputFieldId->GetText().length());
	req.m_idLen = static_cast<uint16_t>(pInputFieldId->GetText().length());
	memcpy(req.m_hpw, hpw, sizeof(req.m_hpw));
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_LOGIN));
	outPacket->WriteBytes(&req, sizeof(req));
	pScriptNetwork->GetClient().Send(std::move(outPacket));
}

void LobbyHandler::OnClickExitGame()
{
	Runtime::GetInstance()->Exit();
}

void LobbyHandler::OnClickSendChatMsg()
{
	InputField* pInputFieldChatMsg = static_cast<InputField*>(m_hInputFieldChatMsg.ToPtr());
	if (pInputFieldChatMsg->GetText().length() == 0)
		return;

	Network* pScriptNetwork = m_hScriptNetwork.ToPtr();
	CSReqBroadcastChatMsg req;
	req.m_chatMsgLen = static_cast<uint16_t>(pInputFieldChatMsg->GetText().length());
	assert(req.m_chatMsgLen <= MAX_CHAT_MSG_LEN);

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::CS_REQ_BROADCAST_CHAT_MSG));
	outPacket->WriteBytes(&req, sizeof(req));
	outPacket->WriteBytes(pInputFieldChatMsg->GetText().c_str(), sizeof(wchar_t) * req.m_chatMsgLen);

	pInputFieldChatMsg->GetText().clear();	// 채팅 입력 클리어

	pScriptNetwork->GetClient().Send(std::move(outPacket));
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

void LobbyHandler::UpdateUI()
{
	switch (m_lobbyState)
	{
	case LobbyState::Login:
		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelGameBrowserRoot.ToPtr()->SetActive(false);
		m_hPanelChat.ToPtr()->SetActive(false);
		m_hButtonOpenShop.ToPtr()->SetActive(false);
		m_hButtonUserInfo.ToPtr()->SetActive(false);
		// #########################################################
		
		// 현재 상태에 대응하는 UI 보이기
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texLoginBgr);
		m_hImageLobbyBgr.ToPtr()->SetActive(true);

		// 로그인 UI 표시
		m_hPanelLoginUIRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::GameListBrowser:
		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);

		// #########################################################
		// 현재 상태에 대응하는 UI 보이기
		
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texGameListBgr);
		m_hImageLobbyBgr.ToPtr()->SetActive(true);

		m_hPanelChat.ToPtr()->SetActive(true);
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);

		// 게임 리스트 브라우저 UI 표시
		m_hPanelGameBrowserRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::GameRoom:
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);
		m_hPanelGameBrowserRoot.ToPtr()->SetActive(false);

		// 상점/정비 버튼 표시
		m_hPanelChat.ToPtr()->SetActive(true);
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);
		break;
	case LobbyState::Scoreboard:

		break;
	default:
		break;
	}
}

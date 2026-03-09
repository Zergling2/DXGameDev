#include "LobbyHandler.h"

using namespace ze;

LobbyHandler::LobbyHandler(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_connectionWithServer(false)
	, m_uiUpdateReq(false)
	, m_lobbyState(LobbyState::Login)
{
}

void LobbyHandler::Awake()
{
	SetResolution(0, 0, DisplayMode::BorderlessWindowed);
}

void LobbyHandler::Update()
{
	if (m_uiUpdateReq)
	{
		this->UpdateUI();
		m_uiUpdateReq = false;
	}

	if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_ESCAPE))
		Runtime::GetInstance()->Exit();
}

void LobbyHandler::OnClickLogin()
{
	InputField* pInputFieldId = static_cast<InputField*>(m_hInputFieldId.ToPtr());
	const wchar_t* pId = pInputFieldId->GetText().c_str();

	InputField* pInputFieldPw = static_cast<InputField*>(m_hInputFieldPw.ToPtr());
	const wchar_t* pPw = pInputFieldPw->GetText().c_str();

	// 서버로 로그인 요청 전송
	
	
	// 로그인 창 UI 숨기기
	m_hPanelLoginUIRoot.ToPtr()->SetActive(false);
	// static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetActive(false);
	static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texGameListBgr);
}

void LobbyHandler::OnClickExitGame()
{
	Runtime::GetInstance()->Exit();
}

void LobbyHandler::UpdateUI()
{
	switch (m_lobbyState)
	{
	case LobbyState::Login:
		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(false);
		// #########################################################
		
		// 현재 상태에 대응하는 UI 보이기
		// 배경 이미지 교체
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texLoginBgr);

		m_hImageLobbyBgr.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::GameListBrowser:
		// #########################################################
		// 현재 상태에 대응하지 않는 UI 숨기기
		m_hPanelLoginUIRoot.ToPtr()->SetActive(false);

		// #########################################################
		// 현재 상태에 대응하는 UI 보이기
		static_cast<Image*>(m_hImageLobbyBgr.ToPtr())->SetTexture(m_hScriptGameResources.ToPtr()->m_texGameListBgr);
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::GameRoom:
		break;
	case LobbyState::Scoreboard:
		break;
	default:
		break;
	}
}

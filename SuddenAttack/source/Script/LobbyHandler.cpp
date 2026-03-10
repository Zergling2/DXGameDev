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

	if (m_lobbyState == LobbyState::GameListBrowser)
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
	const wchar_t* pId = pInputFieldId->GetText().c_str();

	InputField* pInputFieldPw = static_cast<InputField*>(m_hInputFieldPw.ToPtr());
	const wchar_t* pPw = pInputFieldPw->GetText().c_str();

	// 서버로 로그인 요청 전송
	
	
	// 로그인 창 UI 숨기기
	this->SetLobbyState(LobbyState::GameListBrowser);
	this->RequestUIUpdate();
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

		// 상점/정비 버튼 표시
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

		// 상점/정비 버튼 표시
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);

		// 게임 리스트 브라우저 UI 표시
		m_hPanelGameListBrowserRoot.ToPtr()->SetActive(true);
		// #########################################################
		break;
	case LobbyState::GameRoom:
		// 상점/정비 버튼 표시
		m_hButtonOpenShop.ToPtr()->SetActive(true);
		m_hButtonUserInfo.ToPtr()->SetActive(true);
		break;
	case LobbyState::Scoreboard:

		break;
	default:
		break;
	}
}

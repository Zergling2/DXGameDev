#include "LobbyHandler.h"

using namespace ze;

LobbyHandler::LobbyHandler(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_connectionWithServer(false)
{
}

void LobbyHandler::Awake()
{
	// 로그인 서버와 연결 코드

}

void LobbyHandler::OnClickLogin()
{
	InputField* pInputFieldId = static_cast<InputField*>(m_hInputFieldId.ToPtr());
	const wchar_t* pId = pInputFieldId->GetText().c_str();

	InputField* pInputFieldPw = static_cast<InputField*>(m_hInputFieldPw.ToPtr());
	const wchar_t* pPw = pInputFieldPw->GetText().c_str();

	// 서버로 로그인 요청 전송



	m_hImageLoginBackground.ToPtr()->SetActive(false);
}

void LobbyHandler::OnClickExitGame()
{
	Runtime::GetInstance()->Exit();
}

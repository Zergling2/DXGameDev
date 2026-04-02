#include "Account.h"

Account::Account(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_accountId(0)
	, m_level(0)
	, m_exp(0)
	, m_point(0)
	, m_nicknameLen(0)
	, m_nickname{ L'\0' }
{
}

void Account::SetNickname(const wchar_t* nickname, uint16_t len)
{
	assert(len <= MAX_NICKNAME_LEN);

	m_nicknameLen = len;
	wmemcpy_s(m_nickname, _countof(m_nickname), nickname, len);
	m_nickname[len] = L'\0';
}

#include "Account.h"

Account::Account(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_level(0)
	, m_exp(0)
	, m_point(0)
{
}

#include "SASession.h"
#include <strsafe.h>
#include <cstdlib>

void SASession::SetNickname(const wchar_t* nickname)
{
	StringCchCopyW(m_nickname, _countof(m_nickname), nickname);
}

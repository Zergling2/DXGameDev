#include "GameSession.h"
#include <cstdlib>

GameSession::GameSession(uint64_t netId, uint64_t accountId)
	: m_netId(netId)
	, m_accountId(accountId)
	, m_nicknameLen(0)
	, m_nickname{ L'\0' }
	, m_level(0)
	, m_exp(0)
	, m_point(0)
	, m_channelLock()
	, m_disconnected(false)
	, m_pJoiningGameChannel(nullptr)
	, m_wpJoiningGameRoom()
{
}

void GameSession::SetNickname(const wchar_t* nickname, uint16_t len)
{
	m_nicknameLen = len;
	wmemcpy_s(m_nickname, _countof(m_nickname), nickname, len);
	m_nickname[len] = L'\0';
}

void GameSession::SetDisconnectedFlag()
{
	AutoAcquireSlimRWLockExclusive lock(m_channelLock);
	m_disconnected = true;
}

GameChannel* GameSession::GetJoiningGameChannel()
{
	m_channelLock.AcquireLockShared();
	GameChannel* pChannel = m_pJoiningGameChannel;
	m_channelLock.ReleaseLockShared();

	return pChannel;
}

bool GameSession::SetJoiningGameChannel(GameChannel* pChannel)
{
	AutoAcquireSlimRWLockExclusive lock(m_channelLock);

	if (m_disconnected)
		return false;

	m_pJoiningGameChannel = pChannel;
		
	return true;
}

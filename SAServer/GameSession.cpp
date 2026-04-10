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
	, m_team(GameTeam::Unknown)
	, m_teamIndex((std::numeric_limits<uint8_t>::max)())
	, m_ready(false)
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

bool GameSession::GetDisconnectedFlag()
{
	AutoAcquireSlimRWLockShared lock(m_channelLock);

	return m_disconnected;
}

GameChannel* GameSession::GetJoiningGameChannel()
{
	AutoAcquireSlimRWLockShared lock(m_channelLock);

	return m_pJoiningGameChannel;
}

void GameSession::SetJoiningGameChannel(GameChannel* pChannel)
{
	AutoAcquireSlimRWLockExclusive lock(m_channelLock);

	m_pJoiningGameChannel = pChannel;
}

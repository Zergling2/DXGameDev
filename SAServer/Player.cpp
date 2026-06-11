#include "Player.h"
#include <cstdlib>
#include <cassert>
#include <strsafe.h>

Player::Player(uint32_t accountId, uint16_t level, uint32_t point, const wchar_t* nickname)
    : m_pSession(nullptr)
    , m_accountId(accountId)
    , m_inChannel(false)
    , m_channelId((std::numeric_limits<uint8_t>::max)())
    , m_inRoom(false)
    , m_roomId((std::numeric_limits<uint64_t>::max)())
    , m_nicknameLen(0)
    , m_nickname{}
    , m_level(level)
    , m_point(point)
{
    this->SetNickname(nickname);
}

void Player::SetNickname(const wchar_t* nickname)
{
    const uint16_t nicknameLen = static_cast<uint16_t>(wcslen(nickname));
	assert(nicknameLen <= MAX_NICKNAME_LEN);

	m_nicknameLen = nicknameLen;
	StringCchCopyW(m_nickname, _countof(m_nickname), nickname);
}

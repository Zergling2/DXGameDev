#pragma once

#include <cstdint>
#include "Constants.h"
#include "Contents.h"

struct GamePlayerInfo
{
	GamePlayerInfo(uint32_t accountId)
		: m_accountId(accountId)
		, m_nicknameLen(0)
		, m_nickname{}
		, m_team(GameTeam::Unknown)
		, m_level(0)
		, m_kill(0)
		, m_death(0)
		, m_ping(0)
		, m_state(InGamePlayerState::Unknown)
	{
	}
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];	// null termination string
	GameTeam m_team;
	uint32_t m_level;
	uint32_t m_kill;
	uint32_t m_death;
	uint32_t m_ping;
	InGamePlayerState m_state;
};

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
		, m_weaponCodes{ WeaponCode::Unknown, WeaponCode::Unknown }
		, m_currWeapon(WeaponSlot::Unknown)
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
	WeaponCode m_weaponCodes[static_cast<size_t>(WeaponSlot::Count)];
	WeaponSlot m_currWeapon;	// 게임 참가 시 플레이어가 최초 들고있는 무기 뷰 동기화에 필요.
};

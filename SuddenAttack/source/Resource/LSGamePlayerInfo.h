#pragma once

#include "GamePlayerInfo.h"

struct LSGamePlayerInfo : public GamePlayerInfo
{
public:
	LSGamePlayerInfo(uint32_t accountId)
		: GamePlayerInfo(accountId)
		, m_respawnRemainingTime(0.0f)
		, m_hp(0)
		, m_ap(0)
	{
	}
	float m_respawnRemainingTime;
	uint16_t m_hp;
	uint16_t m_ap;
};

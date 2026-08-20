#pragma once

#include "GamePlayerInfo.h"

struct LSGamePlayerInfo : public GamePlayerInfo
{
public:
	LSGamePlayerInfo(uint32_t accountId)
		: GamePlayerInfo(accountId)
		, m_respawnRemainingTime(0.0f)
	{
	}
	float m_respawnRemainingTime;
};

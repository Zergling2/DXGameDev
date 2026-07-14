#pragma once

#include <cstddef>
#include <cstdint>

enum class GameMap : uint16_t
{
	TeamDeathmatchMapBegin = 0,
	tdm_warehouse = TeamDeathmatchMapBegin,
	TeamDeathmatchMapEnd = 4999,

	DemolitionMapBegin = 5000,
	de_a_supply_base = DemolitionMapBegin,
	DemolitionMapEnd = 9999,

	DominationMapBegin = 10000,
	dom_stormveil = DominationMapBegin,
	DominationMapEnd = 14999,

	Unknown = 65535
};

enum class GameRoomState : uint8_t
{
	InWaiting,
	InPlay
};

enum class PlayerState : uint8_t
{
	None,
	Ready,
	Maintenance,
	Playing,

	Unknown
};

enum class GameRoomTeamFormat : uint8_t
{
	Team1vs1 = 0,
	Team2vs2,
	Team3vs3,
	Team4vs4,
	Team5vs5,
	Team6vs6,
	Team7vs7,
	Team8vs8,

	Count,
	Unknown
};

inline size_t GameRoomTeamFormatToMaxPlayerCount(GameRoomTeamFormat tf)
{
	return (static_cast<size_t>(tf) + 1) * 2;
}

enum class GameTeam : uint8_t
{
	RedTeam,
	BlueTeam,

	Unknown
};

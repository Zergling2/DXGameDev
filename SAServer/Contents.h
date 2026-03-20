#pragma once

#include <cstdint>

enum class GameMode : uint8_t
{
	TeamDeathmatch,
	Demolition,
	Domination
};

enum class GameMap : uint8_t
{
	Warehouse,
	ASupplyBase
};

enum class GameRoomState : uint8_t
{
	InWaiting,
	InPlay
};

enum class GameRoomMaxPlayer : uint8_t
{
	Game1vs1,
	Game2vs2,
	Game3vs3,
	Game4vs4,
	Game5vs5,
	Game6vs6,
	Game7vs7,
	Game8vs8
};

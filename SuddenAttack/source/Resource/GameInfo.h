#pragma once

#include "Contents.h"

class GameMapInfo
{
public:
	static const wchar_t* GetGameMapString(GameMap type);
};

class GameModeInfo
{
public:
	static const wchar_t* GetGameModeString(GameMode type);
};

class GameRoomStateInfo
{
public:
	static const wchar_t* GetGameRoomStateString(GameRoomState type);
};

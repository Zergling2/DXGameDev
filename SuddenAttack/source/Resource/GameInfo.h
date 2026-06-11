#pragma once

#include "Contents.h"

class GameMapInfo
{
public:
	static const wchar_t* GetMapNameString(GameMap map);
	static const wchar_t* GetModeNameString(GameMap map);
};

class GameRoomStateInfo
{
public:
	static const wchar_t* GetGameRoomStateString(GameRoomState type);
};



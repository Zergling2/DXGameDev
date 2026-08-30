#pragma once

#include "Contents.h"
#include <unordered_map>
#include "HitboxPart.h"

class GameMapInfo
{
public:
	static const wchar_t* GetMapNameString(GameMap map);
	static const wchar_t* GetMapSceneNameString(GameMap map);
	static const wchar_t* GetModeNameString(GameMap map);
};

class WeaponInfo
{
public:
	static const wchar_t* GetWeaponNameString(WeaponCode code);
	static uint16_t GetWeaponDamage(WeaponCode code, HitboxPart hitPart);
};

class GameRoomStateInfo
{
public:
	static const wchar_t* GetGameRoomStateString(GameRoomState type);
};

class GameSettings
{
public:
	static constexpr float GetRespawnTime() { return 5.0f; }
};

#include "GameInfo.h"
#include <cassert>

const wchar_t* GameMapInfo::GetGameMapString(GameMap type)
{
	switch (type)
	{
	case GameMap::Warehouse:
		return L"웨어하우스";
	case GameMap::ASupplyBase:
		return L"A 보급창고";
	default:
		assert(false);
		return L"Unknown";
	}
}

const wchar_t* GameModeInfo::GetGameModeString(GameMode type)
{
	switch (type)
	{
	case GameMode::TeamDeathmatch:
		return L"팀데스매치";
	case GameMode::Demolition:
		return L"폭파미션";
	case GameMode::Domination:
		return L"점령전";
	default:
		assert(false);
		return L"Unknown";
	}
}

const wchar_t* GameRoomStateInfo::GetGameRoomStateString(GameRoomState type)
{
	switch (type)
	{
	case GameRoomState::InWaiting:
		return L"대기중";
	case GameRoomState::InPlay:
		return L"게임중";
	default:
		assert(false);
		return L"Unknown";
	}
}

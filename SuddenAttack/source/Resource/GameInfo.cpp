#include "GameInfo.h"
#include <cassert>

const wchar_t* GameMapInfo::GetMapNameString(GameMap map)
{
	switch (map)
	{
	case GameMap::tdm_warehouse:
		return L"웨어하우스";
	case GameMap::de_a_supply_base:
		return L"A 보급창고";
	default:
		assert(false);
		return L"Unknown";
	}
}

const wchar_t* GameMapInfo::GetModeNameString(GameMap map)
{
	const uint32_t mapId = static_cast<uint32_t>(map);

	if (static_cast<uint32_t>(GameMap::TeamDeathmatchMapBegin) <= mapId && mapId <= static_cast<uint32_t>(GameMap::TeamDeathmatchMapEnd))
		return L"팀데스매치";
	else if (static_cast<uint32_t>(GameMap::DemolitionMapBegin) <= mapId && mapId <= static_cast<uint32_t>(GameMap::DemolitionMapEnd))
		return L"폭파미션";
	else if (static_cast<uint32_t>(GameMap::DominationMapBegin) <= mapId && mapId <= static_cast<uint32_t>(GameMap::DominationMapEnd))
		return L"점령전";
	else
		return L"Unknown";
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

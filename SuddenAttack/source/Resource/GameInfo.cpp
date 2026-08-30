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

const wchar_t* GameMapInfo::GetMapSceneNameString(GameMap map)
{
	switch (map)
	{
	case GameMap::tdm_warehouse:
		return L"Warehouse";
	case GameMap::de_a_supply_base:
		return L"ASupplyBase";
	default:
		assert(false);
		return L"Unknown";
	}
}

const wchar_t* GameMapInfo::GetModeNameString(GameMap map)
{
	if (GameMap::TeamDeathmatchMapBegin <= map && map <= GameMap::TeamDeathmatchMapEnd)
		return L"팀데스매치";
	else if (GameMap::DemolitionMapBegin <= map && map <= GameMap::DemolitionMapEnd)
		return L"폭파미션";
	else if (GameMap::DominationMapBegin <= map && map <= GameMap::DominationMapEnd)
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

const wchar_t* WeaponInfo::GetWeaponNameString(WeaponCode code)
{
	switch (code)
	{
	case WeaponCode::M16:
		return L"M16";
	case WeaponCode::M4A1:
		return L"M4A1 Carbine";
	case WeaponCode::USP:
		return L"C.USP";
	case WeaponCode::B92FSBlack:
		return L"B.92Fs Black";
	default:
		wprintf(L"Can't find name of weapon code: %zu\n", static_cast<size_t>(code));
		return L"Unknown";
	}
}

uint16_t WeaponInfo::GetWeaponDamage(WeaponCode code, HitboxPart hitPart)
{
	int dmg;
	switch (code)
	{
	case WeaponCode::M16:
		dmg = 28;
		break;
	case WeaponCode::M4A1:
		dmg = 28;
		break;
	case WeaponCode::USP:
		dmg = 19;
		break;
	case WeaponCode::B92FSBlack:
		dmg = 19;
		break;
	default:
		wprintf(L"Can't find damage of weapon code: %zu\n", static_cast<size_t>(code));
		dmg = 0;
		break;
	}

	float factor;
	switch (hitPart)
	{
	case HitboxPart::Body:
		factor = 1.0f;
		break;
	case HitboxPart::Neck:
		factor = 2.0f;
		break;
	case HitboxPart::Head:
		factor = 4.0f;
		break;
	case HitboxPart::UpperArm:
		factor = 0.75f;
		break;
	case HitboxPart::ForeArm:
		factor = 0.75f;
		break;
	case HitboxPart::Thigh:
		factor = 0.75f;
		break;
	case HitboxPart::Calf:
		factor = 0.75f;
		break;
	case HitboxPart::Foot:
		factor = 0.5f;
		break;
	default:
		factor = 1.0f;
		break;
	}

	int finalDamage = static_cast<int>(dmg * factor);
	return finalDamage;
}

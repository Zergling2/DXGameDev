#pragma once

#include <ZergEngine\ZergEngine.h>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "WeaponEvent.h"
#include "WeaponAction.h"
#include "WeaponCode.h"

enum class WeaponType
{
	Rifle,
	SniperRifle,
	Shotgun,
	Pistol,
	Melee,
	Utility,
	Etc,

	Unknown
};

class WeaponEventTable
{
public:
	void AddEvent(float time, WeaponEvent event);
public:
	std::vector<std::pair<float, WeaponEvent>> m_events;
};

class WeaponDefinition
{
public:
	WeaponDefinition(
		WeaponCode code,
		const wchar_t* name,
		int32_t magCapacity,
		int32_t initAuxAmmo,
		int32_t damage,
		float rpm,
		float drawTime,
		float reloadTime,
		float recoilTime
	)
		: m_code(code)
		, m_name(name)
		, m_magCapacity(magCapacity)
		, m_initAuxAmmo(initAuxAmmo)
		, m_damage(damage)
		, m_rpm(rpm)
		, m_fireRate(60.0f / rpm)
		, m_drawTime(drawTime)
		, m_reloadTime(reloadTime)
		, m_recoilTime(recoilTime)
		, m_eventTables()
		, m_spPVMesh()
		, m_spPVArmature()
		, m_spTVMesh()
		, m_materials()
		, m_previewImage()
		, m_actionAnims()
		, m_weaponSounds()
	{
	}

	WeaponCode GetCode() const { return m_code; }
	const std::wstring& GetName() const { return m_name; }
	int32_t GetMagCapacity() const { return m_magCapacity; }
	int32_t GetInitAuxAmmo() const { return m_initAuxAmmo; }
	int32_t GetDamage() const { return m_damage; }
	float GetRPM() const { return m_rpm; }
	float GetFireRate() const { return m_fireRate; }
	float GetDrawTime() const { return m_drawTime; }
	float GetReloadTime() const { return m_reloadTime; }
	float GetRecoilTime() const { return m_recoilTime; }
private:
	const WeaponCode m_code;
	const std::wstring m_name;
	const int32_t m_magCapacity;
	const int32_t m_initAuxAmmo;
	const int32_t m_damage;
	const float m_rpm;
	const float m_fireRate;
	const float m_drawTime;
	const float m_reloadTime;
	const float m_recoilTime;
public:
	std::unordered_map<WeaponAction, std::shared_ptr<WeaponEventTable>> m_eventTables;
	std::shared_ptr<ze::SkinnedMesh> m_spPVMesh;
	std::shared_ptr<ze::Armature> m_spPVArmature;
	std::shared_ptr<ze::StaticMesh> m_spTVMesh;
	std::vector<std::shared_ptr<ze::Material>> m_materials;
	ze::Texture2D m_previewImage;
	std::unordered_map<WeaponAction, std::pair<std::string, std::string>> m_actionAnims;	// pair(weapon anim name, arms anim name)
	std::unordered_map<WeaponEvent, std::shared_ptr<ze::AudioClip>> m_weaponSounds;
};

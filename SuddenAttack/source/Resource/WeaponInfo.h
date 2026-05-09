#pragma once

#include <ZergEngine\ZergEngine.h>

class WeaponViewInfo
{
public:
	WeaponViewInfo(
		std::string drawAnimName, std::string armsDrawAnimName, float drawAnimTime,
		std::string reloadAnimName, std::string armsReloadAnimName, float reloadAnimTime,
		std::string fireAnimName, std::string armsFireAnimName, float fireAnimTime,
		std::string runAnimName, std::string armsRunAnimName, float runAnimTime,
		std::string idleAnimName, std::string armsIdleAnimName, float idleAnimTime,
		std::shared_ptr<ze::SkinnedMesh> spPVMesh, std::shared_ptr<ze::Armature> spPVMeshArmature,
		std::shared_ptr<ze::StaticMesh> spTVMesh, std::vector<std::shared_ptr<ze::Material>> materials,
		ze::Texture2D previewImage
	)
		: m_drawAnimName(std::move(drawAnimName))
		, m_armsDrawAnimName(std::move(armsDrawAnimName))
		, m_drawAnimTime(drawAnimTime)
		, m_reloadAnimName(std::move(reloadAnimName))
		, m_armsReloadAnimName(std::move(armsReloadAnimName))
		, m_reloadAnimTime(reloadAnimTime)
		, m_fireAnimName(std::move(fireAnimName))
		, m_armsFireAnimName(std::move(armsFireAnimName))
		, m_fireAnimTime(fireAnimTime)
		, m_runAnimName(std::move(runAnimName))
		, m_armsRunAnimName(std::move(armsRunAnimName))
		, m_runAnimTime(runAnimTime)
		, m_idleAnimName(std::move(idleAnimName))
		, m_armsIdleAnimName(std::move(armsIdleAnimName))
		, m_idleAnimTime(idleAnimTime)
		, m_spPVMesh(std::move(spPVMesh))
		, m_spPVMeshArmature(std::move(spPVMeshArmature))
		, m_spTVMesh(std::move(spTVMesh))
		, m_materials(std::move(materials))
		, m_previewImage(std::move(previewImage))
	{
	}
	virtual ~WeaponViewInfo() = default;

	const char* GetDrawAnimName() const { return m_drawAnimName.c_str(); }
	const char* GetArmsDrawAnimName() const { return m_armsDrawAnimName.c_str(); }
	float GetDrawAnimTime() const { return m_drawAnimTime; }
	const char* GetReloadAnimName() const { return m_reloadAnimName.c_str(); }
	const char* GetArmsReloadAnimName() const { return m_armsReloadAnimName.c_str(); }
	float GetReloadAnimTime() const { return m_reloadAnimTime; }
	const char* GetFireAnimName() const { return m_fireAnimName.c_str(); }
	const char* GetArmsFireAnimName() const { return m_armsFireAnimName.c_str(); }
	float GetFireAnimTime() const { return m_fireAnimTime; }
	const char* GetRunAnimName() const { return m_runAnimName.c_str(); }
	const char* GetArmsRunAnimName() const { return m_armsRunAnimName.c_str(); }
	float GetRunAnimTime() const { return m_runAnimTime; }
	const char* GetIdleAnimName() const { return m_idleAnimName.c_str(); }
	const char* GetArmsIdleAnimName() const { return m_armsIdleAnimName.c_str(); }
	float GetIdleAnimTime() const { return m_idleAnimTime; }
	std::shared_ptr<ze::SkinnedMesh> GetPVMesh() const { return m_spPVMesh; }
	std::shared_ptr<ze::Armature> GetPVMeshArmature() const { return m_spPVMeshArmature; }
	std::shared_ptr<ze::StaticMesh> GetTVMesh() const { return m_spTVMesh; }
	const std::vector<std::shared_ptr<ze::Material>>& GetMaterials() const { return m_materials; }
	const ze::Texture2D GetPreviewImage() const { return m_previewImage; }
private:
	std::string m_drawAnimName;
	std::string m_armsDrawAnimName;
	float m_drawAnimTime;
	std::string m_reloadAnimName;
	std::string m_armsReloadAnimName;
	float m_reloadAnimTime;
	std::string m_fireAnimName;
	std::string m_armsFireAnimName;
	float m_fireAnimTime;
	std::string m_runAnimName;
	std::string m_armsRunAnimName;
	float m_runAnimTime;
	std::string m_idleAnimName;
	std::string m_armsIdleAnimName;
	float m_idleAnimTime;
	std::shared_ptr<ze::SkinnedMesh> m_spPVMesh;
	std::shared_ptr<ze::Armature> m_spPVMeshArmature;
	std::shared_ptr<ze::StaticMesh> m_spTVMesh;
	std::vector<std::shared_ptr<ze::Material>> m_materials;
	ze::Texture2D m_previewImage;
};

class WeaponInfo
{
public:
	WeaponInfo(std::wstring name, int32_t magCapacity, int32_t auxilirayAmmo, float rpm)
		: m_name(std::move(name))
		, m_magCapacity(magCapacity)
		, m_auxilirayAmmo(auxilirayAmmo)
		, m_rpm(rpm)
		, m_spr(60.0f / rpm)
	{
	}
	~WeaponInfo() = default;


	const wchar_t* GetName() const { return m_name.c_str(); }
	int32_t GetMagCapacity() const { return m_magCapacity; }
	int32_t GetAuxilirayAmmo() const { return m_auxilirayAmmo; }
	float GetRPM() const { return m_rpm; }
	float GetSPR() const { return m_spr; }
private:
	std::wstring m_name;
	int32_t m_magCapacity;
	int32_t m_auxilirayAmmo;
	float m_rpm;	// rounds per minute
	float m_spr;	// secs per round
};

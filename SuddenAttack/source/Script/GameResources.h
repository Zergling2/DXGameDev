#pragma once

#include <ZergEngine\ZergEngine.h>

struct MdlPack
{
	std::shared_ptr<ze::StaticMesh> m_staticMesh;
	std::shared_ptr<ze::SkinnedMesh> m_skinnedMesh;
	std::shared_ptr<ze::Armature> m_armature;
};

enum FireType
{
	Auto,
	Semi
};

struct WeaponInfo
{
	std::wstring m_name;
	size_t m_initCapacity;
	size_t m_initReserved;
	FireType m_fireType;
	uint32_t m_rpm;
	std::string m_armsDrawAnimName;
	std::string m_drawAnimName;
};

class GameResources : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	GameResources(ze::GameObject& owner);
	virtual ~GameResources() = default;

	virtual void Awake() override;

	bool AddModel(std::wstring key, MdlPack value);
	bool AddTexture(std::wstring key, ze::Texture2D value);
	bool AddMaterial(std::wstring key, std::shared_ptr<ze::Material> value);
	bool AddWeaponInfo(std::wstring key, WeaponInfo info);

	MdlPack GetModel(const std::wstring& key) const;
	ze::Texture2D GetTexture(const std::wstring& key) const;
	std::shared_ptr<ze::Material> GetMaterial(const std::wstring& key) const;
	const WeaponInfo* GetWeaponInfo(const std::wstring& key) const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterCollider() const { return m_spCharacterCollider; }
	std::shared_ptr<ze::SphereCollider> GetGroundCheckCollider() const { return m_spGroundCheckSweepCollider; }
	float GetCharacterColliderRadius() const;
	float GetGroundCheckColliderSubtractFactor() const;
private:
	ze::Texture2D m_errTex;
	std::shared_ptr<ze::Material> m_errMaterial;
	MdlPack m_emptyMdlPack;
	std::unordered_map<std::wstring, MdlPack> m_mdlPacks;
	std::unordered_map<std::wstring, ze::Texture2D> m_texturePacks;
	std::unordered_map<std::wstring, std::shared_ptr<ze::Material>> m_materialPacks;
	std::unordered_map<std::wstring, WeaponInfo> m_weaponInfos;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterCollider;
	std::shared_ptr<ze::SphereCollider> m_spGroundCheckSweepCollider;
};

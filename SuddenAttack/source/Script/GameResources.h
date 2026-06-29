#pragma once

#include <ZergEngine\ZergEngine.h>
#include "..\Resource\WeaponCode.h"

class ArmsViewInfo;
class CharacterViewInfo;
class WeaponDefinition;

class GameResources : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	GameResources(ze::GameObject& owner);
	virtual ~GameResources() = default;

	virtual void Awake() override;

	bool AddTexture2D(std::wstring key,ze::Texture2D texture);
	bool AddArmsViewInfo(std::wstring key, std::unique_ptr<ArmsViewInfo> upArmsViewInfo);
	bool AddCharacterViewInfo(std::wstring key, std::unique_ptr<CharacterViewInfo> upCharacterViewInfo);
	bool AddWeaponDefinition(WeaponCode key, std::shared_ptr<WeaponDefinition> spWeaponDef);
	ze::Texture2D GetTexture2D(const std::wstring& key) const;
	const ArmsViewInfo* GetArmsViewinfo(const std::wstring& key) const;
	const CharacterViewInfo* GetCharacterViewInfo(const std::wstring& key) const;
	std::shared_ptr<WeaponDefinition> GetWeaponDefinition(WeaponCode key) const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterCollider() const { return m_spCharacterCollider; }
	std::shared_ptr<ze::SphereCollider> GetGroundCheckCollider() const { return m_spGroundCheckSweepCollider; }
	std::shared_ptr<ze::BoxCollider> GetCharacterBodyCollider() const { return m_spCharacterBodyCollider; }
	const XMFLOAT3& GetCharacterBodyColliderHalfExtents() const;
	std::shared_ptr<ze::CylinderCollider> GetCharacterNeckCollider() const { return m_spCharacterNeckCollider; }
	float GetCharacterNeckColliderHeight() const;
	std::shared_ptr<ze::SphereCollider> GetCharacterHeadCollider() const { return m_spCharacterHeadCollider; }
	float GetCharacterHeadColliderRadius() const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterUpperArmCollider() const { return m_spCharacterUpperArmCollider; }
	float GetCharacterUpperArmColliderRadius() const;
	float GetCharacterUpperArmColliderHeight() const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterForeArmCollider() const { return m_spCharacterForeArmCollider; }
	float GetCharacterForeArmColliderRadius() const;
	float GetCharacterForeArmColliderHeight() const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterThighCollider() const { return m_spCharacterThighCollider; }
	float GetCharacterThighColliderRadius() const;
	float GetCharacterThighColliderHeight() const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterCalfCollider() const { return m_spCharacterCalfCollider; }
	float GetCharacterCalfColliderRadius() const;
	float GetCharacterCalfColliderHeight() const;
	std::shared_ptr<ze::BoxCollider> GetCharacterFootCollider() const { return m_spCharacterFootCollider; }
	const XMFLOAT3& GetCharacterFootColliderHalfExtents() const;
	float GetCharacterColliderRadius() const;
	float GetGroundCheckColliderSubtractFactor() const;
private:
	ze::Texture2D m_errTex;
	std::shared_ptr<ze::Material> m_errMaterial;
	std::unordered_map<std::string, ze::StaticMesh> m_staticMeshes;
	std::unordered_map<std::string, ze::Armature> m_armatures;
	std::unordered_map<std::string, ze::SkinnedMesh> m_skinnedMeshes;
	std::unordered_map<std::wstring, ze::Texture2D> m_texture2ds;
	std::unordered_map<std::wstring, std::unique_ptr<ArmsViewInfo>> m_armsViewInfos;
	std::unordered_map<std::wstring, std::unique_ptr<CharacterViewInfo>> m_characterViewInfos;
	std::unordered_map<WeaponCode, std::shared_ptr<WeaponDefinition>> m_weaponDefs;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterCollider;
	std::shared_ptr<ze::SphereCollider> m_spGroundCheckSweepCollider;
	std::shared_ptr<ze::BoxCollider> m_spCharacterBodyCollider;
	std::shared_ptr<ze::CylinderCollider> m_spCharacterNeckCollider;
	std::shared_ptr<ze::SphereCollider> m_spCharacterHeadCollider;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterUpperArmCollider;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterForeArmCollider;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterThighCollider;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterCalfCollider;
	std::shared_ptr<ze::BoxCollider> m_spCharacterFootCollider;
};

#pragma once

#include <ZergEngine\ZergEngine.h>

class ArmsViewInfo;
class CharacterViewInfo;
class WeaponViewInfo;
class WeaponInfo;

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
	bool AddWeaponViewInfo(std::wstring key, std::unique_ptr<WeaponViewInfo> upWeaponViewInfo);
	bool AddWeaponInfo(std::wstring key, std::unique_ptr<WeaponInfo> upWeaponInfo);
	ze::Texture2D GetTexture2D(const std::wstring& key) const;
	const ArmsViewInfo* GetArmsViewinfo(const std::wstring& key) const;
	const CharacterViewInfo* GetCharacterViewInfo(const std::wstring& key) const;
	const WeaponViewInfo* GetWeaponViewInfo(const std::wstring& key) const;
	const WeaponInfo* GetWeaponInfo(const std::wstring& key) const;
	std::shared_ptr<ze::CapsuleCollider> GetCharacterCollider() const { return m_spCharacterCollider; }
	std::shared_ptr<ze::SphereCollider> GetGroundCheckCollider() const { return m_spGroundCheckSweepCollider; }
	float GetCharacterColliderRadius() const;
	float GetGroundCheckColliderSubtractFactor() const;
private:
	ze::Texture2D m_errTex;
	std::shared_ptr<ze::Material> m_errMaterial;
	std::unordered_map<std::wstring, ze::Texture2D> m_texture2ds;
	std::unordered_map<std::wstring, std::unique_ptr<ArmsViewInfo>> m_armsViewInfos;
	std::unordered_map<std::wstring, std::unique_ptr<CharacterViewInfo>> m_characterViewInfos;
	std::unordered_map<std::wstring, std::unique_ptr<WeaponViewInfo>> m_weaponViewInfos;
	std::unordered_map<std::wstring, std::unique_ptr<WeaponInfo>> m_weaponInfos;
	std::shared_ptr<ze::CapsuleCollider> m_spCharacterCollider;
	std::shared_ptr<ze::SphereCollider> m_spGroundCheckSweepCollider;
};

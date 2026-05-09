#pragma once

#include <ZergEngine\ZergEngine.h>

class GameResources;
class CharacterViewInfo;

class ThirdPersonCharacter : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ThirdPersonCharacter(ze::GameObject& owner);
	virtual ~ThirdPersonCharacter() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void CreateCharacterView(const CharacterViewInfo* pCharacterViewInfo);
private:
	ze::ComponentHandle<GameResources> m_hScriptGameResources;

	static const XMFLOAT3 PRIMARY_WEAPON_TV_OFFSET;		// Primary weapon local pos
	static const XMFLOAT3 SECONDARY_WEAPON_TV_OFFSET;		// Secondary weapon local pos
	XMFLOAT4 m_primaryWeaponLocalRot;
	XMFLOAT4 m_secondaryWeaponLocalRot;
public:
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hSkinnedMeshRendererCharacter;
	ze::GameObjectHandle m_hGameObjectTVWeaponBase;
	ze::GameObjectHandle m_hGameObjectTVWeapons[2];
	ze::GameObjectHandle m_hGameObjectHitboxBody;
	ze::GameObjectHandle m_hGameObjectHitboxHead;
	ze::GameObjectHandle m_hGameObjectHitboxLeftUpperArm;
	ze::GameObjectHandle m_hGameObjectHitboxRightUpperArm;
	ze::GameObjectHandle m_hGameObjectHitboxLeftLowerArm;
	ze::GameObjectHandle m_hGameObjectHitboxRightLowerArm;
	ze::GameObjectHandle m_hGameObjectHitboxLeftThigh;
	ze::GameObjectHandle m_hGameObjectHitboxRightThigh;
	ze::GameObjectHandle m_hGameObjectHitboxLeftCalf;
	ze::GameObjectHandle m_hGameObjectHitboxRightCalf;
};

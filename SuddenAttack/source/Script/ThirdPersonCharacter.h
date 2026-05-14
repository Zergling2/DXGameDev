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
	virtual void FixedUpdate() override;

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
	ze::GameObjectHandle m_hGameObjectHitboxBody;			// Spine0
	ze::GameObjectHandle m_hGameObjectHitboxNeck;			// Neck
	ze::GameObjectHandle m_hGameObjectHitboxHead;			// Head
	ze::GameObjectHandle m_hGameObjectHitboxLeftUpperArm;	// UpperArm.L
	ze::GameObjectHandle m_hGameObjectHitboxRightUpperArm;	// UpperArm.R
	ze::GameObjectHandle m_hGameObjectHitboxLeftForeArm;	// ForeArm.L
	ze::GameObjectHandle m_hGameObjectHitboxRightForeArm;	// ForeArm.R
	ze::GameObjectHandle m_hGameObjectHitboxLeftThigh;		// Thigh.L
	ze::GameObjectHandle m_hGameObjectHitboxRightThigh;		// Thigh.R
	ze::GameObjectHandle m_hGameObjectHitboxLeftCalf;		// Calf.L
	ze::GameObjectHandle m_hGameObjectHitboxRightCalf;		// Calf.R
	ze::GameObjectHandle m_hGameObjectHitboxLeftFoot;		// Foot.L
	ze::GameObjectHandle m_hGameObjectHitboxRightFoot;		// Foot.R
	ze::bone_index_type m_biSpine0;
	ze::bone_index_type m_biNeck;
	ze::bone_index_type m_biHead;
	ze::bone_index_type m_biLeftUpperArm;
	ze::bone_index_type m_biRightUpperArm;
	ze::bone_index_type m_biLeftForeArm;
	ze::bone_index_type m_biRightForeArm;
	ze::bone_index_type m_biLeftThigh;
	ze::bone_index_type m_biRightThigh;
	ze::bone_index_type m_biLeftCalf;
	ze::bone_index_type m_biRightCalf;
	// ze::bone_index_type m_biLeftFoot;
	// ze::bone_index_type m_biRightFoot;
	ze::bone_index_type m_biLeftToe;
	ze::bone_index_type m_biRightToe;
};

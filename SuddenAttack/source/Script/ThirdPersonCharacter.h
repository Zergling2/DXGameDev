#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Contents.h"
#include "..\Resource\WeaponAction.h"

class WeaponDefinition;
class GameResources;
class CharacterViewInfo;
class WeaponEventTable;

class ThirdPersonCharacter : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ThirdPersonCharacter(ze::GameObject& owner);
	virtual ~ThirdPersonCharacter() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void SetCharacterView(const CharacterViewInfo* pCVI);
	void SetWeaponInUse(WeaponSlot slot, WeaponCode weaponCode);

	void ShowView();
	void HideView();
	void OnInit(GameTeam team, WeaponCode primary, WeaponCode secondary, WeaponSlot currWeapon, InGamePlayerState state,
		const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX);
	void OnDraw(WeaponSlot slot);
	void OnFire();
	void OnReload();
	void OnIdle(float exceed);
	void OnRespawn(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX);
	void OnTransform(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX);
	void ActivateCharacterColliderAndHitbox();
	void DeactivateCharacterColliderAndHitbox();

	void SetAnim();

	// void UpdateWeaponBaseTransform();	// deprecated
	void UpdateTVWeaponBaseAndHitboxTransforms();
private:
	void PlayAnimation(WeaponAction action, bool loop);
private:
	static const XMFLOAT3 s_weaponTVOffset[static_cast<size_t>(WeaponSlot::Count)];
	static bool s_weaponLocalRotCalc;
	static XMFLOAT4 s_weaponLocalRotQuaternion[static_cast<size_t>(WeaponSlot::Count)];
	bool m_hitboxActivated;
	WeaponSlot m_currWeaponSlot;
	std::shared_ptr<WeaponDefinition> m_spWeaponDefs[static_cast<size_t>(WeaponSlot::Count)];
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<ze::Rigidbody> m_hCharacterColliderRigidbody;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hSkinnedMeshRendererCharacter;
	ze::ComponentHandle<ze::MeshRenderer> m_hMeshRendererTVWeapon;
	ze::GameObjectHandle m_hGameObjectTVWeaponBase;
	ze::GameObjectHandle m_hGameObjectTVWeapon;
	ze::GameObjectHandle m_hGameObjectHitboxBody;			// Spine0
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxBody;
	ze::GameObjectHandle m_hGameObjectHitboxNeck;			// Neck
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxNeck;
	ze::GameObjectHandle m_hGameObjectHitboxHead;			// Head
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxHead;
	ze::GameObjectHandle m_hGameObjectHitboxLeftUpperArm;	// UpperArm.L
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxLeftUpperArm;
	ze::GameObjectHandle m_hGameObjectHitboxRightUpperArm;	// UpperArm.R
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxRightUpperArm;
	ze::GameObjectHandle m_hGameObjectHitboxLeftForeArm;	// ForeArm.L
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxLeftForeArm;
	ze::GameObjectHandle m_hGameObjectHitboxRightForeArm;	// ForeArm.R
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxRightForeArm;
	ze::GameObjectHandle m_hGameObjectHitboxLeftThigh;		// Thigh.L
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxLeftThigh;
	ze::GameObjectHandle m_hGameObjectHitboxRightThigh;		// Thigh.R
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxRightThigh;
	ze::GameObjectHandle m_hGameObjectHitboxLeftCalf;		// Calf.L
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxLeftCalf;
	ze::GameObjectHandle m_hGameObjectHitboxRightCalf;		// Calf.R
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxRightCalf;
	ze::GameObjectHandle m_hGameObjectHitboxLeftFoot;		// Foot.L
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxLeftFoot;
	ze::GameObjectHandle m_hGameObjectHitboxRightFoot;		// Foot.R
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyHitboxRightFoot;
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
	ze::bone_index_type m_biLeftHand;
	ze::bone_index_type m_biRightHand;

	WeaponAction m_action;
	const WeaponEventTable* m_pCurrWeaponEventTable;
	size_t m_eventIndexCursor;
	float m_actionDuration;
	float m_actionElapsed;
};

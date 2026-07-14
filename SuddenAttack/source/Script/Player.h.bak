#pragma once

#include <ZergEngine\ZergEngine.h>
#include "..\Resource\WeaponCode.h"

class GameResources;
class GameUIManager;
class ArmsViewInfo;
class Weapon;

enum class WeaponSlot
{
	Primary,
	Secondary,
	Melee,
	Utility,

	Count,
	Unknown = Count
};

class Player : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Player(ze::GameObject& owner);
	virtual ~Player() = default;

	virtual void Awake() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;

	void SetProcessingInput(bool b) { m_processingInput = b; }
	void CreateArmsView(const ArmsViewInfo* pArmsViewInfo);

	void EquipWeapon(WeaponSlot slot, WeaponCode wc);
private:
	bool m_processingInput;
	bool m_isStand;
	bool m_isGround;
	bool m_isMoving;
	float m_groundCheckSweepDist;
	float m_stepHeight;
	float m_slope;
	float m_jumpVelocityY;
	float m_velocityY;
	float m_sinTimeAccum;
	float m_bounceFreq;
	float m_ampX;
	float m_ampY;
	XMFLOAT4X4A m_colliderLocalTransform;
	XMFLOAT4X4A m_groundCheckColliderLocalTransform;
public:
	ze::GameObjectHandle m_hGameObjectCamera;
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyPlayer;
	std::shared_ptr<ze::SphereCollider> m_spGroundCheckSweepCollider;

	ze::GameObjectHandle m_hGameObjectArms;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hArmsSkinnedMeshRenderer;
	const ArmsViewInfo* m_pArmsViewInfo;

	ze::GameObjectHandle m_hGameObjectWeapons[static_cast<size_t>(WeaponSlot::Count)];
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(WeaponSlot::Count)];
	ze::ComponentHandle<Weapon> m_hScriptWeapon[static_cast<size_t>(WeaponSlot::Count)];
	WeaponSlot m_currWeaponSlot;
};

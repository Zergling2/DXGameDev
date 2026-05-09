#pragma once

#include <ZergEngine\ZergEngine.h>

class GameResources;
class GameUIManager;
class ArmsViewInfo;
class IWeapon;
class WeaponViewInfo;

enum class WeaponSlot
{
	Primary,
	Secondary,

	ElementCount
};

class IMovementState
{
public:
	IMovementState() = default;
	virtual ~IMovementState() = default;

	virtual void Enter() {}
	virtual void Update(float dt) {}
	virtual void Exit() {}
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
	void CreateWeaponView(WeaponSlot slot, const WeaponViewInfo* pWeaponViewInfo);
private:
	bool m_processingInput;
	bool m_isStand;
	bool m_isGround;
	float m_groundCheckSweepDist;
	float m_stepHeight;
	float m_slope;
	float m_jumpVelocityY;
	float m_velocityY;
	IMovementState* m_pMovementState;
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

	ze::GameObjectHandle m_hGameObjectWeapons[static_cast<size_t>(WeaponSlot::ElementCount)];
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(WeaponSlot::ElementCount)];
	std::shared_ptr<IWeapon> m_weapons[static_cast<size_t>(WeaponSlot::ElementCount)];
	WeaponSlot m_currWeaponSlot;
};

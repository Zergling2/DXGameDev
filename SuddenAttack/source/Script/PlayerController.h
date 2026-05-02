#pragma once

#include <ZergEngine\ZergEngine.h>

class GameResources;

enum class EquipmentState
{
	None,
	Primary,
	Secondary
};

enum class WeaponState
{
	None,
	Drawing,
	Reloading,
	Firing,
	Ready
};

class PlayerController : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	PlayerController(ze::GameObject& owner);
	virtual ~PlayerController() = default;

	virtual void Awake() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;

	void SetPrimaryWeapon(std::wstring key);
	void SetSecondaryWeapon(std::wstring key);
	void SetProcessingInput(bool b) { m_processingInput = b; }

	void ChangePrimaryWeaponState(WeaponState state);
	void ChangeSecondaryWeaponState(WeaponState state);
	void DrawPrimaryWeapon();
	void DrawSecondaryWeapon();
private:
	static const XMFLOAT3 FPSARM_POS;
	static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE;
	static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET;
	static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE;
	static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET;

	bool m_processingInput;
	bool m_isStand;
	bool m_isGround;
	float m_groundCheckSweepDist;
	float m_stepHeight;
	float m_slope;
	float m_jumpVelocityY;
	float m_velocityY;
	XMFLOAT4X4A m_colliderLocalTransform;
	XMFLOAT4X4A m_groundCheckColliderLocalTransform;
	ze::GameObjectHandle m_hGameObjectCamera;
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbodyPlayer;
	std::shared_ptr<ze::SphereCollider> m_spGroundCheckSweepCollider;
	ze::GameObjectHandle m_hGameObjectArms;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hArmsSkinnedMeshRenderer;
	ze::GameObjectHandle m_hGameObjectPrimaryWeapon;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hPrimaryWeaponSkinnedMeshRenderer;
	ze::GameObjectHandle m_hGameObjectSecondaryWeapon;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hSecondaryWeaponSkinnedMeshRenderer;

	std::wstring m_primaryWeaponKey;
	std::wstring m_secondaryWeaponKey;
	EquipmentState m_equipState;
	WeaponState m_primaryWeaponState;
	WeaponState m_secondaryWeaponState;
	bool m_needUpdateArmsWeaponAnim;
	float m_remainingTimeUpdateWeaponState;
	float m_primaryWeaponReadyRemainingTime;
	float m_secondaryWeaponReadyRemainingTime;
};

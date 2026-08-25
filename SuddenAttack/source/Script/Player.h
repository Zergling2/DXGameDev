#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Contents.h"

class GameResources;
class GameUIManager;
class ListenServerClient;
class ArmsViewInfo;
class Weapon;

class Player : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Player(ze::GameObject& owner);
	virtual ~Player() = default;

	virtual void Awake() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;

	bool IsGround() const { return m_isGround; }
	bool CanJump() const { return m_isGround && m_jumpCoolTime <= 0.0f; }

	void SetProcessingInput(bool b) { m_processingInput = b; }
	void SetArmsView(const ArmsViewInfo* pArmsViewInfo);

	void DrawWeapon(WeaponSlot slot);
	void ReloadWeapon();
	void FireWeapon();
	WeaponCode GetCurrentWeaponCode() const;

	void OnDead(float respawnTime);
	void OnRespawn(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX);

	void BroadcastTransform() const;
private:
	bool m_processingInput;
	bool m_isStand;
	bool m_prevIsMoving;
	bool m_isMoving;
	bool m_isGround;
	float m_jumpCoolTime;
	float m_jumpSpeed;
	float m_velocityY;
	float m_sinTimeAccum;
	float m_bounceFreq;
	float m_ampX;
	float m_ampY;
	float m_maxSlope;	// 지면으로 인정되는 최대 각도(라디안)
	float m_groundCheckSweepDistY;
	XMFLOAT4X4A m_playerColliderLocalTransform;
	XMFLOAT4X4A m_groundCheckColliderLocalTransform;
	std::vector<ze::SweepHit> m_sweepResults;
public:
	ze::GameObjectHandle m_hGameObjectCamera;
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
	ze::ComponentHandle<ListenServerClient> m_hScriptListenServerClient;

	ze::ComponentHandle<ze::Camera> m_hCamera;
	ze::ComponentHandle<ze::CharacterController> m_hCharacterController;
	ze::ComponentHandle<ze::Rigidbody> m_hPlayerRigidbody;
	std::shared_ptr<ze::CylinderCollider> m_spGroundCheckCollider;
	ze::GameObjectHandle m_hGameObjectArms;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hArmsSkinnedMeshRenderer;

	ze::GameObjectHandle m_hGameObjectWeapons[static_cast<size_t>(WeaponSlot::Count)];
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(WeaponSlot::Count)];
	ze::ComponentHandle<Weapon> m_hScriptWeapon[static_cast<size_t>(WeaponSlot::Count)];
	WeaponSlot m_currWeaponSlot;
};

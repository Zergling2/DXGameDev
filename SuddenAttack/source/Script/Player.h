#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Contents.h"
#include "..\Resource\MovementType.h"

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

	void OnInit(GameTeam team, WeaponCode primary, WeaponCode secondary, WeaponSlot currWeapon, InGamePlayerState state);
	void SetFoV(uint8_t degree);

	void SetArmsView(const ArmsViewInfo* pArmsViewInfo);
	void SetWeaponInUse(WeaponSlot slot, WeaponCode weaponCode);

	void LoadWeaponFullAmmo();
	void DrawWeapon(WeaponSlot slot);
	void UndrawWeapon();
	void ReloadWeapon();
	void FireWeapon();
	WeaponCode GetCurrentWeaponCode() const;

	void OnDead();
	void OnRespawn(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX, uint16_t hp, uint16_t ap);
	void BroadcastTransform() const;

	void SetProcessingInput(bool b) { m_processingInput = b; }
	bool IsDead() const { return m_isDead; }

	void SetListenServerClientScriptHandle(ze::ComponentHandle<ListenServerClient> hScript) { m_hScriptListenServerClient = hScript; }
	XMMATRIX GetCameraViewMatrix() const;
	XMMATRIX GetCameraProjMatrix() const;
private:
	GameTeam m_team;
	bool m_processingInput;
	bool m_isDead;
	bool m_isStand;
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

	MovementType m_currMoveType;
};

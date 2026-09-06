#include "Player.h"
#include "Weapon.h"
#include "ThirdPersonCharacter.h"
#include "GameResources.h"
#include "GameUIManager.h"
#include "ListenServerClient.h"
#include "..\Resource\GlobalScriptGameObject.h"
#include "..\Resource\GameInfo.h"
#include "..\Resource\Arms.h"
#include "..\Resource\WeaponDefinition.h"
#include "..\Resource\LSProtocol.h"
#include "..\Resource\HitboxName.h"
#include <enet\enet.h>
#include <algorithm>

using namespace ze;

static const XMFLOAT3 FPSARM_POS(0.0f, -0.2f, -0.06f);
static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.1f, -0.04f, 0.23f);
static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET(FPSARM_POS.x + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);
static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.04f, -0.01f, 0.41f);
static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET(FPSARM_POS.x + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 5.0f;
constexpr float WALK_SPEED = SPEED * 0.5f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

Player::Player(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_processingInput(true)
	, m_isDead(true)
	, m_isStand(true)
	, m_isGround(false)
	, m_jumpCoolTime(0.0f)
	, m_jumpSpeed(0.0f)
	, m_velocityY(0.0f)
	, m_sinTimeAccum(0.0f)
	, m_bounceFreq(0.0f)
	, m_ampX(0.0f)
	, m_ampY(0.0f)
	, m_maxSlope(0.0f)
	, m_groundCheckSweepDistY(0.0f)
	, m_currWeaponSlot(WeaponSlot::Unknown)
	, m_currMoveType(MovementType::Unknown)
{
}

void Player::Awake()
{
	// GameUIManager 검색 및 상호참조
	GameObjectHandle hGameObjGlobalScripts = GameObject::Find(GLOBAL_SCRIPTS_GAME_OBJECT_NAME);
	assert(hGameObjGlobalScripts.IsValid());

	ComponentHandle<GameUIManager> hScriptGameUIManager = hGameObjGlobalScripts.ToPtr()->GetComponent<GameUIManager>();
	m_hScriptGameUIManager = hScriptGameUIManager;
	GameUIManager* pGameUIManager = hScriptGameUIManager.ToPtr();
	pGameUIManager->SetPlayerScriptHandle(this->ToHandle());


	// 글로벌 스크립트 오브젝트 검색 및 스크립트 저장
	GameObjectHandle hGameObjectGlobalScripts = GameObject::Find(GLOBAL_SCRIPTS_GAME_OBJECT_NAME);
	assert(hGameObjectGlobalScripts.IsValid());

	m_hScriptGameResources = hGameObjectGlobalScripts.ToPtr()->GetComponent<GameResources>();
	assert(m_hScriptGameResources.IsValid());

	const GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();
	m_jumpSpeed = pScriptGameResources->GetJumpSpeed();
	m_spGroundCheckCollider = pScriptGameResources->GetGroundCheckCollider();

	m_maxSlope = 0.7071067f;	// cos(45도)

	// Rigidbody 컴포넌트 추가
	auto spCharacterCollider = pScriptGameResources->GetCharacterCollider();
	m_groundCheckSweepDistY = spCharacterCollider->GetTotalHeight() / 2.0f;

	ComponentHandle<Rigidbody> hPlayerRigidbody = m_pGameObject->AddComponent<Rigidbody>(
		spCharacterCollider, XMFLOAT3(0.0f, spCharacterCollider->GetTotalHeight() / 2.0f, 0.0f)		// GameObject에 캡슐 콜라이더 밑면이 오게한다.
	);
	m_hPlayerRigidbody = hPlayerRigidbody;
	Rigidbody* pPlayerRigidbody = hPlayerRigidbody.ToPtr();
	pPlayerRigidbody->SetBodyType(RigidbodyType::Kinematic);

	XMMATRIX playerColliderLocalTransform;
	playerColliderLocalTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&pPlayerRigidbody->GetColliderLocalRot()));
	playerColliderLocalTransform.r[3] = XMVectorSetW(XMLoadFloat3(&pPlayerRigidbody->GetColliderLocalPos()), 1.0f);
	XMStoreFloat4x4A(&m_playerColliderLocalTransform, playerColliderLocalTransform);


	// 카메라 컴포넌트 추가 (자식 오브젝트로 추가!)
	GameObjectHandle hGameObjectCamera = Runtime::GetInstance()->CreateGameObject();
	m_hGameObjectCamera = hGameObjectCamera;
	GameObject* pGameObjectCamera = hGameObjectCamera.ToPtr();
	pGameObjectCamera->m_transform.SetParent(&m_pGameObject->m_transform);
	pGameObjectCamera->m_transform.SetPositionY(spCharacterCollider->GetTotalHeight() - 0.2f);
	ComponentHandle<Camera> hCamera = pGameObjectCamera->AddComponent<Camera>();
	m_hCamera = hCamera;
	Camera* pCamera = hCamera.ToPtr();
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(82);
	pCamera->SetClippingPlanes(0.1f, 300.0f);



	// FPS 팔 생성
	GameObjectHandle hGameObjectArms = Runtime::GetInstance()->CreateGameObject(L"pvarms");
	m_hGameObjectArms = hGameObjectArms;
	GameObject* pGameObjectArms = hGameObjectArms.ToPtr();
	pGameObjectArms->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectArms->m_transform.SetPosition(FPSARM_POS);
	ComponentHandle<SkinnedMeshRenderer> hArmsSkinnedMeshRenderer = pGameObjectArms->AddComponent<SkinnedMeshRenderer>();
	m_hArmsSkinnedMeshRenderer = hArmsSkinnedMeshRenderer;
	SetArmsView(pScriptGameResources->GetArmsViewinfo(L"steven"));


	assert(m_hArmsSkinnedMeshRenderer.IsValid() && m_hScriptGameUIManager.IsValid());

	// 무기 오브젝트 생성

	for (size_t i = 0; i < _countof(m_hGameObjectWeapons); ++i)
	{
		m_hGameObjectWeapons[i] = Runtime::GetInstance()->CreateGameObject();
		GameObject* pGameObjWeapon = m_hGameObjectWeapons[i].ToPtr();
		pGameObjWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);	// 무기들은 카메라 오브젝트의 자식으로 추가된다.
		pGameObjWeapon->m_transform.SetPosition(PRIMARY_WEAPON_PV_OFFSET);
		m_hWeaponSkinnedMeshRenderers[i] = pGameObjWeapon->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pWeaponSkinnedMeshRenderer = m_hWeaponSkinnedMeshRenderers[i].ToPtr();

		assert(m_hWeaponSkinnedMeshRenderers[i].IsValid());
		m_hScriptWeapon[i] = pGameObjWeapon->AddComponent<Weapon>();
		Weapon* pScriptWeapon = m_hScriptWeapon[i].ToPtr();
		pScriptWeapon->m_hWeaponMeshRenderer = m_hWeaponSkinnedMeshRenderers[i];
		pScriptWeapon->m_hArmsMeshRenderer = m_hArmsSkinnedMeshRenderer;
		pScriptWeapon->m_hScriptGameUIManager = m_hScriptGameUIManager;

		pScriptWeapon->Undraw();
	}
}

void Player::Update()
{
	const float dt = Time::GetInstance()->GetDeltaTime();

	if (m_jumpCoolTime > 0.0f)
		m_jumpCoolTime = (std::max)(m_jumpCoolTime - dt, 0.0f);

	// 팔과 무기 흔들림 업데이트
	constexpr float BOUNCE_FREQ_WEIGHT_RUNNING = 9.0f;
	const bool isMoving = m_currMoveType != MovementType::Stop && m_currMoveType != MovementType::Unknown;
	bool isRunning = isMoving && Input::GetInstance()->GetKey(Keycode::KEY_LSHIFT) == false;

	float targetBounceFreq = isRunning ? BOUNCE_FREQ_WEIGHT_RUNNING : BOUNCE_FREQ_WEIGHT_RUNNING * 0.5f;
	float targetAmpX = isMoving ? 0.01f : 0.0f;
	float targetAmpY = isMoving ? 0.02f : 0.0f;

	constexpr float INTERPOLATION_SPEED = 12.0f;
	m_bounceFreq = Math::Lerp(m_bounceFreq, targetBounceFreq, dt * INTERPOLATION_SPEED);
	m_ampX = Math::Lerp(m_ampX, targetAmpX, dt * INTERPOLATION_SPEED);
	m_ampY = Math::Lerp(m_ampY, targetAmpY, dt * INTERPOLATION_SPEED);

	m_sinTimeAccum = Math::WrapFloat(m_sinTimeAccum + dt * m_bounceFreq, Math::C_2PI());

	// x bounce weight, y bounce weight
	float xw = m_ampX * std::sin(m_sinTimeAccum);
	float yw = m_ampY * std::abs(std::cos(m_sinTimeAccum));

	m_hGameObjectArms.ToPtr()->m_transform.SetPosition(FPSARM_POS.x + xw, FPSARM_POS.y + yw, FPSARM_POS.z);
	switch (m_currWeaponSlot)
	{
	case WeaponSlot::Primary:
		m_hGameObjectWeapons[static_cast<size_t>(WeaponSlot::Primary)].ToPtr()->m_transform.SetPosition(PRIMARY_WEAPON_PV_OFFSET.x + xw, PRIMARY_WEAPON_PV_OFFSET.y + yw, PRIMARY_WEAPON_PV_OFFSET.z);
		break;
	case WeaponSlot::Secondary:
		m_hGameObjectWeapons[static_cast<size_t>(WeaponSlot::Secondary)].ToPtr()->m_transform.SetPosition(SECONDARY_WEAPON_PV_OFFSET.x + xw, SECONDARY_WEAPON_PV_OFFSET.y + yw, SECONDARY_WEAPON_PV_OFFSET.z);
		break;
	default:
		break;
	}

	if (m_processingInput)
	{
		// 무기 드로잉 처리 (사망 시 리스폰 직전까지 m_currWeaponSlot를 WeaponSlot::Unknown으로 설정해야 함!)
		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_1) && m_currWeaponSlot != WeaponSlot::Primary)
			this->DrawWeapon(WeaponSlot::Primary);

		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_2) && m_currWeaponSlot != WeaponSlot::Secondary)
			this->DrawWeapon(WeaponSlot::Secondary);

		// 장전 처리
		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_R))
			this->ReloadWeapon();

		// 사격 처리
		if (Input::GetInstance()->GetMouseButton(MouseButton::Left))
			this->FireWeapon();


		const int32_t mx = Input::GetInstance()->GetMouseAxisHorizontal();
		const int32_t my = Input::GetInstance()->GetMouseAxisVertical();
		if (mx != 0)	// 마우스 수평 움직임은 플레이어 오브젝트의 y 회전을 조정
		{
			XMVECTOR mainObjectLocalRot = m_pGameObject->m_transform.GetRotation();
			XMVECTOR temp = Math::QuaternionToEuler(mainObjectLocalRot);
			XMFLOAT3A rotationEuler;
			XMStoreFloat3A(&rotationEuler, temp);

			rotationEuler.x = 0.0f;
			rotationEuler.y += XMConvertToRadians(static_cast<float>(mx) * SENSITIVITY);
			rotationEuler.z = 0.0f;

			m_pGameObject->m_transform.SetRotationEuler(rotationEuler);
		}

		if (my != 0)	// 마우스 수직 움직임은 카메라 오브젝트의 회전만을 변경 (카메라 오브젝트는 플레이어 오브젝트의 자식)
		{
			GameObject* pGameObjCamera = m_hGameObjectCamera.ToPtr();
			XMVECTOR cameraObjectLocalRot = pGameObjCamera->m_transform.GetRotation();
			XMVECTOR temp = Math::QuaternionToEuler(cameraObjectLocalRot);
			XMFLOAT3A rotationEuler;
			XMStoreFloat3A(&rotationEuler, temp);

			rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
			rotationEuler.x = Math::Clamp(rotationEuler.x, XMConvertToRadians(-HEAD_CLAMP_ANGLE), XMConvertToRadians(+HEAD_CLAMP_ANGLE));
			rotationEuler.y = 0.0f;
			rotationEuler.z = 0.0f;

			pGameObjCamera->m_transform.SetRotationEuler(rotationEuler);
		}
	}
}

void Player::FixedUpdate()
{
	bool isShiftPressed = Input::GetInstance()->GetKey(KEY_LSHIFT);
	const float speed = isShiftPressed ? WALK_SPEED : SPEED;
	const float dt = Time::GetInstance()->GetFixedDeltaTime();

	XMVECTOR qWorldRotation = m_pGameObject->m_transform.GetWorldRotation();
	XMVECTOR vForward = XMVector3Normalize(XMVectorSetY(XMVector3Rotate(Vector3::Forward(), qWorldRotation), 0.0f)); // Y성분 제거

	XMVECTOR vFinalMove = XMVectorZero(); // 이동해야할 변위량 누적
	MovementType moveType = MovementType::Stop;

	if (m_processingInput)
	{
		// 1. 방향키에 따른 입력 누적
		XMVECTOR vForwardMove = vForward;
		XMVECTOR vRightMove = XMVector3Cross(Vector3::Up(), vForward);


		if (Input::GetInstance()->GetKey(KEY_W))
		{
			moveType = moveType | MovementType::ForwardBackward;
			vFinalMove = XMVectorAdd(vFinalMove, vForwardMove);
		}

		if (Input::GetInstance()->GetKey(KEY_S))
		{
			moveType = moveType | MovementType::ForwardBackward;
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vForwardMove));
		}

		if (Input::GetInstance()->GetKey(KEY_A))
		{
			moveType = moveType | MovementType::LeftRight;
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vRightMove));
		}

		if (Input::GetInstance()->GetKey(KEY_D))
		{
			moveType = moveType | MovementType::LeftRight;
			vFinalMove = XMVectorAdd(vFinalMove, vRightMove);
		}

		const float finalMoveLen = XMVectorGetX(XMVector3Length(vFinalMove));
		if (finalMoveLen > 0.0f)
			vFinalMove = XMVectorScale(vFinalMove, 1.0f / finalMoveLen);

		vFinalMove = XMVectorScale(vFinalMove, speed * dt);
	}

	m_currMoveType = moveType;

	// 2. 중력에 의한 이동량 누적
	if (m_isGround)
		m_velocityY = 0.0f;
	else
		m_velocityY += Physics::GetInstance()->GetGravity().y * 2 * dt;

	if (m_processingInput && Input::GetInstance()->GetKey(Keycode::KEY_SPACE))
	{
		if (this->CanJump())
		{
			constexpr float JUMP_DELAY = 0.6f;
			m_jumpCoolTime = JUMP_DELAY;
			m_velocityY = m_jumpSpeed;
		}
	}

	XMVECTOR vGravityMove = XMVectorSet(0.0f, m_velocityY * dt, 0.0f, 0.0f);
	vFinalMove = XMVectorAdd(vFinalMove, vGravityMove);

	// Sweep 기반 이동
	Rigidbody* pPlayerRigidbody = m_hPlayerRigidbody.ToPtr();
	XMMATRIX playerColliderLocalTransform = XMLoadFloat4x4A(&m_playerColliderLocalTransform);
	XMVECTOR vRemainingMove = vFinalMove;
	constexpr float SKIN_WIDTH = 0.002f;
	constexpr size_t MAX_ITER = 4;
	for (size_t i = 0; i < MAX_ITER; ++i)
	{
		float remainingMoveLen = XMVectorGetX(XMVector3Length(vRemainingMove));
		if (remainingMoveLen < SKIN_WIDTH)
			break;

		XMMATRIX playerColliderWorldTransform = XMMatrixMultiply(playerColliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());

		XMFLOAT3A remainingMove;
		XMStoreFloat3A(&remainingMove, vRemainingMove);

		size_t collisionCount = Physics::GetInstance()->ConvexSweepTestNotIncludeExceptTrigger(
			m_sweepResults,
			playerColliderWorldTransform,
			remainingMove,
			pPlayerRigidbody->GetCollider().get(),
			pPlayerRigidbody
		);

		if (m_sweepResults.size() == 0)
		{
			m_pGameObject->m_transform.Translate(remainingMove);
			break;
		}

		std::sort(m_sweepResults.begin(), m_sweepResults.end(),
			[](const SweepHit& a, const SweepHit& b)
			{
				return a.m_hitFraction < b.m_hitFraction;
			}
		);

		const float closestHitFraction = m_sweepResults[0].m_hitFraction;
		XMVECTOR vMovePart = XMVectorScale(vRemainingMove, closestHitFraction);
		XMVECTOR vSkinPush = XMVectorScale(XMVectorScale(vRemainingMove, 1.0f / remainingMoveLen), -SKIN_WIDTH);
		m_pGameObject->m_transform.Translate(vMovePart + vSkinPush);

		vRemainingMove = XMVectorScale(vRemainingMove, 1.0f - closestHitFraction);

		for (const SweepHit& hitItem : m_sweepResults)
		{
			// if (hitItem.m_hitFraction > closestHitFraction + 0.01f)
			// 	break;

			XMVECTOR vHitNormalWorld = XMVector3Normalize(XMLoadFloat3(&hitItem.m_hitNormalWorld));
			float dot = XMVectorGetX(XMVector3Dot(vRemainingMove, vHitNormalWorld));
			if (dot < 0.0f)
			{
				vRemainingMove = XMVectorSubtract(vRemainingMove, XMVectorScale(vHitNormalWorld, dot));
			}
		}
	}

	// 마지막으로 아래방향 sweep으로 m_isGround 업데이트
	XMMATRIX groundCheckColliderWorldTransform = XMMatrixMultiply(playerColliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());
	const XMFLOAT3A groundCheckSweepVector(0.0f, -m_groundCheckSweepDistY, 0.0f);

	// Physics::GetInstance()->ConvexSweepTestNotIncludeExceptTrigger(m_sweepResults, groundCheckColliderWorldTransform, groundCheckSweepVector, m_spGroundCheckCollider.get(), pPlayerRigidbody);
	// bool hit = false;
	// for (const SweepHit& item : m_sweepResults)
	// {
	// 	const float dot = XMVectorGetX(XMVector3Dot(Vector3::Up(), XMVector3Normalize(XMLoadFloat3(&item.m_hitNormalWorld))));
	// 	if (dot > m_maxSlope)
	// 	{
	// 		hit = true;
	// 		break;
	// 	}
	// }
	// 
	// m_isGround = hit;

	SweepHit groundSweep;
	if (Physics::GetInstance()->ClosestConvexSweepTestNotIncludeExceptTrigger(groundSweep, groundCheckColliderWorldTransform, groundCheckSweepVector, m_spGroundCheckCollider.get(), pPlayerRigidbody))
	{
		if (groundSweep.m_hitFraction < 1.0f)
		{
			XMVECTOR vCorrection = XMVectorScale(XMVectorNegate(XMLoadFloat3A(&groundCheckSweepVector)), (1.0f - groundSweep.m_hitFraction));
			m_pGameObject->m_transform.Translate(vCorrection);
		}
	
		const float dot = XMVectorGetX(XMVector3Dot(Vector3::Up(), XMVector3Normalize(XMLoadFloat3(&groundSweep.m_hitNormalWorld))));
		m_isGround = dot > m_maxSlope;
	}
	else
	{
		m_isGround = false;
	}



	// 위치 브로드캐스팅
	if (!m_isDead)
		this->BroadcastTransform();
}

void Player::OnInit(GameTeam team, WeaponCode primary, WeaponCode secondary, WeaponSlot currWeapon, InGamePlayerState state)
{
	UNREFERENCED_PARAMETER(state);

	m_team = team;

	this->SetProcessingInput(false);

	const GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();

	// 1인칭 팔 뷰 설정
	this->SetArmsView(pScriptGameResources->GetArmsViewinfo(L"steven"));

	// 무기 뷰 설정
	this->SetWeaponInUse(WeaponSlot::Primary, primary);
	this->SetWeaponInUse(WeaponSlot::Secondary, secondary);

	this->DrawWeapon(currWeapon);

	// m_pGameObject->m_transform.SetPosition(0.0f, 0.0f, 0.0f);
}

void Player::SetFoV(uint8_t degree)
{
	Camera* pCamera = m_hCamera.ToPtr();
	if (!pCamera)
		return;

	pCamera->SetFieldOfView(degree);
}

void Player::SetArmsView(const ArmsViewInfo* pArmsViewInfo)
{
	SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();

	pArmsSkinnedMeshRenderer->SetMesh(pArmsViewInfo->GetMesh());
	pArmsSkinnedMeshRenderer->SetArmature(pArmsViewInfo->GetArmature());

	for (size_t i = 0; i < pArmsViewInfo->GetMaterials().size(); ++i)
		pArmsSkinnedMeshRenderer->SetMaterial(i, pArmsViewInfo->GetMaterials()[i]);

	// FPS 팔 애니메이션
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_idle_m16a1", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_draw_m16a1", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_idle_usp", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_draw_usp", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_reload_m16a1", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_reload_usp", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_shoot_m16a1", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_shoot_usp", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_run_m16a1", true);
	// pArmsSkinnedMeshRenderer->PlayAnimation("arms_run_usp", true);
}

void Player::SetWeaponInUse(WeaponSlot slot, WeaponCode weaponCode)
{
	const GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();

	Weapon* pScriptWeapon = m_hScriptWeapon[static_cast<size_t>(slot)].ToPtr();

	const std::shared_ptr<WeaponDefinition> spWeaponDef = pScriptGameResources->GetWeaponDefinition(weaponCode);
	if (spWeaponDef)
	{
		pScriptWeapon->Init(spWeaponDef, spWeaponDef->GetMagCapacity(), spWeaponDef->GetInitAuxAmmo());
	}
	else
	{
		wprintf(L"[Player::SetWeaponInUse()] Invalid weapon code: %zu\n", static_cast<size_t>(weaponCode));
	}
}

void Player::LoadWeaponFullAmmo()
{
	for (size_t i = 0; i < _countof(m_hScriptWeapon); ++i)
	{
		Weapon* pScriptWeapon = m_hScriptWeapon[i].ToPtr();
		if (!pScriptWeapon)
			continue;

		pScriptWeapon->LoadFullAmmo();
	}
}

void Player::DrawWeapon(WeaponSlot slot)
{
	if (!(slot < WeaponSlot::Count))
		return;

	if (m_currWeaponSlot != slot)
	{
		const WeaponSlot oldWeaponSlot = m_currWeaponSlot;
		m_currWeaponSlot = slot;

		if (oldWeaponSlot < WeaponSlot::Count)
		{
			Weapon* pScriptOldWeapon = m_hScriptWeapon[static_cast<size_t>(oldWeaponSlot)].ToPtr();
			pScriptOldWeapon->Undraw();
		}

		Weapon* pScriptCurrWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
		pScriptCurrWeapon->Draw();

		GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
		pScriptGameUIManager->SetTextWeaponName(WeaponInfo::GetWeaponNameString(pScriptCurrWeapon->GetWeaponCode()));

		LSCSNotifyGamePlayerWeaponEvent ntfy;
		ntfy.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_WEAPON_EVENT;
		ntfy.m_action = WeaponAction::Draw;
		ntfy.m_slot = m_currWeaponSlot;	// m_slot 멤버 필드는 Draw Weapon Event인 경우에만 사용됨.

		ListenServerClient* pScriptListenServerClient = m_hScriptListenServerClient.ToPtr();

		// 사격 이벤트는 UNRELIABLE, 나머지 이벤트는 RELIABLE 채널로 송신.
		ENetPacket* pNtfyPktWeaponEvent = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
		if (!pScriptListenServerClient->SendPacket(pNtfyPktWeaponEvent))
		{
			enet_packet_destroy(pNtfyPktWeaponEvent);
			pNtfyPktWeaponEvent = nullptr;
		}
	}
}

void Player::UndrawWeapon()
{
	if (m_currWeaponSlot < WeaponSlot::Count)
	{
		Weapon* pScriptOldWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
		pScriptOldWeapon->Undraw();

		m_currWeaponSlot = WeaponSlot::Unknown;
	}
}

void Player::ReloadWeapon()
{
	if (m_currWeaponSlot == WeaponSlot::Unknown)
		return;

	Weapon* pScriptCurrWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
	if (!pScriptCurrWeapon->CanReload())
		return;

	pScriptCurrWeapon->Reload();

	LSCSNotifyGamePlayerWeaponEvent ntfy;
	ntfy.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_WEAPON_EVENT;
	ntfy.m_action = WeaponAction::Reload;
	ntfy.m_slot = m_currWeaponSlot;	// m_slot 멤버 필드는 Draw Weapon Event인 경우에만 사용되지만 참고용으로 전달.

	ListenServerClient* pScriptListenServerClient = m_hScriptListenServerClient.ToPtr();

	// 사격 이벤트는 UNRELIABLE, 나머지 이벤트는 RELIABLE 채널로 송신.
	ENetPacket* pNtfyPktWeaponEvent = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
	if (!pScriptListenServerClient->SendPacket(pNtfyPktWeaponEvent))
	{
		enet_packet_destroy(pNtfyPktWeaponEvent);
		pNtfyPktWeaponEvent = nullptr;
	}
}

void Player::FireWeapon()
{
	if (m_currWeaponSlot == WeaponSlot::Unknown)
		return;

	Weapon* pScriptCurrWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
	if (!pScriptCurrWeapon->CanFire())
		return;

	pScriptCurrWeapon->Fire();

	LSCSNotifyGamePlayerWeaponEvent ntfy;
	ntfy.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_WEAPON_EVENT;
	ntfy.m_action = WeaponAction::Fire;
	ntfy.m_slot = m_currWeaponSlot;	// m_slot 멤버 필드는 Draw Weapon Event인 경우에만 사용되지만 참고용으로 전달.

	ListenServerClient* pScriptListenServerClient = m_hScriptListenServerClient.ToPtr();

	// 사격 이벤트는 UNRELIABLE, 나머지 이벤트는 RELIABLE 채널로 송신.
	ENetPacket* pNtfyPktWeaponEvent = enet_packet_create(&ntfy, sizeof(ntfy), ENET_PACKET_FLAG_RELIABLE);
	if (!pScriptListenServerClient->SendPacket(pNtfyPktWeaponEvent))
	{
		enet_packet_destroy(pNtfyPktWeaponEvent);
		pNtfyPktWeaponEvent = nullptr;
	}


	const GameObject* pGameObjCamera = m_hGameObjectCamera.ToPtr();
	XMFLOAT3A from;
	XMStoreFloat3A(&from, pGameObjCamera->m_transform.GetWorldPosition());
	XMFLOAT3A to;
	constexpr float RAY_DIST = 1000.0f;
	XMVECTOR dir = XMVector3Normalize(pGameObjCamera->m_transform.GetWorldTransformMatrix().r[2]);
	XMStoreFloat3A(&to, XMVectorScale(dir, RAY_DIST));	// 기저벡터의 z축이 향하는 방향

	RayHit triggerHit;
	if (Physics::GetInstance()->ClosestRaycastTestOnlyTrigger(triggerHit, from, to))
	{
		const Rigidbody* pHitRigidbody = triggerHit.m_pHitObject;
		wprintf(L"Ray Hit Part: %s (Hit Fraction: %f)\n", pHitRigidbody->GetGameObjectHandle().ToPtr()->GetName(), triggerHit.m_hitFraction);

		const GameObject* pGameObjHitbox = pHitRigidbody->GetGameObjectHandle().ToPtr();
		const GameObject* pGameObj = pGameObjHitbox->m_transform.GetParent()->GetGameObject();
		ComponentHandle<ThirdPersonCharacter> hScriptThirdPersonCharacter = pGameObj->GetComponent<ThirdPersonCharacter>();
		const ThirdPersonCharacter* pScriptThirdPersonCharacter = hScriptThirdPersonCharacter.ToPtr();

		if (pScriptThirdPersonCharacter->GetTeam() == m_team)	// 아군 사격 예외처리
			return;


		// 현재 예외처리 루틴이 없어서 지형지물을 통과해서 판정됨.
		// TPC 캡슐 콜라이더들만을 제외하고 ClosestRaycast를 수행하고 hitFraction을 비교한다.
		// ...

		LSCSNotifyGamePlayerHit notify;
		notify.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_HIT;
		notify.m_accountIdWhoWasShot = pScriptThirdPersonCharacter->GetAccountId();
		notify.m_weaponCode = pScriptCurrWeapon->GetWeaponCode();
		if (wcscmp(pGameObjHitbox->GetName(), HTB_BODY_NAME) == 0)
			notify.m_hitPart = HitboxPart::Body;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_NECK_NAME) == 0)
			notify.m_hitPart = HitboxPart::Neck;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_HEAD_NAME) == 0)
			notify.m_hitPart = HitboxPart::Head;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_UPPER_ARM_NAME) == 0)
			notify.m_hitPart = HitboxPart::UpperArm;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_FORE_ARM_NAME) == 0)
			notify.m_hitPart = HitboxPart::ForeArm;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_THIGH_NAME) == 0)
			notify.m_hitPart = HitboxPart::Thigh;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_CALF_NAME) == 0)
			notify.m_hitPart = HitboxPart::Calf;
		else if (wcscmp(pGameObjHitbox->GetName(), HTB_FOOT_NAME) == 0)
			notify.m_hitPart = HitboxPart::Foot;
		else
			notify.m_hitPart = HitboxPart::Body;


		ENetPacket* pPkt = enet_packet_create(&notify, sizeof(notify), ENET_PACKET_FLAG_RELIABLE);
		if (!m_hScriptListenServerClient.ToPtr()->SendPacket(pPkt))
		{
			enet_packet_destroy(pPkt);
			pPkt = nullptr;
		}
	}
}

WeaponCode Player::GetCurrentWeaponCode() const
{
	if (m_currWeaponSlot < WeaponSlot::Count)
		return m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->GetWeaponCode();
	else
		return WeaponCode::Unknown;
}

void Player::OnDead()
{
	m_isDead = true;
	m_currMoveType = MovementType::Stop;

	this->SetProcessingInput(false);

	this->UndrawWeapon();
}

void Player::OnRespawn(const XMFLOAT3& pos, const XMFLOAT4& rot, float camRotX, uint16_t hp, uint16_t ap)
{
	m_isDead = false;
	m_currMoveType = MovementType::Stop;

	this->LoadWeaponFullAmmo();

	this->SetProcessingInput(true);

	GameUIManager* pScriptGameUIManager = m_hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->SetTextHP(hp);
	pScriptGameUIManager->SetTextAP(ap);

	m_pGameObject->m_transform.SetPosition(pos);
	m_pGameObject->m_transform.SetRotationQuaternion(rot);
	
	GameObject* pGameObjCamera = m_hGameObjectCamera.ToPtr();
	pGameObjCamera->m_transform.SetRotationEuler(camRotX, 0.0f, 0.0f);

	this->DrawWeapon(WeaponSlot::Secondary);	// 리스폰 시 보조무기를 들고 시작.
}

void Player::BroadcastTransform() const
{
	LSCSNotifyGamePlayerTransform ntfyTransform;
	ntfyTransform.m_protocol = LSProtocol::CS_NOTIFY_GAME_PLAYER_TRANSFORM;

	
	ntfyTransform.m_x = m_pGameObject->m_transform.GetPositionX();
	ntfyTransform.m_y = m_pGameObject->m_transform.GetPositionY();
	ntfyTransform.m_z = m_pGameObject->m_transform.GetPositionZ();
	ntfyTransform.m_rx = m_pGameObject->m_transform.GetRotationX();
	ntfyTransform.m_ry = m_pGameObject->m_transform.GetRotationY();
	ntfyTransform.m_rz = m_pGameObject->m_transform.GetRotationZ();
	ntfyTransform.m_rw = m_pGameObject->m_transform.GetRotationW();
	ntfyTransform.m_moveType = m_currMoveType;

	GameObject* pGameObjCamera = m_hGameObjectCamera.ToPtr();
	const XMVECTOR rotEuler = Math::QuaternionToEulerNormal(pGameObjCamera->m_transform.GetRotation());
	ntfyTransform.m_camRotX = XMVectorGetX(rotEuler);

	ENetPacket* pNtfyPktTransform = enet_packet_create(&ntfyTransform, sizeof(ntfyTransform), 0);
	ListenServerClient* pScriptListenServerClient = m_hScriptListenServerClient.ToPtr();
	if (!pScriptListenServerClient->SendPacket(pNtfyPktTransform))
	{
		enet_packet_destroy(pNtfyPktTransform);
		pNtfyPktTransform = nullptr;
	}
}

XMMATRIX Player::GetCameraViewMatrix() const
{
	const Camera* pCamera = m_hCamera.ToPtr();
	if (!pCamera)
		return XMMatrixIdentity();
	else
		return pCamera->GetViewMatrix();
}

XMMATRIX Player::GetCameraProjMatrix() const
{
	const Camera* pCamera = m_hCamera.ToPtr();
	if (!pCamera)
		return XMMatrixIdentity();
	else
		return pCamera->GetProjMatrix();
}

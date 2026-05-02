#include "PlayerController.h"
#include "../Resource/GlobalGameObjects.h"
#include "GameResources.h"
#include "AnimControl.h"
#include <algorithm>

using namespace ze;

const XMFLOAT3 PlayerController::FPSARM_POS(0.0f, -0.2f, -0.06f);
const XMFLOAT3 PlayerController::PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.1f, -0.04f, 0.23f);
const XMFLOAT3 PlayerController::PRIMARY_WEAPON_PV_OFFSET(FPSARM_POS.x + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);
const XMFLOAT3 PlayerController::SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.04f, -0.01f, 0.41f);
const XMFLOAT3 PlayerController::SECONDARY_WEAPON_PV_OFFSET(FPSARM_POS.x + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 4.2f;
constexpr float WALK_SPEED = SPEED * 0.5f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

static void InitSkinnedMeshStevenArms(GameResources* pScriptGameResources, SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	auto md = pScriptGameResources->GetModel(L"steven_arms");
	auto& arma = md.m_armature;
	auto& mesh = md.m_skinnedMesh;
	auto mtl = pScriptGameResources->GetMaterial(L"steven_arms_mtl0");

	pSkinnedMeshRenderer->SetMesh(mesh);
	pSkinnedMeshRenderer->SetArmature(arma);
	pSkinnedMeshRenderer->SetMaterial(0, std::move(mtl));
}

static void InitSkinnedMeshM16A1(GameResources* pScriptGameResources, SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	auto md = pScriptGameResources->GetModel(L"m16a1");
	auto& arma = md.m_armature;
	auto& mesh = md.m_skinnedMesh;
	auto mtl0 = pScriptGameResources->GetMaterial(L"m16a1_mtl0");
	auto mtl1 = pScriptGameResources->GetMaterial(L"m16a1_mtl1");
	auto mtl_mag = pScriptGameResources->GetMaterial(L"m16a1_mag_mtl0");
	pSkinnedMeshRenderer->SetMesh(mesh);
	pSkinnedMeshRenderer->SetArmature(arma);
	pSkinnedMeshRenderer->SetMaterial(0, std::move(mtl0));
	pSkinnedMeshRenderer->SetMaterial(1, std::move(mtl1));
	pSkinnedMeshRenderer->SetMaterial(2, std::move(mtl_mag));
}

static void InitSkinnedMeshM4A1(GameResources* pScriptGameResources, SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	auto md = pScriptGameResources->GetModel(L"m4a1");
	auto& arma = md.m_armature;
	auto& mesh = md.m_skinnedMesh;
	auto mtl0 = pScriptGameResources->GetMaterial(L"m4a1_mtl0");
	auto mtl1 = pScriptGameResources->GetMaterial(L"m4a1_mtl1");
	auto mtl2 = pScriptGameResources->GetMaterial(L"m4a1_mtl2");
	auto mtl_mag = pScriptGameResources->GetMaterial(L"m16a1_mag_mtl0");
	pSkinnedMeshRenderer->SetMesh(mesh);
	pSkinnedMeshRenderer->SetArmature(arma);
	pSkinnedMeshRenderer->SetMaterial(0, std::move(mtl0));
	pSkinnedMeshRenderer->SetMaterial(1, std::move(mtl1));
	pSkinnedMeshRenderer->SetMaterial(2, std::move(mtl2));
	pSkinnedMeshRenderer->SetMaterial(3, std::move(mtl_mag));
}

static void InitSkinnedMeshUSP(GameResources* pScriptGameResources, SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	auto md = pScriptGameResources->GetModel(L"usp");
	auto& arma = md.m_armature;
	auto& mesh = md.m_skinnedMesh;
	auto mtl0 = pScriptGameResources->GetMaterial(L"usp_mtl0");
	pSkinnedMeshRenderer->SetMesh(mesh);
	pSkinnedMeshRenderer->SetArmature(arma);
	pSkinnedMeshRenderer->SetMaterial(0, std::move(mtl0));
}

static void InitSkinnedMeshB92fsb(GameResources* pScriptGameResources, SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	auto md = pScriptGameResources->GetModel(L"b92fsb");
	auto& arma = md.m_armature;
	auto& mesh = md.m_skinnedMesh;
	auto mtl0 = pScriptGameResources->GetMaterial(L"b92fsb_mtl0");
	auto mtl1 = pScriptGameResources->GetMaterial(L"b92fsb_mtl1");
	auto mtl2 = pScriptGameResources->GetMaterial(L"b92fsb_mtl2");
	pSkinnedMeshRenderer->SetMesh(mesh);
	pSkinnedMeshRenderer->SetArmature(arma);
	pSkinnedMeshRenderer->SetMaterial(0, std::move(mtl0));
	pSkinnedMeshRenderer->SetMaterial(1, std::move(mtl1));
	pSkinnedMeshRenderer->SetMaterial(2, std::move(mtl2));
}

PlayerController::PlayerController(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_processingInput(true)
	, m_isStand(true)
	, m_isGround(false)
	, m_groundCheckSweepDist(0.0f)
	, m_stepHeight(0.2f)
	, m_slope(0.707106f)
	, m_velocityY(0.0f)
	, m_primaryWeaponKey()
	, m_secondaryWeaponKey()
	, m_equipState(EquipmentState::None)
	, m_primaryWeaponState(WeaponState::None)
	, m_secondaryWeaponState(WeaponState::None)
	, m_needUpdateArmsWeaponAnim(false)
	, m_remainingTimeUpdateWeaponState(0.0f)
	, m_primaryWeaponReadyRemainingTime(0.0f)
	, m_secondaryWeaponReadyRemainingTime(0.0f)
{
	// 점프 초기속도 계산
	// 등가속도 공식 3번째
	// 2as = v^2 - v0^2
	// v^2 = v0^2 + 2as
	// 최고점에서는 속도가 0이므로, 0 = v0^2 + 2as
	// 여기서 a = -g, s = h(점프 높이)
	// v0^2 = 2gh, 따라서 v0(초기속도) = sqrt(2gh)로 정리된다.

	float jumpHeight = 0.8f;
	float g = abs(Physics::GetInstance()->GetGravity().y * 2);
	m_jumpVelocityY = std::sqrt(2.0f * g * jumpHeight);
}

void PlayerController::Awake()
{
	m_equipState = EquipmentState::None;

	// GameResources 오브젝트 검색 및 스크립트 저장
	GameObjectHandle hGameObjectGameResources = GameObject::Find(GO_GAME_RESOURCES_NAME);
	assert(hGameObjectGameResources.IsValid());

	m_hScriptGameResources = hGameObjectGameResources.ToPtr()->GetComponent<GameResources>();
	assert(m_hScriptGameResources.IsValid());

	GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();
	m_spGroundCheckSweepCollider = pScriptGameResources->GetGroundCheckCollider();

	// Rigidbody 컴포넌트 추가
	auto spCharacterCollider = pScriptGameResources->GetCharacterCollider();
	m_groundCheckSweepDist = spCharacterCollider->GetHeight() / 2.0f + pScriptGameResources->GetGroundCheckColliderSubtractFactor();

	ComponentHandle<Rigidbody> hRigidbodyPlayer = m_pGameObject->AddComponent<Rigidbody>(spCharacterCollider, XMFLOAT3(0.0f, spCharacterCollider->GetTotalHeight() / 2.0f, 0.0f));
	m_hRigidbodyPlayer = hRigidbodyPlayer;
	Rigidbody* pPlayerRigidbody = hRigidbodyPlayer.ToPtr();
	pPlayerRigidbody->SetBodyType(RigidbodyType::Kinematic);

	XMMATRIX colliderLocalTransform;
	colliderLocalTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&pPlayerRigidbody->GetColliderLocalRot()));
	colliderLocalTransform.r[3] = XMVectorSetW(XMLoadFloat3(&pPlayerRigidbody->GetColliderLocalPos()), 1.0f);
	XMStoreFloat4x4A(&m_colliderLocalTransform, colliderLocalTransform);


	// 자식 오브젝트로 카메라 추가
	GameObjectHandle hGameObjectCamera = Runtime::GetInstance()->CreateGameObject();
	m_hGameObjectCamera = hGameObjectCamera;
	GameObject* pGameObjectCamera = hGameObjectCamera.ToPtr();
	pGameObjectCamera->m_transform.SetParent(&m_pGameObject->m_transform);
	pGameObjectCamera->m_transform.SetPositionY(spCharacterCollider->GetTotalHeight() - 0.1f);
	ComponentHandle<Camera> hCamera = pGameObjectCamera->AddComponent<Camera>();
	Camera* pCamera = hCamera.ToPtr();
	pCamera->SetDepth(0);
	pCamera->SetFieldOfView(82);
	pCamera->SetClippingPlanes(0.1f, 300.0f);


	// FPS 팔 생성
	GameObjectHandle hGameObjectArms = Runtime::GetInstance()->CreateGameObject(L"fpsarms");
	m_hGameObjectArms = hGameObjectArms;
	GameObject* pGameObjectArms = hGameObjectArms.ToPtr();
	pGameObjectArms->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectArms->m_transform.SetPosition(FPSARM_POS);
	ComponentHandle<SkinnedMeshRenderer> hArmsSkinnedMeshRenderer = pGameObjectArms->AddComponent<SkinnedMeshRenderer>();
	m_hArmsSkinnedMeshRenderer = hArmsSkinnedMeshRenderer;
	SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = hArmsSkinnedMeshRenderer.ToPtr();
	InitSkinnedMeshStevenArms(pScriptGameResources, pArmsSkinnedMeshRenderer);

	// 재생할 FPS 팔 애니메이션
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


	// 무기 오브젝트 생성
	m_hGameObjectPrimaryWeapon = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectPrimaryWeapon = m_hGameObjectPrimaryWeapon.ToPtr();
	pGameObjectPrimaryWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectPrimaryWeapon->m_transform.SetPosition(PRIMARY_WEAPON_PV_OFFSET);
	m_hPrimaryWeaponSkinnedMeshRenderer = pGameObjectPrimaryWeapon->AddComponent<SkinnedMeshRenderer>();
	auto pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
	InitSkinnedMeshM16A1(pScriptGameResources, pPrimaryWeaponSkinnedMeshRenderer);
	pGameObjectPrimaryWeapon->SetActive(false);

	m_hGameObjectSecondaryWeapon = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectSecondaryWeapon = m_hGameObjectSecondaryWeapon.ToPtr();
	pGameObjectSecondaryWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectSecondaryWeapon->m_transform.SetPosition(SECONDARY_WEAPON_PV_OFFSET);
	m_hSecondaryWeaponSkinnedMeshRenderer = pGameObjectSecondaryWeapon->AddComponent<SkinnedMeshRenderer>();
	auto pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
	InitSkinnedMeshUSP(pScriptGameResources, pSecondaryWeaponSkinnedMeshRenderer);
	pGameObjectSecondaryWeapon->SetActive(false);
}

void PlayerController::Update()
{
	const float dt = Time::GetInstance()->GetDeltaTime();

	if (m_needUpdateArmsWeaponAnim)
		m_remainingTimeUpdateWeaponState -= dt;

	if (m_primaryWeaponReadyRemainingTime > 0.0f)
		m_primaryWeaponReadyRemainingTime -= dt;

	if (m_secondaryWeaponReadyRemainingTime > 0.0f)
		m_secondaryWeaponReadyRemainingTime -= dt;

	float remainingTimeUpdateArmsWeaponAnim = m_remainingTimeUpdateWeaponState;
	if (remainingTimeUpdateArmsWeaponAnim <= 0.0f)
	{
		switch (m_equipState)
		{
		case EquipmentState::Primary:
			if (m_primaryWeaponState != WeaponState::Ready)
			{
				// 만약 탄창에 탄이 다 떨어졌고, Reserved Ammo가 있는 경우라면 ChangePrimaryWeaponState를 Reloading으로.
				// 그게 아닌 모든 경우에는 Ready 상태로 변경하면 된다.

				// 지금은 테스트 코드로 무조건 Ready 상태로 설정
				ChangePrimaryWeaponState(WeaponState::Ready);
			}
			break;
		case EquipmentState::Secondary:
			if (m_secondaryWeaponState != WeaponState::Ready)
			{
				// 만약 탄창에 탄이 다 떨어졌고, Reserved Ammo가 있는 경우라면 ChangeSecondaryWeaponState를 Reloading으로.
				// 그게 아닌 모든 경우에는 Ready 상태로 변경하면 된다.

				// 지금은 테스트 코드로 무조건 Ready 상태로 설정
				ChangeSecondaryWeaponState(WeaponState::Ready);
			}
			break;
		default:
			break;
		}
	}

	// 메뉴창 등
	if (!m_processingInput)
		return;

	// 무기 드로잉 처리
	if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_1))
	{
		if (m_equipState != EquipmentState::Primary)
		{
			DrawPrimaryWeapon();
		}
	}

	if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_2))
	{
		if (m_equipState != EquipmentState::Secondary)
		{
			DrawSecondaryWeapon();
		}
	}


	// 장전 처리
	if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_R))
	{
		switch (m_equipState)
		{
		case EquipmentState::Primary:
			if (m_primaryWeaponState == WeaponState::Ready)
				ChangePrimaryWeaponState(WeaponState::Reloading);
			break;
		case EquipmentState::Secondary:
			if (m_secondaryWeaponState == WeaponState::Ready)
				ChangeSecondaryWeaponState(WeaponState::Reloading);
			break;
		default:
			break;
		}
	}


	constexpr float primary_weapon_rpm = 600.0f;
	constexpr float secondary_weapon_rpm = 320.0f;
	constexpr float primary_weapon_spr = 1.0f / (primary_weapon_rpm / 60.0f);
	constexpr float secondary_weapon_spr = 1.0f / (secondary_weapon_rpm / 60.0f);
	// 사격 처리
	if (Input::GetInstance()->GetMouseButton(MouseButton::Left))
	{
		switch (m_equipState)
		{
		case EquipmentState::Primary:
			// 탄이 없으면 break;
			
			if (m_primaryWeaponState == WeaponState::Ready)
			{
				ChangePrimaryWeaponState(WeaponState::Firing);
				m_primaryWeaponReadyRemainingTime = primary_weapon_spr;
			}
			else if (m_primaryWeaponState == WeaponState::Firing && m_primaryWeaponReadyRemainingTime <= 0.0f)
			{
				ChangePrimaryWeaponState(WeaponState::Firing);
				m_primaryWeaponReadyRemainingTime = primary_weapon_spr;
			}
			break;
		case EquipmentState::Secondary:
			// 탄이 없으면 break;
			
			if (m_secondaryWeaponState == WeaponState::Ready)
			{
				ChangeSecondaryWeaponState(WeaponState::Firing);
				m_secondaryWeaponReadyRemainingTime = secondary_weapon_spr;
			}
			else if (m_secondaryWeaponState == WeaponState::Firing && m_secondaryWeaponReadyRemainingTime <= 0.0f)
			{
				ChangeSecondaryWeaponState(WeaponState::Firing);
				m_secondaryWeaponReadyRemainingTime = secondary_weapon_spr;
			}
			break;
		default:
			break;
		}
	}


	const int32_t mx = Input::GetInstance()->GetMouseAxisHorizontal();
	const int32_t my = Input::GetInstance()->GetMouseAxisVertical();
	if (mx != 0)
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

	if (my != 0)
	{
		GameObject* pGameObjectCamera = m_hGameObjectCamera.ToPtr();
		XMVECTOR cameraObjectLocalRot = pGameObjectCamera->m_transform.GetRotation();
		XMVECTOR temp = Math::QuaternionToEuler(cameraObjectLocalRot);
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, temp);

		rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
		rotationEuler.x = Math::Clamp(rotationEuler.x, XMConvertToRadians(-HEAD_CLAMP_ANGLE), XMConvertToRadians(+HEAD_CLAMP_ANGLE));
		rotationEuler.y = 0.0f;
		rotationEuler.z = 0.0f;

		pGameObjectCamera->m_transform.SetRotationEuler(rotationEuler);
	}
}

void PlayerController::FixedUpdate()
{
	const float speed = Input::GetInstance()->GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED;
	const float dt = Time::GetInstance()->GetFixedDeltaTime();

	XMVECTOR qWorldRotation = m_pGameObject->m_transform.GetWorldRotation();
	XMVECTOR vForward = XMVector3Normalize(XMVectorSetY(XMVector3Rotate(Vector3::Forward(), qWorldRotation), 0.0f)); // Y성분 제거

	XMVECTOR vFinalMove = XMVectorZero(); // 이동해야할 변위량 누적


	if (m_processingInput)
	{
		// 1. 방향키에 따른 입력 누적
		XMVECTOR vForwardMove = vForward;
		XMVECTOR vRightMove = XMVector3Cross(Vector3::Up(), vForward);

		if (Input::GetInstance()->GetKey(KEY_W))
			vFinalMove = XMVectorAdd(vFinalMove, vForwardMove);

		if (Input::GetInstance()->GetKey(KEY_S))
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vForwardMove));

		if (Input::GetInstance()->GetKey(KEY_A))
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vRightMove));

		if (Input::GetInstance()->GetKey(KEY_D))
			vFinalMove = XMVectorAdd(vFinalMove, vRightMove);

		vFinalMove = XMVectorScale(XMVector3Normalize(vFinalMove), speed * dt);
	}

	// 2. 중력에 의한 이동량 누적
	if (m_isGround)
	{
		m_velocityY = 0.0f;
		if (Input::GetInstance()->GetKey(KEYCODE::KEY_SPACE))
			m_velocityY = m_jumpVelocityY;
	}
	else
	{
		m_velocityY += Physics::GetInstance()->GetGravity().y * 2 * dt;
	}

	XMVECTOR vGravityMove = XMVectorSet(0.0f, m_velocityY * dt, 0.0f, 0.0f);
	vFinalMove = XMVectorAdd(vFinalMove, vGravityMove);

	// Sweep 기반 이동
	Rigidbody* pPlayerRigidbody = m_hRigidbodyPlayer.ToPtr();
	XMMATRIX colliderLocalTransform = XMLoadFloat4x4A(&m_colliderLocalTransform);

	XMVECTOR vRemainingMove = vFinalMove;

	constexpr size_t MAX_ITER = 4;
	constexpr float MOVE_EPSILON = 0.001f;
	wprintf(L"########################\n");
	for (size_t i = 0; i < MAX_ITER; ++i)
	{
		if (XMVectorGetX(XMVector3Length(vRemainingMove)) < MOVE_EPSILON)
			break;

		XMMATRIX colliderWorldTransform = XMMatrixMultiply(colliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());

		XMFLOAT3A remainingMove;
		XMStoreFloat3A(&remainingMove, vRemainingMove);
		auto sweepResult = Physics::GetInstance()->CapsuleSweepTest(colliderWorldTransform, remainingMove, static_cast<CapsuleCollider*>(m_hRigidbodyPlayer.ToPtr()->GetCollider().get()));

		auto selfIter = std::remove_if(
			sweepResult.begin(),
			sweepResult.end(),
			[pPlayerRigidbody](const SweepHit& hit)
			{
				return hit.m_pHitObject == pPlayerRigidbody;
			}
		);
		sweepResult.erase(selfIter, sweepResult.end());

		if (sweepResult.size() == 0)
		{
			m_pGameObject->m_transform.Translate(vRemainingMove);
			break;
		}

		std::sort(
			sweepResult.begin(),
			sweepResult.end(),
			[](const SweepHit& a, const SweepHit& b)
			{
				return a.m_hitFraction < b.m_hitFraction;
			}
		);

		XMVECTOR vMovePart = vRemainingMove;
		const float closestHitFraction = sweepResult[0].m_hitFraction;

		vMovePart = XMVectorScale(vMovePart, closestHitFraction);
		m_pGameObject->m_transform.Translate(vMovePart);

		vRemainingMove = XMVectorScale(vRemainingMove, 1.0f - closestHitFraction);

		for (const auto& hitItem : sweepResult)
		{
			wprintf(L"hit fraction: %f\n", hitItem.m_hitFraction);
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

	wprintf(L"########################\n");

	// 마지막으로 아래방향 sweep으로 m_isGround 업데이트
	XMMATRIX colliderWorldTransform = XMMatrixMultiply(colliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());
	const XMFLOAT3 groundCheckSweepVector(0.0f, -m_groundCheckSweepDist - 0.05f, 0.0f);
	auto ret = Physics::GetInstance()->SphereSweepTest(colliderWorldTransform, groundCheckSweepVector, m_spGroundCheckSweepCollider.get());
	bool hit = false;
	for (const auto& item : ret)
	{
		if (item.m_pHitObject == pPlayerRigidbody)
			continue;

		const float dot = XMVectorGetX(XMVector3Dot(Vector3::Up(), XMVector3Normalize(XMLoadFloat3(&item.m_hitNormalWorld))));
		if (dot > m_slope)
		{
			hit = true;
			break;
		}
	}

	m_isGround = hit;
}

void PlayerController::ChangePrimaryWeaponState(WeaponState state)
{
	switch (state)
	{
	case WeaponState::None:
	{
		m_primaryWeaponState = WeaponState::None;
		// 
		SkinnedMeshRenderer* pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
		pPrimaryWeaponSkinnedMeshRenderer->StopAnimation();
		// 
		SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
		pArmsSkinnedMeshRenderer->StopAnimation();

		// m_remainingTimeUpdateWeaponState = 0.0f;
		m_needUpdateArmsWeaponAnim = false;
		break;
	}
	case WeaponState::Drawing:
	{
		m_primaryWeaponState = WeaponState::Drawing;

		// 
		const char* primaryWeaponDrawAnimName = "m16a1_draw";
		const char* primaryWeaponDrawArmsAnimName = "arms_draw_m16a1";
		SkinnedMeshRenderer* pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
		pPrimaryWeaponSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawAnimName, false);
		const Armature* pPrimaryWeaponArmature = pPrimaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

		// 
		SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
		pArmsSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawArmsAnimName, false);

		m_remainingTimeUpdateWeaponState = pPrimaryWeaponArmature->GetAnimation(primaryWeaponDrawAnimName)->GetDuration();
		m_needUpdateArmsWeaponAnim = true;
		break;
	}
	case WeaponState::Reloading:
	{
		m_primaryWeaponState = WeaponState::Reloading;

		// 
		const char* primaryWeaponDrawAnimName = "m16a1_reload";
		const char* primaryWeaponDrawArmsAnimName = "arms_reload_m16a1";
		SkinnedMeshRenderer* pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
		pPrimaryWeaponSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawAnimName, false);
		const Armature* pPrimaryWeaponArmature = pPrimaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

		// 
		SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
		pArmsSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawArmsAnimName, false);

		m_remainingTimeUpdateWeaponState = pPrimaryWeaponArmature->GetAnimation(primaryWeaponDrawAnimName)->GetDuration();
		m_needUpdateArmsWeaponAnim = true;
		break;
	}
	case WeaponState::Firing:
	{
		m_primaryWeaponState = WeaponState::Firing;

		// 
		const char* primaryWeaponDrawAnimName = "m16a1_shoot";
		const char* primaryWeaponDrawArmsAnimName = "arms_shoot_m16a1";
		SkinnedMeshRenderer* pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
		pPrimaryWeaponSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawAnimName, false);
		const Armature* pPrimaryWeaponArmature = pPrimaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

		// 
		SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
		pArmsSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawArmsAnimName, false);

		m_remainingTimeUpdateWeaponState = pPrimaryWeaponArmature->GetAnimation(primaryWeaponDrawAnimName)->GetDuration();
		m_needUpdateArmsWeaponAnim = true;
		break;
	}
	case WeaponState::Ready:
	{
		m_primaryWeaponState = WeaponState::Ready;
		// 
		const char* primaryWeaponDrawAnimName = "m16a1_idle";
		const char* primaryWeaponDrawArmsAnimName = "arms_idle_m16a1";
		SkinnedMeshRenderer* pPrimaryWeaponSkinnedMeshRenderer = m_hPrimaryWeaponSkinnedMeshRenderer.ToPtr();
		pPrimaryWeaponSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawAnimName, true);

		// 
		SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
		pArmsSkinnedMeshRenderer->PlayAnimation(primaryWeaponDrawArmsAnimName, true);

		// m_remainingTimeUpdateWeaponState = 0.0f;
		m_needUpdateArmsWeaponAnim = false;
		break;
	}
	}
}

void PlayerController::ChangeSecondaryWeaponState(WeaponState state)
{
	switch (state)
	{
	case WeaponState::None:
		{
			m_secondaryWeaponState = WeaponState::None;
			// 
			SkinnedMeshRenderer* pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
			pSecondaryWeaponSkinnedMeshRenderer->StopAnimation();
			// 
			SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
			pArmsSkinnedMeshRenderer->StopAnimation();

			// m_remainingTimeUpdateWeaponState = 0.0f;
			m_needUpdateArmsWeaponAnim = false;
			break;
		}
		case WeaponState::Drawing:
		{
			m_secondaryWeaponState = WeaponState::Drawing;

			// 
			const char* secondaryWeaponDrawAnimName = "usp_draw";
			const char* secondaryWeaponDrawArmsAnimName = "arms_draw_usp";
			SkinnedMeshRenderer* pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
			pSecondaryWeaponSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawAnimName, false);
			const Armature* pSecondaryWeaponArmature = pSecondaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

			// 
			SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
			pArmsSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawArmsAnimName, false);

			m_remainingTimeUpdateWeaponState = pSecondaryWeaponArmature->GetAnimation(secondaryWeaponDrawAnimName)->GetDuration();
			m_needUpdateArmsWeaponAnim = true;
			break;
		}
		case WeaponState::Reloading:
		{
			m_secondaryWeaponState = WeaponState::Reloading;

			// 
			const char* secondaryWeaponDrawAnimName = "usp_reload";
			const char* secondaryWeaponDrawArmsAnimName = "arms_reload_usp";
			SkinnedMeshRenderer* pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
			pSecondaryWeaponSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawAnimName, false);
			const Armature* pSecondaryWeaponArmature = pSecondaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

			// 
			SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
			pArmsSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawArmsAnimName, false);

			m_remainingTimeUpdateWeaponState = pSecondaryWeaponArmature->GetAnimation(secondaryWeaponDrawAnimName)->GetDuration();
			m_needUpdateArmsWeaponAnim = true;
			break;
		}
		case WeaponState::Firing:
		{
			m_secondaryWeaponState = WeaponState::Firing;

			// 
			const char* secondaryWeaponDrawAnimName = "usp_shoot";
			const char* secondaryWeaponDrawArmsAnimName = "arms_shoot_usp";
			SkinnedMeshRenderer* pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
			pSecondaryWeaponSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawAnimName, false);
			const Armature* pSecondaryWeaponArmature = pSecondaryWeaponSkinnedMeshRenderer->GetArmaturePtr();

			// 
			SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
			pArmsSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawArmsAnimName, false);

			m_remainingTimeUpdateWeaponState = pSecondaryWeaponArmature->GetAnimation(secondaryWeaponDrawAnimName)->GetDuration();
			m_needUpdateArmsWeaponAnim = true;
			break;
		}
		case WeaponState::Ready:
		{
			m_secondaryWeaponState = WeaponState::Ready;
			// 
			const char* secondaryWeaponDrawAnimName = "usp_idle";
			const char* secondaryWeaponDrawArmsAnimName = "arms_idle_usp";
			SkinnedMeshRenderer* pSecondaryWeaponSkinnedMeshRenderer = m_hSecondaryWeaponSkinnedMeshRenderer.ToPtr();
			pSecondaryWeaponSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawAnimName, true);

			// 
			SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
			pArmsSkinnedMeshRenderer->PlayAnimation(secondaryWeaponDrawArmsAnimName, true);

			// m_remainingTimeUpdateWeaponState = 0.0f;
			m_needUpdateArmsWeaponAnim = false;
			break;
		}
	}
}

void PlayerController::DrawPrimaryWeapon()
{
	m_equipState = EquipmentState::Primary;

	
	m_hGameObjectSecondaryWeapon.ToPtr()->SetActive(false);
	m_hGameObjectPrimaryWeapon.ToPtr()->SetActive(true);
	ChangeSecondaryWeaponState(WeaponState::None);
	ChangePrimaryWeaponState(WeaponState::Drawing);
}

void PlayerController::DrawSecondaryWeapon()
{
	m_equipState = EquipmentState::Secondary;

	m_hGameObjectPrimaryWeapon.ToPtr()->SetActive(false);
	m_hGameObjectSecondaryWeapon.ToPtr()->SetActive(true);
	ChangePrimaryWeaponState(WeaponState::None);
	ChangeSecondaryWeaponState(WeaponState::Drawing);
}

/*
void PlayerController::FixedUpdate()
{
	const float speed = Input::GetInstance()->GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED;
	const float dt = Time::GetInstance()->GetFixedDeltaTime();

	XMVECTOR qWorldRotation = m_pGameObject->m_transform.GetWorldRotation();
	XMVECTOR vForward = XMVector3Normalize(XMVectorSetY(XMVector3Rotate(Vector3::Forward(), qWorldRotation), 0.0f)); // Y성분 제거

	XMVECTOR vFinalMove = XMVectorZero(); // 이동해야할 변위량 누적

	// 1. 방향키에 따른 입력 누적
	XMVECTOR vForwardMove = XMVectorScale(vForward, speed * dt);
	XMVECTOR vRightMove = XMVectorScale(XMVector3Cross(Vector3::Up(), vForward), speed * dt);

	if (Input::GetInstance()->GetKey(KEY_W))
		vFinalMove = XMVectorAdd(vFinalMove, vForwardMove);

	if (Input::GetInstance()->GetKey(KEY_S))
		vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vForwardMove));

	if (Input::GetInstance()->GetKey(KEY_A))
		vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vRightMove));

	if (Input::GetInstance()->GetKey(KEY_D))
		vFinalMove = XMVectorAdd(vFinalMove, vRightMove);


	// 2. 중력에 의한 이동량 누적
	if (m_isGround)
	{
		m_velocityY = 0.0f;
		if (Input::GetInstance()->GetKey(KEYCODE::KEY_SPACE))
		{
			m_velocityY = m_jumpVelocityY;
		}
	}
	else
	{
		m_velocityY += Physics::GetInstance()->GetGravity().y * dt;
	}

	XMVECTOR vGravityMove = XMVectorSet(0.0f, m_velocityY * dt, 0.0f, 0.0f);
	vFinalMove = XMVectorAdd(vFinalMove, vGravityMove);

	// Sweep 기반 이동
	Rigidbody* pPlayerRigidbody = m_hRigidbodyPlayer.ToPtr();
	XMMATRIX colliderLocalTransform = XMLoadFloat4x4A(&m_colliderLocalTransform);

	XMVECTOR vRemainingMove = vFinalMove;

	constexpr size_t MAX_ITER = 4;
	constexpr float MOVE_EPSILON = 0.001f;
	wprintf(L"########################\n");
	for (size_t i = 0; i < MAX_ITER; ++i)
	{
		if (XMVectorGetX(XMVector3Length(vRemainingMove)) < MOVE_EPSILON)
			break;

		XMMATRIX colliderWorldTransform = XMMatrixMultiply(colliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());

		XMFLOAT3A remainingMove;
		XMStoreFloat3A(&remainingMove, vRemainingMove);
		auto sweepResult = Physics::GetInstance()->CapsuleSweepTest(colliderWorldTransform, remainingMove, static_cast<CapsuleCollider*>(m_hRigidbodyPlayer.ToPtr()->GetCollider().get()));

		auto selfIter = std::remove_if(
			sweepResult.begin(),
			sweepResult.end(),
			[pPlayerRigidbody](const SweepHit& hit)
			{
				return hit.m_pHitObject == pPlayerRigidbody;
			}
		);
		sweepResult.erase(selfIter, sweepResult.end());

		if (sweepResult.size() == 0)
		{
			m_pGameObject->m_transform.Translate(vRemainingMove);
			break;
		}

		std::sort(
			sweepResult.begin(),
			sweepResult.end(),
			[](const SweepHit& a, const SweepHit& b)
			{
				return a.m_hitFraction < b.m_hitFraction;
			}
		);

		XMVECTOR vMovePart = vRemainingMove;
		const float closestHitFraction = sweepResult[0].m_hitFraction;
		for (const auto& hitItem : sweepResult)
		{
			wprintf(L"hit fraction: %f\n", hitItem.m_hitFraction);
			if (hitItem.m_hitFraction > closestHitFraction + 0.01f)
				break;

			XMVECTOR vHitNormalWorld = XMVector3Normalize(XMLoadFloat3(&hitItem.m_hitNormalWorld));
			float dot = XMVectorGetX(XMVector3Dot(vMovePart, vHitNormalWorld));
			if (dot < 0.0f)
			{
				vMovePart = XMVectorSubtract(vMovePart, XMVectorScale(vHitNormalWorld, dot));
			}
		}

		m_pGameObject->m_transform.Translate(vMovePart);

		vRemainingMove = XMVectorZero();	// vRemainingMove를 어떻게 계산해야 하지?
	}

	wprintf(L"########################\n");

	// 마지막으로 아래방향 sweep으로 m_isGround 업데이트
	XMMATRIX colliderWorldTransform = XMMatrixMultiply(colliderLocalTransform, m_pGameObject->m_transform.GetWorldTransformMatrix());
	const XMFLOAT3 groundCheckSweepVector(0.0f, -m_groundCheckSweepDist - 0.05f, 0.0f);
	auto ret = Physics::GetInstance()->SphereSweepTest(colliderWorldTransform, groundCheckSweepVector, m_spGroundCheckSweepCollider.get());
	bool hit = false;

	for (const auto& item : ret)
	{
		if (item.m_pHitObject == pPlayerRigidbody)
			continue;

		const float dot = XMVectorGetX(XMVector3Dot(Vector3::Up(), XMVector3Normalize(XMLoadFloat3(&item.m_hitNormalWorld))));
		if (dot > m_slope)
		{
			hit = true;
			break;
		}
	}

	m_isGround = hit;
}

*/
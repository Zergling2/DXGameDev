#include "Player.h"
#include "..\Resource/GlobalGameObjects.h"
#include "..\Resource\Arms.h"
#include "..\Resource\WeaponInfo.h"
#include "..\Resource\Weapon.h"
#include "GameResources.h"
#include "GameUIManager.h"
#include <algorithm>

using namespace ze;

static const XMFLOAT3 FPSARM_POS(0.0f, -0.2f, -0.06f);
static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.1f, -0.04f, 0.23f);
static const XMFLOAT3 PRIMARY_WEAPON_PV_OFFSET(FPSARM_POS.x + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + PRIMARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);
static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE(0.04f, -0.01f, 0.41f);
static const XMFLOAT3 SECONDARY_WEAPON_PV_OFFSET(FPSARM_POS.x + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.x, FPSARM_POS.y + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.y, FPSARM_POS.z + SECONDARY_WEAPON_PV_OFFSET_IN_FPSARM_SPACE.z);

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 4.2f;
constexpr float WALK_SPEED = SPEED * 0.5f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

Player::Player(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_processingInput(true)
	, m_isStand(true)
	, m_isGround(false)
	, m_groundCheckSweepDist(0.0f)
	, m_stepHeight(0.2f)
	, m_slope(0.707106f)	// 약 45도
	, m_velocityY(0.0f)
	, m_weapons()
	, m_currWeaponSlot(WeaponSlot::Secondary)
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

void Player::Awake()
{
	std::function<void(WeaponEvent, const WeaponViewInfo*)> weaponEventHandler =
		[this](WeaponEvent we, const WeaponViewInfo* pWeaponViewInfo)
		{
			// 시각 효과만 처리. 논리 로직은 무기 및 무기 상태에서 전담!!

			bool result;

			switch (we)
			{
			case WeaponEvent::DrawingStarted:
				m_hArmsSkinnedMeshRenderer.ToPtr()->PlayAnimation(pWeaponViewInfo->GetArmsDrawAnimName(), false);
				m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->PlayAnimation(pWeaponViewInfo->GetDrawAnimName(), false);
				break;
			case WeaponEvent::ReloadingStarted:
				wprintf(L"reloading started!\n");
				result = m_hArmsSkinnedMeshRenderer.ToPtr()->PlayAnimation(pWeaponViewInfo->GetArmsReloadAnimName(), false);
				assert(result);
				result = m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->PlayAnimation(pWeaponViewInfo->GetReloadAnimName(), false);
				assert(result);
				break;
			case WeaponEvent::ReloadComplete:
				break;
			case WeaponEvent::FiringStarted:
				m_hArmsSkinnedMeshRenderer.ToPtr()->PlayAnimation(pWeaponViewInfo->GetArmsFireAnimName(), false);
				m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->PlayAnimation(pWeaponViewInfo->GetFireAnimName(), false);
				break;
			case WeaponEvent::IdlingStarted:
				m_hArmsSkinnedMeshRenderer.ToPtr()->PlayAnimation(pWeaponViewInfo->GetArmsIdleAnimName(), true);
				m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->PlayAnimation(pWeaponViewInfo->GetIdleAnimName(), true);
				break;
			case WeaponEvent::EmptyFiringStarted:
				break;
			default:
				break;
			}
		};

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

	m_pArmsViewInfo = pScriptGameResources->GetArmsViewinfo(L"steven");

	// 무기 오브젝트 생성
	m_hGameObjectWeapons[0] = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectPrimaryWeapon = m_hGameObjectWeapons[0].ToPtr();
	pGameObjectPrimaryWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectPrimaryWeapon->m_transform.SetPosition(PRIMARY_WEAPON_PV_OFFSET);
	m_hWeaponSkinnedMeshRenderers[0] = pGameObjectPrimaryWeapon->AddComponent<SkinnedMeshRenderer>();
	auto pPrimaryWeaponSkinnedMeshRenderer = m_hWeaponSkinnedMeshRenderers[0].ToPtr();
	pGameObjectPrimaryWeapon->SetActive(false);

	m_hGameObjectWeapons[1] = Runtime::GetInstance()->CreateGameObject();
	GameObject* pGameObjectSecondaryWeapon = m_hGameObjectWeapons[1].ToPtr();
	pGameObjectSecondaryWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);
	pGameObjectSecondaryWeapon->m_transform.SetPosition(SECONDARY_WEAPON_PV_OFFSET);
	m_hWeaponSkinnedMeshRenderers[1] = pGameObjectSecondaryWeapon->AddComponent<SkinnedMeshRenderer>();
	auto pSecondaryWeaponSkinnedMeshRenderer = m_hWeaponSkinnedMeshRenderers[1].ToPtr();
	
	m_weapons[0] = std::make_shared<AutoRifle>(pScriptGameResources->GetWeaponViewInfo(L"m16a1"), pScriptGameResources->GetWeaponInfo(L"m16a1"));
	m_weapons[0]->SetWeaponEventHandler(weaponEventHandler);
	m_weapons[1] = std::make_shared<SemiAutoPistol>(pScriptGameResources->GetWeaponViewInfo(L"usp"), pScriptGameResources->GetWeaponInfo(L"usp"));
	m_weapons[1]->SetWeaponEventHandler(weaponEventHandler);



	CreateArmsView(m_pArmsViewInfo);
	CreateWeaponView(WeaponSlot::Primary, m_weapons[static_cast<size_t>(WeaponSlot::Primary)]->GetWeaponViewInfo());
	CreateWeaponView(WeaponSlot::Secondary, m_weapons[static_cast<size_t>(WeaponSlot::Secondary)]->GetWeaponViewInfo());

	m_currWeaponSlot = WeaponSlot::Secondary;
	m_weapons[static_cast<size_t>(m_currWeaponSlot)]->StartDraw();
}

void Player::Update()
{
	const float dt = Time::GetInstance()->GetDeltaTime();

	IWeapon* pCurrentWeapon = m_weapons[static_cast<size_t>(m_currWeaponSlot)].get();
	pCurrentWeapon->Update(dt);

	// 메뉴창 등
	if (m_processingInput)
	{
		// 무기 드로잉 처리
		if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_1))
		{
		}
		if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_2))
		{
		}

		// 장전 처리
		if (Input::GetInstance()->GetKeyDown(KEYCODE::KEY_R))
		{
			pCurrentWeapon->StartReload();
		}


		// 사격 처리
		if (Input::GetInstance()->GetMouseButton(MouseButton::Left))
		{
			pCurrentWeapon->StartFire();
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

	m_hScriptGameUIManager.ToPtr()->SetTextAmmoState(m_weapons[static_cast<size_t>(m_currWeaponSlot)]->GetAmmoStateText());
}

void Player::FixedUpdate()
{
	bool isShiftPressed = Input::GetInstance()->GetKey(KEY_LSHIFT);
	const float speed = isShiftPressed ? WALK_SPEED : SPEED;
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
		if (m_processingInput)
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

void Player::CreateArmsView(const ArmsViewInfo* pArmsViewInfo)
{
	SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();

	pArmsSkinnedMeshRenderer->SetMesh(pArmsViewInfo->GetMesh());
	pArmsSkinnedMeshRenderer->SetArmature(pArmsViewInfo->GetArmature());

	for (size_t i = 0; i < pArmsViewInfo->GetMaterials().size(); ++i)
		pArmsSkinnedMeshRenderer->SetMaterial(i, pArmsViewInfo->GetMaterials()[i]);

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
}

void Player::CreateWeaponView(WeaponSlot slot, const WeaponViewInfo* pWeaponViewInfo)
{
	if (slot == WeaponSlot::ElementCount)
		return;

	SkinnedMeshRenderer* pSkinnedMeshRendererWeapon = m_hWeaponSkinnedMeshRenderers[static_cast<size_t>(slot)].ToPtr();

	pSkinnedMeshRendererWeapon->SetMesh(pWeaponViewInfo->GetPVMesh());
	pSkinnedMeshRendererWeapon->SetArmature(pWeaponViewInfo->GetPVMeshArmature());

	for (size_t i = 0; i < pWeaponViewInfo->GetMaterials().size(); ++i)
		pSkinnedMeshRendererWeapon->SetMaterial(i, pWeaponViewInfo->GetMaterials()[i]);
}

/*
void Player::FixedUpdate()
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

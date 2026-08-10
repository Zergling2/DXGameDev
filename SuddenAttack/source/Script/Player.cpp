#include "Player.h"
#include "..\Resource\GlobalScriptGameObject.h"
#include "..\Resource\Arms.h"
#include "Weapon.h"
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
constexpr float SPEED = 5.0f;
constexpr float WALK_SPEED = SPEED * 0.5f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

Player::Player(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_processingInput(true)
	, m_isStand(true)
	, m_isMoving(false)
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
	, m_pArmsViewInfo(nullptr)
	, m_currWeaponSlot(WeaponSlot::Unknown)
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


	// GameResources 오브젝트 검색 및 스크립트 저장
	GameObjectHandle hGameObjectGameResources = GameObject::Find(GAME_RESOURCES_GAME_OBJECT_NAME);
	assert(hGameObjectGameResources.IsValid());

	m_hScriptGameResources = hGameObjectGameResources.ToPtr()->GetComponent<GameResources>();
	assert(m_hScriptGameResources.IsValid());

	GameResources* pScriptGameResources = m_hScriptGameResources.ToPtr();
	m_jumpSpeed = pScriptGameResources->GetJumpSpeed();
	m_spGroundCheckCollider = pScriptGameResources->GetGroundCheckCollider();

	m_maxSlope = 0.7071067f;	// cos(45도)

	// Rigidbody 컴포넌트 추가
	auto spCharacterCollider = pScriptGameResources->GetCharacterCollider();
	m_groundCheckSweepDistY = spCharacterCollider->GetTotalHeight() / 2.0f + 0.02f;

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


	// 카메라 컴포넌트 추가
	// (자식 오브젝트로 추가한다.)
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
	CreateArmsView(m_pArmsViewInfo);


	// 무기 오브젝트 생성
	for (size_t i = 0; i < _countof(m_hGameObjectWeapons); ++i)
	{
		m_hGameObjectWeapons[i] = Runtime::GetInstance()->CreateGameObject();
		GameObject* pGameObjectWeapon = m_hGameObjectWeapons[i].ToPtr();
		pGameObjectWeapon->m_transform.SetParent(&pGameObjectCamera->m_transform);
		pGameObjectWeapon->m_transform.SetPosition(PRIMARY_WEAPON_PV_OFFSET);
		m_hWeaponSkinnedMeshRenderers[i] = pGameObjectWeapon->AddComponent<SkinnedMeshRenderer>();
		auto pWeaponSkinnedMeshRenderer = m_hWeaponSkinnedMeshRenderers[i].ToPtr();

		assert(m_hWeaponSkinnedMeshRenderers[i].IsValid() && m_hArmsSkinnedMeshRenderer.IsValid() && m_hScriptGameUIManager.IsValid());
		m_hScriptWeapon[i] = pGameObjectWeapon->AddComponent<Weapon>();
		auto pScriptWeapon = m_hScriptWeapon[i].ToPtr();
		pScriptWeapon->m_hWeaponMeshRenderer = m_hWeaponSkinnedMeshRenderers[i];
		pScriptWeapon->m_hArmsMeshRenderer = m_hArmsSkinnedMeshRenderer;
		pScriptWeapon->m_hScriptGameUIManager = m_hScriptGameUIManager;

		pScriptWeapon->Undraw();
	}

	m_hScriptWeapon[static_cast<size_t>(WeaponSlot::Primary)].ToPtr()->Init(pScriptGameResources->GetWeaponDefinition(WeaponCode::M16), 24, 115);
	m_hScriptWeapon[static_cast<size_t>(WeaponSlot::Secondary)].ToPtr()->Init(pScriptGameResources->GetWeaponDefinition(WeaponCode::USP), 12, 24);

	m_currWeaponSlot = WeaponSlot::Secondary;
	m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->Draw();
}

void Player::Update()
{
	const float dt = Time::GetInstance()->GetDeltaTime();

	if (m_jumpCoolTime > 0.0f)
		m_jumpCoolTime = (std::max)(m_jumpCoolTime - dt, 0.0f);

	// 팔과 무기 흔들림 업데이트
	constexpr float BOUNCE_FREQ_WEIGHT_RUNNING = 9.0f;
	bool isRunning = m_isMoving && Input::GetInstance()->GetKey(Keycode::KEY_LSHIFT) == false;
	float targetBounceFreq = isRunning ? BOUNCE_FREQ_WEIGHT_RUNNING : BOUNCE_FREQ_WEIGHT_RUNNING * 0.5f;
	float targetAmpX = m_isMoving ? 0.01f : 0.0f;
	float targetAmpY = m_isMoving ? 0.02f : 0.0f;

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
	case WeaponSlot::Melee:
		// ...
		break;
	case WeaponSlot::Utility:
		// ...
		break;
	default:
		break;
	}

	if (m_processingInput)
	{
		// 무기 드로잉 처리
		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_1))
		{
			// 사망 시 리스폰 직전까지 m_currWeaponSlot를 WeaponSlot::Unknown으로 설정해야 함!
			
			if (m_currWeaponSlot != WeaponSlot::Primary)
			{
				const WeaponSlot oldWeaponSlot = m_currWeaponSlot;
				m_currWeaponSlot = WeaponSlot::Primary;

				if (oldWeaponSlot != WeaponSlot::Unknown)
				{
					Weapon* pOldScriptWeapon = m_hScriptWeapon[static_cast<size_t>(oldWeaponSlot)].ToPtr();
					pOldScriptWeapon->Undraw();
				}

				Weapon* pCurrScriptWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
				pCurrScriptWeapon->Draw();
			}
		}

		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_2))
		{
			if (m_currWeaponSlot != WeaponSlot::Secondary)
			{
				const WeaponSlot oldWeaponSlot = m_currWeaponSlot;
				m_currWeaponSlot = WeaponSlot::Secondary;

				if (oldWeaponSlot != WeaponSlot::Unknown)
				{
					Weapon* pOldScriptWeapon = m_hScriptWeapon[static_cast<size_t>(oldWeaponSlot)].ToPtr();
					pOldScriptWeapon->Undraw();
				}

				Weapon* pCurrScriptWeapon = m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr();
				pCurrScriptWeapon->Draw();
			}
		}

		// 장전 처리
		if (Input::GetInstance()->GetKeyDown(Keycode::KEY_R))
			m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->Reload();


		// 사격 처리
		if (Input::GetInstance()->GetMouseButton(MouseButton::Left))
			m_hScriptWeapon[static_cast<size_t>(m_currWeaponSlot)].ToPtr()->Fire();


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
}

void Player::FixedUpdate()
{
	bool isShiftPressed = Input::GetInstance()->GetKey(KEY_LSHIFT);
	const float speed = isShiftPressed ? WALK_SPEED : SPEED;
	const float dt = Time::GetInstance()->GetFixedDeltaTime();

	XMVECTOR qWorldRotation = m_pGameObject->m_transform.GetWorldRotation();
	XMVECTOR vForward = XMVector3Normalize(XMVectorSetY(XMVector3Rotate(Vector3::Forward(), qWorldRotation), 0.0f)); // Y성분 제거

	XMVECTOR vFinalMove = XMVectorZero(); // 이동해야할 변위량 누적

	m_isMoving = false;

	if (m_processingInput)
	{
		// 1. 방향키에 따른 입력 누적
		XMVECTOR vForwardMove = vForward;
		XMVECTOR vRightMove = XMVector3Cross(Vector3::Up(), vForward);


		if (Input::GetInstance()->GetKey(KEY_W))
		{
			m_isMoving = true;
			vFinalMove = XMVectorAdd(vFinalMove, vForwardMove);
		}

		if (Input::GetInstance()->GetKey(KEY_S))
		{
			m_isMoving = true;
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vForwardMove));
		}

		if (Input::GetInstance()->GetKey(KEY_A))
		{
			m_isMoving = true;
			vFinalMove = XMVectorAdd(vFinalMove, XMVectorNegate(vRightMove));
		}

		if (Input::GetInstance()->GetKey(KEY_D))
		{
			m_isMoving = true;
			vFinalMove = XMVectorAdd(vFinalMove, vRightMove);
		}

		const float finalMoveLen = XMVectorGetX(XMVector3Length(vFinalMove));
		if (finalMoveLen > 0.0f)
			vFinalMove = XMVectorScale(vFinalMove, 1.0f / finalMoveLen);

		vFinalMove = XMVectorScale(vFinalMove, speed * dt);
	}

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

		size_t collisionCount = Physics::GetInstance()->ConvexSweepTestAllNotIncludeExceptTrigger(
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

		for (const auto& hitItem : m_sweepResults)
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
	const XMFLOAT3 groundCheckSweepVector(0.0f, -m_groundCheckSweepDistY, 0.0f);

	Physics::GetInstance()->ConvexSweepTestAllNotIncludeExceptTrigger(m_sweepResults, groundCheckColliderWorldTransform, groundCheckSweepVector, m_spGroundCheckCollider.get(), pPlayerRigidbody);
	bool hit = false;
	for (const auto& item : m_sweepResults)
	{
		const float dot = XMVectorGetX(XMVector3Dot(Vector3::Up(), XMVector3Normalize(XMLoadFloat3(&item.m_hitNormalWorld))));
		if (dot > m_maxSlope)
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

#include "FirstPersonMovement.h"

using namespace ze;

constexpr float SENSITIVITY = 0.16f;
constexpr float SPEED = 4.0f;
constexpr float WALK_SPEED = SPEED * 0.25f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

void FirstPersonMovement::Update()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	const float dt = Time::GetInstance()->GetDeltaTime();
	XMVECTOR localRotation = pGameObject->m_transform.GetRotation();
	XMVECTOR worldForwardAxis = XMVector3Rotate(Math::Vector3::Forward(), localRotation);
	XMVECTOR worldRightAxis = XMVector3Rotate(Math::Vector3::Right(), localRotation);

	const int32_t mx = Input::GetInstance()->GetMouseAxisHorizontal();
	const int32_t my = Input::GetInstance()->GetMouseAxisVertical();
	if (mx != 0 || my != 0)
	{
		XMVECTOR temp = Math::QuaternionToEuler(localRotation);
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, temp);

		rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
		Math::Clamp(rotationEuler.x, XMConvertToRadians(-HEAD_CLAMP_ANGLE), XMConvertToRadians(+HEAD_CLAMP_ANGLE));
		rotationEuler.y += XMConvertToRadians(static_cast<float>(mx) * SENSITIVITY);
		rotationEuler.z = 0.0f;

		temp = XMLoadFloat3A(&rotationEuler);
		pGameObject->m_transform.SetRotationEuler(temp);
	}

	const float speed = dt * (Input::GetInstance()->GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED);

	if (Input::GetInstance()->GetKey(KEY_W))
		pGameObject->m_transform.Translate(worldForwardAxis * speed);

	if (Input::GetInstance()->GetKey(KEY_S))
		pGameObject->m_transform.Translate(worldForwardAxis * -speed);

	if (Input::GetInstance()->GetKey(KEY_A))
		pGameObject->m_transform.Translate(worldRightAxis * -speed);

	if (Input::GetInstance()->GetKey(KEY_D))
		pGameObject->m_transform.Translate(worldRightAxis * speed);

	if (Input::GetInstance()->GetKey(KEY_ESCAPE))
		Runtime::GetInstance()->Exit();

	bool weaponChange = false;
	size_t weaponIndex;
	if (Input::GetInstance()->GetKeyDown(KEY_1))
	{
		weaponChange = true;
		weaponIndex = 0;
	}
	if (Input::GetInstance()->GetKeyDown(KEY_2))
	{
		weaponChange = true;
		weaponIndex = 1;
	}
	if (Input::GetInstance()->GetKeyDown(KEY_3))
	{
		weaponChange = true;
		weaponIndex = 2;
	}
	if (Input::GetInstance()->GetKeyDown(KEY_4))
	{
		weaponChange = true;
		weaponIndex = 3;
	}

	if (weaponChange)
	{
		m_hWeapons[0].ToPtr()->SetActive(false);
		m_hWeapons[1].ToPtr()->SetActive(false);
		m_hWeapons[2].ToPtr()->SetActive(false);
		m_hWeapons[3].ToPtr()->SetActive(false);
		m_hWeapons[weaponIndex].ToPtr()->SetActive(true);
	}
}

#include "FirstPersonCamera.h"

using namespace pubg;
using namespace ze;

constexpr float SENSITIVITY = 0.16f;
constexpr float SPEED = 0.1f;
constexpr float WALK_SPEED = SPEED * 0.25f;

void FirstPersonCamera::Update()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	XMVECTOR localRotation = pGameObject->m_transform.GetLocalRotation();
	XMVECTOR worldForwardAxis = XMVector3Rotate(Math::Vector3::FORWARD, localRotation);
	XMVECTOR worldRightAxis = XMVector3Rotate(Math::Vector3::RIGHT, localRotation);

	const int32_t mx = Input::GetInstance()->GetMouseAxisHorizontal();
	const int32_t my = Input::GetInstance()->GetMouseAxisVertical();
	if (mx != 0 || my != 0)
	{
		XMVECTOR temp = Math::QuaternionToEuler(localRotation);
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, temp);

		rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
		Math::Clamp(rotationEuler.x, XMConvertToRadians(-85.0f), XMConvertToRadians(+85.0f));
		rotationEuler.y += XMConvertToRadians(static_cast<float>(mx) * SENSITIVITY);
		rotationEuler.z = 0.0f;

		temp = XMLoadFloat3A(&rotationEuler);
		temp = XMQuaternionRotationRollPitchYawFromVector(temp);
		pGameObject->m_transform.SetRotation(temp);
	}

	const float speed = Input::GetInstance()->GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED;

	if (Input::GetInstance()->GetKey(KEY_W))
		pGameObject->m_transform.Translate(worldForwardAxis * speed);

	if (Input::GetInstance()->GetKey(KEY_S))
		pGameObject->m_transform.Translate(worldForwardAxis * -speed);

	if (Input::GetInstance()->GetKey(KEY_A))
		pGameObject->m_transform.Translate(worldRightAxis * -speed);

	if (Input::GetInstance()->GetKey(KEY_D))
		pGameObject->m_transform.Translate(worldRightAxis * speed);



	if (Input::GetInstance()->GetKeyDown(KEY_APOSTROPHE))
		Runtime::GetInstance()->SetResolution(1600, 900, DISPLAY_MODE::WINDOWED);

	if (Input::GetInstance()->GetKeyDown(KEY_SEMICOLON))
		Runtime::GetInstance()->SetResolution(0, 0, DISPLAY_MODE::BORDERLESS_WINDOWED);

	if (Input::GetInstance()->GetKeyDown(KEY_U))
		this->Disable();

	if (Input::GetInstance()->GetKeyDown(KEY_Y))
		this->Enable();
}

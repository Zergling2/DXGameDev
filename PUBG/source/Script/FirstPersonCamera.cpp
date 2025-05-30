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
	XMVECTOR worldForwardAxis = XMVector3Rotate(LOCAL_FORWARD, localRotation);
	XMVECTOR worldRightAxis = XMVector3Rotate(LOCAL_RIGHT, localRotation);

	const int32_t mx = Input.GetMouseAxisHorizontal();
	const int32_t my = Input.GetMouseAxisVertical();
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

	const float speed = Input.GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED;

	if (Input.GetKey(KEY_W))
		pGameObject->m_transform.Translate(worldForwardAxis * speed);

	if (Input.GetKey(KEY_S))
		pGameObject->m_transform.Translate(worldForwardAxis * -speed);

	if (Input.GetKey(KEY_A))
		pGameObject->m_transform.Translate(worldRightAxis * -speed);

	if (Input.GetKey(KEY_D))
		pGameObject->m_transform.Translate(worldRightAxis * speed);







	if (Input.GetKeyDown(KEY_APOSTROPHE))
		Window.SetResolution(1280, 720, WINDOW_MODE::FULLSCREEN);

	if (Input.GetKeyDown(KEY_SEMICOLON))
		Window.SetResolution(1280, 720, WINDOW_MODE::WINDOWED);

	if (Input.GetKeyDown(KEY_U))
		this->Disable();

	if (Input.GetKeyDown(KEY_Y))
		this->Enable();
}

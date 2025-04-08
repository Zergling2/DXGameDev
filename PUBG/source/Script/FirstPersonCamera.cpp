#include "FirstPersonCamera.h"

using namespace pubg;
using namespace ze;

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 0.1f;
constexpr float WALK_SPEED = SPEED * 0.25f;

void FirstPersonCamera::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	Transform& transform = pGameObject->GetTransform();
	XMVECTOR worldForwardAxis = XMVector3Rotate(
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation))
	);
	XMVECTOR worldRightAxis = XMVector3Rotate(
		XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation))
	);

	const int32_t mx = Input.GetMouseMoveX();
	const int32_t my = Input.GetMouseMoveY();
	if (mx != 0 || my != 0)
	{
		
		transform.m_rotation.y += XMConvertToRadians(static_cast<float>(mx) * SENSITIVITY);
		transform.m_rotation.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);

		if (transform.m_rotation.x > XMConvertToRadians(85))
			transform.m_rotation.x = XMConvertToRadians(85);
		if (transform.m_rotation.x < -XMConvertToRadians(85))
			transform.m_rotation.x = -XMConvertToRadians(85);
	}

	const float speed = Input.GetKey(DIK_LSHIFT) ? WALK_SPEED : SPEED;
	
	if (Input.GetKey(DIK_W))
		XMStoreFloat3A(&transform.m_position, XMLoadFloat3A(&transform.m_position) + worldForwardAxis * speed);

	if (Input.GetKey(DIK_S))
		XMStoreFloat3A(&transform.m_position, XMLoadFloat3A(&transform.m_position) - worldForwardAxis * speed);

	if (Input.GetKey(DIK_A))
		XMStoreFloat3A(&transform.m_position, XMLoadFloat3A(&transform.m_position) - worldRightAxis * speed);

	if (Input.GetKey(DIK_D))
		XMStoreFloat3A(&transform.m_position, XMLoadFloat3A(&transform.m_position) + worldRightAxis * speed);

	if (Input.GetKeyDown(DIK_J))
		Window.SetResolution(800, 600, true);

	if (Input.GetKeyDown(DIK_K))
		Window.SetResolution(1280, 720, false);
}

#include "FirstPersonController.h"

using namespace ze;

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 16.0f;
constexpr float WALK_SPEED = SPEED * 0.25f;
constexpr float HEAD_CLAMP_ANGLE = 89.5f;

FirstPersonController::FirstPersonController(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
{
}

void FirstPersonController::Awake()
{
	Cursor::SetVisible(false);
	Cursor::SetLockState(CursorLockMode::Locked);

	SetResolution(1920, 1080, DisplayMode::BorderlessWindowed);
}

void FirstPersonController::Update()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	const float dt = Time::GetInstance()->GetDeltaTime();
	XMVECTOR localRotation = pGameObject->m_transform.GetRotation();
	XMVECTOR worldForwardAxis = XMVector3Rotate(Vector3::Forward(), localRotation);
	XMVECTOR worldRightAxis = XMVector3Rotate(Vector3::Right(), localRotation);

	const int32_t mx = Input::GetInstance()->GetMouseAxisHorizontal();
	const int32_t my = Input::GetInstance()->GetMouseAxisVertical();
	if (mx != 0 || my != 0)
	{
		XMVECTOR temp = Math::QuaternionToEuler(localRotation);
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, temp);

		rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
		rotationEuler.x = Math::Clamp(rotationEuler.x, XMConvertToRadians(-HEAD_CLAMP_ANGLE), XMConvertToRadians(+HEAD_CLAMP_ANGLE));
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

	if (Input::GetInstance()->GetKeyDown(KEY_ESCAPE))
		Runtime::GetInstance()->Exit();
}

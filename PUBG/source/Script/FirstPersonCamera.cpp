#include "FirstPersonCamera.h"

using namespace pubg;
using namespace ze;

constexpr float SENSITIVITY = 0.16f;
constexpr float SPEED = 0.1f;
constexpr float WALK_SPEED = SPEED * 0.25f;

void FirstPersonCamera::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	ComponentHandle<Transform> hTransform = pGameObject->GetComponent<Transform>();
	Transform* pTransform = hTransform.ToPtr();

	XMVECTOR worldForwardAxis = XMVector3Rotate(LOCAL_FORWARD, pTransform->GetRotation());
	XMVECTOR worldRightAxis = XMVector3Rotate(LOCAL_RIGHT, pTransform->GetRotation());

	const int32_t mx = Input.GetMouseAxisHorizontal();
	const int32_t my = Input.GetMouseAxisVertical();
	if (mx != 0 || my != 0)
	{
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, Math::QuaternionToEuler(pTransform->GetRotation()));

		rotationEuler.x += static_cast<float>(my) * SENSITIVITY;
		rotationEuler.y += static_cast<float>(mx) * SENSITIVITY;

		Math::Clamp(rotationEuler.x, -85.0f, +85.0f);

		rotationEuler.x = XMConvertToRadians(rotationEuler.x);
		rotationEuler.y = XMConvertToRadians(rotationEuler.y);
		rotationEuler.z = 0.0f;
		pTransform->SetRotation(rotationEuler);
	}

	const float speed = Input.GetKey(KEY_LSHIFT) ? WALK_SPEED : SPEED;
	
	if (Input.GetKey(KEY_W))
		pTransform->Translate(worldForwardAxis * speed);

	if (Input.GetKey(KEY_S))
		pTransform->Translate(worldForwardAxis * -speed);

	if (Input.GetKey(KEY_A))
		pTransform->Translate(worldRightAxis * -speed);

	if (Input.GetKey(KEY_D))
		pTransform->Translate(worldRightAxis * speed);
}

void FirstPersonCamera::Update()
{
	if (Input.GetKeyDown(KEY_APOSTROPHE))
		Window.SetResolution(1920, 1080, true);

	if (Input.GetKeyDown(KEY_SEMICOLON))
		Window.SetResolution(1280, 720, false);

	if (Input.GetKeyDown(KEY_COMMA))
		SceneManager.LoadScene("TestScene1");

	if (Input.GetKeyDown(KEY_PERIOD))
		SceneManager.LoadScene("TestScene2");
}

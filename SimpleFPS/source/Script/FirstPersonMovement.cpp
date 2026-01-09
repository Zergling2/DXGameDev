#include "FirstPersonMovement.h"

using namespace ze;

constexpr float SENSITIVITY = 0.1f;
constexpr float SPEED = 16.0f;
constexpr float WALK_SPEED = SPEED * 0.25f;
constexpr float HEAD_CLAMP_ANGLE = 89.5f;

void FirstPersonMovement::Awake()
{
	m_ui = false;
	Cursor::SetVisible(false);
	Cursor::SetLockState(CursorLockMode::Locked);

	SetResolution(1920, 1080, DisplayMode::BorderlessWindowed);
}

void FirstPersonMovement::Update()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	if (m_ui)
	{
		if (UIObjectManager::GetInstance()->GetFocusedUI() == nullptr)
		{
			if (Input::GetInstance()->GetKeyDown(KEY_N))
			{
				m_hWeaponChangePanel.ToPtr()->SetActive(false);

				m_ui = false;
				Cursor::SetVisible(false);
				Cursor::SetLockState(CursorLockMode::Locked);
			}
		}
		
		return;
	}

	if (Input::GetInstance()->GetKeyDown(KEY_N))
	{
		m_hWeaponChangePanel.ToPtr()->SetActive(true);

		m_ui = true;
		Cursor::SetVisible(true);
		Cursor::SetLockState(CursorLockMode::None);

		return;
	}

	this->MovementProcess(pGameObject);

	if (Input::GetInstance()->GetKeyDown(KEY_ESCAPE))
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
	if (Input::GetInstance()->GetKeyDown(KEY_5))
	{
		weaponChange = true;
		weaponIndex = 4;
	}

	if (weaponChange)
	{
		for (size_t i = 0; i < _countof(m_hWeapons); ++i)
		{
			GameObject* pWeapon = m_hWeapons[i].ToPtr();
			if (pWeapon)
				pWeapon->SetActive(false);
		}

		if (weaponIndex < _countof(m_hWeapons))
		{
			GameObject* pCurrWeapon = m_hWeapons[weaponIndex].ToPtr();
			if (pCurrWeapon)
				pCurrWeapon->SetActive(true);
		}
	}
}

void FirstPersonMovement::MovementProcess(ze::GameObject* pGameObject)
{
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
}

void FirstPersonMovement::TestHandlerOnClick01()
{
	printf("OnClick01!\n");
}

void FirstPersonMovement::TestHandlerOnClick02()
{
	printf("OnClick02!\n");
}

void FirstPersonMovement::TestHandlerOnClick03()
{
	printf("OnClick03!\n");
}

void FirstPersonMovement::EventHandlerAmbientChange()
{
	SliderControl* pSC = static_cast<SliderControl*>(m_hSlider.ToPtr());
	if (!pSC)
		return;

	float v = static_cast<float>(pSC->GetThumbPos()) * 0.1f;
	RenderSettings::GetInstance()->SetAmbientLightIntensity(v);
}

void FirstPersonMovement::EventHandlerMSAAChange()
{
	SliderControl* pSC = static_cast<SliderControl*>(m_hSliderMSAAChanger.ToPtr());
	if (!pSC)
		return;

	Camera* pCamera = static_cast<Camera*>(m_hComponentCamera.ToPtr());
	switch (pSC->GetThumbPos())
	{
	case 0:
		pCamera->SetMSAAMode(MSAAMode::Off);
		break;
	case 1:
		pCamera->SetMSAAMode(MSAAMode::x2);
		break;
	case 2:
		pCamera->SetMSAAMode(MSAAMode::x4);
		break;
	case 3:
		pCamera->SetMSAAMode(MSAAMode::x8);
		break;
	default:
		pCamera->SetMSAAMode(MSAAMode::Off);
		break;
	}
}

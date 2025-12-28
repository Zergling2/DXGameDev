#pragma once

#include <ZergEngine\ZergEngine.h>

class FirstPersonMovement : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	FirstPersonMovement() = default;
	virtual ~FirstPersonMovement() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void MovementProcess(ze::GameObject* pGameObject);

	void TestSliderHandler01();
public:
	ze::GameObjectHandle m_hWeapons[4];
	bool m_ui;
	ze::UIObjectHandle m_hWeaponChangePanel;

	ze::UIObjectHandle m_hSlider;
};

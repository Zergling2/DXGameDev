#pragma once

#include <ZergEngine\ZergEngine.h>

class FirstPersonMovement : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	FirstPersonMovement(ze::GameObject& owner);
	virtual ~FirstPersonMovement() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void MovementProcess(ze::GameObject* pGameObject);

	void TestHandlerOnClick01();
	void TestHandlerOnClick02();
	void TestHandlerOnClick03();
	void EventHandlerAmbientChange();
	void EventHandlerMSAAChange();
public:
	ze::GameObjectHandle m_hWeapons[4];
	bool m_ui;
	ze::UIObjectHandle m_hWeaponChangePanel;
	ze::ComponentHandle<ze::Camera> m_hComponentCamera;

	ze::UIObjectHandle m_hSlider;
	ze::UIObjectHandle m_hSliderMSAAChanger;
};

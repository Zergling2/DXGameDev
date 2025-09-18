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
public:
	ze::GameObjectHandle m_hWeapons[4];
	bool m_ui;
	ze::GameObjectHandle m_hDestroyTest;
};

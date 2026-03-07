#pragma once

#include <ZergEngine\ZergEngine.h>

class Character : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Character(ze::GameObject& owner);
	virtual ~Character() = default;

	virtual void Update() override;
	virtual void LateUpdate() override;
public:
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hSkinnedMeshRenderer;
	ze::GameObjectHandle m_hTvWeaponBase;
	ze::GameObjectHandle m_hPrimaryWeapon;
	ze::GameObjectHandle m_hSecondaryWeapon;
};

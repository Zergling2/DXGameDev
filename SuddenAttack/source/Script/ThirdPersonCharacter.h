#pragma once

#include <ZergEngine\ZergEngine.h>

class ThirdPersonCharacter : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ThirdPersonCharacter(ze::GameObject& owner);
	virtual ~ThirdPersonCharacter() = default;

	virtual void Update() override;
	virtual void LateUpdate() override;
public:
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hSkinnedMeshRendererThirdPersonCharacter;
	ze::GameObjectHandle m_hGameObjectTVWeaponBase;
	ze::GameObjectHandle m_hGameObjectTVWeapons[2];
};

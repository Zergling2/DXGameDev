#pragma once

#include <ZergEngine\ZergEngine.h>
#include <vector>
#include <memory>
#include "..\Resource\WeaponAction.h"

class GameUIManager;
class WeaponDefinition;
class WeaponEventTable;

class Weapon : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Weapon(ze::GameObject& owner);
	virtual ~Weapon() = default;

	void Init(std::shared_ptr<WeaponDefinition> spWeaponDef, int32_t ammoInMag, int32_t auxAmmo);

	virtual void Update() override;

	bool CanReload() const;
	bool CanFire() const;

	void Draw();
	void Reload();
	void Fire();
	void Undraw();
private:
	void OnReloadFinished();
	void OnIdle(float exceed);
	void CreateAmmoUIText(wchar_t* pBuf, size_t len) const;
	void PlayAnim(WeaponAction action, bool loop);
public:
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hWeaponMeshRenderer;
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hArmsMeshRenderer;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
private:
	std::shared_ptr<const WeaponDefinition> m_spWeaponDef;
	int32_t m_ammoInMag;
	int32_t m_auxAmmo;

	WeaponAction m_action;
	const WeaponEventTable* m_pCurrWeaponEventTable;
	size_t m_eventIndexCursor;

	float m_actionDuration;
	float m_actionElapsed;
	float m_fireCoolTime;
};

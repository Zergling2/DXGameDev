#include "Weapon.h"
#include "GameUIManager.h"
#include "..\Resource\WeaponDefinition.h"

Weapon::Weapon(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_hWeaponMeshRenderer()
	, m_hArmsMeshRenderer()
	, m_hScriptGameUIManager()
	, m_spWeaponDef()
	, m_ammoInMag(0)
	, m_auxAmmo(0)
	, m_action(WeaponAction::None)
	, m_pCurrWeaponEventTable(nullptr)
	, m_eventIndexCursor(0)
	, m_actionDuration(0.0f)
	, m_actionElapsed(0.0f)
	, m_fireCoolTime(0.0f)
{
}

void Weapon::Init(std::shared_ptr<WeaponDefinition> spWeaponDef, int32_t ammoInMag, int32_t auxAmmo)
{
	m_spWeaponDef = spWeaponDef;
	m_ammoInMag = ammoInMag;
	m_auxAmmo = auxAmmo;

	m_action = WeaponAction::None;
	m_pCurrWeaponEventTable = nullptr;
	m_eventIndexCursor = 0;

	m_actionDuration = 0.0f;
	m_actionElapsed = 0.0f;
	m_fireCoolTime = 0.0f;

	ze::SkinnedMeshRenderer* pWeaponMeshRenderer = m_hWeaponMeshRenderer.ToPtr();
	pWeaponMeshRenderer->SetMesh(m_spWeaponDef->m_spPVMesh);
	pWeaponMeshRenderer->SetArmature(m_spWeaponDef->m_spPVArmature);
	for (size_t i = 0; i < m_spWeaponDef->m_materials.size(); ++i)
		pWeaponMeshRenderer->SetMaterial(i, m_spWeaponDef->m_materials[i]);
}

void Weapon::Update()
{
	if (!m_spWeaponDef)
		return;

	const float dt = ze::Time::GetInstance()->GetDeltaTime();

	if (m_action != WeaponAction::Idle && m_action != WeaponAction::None)
	{
		m_actionElapsed += dt;		// 업데이트

		// 이벤트 테이블 참조 및 이벤트 처리
		if (m_pCurrWeaponEventTable)
		{
			while (m_eventIndexCursor < m_pCurrWeaponEventTable->m_events.size())
			{
				const auto& item = m_pCurrWeaponEventTable->m_events[m_eventIndexCursor];
				if (item.first <= m_actionElapsed)
				{
					// 무기 이벤트 처리
					if (item.second == WeaponEvent::Fire)
					{
						printf("Fire\n");
					}

					// 1. 사운드 처리
					const auto iter = m_spWeaponDef->m_weaponSounds.find(item.second);
					if (iter != m_spWeaponDef->m_weaponSounds.end())
					{
						if (iter->second)
							iter->second->Play();
					}

					++m_eventIndexCursor;
				}
				else
				{
					break;
				}
			}
		}

		switch (m_action)
		{
		case WeaponAction::Draw:
			break;
		case WeaponAction::Reload:
			if (m_actionDuration <= m_actionElapsed)
				this->OnReloadFinished();
			break;
		case WeaponAction::Fire:
			m_fireCoolTime = (std::max)(m_fireCoolTime - dt, 0.0f);
			break;
		default:
			break;
		}

		if (m_actionDuration <= m_actionElapsed)
			this->OnIdle(m_actionElapsed - m_actionDuration);
	}
}

bool Weapon::CanReload() const
{
	if (!m_spWeaponDef)
		return false;

	return m_action == WeaponAction::Idle && m_ammoInMag < m_spWeaponDef->GetMagCapacity() && m_auxAmmo > 0;
}

bool Weapon::CanFire() const
{
	return m_fireCoolTime <= 0.0f && m_ammoInMag > 0 && (m_action == WeaponAction::Idle || m_action == WeaponAction::Fire);
}

void Weapon::Draw()
{
	if (!m_spWeaponDef)
		return;

	m_pGameObject->SetActive(true);

	m_action = WeaponAction::Draw;
	auto eventTableIter = m_spWeaponDef->m_eventTables.find(m_action);
	m_pCurrWeaponEventTable = eventTableIter == m_spWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
	m_eventIndexCursor = 0;

	m_actionDuration = m_spWeaponDef->GetDrawTime();
	m_actionElapsed = 0.0f;

	this->PlayAnim(WeaponAction::Draw, false);

	// 탄약 UI 업데이트
	wchar_t buf[32];
	this->CreateAmmoUIText(buf, _countof(buf));
	GameUIManager* pGameUIManager = m_hScriptGameUIManager.ToPtr();
	static_cast<ze::Text*>(pGameUIManager->m_hTextAmmoState.ToPtr())->SetText(buf);
	static_cast<ze::Text*>(pGameUIManager->m_hTextWeaponName.ToPtr())->SetText(m_spWeaponDef->GetName());
}

void Weapon::Reload()
{
	if (!m_spWeaponDef)
		return;

	if (!this->CanReload())
		return;

	m_action = WeaponAction::Reload;
	auto eventTableIter = m_spWeaponDef->m_eventTables.find(m_action);
	m_pCurrWeaponEventTable = eventTableIter == m_spWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
	m_eventIndexCursor = 0;

	m_actionDuration = m_spWeaponDef->GetReloadTime();
	m_actionElapsed = 0.0f;

	this->PlayAnim(WeaponAction::Reload, false);
}

void Weapon::Fire()
{
	if (!m_spWeaponDef)
		return;

	if (!this->CanFire())
		return;

	m_action = WeaponAction::Fire;
	auto eventTableIter = m_spWeaponDef->m_eventTables.find(m_action);
	m_pCurrWeaponEventTable = eventTableIter == m_spWeaponDef->m_eventTables.end() ? nullptr : eventTableIter->second.get();
	m_eventIndexCursor = 0;

	m_actionDuration = m_spWeaponDef->GetRecoilTime();
	m_actionElapsed = 0.0f;
	m_fireCoolTime = m_spWeaponDef->GetFireRate();

	m_ammoInMag -= 1;

	// 탄약 UI 업데이트
	wchar_t buf[32];
	this->CreateAmmoUIText(buf, _countof(buf));
	GameUIManager* pGameUIManager = m_hScriptGameUIManager.ToPtr();
	static_cast<ze::Text*>(pGameUIManager->m_hTextAmmoState.ToPtr())->SetText(buf);

	this->PlayAnim(WeaponAction::Fire, false);
}

void Weapon::Undraw()
{
	m_hWeaponMeshRenderer.ToPtr()->StopAnimation();
	m_hArmsMeshRenderer.ToPtr()->StopAnimation();

	m_pGameObject->SetActive(false);

	m_action = WeaponAction::None;
	m_actionDuration = 0.0f;
	m_actionElapsed = 0.0f;
	m_fireCoolTime = 0.0f;

	m_pCurrWeaponEventTable = nullptr;
	m_eventIndexCursor = 0;

	// 탄약 UI 업데이트
	GameUIManager* pGameUIManager = m_hScriptGameUIManager.ToPtr();
	static_cast<ze::Text*>(pGameUIManager->m_hTextAmmoState.ToPtr())->GetText().clear();
	static_cast<ze::Text*>(pGameUIManager->m_hTextWeaponName.ToPtr())->GetText().clear();
}

void Weapon::OnReloadFinished()
{
	const int32_t needed = m_spWeaponDef->GetMagCapacity() - m_ammoInMag;
	assert(needed > 0);

	const int32_t toLoad = (std::min)(needed, m_auxAmmo);

	const int32_t newAmmoInMag = m_ammoInMag + toLoad;
	const int32_t newAuxAmmo = m_auxAmmo - toLoad;
	assert(0 < newAmmoInMag && newAmmoInMag <= m_spWeaponDef->GetMagCapacity());

	m_ammoInMag = newAmmoInMag;
	m_auxAmmo = newAuxAmmo;

	// 탄약 UI 업데이트
	wchar_t buf[32];
	this->CreateAmmoUIText(buf, _countof(buf));
	GameUIManager* pGameUIManager = m_hScriptGameUIManager.ToPtr();
	static_cast<ze::Text*>(pGameUIManager->m_hTextAmmoState.ToPtr())->SetText(buf);
}

void Weapon::OnIdle(float exceed)
{
	m_action = WeaponAction::Idle;
	m_actionDuration = 0.0f;
	m_actionElapsed = 0.0f;

	this->PlayAnim(WeaponAction::Idle, true);
}

void Weapon::CreateAmmoUIText(wchar_t* pBuf, size_t len) const
{
	StringCchPrintfW(pBuf, len, L"%d / %d", m_ammoInMag, m_auxAmmo);
}

void Weapon::PlayAnim(WeaponAction action, bool loop)
{
	if (!m_spWeaponDef)
		return;

	const auto actionAnimsIter = m_spWeaponDef->m_actionAnims.find(action);
	if (actionAnimsIter != m_spWeaponDef->m_actionAnims.end())
	{
		m_hWeaponMeshRenderer.ToPtr()->PlayAnimation(actionAnimsIter->second.first, loop);
		m_hArmsMeshRenderer.ToPtr()->PlayAnimation(actionAnimsIter->second.second, loop);
	}
}

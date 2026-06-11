#include "Weapon.h"
#include "WeaponInfo.h"
#include "WeaponState.h"
#include <strsafe.h>
#include <algorithm>

FirearmBase::FirearmBase(const WeaponViewInfo* pWeaponViewInfo, const WeaponInfo* pWeaponInfo)
	: m_pWeaponViewInfo(pWeaponViewInfo)
	, m_pWeaponInfo(pWeaponInfo)
	, m_upWeaponState()
	, m_ammoInMag(m_pWeaponInfo->GetMagCapacity())
	, m_reservedAmmo(m_pWeaponInfo->GetAuxilirayAmmo())
	, m_ammoStateTextDirty(true)
	, m_fireCooldown(0.0f)
{
	m_ammoStateTextBuf[0] = L'\0';
}

const wchar_t* FirearmBase::GetName() const
{
	return m_pWeaponInfo->GetName();
}

void FirearmBase::StartDraw()
{
	this->SetState(std::make_unique<WSDrawing>(m_pWeaponViewInfo->GetDrawAnimTime()));
}

void FirearmBase::StartReload()
{
	if (!this->CanReload())
		return;

	// 상태 전환
	this->SetState(std::make_unique<WSReloading>(m_pWeaponViewInfo->GetReloadAnimTime()));
}

void FirearmBase::StartFire()
{
	if (!this->CanFire())
		return;

	this->SetAmmoState(this->GetAmmoInMag() - 1, this->GetReservedAmmo());
	m_fireCooldown = m_pWeaponInfo->GetSPR();

	// 상태 전환
	this->SetState(std::make_unique<WSFiring>(m_pWeaponViewInfo->GetFireAnimTime()));
}

void FirearmBase::OnReloadComplete()
{
	const int32_t needed = m_pWeaponInfo->GetMagCapacity() - m_ammoInMag;
	assert(needed > 0);

	const int32_t toLoad = (std::min)(needed, m_reservedAmmo);

	const int32_t newAmmoInMag = GetAmmoInMag() + toLoad;
	const int32_t newReservedAmmo = GetReservedAmmo() - toLoad;
	assert(0 < newAmmoInMag && newAmmoInMag <= m_pWeaponInfo->GetMagCapacity());

	this->SetAmmoState(newAmmoInMag, newReservedAmmo);
}

bool FirearmBase::CanFire() const
{
	if (!m_upWeaponState)
		return false;

	return m_ammoInMag > 0 && m_fireCooldown <= 0.0f && m_upWeaponState->CanFire();
}

bool FirearmBase::CanReload() const
{
	if (!m_upWeaponState)
		return false;

	return m_ammoInMag < m_pWeaponInfo->GetMagCapacity() && m_reservedAmmo > 0 && m_upWeaponState->CanReload();
}

void FirearmBase::Update(float dt)
{
	if (m_fireCooldown > 0.0f)
		m_fireCooldown -= dt;

	if (m_upWeaponState)
	{
		WeaponStateTransition wst = m_upWeaponState->Update(dt, this);

		switch (wst)
		{
		case WeaponStateTransition::None:
			break;
		case WeaponStateTransition::ToReloading:
			this->SetState(std::make_unique<WSReloading>(m_pWeaponViewInfo->GetReloadAnimTime()));
			break;
		case WeaponStateTransition::ToFiring:
			this->SetState(std::make_unique<WSFiring>(m_pWeaponViewInfo->GetFireAnimTime()));
			break;
		case WeaponStateTransition::ToIdling:
			this->SetState(std::make_unique<WSIdle>());
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;
			break;
		}
	}
}

void FirearmBase::DispatchEvent(WeaponEvent type)
{
	if (m_weaponEventHandler)
		m_weaponEventHandler(type, m_pWeaponViewInfo);
}

void FirearmBase::SetAmmoState(int32_t ammoInMag, int32_t reservedAmmo)
{
	m_ammoInMag = ammoInMag; m_reservedAmmo = reservedAmmo;
	m_ammoStateTextDirty = true;
}

void FirearmBase::SetState(std::unique_ptr<IWeaponState> upWeaponState)
{
	if (m_upWeaponState)
		m_upWeaponState->Exit(this);

	m_upWeaponState = std::move(upWeaponState);

	if (m_upWeaponState)
		m_upWeaponState->Enter(this);
}

const wchar_t* FirearmBase::GetAmmoStateText() const
{
	if (m_ammoStateTextDirty)
	{
		StringCchPrintfW(m_ammoStateTextBuf, _countof(m_ammoStateTextBuf), L"%d / %d", m_ammoInMag, m_reservedAmmo);
		m_ammoStateTextDirty = false;
	}

	return m_ammoStateTextBuf;
}

#include "WeaponState.h"
#include "Weapon.h"

void WSDrawing::Enter(IWeapon* pWeapon)
{
	pWeapon->DispatchEvent(WeaponEvent::DrawingStarted);
}

WeaponStateTransition WSDrawing::Update(float dt, IWeapon* pWeapon)
{
	m_remainingTime -= dt;

	if (m_remainingTime <= 0.0f)
		return WeaponStateTransition::ToIdling;

	return WeaponStateTransition::None;
}

void WSDrawing::Exit(IWeapon* pWeapon)
{
}

void WSReloading::Enter(IWeapon* pWeapon)
{
	pWeapon->DispatchEvent(WeaponEvent::ReloadingStarted);
}

WeaponStateTransition WSReloading::Update(float dt, IWeapon* pWeapon)
{
	m_remainingTime -= dt;

	if (m_remainingTime <= 0.0f)
	{
		pWeapon->OnReloadComplete();
		pWeapon->DispatchEvent(WeaponEvent::ReloadComplete);

		return WeaponStateTransition::ToIdling;
	}

	return WeaponStateTransition::None;
}

void WSReloading::Exit(IWeapon* pWeapon)
{
}

void WSFiring::Enter(IWeapon* pWeapon)
{
	pWeapon->DispatchEvent(WeaponEvent::FiringStarted);
}

WeaponStateTransition WSFiring::Update(float dt, IWeapon* pWeapon)
{
	m_remainingTime -= dt;

	if (m_remainingTime <= 0.0f)
		return WeaponStateTransition::ToIdling;

	return WeaponStateTransition::None;
}

void WSFiring::Exit(IWeapon* pWeapon)
{
}

void WSIdle::Enter(IWeapon* pWeapon)
{
	pWeapon->DispatchEvent(WeaponEvent::IdlingStarted);
}

WeaponStateTransition WSIdle::Update(float dt, IWeapon* pWeapon)
{
	return WeaponStateTransition::None;
}

void WSIdle::Exit(IWeapon* pWeapon)
{
}

#pragma once

#include "WeaponState.h"
#include <memory>
#include <functional>

class WeaponViewInfo;
class WeaponInfo;

enum class WeaponEvent
{
	DrawingStarted,
	ReloadingStarted,
	ReloadComplete,
	FiringStarted,
	IdlingStarted,
	EmptyFiringStarted
};

class IWeapon
{
public:
	IWeapon() = default;
	virtual ~IWeapon() = default;

	virtual const WeaponViewInfo* GetWeaponViewInfo() const = 0;
	virtual const WeaponInfo* GetWeaponInfo() const = 0;
	virtual const wchar_t* GetName() const = 0;
	virtual void SetState(std::unique_ptr<IWeaponState> upWeaponState) = 0;
	virtual const wchar_t* GetAmmoStateText() const = 0;
	virtual void StartDraw() = 0;
	virtual void StartReload() = 0;
	virtual void StartFire() = 0;
	virtual void OnReloadComplete() = 0;
	virtual bool CanFire() const = 0;
	virtual bool CanReload() const = 0;
	virtual void Update(float dt) = 0;
	virtual void DispatchEvent(WeaponEvent type) = 0;
	virtual void SetWeaponEventHandler(std::function<void(WeaponEvent, const WeaponViewInfo*)> handler) = 0;
};

class FirearmBase : public IWeapon
{
public:
	FirearmBase(const WeaponViewInfo* pWeaponViewInfo, const WeaponInfo* pWeaponInfo);
	virtual ~FirearmBase() = default;

	virtual const WeaponViewInfo* GetWeaponViewInfo() const { return m_pWeaponViewInfo; }
	virtual const WeaponInfo* GetWeaponInfo() const { return m_pWeaponInfo; }
	virtual const wchar_t* GetName() const override;
	virtual void SetState(std::unique_ptr<IWeaponState> upWeaponState) override;
	virtual const wchar_t* GetAmmoStateText() const override;
	virtual void StartDraw() override;
	virtual void StartReload() override;
	virtual void StartFire() override;
	virtual void OnReloadComplete() override;
	virtual bool CanFire() const override;
	virtual bool CanReload() const override;
	virtual void Update(float dt) override;
	virtual void DispatchEvent(WeaponEvent type) override;
	virtual void SetWeaponEventHandler(std::function<void(WeaponEvent, const WeaponViewInfo*)> handler) override { m_weaponEventHandler = std::move(handler); }
private:
	void SetAmmoState(int32_t ammoInMag, int32_t reservedAmmo);
	int32_t GetAmmoInMag() const { return m_ammoInMag; }
	int32_t GetReservedAmmo() const { return m_reservedAmmo; }
protected:
	const WeaponViewInfo* m_pWeaponViewInfo;
	const WeaponInfo* m_pWeaponInfo;
	std::unique_ptr<IWeaponState> m_upWeaponState;
	int32_t m_ammoInMag;
	int32_t m_reservedAmmo;
	mutable bool m_ammoStateTextDirty;
	mutable wchar_t m_ammoStateTextBuf[16];
	std::function<void(WeaponEvent, const WeaponViewInfo*)> m_weaponEventHandler;
	float m_fireCooldown;
};

class AutoRifle : public FirearmBase
{
public:
	AutoRifle(const WeaponViewInfo* pWeaponViewInfo, const WeaponInfo* pWeaponInfo)
		: FirearmBase(pWeaponViewInfo, pWeaponInfo)
	{
	}
	virtual ~AutoRifle() = default;
};

class SemiAutoPistol : public FirearmBase
{
public:
	SemiAutoPistol(const WeaponViewInfo* pWeaponViewInfo, const WeaponInfo* pWeaponInfo)
		: FirearmBase(pWeaponViewInfo, pWeaponInfo)
	{
	}
	virtual ~SemiAutoPistol() = default;
};

class AutoPistol : public FirearmBase
{
public:
	AutoPistol(const WeaponViewInfo* pWeaponViewInfo, const WeaponInfo* pWeaponInfo)
		: FirearmBase(pWeaponViewInfo, pWeaponInfo)
	{
	}
	virtual ~AutoPistol() = default;
};

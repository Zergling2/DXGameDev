#pragma once

class IWeapon;

enum class WeaponStateTransition
{
	None,
	ToReloading,
	ToFiring,
	ToIdling
};

class IWeaponState
{
public:
	IWeaponState() = default;
	virtual ~IWeaponState() = default;

	virtual void Enter(IWeapon* pWeapon) = 0;
	virtual WeaponStateTransition Update(float dt, IWeapon* pWeapon) = 0;
	virtual void Exit(IWeapon* pWeapon) = 0;
	virtual bool CanFire() const = 0;
	virtual bool CanReload() const = 0;
};

class WSDrawing : public IWeaponState
{
public:
	WSDrawing(float remainingTime)
		: m_remainingTime(remainingTime)
	{
	}
	virtual ~WSDrawing() = default;

	virtual void Enter(IWeapon* pWeapon) override;
	virtual WeaponStateTransition Update(float dt, IWeapon* pWeapon) override;
	virtual void Exit(IWeapon* pWeapon) override;
	virtual bool CanFire() const override { return false; }
	virtual bool CanReload() const override { return false; }

	float GetRemainingTime() const { return m_remainingTime; }
	void SetRemainingTime(float time) { m_remainingTime = time; }
private:
	float m_remainingTime;
};

class WSReloading : public IWeaponState
{
public:
	WSReloading(float remainingTime)
		: m_remainingTime(remainingTime)
	{
	}
	virtual ~WSReloading() = default;

	virtual void Enter(IWeapon* pWeapon) override;
	virtual WeaponStateTransition Update(float dt, IWeapon* pWeapon) override;
	virtual void Exit(IWeapon* pWeapon) override;
	virtual bool CanFire() const override { return false; }
	virtual bool CanReload() const override { return false; }

	float GetRemainingTime() const { return m_remainingTime; }
	void SetRemainingTime(float time) { m_remainingTime = time; }
private:
	float m_remainingTime;
};

class WSFiring : public IWeaponState
{
public:
	WSFiring(float remainingTime)
		: m_remainingTime(remainingTime)
	{
	}
	virtual ~WSFiring() = default;

	float GetRemainingTime() const { return m_remainingTime; }
	void SetRemainingTime(float time) { m_remainingTime = time; }

	virtual void Enter(IWeapon* pWeapon) override;
	virtual WeaponStateTransition Update(float dt, IWeapon* pWeapon) override;
	virtual void Exit(IWeapon* pWeapon) override;
	virtual bool CanFire() const override { return true; }
	virtual bool CanReload() const override { return false; }
private:
	float m_remainingTime;
};

class WSIdle : public IWeaponState
{
public:
	WSIdle() = default;
	virtual ~WSIdle() = default;

	virtual void Enter(IWeapon* pWeapon) override;
	virtual WeaponStateTransition Update(float dt, IWeapon* pWeapon) override;
	virtual void Exit(IWeapon* pWeapon) override;
	virtual bool CanFire() const override { return true; }
	virtual bool CanReload() const override { return true; }
};

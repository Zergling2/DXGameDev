#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class TimeImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(TimeImpl);
	public:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		inline float GetTimeScale() const { return m_ts; }
		void SetTimeScale(float ts);
		inline float GetFixedDeltaTime() const { return m_fdt; }
		bool SetFixedDeltaTime(float fdt);
		inline float GetDeltaTime() const { return m_dt; }
		inline float GetUnscaledDeltaTime() const { return m_udt; }

		void Update();							// 일시정지 상태일때는 cache paused time, 아닐때는 cache delta time
		void ChangeDeltaTimeToFixedDeltaTime();	// FixedUpdate 함수에서 GetDeltaTime을 사용해도 FIXED_DELTA_TIME을 반환해주기 위해 호출
		void RecoverDeltaTime();
	private:
		float m_spc;				// seconds per count
		float m_ts;					// time scale
		float m_fdt;				// fixed delta time
		float m_udt;				// unscaled delta time
		float m_dt;					// scaled delta time
		float m_dtBackup;			// backup for transition between FixedUpdate <---> Update
		LARGE_INTEGER m_basePC;		// base count
		LARGE_INTEGER m_deltaPC;	// delta count
		LARGE_INTEGER m_prevPC;		// previous count
		LARGE_INTEGER m_currPC;		// current count
		LARGE_INTEGER m_pausedPC;	// paused count
	};

	extern TimeImpl Time;
}

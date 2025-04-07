#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class TimeImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(TimeImpl);
	public:
		static constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f;
	public:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		inline float GetTimeScale() { return m_ts; }
		void SetTimeScale(float ts);
		inline float GetFixedDeltaTime() { return FIXED_DELTA_TIME; }
		inline float GetDeltaTime() { return m_dt; }
		inline float GetUnscaledDeltaTime() { return m_udt; }

		void Update();							// 일시정지 상태일때는 cache paused time, 아닐때는 cache delta time
		void ChangeDeltaTimeToFixedDeltaTime();	// FixedUpdate 함수에서 GetDeltaTime을 사용해도 FIXED_DELTA_TIME을 반환해주기 위해 호출
		void RecoverDeltaTime();
	private:
		float m_spc;					// seconds per count
		float m_ts;						// time scale
		float m_udt;					// unscaled delta time
		float m_dt;						// scaled delta time
		float m_dtBackup;				// backup for transition between FixedUpdate <---> Update
		LARGE_INTEGER m_qpcBaseCnt;		// base count
		LARGE_INTEGER m_qpcDeltaCnt;	// delta count
		LARGE_INTEGER m_qpcPrevCnt;		// previous count
		LARGE_INTEGER m_qpcCurrentCnt;	// current count
		LARGE_INTEGER m_qpcPausedCnt;	// paused count
	};

	extern TimeImpl Time;
}

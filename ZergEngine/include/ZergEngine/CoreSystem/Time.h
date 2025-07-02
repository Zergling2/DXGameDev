#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Time
	{
		friend class Runtime;
	public:
		static Time* GetInstance() { return s_pInstance; }
	private:
		Time() = default;
		~Time() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();

		void Update();							// �Ͻ����� �����϶��� cache paused time, �ƴҶ��� cache delta time
		void ChangeDeltaTimeToFixedDeltaTime();	// FixedUpdate �Լ����� GetDeltaTime�� ����ص� FIXED_DELTA_TIME�� ��ȯ���ֱ� ���� ȣ��
		void RecoverDeltaTime();

		LONGLONG GetFixedDeltaPerformanceCounter() const { return m_fdtPC.QuadPart; }
		LONGLONG GetDeltaPerformanceCounter() const { return m_deltaPC.QuadPart; }
	public:
		float GetTimeScale() const { return mTs; }
		void SetTimeScale(float ts);
		float GetFixedDeltaTime() const { return m_fdt; }
		bool SetFixedDeltaTime(float fdt);
		float GetDeltaTime() const { return m_dt; }
		float GetUnscaledDeltaTime() const { return m_udt; }
	private:
		static Time* s_pInstance;
		float m_spc;				// seconds per count
		float mTs;					// time scale
		float m_fdt;				// fixed delta time
		float m_udt;				// unscaled delta time
		float m_dt;					// scaled delta time
		float m_dtBackup;			// backup for transition between FixedUpdate <---> Update
		LARGE_INTEGER m_fdtPC;
		LARGE_INTEGER m_basePC;		// base count
		LARGE_INTEGER m_deltaPC;	// delta count
		LARGE_INTEGER m_prevPC;		// previous count
		LARGE_INTEGER m_currPC;		// current count
		LARGE_INTEGER m_pausedPC;	// paused count
	};
}

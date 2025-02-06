#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Time
	{
		friend class Engine;
	private:
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();
	public:
		static inline float GetTimeScale() { return Time::s_ts; }
		static void SetTimeScale(float ts);
		static inline float GetFixedDeltaTime() { return FIXED_DELTA_TIME; }
		static inline float GetDeltaTime() { return Time::s_dtCache; }
		static inline float GetUnscaledDeltaTime() { return Time::s_udt; }
	private:
		static void Update();				// 일시정지 상태일때는 cache paused time, 아닐때는 cache delta time
		static void SwitchDeltaTimeToFixedDeltaTime();	// is called by engine before execute FixedUpdate for all scripts.
		static void RecoverDeltaTime();
	private:
		static float s_spc;						// seconds per count
		static float s_ts;						// time scale
		static float s_udt;						// unscaled delta time
		static float s_dtCache;					// unscaled delta time x time scale cache
		static float s_dtBackup;				// backup for transition between FixedUpdate <---> Update
		static LARGE_INTEGER s_qpcBaseCnt;		// base count
		static LARGE_INTEGER s_qpcDeltaCnt;		// delta count
		static LARGE_INTEGER s_qpcPrevCnt;		// previous count
		static LARGE_INTEGER s_qpcCurrentCnt;	// current count
		static LARGE_INTEGER s_qpcPausedCnt;	// paused count
	};
}

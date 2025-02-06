#include <ZergEngine\System\Time.h>

using namespace zergengine;

float Time::s_spc;						// seconds per count
float Time::s_ts;						// time scale
float Time::s_udt;						// unscaled delta time
float Time::s_dtCache;					// unscaled delta time x time scale cache
float Time::s_dtBackup;					// backup for transition between FixedUpdate <---> Update
LARGE_INTEGER Time::s_qpcBaseCnt;		// base count
LARGE_INTEGER Time::s_qpcDeltaCnt;		// delta count
LARGE_INTEGER Time::s_qpcPrevCnt;		// previous count
LARGE_INTEGER Time::s_qpcCurrentCnt;	// current count
LARGE_INTEGER Time::s_qpcPausedCnt;		// paused count

sysresult Time::SysStartup(const EngineSystemInitDescriptor& desc)
{
	UNREFERENCED_PARAMETER(desc);

	LARGE_INTEGER freq;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceFrequency(&freq) == FALSE)
		return -1;

	Time::s_spc = 1.0f / static_cast<float>(freq.QuadPart);
	Time::s_ts = 1.0f;
	Time::s_udt = 0.0f;
	Time::s_dtCache = 0.0f;
	Time::s_dtBackup = Time::s_dtCache;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	QueryPerformanceCounter(&Time::s_qpcBaseCnt);
	Time::s_qpcDeltaCnt.QuadPart = 0;
	Time::s_qpcPrevCnt = Time::s_qpcBaseCnt;
	Time::s_qpcCurrentCnt = Time::s_qpcBaseCnt;
	Time::s_qpcPausedCnt.QuadPart = 0;

	return 0;
}

void Time::SysCleanup()
{
}

void Time::SetTimeScale(float ts)
{
	if (ts < 0.0f)
		return;

	Time::s_ts = ts;
}

void Time::Update()
{
	QueryPerformanceCounter(&Time::s_qpcCurrentCnt);
	Time::s_qpcDeltaCnt.QuadPart = Time::s_qpcCurrentCnt.QuadPart - Time::s_qpcPrevCnt.QuadPart;
	if (Time::s_qpcDeltaCnt.QuadPart < 0)
		Time::s_qpcDeltaCnt.QuadPart = 0;

	Time::s_udt = static_cast<float>(Time::s_qpcDeltaCnt.QuadPart) * Time::s_spc;
	Time::s_dtCache = Time::s_udt * Time::s_ts;

	Time::s_qpcPrevCnt = Time::s_qpcCurrentCnt;
}

void Time::SwitchDeltaTimeToFixedDeltaTime()
{
	Time::s_dtBackup = Time::s_dtCache;
	Time::s_dtCache = FIXED_DELTA_TIME;
}

void Time::RecoverDeltaTime()
{
	Time::s_dtCache = Time::s_dtBackup;
}

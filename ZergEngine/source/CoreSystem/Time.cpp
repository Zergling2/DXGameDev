#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Time);

Time::Time()
{
}

Time::~Time()
{
}

void Time::Init(void* pDesc)
{
	LARGE_INTEGER freq;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceFrequency(&freq) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"Time::Init() > QueryPerformanceFrequency()", GetLastError());

	m_spc = 1.0f / static_cast<float>(freq.QuadPart);
	m_ts = 1.0f;
	m_udt = 0.0f;
	m_dt = 0.0f;
	m_dtBackup = m_dt;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceCounter(&m_qpcBaseCnt) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"Time::Init() > QueryPerformanceCounter()", GetLastError());

	m_qpcDeltaCnt.QuadPart = 0;
	m_qpcPrevCnt = m_qpcBaseCnt;
	m_qpcCurrentCnt = m_qpcBaseCnt;
	m_qpcPausedCnt.QuadPart = 0;
}

void Time::Release()
{
}

void Time::SetTimeScale(float ts)
{
	if (ts < 0.0f)
		return;

	m_ts = ts;
}

void Time::Update()
{
	QueryPerformanceCounter(&m_qpcCurrentCnt);
	m_qpcDeltaCnt.QuadPart = m_qpcCurrentCnt.QuadPart - m_qpcPrevCnt.QuadPart;
	if (m_qpcDeltaCnt.QuadPart < 0)
		m_qpcDeltaCnt.QuadPart = 0;

	m_udt = static_cast<float>(m_qpcDeltaCnt.QuadPart) * m_spc;
	m_dt = m_udt * m_ts;

	m_qpcPrevCnt = m_qpcCurrentCnt;
}

void Time::ChangeDeltaTimeToFixedDeltaTime()
{
	m_dtBackup = m_dt;
	m_dt = FIXED_DELTA_TIME;
}

void Time::RecoverDeltaTime()
{
	m_dt = m_dtBackup;
}

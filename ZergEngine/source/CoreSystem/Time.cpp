#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Debug.h>

namespace ze
{
	TimeImpl ze::Time;
}

using namespace ze;

static constexpr float DEFAULT_FIXED_DELTA_TIME = 1.0f / 50.0f;

TimeImpl::TimeImpl()
{
}

TimeImpl::~TimeImpl()
{
}

void TimeImpl::Init(void* pDesc)
{
	LARGE_INTEGER freq;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceFrequency(&freq) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"TimeImpl::Init() > QueryPerformanceFrequency()", GetLastError());

	m_spc = 1.0f / static_cast<float>(freq.QuadPart);
	m_ts = 1.0f;
	m_fdt = DEFAULT_FIXED_DELTA_TIME;
	m_udt = 0.0f;
	m_dt = 0.0f;
	m_dtBackup = m_dt;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceCounter(&m_basePC) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"TimeImpl::Init() > QueryPerformanceCounter()", GetLastError());

	m_deltaPC.QuadPart = 0;
	m_prevPC = m_basePC;
	m_currPC = m_basePC;
	m_pausedPC.QuadPart = 0;
}

void TimeImpl::Release()
{
}

void TimeImpl::SetTimeScale(float ts)
{
	if (ts < 0.0f)
		return;

	m_ts = ts;
}

bool TimeImpl::SetFixedDeltaTime(float fdt)
{
	if (fdt <= 0.0f)
		return false;

	m_fdt = fdt;
	return true;
}

void TimeImpl::Update()
{
	QueryPerformanceCounter(&m_currPC);
	m_deltaPC.QuadPart = m_currPC.QuadPart - m_prevPC.QuadPart;
	if (m_deltaPC.QuadPart < 0)
		m_deltaPC.QuadPart = 0;

	m_udt = static_cast<float>(m_deltaPC.QuadPart) * m_spc;
	m_dt = m_udt * m_ts;

	m_prevPC = m_currPC;
}

void TimeImpl::ChangeDeltaTimeToFixedDeltaTime()
{
	m_dtBackup = m_dt;
	m_dt = m_fdt;
}

void TimeImpl::RecoverDeltaTime()
{
	m_dt = m_dtBackup;
}

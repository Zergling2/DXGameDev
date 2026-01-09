#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

static constexpr uint32_t FIXED_UPDATE_HZ = 50;
static constexpr float FIXED_DELTA_TIME = 1.0f / static_cast<float>(FIXED_UPDATE_HZ);

Time* Time::s_pInstance = nullptr;

void Time::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new Time();
}

void Time::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void Time::Init()
{
	LARGE_INTEGER freq;

	// On systems that run Windows XP or later, 
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceFrequency(&freq) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"QueryPerformanceFrequency()", GetLastError());

	if (freq.QuadPart % FIXED_UPDATE_HZ != 0)
		Debug::ForceCrashWithMessageBox(L"FixedUpdateHz Error", L"Hardware counters are not supported at 50 Hz.");

	m_spc = 1.0f / static_cast<float>(freq.QuadPart);
	m_ts = 1.0f;
	m_fdt = FIXED_DELTA_TIME;
	m_udt = 0.0f;
	m_dt = 0.0f;
	m_dtBackup = m_dt;
	m_fdtPC.QuadPart = freq.QuadPart / FIXED_UPDATE_HZ;

	// On systems that run Windows XP or later,
	// the function will always succeed and will thus never return zero.
	if (QueryPerformanceCounter(&m_basePC) == FALSE)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"QueryPerformanceCounter()", GetLastError());

	m_deltaPC.QuadPart = 0;
	m_prevPC = m_basePC;
	m_currPC = m_basePC;
	m_pausedPC.QuadPart = 0;
}

void Time::UnInit()
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
	QueryPerformanceCounter(&m_currPC);

	m_deltaPC.QuadPart = m_currPC.QuadPart - m_prevPC.QuadPart;
	if (m_deltaPC.QuadPart < 0)
		m_deltaPC.QuadPart = 0;

	m_udt = static_cast<float>(m_deltaPC.QuadPart) * m_spc;
	m_dt = m_udt * m_ts;

	m_prevPC = m_currPC;
}

void Time::ChangeDeltaTimeToFixedDeltaTime()
{
	m_dtBackup = m_dt;
	m_dt = m_fdt;
}

void Time::RecoverDeltaTime()
{
	m_dt = m_dtBackup;
}

#include <ZergEngine\CoreSystem\COMInitializer.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(COMInitializer);

COMInitializer::COMInitializer()
	: m_init(false)
{
}

COMInitializer::~COMInitializer()
{
	this->Release();
}

void COMInitializer::Init(void* pDesc)
{
	HRESULT hr;

	if (!m_init)
	{
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (SUCCEEDED(hr))
			m_init = true;
		else
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"COMInitializer::Init()", hr);
	}
}

void COMInitializer::Release()
{
	if (m_init)
	{
		CoUninitialize();
		m_init = false;		// ºÒÇÊ¿ä
	}
}

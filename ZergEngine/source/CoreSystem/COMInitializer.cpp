#include <ZergEngine\CoreSystem\COMInitializer.h>
#include <ZergEngine\CoreSystem\Debug.h>

namespace ze
{
	COMInitializerImpl COMInitializer;
}

using namespace ze;

COMInitializerImpl::COMInitializerImpl()
	: m_init(false)
{
}

COMInitializerImpl::~COMInitializerImpl()
{
	this->Release();
}

void COMInitializerImpl::Init(void* pDesc)
{
	HRESULT hr;

	if (!m_init)
	{
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (SUCCEEDED(hr))
			m_init = true;
		else
			Debug::ForceCrashWithHRESULTErrorMessageBox(L"COMInitializerImpl::Init()", hr);
	}
}

void COMInitializerImpl::Release()
{
	if (m_init)
	{
		CoUninitialize();
		m_init = false;		// ºÒÇÊ¿ä
	}
}

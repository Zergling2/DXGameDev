#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(FileSystem);

FileSystem::FileSystem()
	: m_exePath{}
	, m_exeRelPath{}
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::Init(void* pDesc)
{
	const DWORD ret = GetModuleFileNameW(NULL, m_exePath, _countof(m_exePath));
	const DWORD ec = GetLastError();

	if ((ret != 0 && ec == ERROR_INSUFFICIENT_BUFFER) || ret == 0)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"FileSystem::Init()", ec);

	HRESULT hr;

	hr = StringCbCopyW(m_exeRelPath, sizeof(m_exeRelPath), m_exePath);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"FileSystem::Init()", hr);

	*(wcsrchr(m_exeRelPath, L'\\') + 1) = L'\0';
}

void FileSystem::Release()
{
	// Nothing to do.
}

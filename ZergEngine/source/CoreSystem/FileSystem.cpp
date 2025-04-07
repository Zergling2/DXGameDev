#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>

namespace ze
{
	FileSystemImpl FileSystem;
}

using namespace ze;

FileSystemImpl::FileSystemImpl()
	: m_exePath{}
	, m_exeRelPath{}
{
}

FileSystemImpl::~FileSystemImpl()
{
}

void FileSystemImpl::Init(void* pDesc)
{
	const DWORD ret = GetModuleFileNameW(NULL, m_exePath, _countof(m_exePath));
	const DWORD ec = GetLastError();

	if ((ret != 0 && ec == ERROR_INSUFFICIENT_BUFFER) || ret == 0)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"FileSystemImpl::Init()", ec);

	HRESULT hr;

	hr = StringCbCopyW(m_exeRelPath, sizeof(m_exeRelPath), m_exePath);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"FileSystemImpl::Init()", hr);

	*(wcsrchr(m_exeRelPath, L'\\') + 1) = L'\0';
}

void FileSystemImpl::Release()
{
	// Nothing to do.
}

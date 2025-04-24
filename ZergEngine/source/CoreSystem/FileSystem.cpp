#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>

namespace ze
{
	FileSystemImpl FileSystem;
}

using namespace ze;

FileSystemImpl::FileSystemImpl()
{
	// ZeroMemory(m_appPath, sizeof(m_appPath));
	// ZeroMemory(m_appFolderPath, sizeof(m_appFolderPath));
}

FileSystemImpl::~FileSystemImpl()
{
}

void FileSystemImpl::Init(void* pDesc)
{
	const DWORD ret = GetModuleFileNameW(NULL, m_appPath, _countof(m_appPath));
	const DWORD ec = GetLastError();

	if ((ret != 0 && ec == ERROR_INSUFFICIENT_BUFFER) || ret == 0)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"FileSystemImpl::Init()", ec);

	HRESULT hr;

	hr = StringCbCopyW(m_appFolderPath, sizeof(m_appFolderPath), m_appPath);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"FileSystemImpl::Init()", hr);

	*(wcsrchr(m_appFolderPath, L'\\') + 1) = L'\0';		// ...\...\MyGames\TheGame\'\0'
}

void FileSystemImpl::Release()
{
	// Nothing to do.
}

HRESULT FileSystemImpl::RelativePathToFullPath(PCWSTR relativePath, PWSTR buffer, size_t cbBufSize)
{
	HRESULT hr = S_OK;

	do
	{
		hr = StringCbCopyW(buffer, cbBufSize, GetAppFolderPath());
		if (FAILED(hr))
			break;

		hr = StringCbCatW(buffer, cbBufSize, relativePath);
		if (FAILED(hr))
			break;
	} while (false);

	return hr;
}

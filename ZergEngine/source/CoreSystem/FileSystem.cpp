#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

FileSystem* FileSystem::s_pInstance = nullptr;

void FileSystem::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new FileSystem();
}

void FileSystem::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void FileSystem::Init()
{
	const DWORD ret = GetModuleFileNameW(NULL, m_appPath, _countof(m_appPath));
	const DWORD ec = GetLastError();

	if ((ret != 0 && ec == ERROR_INSUFFICIENT_BUFFER) || ret == 0)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"FileSystem::Init()", ec);

	HRESULT hr;

	hr = StringCbCopyW(m_appFolderPath, sizeof(m_appFolderPath), m_appPath);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"FileSystem::Init()", hr);

	*(wcsrchr(m_appFolderPath, L'\\') + 1) = L'\0';		// ...\...\MyGames\TheGame\'\0'
}

void FileSystem::Shutdown()
{
	// Nothing to do.
}

HRESULT FileSystem::RelativePathToFullPath(PCWSTR relativePath, PWSTR buffer, size_t cbBufSize)
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

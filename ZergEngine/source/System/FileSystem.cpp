#include <ZergEngine\System\FileSystem.h>

using namespace zergengine;

WCHAR FileSystem::s_exePath[MAX_PATH];
WCHAR FileSystem::s_resourcePath[MAX_PATH];

sysresult FileSystem::SysStartup(const EngineSystemInitDescriptor& desc)
{
	const DWORD ret = GetModuleFileNameW(NULL, FileSystem::s_exePath, _countof(FileSystem::s_exePath));
	if (ret == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		return -1;

	errno_t e;
	e = wcscpy_s(FileSystem::s_resourcePath, FileSystem::s_exePath);
	if (e != 0)
		return -1;

	*(wcsrchr(FileSystem::s_resourcePath, L'\\') + 1) = L'\0';

	e = wcscat_s(FileSystem::s_resourcePath, L"Resource\\");
	if (e != 0)
		return -1;

	return 0;
}

void FileSystem::SysCleanup()
{
}

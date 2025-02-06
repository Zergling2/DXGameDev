#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class FileSystem
	{
		friend class Engine;
	private:
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();
	public:
		static inline LPCWSTR GetExePath() { return FileSystem::s_exePath; }
		static inline LPCWSTR GetResourcePath() { return FileSystem::s_resourcePath; }
	private:
		static WCHAR s_exePath[MAX_PATH];
		static WCHAR s_resourcePath[MAX_PATH];
	};
}

#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class FileSystem
	{
		friend class Runtime;
	public:
		static FileSystem* GetInstance() { return s_pInstance; }
	private:
		FileSystem() = default;
		~FileSystem() = default;

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void Shutdown();
	public:
		PCWSTR GetAppPath() { return m_appPath; }
		PCWSTR GetAppFolderPath() { return m_appFolderPath; }
		HRESULT RelativePathToFullPath(PCWSTR relativePath, PWSTR buffer, size_t cbBufSize);
	private:
		static FileSystem* s_pInstance;
		WCHAR m_appPath[MAX_PATH];
		WCHAR m_appFolderPath[MAX_PATH];
	};
}

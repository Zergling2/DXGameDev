#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class FileSystemImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(FileSystemImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		PCWSTR GetAppPath() { return m_appPath; }
		PCWSTR GetAppFolderPath() { return m_appFolderPath; }
		HRESULT RelativePathToFullPath(PCWSTR relativePath, PWSTR buffer, size_t cbBufSize);
	private:
		WCHAR m_appPath[MAX_PATH];
		WCHAR m_appFolderPath[MAX_PATH];
	};

	extern FileSystemImpl FileSystem;
}

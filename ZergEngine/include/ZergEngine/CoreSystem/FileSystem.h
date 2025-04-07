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
		inline PCWSTR GetExePath() { return m_exePath; }
		inline PCWSTR GetExeRelativePath() { return m_exeRelPath; }
	private:
		WCHAR m_exePath[MAX_PATH];
		WCHAR m_exeRelPath[MAX_PATH];
	};

	extern FileSystemImpl FileSystem;
}

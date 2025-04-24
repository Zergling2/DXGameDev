#include <ZergEngine\Common\EngineHelper.h>

using namespace ze;

void Helper::SafeCloseWinThreadHandle(HANDLE& handle)
{
	if (handle != NULL)
	{
		CloseHandle(handle);
		handle = NULL;
	}
}

void Helper::AutoCRTFileCloser::Close()
{
	if (m_pFile != nullptr)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}
}

#include <ZergEngine\Common\EngineHelper.h>

using namespace ze;

void ze::SafeCloseCRTFile(FILE*& pFile)
{
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
}

void ze::SafeCloseWinThreadHandle(HANDLE& handle)
{
	if (handle != NULL)
	{
		CloseHandle(handle);
		handle = NULL;
	}
}

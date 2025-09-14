#include <ZergEngine\ZergEngine.h>

#if defined(DEBUG) || defined(_DEBUG)
#if defined(UNICODE) || defined(_UNICODE)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

using namespace ze;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks 
	// and generate an error report if the application failed to free all the memory it allocated.
#endif

	Runtime::CreateInstance();
	Runtime::GetInstance()->Init(hInstance, nShowCmd, 1280, 960, L"Simple FPS", L"Warehouse");

	Runtime::GetInstance()->Run();

	Runtime::GetInstance()->UnInit();
	Runtime::DestroyInstance();

	return 0;
}

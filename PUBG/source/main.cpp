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
	Runtime::CreateInstance();
	Runtime::GetInstance()->Init(hInstance, nShowCmd, 1366, 768, L"BattleRoyale", L"TestScene1");

	Runtime::GetInstance()->Run();

	Runtime::GetInstance()->UnInit();
	Runtime::DestroyInstance();

	return 0;
}

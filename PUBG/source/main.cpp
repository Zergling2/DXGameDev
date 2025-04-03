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
	Runtime::Run(hInstance, nShowCmd, L"PUBG", L"Lobby", SIZE{ 1280, 720 }, false);

	return 0;
}

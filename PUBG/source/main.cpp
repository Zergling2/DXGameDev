#include <ZergEngine\ZergEngine.h>
#include "Scene\Lobby.h"

#if defined(DEBUG) || defined(_DEBUG)
#if defined(UNICODE) || defined(_UNICODE)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

using namespace zergengine;
using namespace pubg;

IScene* LobbySceneFactory() { return new Lobby(); }

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	SceneTableMap stm;
	stm[L"Lobby"] = LobbySceneFactory;

	return Engine::Start(hInstance, nShowCmd, L"PUBG", stm, L"Lobby", 1280, 720, false);
}

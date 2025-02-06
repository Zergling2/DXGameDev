#include <ZergEngine\System\Runtime.h>
#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\FileSystem.h>
#include <ZergEngine\System\Time.h>
#include <ZergEngine\System\Input.h>
#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\System\Resource.h>
#include <ZergEngine\System\Scene.h>
#include <ZergEngine\System\SceneManager.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\GameObjectManager.h>
#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\System\Component\Camera.h>
#include <ZergEngine\System\Component\IScript.h>

using namespace zergengine;

static LPCWSTR WNDCLASS_NAME = L"EngineWnd";

bool Engine::s_coInited = false;
SYSTEM_INFO Engine::s_sysInfo;
uint32_t Engine::s_wndStateFlag = Engine::WINDOW_STATE_FLAG::NORMAL;
HINSTANCE Engine::s_hInstance = NULL;
std::wstring Engine::s_startSceneName;
HWND Engine::s_hMainWnd = NULL;
float Engine::s_loopTime = 0.0f;

sysresult Engine::Start(HINSTANCE hInstance, int nShowCmd, LPCWSTR wndTitle, const SceneTableMap& stm, LPCWSTR startSceneName,
    uint32_t width, uint32_t height, bool fullscreen)
{
    if (Engine::Startup(hInstance, wndTitle, stm, startSceneName, width, height, fullscreen) < 0)
    {
        Engine::Cleanup();
        return -1;
    }

    ShowWindow(Engine::GetMainWindowHandle(), nShowCmd);
    // UpdateWindow(Engine::GetMainWindowHandle());

    // update initial time value
    Time::Update();

    MSG msg;
    do
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Engine::RuntimeLoop();
        }
    } while (msg.message != WM_QUIT);

    Engine::Cleanup();

    return static_cast<int>(msg.wParam);
}

void Engine::Exit()
{
    DestroyWindow(Engine::GetMainWindowHandle());
}

LRESULT Engine::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        Graphics::RequestResolution(LOWORD(lParam), HIWORD(lParam), false);
        if (!(Engine::GetWindowStateFlag() & WINDOW_STATE_FLAG::IS_RESIZING))       // e.g.) Changed by MoveWindow function...
            Graphics::CommitResolution();
        return 0;
    case WM_ACTIVATE:
        LOWORD(wParam) == WA_INACTIVE ? Graphics::DisableRendering() : Graphics::EnableRendering();
        return 0;
    case WM_ENTERSIZEMOVE:
        Engine::SetWindowStateFlag(WINDOW_STATE_FLAG::IS_RESIZING);
        Graphics::DisableRendering();
        return 0;
    case WM_EXITSIZEMOVE:
        Engine::UnsetWindowStateFlag(WINDOW_STATE_FLAG::IS_RESIZING);
        Graphics::CommitResolution();
        Graphics::EnableRendering();
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

sysresult Engine::Startup(HINSTANCE hInstance, LPCWSTR wndTitle, const SceneTableMap& stm, LPCWSTR startSceneName,
    uint32_t width, uint32_t height, bool fullscreen)
{
    if (!XMVerifyCPUSupport())
        return -1;

    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
        Engine::s_coInited = true;
    else
        return -1;

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks 
    // and generate an error report if the application failed to free all the memory it allocated.
#endif

    Engine::s_hInstance = hInstance;
    Engine::s_startSceneName = startSceneName;

    ::GetSystemInfo(&Engine::s_sysInfo);

    // ######################################################
    // Create window
    // Register the window class.
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Engine::WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = hInstance;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = WNDCLASS_NAME;
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    int wndFrameWidth;
    int wndFrameHeight;

    if (fullscreen)
    {
        wndFrameWidth = GetSystemMetrics(SM_CXSCREEN);
        wndFrameHeight = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        RECT cr = RECT{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        if (AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW, FALSE) == FALSE)
        {
            printf("[AdjustWindowRect()] error code: %u", GetLastError());
            return -1;
        }
        wndFrameWidth = cr.right - cr.left;
        wndFrameHeight = cr.bottom - cr.top;
    }

    // Create the window.
    Engine::s_hMainWnd = CreateWindowW(
        WNDCLASS_NAME,                                              // Window class
        wndTitle,                                                   // Window title
        fullscreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW,   // Window style
        CW_USEDEFAULT,                                              // Pos X
        CW_USEDEFAULT,                                              // Pos Y
        wndFrameWidth,                                              // Width
        wndFrameHeight,                                             // Height
        NULL,                                                       // Parent window    
        NULL,                                                       // Menu
        hInstance,                                                  // Instance handle
        nullptr                                                     // Additional application data
    );

    if (!Engine::GetMainWindowHandle())
        return -1;

    // ######################################################
    // Create system modules__SINSTANCE(Time).SysCleanup();
    EngineSystemInitDescriptor descESI = {
        fullscreen ? wndFrameWidth : width,
        fullscreen ? wndFrameHeight : height,
        fullscreen,
        stm
    };

    // 객체 생성으로 대신하자..
    AsyncConsoleLogger::SysStartup(descESI);
    FileSystem::SysStartup(descESI);
    AsyncFileLogger::SysStartup(descESI);
    Time::SysStartup(descESI);
    Graphics::SysStartup(descESI);
    Input::SysStartup(descESI);
    SceneManager::SysStartup(descESI);
    // ######################################################

    return 0;
}

void Engine::RuntimeLoop()
{
    // Update timer.
    Time::Update();

    Graphics::HandleDevice();
    if (SceneManager::HandleSceneChange())
        Engine::s_loopTime = 0.0f;  // Reset fixedupdate loop timer.

    Input::Update();

    // FixedUpdate
    Engine::s_loopTime += Time::GetUnscaledDeltaTime();

    Time::SwitchDeltaTimeToFixedDeltaTime();
    while (Time::GetFixedDeltaTime() <= Engine::s_loopTime)
    {
        ComponentSystem::ScriptManager::FixedUpdateScripts();
        Engine::s_loopTime -= Time::GetFixedDeltaTime();
    }
    Time::RecoverDeltaTime();

    ComponentSystem::ScriptManager::UpdateScripts();
    ComponentSystem::ScriptManager::LateUpdateScripts();

    // Render
    Graphics::RenderFrame();

    // Input::Clear();
    Sleep(6);       // 나중에 Max fps 기능 추가해야함
}

void Engine::Cleanup()
{
    // ######################################################
    // Cleanup all system modules (reversed order)
    SceneManager::SysCleanup();
    Input::SysCleanup();
    Graphics::SysCleanup();
    Time::SysCleanup();
    AsyncFileLogger::SysCleanup();
    FileSystem::SysCleanup();
    AsyncConsoleLogger::SysCleanup();
    // ######################################################

    UnregisterClassW(WNDCLASS_NAME, Engine::GetInstanceHandle());

    if (Engine::s_coInited)
    {
        CoUninitialize();
        Engine::s_coInited = false;
    }
}

void zergengine::DontDestroyOnLoad(std::shared_ptr<GameObject>& gameObject)
{
    gameObject->m_dontDestroyOnLoad = true;
}

void zergengine::Destroy(std::shared_ptr<GameObject>& gameObject)
{
    if (gameObject == nullptr)
        return;

    if (!gameObject->IsDestroyed())
    {
        gameObject->m_destroyed = true;
        gameObject->DestroyAllComponents();
        GameObjectManager::RemoveGameObject(gameObject.get());
    }

    gameObject.reset();
}

void zergengine::Destroy(std::shared_ptr<IComponent>& component)
{
    if (component == nullptr)
        return;

    ComponentSystem::RemoveComponentFromSystemAndComponentList(component);

    component.reset();
}

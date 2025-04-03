#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\SystemInfo.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\COMInitializer.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\SceneManager.h>
#include <ZergEngine\CoreSystem\ResourceManager.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\Renderer.h>
#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\GameObjectManager.h>
#include <ZergEngine\CoreSystem\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\CameraManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\PointLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\SpotLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ScriptManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\TerrainManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Runtime);

HINSTANCE Runtime::s_hInstance;
std::wstring Runtime::s_startScene;
float Runtime::s_loopTime;
uint32_t Runtime::s_flag;

Runtime::Runtime()
{
}

Runtime::~Runtime()
{
}

void Runtime::Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCWSTR startScene, SIZE resolution, bool fullscreen)
{
    Runtime::s_hInstance = hInstance;
    Runtime::s_startScene = startScene;
    Runtime::s_loopTime = 0.0f;
    Runtime::s_flag = RENDER_ENABLED | SCRIPT_UPDATE;

    Runtime::InitAllSubsystem(wndTitle, resolution, fullscreen);

    ShowWindow(Window::GetInstance().GetWindowHandle(), nShowCmd);
    // UpdateWindow(Runtime::GetMainWindowHandle());

    // update initial time value
    Time::GetInstance().Update();

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
            Runtime::OnIdle();
        }
    } while (msg.message != WM_QUIT);

    const int ret = static_cast<int>(msg.wParam);

    Runtime::ReleaseAllSubsystem();
}

void Runtime::Exit()
{
    DestroyWindow(Window::GetInstance().GetWindowHandle());
}

void Runtime::InitAllSubsystem(PCWSTR wndTitle, SIZE resolution, bool fullscreen)
{
    const time_t rawTime = time(nullptr);
    tm timeInfo;
    localtime_s(&timeInfo, &rawTime);

    WCHAR syncLogName[64];
    StringCbPrintfW(syncLogName, sizeof(syncLogName),
        L"Log%d%02d%02d-%02d%02d%02d.log",
        1900 + timeInfo.tm_year,
        1 + timeInfo.tm_mon,
        timeInfo.tm_mday,
        timeInfo.tm_hour,
        timeInfo.tm_min,
        timeInfo.tm_sec
    );

    if (!XMVerifyCPUSupport())
        Debug::ForceCrashWithMessageBox(L"Error", L"DirectXMath Library does not supports a given platform.");

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks 
    // and generate an error report if the application failed to free all the memory it allocated.
#endif

    SystemInfo::GetInstance().Init(nullptr);
    MemoryAllocator::GetInstance().Init(nullptr);
    FileSystem::GetInstance().Init(nullptr);
    GlobalLog::GetInstance().Init(syncLogName);
    COMInitializer::GetInstance().Init(nullptr);
    Time::GetInstance().Init(nullptr);

    Window::InitDesc initDesc;
    initDesc.m_resolution = resolution;
    initDesc.m_fullscreen = fullscreen;
    initDesc.m_title = wndTitle;
    Window::GetInstance().Init(&initDesc);

    GraphicDevice::GetInstance().Init(nullptr);
    SceneManager::GetInstance().Init(nullptr);
    ResourceManager::GetInstance().Init(nullptr);
    Renderer::GetInstance().Init(nullptr);
    Input::GetInstance().Init(nullptr);
    GameObjectManager::GetInstance().Init(nullptr);
    Environment::GetInstance().Init(nullptr);
    CameraManager::GetInstance().Init(nullptr);
    DirectionalLightManager::GetInstance().Init(nullptr);
    PointLightManager::GetInstance().Init(nullptr);
    SpotLightManager::GetInstance().Init(nullptr);
    MeshRendererManager::GetInstance().Init(nullptr);
    ScriptManager::GetInstance().Init(nullptr);
    TerrainManager::GetInstance().Init(nullptr);
}

void Runtime::ReleaseAllSubsystem()
{
    TerrainManager::GetInstance().Release();
    ScriptManager::GetInstance().Release();
    MeshRendererManager::GetInstance().Release();
    SpotLightManager::GetInstance().Release();
    PointLightManager::GetInstance().Release();
    DirectionalLightManager::GetInstance().Release();
    CameraManager::GetInstance().Release();
    Environment::GetInstance().Release();
    GameObjectManager::GetInstance().Release();
    Input::GetInstance().Release();
    Renderer::GetInstance().Release();
    ResourceManager::GetInstance().Release();
    SceneManager::GetInstance().Release();
    GraphicDevice::GetInstance().Release();
    Window::GetInstance().Release();
    Time::GetInstance().Release();
    COMInitializer::GetInstance().Release();
    GlobalLog::GetInstance().Release();
    FileSystem::GetInstance().Release();
    MemoryAllocator::GetInstance().Release();
    SystemInfo::GetInstance().Release();
}

void Runtime::OnIdle()
{
    // Update timer.
    Time::GetInstance().Update();

    SceneManager::GetInstance().Update(&Runtime::s_loopTime);

    Input::GetInstance().Update();

    // For the FixedUpdate
    Runtime::s_loopTime += Time::GetInstance().GetUnscaledDeltaTime();

    // 정수 기준 루프로 수정 필요
    Time::GetInstance().ChangeDeltaTimeToFixedDeltaTime();
    while (Time::GetInstance().GetFixedDeltaTime() <= Runtime::s_loopTime)
    {
        ScriptManager::GetInstance().FixedUpdateScripts();
        Runtime::s_loopTime -= Time::GetInstance().GetFixedDeltaTime();
    }
    Time::GetInstance().RecoverDeltaTime();

    ScriptManager::GetInstance().UpdateScripts();
    ScriptManager::GetInstance().LateUpdateScripts();

    // Render
    if (Runtime::s_flag & RUNTIME_FLAG::RENDER_ENABLED)
        Renderer::GetInstance().RenderFrame();

    // Input::GetInstance().Clear();
    
    Sleep(6);       // 나중에 Max fps 기능 추가해야함
}

GameObjectHandle Runtime::CreateGameObject(PCWSTR name)
{
    GameObjectHandle hGameObject;

    do
    {
        GameObject* pGameObject = new(std::nothrow) GameObject(false, name);
        if (!pGameObject)
            break;

        hGameObject = GameObjectManager::GetInstance().Register(pGameObject);
    } while (false);

    return hGameObject;
}

void Runtime::DontDestroyOnLoad(GameObjectHandle gameObject)
{
    GameObject* pGameObject = gameObject.ToPtr();
    if (!pGameObject)
        return;

    pGameObject->m_flag = static_cast<GAMEOBJECT_FLAG>(pGameObject->m_flag | GOF_DONT_DESTROY_ON_LOAD);
}

/*
Destroy routine!
1. check is already destroyed.
2. unregister
3. set destroyed flag
*/

void Runtime::Destroy(GameObjectHandle hGameObject)
{
    // GameObjectManager의 Unregister()를 호출해주고 메모리 해제도 해주어야 한다.
    // SceneManager에서 GameObject 파괴하는 부분 참고해서 최대한 실수 없이 만들자.

    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    assert(pGameObject->IsDeferred() == false);

    // 모든 컴포넌트 제거
    Runtime::DestroyAllComponents(hGameObject);

    GameObjectManager::GetInstance().Unregister(pGameObject);        // 전역 관리자에서 제거

    delete pGameObject; // 메모리 해제
}

void Runtime::DestroyAllComponents(GameObjectHandle hGameObject)
{
    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    assert(pGameObject->IsDeferred() == false);

    while (!pGameObject->m_components.empty())
    {
        ComponentHandle hComponent = *pGameObject->m_components.begin();
        assert(hComponent.ToPtr() != nullptr);      // 컴포넌트는 자신이 제거될 때 자신을 소유하는 게임 오브젝트에 접근해 자신의 핸들을 제거한다.

        Destroy(hComponent);
    }
}

void Runtime::Destroy(ComponentHandle hComponent)
{
    IComponent* pComponent = hComponent.ToPtr();
    if (!pComponent)
        return;

    GameObject* pGameObject = pComponent->GetGameObjectHandle().ToPtr();
    assert(pGameObject != nullptr);

    // GameObject의 컴포넌트 리스트에서 제거
    auto& components = pGameObject->m_components;

    const auto end = components.cend();
    auto iter = components.cbegin();
    while (iter != end)
    {
        if (hComponent == *iter)
        {
            components.erase(iter);
            break;
        }
        ++iter;
    }

    IComponentManager* pComponentManager = pComponent->GetComponentManager();
    pComponentManager->Unregister(pComponent);

    delete pComponent;  // 메모리 해제
}

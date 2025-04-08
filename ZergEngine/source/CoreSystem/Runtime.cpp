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

namespace ze
{
    RuntimeImpl Runtime;
}

using namespace ze;

enum RUNTIME_FLAG : uint32_t
{
    RENDER_ENABLED = 0x00000001,
    SCRIPT_UPDATE = 0x00000002
};

RuntimeImpl::RuntimeImpl()
    : m_hInstance(NULL)
    , m_startScene()
    , m_fixedUpdateTimer(0.0f)
    , m_flag(0)
{
}

RuntimeImpl::~RuntimeImpl()
{
}

void RuntimeImpl::Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCWSTR startScene, uint32_t width, uint32_t height, bool fullscreen)
{
    m_hInstance = hInstance;
    m_startScene = startScene;
    m_fixedUpdateTimer = 0.0f;
    m_flag = RENDER_ENABLED | SCRIPT_UPDATE;

    RuntimeImpl::InitAllSubsystem(wndTitle, width, height, fullscreen);

    ShowWindow(Window.GetWindowHandle(), nShowCmd);
    // UpdateWindow(Runtime.GetMainWindowHandle());

    // update initial time value
    Time.Update();

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
            RuntimeImpl::OnIdle();
        }
    } while (msg.message != WM_QUIT);

    const int ret = static_cast<int>(msg.wParam);

    RuntimeImpl::ReleaseAllSubsystem();

#if defined(DEBUG) || defined(_DEBUG)
    HMODULE dxgiDebugDll = GetModuleHandleW(L"dxgidebug.dll");
    if (dxgiDebugDll != NULL)
    {
        decltype(&DXGIGetDebugInterface) DebugInterface =
            reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgiDebugDll, "DXGIGetDebugInterface"));

        IDXGIDebug* pDXGIDebug;
        DebugInterface(IID_PPV_ARGS(&pDXGIDebug));

        OutputDebugStringW(L"##### DXGI DEBUG INFO START #####\n");
        pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
        OutputDebugStringW(L"##### DXGI DEBUG INFO END #####\n");

        pDXGIDebug->Release();
    }
    else
    {
        OutputDebugStringW(L"Failed to get dxgidebug.dll module handle. Can't use ReportLiveObjects() function.\n");
    }
#endif
}

void RuntimeImpl::Exit()
{
    DestroyWindow(Window.GetWindowHandle());
}

void RuntimeImpl::EnableRendering()
{
    m_flag |= RENDER_ENABLED;
}

void RuntimeImpl::DisableRendering()
{
    m_flag &= ~RENDER_ENABLED;
}

GameObjectHandle RuntimeImpl::Find(PCWSTR name)
{
    return GameObjectManager.FindGameObject(name);
}

void RuntimeImpl::InitAllSubsystem(PCWSTR wndTitle, uint32_t width, uint32_t height, bool fullscreen)
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

    SystemInfo.Init(nullptr);
    MemoryAllocator.Init(nullptr);
    FileSystem.Init(nullptr);
    GlobalLog.Init(syncLogName);
    COMInitializer.Init(nullptr);
    Time.Init(nullptr);

    WindowImpl::InitDesc initDesc;
    initDesc.m_width = width;
    initDesc.m_height = height;
    initDesc.m_fullscreen = fullscreen;
    initDesc.m_title = wndTitle;
    Window.Init(&initDesc);

    GraphicDevice.Init(nullptr);
    SceneManager.Init(nullptr);
    Resource.Init(nullptr);
    Renderer.Init(nullptr);
    Input.Init(nullptr);
    GameObjectManager.Init(nullptr);
    Environment.Init(nullptr);
    CameraManager.Init(nullptr);
    DirectionalLightManager.Init(nullptr);
    PointLightManager.Init(nullptr);
    SpotLightManager.Init(nullptr);
    MeshRendererManager.Init(nullptr);
    ScriptManager.Init(nullptr);
    TerrainManager.Init(nullptr);
}

void RuntimeImpl::ReleaseAllSubsystem()
{
    TerrainManager.Release();
    ScriptManager.Release();
    MeshRendererManager.Release();
    SpotLightManager.Release();
    PointLightManager.Release();
    DirectionalLightManager.Release();
    CameraManager.Release();
    Environment.Release();
    GameObjectManager.Release();
    Input.Release();
    Renderer.Release();
    Resource.Release();
    SceneManager.Release();
    GraphicDevice.Release();
    Window.Release();
    Time.Release();
    COMInitializer.Release();
    GlobalLog.Release();
    FileSystem.Release();
    MemoryAllocator.Release();
    SystemInfo.Release();
}

void RuntimeImpl::OnIdle()
{
    // Update timer.
    Time.Update();

    SceneManager.Update(&m_fixedUpdateTimer);

    Input.Update();

    // For the FixedUpdate
    m_fixedUpdateTimer += Time.GetUnscaledDeltaTime();
    Time.ChangeDeltaTimeToFixedDeltaTime();
    while (Time.GetFixedDeltaTime() <= m_fixedUpdateTimer)
    {
        ScriptManager.FixedUpdateScripts();
        m_fixedUpdateTimer -= Time.GetFixedDeltaTime();
    }
    Time.RecoverDeltaTime();

    ScriptManager.UpdateScripts();
    ScriptManager.LateUpdateScripts();

    Input.FinalUpdate();

    // Render
    if (m_flag & RUNTIME_FLAG::RENDER_ENABLED)
        Renderer.RenderFrame();
    
    Sleep(5);       // 나중에 Max fps 기능 추가해야함
}

GameObjectHandle RuntimeImpl::CreateGameObject(PCWSTR name)
{
    GameObjectHandle hGameObject;

    do
    {
        GameObject* pGameObject = new(std::nothrow) GameObject(false, name);
        if (!pGameObject)
            break;

        hGameObject = GameObjectManager.Register(pGameObject);
    } while (false);

    return hGameObject;
}

void RuntimeImpl::DontDestroyOnLoad(GameObjectHandle gameObject)
{
    GameObject* pGameObject = gameObject.ToPtr();
    if (!pGameObject)
        return;

    pGameObject->m_flag = static_cast<GAMEOBJECT_FLAG>(pGameObject->m_flag | GOF_DONT_DESTROY_ON_LOAD);
}

void RuntimeImpl::Destroy(GameObjectHandle hGameObject)
{
    // GameObjectManager의 Unregister()를 호출해주고 메모리 해제도 해주어야 한다.
    // SceneManager에서 GameObject 파괴하는 부분 참고해서 최대한 실수 없이 만들자.

    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    assert(pGameObject->IsDeferred() == false);

    // 모든 컴포넌트 제거
    RuntimeImpl::DestroyAllComponents(hGameObject);

    GameObjectManager.Unregister(pGameObject);        // 전역 관리자에서 제거

    delete pGameObject; // 메모리 해제
}

void RuntimeImpl::DestroyAllComponents(GameObjectHandle hGameObject)
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

void RuntimeImpl::Destroy(ComponentHandle hComponent)
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

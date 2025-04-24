#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\SystemInfo.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\COMInitializer.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\Manager\ResourceManager.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TransformManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
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

void RuntimeImpl::Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCSTR startScene, uint32_t width, uint32_t height, bool fullscreen)
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

    auto& activeGameObjects = GameObjectManager.m_activeGameObjects;
    for (GameObject* pGameObject : activeGameObjects)
        if (!pGameObject->IsDontDestroyOnLoad())
            Runtime.Destroy(pGameObject);

    Runtime.RemoveDestroyedComponentsAndGameObjects();

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

    WCHAR engineLogName[64];
    StringCbPrintfW(engineLogName, sizeof(engineLogName),
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
    FileLog.Init(engineLogName);
    COMInitializer.Init(nullptr);
    Time.Init(nullptr);

    WindowImpl::InitDesc initDesc;
    initDesc.m_width = width;
    initDesc.m_height = height;
    initDesc.m_fullscreen = fullscreen;
    initDesc.m_title = wndTitle;
    Window.Init(&initDesc);

    Input.Init(nullptr);
    GraphicDevice.Init(nullptr);
    Resource.Init(nullptr);
    Renderer.Init(nullptr);
    GameObjectManager.Init(nullptr);
    Environment.Init(nullptr);
    TransformManager.Init(nullptr);
    CameraManager.Init(nullptr);
    DirectionalLightManager.Init(nullptr);
    PointLightManager.Init(nullptr);
    SpotLightManager.Init(nullptr);
    MeshRendererManager.Init(nullptr);
    TerrainManager.Init(nullptr);
    ScriptManager.Init(nullptr);
    SceneManager.Init(nullptr);
}

void RuntimeImpl::ReleaseAllSubsystem()
{
    SceneManager.Release();
    TerrainManager.Release();
    ScriptManager.Release();
    MeshRendererManager.Release();
    SpotLightManager.Release();
    PointLightManager.Release();
    DirectionalLightManager.Release();
    CameraManager.Release();
    TransformManager.Release();
    Environment.Release();
    GameObjectManager.Release();
    Renderer.Release();
    Resource.Release();
    GraphicDevice.Release();
    Input.Release();
    Window.Release();
    Time.Release();
    COMInitializer.Release();
    FileLog.Release();
    FileSystem.Release();
    MemoryAllocator.Release();
    SystemInfo.Release();
}

void RuntimeImpl::OnIdle()
{
    // Update timer.
    Time.Update();
    Input.Update();

    SceneManager.Update(&m_fixedUpdateTimer);

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

    RemoveDestroyedComponentsAndGameObjects();

    // Render
    if (m_flag & RUNTIME_FLAG::RENDER_ENABLED)
        Renderer.RenderFrame();

    Sleep(5);
}

GameObjectHandle RuntimeImpl::CreateGameObject(PCWSTR name)
{
    GameObjectHandle hGameObject;

    do
    {
        GameObject* pGameObject = new(std::nothrow) GameObject(
            static_cast<GAMEOBJECT_FLAG>(GOF_ACTIVE),
            name
        );
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
    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    this->Destroy(pGameObject);
}

void RuntimeImpl::DestroyAllComponents(GameObjectHandle hGameObject)
{
    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    this->DestroyAllComponents(pGameObject);
}

void RuntimeImpl::Destroy(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    // 자식 게임 오브젝트까지 재귀적으로 삭제
    // 자식과의 연결을 끊는 동작은 최대한 지연시킨다. (OnDestroy에서 최대한의 객체 접근 자유도 보장)
    // 자식과의 연결을 끊는 동작은 GameObjectManager의 RemoveDestroyedGameObjects() 함수에서 수행.
    for (GameObject* pChildGameObject : pGameObject->m_children)
    {
        assert(pChildGameObject != nullptr);
        this->Destroy(pChildGameObject);
    }

    if (pGameObject->IsOnTheDestroyQueue())
        return;

    this->DestroyAllComponents(pGameObject);
    GameObjectManager.AddToDestroyQueue(pGameObject);
}

void RuntimeImpl::Destroy(IComponent* pComponent)
{
    if (pComponent->IsOnTheDestroyQueue())  // 먼저 검사하면 가상함수 호출 비용 절약 가능
        return;

    IComponentManager* pComponentManager = pComponent->GetComponentManager();
    pComponentManager->AddToDestroyQueue(pComponent);
}

void RuntimeImpl::DestroyAllComponents(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    for (IComponent* pComponent : pGameObject->m_components)
    {
        if (pComponent->IsOnTheDestroyQueue())  // 먼저 검사하면 가상함수 호출 비용 절약 가능
            continue;

        IComponentManager* pComponentManager = pComponent->GetComponentManager();
        pComponentManager->AddToDestroyQueue(pComponent);
    }
}

void RuntimeImpl::RemoveDestroyedComponentsAndGameObjects()
{
    // 컴포넌트 제거 작업
    ScriptManager.RemoveDestroyedComponents();
    TransformManager.RemoveDestroyedComponents();
    CameraManager.RemoveDestroyedComponents();
    DirectionalLightManager.RemoveDestroyedComponents();
    PointLightManager.RemoveDestroyedComponents();
    SpotLightManager.RemoveDestroyedComponents();
    MeshRendererManager.RemoveDestroyedComponents();
    TerrainManager.RemoveDestroyedComponents();
    // 반드시 컴포넌트 제거 작업 이후 실행
    GameObjectManager.RemoveDestroyedGameObjects();
}

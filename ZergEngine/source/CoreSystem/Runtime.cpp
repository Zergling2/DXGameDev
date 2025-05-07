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
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>

namespace ze
{
    RuntimeImpl Runtime;
}

using namespace ze;

enum RUNTIME_FLAG : uint32_t
{
    RTF_RENDER_ENABLED = 0x00000001,
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
    m_flag = RTF_RENDER_ENABLED;

    RuntimeImpl::InitAllSubsystem(wndTitle, width, height, fullscreen);

    ShowWindow(Window.GetWindowHandle(), nShowCmd);
    // UpdateWindow(Runtime.GetMainWindowHandle());

    // update initial time value
    Time.Update();

    MSG msg;
    for (;;)
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }
        else
        {
            RuntimeImpl::OnIdle();
        }
    }

    // 모든 게임 오브젝트 및 컴포넌트를 제거
    for (GameObject* pGameObject : GameObjectManager.m_inactiveGameObjects)
        this->Destroy(pGameObject);
    for (GameObject* pGameObject : GameObjectManager.m_activeGameObjects)
        this->Destroy(pGameObject);
    Runtime.RemoveDestroyedComponentsAndGameObjects();

    // 시스템 모듈들 해제
    RuntimeImpl::ReleaseAllSubsystem();

#if defined(DEBUG) || defined(_DEBUG)
    HMODULE hDXGIDebugDll = GetModuleHandleW(L"dxgidebug.dll");
    if (hDXGIDebugDll != NULL)
    {
        decltype(&DXGIGetDebugInterface) DebugInterface =
            reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(hDXGIDebugDll, "DXGIGetDebugInterface"));

        ComPtr<IDXGIDebug> cpDXGIDebug;
        DebugInterface(IID_PPV_ARGS(&cpDXGIDebug));

        OutputDebugStringW(L"##### DXGI DEBUG INFO START #####\n");
        cpDXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
        OutputDebugStringW(L"##### DXGI DEBUG INFO END #####\n");
    }
    else
    {
        OutputDebugStringW(L"Failed to get dxgidebug.dll module handle.\n");
    }
#endif
}

void RuntimeImpl::Exit()
{
    DestroyWindow(Window.GetWindowHandle());
}

void RuntimeImpl::EnableRendering()
{
    m_flag |= RTF_RENDER_ENABLED;
}

void RuntimeImpl::DisableRendering()
{
    m_flag &= ~RTF_RENDER_ENABLED;
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

    // Call IScript::Start() for all starting scripts.
    ScriptManager.CallStart();

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
    if (m_flag & RUNTIME_FLAG::RTF_RENDER_ENABLED)
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

void RuntimeImpl::Destroy(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    // 자식 게임 오브젝트까지 Destroy
    for (Transform* pChildTransform : pGameObject->m_transform.m_children)
        this->Destroy(pChildTransform->m_pGameObject);

    if (pGameObject->IsOnTheDestroyQueue())
        return;

    this->DestroyAllComponents(pGameObject);
    GameObjectManager.AddToDestroyQueue(pGameObject);
}

void RuntimeImpl::Destroy(IComponent* pComponent)
{
    if (pComponent->IsOnTheDestroyQueue())  // 먼저 검사하면 IComponent::GetComponentManager() 가상함수 호출 비용 절약 가능
        return;

    IComponentManager* pComponentManager = pComponent->GetComponentManager();
    pComponentManager->AddToDestroyQueue(pComponent);
}

void RuntimeImpl::DestroyAllComponents(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    for (IComponent* pComponent : pGameObject->m_components)
    {
        if (pComponent->IsOnTheDestroyQueue())  // 먼저 검사하면 IComponent::GetComponentManager() 가상함수 호출 비용 절약 가능
            continue;

        IComponentManager* pComponentManager = pComponent->GetComponentManager();
        pComponentManager->AddToDestroyQueue(pComponent);
    }
}

void RuntimeImpl::RemoveDestroyedComponentsAndGameObjects()
{
    // 컴포넌트 제거 작업
    ScriptManager.RemoveDestroyedComponents();
    CameraManager.RemoveDestroyedComponents();
    DirectionalLightManager.RemoveDestroyedComponents();
    PointLightManager.RemoveDestroyedComponents();
    SpotLightManager.RemoveDestroyedComponents();
    MeshRendererManager.RemoveDestroyedComponents();
    TerrainManager.RemoveDestroyedComponents();

    // 반드시 컴포넌트 제거 작업 이후 실행
    GameObjectManager.RemoveDestroyedGameObjects();
}

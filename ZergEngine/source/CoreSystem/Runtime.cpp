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
#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>

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
    , m_deltaPC(0)
    , m_flag(0)
{
}

RuntimeImpl::~RuntimeImpl()
{
}

void RuntimeImpl::Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCSTR startScene, uint32_t width, uint32_t height, WINDOW_MODE mode)
{
    m_hInstance = hInstance;
    m_startScene = startScene;
    m_deltaPC = 0;
    m_flag = RTF_RENDER_ENABLED;

    RuntimeImpl::InitAllSubsystem(wndTitle, width, height, mode);

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

    // ��� ���� ������Ʈ �� ������Ʈ�� ����
    for (GameObject* pGameObject : GameObjectManager.m_activeGameObjects)
        Runtime.Destroy(pGameObject);
    for (GameObject* pGameObject : GameObjectManager.m_inactiveGameObjects)
        Runtime.Destroy(pGameObject);
    for (IUIObject* pUIObject : UIObjectManager.m_activeUIObjects)
        Runtime.Destroy(pUIObject);
    for (IUIObject* pUIObject : UIObjectManager.m_inactiveUIObjects)
        Runtime.Destroy(pUIObject);

    Runtime.RemoveDestroyedComponentsAndObjects();

    // �ý��� ���� ����
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

void RuntimeImpl::InitAllSubsystem(PCWSTR wndTitle, uint32_t width, uint32_t height, WINDOW_MODE mode)
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
    // FileLog.Init(engineLogName);
    COMInitializer.Init(nullptr);
    Time.Init(nullptr);

    WindowImpl::InitDesc initDesc;
    initDesc.m_width = width;
    initDesc.m_height = height;
    initDesc.m_mode = mode;
    initDesc.mTitle = wndTitle;
    Window.Init(&initDesc);

    Input.Init(nullptr);
    GraphicDevice.Init(nullptr);
    Resource.Init(nullptr);
    Renderer.Init(nullptr);
    GameObjectManager.Init(nullptr);
    UIObjectManager.Init(nullptr);
    Environment.Init(nullptr);
    CameraManager.Init(nullptr);
    DirectionalLightManager.Init(nullptr);
    PointLightManager.Init(nullptr);
    SpotLightManager.Init(nullptr);
    MeshRendererManager.Init(nullptr);
    TerrainManager.Init(nullptr);
    MonoBehaviourManager.Init(nullptr);
    SceneManager.Init(nullptr);
}

void RuntimeImpl::ReleaseAllSubsystem()
{
    SceneManager.Release();
    TerrainManager.Release();
    MonoBehaviourManager.Release();
    MeshRendererManager.Release();
    SpotLightManager.Release();
    PointLightManager.Release();
    DirectionalLightManager.Release();
    CameraManager.Release();
    Environment.Release();
    UIObjectManager.Release();
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

    SceneManager.Update(&m_deltaPC);

    // Call MonoBehaviour::Start() for all starting scripts.
    MonoBehaviourManager.CallStart();

    // For the FixedUpdate
    m_deltaPC += Time.GetDeltaPerformanceCounter();
    Time.ChangeDeltatime_toFixedDeltaTime(); // FixeUpdate������ GetDeltaTime() �Լ��� GetFixedDeltaTime() �Լ��� ��ȯ�� �ϰ��� ����
    while (Time.GetFixedDeltaPerformanceCounter() <= m_deltaPC)
    {
        MonoBehaviourManager.FixedUpdateScripts();
        m_deltaPC -= Time.GetFixedDeltaPerformanceCounter();
    }
    Time.RecoverDeltaTime();

    MonoBehaviourManager.UpdateScripts();
    MonoBehaviourManager.LateUpdateScripts();

    RemoveDestroyedComponentsAndObjects();

    // Render
    if (m_flag & RUNTIME_FLAG::RTF_RENDER_ENABLED)
        Renderer.RenderFrame();

    Sleep(3);
}

GameObjectHandle RuntimeImpl::CreateGameObject(PCWSTR name)
{
    // Not deferred game object.
    GameObject* pGameObject = new GameObject(GAMEOBJECT_FLAG::ACTIVE, name);

    GameObjectHandle hGameObject = GameObjectManager.RegisterToHandleTable(pGameObject);

    GameObjectManager.AddPtrToActiveVector(pGameObject);

    return hGameObject;
}

UIObjectHandle RuntimeImpl::CreatePanel(PCWSTR name)
{
    // Not deferred ui object.
    Panel* pPanel = new Panel(UIOBJECT_FLAG::ACTIVE, name);

    return this->RegisterRootUIObject(pPanel);
}

UIObjectHandle RuntimeImpl::CreateImage(PCWSTR name)
{
    // Not deferred ui object.
    Image* pImage = new Image(UIOBJECT_FLAG::ACTIVE, name);

    return this->RegisterRootUIObject(pImage);
}

UIObjectHandle RuntimeImpl::CreateButton(PCWSTR name)
{
    // Not deferred ui object.
    Button* pButton = new Button(UIOBJECT_FLAG::ACTIVE, name);

    return this->RegisterRootUIObject(pButton);
}

void RuntimeImpl::DontDestroyOnLoad(GameObjectHandle gameObject)
{
    GameObject* pGameObject = gameObject.ToPtr();
    if (!pGameObject)
        return;

    pGameObject->OnFlag(GAMEOBJECT_FLAG::DONT_DESTROY_ON_LOAD);
}

void RuntimeImpl::DontDestroyOnLoad(UIObjectHandle uiObject)
{
    IUIObject* pUIObject = uiObject.ToPtr();
    if (!pUIObject)
        return;

    pUIObject->OnFlag(UIOBJECT_FLAG::DONT_DESTROY_ON_LOAD);
}

void RuntimeImpl::Destroy(GameObjectHandle hGameObject)
{
    GameObject* pGameObject = hGameObject.ToPtr();
    if (!pGameObject)
        return;

    // ������ ���� ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
    if (pGameObject->IsDeferred())
        return;

    this->Destroy(pGameObject);
}

void RuntimeImpl::Destroy(UIObjectHandle hUIObject)
{
    IUIObject* pUIObject = hUIObject.ToPtr();
    if (!pUIObject)
        return;

    // ������ ���� ������Ʈ�� �����ϴ� ���� OnLoadScene���� Destroy�� �Ѵٴ� �ǹ��ε� �̰��� ������� �ʴ´�.
    if (pUIObject->IsDeferred())
        return;

    this->Destroy(pUIObject);
}

void RuntimeImpl::Destroy(MonoBehaviour* pScript)
{
    assert(pScript != nullptr);

    this->Destroy(static_cast<IComponent*>(pScript));
}

void RuntimeImpl::Destroy(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    // �ڽ� ���� ������Ʈ���� Destroy
    for (Transform* pChildTransform : pGameObject->m_transform.m_children)
        this->Destroy(pChildTransform->m_pGameObject);

    if (pGameObject->IsOnTheDestroyQueue())
        return;

    this->DestroyAllComponents(pGameObject);
    GameObjectManager.AddToDestroyQueue(pGameObject);
}

void RuntimeImpl::Destroy(IUIObject* pUIObject)
{
    assert(pUIObject->IsDeferred() == false);

    // �ڽ� ���� ������Ʈ���� Destroy
    for (RectTransform* pChildTransform : pUIObject->m_transform.m_children)
        this->Destroy(pChildTransform->m_pUIObject);

    if (pUIObject->IsOnTheDestroyQueue())
        return;

    UIObjectManager.AddToDestroyQueue(pUIObject);
}

void RuntimeImpl::Destroy(IComponent* pComponent)
{
    if (pComponent->IsOnTheDestroyQueue())  // ���� �˻��ϸ� IComponent::GetComponentManager() �����Լ� ȣ�� ��� ���� ����
        return;
    
    // OnLoadScene �Լ����� �ı��� ������� �ʴ´�.
    if (pComponent->m_pGameObject->IsDeferred())
        return;

    IComponentManager* pComponentManager = pComponent->GetComponentManager();
    pComponentManager->AddToDestroyQueue(pComponent);
}

void RuntimeImpl::DestroyAllComponents(GameObject* pGameObject)
{
    assert(pGameObject->IsDeferred() == false);

    for (IComponent* pComponent : pGameObject->m_components)
    {
        if (pComponent->IsOnTheDestroyQueue())  // ���� �˻��ϸ� IComponent::GetComponentManager() �����Լ� ȣ�� ��� ���� ����
            continue;

        IComponentManager* pComponentManager = pComponent->GetComponentManager();
        pComponentManager->AddToDestroyQueue(pComponent);
    }
}

void RuntimeImpl::RemoveDestroyedComponentsAndObjects()
{
    // ������Ʈ ���� �۾�
    MonoBehaviourManager.RemoveDestroyedComponents();
    CameraManager.RemoveDestroyedComponents();
    DirectionalLightManager.RemoveDestroyedComponents();
    PointLightManager.RemoveDestroyedComponents();
    SpotLightManager.RemoveDestroyedComponents();
    MeshRendererManager.RemoveDestroyedComponents();
    TerrainManager.RemoveDestroyedComponents();

    UIObjectManager.RemoveDestroyedUIObjects();
    // �ݵ�� ������Ʈ ���� �۾� ���� ����
    GameObjectManager.RemoveDestroyedGameObjects();
}

UIObjectHandle RuntimeImpl::RegisterRootUIObject(IUIObject* pUIObject)
{
    UIObjectHandle hUIObject = UIObjectManager.RegisterToHandleTable(pUIObject);
    UIObjectManager.AddPtrToActiveVector(pUIObject);

    pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);
    UIObjectManager.AddPtrToRootVector(pUIObject);

    return hUIObject;
}

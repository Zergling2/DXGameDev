#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\ResourceLoader.h>
#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

constexpr DWORD BORDERLESS_WINDOW_STYLE = WS_POPUP;
constexpr DWORD TITLEBAR_WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

Runtime* Runtime::s_pInstance = nullptr;

Runtime::Runtime()
    : m_deltaPerformanceCount(0)
    , m_isEditor(false)
    , m_render(true)
    , m_nCmdShow(0)
    , m_window()
{
}

void Runtime::CreateInstance()
{
    assert(s_pInstance == nullptr);

    s_pInstance = new Runtime();
}

void Runtime::DestroyInstance()
{
    assert(s_pInstance != nullptr);

    delete s_pInstance;
    s_pInstance = nullptr;
}

void Runtime::Init(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height, PCWSTR title, PCWSTR startScene)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks 
    // and generate an error report if the application failed to free all the memory it allocated.
#endif
    m_isEditor = false;
    m_nCmdShow = nCmdShow;

    // ��Ÿ �ʱ�ȭ �۾�...
    if (!XMVerifyCPUSupport())
    {
        Debug::ForceCrashWithMessageBox(L"Unsupported platform", L"DirectXMath library does not supports a given platform.");
        return;
    }

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    MemoryAllocator::CreateInstance();
    FileSystem::CreateInstance();
    Time::CreateInstance();
    Input::CreateInstance();
    GraphicDevice::CreateInstance();
    ResourceLoader::CreateInstance();
    Renderer::CreateInstance();
    Environment::CreateInstance();
    GameObjectManager::CreateInstance();
    UIObjectManager::CreateInstance();
    AudioSourceManager::CreateInstance();
    CameraManager::CreateInstance();
    DirectionalLightManager::CreateInstance();
    PointLightManager::CreateInstance();
    SpotLightManager::CreateInstance();
    MeshRendererManager::CreateInstance();
    TerrainManager::CreateInstance();
    MonoBehaviourManager::CreateInstance();
    SceneManager::CreateInstance();

    m_window.Create(this, hInstance, TITLEBAR_WINDOW_STYLE, width, height, title);

    MemoryAllocator::GetInstance()->Init();
    FileSystem::GetInstance()->Init();
    Time::GetInstance()->Init();
    Input::GetInstance()->Init(hInstance, m_window.GetHandle());
    GraphicDevice::GetInstance()->Init(m_window.GetHandle(), width, height, false);
    ResourceLoader::GetInstance()->Init();
    Renderer::GetInstance()->Init();
    Environment::GetInstance()->Init();
    GameObjectManager::GetInstance()->Init();
    UIObjectManager::GetInstance()->Init();
    AudioSourceManager::GetInstance()->Init();
    CameraManager::GetInstance()->Init();
    DirectionalLightManager::GetInstance()->Init();
    PointLightManager::GetInstance()->Init();
    SpotLightManager::GetInstance()->Init();
    MeshRendererManager::GetInstance()->Init();
    TerrainManager::GetInstance()->Init();
    MonoBehaviourManager::GetInstance()->Init();
    SceneManager::GetInstance()->Init(startScene);
}

void Runtime::InitEditor(HINSTANCE hInstance, HWND hMainFrameWnd, HWND hViewWnd, uint32_t width, uint32_t height)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks 
    // and generate an error report if the application failed to free all the memory it allocated.
#endif

    m_isEditor = true;

    // ��Ÿ �ʱ�ȭ �۾�...
    if (!XMVerifyCPUSupport())
    {
        Debug::ForceCrashWithMessageBox(L"Unsupported platform", L"DirectXMath library does not supports a given platform.");
        return;
    }

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    MemoryAllocator::CreateInstance();
    FileSystem::CreateInstance();
    Time::CreateInstance();
    Input::CreateInstance();
    GraphicDevice::CreateInstance();
    ResourceLoader::CreateInstance();
    Renderer::CreateInstance();
    Environment::CreateInstance();
    GameObjectManager::CreateInstance();
    UIObjectManager::CreateInstance();
    AudioSourceManager::CreateInstance();
    CameraManager::CreateInstance();
    DirectionalLightManager::CreateInstance();
    PointLightManager::CreateInstance();
    SpotLightManager::CreateInstance();
    MeshRendererManager::CreateInstance();
    TerrainManager::CreateInstance();
    MonoBehaviourManager::CreateInstance();
    SceneManager::CreateInstance();

    MemoryAllocator::GetInstance()->Init();
    FileSystem::GetInstance()->Init();
    Time::GetInstance()->Init();
    Input::GetInstance()->Init(hInstance, hMainFrameWnd);
    GraphicDevice::GetInstance()->Init(hViewWnd, width, height, false);
    ResourceLoader::GetInstance()->Init();
    Renderer::GetInstance()->Init();
    Environment::GetInstance()->Init();
    GameObjectManager::GetInstance()->Init();
    UIObjectManager::GetInstance()->Init();
    AudioSourceManager::GetInstance()->Init();
    CameraManager::GetInstance()->Init();
    DirectionalLightManager::GetInstance()->Init();
    PointLightManager::GetInstance()->Init();
    SpotLightManager::GetInstance()->Init();
    MeshRendererManager::GetInstance()->Init();
    TerrainManager::GetInstance()->Init();
    MonoBehaviourManager::GetInstance()->Init();
    SceneManager::GetInstance()->Init(nullptr);
}

void Runtime::UnInit()
{
    // �ڿ� ���� �۾�...
    SceneManager::GetInstance()->UnInit();
    MonoBehaviourManager::GetInstance()->UnInit();
    TerrainManager::GetInstance()->UnInit();
    MeshRendererManager::GetInstance()->UnInit();
    SpotLightManager::GetInstance()->UnInit();
    PointLightManager::GetInstance()->UnInit();
    DirectionalLightManager::GetInstance()->UnInit();
    CameraManager::GetInstance()->UnInit();
    AudioSourceManager::GetInstance()->UnInit();
    UIObjectManager::GetInstance()->UnInit();
    GameObjectManager::GetInstance()->UnInit();
    Environment::GetInstance()->UnInit();
    Renderer::GetInstance()->UnInit();
    ResourceLoader::GetInstance()->UnInit();
    GraphicDevice::GetInstance()->UnInit();
    Input::GetInstance()->UnInit();
    Time::GetInstance()->UnInit();
    FileSystem::GetInstance()->UnInit();
    MemoryAllocator::GetInstance()->UnInit();

    SceneManager::DestroyInstance();
    MonoBehaviourManager::DestroyInstance();
    TerrainManager::DestroyInstance();
    MeshRendererManager::DestroyInstance();
    SpotLightManager::DestroyInstance();
    PointLightManager::DestroyInstance();
    DirectionalLightManager::DestroyInstance();
    CameraManager::DestroyInstance();
    AudioSourceManager::DestroyInstance();
    UIObjectManager::DestroyInstance();
    GameObjectManager::DestroyInstance();
    Environment::DestroyInstance();
    Renderer::DestroyInstance();
    ResourceLoader::DestroyInstance();
    GraphicDevice::DestroyInstance();
    Input::DestroyInstance();
    Time::DestroyInstance();
    FileSystem::DestroyInstance();
    MemoryAllocator::DestroyInstance();
    CoUninitialize();

    // �����
    this->OutputDXGIDebugLog();
}

void Runtime::Run()
{
    ShowWindow(m_window.GetHandle(), m_nCmdShow);
    // UpdateWindow(m_window.GetHandle());

    // update initial time value
    Time::GetInstance()->Update();

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
            OnIdle();
        }
    }

    this->DestroyAllObject();
    this->RemoveDestroyedComponentsAndObjects();
}

void Runtime::Exit()
{
    m_window.Destroy();	// �޽��� ���� ���Ḧ ����
}

void Runtime::OnIdle()
{
    // Update timer.
    Time::GetInstance()->Update();
    Input::GetInstance()->Update();

    IScene* pNextScene = SceneManager::GetInstance()->PopNextScene();
    if (pNextScene)
    {
        this->LoadNextScene(pNextScene);
        delete pNextScene;

        m_deltaPerformanceCount = 0;
    }
    // �ٸ� ������ ������Ʈ���� ��� �ʱ�ȭ �� ��
    // Call MonoBehaviour::Start() for all starting scripts.
    MonoBehaviourManager::GetInstance()->CallStart();

    // For the FixedUpdate
    m_deltaPerformanceCount += Time::GetInstance()->GetDeltaPerformanceCounter();
    Time::GetInstance()->ChangeDeltaTimeToFixedDeltaTime(); // FixeUpdate������ GetDeltaTime() �Լ��� GetFixedDeltaTime() �Լ��� ��ȯ�� �ϰ��� ����
    while (Time::GetInstance()->GetFixedDeltaPerformanceCounter() <= m_deltaPerformanceCount)
    {
        MonoBehaviourManager::GetInstance()->FixedUpdate();
        m_deltaPerformanceCount -= Time::GetInstance()->GetFixedDeltaPerformanceCounter();
    }
    Time::GetInstance()->RecoverDeltaTime();

    MonoBehaviourManager::GetInstance()->Update();
    MonoBehaviourManager::GetInstance()->LateUpdate();

    RemoveDestroyedComponentsAndObjects();

    // Render
    if (m_render)
        Renderer::GetInstance()->RenderFrame();

    // Sleep(3);
}

GameObjectHandle Runtime::CreateGameObject(PCWSTR name)
{
    return GameObjectManager::GetInstance()->CreateObject(name);
}

UIObjectHandle Runtime::CreatePanel(PCWSTR name)
{
    // Not deferred ui object.
    return UIObjectManager::GetInstance()->CreateObject<Panel>(name);
}

UIObjectHandle Runtime::CreateImage(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<Image>(name);
}

UIObjectHandle Runtime::CreateButton(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<Button>(name);
}

void Runtime::OnSize(UINT nType, int cx, int cy)
{
    bool resize = false;

    switch (nType)
    {
    case SIZE_RESTORED:
        resize = true;
        m_render = true;
        break;
    case SIZE_MINIMIZED:
        m_render = false;
        break;
    case SIZE_MAXIMIZED:
        m_render = true;
        break;
    case SIZE_MAXSHOW:  // �ٸ� â�� �ִ�ȭ ���¿��� ���� ũ��� ���ư� ���
        m_render = true;
        break;
    case SIZE_MAXHIDE:  // �ٸ� â�� �ִ�ȭ�� ���
        m_render = false;
        break;
    default:
        m_render = true;
        break;
    }

    if (resize)
    {
        const uint32_t newWidth = static_cast<uint32_t>(cx);
        const uint32_t newHeight = static_cast<uint32_t>(cy);

        // SwapChain Resize �� Depth/Stencil Buffer Resize
        GraphicDevice::GetInstance()->ResizeBuffer(newWidth, newHeight);

        // Camera Color Buffer & Depth/Stencil Buffer Resize & Projection Matrix Update
        CameraManager::GetInstance()->ResizeBuffer(newWidth, newHeight);
    }
}

void Runtime::OnChar(WPARAM wParam, LPARAM lParam)
{
    UIObjectManager::GetInstance()->OnChar(wParam, lParam);
}

void Runtime::OnMouseMove(UINT flags, POINT pt)
{
    Input::GetInstance()->SetMousePos(pt);
}

void Runtime::OnLButtonDown(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnLButtonDown(pt);
}

void Runtime::OnLButtonUp(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnLButtonUp(pt);
}

void Runtime::OnRButtonDown(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnRButtonDown(pt);
}

void Runtime::OnRButtonUp(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnRButtonUp(pt);
}

void Runtime::OnMButtonDown(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnMButtonDown(pt);
}

void Runtime::OnMButtonUp(UINT flags, POINT pt)
{
    UIObjectManager::GetInstance()->OnMButtonUp(pt);
}

void Runtime::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
    m_render = false;
}

void Runtime::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
    m_render = true;
}

void Runtime::DestroyAllObjectExceptDontDestroyOnLoad()
{
    // DontDestroyOnLoad ������Ʈ�� �����ϰ� ��� �ı�
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_activeGroup)
        if (!pGameObject->IsDontDestroyOnLoad())
            pGameObject->Destroy();
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_inactiveGroup)
        if (!pGameObject->IsDontDestroyOnLoad())
            pGameObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_activeGroup)
        if (!pUIObject->IsDontDestroyOnLoad())
            pUIObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_inactiveGroup)
        if (!pUIObject->IsDontDestroyOnLoad())
            pUIObject->Destroy();
}

void Runtime::DestroyAllObject()
{
    // DontDestroyOnLoad ������Ʈ�� �����ϰ� ��� �ı�
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_activeGroup)
        pGameObject->Destroy();
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_inactiveGroup)
        pGameObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_activeGroup)
        pUIObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_inactiveGroup)
        pUIObject->Destroy();
}

void Runtime::RemoveDestroyedComponentsAndObjects()
{
    // ������Ʈ ���� �۾�
    MonoBehaviourManager::GetInstance()->RemoveDestroyedComponents();
    CameraManager::GetInstance()->RemoveDestroyedComponents();
    DirectionalLightManager::GetInstance()->RemoveDestroyedComponents();
    PointLightManager::GetInstance()->RemoveDestroyedComponents();
    SpotLightManager::GetInstance()->RemoveDestroyedComponents();
    MeshRendererManager::GetInstance()->RemoveDestroyedComponents();
    TerrainManager::GetInstance()->RemoveDestroyedComponents();

    UIObjectManager::GetInstance()->RemoveDestroyedUIObjects();
    // �ݵ�� ������Ʈ ���� �۾� ���� ����
    GameObjectManager::GetInstance()->RemoveDestroyedGameObjects();
}

void Runtime::OutputDXGIDebugLog() const
{
    // �����
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

bool Runtime::SetResolution(uint32_t width, uint32_t height, DISPLAY_MODE mode)
{
    if (m_isEditor)
        return false;

    HMONITOR hMonitor;
    MONITORINFO mi;
    bool result;

    switch (mode)
    {
    case DISPLAY_MODE::WINDOWED:
        m_window.SetStyle(TITLEBAR_WINDOW_STYLE);
        result = m_window.Resize(width, height);
        break;
    case DISPLAY_MODE::BORDERLESS_WINDOWED:
        m_window.SetStyle(BORDERLESS_WINDOW_STYLE);
        ZeroMemory(&mi, sizeof(mi));
        mi.cbSize = sizeof(mi);
        hMonitor = MonitorFromWindow(m_window.GetHandle(), MONITOR_DEFAULTTONEAREST);
        GetMonitorInfo(hMonitor, &mi);
        width = static_cast<uint32_t>(mi.rcMonitor.right - mi.rcMonitor.left);
        height = static_cast<uint32_t>(mi.rcMonitor.bottom - mi.rcMonitor.top);
        result = m_window.Resize(width, height);
        break;
    case DISPLAY_MODE::FULLSCREEN:
        result = false;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

void Runtime::LoadNextScene(IScene* pNextScene)
{
    this->DestroyAllObjectExceptDontDestroyOnLoad();
    this->RemoveDestroyedComponentsAndObjects();

    // ������ ���ӿ�����Ʈ�� �� ������Ʈ���� ���� ����
    auto& pendingUIObjects = pNextScene->m_pendingUIObjects;
    for (IUIObject* pUIObject : pendingUIObjects)
        UIObjectManager::GetInstance()->Deploy(pUIObject);

    auto& pendingGameObjects = pNextScene->m_pendingGameObjects;
    for (GameObject* pGameObject : pendingGameObjects)
    {
        GameObjectManager::GetInstance()->Deploy(pGameObject);

        // ������Ʈ�鵵 Deploy
        for (IComponent* pComponent : pGameObject->m_components)
        {
            IComponentManager* pComponentManager = pComponent->GetComponentManager();
            pComponentManager->Deploy(pComponent);
        }
    }

    // ���ӿ�����Ʈ, UI������Ʈ�� ������Ʈ���� ��� ���� �����ڿ� ��ϵ� ���Ŀ� Awake, OnEnable, Startť�� ó��
    MonoBehaviourManager::GetInstance()->AwakeDeployedComponents();
}

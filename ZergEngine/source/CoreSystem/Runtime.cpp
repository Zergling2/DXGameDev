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
    : m_hInstance(NULL)
    , m_nCmdShow(0)
    , m_window()
    , m_deltaPerformanceCount(0)
    , m_render(true)
{
}

Runtime::~Runtime()
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

    m_hInstance = hInstance;
    m_nCmdShow = nCmdShow;

    // 기타 초기화 작업...
    if (!XMVerifyCPUSupport())
    {
        MessageBox(NULL, _T("DirectXMath Library does not supports a given platform."), _T("Unsupported platform"), MB_OK);
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
    Input::GetInstance()->Init(this->GetInstanceHandle(), m_window.GetHandle());
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

void Runtime::UnInit()
{
    // 자원 해제 작업...
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

    m_hInstance = NULL;

    // 디버그
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

void Runtime::Run()
{
    ShowWindow(m_window.GetHandle(), m_nCmdShow);
    // UpdateWindow(Runtime.GetMainWindowHandle());

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
}

void Runtime::Exit()
{
    m_window.Destroy();	// 메시지 루프 종료를 유도
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
    // 다른 종류의 컴포넌트들이 모두 초기화 된 후
    // Call MonoBehaviour::Start() for all starting scripts.
    MonoBehaviourManager::GetInstance()->CallStart();

    // For the FixedUpdate
    m_deltaPerformanceCount += Time::GetInstance()->GetDeltaPerformanceCounter();
    Time::GetInstance()->ChangeDeltaTimeToFixedDeltaTime(); // FixeUpdate에서의 GetDeltaTime() 함수와 GetFixedDeltaTime() 함수의 반환값 일관성 보장
    while (Time::GetInstance()->GetFixedDeltaPerformanceCounter() <= m_deltaPerformanceCount)
    {
        MonoBehaviourManager::GetInstance()->FixedUpdateScripts();
        m_deltaPerformanceCount -= Time::GetInstance()->GetFixedDeltaPerformanceCounter();
    }
    Time::GetInstance()->RecoverDeltaTime();

    MonoBehaviourManager::GetInstance()->UpdateScripts();
    MonoBehaviourManager::GetInstance()->LateUpdateScripts();

    RemoveDestroyedComponentsAndObjects();

    // Render
    if (m_render)
        Renderer::GetInstance()->RenderFrame();

    // Sleep(3);
}

bool Runtime::SetResolution(uint32_t width, uint32_t height, DISPLAY_MODE mode)
{
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

void Runtime::LoadNextScene(IScene* pNextScene)
{
    this->DestroyAllObjectExceptDontDestroyOnLoad();

    // 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
    auto& pendingUIObjects = pNextScene->m_pendingUIObjects;
    for (IUIObject* pUIObject : pendingUIObjects)
        UIObjectManager::GetInstance()->Deploy(pUIObject);

    auto& pendingGameObjects = pNextScene->m_pendingGameObjects;
    for (GameObject* pGameObject : pendingGameObjects)
    {
        GameObjectManager::GetInstance()->Deploy(pGameObject);

        // 컴포넌트들도 Deploy
        for (IComponent* pComponent : pGameObject->m_components)
        {
            IComponentManager* pComponentManager = pComponent->GetComponentManager();
            pComponentManager->Deploy(pComponent);
        }
    }

    // 게임오브젝트, UI오브젝트와 컴포넌트들이 모두 전역 관리자에 등록된 이후에 Awake, OnEnable, Start큐잉 처리
    MonoBehaviourManager::GetInstance()->AwakeDeployedComponents();
}

void Runtime::DestroyAllObjectExceptDontDestroyOnLoad()
{
    // DontDestroyOnLoad 오브젝트를 제외하고 모두 파괴
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

    this->RemoveDestroyedComponentsAndObjects();
}

void Runtime::DestroyAllObject()
{
    // DontDestroyOnLoad 오브젝트를 제외하고 모두 파괴
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_activeGroup)
        pGameObject->Destroy();
    for (GameObject* pGameObject : GameObjectManager::GetInstance()->m_inactiveGroup)
        pGameObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_activeGroup)
        pUIObject->Destroy();
    for (IUIObject* pUIObject : UIObjectManager::GetInstance()->m_inactiveGroup)
        pUIObject->Destroy();

    this->RemoveDestroyedComponentsAndObjects();
}

void Runtime::RemoveDestroyedComponentsAndObjects()
{
    // 컴포넌트 제거 작업
    MonoBehaviourManager::GetInstance()->RemoveDestroyedComponents();
    CameraManager::GetInstance()->RemoveDestroyedComponents();
    DirectionalLightManager::GetInstance()->RemoveDestroyedComponents();
    PointLightManager::GetInstance()->RemoveDestroyedComponents();
    SpotLightManager::GetInstance()->RemoveDestroyedComponents();
    MeshRendererManager::GetInstance()->RemoveDestroyedComponents();
    TerrainManager::GetInstance()->RemoveDestroyedComponents();

    UIObjectManager::GetInstance()->RemoveDestroyedUIObjects();
    // 반드시 컴포넌트 제거 작업 이후 실행
    GameObjectManager::GetInstance()->RemoveDestroyedGameObjects();
}

void Runtime::OnSize(WPARAM wParam, LPARAM lParam)
{
    bool resize = false;

    switch (wParam)
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
    case SIZE_MAXSHOW:  // 다른 창이 최대화 상태에서 이전 크기로 돌아간 경우
        m_render = true;
        break;
    case SIZE_MAXHIDE:  // 다른 창이 최대화된 경우
        m_render = false;
        break;
    default:
        m_render = true;
        break;
    }

    if (resize)
    {
        const uint32_t newWidth = static_cast<uint32_t>(GET_X_LPARAM(lParam));
        const uint32_t newHeight = static_cast<uint32_t>(GET_Y_LPARAM(lParam));

        // SwapChain Resize 및 Depth/Stencil Buffer Resize
        GraphicDevice::GetInstance()->ResizeBuffer(newWidth, newHeight);

        // Camera Color Buffer & Depth/Stencil Buffer Resize & Projection Matrix Update
        CameraManager::GetInstance()->ResizeBuffer(newWidth, newHeight);
    }
}

void Runtime::OnShowWindow(WPARAM wParam, LPARAM lParam)
{
    if (wParam == FALSE)
        m_render = false;
    else
        m_render = true;
}

void Runtime::OnChar(WPARAM wParam, LPARAM lParam)
{
    UIObjectManager::GetInstance()->OnChar(wParam, lParam);
}

void Runtime::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    assert(m_window.GetHandle() != NULL);
    ScreenToClient(m_window.GetHandle(), &pt);

    // Y가 위쪽 방향으로 증가하는 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 구현해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;

    Input::GetInstance()->SetMousePos(pt);
}

void Runtime::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

    UIObjectManager::GetInstance()->OnLButtonDown(pt);
}

void Runtime::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

    UIObjectManager::GetInstance()->OnLButtonUp(pt);
}

void Runtime::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

    UIObjectManager::GetInstance()->OnRButtonDown(pt);
}

void Runtime::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

    UIObjectManager::GetInstance()->OnRButtonUp(pt);
}

void Runtime::OnMButtonDown(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

    UIObjectManager::GetInstance()->OnMButtonDown(pt);
}

void Runtime::OnMButtonUp(WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    // 유니티 스크린 좌표 시스템으로 변환
    // 전체화면 모드에서는 다르게 처리해야 함!
    pt.y = static_cast<LONG>(m_window.GetClientHeight()) - pt.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전

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
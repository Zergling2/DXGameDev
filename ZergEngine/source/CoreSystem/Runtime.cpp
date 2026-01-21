#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Cursor.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\ResourceLoader.h>
#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\RenderSettings.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\RigidbodyManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CollisionTriggerManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\AudioSourceManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SliderControl.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Checkbox.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\RadioButton.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <bullet3\LinearMath\btQuickprof.h>

using namespace ze;

constexpr DWORD BORDERLESS_WINDOW_STYLE = WS_POPUP;
constexpr DWORD TITLEBAR_WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

Runtime* Runtime::s_pInstance = nullptr;

Runtime::Runtime()
    : m_accumDeltaTime(0.0f)
    , m_isEditor(false)
    , m_render(true)
    , m_nCmdShow(0)
    , m_window()
    , m_hGameWnd(NULL)
    , m_sfl()
    , m_afl()
{
}

void Runtime::CreateInstance()
{
    assert(s_pInstance == nullptr);

    s_pInstance = reinterpret_cast<Runtime*>(_aligned_malloc_dbg(sizeof(Runtime), 64, __FILE__, __LINE__));
    new(s_pInstance) Runtime();
}

void Runtime::DestroyInstance()
{
    assert(s_pInstance != nullptr);

    s_pInstance->~Runtime();

    _aligned_free_dbg(s_pInstance);
    s_pInstance = nullptr;
}

void Runtime::Init(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height, PCWSTR title, PCWSTR startScene)
{
    m_isEditor = false;
    m_nCmdShow = nCmdShow;

    // 기타 초기화 작업...
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
    Physics::CreateInstance();
    ResourceLoader::CreateInstance();
    Renderer::CreateInstance();
    RenderSettings::CreateInstance();
    GameObjectManager::CreateInstance();
    UIObjectManager::CreateInstance();
    RigidbodyManager::CreateInstance();
    CollisionTriggerManager::CreateInstance();
    AudioSourceManager::CreateInstance();
    CameraManager::CreateInstance();
    DirectionalLightManager::CreateInstance();
    PointLightManager::CreateInstance();
    SpotLightManager::CreateInstance();
    MeshRendererManager::CreateInstance();
    SkinnedMeshRendererManager::CreateInstance();
    BillboardManager::CreateInstance();
    TerrainManager::CreateInstance();
    MonoBehaviourManager::CreateInstance();
    SceneManager::CreateInstance();

    m_window.Create(this, hInstance, TITLEBAR_WINDOW_STYLE, width, height, title);
    m_hGameWnd = m_window.GetHandle();

    MemoryAllocator::GetInstance()->Init();
    this->CreateLoggers();

    FileSystem::GetInstance()->Init();
    Time::GetInstance()->Init();
    Input::GetInstance()->Init(hInstance, m_hGameWnd);
    GraphicDevice::GetInstance()->Init(m_hGameWnd, width, height, false);
    Physics::GetInstance()->Init();
    ResourceLoader::GetInstance()->Init();
    Renderer::GetInstance()->Init();
    RenderSettings::GetInstance()->Init();
    GameObjectManager::GetInstance()->Init();
    UIObjectManager::GetInstance()->Init();
    RigidbodyManager::GetInstance()->Init();
    CollisionTriggerManager::GetInstance()->Init();
    AudioSourceManager::GetInstance()->Init();
    CameraManager::GetInstance()->Init();
    DirectionalLightManager::GetInstance()->Init();
    PointLightManager::GetInstance()->Init();
    SpotLightManager::GetInstance()->Init();
    MeshRendererManager::GetInstance()->Init();
    SkinnedMeshRendererManager::GetInstance()->Init();
    BillboardManager::GetInstance()->Init();
    TerrainManager::GetInstance()->Init();
    MonoBehaviourManager::GetInstance()->Init();
    SceneManager::GetInstance()->Init(startScene);
}

void Runtime::InitEditor(HINSTANCE hInstance, HWND hMainFrameWnd, HWND hViewWnd, uint32_t width, uint32_t height)
{
    m_isEditor = true;

    // 기타 초기화 작업...
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
    Physics::CreateInstance();
    ResourceLoader::CreateInstance();
    Renderer::CreateInstance();
    RenderSettings::CreateInstance();
    GameObjectManager::CreateInstance();
    UIObjectManager::CreateInstance();
    RigidbodyManager::CreateInstance();
    CollisionTriggerManager::CreateInstance();
    AudioSourceManager::CreateInstance();
    CameraManager::CreateInstance();
    DirectionalLightManager::CreateInstance();
    PointLightManager::CreateInstance();
    SpotLightManager::CreateInstance();
    MeshRendererManager::CreateInstance();
    SkinnedMeshRendererManager::CreateInstance();
    BillboardManager::CreateInstance();
    TerrainManager::CreateInstance();
    MonoBehaviourManager::CreateInstance();
    SceneManager::CreateInstance();

    m_hGameWnd = hViewWnd;
    MemoryAllocator::GetInstance()->Init();
    this->CreateLoggers();

    FileSystem::GetInstance()->Init();
    Time::GetInstance()->Init();
    Input::GetInstance()->Init(hInstance, hMainFrameWnd);
    GraphicDevice::GetInstance()->Init(m_hGameWnd, width, height, false);
    Physics::GetInstance()->Init();
    ResourceLoader::GetInstance()->Init();
    Renderer::GetInstance()->Init();
    RenderSettings::GetInstance()->Init();
    GameObjectManager::GetInstance()->Init();
    UIObjectManager::GetInstance()->Init();
    RigidbodyManager::GetInstance()->Init();
    CollisionTriggerManager::GetInstance()->Init();
    AudioSourceManager::GetInstance()->Init();
    CameraManager::GetInstance()->Init();
    DirectionalLightManager::GetInstance()->Init();
    PointLightManager::GetInstance()->Init();
    SpotLightManager::GetInstance()->Init();
    MeshRendererManager::GetInstance()->Init();
    SkinnedMeshRendererManager::GetInstance()->Init();
    BillboardManager::GetInstance()->Init();
    TerrainManager::GetInstance()->Init();
    MonoBehaviourManager::GetInstance()->Init();
    SceneManager::GetInstance()->Init(nullptr);
}

void Runtime::Release()
{
    // 자원 해제 작업...
    SceneManager::GetInstance()->Shutdown();
    MonoBehaviourManager::GetInstance()->Shutdown();
    TerrainManager::GetInstance()->Shutdown();
    BillboardManager::GetInstance()->Shutdown();
    SkinnedMeshRendererManager::GetInstance()->Shutdown();
    MeshRendererManager::GetInstance()->Shutdown();
    SpotLightManager::GetInstance()->Shutdown();
    PointLightManager::GetInstance()->Shutdown();
    DirectionalLightManager::GetInstance()->Shutdown();
    CameraManager::GetInstance()->Shutdown();
    AudioSourceManager::GetInstance()->Shutdown();
    CollisionTriggerManager::GetInstance()->Shutdown();
    RigidbodyManager::GetInstance()->Shutdown();
    UIObjectManager::GetInstance()->Shutdown();
    GameObjectManager::GetInstance()->Shutdown();
    RenderSettings::GetInstance()->Shutdown();
    Renderer::GetInstance()->Shutdown();
    ResourceLoader::GetInstance()->Shutdown();
    Physics::GetInstance()->Shutdown();
    GraphicDevice::GetInstance()->Shutdown();
    Input::GetInstance()->Shutdown();
    Time::GetInstance()->Shutdown();
    FileSystem::GetInstance()->Shutdown();

    this->ReleaseLoggers();
    MemoryAllocator::GetInstance()->Shutdown();

    SceneManager::DestroyInstance();
    MonoBehaviourManager::DestroyInstance();
    TerrainManager::DestroyInstance();
    BillboardManager::DestroyInstance();
    SkinnedMeshRendererManager::DestroyInstance();
    MeshRendererManager::DestroyInstance();
    SpotLightManager::DestroyInstance();
    PointLightManager::DestroyInstance();
    DirectionalLightManager::DestroyInstance();
    CameraManager::DestroyInstance();
    AudioSourceManager::DestroyInstance();
    CollisionTriggerManager::DestroyInstance();
    RigidbodyManager::DestroyInstance();
    UIObjectManager::DestroyInstance();
    GameObjectManager::DestroyInstance();
    RenderSettings::DestroyInstance();
    Renderer::DestroyInstance();
    ResourceLoader::DestroyInstance();
    Physics::DestroyInstance();
    GraphicDevice::DestroyInstance();
    Input::DestroyInstance();
    Time::DestroyInstance();
    FileSystem::DestroyInstance();
    MemoryAllocator::DestroyInstance();
    CoUninitialize();

    // DXGI Debug
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

        m_accumDeltaTime = 0.0f;
    }
    // 다른 종류의 컴포넌트들이 모두 초기화 된 후
    // Call MonoBehaviour::Start() for all starting scripts.
    MonoBehaviourManager::GetInstance()->CallStart();

    // FixedUpdate
    m_accumDeltaTime += Time::GetInstance()->GetDeltaTime();
    Time::GetInstance()->ChangeToFixedDeltaTimeMode();  // FixedUpdate 스크립트의 DeltaTime, FixedDeltaTime 일치화
    while (Time::GetInstance()->GetFixedDeltaTime() <= m_accumDeltaTime)
    {
        // FixedUpdate 우선
        MonoBehaviourManager::GetInstance()->FixedUpdate();

        CollisionTriggerManager::GetInstance()->UpdateCollisionTriggerTransform();   // 트리거 오브젝트의 Transform을 동기화 (ZergEngine -> Bullet)

        // 물리 시뮬레이션 스텝
        Physics::GetInstance()->StepSimulation(FIXED_DELTA_TIME);

        m_accumDeltaTime -= Time::GetInstance()->GetFixedDeltaTime();
    }
    Time::GetInstance()->RecoverToDeltaTimeMode();  // DeltaTime이 실제 Delta time을 가리키도록 복구
    
    MonoBehaviourManager::GetInstance()->Update();
    MonoBehaviourManager::GetInstance()->LateUpdate();

    CollisionTriggerManager::GetInstance()->UpdateCollisionTriggerTransform();      // 트리거 오브젝트의 Transform을 동기화 (ZergEngine -> Bullet)

    // Update animation time cursor
    for (IComponent* pComponent : SkinnedMeshRendererManager::GetInstance()->m_directAccessGroup)
    {
        SkinnedMeshRenderer* pSkinnedMeshRenderer = static_cast<SkinnedMeshRenderer*>(pComponent);
        const Animation* pCurrAnim = pSkinnedMeshRenderer->GetCurrentAnimationPtr();
        const bool pauseAnim = pSkinnedMeshRenderer->IsAnimationPaused();

        if (pCurrAnim == nullptr || pauseAnim == true)
            continue;

        float newAnimTimeCursor =
            pSkinnedMeshRenderer->GetAnimationTimeCursor() + Time::GetInstance()->GetDeltaTime() * pSkinnedMeshRenderer->GetAnimationSpeed();

        if (pSkinnedMeshRenderer->IsLoopAnimation())
            newAnimTimeCursor = Math::WrapFloat(newAnimTimeCursor, pCurrAnim->GetDuration());
        else
            newAnimTimeCursor = Math::Clamp(newAnimTimeCursor, 0.0f, pCurrAnim->GetDuration());

        pSkinnedMeshRenderer->SetAnimationTimeCursor(newAnimTimeCursor);
    }

    RemoveDestroyedComponentsAndObjects();

    // Render
    if (m_render)
    {
        if (Physics::GetInstance()->m_drawDebugInfo)
            Physics::GetInstance()->DebugDrawWorld();

        // 디버그 시각 정보 버퍼 업데이트
        Physics::GetInstance()->UpdateDebugDrawerResources(GraphicDevice::GetInstance()->GetImmediateContext());

        Renderer::GetInstance()->RenderFrame();

        // 디버그 시각 정보 프리미티브 제거
        Physics::GetInstance()->ClearDebugDrawerCache();
    }

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

UIObjectHandle Runtime::CreateText(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<Text>(name);
}

UIObjectHandle Runtime::CreateButton(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<Button>(name);
}

UIObjectHandle Runtime::CreateInputField(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<InputField>(name);
}

UIObjectHandle Runtime::CreateSliderControl(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<SliderControl>(name);
}

UIObjectHandle Runtime::CreateCheckbox(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<Checkbox>(name);
}

UIObjectHandle Runtime::CreateRadioButton(PCWSTR name)
{
    return UIObjectManager::GetInstance()->CreateObject<RadioButton>(name);
}

void Runtime::OnDestroy(WPARAM wParam, LPARAM lParam)
{
    this->DestroyAllObject();
    this->RemoveDestroyedComponentsAndObjects();
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
        RECT rect;
        GetClientRect(m_hGameWnd, &rect);

        const uint32_t newWidth = rect.right - rect.left;
        const uint32_t newHeight = rect.bottom - rect.top;

        // SwapChain Resize 및 Depth/Stencil Buffer Resize
        constexpr int GDR_CREATION_RETRY_COUNT = 3;

        // 그래픽디바이스 시스템 리소스들 재생성
        int retryInterval = 500;
        bool success = false;
        for (int retry = 0; retry < GDR_CREATION_RETRY_COUNT; ++retry)
        {
            bool result = GraphicDevice::GetInstance()->ResizeBuffer(newWidth, newHeight);
            if (result)
            {
                success = true;
                break;
            }

            Sleep(retryInterval);
            retryInterval <<= 1;
        }

        if (!success)
            Debug::ForceCrashWithMessageBox(L"Error", L"GraphicDevice::ResizeBuffer failed.");


        // 카메라의 그래픽 리소스들 재생성 유도
        CameraManager::GetInstance()->ReleaseAllCameraBuffer();

        // 만약 Lock 모드 또는 Confined 모드였다면 커서 클립 영역을 재계산해야 하므로 설정.
        Cursor::SetLockState(Cursor::GetLockState());
    }
}

void Runtime::OnMove(WPARAM wParam, LPARAM lParam)
{
    Cursor::SetLockState(Cursor::GetLockState());
}

void Runtime::OnActivateApp(WPARAM wParam, LPARAM lParam)
{
    CURSORINFO ci;

    switch (wParam)
    {
    case FALSE: // DEACTIVATED
        Cursor::SetChangeOnlyFlag(true);

        ClipCursor(nullptr);

        ci.cbSize = sizeof(ci);
        GetCursorInfo(&ci);

        if (ci.flags == 0)  // 커서가 숨겨진 상태라면
            while (ShowCursor(TRUE) < 0);   // 커서를 보이게 한다.
        break;
    case TRUE:  // ACTIVATED
        Cursor::SetChangeOnlyFlag(false);

        // 상태 복구
        Cursor::SetVisible(Cursor::IsVisible());
        Cursor::SetLockState(Cursor::GetLockState());
        break;
    default:
        break;
    }
}

void Runtime::OnChar(WPARAM wParam, LPARAM lParam)
{
    UIObjectManager::GetInstance()->OnChar(wParam, lParam);
}

void Runtime::OnMouseMove(UINT flags, POINT pt)
{
    Input::GetInstance()->SetMousePos(pt);
    UIObjectManager::GetInstance()->OnMouseMove(flags, pt);
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
}

void Runtime::RemoveDestroyedComponentsAndObjects()
{
    // 컴포넌트 제거 작업
    MonoBehaviourManager::GetInstance()->RemoveDestroyedComponents();   // 가장 먼저 수행하여 오브젝트 및 컴포넌트에 대해 최대한의 접근도 보장
    RigidbodyManager::GetInstance()->RemoveDestroyedComponents();
    CollisionTriggerManager::GetInstance()->RemoveDestroyedComponents();
    TerrainManager::GetInstance()->RemoveDestroyedComponents();
    MeshRendererManager::GetInstance()->RemoveDestroyedComponents();
    SkinnedMeshRendererManager::GetInstance()->RemoveDestroyedComponents();
    BillboardManager::GetInstance()->RemoveDestroyedComponents();
    DirectionalLightManager::GetInstance()->RemoveDestroyedComponents();
    PointLightManager::GetInstance()->RemoveDestroyedComponents();
    SpotLightManager::GetInstance()->RemoveDestroyedComponents();
    CameraManager::GetInstance()->RemoveDestroyedComponents();
    AudioSourceManager::GetInstance()->RemoveDestroyedComponents();

    UIObjectManager::GetInstance()->RemoveDestroyedUIObjects();
    // 반드시 컴포넌트 제거 작업 이후 실행
    GameObjectManager::GetInstance()->RemoveDestroyedGameObjects();
}

void Runtime::OutputDXGIDebugLog() const
{
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

void Runtime::CreateLoggers()
{
    LPCWSTR logDirName = L"logs";
    BOOL ret = CreateDirectoryW(logDirName, nullptr);

    if (ret == FALSE && GetLastError() != ERROR_ALREADY_EXISTS)
        Debug::ForceCrashWithMessageBox(L"Error", L"Failed to create log directory.");

    __time64_t now = _time64(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    WCHAR logFileName[64];

#if defined(DEBUG) || defined(_DEBUG)
    StringCbPrintfW(logFileName, sizeof(logFileName), L"logs\\syslog_dbg_%02d%02d%02d_%02d%02d%02d.log",
        (localTime.tm_year + 1900) % 100, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec
    );
#else
    StringCbPrintfW(logFileName, sizeof(logFileName), L"logs\\syslog_%02d%02d%02d_%02d%02d%02d.log",
        (localTime.tm_year + 1900) % 100, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec
    );
#endif
    m_sfl.Init(logFileName);

#if defined(DEBUG) || defined(_DEBUG)
    StringCbPrintfW(logFileName, sizeof(logFileName), L"logs\\asynclog_dbg_%02d%02d%02d_%02d%02d%02d.log",
        (localTime.tm_year + 1900) % 100, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec
    );
#else
    StringCbPrintfW(logFileName, sizeof(logFileName), L"logs\\asynclog_%02d%02d%02d_%02d%02d%02d.log",
        (localTime.tm_year + 1900) % 100, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec
    );
#endif
    m_afl.Init(logFileName);
}

void Runtime::ReleaseLoggers()
{
    m_sfl.Release();
    m_afl.Release();
}

bool Runtime::__$$SetResolutionImpl(uint32_t width, uint32_t height, DisplayMode mode)
{
    if (m_isEditor)
        return false;

    HMONITOR hMonitor;
    MONITORINFO mi;
    bool result;

    switch (mode)
    {
    case DisplayMode::Windowed:
        m_window.SetStyle(TITLEBAR_WINDOW_STYLE);
        result = m_window.Resize(width, height);
        break;
    case DisplayMode::BorderlessWindowed:
        m_window.SetStyle(BORDERLESS_WINDOW_STYLE);
        ZeroMemory(&mi, sizeof(mi));
        mi.cbSize = sizeof(mi);
        hMonitor = MonitorFromWindow(m_window.GetHandle(), MONITOR_DEFAULTTONEAREST);
        GetMonitorInfo(hMonitor, &mi);
        width = static_cast<uint32_t>(mi.rcMonitor.right - mi.rcMonitor.left);
        height = static_cast<uint32_t>(mi.rcMonitor.bottom - mi.rcMonitor.top);
        result = m_window.Resize(width, height);
        break;
    case DisplayMode::Fullscreen:
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

    // 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
    auto& pendingUIObjects = pNextScene->m_pendingUIObjects;
    for (IUIObject* pUIObject : pendingUIObjects)
        pUIObject->OnDeploySysJob();

    auto& pendingGameObjects = pNextScene->m_pendingGameObjects;
    for (GameObject* pGameObject : pendingGameObjects)
    {
        pGameObject->OnDeploySysJob();

        // 컴포넌트들도 씬에 배치
        for (IComponent* pComponent : pGameObject->m_components)
            pComponent->OnDeploySysJob();
    }

    // 게임오브젝트, UI오브젝트와 컴포넌트들이 모두 전역 관리자에 등록된 이후에 Awake, OnEnable, Start큐잉 처리
    // 이렇게 해야 스크립트에서 씬에 처음 배치된 오브젝트들에 대한 접근이 원활히 가능하다.
    MonoBehaviourManager::GetInstance()->AwakeDeployedComponents();
}

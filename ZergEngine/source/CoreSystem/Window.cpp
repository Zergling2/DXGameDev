#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\ComponentSystem\CameraManager.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Window);

PCWSTR WNDCLASS_NAME = L"zewndclass";

Window::Window()
    : m_hWnd(NULL)
    , m_resolution{ 0, 0 }
    , m_fullscreen(false)
    , m_fullClientViewport { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
{
}

Window::~Window()
{
}

void Window::Init(void* pDesc)
{
    Window::InitDesc* pInitDesc = reinterpret_cast<Window::InitDesc*>(pDesc);

    m_hWnd = NULL;
    m_resolution = pInitDesc->m_resolution;
    m_fullscreen = pInitDesc->m_fullscreen;

    m_fullClientViewport.TopLeftX = 0.0f;
    m_fullClientViewport.TopLeftY = 0.0f;
    m_fullClientViewport.Width = static_cast<float>(m_resolution.cx);
    m_fullClientViewport.Height = static_cast<float>(m_resolution.cy);
    m_fullClientViewport.MinDepth = 0.0f;
    m_fullClientViewport.MaxDepth = 1.0f;

    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WinProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = Runtime::GetInstanceHandle();
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = WNDCLASS_NAME;
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    int wndFrameWidth;
    int wndFrameHeight;

    if (m_fullscreen)
    {
        wndFrameWidth = GetSystemMetrics(SM_CXSCREEN);
        wndFrameHeight = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        RECT cr = RECT{ 0, 0, m_resolution.cx, m_resolution.cy };
        if (AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW, FALSE) == FALSE)
            Debug::ForceCrashWithWin32ErrorMessageBox(L"Window::Init() > AdjustWindowRect()", GetLastError());

        wndFrameWidth = cr.right - cr.left;
        wndFrameHeight = cr.bottom - cr.top;
    }

    // Create the window.
    m_hWnd = CreateWindowW(
        WNDCLASS_NAME,                                                  // Window class
        pInitDesc->m_title,                                             // Window title
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,       // Window style
        CW_USEDEFAULT,                                                  // Pos X
        CW_USEDEFAULT,                                                  // Pos Y
        wndFrameWidth,                                                  // Width
        wndFrameHeight,                                                 // Height
        NULL,                                                           // Parent window    
        NULL,                                                           // Menu
        Runtime::GetInstanceHandle(),                                   // Instance handle
        nullptr                                                         // Additional application data
    );
}

void Window::Release()
{
}

void Window::SetResolution(SIZE resolution, bool fullscreen)
{
    if (fullscreen == false)
        MoveWindow(Window::GetWindowHandle(), 0, 0, static_cast<int>(resolution.cx), static_cast<int>(resolution.cy), FALSE);

    Window::m_fullscreen = fullscreen;
    SendMessage(m_hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(resolution.cx, resolution.cy));
}

LRESULT Window::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        switch (wParam)
        {
        case SIZE_MINIMIZED:
            Runtime::DisableRendering();
            break;
        case SIZE_MAXIMIZED:
            Runtime::EnableRendering();
            break;
        case SIZE_RESTORED:
            Runtime::EnableRendering();
            break;
        default:
            break;
        }
        Window::OnResize(SIZE{ LOWORD(lParam), HIWORD(lParam) });
        return 0;
    case WM_SHOWWINDOW:
        if (wParam == TRUE)
            Runtime::EnableRendering();
        else
            Runtime::DisableRendering();
        return 0;
    case WM_ENTERSIZEMOVE:
        Runtime::DisableRendering();
        return 0;
    case WM_EXITSIZEMOVE:
        Runtime::EnableRendering();
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void Window::OnResize(SIZE resolution)
{
    if (resolution.cx == 0 || resolution.cy == 0)
        return;
    
    // If fullscreen requested...
    // wndFrameWidth = GetSystemMetrics(SM_CXSCREEN);
    // wndFrameHeight = GetSystemMetrics(SM_CYSCREEN);

    Window::GetInstance().m_resolution = resolution;

    // Update full client area viewport
    Window::GetInstance().m_fullClientViewport.TopLeftX = 0.0f;
    Window::GetInstance().m_fullClientViewport.TopLeftY = 0.0f;
    Window::GetInstance().m_fullClientViewport.Width = static_cast<float>(resolution.cx);
    Window::GetInstance().m_fullClientViewport.Height = static_cast<float>(resolution.cy);
    Window::GetInstance().m_fullClientViewport.MinDepth = 0.0f;
    Window::GetInstance().m_fullClientViewport.MaxDepth = 1.0f;

    GraphicDevice::GetInstance().OnResize();
    CameraManager::GetInstance().OnResize();    // 카메라 렌더버퍼 재생성, 투영 행렬, D3D11 뷰포트 구조체 업데이트
}

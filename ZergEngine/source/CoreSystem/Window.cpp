#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>

namespace ze
{
    WindowImpl Window;
}

using namespace ze;

PCWSTR WNDCLASS_NAME = L"zemainframe";

WindowImpl::WindowImpl()
    : m_hWnd(NULL)
    // , m_style(WS_POPUP)  // 테두리 없음
    , m_style(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
    , m_width(0)
    , m_height(0)
    , m_fullscreen(false)
    , m_fullClientViewport { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
{
}

WindowImpl::~WindowImpl()
{
}

void WindowImpl::Init(void* pDesc)
{
    WindowImpl::InitDesc* pInitDesc = reinterpret_cast<WindowImpl::InitDesc*>(pDesc);

    m_hWnd = NULL;
    m_width = pInitDesc->m_width;
    m_height = pInitDesc->m_height;
    m_fullscreen = pInitDesc->m_fullscreen;

    m_fullClientViewport.TopLeftX = 0.0f;
    m_fullClientViewport.TopLeftY = 0.0f;
    m_fullClientViewport.Width = static_cast<float>(m_width);
    m_fullClientViewport.Height = static_cast<float>(m_height);
    m_fullClientViewport.MinDepth = 0.0f;
    m_fullClientViewport.MaxDepth = 1.0f;

    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowImpl::WinProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = Runtime.GetInstanceHandle();
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
        RECT cr = RECT{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        if (AdjustWindowRect(&cr, m_style, FALSE) == FALSE)
            Debug::ForceCrashWithWin32ErrorMessageBox(L"WindowImpl::Init() > AdjustWindowRect()", GetLastError());

        wndFrameWidth = cr.right - cr.left;
        wndFrameHeight = cr.bottom - cr.top;
    }

    // Create the window.
    m_hWnd = CreateWindowW(
        WNDCLASS_NAME,                                                  // Window class
        pInitDesc->m_title,                                             // Window title
        m_style,       // Window style
        CW_USEDEFAULT,                                                  // Pos X
        CW_USEDEFAULT,                                                  // Pos Y
        wndFrameWidth,                                                  // Width
        wndFrameHeight,                                                 // Height
        NULL,                                                           // Parent window    
        NULL,                                                           // Menu
        Runtime.GetInstanceHandle(),                                    // Instance handle
        nullptr                                                         // Additional application data
    );
}

void WindowImpl::Release()
{
}

void WindowImpl::SetResolution(uint32_t width, uint32_t height, bool fullscreen)
{
    if (width == 0 || height == 0)
        return;

    m_fullscreen = fullscreen;
    m_width = width;
    m_height = height;

    GraphicDevice.OnResize();
    CameraManager.OnResize();    // 카메라 렌더버퍼 재생성, 투영 행렬, D3D11 뷰포트 구조체 업데이트

    if (!m_fullscreen)
    {
        RECT cr = RECT{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        if (AdjustWindowRect(&cr, m_style, FALSE) == FALSE)
            Debug::ForceCrashWithWin32ErrorMessageBox(L"WindowImpl::SetResolution() > AdjustWindowRect()", GetLastError());

        const int wndFrameWidth = cr.right - cr.left;
        const int wndFrameHeight = cr.bottom - cr.top;
        MoveWindow(Window.GetWindowHandle(), 0, 0, wndFrameWidth, wndFrameHeight, FALSE);
    }

    // Update full client area viewport
    Window.m_fullClientViewport.TopLeftX = 0.0f;
    Window.m_fullClientViewport.TopLeftY = 0.0f;
    Window.m_fullClientViewport.Width = static_cast<float>(m_width);
    Window.m_fullClientViewport.Height = static_cast<float>(m_height);
    Window.m_fullClientViewport.MinDepth = 0.0f;
    Window.m_fullClientViewport.MaxDepth = 1.0f;
}

LRESULT WindowImpl::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            Runtime.DisableRendering();
            break;
        case SIZE_MAXIMIZED:
            Runtime.EnableRendering();
            break;
        case SIZE_RESTORED:
            Runtime.EnableRendering();
            break;
        default:
            break;
        }
        return 0;
    case WM_SHOWWINDOW:
        if (wParam == TRUE)
            Runtime.EnableRendering();
        else
            Runtime.DisableRendering();
        return 0;
    case WM_ENTERSIZEMOVE:
        Runtime.DisableRendering();
        return 0;
    case WM_EXITSIZEMOVE:
        Runtime.EnableRendering();
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

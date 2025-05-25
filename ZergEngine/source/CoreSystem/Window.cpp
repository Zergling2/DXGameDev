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
    , m_sizeFlt(0.0f, 0.0f)
    , m_halfSizeFlt(0.0f, 0.0f)
    , m_mode(WINDOW_MODE::WINDOWED)
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
    m_sizeFlt = XMFLOAT2(static_cast<FLOAT>(m_width), static_cast<FLOAT>(m_height));
    m_halfSizeFlt = XMFLOAT2(m_sizeFlt.x * 0.5f, m_sizeFlt.y * 0.5f);
    m_mode = pInitDesc->m_mode;

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
    RECT cr;

    switch (m_mode)
    {
    case WINDOW_MODE::WINDOWED:
        __fallthrough;
    case WINDOW_MODE::WINDOWED_FULLSCREEN:
        cr = RECT{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        if (AdjustWindowRect(&cr, m_style, FALSE) == FALSE)
            Debug::ForceCrashWithWin32ErrorMessageBox(L"AdjustWindowRect()", GetLastError());

        wndFrameWidth = cr.right - cr.left;
        wndFrameHeight = cr.bottom - cr.top;
        break;
    case WINDOW_MODE::FULLSCREEN:
        wndFrameWidth = GetSystemMetrics(SM_CXSCREEN);
        wndFrameHeight = GetSystemMetrics(SM_CYSCREEN);
        break;
    }

    // Create the window.
    m_hWnd = CreateWindowW(
        WNDCLASS_NAME,                                                  // Window class
        pInitDesc->mTitle,                                             // Window title
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

void WindowImpl::SetResolution(uint32_t width, uint32_t height, WINDOW_MODE mode)
{
    if (width == 0 || height == 0)
        return;

    m_width = width;
    m_height = height;
    m_sizeFlt = XMFLOAT2(static_cast<FLOAT>(m_width), static_cast<FLOAT>(m_height));
    m_halfSizeFlt = XMFLOAT2(m_sizeFlt.x * 0.5f, m_sizeFlt.y * 0.5f);
    m_mode = mode;

    GraphicDevice.OnResize();
    CameraManager.OnResize();    // 카메라 렌더버퍼 재생성, 투영 행렬, D3D11 뷰포트 구조체 업데이트

    // 아래 MoveWindow로 윈도우 크기 조절은 GraphicDevice::OnResize() 에서 SetFullscreenState 처리 뒤 처리해야 한다.
    if (m_mode == WINDOW_MODE::WINDOWED || m_mode == WINDOW_MODE::WINDOWED_FULLSCREEN)
    {
        int wndFrameWidth;
        int wndFrameHeight;
        RECT cr;

        cr = RECT{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        if (AdjustWindowRect(&cr, m_style, FALSE) == FALSE)
            Debug::ForceCrashWithWin32ErrorMessageBox(L"AdjustWindowRect()", GetLastError());

        wndFrameWidth = cr.right - cr.left;
        wndFrameHeight = cr.bottom - cr.top;
        MoveWindow(Window.GetWindowHandle(), 0, 0, wndFrameWidth, wndFrameHeight, FALSE);
    }
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

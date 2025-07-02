#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

PCTSTR WNDCLASS_NAME = _T("ZEMAINFRM");

Window::Window()
    : m_pHandler(nullptr)
    , m_hInstance(NULL)
    , m_hWnd(NULL)
    , m_clientSize{ 0, 0 }
{
}

Window::~Window()
{
}

void Window::Create(IWindowMessageHandler* pHandler, HINSTANCE hInstance, DWORD style, uint32_t width, uint32_t height, PCWSTR title)
{
    // 1. 초기값
    m_pHandler = pHandler;
    m_hInstance = hInstance;
    m_hWnd = NULL;
    m_clientSize.cx = 0;
    m_clientSize.cy = 0;

    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WinProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = hInstance;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = WNDCLASS_NAME;
    wcex.hIconSm = NULL;
    RegisterClassEx(&wcex);

    RECT cr;
    cr.left = 0;
    cr.top = 0;
    cr.right = static_cast<LONG>(width);
    cr.bottom = static_cast<LONG>(height);
    if (AdjustWindowRect(&cr, style, FALSE) == FALSE)
        Debug::ForceCrashWithWin32ErrorMessageBox(L"AdjustWindowRect()", GetLastError());

    // Create the window.
    m_hWnd = CreateWindowW(
        WNDCLASS_NAME,          // Window class
        title,                  // Window title
        style,                  // Window style
        CW_USEDEFAULT,          // Pos X
        CW_USEDEFAULT,          // Pos Y
        cr.right - cr.left,     // Width
        cr.bottom - cr.top,     // Height
        NULL,                   // Parent window    
        NULL,                   // Menu
        hInstance,              // Instance handle
        this                // Additional application data
    );

    if (m_hWnd)
    {
        m_clientSize.cx = width;
        m_clientSize.cy = height;
    }
}

void Window::Destroy()
{
    DestroyWindow(m_hWnd);

    UnregisterClass(WNDCLASS_NAME, m_hInstance);
}

DWORD Window::GetStyle() const
{
    DWORD currentStyle = static_cast<DWORD>(GetWindowLongPtr(m_hWnd, GWL_STYLE));

    return currentStyle;
}

void Window::SetStyle(DWORD style)
{
    // MSDN (SetWindowLongPtr 호출 후에는 SetWindowPos 함수 호출 필요)
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowlongptra?redirectedfrom=MSDN
    // Certain window data is cached, so changes you make using SetWindowLongPtr will not take
    // effect until you call the SetWindowPos function.
    SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

    // 새 스타일 적용을 위해서 호출 필요 (MSDN 해당 내용 Remark 섹션 내용에서 SWP_SHOWWINDOW 플래그 안 넣으니 오작동함)
    SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

BOOL Window::ShowWindow(int nCmdShow)
{
    return ::ShowWindow(m_hWnd, nCmdShow);
}

BOOL Window::Resize(uint32_t width, uint32_t height)
{
    const DWORD currentStyle = this->GetStyle();

    RECT cr;
    cr.left = 0;
    cr.top = 0;
    cr.right = static_cast<LONG>(width);
    cr.bottom = static_cast<LONG>(height);

    if (AdjustWindowRect(&cr, currentStyle, FALSE) == FALSE)
        return FALSE;

    BOOL ret = SetWindowPos(m_hWnd, NULL, 0, 0, cr.right - cr.left, cr.bottom - cr.top, 0);    // WM_SIZE 메시지 생성
    if (ret)
    {
        m_clientSize.cx = width;
        m_clientSize.cy = height;
    }

    return ret;
}

LRESULT Window::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IWindowMessageHandler* pHandler = reinterpret_cast<IWindowMessageHandler*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    LRESULT ret = 0;

    switch (uMsg)
    {
    case WM_CREATE:
        pHandler = reinterpret_cast<Window*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams)->m_pHandler;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pHandler));
        pHandler->OnCreate(wParam, lParam);
        break;
    case WM_DESTROY:
        pHandler->OnDestroy(wParam, lParam);
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        pHandler->OnSize(wParam, lParam);
        break;
    case WM_SHOWWINDOW:
        pHandler->OnShowWindow(wParam, lParam);
        break;
    case WM_CHAR:
        pHandler->OnChar(wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        pHandler->OnMouseMove(wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        pHandler->OnLButtonDown(wParam, lParam);
        break;
    case WM_LBUTTONUP:
        pHandler->OnLButtonUp(wParam, lParam);
        break;
    case WM_RBUTTONDOWN:
        pHandler->OnRButtonDown(wParam, lParam);
        break;
    case WM_RBUTTONUP:
        pHandler->OnRButtonUp(wParam, lParam);
        break;
    case WM_MBUTTONDOWN:
        pHandler->OnMButtonDown(wParam, lParam);
        break;
    case WM_MBUTTONUP:
        pHandler->OnMButtonUp(wParam, lParam);
        break;
    case WM_ENTERSIZEMOVE:
        pHandler->OnEnterSizeMove(wParam, lParam);
        break;
    case WM_EXITSIZEMOVE:
        pHandler->OnExitSizeMove(wParam, lParam);
        break;
    default:
        ret = DefWindowProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return ret;
}

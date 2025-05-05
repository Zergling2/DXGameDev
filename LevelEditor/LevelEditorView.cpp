// LevelEditorView.cpp : implementation of the CLevelEditorView class
//

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LevelEditor.h"
#endif

#include "LevelEditorView.h"
#include "LevelEditorDoc.h"
#include "Settings.h"
#include "Helper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelEditorView

IMPLEMENT_DYNCREATE(CLevelEditorView, CView)

BEGIN_MESSAGE_MAP(CLevelEditorView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// CLevelEditorView construction/destruction

CLevelEditorView::CLevelEditorView() noexcept
    : m_pDevice(nullptr)
    , m_pImmediateContext(nullptr)
    , m_pSwapChain(nullptr)
    , m_pSwapChainRTV(nullptr)
{
	// TODO: add construction code here
}

CLevelEditorView::~CLevelEditorView()
{
}

BOOL CLevelEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLevelEditorView drawing

void CLevelEditorView::OnDraw(CDC* /*pDC*/)
{
	CLevelEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CLevelEditorView diagnostics

#ifdef _DEBUG
void CLevelEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CLevelEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLevelEditorDoc* CLevelEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLevelEditorDoc)));
	return (CLevelEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void CLevelEditorView::Render()
{
    m_pImmediateContext->ClearRenderTargetView(m_pSwapChainRTV, Colors::Blue);

    m_pSwapChain->Present(1, 0);
}

BOOL CLevelEditorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;

	// return CView::OnEraseBkgnd(pDC);
}


int CLevelEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
    
    DXGI_SWAP_CHAIN_DESC descSwapChain;
    ZeroMemory(&descSwapChain, sizeof(descSwapChain));
    descSwapChain.BufferDesc.Format = BACK_BUFFER_FORMAT;
    // descSwapChain.BufferDesc.Width = 0;
    // descSwapChain.BufferDesc.Height = 0;
    // descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    descSwapChain.SampleDesc.Count = 1;
    descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    descSwapChain.BufferCount = 2;
    descSwapChain.OutputWindow = this->m_hWnd;
    descSwapChain.Windowed = TRUE;
    descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    descSwapChain.Flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT createDeviceFlags = 0;
#endif

    D3D_FEATURE_LEVEL featureLevelTest[] =
    {
        D3D_FEATURE_LEVEL_11_1
    };
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelTest,
        _countof(featureLevelTest),
        D3D11_SDK_VERSION,
        &descSwapChain,
        &m_pSwapChain,
        &m_pDevice,
        &featureLevel,
        &m_pImmediateContext
    );
    if (FAILED(hr))
        return -1;

    // RTV 생성
    ComPtr<ID3D11Texture2D> cpBackBuffer;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(cpBackBuffer.GetAddressOf()));
    if (FAILED(hr))
        return -1;

    hr = m_pDevice->CreateRenderTargetView(cpBackBuffer.Get(), nullptr, &m_pSwapChainRTV);
    if (FAILED(hr))
        return -1;

    theApp.m_pLevelEditorView = this;

	return 0;
}


void CLevelEditorView::OnDestroy()
{
    // TODO: Add your message handler code here
    SafeReleaseComPtr(m_pSwapChainRTV);
    SafeReleaseComPtr(m_pSwapChain);
    SafeReleaseComPtr(m_pImmediateContext);
    SafeReleaseComPtr(m_pDevice);

    CView::OnDestroy();
}


void CLevelEditorView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    WCHAR log[64];
    CRect cr;

    if (!m_pDevice || !m_pSwapChain)
        return;

    GetClientRect(&cr);
    StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnSize() cr: %d %d %d %d\n", cr.left, cr.top, cr.right, cr.bottom);
    OutputDebugStringW(log);

    switch (nType)
    {
    case SIZE_MINIMIZED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_MINIMIZED\n");
        break;
    case SIZE_MAXIMIZED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_MAXIMIZED\n");
        break;
    case SIZE_RESTORED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_RESTORED\n");
        break;
    default:
        StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnSize() Parameter nType was %u\n", nType);
        OutputDebugStringW(log);
        break;
    }

    // 스왑 체인 간접/직접 참조 모두 해제
    SafeReleaseComPtr(m_pSwapChainRTV);

    // Resize
    m_pSwapChain->ResizeBuffers(0, cx, cy, DXGI_FORMAT_UNKNOWN, 0);

    // RTV 재생성
    ComPtr<ID3D11Texture2D> backBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pSwapChainRTV);

    // 카메라 투영행렬 업데이트
    // ...
}

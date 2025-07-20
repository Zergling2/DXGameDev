// LevelEditorView.cpp : implementation of the CLevelEditorView class
//

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LevelEditor.h"
#endif

#include "LevelEditorView.h"
#include "LevelEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelEditorView

IMPLEMENT_DYNCREATE(CLevelEditorView, CView)

BEGIN_MESSAGE_MAP(CLevelEditorView, CView)
	ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CLevelEditorView construction/destruction

CLevelEditorView::CLevelEditorView() noexcept
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

BOOL CLevelEditorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;

	// return CView::OnEraseBkgnd(pDC);
}

void CLevelEditorView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    WCHAR log[64];
    CRect cr;

    GetClientRect(&cr);
    StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnSize() cr: %d %d %d %d\n", cr.left, cr.top, cr.right, cr.bottom);
    OutputDebugStringW(log);

    switch (nType)
    {
    case SIZE_RESTORED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_RESTORED\n");
        break;
    case SIZE_MINIMIZED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_MINIMIZED\n");
        break;
    case SIZE_MAXIMIZED:
        OutputDebugStringW(L"CLevelEditorView::OnSize SIZE_MAXIMIZED\n");
        break;
    default:
        StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnSize() Parameter nType was %u\n", nType);
        OutputDebugStringW(log);
        break;
    }

    ze::Runtime* pRuntime = ze::Runtime::GetInstance();
    if (pRuntime)
        pRuntime->OnSize(nType, cx, cy);
}

void CLevelEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    WCHAR log[64];

    StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnLButtonDown() CPoint: %d %d\n", point.x, point.y);
    OutputDebugStringW(log);

    ze::Runtime::GetInstance()->OnLButtonDown(nFlags, point);

    CView::OnLButtonDown(nFlags, point);
}

void CLevelEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    WCHAR log[64];

    StringCbPrintfW(log, sizeof(log), L"CLevelEditorView::OnLButtonUp() CPoint: %d %d\n", point.x, point.y);
    OutputDebugStringW(log);

    ze::Runtime::GetInstance()->OnLButtonUp(nFlags, point);

    CView::OnLButtonUp(nFlags, point);
}

void CLevelEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    ze::Runtime::GetInstance()->OnMouseMove(nFlags, point);

    CView::OnMouseMove(nFlags, point);
}

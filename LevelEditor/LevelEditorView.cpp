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
    ON_WM_MOUSEMOVE()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CLevelEditorView construction/destruction

CLevelEditorView::CLevelEditorView() noexcept
    : m_focused(false)
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
    if (pRuntime && cx > 0 && cy > 0)
        pRuntime->OnSize(nType, cx, cy);
}

void CLevelEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    ze::Runtime::GetInstance()->OnMouseMove(nFlags, point);

    CView::OnMouseMove(nFlags, point);
}

void CLevelEditorView::OnSetFocus(CWnd* pOldWnd)
{
    CView::OnSetFocus(pOldWnd);

    // TODO: Add your message handler code here
    m_focused = true;
    OutputDebugString(_T("OnSetFocus\n"));
}

void CLevelEditorView::OnKillFocus(CWnd* pNewWnd)
{
    CView::OnKillFocus(pNewWnd);

    // TODO: Add your message handler code here
    m_focused = false;
    OutputDebugString(_T("OnKillFocus\n"));
}

void CLevelEditorView::OnDestroy()
{
    CView::OnDestroy();

    // TODO: Add your message handler code here
    ze::Runtime::GetInstance()->DestroyAllObject();
    ze::Runtime::GetInstance()->RemoveDestroyedComponentsAndObjects();
}

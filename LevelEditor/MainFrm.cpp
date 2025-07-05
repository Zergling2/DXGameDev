// MainFrm.cpp : implementation of the CMainFrame class
//

#include "MainFrm.h"
#include "LevelEditorView.h"
#include "ComponentListView.h"
#include "TransformInspectorFormView.h"
#include "MeshRendererInspectorFormView.h"
#include "HierarchyTreeView.h"
#include "LogListView.h"
#include "TerrainGenerationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_3DOBJECT_TERRAIN, &CMainFrame::On3DObjectTerrain)
	ON_COMMAND(ID_GAMEOBJECT_CREATEEMPTY, &CMainFrame::OnGameObjectCreateEmpty)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

constexpr int FIRST_LRSPLIT_RIGHT_WIDTH = 332;		// ComponentListView, ComponentInspectorFormView 너비
constexpr int SECOND_UDSPLIT_BOTTOM_HEIGHT = 256;	// LogListView 높이
constexpr int THIRD_LRSPLIT_LEFT_WIDTH = 256;		// HierarchyTreeView 너비
constexpr int FOURTH_UDSPLIT_TOP_HEIGHT = 256;		// ComponentListView 높이

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
	: m_splitterCreated(false)
	, m_wndSplitter{}
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	BOOL result = TRUE;

	do
	{
		CRect cr;
		GetClientRect(&cr);
		
		// ################################################
		// 1. 좌우 분할
		result = m_wndSplitter[0].CreateStatic(this, 1, 2);
		if (!result)
			break;

		// 2. 좌측 상하 분할
		result = m_wndSplitter[1].CreateStatic(&m_wndSplitter[0], 2, 1,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[0].IdFromRowCol(0, 0));
		if (!result)
			break;

		// 3. 좌측 상단 좌우 분할 (HierarchyTreeView | DirectXRenderingView)
		result = m_wndSplitter[2].CreateStatic(&m_wndSplitter[1], 1, 2,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[1].IdFromRowCol(0, 0));
		if (!result)
			break;

		// 4. 우측 상하 분할 (ComponentListView | ComponentInspectorFormView)
		result = m_wndSplitter[3].CreateStatic(&m_wndSplitter[0], 2, 1,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[0].IdFromRowCol(0, 1));
		if (!result)
			break;

		// 5. HierarchyTreeView 생성
		result = m_wndSplitter[2].CreateView(0, 0,
			RUNTIME_CLASS(CHierarchyTreeView),
			CSize(THIRD_LRSPLIT_LEFT_WIDTH, 0),
			pContext
		);
		if (!result)
			break;

		// 6. DirectXRenderingView 생성
		result = m_wndSplitter[2].CreateView(0, 1,
			RUNTIME_CLASS(CLevelEditorView),
			CSize(0, 0),
			pContext
		);
		if (!result)
			break;

		// 7. LogListView 생성
		result = m_wndSplitter[1].CreateView(1, 0,
			RUNTIME_CLASS(CLogListView),
			CSize(0, SECOND_UDSPLIT_BOTTOM_HEIGHT),
			pContext
		);
		if (!result)
			break;

		// 8. ComponentListView 생성
		result = m_wndSplitter[3].CreateView(0, 0,
			RUNTIME_CLASS(CComponentListView),
			CSize(0, FIRST_LRSPLIT_RIGHT_WIDTH),
			pContext
		);
		if (!result)
			break;

		// 9. ComponentInspectorFormView 생성
		result = m_wndSplitter[3].CreateView(1, 0,
			RUNTIME_CLASS(CTransformInspectorFormView),
			CSize(0, 0),
			pContext
		);
		if (!result)
			break;

		m_splitterCreated = true;

		m_wndSplitter[0].SetColumnInfo(0, cr.Width() - FIRST_LRSPLIT_RIGHT_WIDTH, 0);
		m_wndSplitter[1].SetRowInfo(0, cr.Height() - SECOND_UDSPLIT_BOTTOM_HEIGHT, 0);
		m_wndSplitter[2].SetColumnInfo(0, THIRD_LRSPLIT_LEFT_WIDTH, 0);
		m_wndSplitter[3].SetRowInfo(0, FOURTH_UDSPLIT_TOP_HEIGHT, 0);
	} while (false);

	return result;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG



void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 1366;
	lpMMI->ptMinTrackSize.y = 768;

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	WCHAR log[64];
	CRect cr;

	if (!m_splitterCreated)
		return;

	switch (nType)
	{
	case SIZE_MINIMIZED:
		break;
	case SIZE_MAXIMIZED:
		__fallthrough;
	case SIZE_RESTORED:
		GetClientRect(&cr);

		m_wndSplitter[0].SetColumnInfo(0, cr.Width() - FIRST_LRSPLIT_RIGHT_WIDTH, 0);
		m_wndSplitter[1].SetRowInfo(0, cr.Height() - SECOND_UDSPLIT_BOTTOM_HEIGHT, 0);
		m_wndSplitter[2].SetColumnInfo(0, THIRD_LRSPLIT_LEFT_WIDTH, 0);
		m_wndSplitter[3].SetRowInfo(0, FOURTH_UDSPLIT_TOP_HEIGHT, 0);

		m_wndSplitter[0].RecalcLayout();
		m_wndSplitter[1].RecalcLayout();
		m_wndSplitter[2].RecalcLayout();
		m_wndSplitter[3].RecalcLayout();
		break;
	default:
		StringCbPrintfW(log, sizeof(log), L"CMainFrame::OnSize() Parameter nType was %u\n", nType);
		OutputDebugStringW(log);
		break;
	}
}

void CMainFrame::On3DObjectTerrain()
{
	// TODO: Add your command handler code here
	CTerrainGenerationDialog dlg;
	dlg.DoModal();
}

void CMainFrame::OnGameObjectCreateEmpty()
{
	// TODO: Add your command handler code here

	
}

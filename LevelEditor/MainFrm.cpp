// MainFrm.cpp : implementation of the CMainFrame class
//

#include "MainFrm.h"
#include "LevelEditorView.h"
#include "ComponentListView.h"
#include "EmptyInspectorFormView.h"
#include "TransformInspectorFormView.h"
#include "MeshRendererInspectorFormView.h"
#include "HierarchyTreeView.h"
#include "LogListView.h"
#include "AssetTreeView.h"
#include "CommandHandler\CommandHandler.h"

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
	ON_COMMAND(ID_GAMEOBJECT_RENAME, &CMainFrame::OnGameObjectRename)
	ON_COMMAND(ID_LIGHT_DIRECTIONALLIGHT, &CMainFrame::OnLightDirectionalLight)
	ON_COMMAND(ID_LIGHT_POINTLIGHT, &CMainFrame::OnLightPointLight)
	ON_COMMAND(ID_LIGHT_SPOTLIGHT, &CMainFrame::OnLightSpotLight)
	ON_COMMAND(ID_COMPONENT_MESHRENDERER, &CMainFrame::OnComponentMeshRenderer)
	ON_COMMAND(ID_CREATE_FOLDER, &CMainFrame::OnCreateAssetFolder)
	ON_COMMAND(ID_CREATE_MATERIAL, &CMainFrame::OnCreateAssetMaterial)
	ON_COMMAND(ID_CREATE_TEXTURE, &CMainFrame::OnCreateAssetTexture)
	ON_COMMAND(ID_CREATE_WAVEFRONTOBJ, &CMainFrame::OnCreateAssetWavefrontOBJ)
	ON_COMMAND(ID_RENDERING_LIGHTING, &CMainFrame::OnRenderingLighting)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

constexpr int LOG_LIST_VIEW_WIDTH = 512;			// LogListView 높이
constexpr int LOG_LIST_VIEW_HEIGHT = 256;			// LogListView 높이
constexpr int HIERARCHY_TREE_VIEW_WIDTH = 256;		// HierarchyTreeView 너비
constexpr int HIERARCHY_TREE_VIEW_HEIGHT = 600;		// HierarchyTreeView 높이
constexpr int COMPONENT_LIST_VIEW_WIDTH = 332;		// ComponentListView, InspectorFormView 너비
constexpr int COMPONENT_LIST_VIEW_HEIGHT = 256;		// ComponentListView 높이

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
	: m_splitterCreated(false)
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

CHierarchyTreeView* CMainFrame::GetHierarchyTreeView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CHierarchyTreeView*>(m_wndSplitter[2].GetPane(0, 0));
}

CComponentListView* CMainFrame::GetComponentListView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CComponentListView*>(m_wndSplitter[4].GetPane(0, 0));
}

CLevelEditorView* CMainFrame::GetLevelEditorView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CLevelEditorView*>(m_wndSplitter[2].GetPane(0, 1));
}

CLogListView* CMainFrame::GetLogListView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CLogListView*>(m_wndSplitter[3].GetPane(0, 0));
}

CAssetTreeView* CMainFrame::GetAssetTreeView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CAssetTreeView*>(m_wndSplitter[3].GetPane(0, 1));
}

CFormView* CMainFrame::GetInspectorFormView() const
{
	if (!m_splitterCreated)
		return nullptr;

	return static_cast<CFormView*>(m_wndSplitter[4].GetPane(1, 0));
}

CFormView* CMainFrame::SwitchInspectorFormView(CRuntimeClass* pRtClass)
{
	CCreateContext context;
	CView* pOldView = static_cast<CView*>(m_wndSplitter[4].GetPane(1, 0));
	
	if (!pOldView)
		return nullptr;

	/*
	if (pOldView->IsKindOf(pRtClass))
		return static_cast<CFormView*>(pOldView);
	*/

	// CCreateContext 세팅
	context.m_pCurrentDoc = pOldView->GetDocument();	// pOldView가 파괴되기 전에 도큐먼트 포인터 값 얻어오기
	context.m_pNewViewClass = pRtClass;
	context.m_pNewDocTemplate = NULL;
	context.m_pLastView = NULL;
	context.m_pCurrentFrame = this;

	// 기존 뷰 제거
	pOldView->DestroyWindow();
	
	// 새 뷰 생성
	if (!m_wndSplitter[4].CreateView(1, 0, pRtClass, CSize(0, 0), &context))
		return nullptr;
	
	// 새 뷰 활성화
	CView* pNewView = static_cast<CView*>(m_wndSplitter[4].GetPane(1, 0));
	
	pNewView->SendMessage(WM_INITIALUPDATE);

	m_wndSplitter[4].RecalcLayout();
	
	return static_cast<CFormView*>(pNewView);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
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

		// 3. 좌측 상단 좌우 분할 (HierarchyTreeView | LevelEditorView)
		result = m_wndSplitter[2].CreateStatic(&m_wndSplitter[1], 1, 2,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[1].IdFromRowCol(0, 0));
		if (!result)
			break;

		// 5. 좌측 하단 좌우 분할 (LogListView | AssetListView)
		result = m_wndSplitter[3].CreateStatic(&m_wndSplitter[1], 1, 2,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[1].IdFromRowCol(1, 0));
		if (!result)
			break;

		// 4. 우측 상하 분할 (ComponentListView | InspectorFormView)
		result = m_wndSplitter[4].CreateStatic(&m_wndSplitter[0], 2, 1,
			WS_CHILD | WS_VISIBLE, m_wndSplitter[0].IdFromRowCol(0, 1));
		if (!result)
			break;

		

		// HierarchyTreeView 생성
		result = m_wndSplitter[2].CreateView(0, 0,
			RUNTIME_CLASS(CHierarchyTreeView),
			CSize(HIERARCHY_TREE_VIEW_WIDTH, HIERARCHY_TREE_VIEW_HEIGHT),
			pContext
		);
		if (!result)
			break;

		// LevelEditorView 생성
		result = m_wndSplitter[2].CreateView(0, 1,
			RUNTIME_CLASS(CLevelEditorView),
			CSize(0, 0),
			pContext
		);
		if (!result)
			break;

		// LogListView 생성
		result = m_wndSplitter[3].CreateView(0, 0,
			RUNTIME_CLASS(CLogListView),
			CSize(LOG_LIST_VIEW_WIDTH, LOG_LIST_VIEW_HEIGHT),
			pContext
		);
		if (!result)
			break;

		// AssetListView 생성
		result = m_wndSplitter[3].CreateView(0, 1,
			RUNTIME_CLASS(CAssetTreeView),
			CSize(0, 0),
			pContext
		);
		if (!result)
			break;

		// ComponentListView 생성
		result = m_wndSplitter[4].CreateView(0, 0,
			RUNTIME_CLASS(CComponentListView),
			CSize(COMPONENT_LIST_VIEW_WIDTH, COMPONENT_LIST_VIEW_HEIGHT),
			pContext
		);
		if (!result)
			break;

		// EmptyInspectorFormView 생성
		result = m_wndSplitter[4].CreateView(1, 0,
			RUNTIME_CLASS(CEmptyInspectorFormView),
			CSize(0, 0),
			pContext
		);
		if (!result)
			break;

		m_wndSplitter[0].SetColumnInfo(0, cr.Width() - COMPONENT_LIST_VIEW_WIDTH, 0);
		m_wndSplitter[1].SetRowInfo(0, cr.Height() - LOG_LIST_VIEW_HEIGHT, 0);
		m_wndSplitter[2].SetColumnInfo(0, HIERARCHY_TREE_VIEW_WIDTH, 0);
		m_wndSplitter[3].SetColumnInfo(0, LOG_LIST_VIEW_WIDTH, 0);
		m_wndSplitter[4].SetRowInfo(0, COMPONENT_LIST_VIEW_HEIGHT, 0);

		for (size_t i = 0; i < _countof(m_wndSplitter); ++i)
			m_wndSplitter[i].RecalcLayout();

		m_splitterCreated = true;
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

		m_wndSplitter[0].SetColumnInfo(0, cr.Width() - COMPONENT_LIST_VIEW_WIDTH, 0);
		m_wndSplitter[1].SetRowInfo(0, cr.Height() - LOG_LIST_VIEW_HEIGHT, 0);
		m_wndSplitter[2].SetColumnInfo(0, HIERARCHY_TREE_VIEW_WIDTH, 0);
		m_wndSplitter[3].SetColumnInfo(0, LOG_LIST_VIEW_WIDTH, 0);
		m_wndSplitter[4].SetRowInfo(0, COMPONENT_LIST_VIEW_HEIGHT, 0);

		for (size_t i = 0; i < _countof(m_wndSplitter); ++i)
			m_wndSplitter[i].RecalcLayout();
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
	::On3DObjectTerrain();
}

void CMainFrame::OnGameObjectCreateEmpty()
{
	// TODO: Add your command handler code here
	::OnGameObjectCreateEmpty();
}

void CMainFrame::OnGameObjectRename()
{
	// TODO: Add your command handler code here
	::OnGameObjectRename();
}

void CMainFrame::OnLightDirectionalLight()
{
	// TODO: Add your command handler code here
	::OnLightDirectionalLight();
}

void CMainFrame::OnLightPointLight()
{
	// TODO: Add your command handler code here
	::OnLightPointLight();
}

void CMainFrame::OnLightSpotLight()
{
	// TODO: Add your command handler code here
	::OnLightSpotLight();
}

void CMainFrame::OnComponentMeshRenderer()
{
	// TODO: Add your command handler code here
	::OnComponentMeshRenderer();
}

void CMainFrame::OnCreateAssetFolder()
{
	// TODO: Add your command handler code here
	::OnCreateAssetFolder();
}

void CMainFrame::OnCreateAssetMaterial()
{
	// TODO: Add your command handler code here
	::OnCreateAssetMaterial();
}

void CMainFrame::OnCreateAssetWavefrontOBJ()
{
	// TODO: Add your command handler code here
	::OnCreateAssetWavefrontOBJ();
}

void CMainFrame::OnCreateAssetTexture()
{
	// TODO: Add your command handler code here
	::OnCreateAssetTexture();
}

void CMainFrame::OnRenderingLighting()
{
	// TODO: Add your command handler code here
	::OnRenderingLighting();
}

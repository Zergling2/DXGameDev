#include "HierarchyTreeView.h"
#include "LevelEditor.h"
#include "Settings.h"
#include "HTVItem\HTVItemRoot.h"
#include "HTVItem\HTVItemEmpty.h"
#include "CommandHandler\CommandHandler.h"

// CHierarchyTreeView

IMPLEMENT_DYNCREATE(CHierarchyTreeView, CTreeView)

CHierarchyTreeView::CHierarchyTreeView()
	: m_initialized(false)
{
}

CHierarchyTreeView::~CHierarchyTreeView()
{
}

BEGIN_MESSAGE_MAP(CHierarchyTreeView, CTreeView)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CHierarchyTreeView::OnNMRClick)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CHierarchyTreeView::OnTvnEndlabeledit)
	ON_COMMAND(ID_3DOBJECT_TERRAIN, &CHierarchyTreeView::On3DObjectTerrain)
	ON_COMMAND(ID_GAMEOBJECT_CREATEEMPTY, &CHierarchyTreeView::OnGameObjectCreateEmpty)
	ON_COMMAND(ID_GAMEOBJECT_RENAME, &CHierarchyTreeView::OnGameObjectRename)
	ON_COMMAND(ID_LIGHT_DIRECTIONALLIGHT, &CHierarchyTreeView::OnLightDirectionalLight)
	ON_COMMAND(ID_LIGHT_POINTLIGHT, &CHierarchyTreeView::OnLightPointLight)
	ON_COMMAND(ID_LIGHT_SPOTLIGHT, &CHierarchyTreeView::OnLightSpotLight)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CHierarchyTreeView diagnostics

#ifdef _DEBUG
void CHierarchyTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CHierarchyTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG

void CHierarchyTreeView::RemoveTreeItemPostOrder(HTREEITEM hItem)
{
	// 자식 항목이 있으면 자식 항목을 먼저 제거 (후위 순회)
	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hChildItem = tc.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		HTREEITEM hNextChild = tc.GetNextSiblingItem(hChildItem);	// 미리 구해놔야 함
		RemoveTreeItemPostOrder(hChildItem);	// 자식 항목을 후위 순회 방식으로 제거
		hChildItem = hNextChild;				// 자식 항목의 형제 순회
	}

	// 자식 항목 제거 후 자신을 삭제
	IHTVItem* pHTVItem = reinterpret_cast<IHTVItem*>(tc.GetItemData(hItem));
	delete pHTVItem;
	tc.DeleteItem(hItem);
}

BOOL CHierarchyTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_TRACKSELECT;

	return CTreeView::PreCreateWindow(cs);
}


void CHierarchyTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl& tc = GetTreeCtrl();

	if (!m_initialized)
	{
		// 1. 스타일 및 배경, 텍스트 색상 설정
		tc.SetBkColor(HIERARCHY_TREEVIEW_BK_COLOR);
		tc.SetTextColor(HIERARCHY_TREEVIEW_TEXT_COLOR);

		// 2. 아이콘 리스트 로드 및 설정
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		tc.SetImageList(&iconList, TVSIL_NORMAL);

		// Scene 루트 노드 추가
		HTVItemRoot* pHTVItemRoot = new HTVItemRoot();
		tc.InsertItem(
			TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE,
			_T("New Scene"),
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			0,
			0,
			reinterpret_cast<LPARAM>(pHTVItemRoot),
			TVI_ROOT,
			TVI_LAST
		);

		m_initialized = true;
	}
}


// CHierarchyTreeView message handlers
void CHierarchyTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CTreeCtrl& tc = this->GetTreeCtrl();

	UINT flags;
	HTREEITEM hItem = tc.HitTest(point, &flags);
	if (hItem != NULL)
	{
		tc.SelectItem(hItem);
		DWORD_PTR data = tc.GetItemData(hItem);
		IHTVItem* pHTVItem = reinterpret_cast<IHTVItem*>(data);
		ASSERT(pHTVItem != nullptr);

		pHTVItem->OnSelect();
	}
	else
	{
		tc.SelectItem(NULL);
		HTVItemEmpty e;
		e.OnSelect();
	}

	CTreeView::OnLButtonDown(nFlags, point);
}


void CHierarchyTreeView::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	CTreeCtrl& tc = this->GetTreeCtrl();
	
	CPoint pt;
	GetCursorPos(&pt);

	CPoint clientPt = pt;
	tc.ScreenToClient(&clientPt);

	UINT flags;
	HTREEITEM hItem = tc.HitTest(clientPt, &flags);
	if (hItem != NULL)
	{
		tc.SelectItem(hItem);
		DWORD_PTR data = tc.GetItemData(hItem);
		IHTVItem* pHTVItem = reinterpret_cast<IHTVItem*>(data);
		ASSERT(pHTVItem != nullptr);

		pHTVItem->OnSelect();
	}
	else
	{
		tc.SelectItem(NULL);
		HTVItemEmpty e;
		e.OnSelect();
	}

	// 메뉴 출력
	CMenu menu;
	if (menu.LoadMenu(IDR_MAINFRAME))
	{
		CMenu* pSubMenu = menu.GetSubMenu(GAMEOBJECT_MENU_INDEX);
		if (pSubMenu)
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	}

	*pResult = 0;
}

void CHierarchyTreeView::OnTvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (pTVDispInfo->item.pszText != nullptr)
	{
		// 문자열 교체
		this->GetTreeCtrl().SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
	}

	*pResult = 0;
}

void CHierarchyTreeView::On3DObjectTerrain()
{
	// TODO: Add your command handler code here
	::On3DObjectTerrain();
}

void CHierarchyTreeView::OnGameObjectCreateEmpty()
{
	// TODO: Add your command handler code here
	::OnGameObjectCreateEmpty();
}

void CHierarchyTreeView::OnGameObjectRename()
{
	// TODO: Add your command handler code here
	::OnGameObjectRename();
}

void CHierarchyTreeView::OnLightDirectionalLight()
{
	// TODO: Add your command handler code here
	::OnLightDirectionalLight();
}

void CHierarchyTreeView::OnLightPointLight()
{
	// TODO: Add your command handler code here
	::OnLightPointLight();
}

void CHierarchyTreeView::OnLightSpotLight()
{
	// TODO: Add your command handler code here
	::OnLightSpotLight();
}

void CHierarchyTreeView::OnDestroy()
{
	// CTreeView::OnDestroy가 호출되면 트리 컨트롤이 파괴되므로 그 전에 트리 아이템에 부착된 동적할당 Data를 해제한다.
	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hRootItem = tc.GetRootItem();
	// 루트 항목부터 시작하여 후위 순회 방식으로 모든 항목 제거
	while (hRootItem != NULL)
	{
		HTREEITEM hNextRootItem = tc.GetNextSiblingItem(hRootItem);	// RemoveItremPostOrder 함수에 들어가면 hRootItem이 삭제되므로 미리 구해놔야 함.
		RemoveTreeItemPostOrder(hRootItem);

		// 다음 형제 항목으로
		hRootItem = hNextRootItem;
	}
	tc.DeleteAllItems();	// 불필요

	CTreeView::OnDestroy();
	// TODO: Add your message handler code here
	// ...

}

#include "AssetTreeView.h"
#include "LevelEditor.h"
#include "Settings.h"
#include "ATVItem\ATVItemEmpty.h"
#include "ATVItem\ATVItemFolder.h"
#include "CommandHandler\CommandHandler.h"

// CAssetTreeView

IMPLEMENT_DYNCREATE(CAssetTreeView, CTreeView)

CAssetTreeView::CAssetTreeView()
	: m_initialized(false)
{
}

CAssetTreeView::~CAssetTreeView()
{
}

BEGIN_MESSAGE_MAP(CAssetTreeView, CTreeView)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CAssetTreeView::OnNMRClick)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CAssetTreeView::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CAssetTreeView::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CAssetTreeView::OnTvnSelchanged)
	ON_COMMAND(ID_CREATE_FOLDER, &CAssetTreeView::OnCreateAssetFolder)
	ON_COMMAND(ID_CREATE_MATERIAL, &CAssetTreeView::OnCreateAssetMaterial)
	ON_COMMAND(ID_CREATE_TEXTURE, &CAssetTreeView::OnCreateAssetTexture)
	ON_COMMAND(ID_CREATE_WAVEFRONTOBJ, &CAssetTreeView::OnCreateAssetWavefrontOBJ)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAssetTreeView diagnostics

#ifdef _DEBUG
void CAssetTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CAssetTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG

void CAssetTreeView::RemoveTreeItemPostOrder(HTREEITEM hItem)
{
	// 자식 항목이 있으면 자식 항목을 먼저 제거 (후위 순회)
	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hChildItem = tc.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		HTREEITEM hNextChild = tc.GetNextSiblingItem(hChildItem);		// 미리 구해놔야 함
		RemoveTreeItemPostOrder(hChildItem);	// 자식 항목을 후위 순회 방식으로 제거
		hChildItem = hNextChild;				// 자식 항목의 형제 순회
	}

	// 자식 항목 제거 후 자신을 삭제
	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hItem));
	delete pATVItem;
	tc.DeleteItem(hItem);
}

BOOL CAssetTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_TRACKSELECT;

	return CTreeView::PreCreateWindow(cs);
}


void CAssetTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl& tc = GetTreeCtrl();

	if (!m_initialized)
	{
		// 1. 스타일 및 배경, 텍스트 색상 설정
		tc.SetBkColor(ASSET_TREEVIEW_BK_COLOR);
		tc.SetTextColor(ASSET_TREEVIEW_TEXT_COLOR);

		// 2. 아이콘 리스트 로드 및 설정
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		tc.SetImageList(&iconList, TVSIL_NORMAL);

		// 루트 노드 추가
		ATVItemFolder* pATVItemFolder = new ATVItemFolder();
		tc.InsertItem(
			TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE,
			_T("Asset"),
			ZE_ICON_INDEX::FOLDER_CLOSED_ICON,
			ZE_ICON_INDEX::FOLDER_CLOSED_ICON,
			0,
			0,
			reinterpret_cast<LPARAM>(pATVItemFolder),
			TVI_ROOT,
			TVI_LAST
		);

		m_initialized = true;
	}
}

// CAssetTreeView message handlers
void CAssetTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CTreeCtrl& tc = this->GetTreeCtrl();

	UINT flags;
	HTREEITEM hItem = tc.HitTest(point, &flags);
	if (hItem != NULL)
	{
		tc.SelectItem(hItem);
		DWORD_PTR data = tc.GetItemData(hItem);
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(data);
		assert(pATVItem != nullptr);

		pATVItem->OnSelect();
	}
	else
	{
		tc.SelectItem(NULL);
		ATVItemEmpty e;
		e.OnSelect();
	}

	CTreeView::OnLButtonDown(nFlags, point);
}


void CAssetTreeView::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
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
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(data);
		assert(pATVItem != nullptr);

		pATVItem->OnSelect();
	}
	else
	{
		tc.SelectItem(NULL);
		ATVItemEmpty e;
		e.OnSelect();
	}

	// 메뉴 출력
	CMenu menu;
	if (menu.LoadMenu(IDR_MAINFRAME))
	{
		CMenu* pSubMenu = menu.GetSubMenu(ASSET_MENU_INDEX);
		if (pSubMenu)
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	}


	*pResult = 0;
}


void CAssetTreeView::OnTvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
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


void CAssetTreeView::OnTvnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CTreeCtrl& tc = this->GetTreeCtrl();
	const HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	const UINT action = pNMTreeView->action;

	switch (action)
	{
	case TVE_COLLAPSE:
		tc.SetItemImage(hItem, ZE_ICON_INDEX::FOLDER_CLOSED_ICON, ZE_ICON_INDEX::FOLDER_CLOSED_ICON);
		break;
	case TVE_EXPAND:
		tc.SetItemImage(hItem, ZE_ICON_INDEX::FOLDER_OPENED_ICON, ZE_ICON_INDEX::FOLDER_OPENED_ICON);
		break;
	default:
		break;
	}

	*pResult = 0;
}


void CAssetTreeView::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CTreeCtrl& tc = this->GetTreeCtrl();
	const HTREEITEM hSelectedItem = pNMTreeView->itemNew.hItem;

	if (hSelectedItem != NULL)
	{
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
		pATVItem->OnSelect();
	}
	else
	{
		ATVItemEmpty e;
		e.OnSelect();
	}

	*pResult = 0;
}


void CAssetTreeView::OnCreateAssetFolder()
{
	// TODO: Add your command handler code here
	::OnCreateAssetFolder();
}


void CAssetTreeView::OnCreateAssetMaterial()
{
	// TODO: Add your command handler code here
	::OnCreateAssetMaterial();
}


void CAssetTreeView::OnCreateAssetTexture()
{
	// TODO: Add your command handler code here
	::OnCreateAssetTexture();
}


void CAssetTreeView::OnCreateAssetWavefrontOBJ()
{
	// TODO: Add your command handler code here
	::OnCreateAssetWavefrontOBJ();
}

void CAssetTreeView::OnDestroy()
{
	// CTreeView::OnDestroy가 호출되면 트리 컨트롤이 파괴되므로 그 전에 트리 아이템에 부착된 동적할당 Data를 해제한다.
	// Asset Manager에 등록된 핸들들이 이제는 유효하지 않게 되므로 모두 제거
	static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().RemoveAllATVItemHandle();

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

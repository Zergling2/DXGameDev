#include "AssetTreeView.h"
#include "LevelEditor.h"
#include "Settings.h"
#include "ATVItem\ATVItemFolder.h"

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

void CAssetTreeView::DeleteTreeItemDataRecursive(CTreeCtrl& tc, HTREEITEM hItem)
{
	while (hItem)
	{
		// 자식 먼저 처리
		HTREEITEM hChildItem = tc.GetChildItem(hItem);
		if (hChildItem)
			this->DeleteTreeItemDataRecursive(tc, hChildItem);

		DWORD_PTR data = tc.GetItemData(hItem);
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(data);
		assert(pATVItem != nullptr);

		delete pATVItem;

		// 다음 형제 항목
		hItem = tc.GetNextSiblingItem(hItem);
	}
}

// CAssetTreeView message handlers

void CAssetTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl& tc = GetTreeCtrl();

	if (!m_initialized)
	{
		// 1. 스타일 및 배경, 텍스트 색상 설정
		tc.ModifyStyle(0, TVS_EDITLABELS);
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
			ZE_ICON_INDEX::FOLDER_OPENED_ICON,
			ZE_ICON_INDEX::FOLDER_OPENED_ICON,
			0,
			0,
			reinterpret_cast<LPARAM>(pATVItemFolder),
			TVI_ROOT,
			TVI_LAST
		);

		m_initialized = true;
	}
}

void CAssetTreeView::OnDestroy()
{
	// CTreeView::OnDestroy가 호출되면 트리 컨트롤이 파괴되므로 그 전에 트리 아이템에 부착된 동적할당 Data를 해제한다.
	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hRootItem = tc.GetRootItem();
	while (hRootItem)
	{
		this->DeleteTreeItemDataRecursive(tc, hRootItem);

		hRootItem = tc.GetNextItem(hRootItem, TVGN_NEXT);
	}
	// tc.DeleteAllItems();	// 객체 파괴시 자동 수행

	CTreeView::OnDestroy();

	// TODO: Add your message handler code here
	// ...
}

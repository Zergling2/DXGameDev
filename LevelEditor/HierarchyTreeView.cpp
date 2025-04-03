// HierarchyTreeView.cpp : implementation file
//

#include "HierarchyTreeView.h"
#include "Settings.h"

// CHierarchyTreeView

IMPLEMENT_DYNCREATE(CHierarchyTreeView, CTreeView)

CHierarchyTreeView::CHierarchyTreeView()
{

}

CHierarchyTreeView::~CHierarchyTreeView()
{
}

BEGIN_MESSAGE_MAP(CHierarchyTreeView, CTreeView)
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


// CHierarchyTreeView message handlers

void CHierarchyTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl& tree = GetTreeCtrl();
	tree.SetBkColor(HIERARCHY_TREEVIEW_BK_COLOR);
	tree.SetTextColor(HIERARCHY_TREEVIEW_TEXT_COLOR);

	CBitmap iconBitmap;
	iconBitmap.LoadBitmap(IDB_ZEPACKEDICON);

	CImageList iconList;
	iconList.Create(
		ZE_PACKED_ICON_SIZE_X,
		ZE_PACKED_ICON_SIZE_Y,
		ILC_COLOR24 | ILC_MASK,
		ZE_PACKED_ICON_COUNT,
		0
	);
	iconList.Add(&iconBitmap, ZE_PACKED_ICON_COLOR_MASK);
	iconBitmap.Detach();

	tree.SetImageList(&iconList, TVSIL_NORMAL);
	iconList.Detach();

	{
		// 1. 레벨 초기화
		HTREEITEM hRoot = tree.InsertItem(
			_T("New Scene"),
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			TVI_ROOT,
			TVI_LAST
		);

		// 2. 레벨 초기화
		HTREEITEM hKind[5];

		const CString kind[] = { _T("집"), _T("창고"), _T("플레이어"), _T("차량"), _T("계단") };

		for (int i = 0; i < _countof(hKind); ++i)
			hKind[i] = tree.InsertItem(
				kind[i],
				ZE_ICON_INDEX::GAMEOBJECT_ICON,
				ZE_ICON_INDEX::GAMEOBJECT_ICON,
				hRoot,
				TVI_LAST
			);

		// 3. 레벨 초기화
		tree.InsertItem(
			_T("Body"),
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Hand"),
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Shoes"),
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("aaaaa"),
			ZE_ICON_INDEX::PREFAB_ICON,
			ZE_ICON_INDEX::PREFAB_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("bbbbb"),
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("ccccc"),
			ZE_ICON_INDEX::PREFAB_ICON,
			ZE_ICON_INDEX::PREFAB_ICON,
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("ddddd"),
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hKind[2],
			TVI_LAST
		);
	}
}


BOOL CHierarchyTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_TRACKSELECT;


	return CTreeView::PreCreateWindow(cs);
}

// HierarchyTreeView.cpp : implementation file
//

#include "HierarchyTreeView.h"
#include "Constants.h"

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

	CImageList smallImageList;
	smallImageList.Create(IDB_ZEPACKEDICON, 16, GetZergEnginePackedIconCount(), GetZergEnginePackedIconColorMask());

	tree.SetImageList(&smallImageList, TVSIL_NORMAL);
	smallImageList.Detach();

	{
		// 1. ���� �ʱ�ȭ
		HTREEITEM hRoot = tree.InsertItem(
			_T("New Scene"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::ENGINE_LOGO_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::ENGINE_LOGO_ICON),
			TVI_ROOT,
			TVI_LAST
		);

		// 2. ���� �ʱ�ȭ
		HTREEITEM hKind[5];

		const CString kind[] = { _T("��"), _T("â��"), _T("�÷��̾�"), _T("����"), _T("���") };

		for (int i = 0; i < _countof(hKind); ++i)
			hKind[i] = tree.InsertItem(
				kind[i],
				GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
				GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
				hRoot,
				TVI_LAST
			);

		// 3. ���� �ʱ�ȭ
		tree.InsertItem(
			_T("Excel"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Word"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Dream"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("OS"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("DB"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Programming Language"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			hKind[2],
			TVI_LAST
		);

		tree.InsertItem(
			_T("Computer Engineering"),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
			GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::GAMEOBJECT_ICON),
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

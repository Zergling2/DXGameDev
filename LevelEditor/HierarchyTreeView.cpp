// HierarchyTreeView.cpp : implementation file
//

#include "HierarchyTreeView.h"
#include "Settings.h"
#include "TerrainGenerationDialog.h"

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
	ON_NOTIFY_REFLECT(NM_RCLICK, &CHierarchyTreeView::OnNMRClick)
	ON_COMMAND(ID_3DOBJECT_TERRAIN, &CHierarchyTreeView::On3DObjectTerrain)
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

	if (!m_initialized)
	{
		// 1. ��� �� �ؽ�Ʈ ���� ����
		tree.SetBkColor(HIERARCHY_TREEVIEW_BK_COLOR);
		tree.SetTextColor(HIERARCHY_TREEVIEW_TEXT_COLOR);

		// 2. ������ ����Ʈ �ε� �� ����
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

		// Scene ��Ʈ ��� �߰�
		m_hRoot = tree.InsertItem(
			_T("New Scene"),
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			ZE_ICON_INDEX::ENGINE_LOGO_ICON,
			TVI_ROOT,
			TVI_LAST
		);

		m_initialized = true;
	}

	// ���� �ʱ�ȭ
	HTREEITEM hRoot = m_hRoot;
	HTREEITEM hKind[5];

	const CString kind[] = { _T("��"), _T("â��"), _T("�÷��̾�"), _T("����"), _T("���") };

	for (int i = 0; i < _countof(hKind); ++i)
		hKind[i] = tree.InsertItem(
			kind[i],
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			ZE_ICON_INDEX::GAMEOBJECT_ICON,
			hRoot,
			TVI_LAST
		);

	// 3. ���� �ʱ�ȭ
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


BOOL CHierarchyTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_TRACKSELECT;

	return CTreeView::PreCreateWindow(cs);
}

void CHierarchyTreeView::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	CTreeCtrl& ctrl = this->GetTreeCtrl();

	CPoint pt;
	GetCursorPos(&pt);

	CPoint ptClient = pt;
	ctrl.ScreenToClient(&ptClient);

	UINT flags;
	HTREEITEM hItem = ctrl.HitTest(ptClient, &flags);
	if (hItem != nullptr)
	{
		// ��Ŭ�� ������ Ʈ�� �������� �־��ٸ� ����
		ctrl.SelectItem(hItem);
	}
	else
	{
		// �� ������ Ŭ���ߴٸ� �˾� �޴� ���
		CMenu menu;
		if (menu.LoadMenu(IDR_MAINFRAME))
		{
			constexpr int GAMEOBJECT_MENU_INDEX = 3;
			CMenu* pSubMenu = menu.GetSubMenu(GAMEOBJECT_MENU_INDEX);
			if (pSubMenu)
			{
				pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
			}
		}
	}

	*pResult = 0;
}

void CHierarchyTreeView::On3DObjectTerrain()
{
	// TODO: Add your command handler code here
	CTerrainGenerationDialog dlg;
	INT_PTR ret = dlg.DoModal();

	switch (ret)
	{
	case IDOK:
		break;
	case IDCANCEL:
		__fallthrough;
	default:
		break;
	}
}

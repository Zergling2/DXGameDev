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
	// �ڽ� �׸��� ������ �ڽ� �׸��� ���� ���� (���� ��ȸ)
	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hChildItem = tc.GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		HTREEITEM hNextChild = tc.GetNextSiblingItem(hChildItem);		// �̸� ���س��� ��
		RemoveTreeItemPostOrder(hChildItem);	// �ڽ� �׸��� ���� ��ȸ ������� ����
		hChildItem = hNextChild;				// �ڽ� �׸��� ���� ��ȸ
	}

	// �ڽ� �׸� ���� �� �ڽ��� ����
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
		// 1. ��Ÿ�� �� ���, �ؽ�Ʈ ���� ����
		tc.SetBkColor(ASSET_TREEVIEW_BK_COLOR);
		tc.SetTextColor(ASSET_TREEVIEW_TEXT_COLOR);

		// 2. ������ ����Ʈ �ε� �� ����
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		tc.SetImageList(&iconList, TVSIL_NORMAL);

		// ��Ʈ ��� �߰�
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

	// �޴� ���
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
		// ���ڿ� ��ü
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
	// CTreeView::OnDestroy�� ȣ��Ǹ� Ʈ�� ��Ʈ���� �ı��ǹǷ� �� ���� Ʈ�� �����ۿ� ������ �����Ҵ� Data�� �����Ѵ�.
	// Asset Manager�� ��ϵ� �ڵ���� ������ ��ȿ���� �ʰ� �ǹǷ� ��� ����
	static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().RemoveAllATVItemHandle();

	CTreeCtrl& tc = this->GetTreeCtrl();
	HTREEITEM hRootItem = tc.GetRootItem();
	// ��Ʈ �׸���� �����Ͽ� ���� ��ȸ ������� ��� �׸� ����
	while (hRootItem != NULL)
	{
		HTREEITEM hNextRootItem = tc.GetNextSiblingItem(hRootItem);	// RemoveItremPostOrder �Լ��� ���� hRootItem�� �����ǹǷ� �̸� ���س��� ��.
		RemoveTreeItemPostOrder(hRootItem);

		// ���� ���� �׸�����
		hRootItem = hNextRootItem;
	}
	tc.DeleteAllItems();	// ���ʿ�

	CTreeView::OnDestroy();

	// TODO: Add your message handler code here
	// ...
}

#include "AssetTreeView.h"
#include "LevelEditor.h"
#include "Settings.h"

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


// CAssetTreeView message handlers

void CAssetTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CTreeCtrl& tc = GetTreeCtrl();

	if (!m_initialized)
	{
		// 1. ��Ÿ�� �� ���, �ؽ�Ʈ ���� ����
		tc.ModifyStyle(0, TVS_EDITLABELS);
		tc.SetBkColor(ASSET_TREEVIEW_BK_COLOR);
		tc.SetTextColor(ASSET_TREEVIEW_TEXT_COLOR);

		// 2. ������ ����Ʈ �ε� �� ����
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		tc.SetImageList(&iconList, TVSIL_NORMAL);

		// ��Ʈ ��� �߰�
		tc.InsertItem(
			TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE,
			_T("Asset"),
			ZE_ICON_INDEX::FOLDER_OPENED_ICON,
			ZE_ICON_INDEX::FOLDER_OPENED_ICON,
			0,
			0,
			NULL,
			TVI_ROOT,
			TVI_LAST
		);

		m_initialized = true;
	}
}

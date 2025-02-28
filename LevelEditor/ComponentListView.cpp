// ComponentListView.cpp : implementation file
//

#include "ComponentListView.h"
#include "Constants.h"

// CComponentListView

IMPLEMENT_DYNCREATE(CComponentListView, CListView)

CComponentListView::CComponentListView()
{

}

CComponentListView::~CComponentListView()
{
}

BEGIN_MESSAGE_MAP(CComponentListView, CListView)
END_MESSAGE_MAP()


// CComponentListView diagnostics

#ifdef _DEBUG
void CComponentListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CComponentListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CComponentListView message handlers


void CComponentListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl& list = this->GetListCtrl();

	list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL);
	list.SetBkColor(Settings::GetListBkColor());
	list.SetTextColor(RGB(0, 0, 0));
	list.SetTextBkColor(Settings::GetListBkColor());

	CImageList smallImageList;
	smallImageList.Create(IDB_ZEPACKEDICON, 16, GetZergEnginePackedIconCount(), GetZergEnginePackedIconColorMask());

	list.SetImageList(&smallImageList, LVSIL_SMALL);
	smallImageList.Detach();

	CRect cr;
	list.GetClientRect(&cr);

	int ret;

	ret = list.InsertColumn(0, _T("Component List"), LVCFMT_LEFT, cr.Width());

	ret = list.InsertItem(0, _T("Transform"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	ret = list.InsertItem(1, _T("MeshFilter"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::MESH_FILTER_ICON));
	ret = list.InsertItem(2, _T("MeshRenderer"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::MESH_RENDERER_ICON));
	ret = list.InsertItem(3, _T("Rigidbody"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::WARNING_ICON));
	ret = list.InsertItem(4, _T("Script"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::ERROR_ICON));
}

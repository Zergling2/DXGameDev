// ComponentListView.cpp : implementation file
//

#include "ComponentListView.h"
#include "Settings.h"

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
	CListCtrl& list = this->GetListCtrl();

	list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL);
	list.SetBkColor(COMPONENT_LISTVIEW_BK_COLOR);
	list.SetTextColor(COMPONENT_LISTVIEW_TEXT_COLOR);
	list.SetTextBkColor(COMPONENT_LISTVIEW_BK_COLOR);
	
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

	list.SetImageList(&iconList, LVSIL_SMALL);
	iconList.Detach();

	CRect cr;
	list.GetClientRect(&cr);

	int ret;

	ret = list.InsertColumn(0, _T("Component List"), LVCFMT_LEFT, cr.Width() - 4);

	ret = list.InsertItem(0, _T("Transform"), ZE_ICON_INDEX::TRANSFORM_ICON);
	ret = list.InsertItem(1, _T("MeshFilter"), ZE_ICON_INDEX::MESH_FILTER_ICON);
	ret = list.InsertItem(2, _T("MeshRenderer"), ZE_ICON_INDEX::MESH_RENDERER_ICON);
	ret = list.InsertItem(3, _T("Audio Source"), ZE_ICON_INDEX::AUDIO_SOURCE_ICON);
	ret = list.InsertItem(4, _T("Script"), ZE_ICON_INDEX::SCRIPT_ICON);
	ret = list.InsertItem(5, _T("Rigidbody"), ZE_ICON_INDEX::RIGIDBODY_ICON);
	ret = list.InsertItem(6, _T("Camera"), ZE_ICON_INDEX::CAMERA_ICON);
}

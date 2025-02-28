// LogListView.cpp : implementation file
//

#include "LogListView.h"
#include "Constants.h"

// CLogListView

IMPLEMENT_DYNCREATE(CLogListView, CListView)

CLogListView::CLogListView()
{

}

CLogListView::~CLogListView()
{
}

BEGIN_MESSAGE_MAP(CLogListView, CListView)
END_MESSAGE_MAP()


// CLogListView diagnostics

#ifdef _DEBUG
void CLogListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLogListView message handlers


void CLogListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CListCtrl& list = this->GetListCtrl();

	list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	list.SetBkColor(Settings::GetListBkColor());
	list.SetTextColor(RGB(0, 0, 0));
	list.SetTextBkColor(Settings::GetListBkColor());

	CImageList smallImageList;
	smallImageList.Create(IDB_ZEPACKEDICON, 16, GetZergEnginePackedIconCount(), GetZergEnginePackedIconColorMask());

	list.SetImageList(&smallImageList, LVSIL_SMALL);
	smallImageList.Detach();

	CRect cr;
	list.GetClientRect(&cr);

	constexpr float LOG_COLUMN_DESCRIPTION_WIDTH_RATIO = 0.8f;
	const int descriptionWidth = static_cast<int>(static_cast<float>(cr.Width()) * LOG_COLUMN_DESCRIPTION_WIDTH_RATIO);
	const int timeWidth = cr.Width() - descriptionWidth;

	int ret;

	ret = list.InsertColumn(0, _T("Description"), LVCFMT_LEFT, descriptionWidth);
	ret = list.InsertColumn(1, _T("Time"), LVCFMT_LEFT, timeWidth);

	list.InsertItem(0, _T("This is a test log 0"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	list.InsertItem(1, _T("This is a test log 1"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	list.InsertItem(2, _T("This is a test log 2"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::WARNING_ICON));
	list.InsertItem(3, _T("This is a test log 3"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	list.InsertItem(4, _T("This is a test log 4"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::ERROR_ICON));
	list.InsertItem(5, _T("This is a test log 5"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::ERROR_ICON));
	list.InsertItem(6, _T("This is a test log 6"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	list.InsertItem(7, _T("This is a test log 7"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
	list.InsertItem(8, _T("This is a test log 8"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::WARNING_ICON));
	list.InsertItem(9, _T("This is a test log 9"), GetZergEngineIconIndex(ZERGENGINE_ICON_INDEX::INFO_ICON));
}


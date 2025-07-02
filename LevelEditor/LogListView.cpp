// LogListView.cpp : implementation file
//

#include "LogListView.h"
#include "Settings.h"

// CLogListView

IMPLEMENT_DYNCREATE(CLogListView, CListView)

CLogListView::CLogListView()
	: m_initialized(false)
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

	if (!m_initialized)
	{
		// 1. 리스트 속성 설정
		list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
		list.SetBkColor(LOG_LISTVIEW_BK_COLOR);
		list.SetTextColor(LOG_LISTVIEW_TEXT_COLOR);
		list.SetTextBkColor(LOG_LISTVIEW_BK_COLOR);

		// 2. 아이콘 리스트 로드 및 설정
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

		// 3. "Description", "Time" 열 추가
		CRect cr;
		list.GetClientRect(&cr);

		constexpr float LOG_COLUMN_DESCRIPTION_WIDTH_RATIO = 0.8f;
		const int descriptionWidth = static_cast<int>(static_cast<float>(cr.Width()) * LOG_COLUMN_DESCRIPTION_WIDTH_RATIO);
		const int timeWidth = cr.Width() - descriptionWidth;

		int ret;

		ret = list.InsertColumn(0, _T("Description"), LVCFMT_LEFT, descriptionWidth);
		ret = list.InsertColumn(1, _T("Time"), LVCFMT_LEFT, timeWidth);

		m_initialized = true;
	}

	list.InsertItem(0, _T("This is a test log 0"), ZE_ICON_INDEX::INFO_ICON);
	list.InsertItem(1, _T("This is a test log 1"), ZE_ICON_INDEX::INFO_ICON);
	list.InsertItem(2, _T("This is a test log 2"), ZE_ICON_INDEX::WARNING_ICON);
	list.InsertItem(3, _T("This is a test log 3"), ZE_ICON_INDEX::INFO_ICON);
	list.InsertItem(4, _T("This is a test log 4"), ZE_ICON_INDEX::ERROR_ICON);
	list.InsertItem(5, _T("This is a test log 5"), ZE_ICON_INDEX::ERROR_ICON);
	list.InsertItem(6, _T("This is a test log 6"), ZE_ICON_INDEX::INFO_ICON);
	list.InsertItem(7, _T("This is a test log 7"), ZE_ICON_INDEX::INFO_ICON);
	list.InsertItem(8, _T("This is a test log 8"), ZE_ICON_INDEX::WARNING_ICON);
	list.InsertItem(9, _T("This is a test log 9"), ZE_ICON_INDEX::INFO_ICON);
}

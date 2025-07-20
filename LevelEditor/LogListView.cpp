#include "LogListView.h"
#include "LevelEditor.h"
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


void CLogListView::AddLog(LPTSTR msg, LOG_TYPE type)
{
	CTime time = CTime::GetCurrentTime();
	TCHAR timeString[32];

	StringCbPrintf(timeString, sizeof(timeString), _T("%02d:%02d:%02d"), time.GetHour(), time.GetMinute(), time.GetSecond());

	CListCtrl& lc = this->GetListCtrl();
	int imageIndex;
	switch (type)
	{
	case LOG_TYPE::LT_INFO:
		imageIndex = ZE_ICON_INDEX::INFO_ICON;
		break;
	case LOG_TYPE::LT_WARNING:
		imageIndex = ZE_ICON_INDEX::WARNING_ICON;
		break;
	case LOG_TYPE::LT_ERROR:
		imageIndex = ZE_ICON_INDEX::ERROR_ICON;
		break;
	default:
		imageIndex = 0;
		break;
	}

	int newItemIndex = lc.GetItemCount();

	// 로그 아이템 삽입
	lc.InsertItem(newItemIndex, msg, imageIndex);
	constexpr int LOG_TIME_COLUMN_INDEX = 1;

	// 로그 시간 추가
	lc.SetItemText(newItemIndex, LOG_TIME_COLUMN_INDEX, timeString);
}

void CLogListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CListCtrl& lc = this->GetListCtrl();

	if (!m_initialized)
	{
		// 1. 리스트 속성 설정
		lc.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
		lc.SetBkColor(LOG_LISTVIEW_BK_COLOR);
		lc.SetTextColor(LOG_LISTVIEW_TEXT_COLOR);
		lc.SetTextBkColor(LOG_LISTVIEW_BK_COLOR);

		// 2. 아이콘 리스트 로드 및 설정
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		lc.SetImageList(&iconList, LVSIL_SMALL);

		// 3. "Description", "Time" 열 추가
		CRect cr;
		lc.GetClientRect(&cr);

		constexpr float LOG_COLUMN_DESCRIPTION_WIDTH_RATIO = 0.8f;
		const int descriptionWidth = static_cast<int>(static_cast<float>(cr.Width()) * LOG_COLUMN_DESCRIPTION_WIDTH_RATIO);
		const int timeWidth = cr.Width() - descriptionWidth;

		int ret;

		ret = lc.InsertColumn(0, _T("Description"), LVCFMT_LEFT, descriptionWidth);
		ret = lc.InsertColumn(1, _T("Time"), LVCFMT_LEFT, timeWidth);

		m_initialized = true;
	}

	this->AddLog(_T("TEST LOG┼"), LOG_TYPE::LT_WARNING);
	this->AddLog(_T("테스트 Log★"), LOG_TYPE::LT_INFO);
	this->AddLog(_T("TEST 로그§"), LOG_TYPE::LT_ERROR);
}

#include "ComponentListView.h"
#include "LevelEditor.h"
#include "Settings.h"
#include "CLVItem\CLVItemEmpty.h"

// CComponentListView

IMPLEMENT_DYNCREATE(CComponentListView, CListView)

CComponentListView::CComponentListView()
	: m_initialized(false)
{
}

CComponentListView::~CComponentListView()
{
}

BEGIN_MESSAGE_MAP(CComponentListView, CListView)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CLICK, &CComponentListView::OnNMClick)
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

void CComponentListView::ClearListCtrl()
{
	CListCtrl& lc = this->GetListCtrl();

	// ����Ʈ ��ȸ�ϸ� �����Ҵ� �����͵���� ����
	LVITEM item;
	ZeroMemory(&item, sizeof(item));
	item.iItem = 0;
	item.iSubItem = 0;
	item.mask = LVIF_PARAM;

	while (lc.GetItemCount() > 0)
	{
		lc.GetItem(&item);
		
		ICLVItem* pCLVItem = reinterpret_cast<ICLVItem*>(item.lParam);
		delete pCLVItem;

		lc.DeleteItem(0);
	}
}

void CComponentListView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CListCtrl& lc = this->GetListCtrl();

	if (!m_initialized)
	{
		// 1. ����Ʈ ��Ʈ�� �Ӽ� ����
		lc.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL);
		lc.SetBkColor(COMPONENT_LISTVIEW_BK_COLOR);
		lc.SetTextColor(COMPONENT_LISTVIEW_TEXT_COLOR);
		lc.SetTextBkColor(COMPONENT_LISTVIEW_BK_COLOR);

		// 2. ������ ����Ʈ �ε� �� ����
		CLevelEditorApp* pApp = static_cast<CLevelEditorApp*>(AfxGetApp());
		CImageList& iconList = pApp->GetZEIconList();
		lc.SetImageList(&iconList, LVSIL_SMALL);

		// 3. "Component List"�� �߰�
		CRect cr;
		lc.GetClientRect(&cr);
		lc.InsertColumn(0, _T("Component List"), LVCFMT_LEFT, cr.Width());

		m_initialized = true;
	}
}

void CComponentListView::OnDestroy()
{
	// CListView::OnDestroy�� ȣ��Ǹ� ����Ʈ ��Ʈ���� �ı��ǹǷ� �� ���� �����ۿ� ������ �����Ҵ� Data�� �����Ѵ�.
	this->ClearListCtrl();

	CListView::OnDestroy();

	// TODO: Add your message handler code here
}

void CComponentListView::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CListCtrl& lc = this->GetListCtrl();

	CPoint pt;
	GetCursorPos(&pt);

	CPoint clientPt = pt;
	lc.ScreenToClient(&clientPt);

	UINT flags;
	int index = lc.HitTest(clientPt, &flags);
	if (index == -1)
	{
		CLVItemEmpty item;
		item.OnSelect();
	}
	else
	{
		ICLVItem* pCLVItem = reinterpret_cast<ICLVItem*>(lc.GetItemData(index));
		pCLVItem->OnSelect();
	}

	*pResult = 0;
}

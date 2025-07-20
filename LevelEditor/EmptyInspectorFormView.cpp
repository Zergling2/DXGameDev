#include "LevelEditor.h"
#include "EmptyInspectorFormView.h"


// CEmptyInspectorFormView

IMPLEMENT_DYNCREATE(CEmptyInspectorFormView, CFormView)

CEmptyInspectorFormView::CEmptyInspectorFormView()
	: CFormView(IDD_EMPTY_INSPECTOR_FORMVIEW)
{

}

CEmptyInspectorFormView::~CEmptyInspectorFormView()
{
}

void CEmptyInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEmptyInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CEmptyInspectorFormView diagnostics

#ifdef _DEBUG
void CEmptyInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEmptyInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEmptyInspectorFormView message handlers

#include "LevelEditor.h"
#include "MaterialInspectorFormView.h"

// CMaterialInspectorFormView

IMPLEMENT_DYNCREATE(CMaterialInspectorFormView, CFormView)

CMaterialInspectorFormView::CMaterialInspectorFormView()
	: CFormView(IDD_MATERIAL_INSPECTOR_FORMVIEW)
{
}

CMaterialInspectorFormView::~CMaterialInspectorFormView()
{
}

void CMaterialInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMaterialInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CMaterialInspectorFormView diagnostics

#ifdef _DEBUG
void CMaterialInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMaterialInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMaterialInspectorFormView message handlers

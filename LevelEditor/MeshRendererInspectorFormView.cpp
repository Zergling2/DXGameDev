// C:\Users\admin\source\repos\DXGameDev\LevelEditor\MeshRendererInspectorFormView.cpp : implementation file
//

#include "LevelEditor.h"
#include "MeshRendererInspectorFormView.h"


// CMeshRendererInspectorFormView

IMPLEMENT_DYNCREATE(CMeshRendererInspectorFormView, CFormView)

CMeshRendererInspectorFormView::CMeshRendererInspectorFormView()
	: CFormView(IDD_MESH_RENDERER_INSPECTOR_FORMVIEW)
{

}

CMeshRendererInspectorFormView::~CMeshRendererInspectorFormView()
{
}

void CMeshRendererInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMeshRendererInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CMeshRendererInspectorFormView diagnostics

#ifdef _DEBUG
void CMeshRendererInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMeshRendererInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMeshRendererInspectorFormView message handlers

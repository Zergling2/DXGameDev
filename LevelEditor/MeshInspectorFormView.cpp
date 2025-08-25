#include "LevelEditor.h"
#include "MeshInspectorFormView.h"


// CMeshInspectorFormView

IMPLEMENT_DYNCREATE(CMeshInspectorFormView, CFormView)

CMeshInspectorFormView::CMeshInspectorFormView()
	: CFormView(IDD_MESH_INSPECTOR_FORMVIEW)
	, m_staticMeshSubsetCount(0)
	, m_staticMeshName(_T(""))
	, m_staticMeshVertexCount(0)
	, m_staticMeshVertexFormatType(_T(""))
{
}

CMeshInspectorFormView::~CMeshInspectorFormView()
{
}

void CMeshInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MESH_SUBSET_COUNT, m_staticMeshSubsetCount);
	DDX_Text(pDX, IDC_STATIC_MESH_NAME, m_staticMeshName);
	DDX_Text(pDX, IDC_STATIC_MESH_VERTEX_COUNT, m_staticMeshVertexCount);
	DDX_Text(pDX, IDC_STATIC_MESH_VERTEX_FORMAT_TYPE, m_staticMeshVertexFormatType);
}

BEGIN_MESSAGE_MAP(CMeshInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CMeshInspectorFormView diagnostics

#ifdef _DEBUG
void CMeshInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMeshInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMeshInspectorFormView message handlers

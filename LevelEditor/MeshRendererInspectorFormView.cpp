// C:\Users\admin\source\repos\DXGameDev\LevelEditor\MeshRendererInspectorFormView.cpp : implementation file
//

#include "LevelEditor.h"
#include "CLVItem\CLVItemMeshRenderer.h"
#include "MeshRendererInspectorFormView.h"

// CMeshRendererInspectorFormView

IMPLEMENT_DYNCREATE(CMeshRendererInspectorFormView, CFormView)

CMeshRendererInspectorFormView::CMeshRendererInspectorFormView()
	: CFormView(IDD_MESH_RENDERER_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{

}

CMeshRendererInspectorFormView::~CMeshRendererInspectorFormView()
{
}

void CMeshRendererInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESH_RENDERER_CAST_SHADOWS, m_castShadows);
	DDX_Control(pDX, IDC_MESH_RENDERER_RECEIVE_SHADOWS, m_receiveShadows);
}

BEGIN_MESSAGE_MAP(CMeshRendererInspectorFormView, CFormView)
	ON_BN_CLICKED(IDC_MESH_RENDERER_CAST_SHADOWS, &CMeshRendererInspectorFormView::OnBnClickedMeshRendererCastShadows)
	ON_BN_CLICKED(IDC_MESH_RENDERER_RECEIVE_SHADOWS, &CMeshRendererInspectorFormView::OnBnClickedMeshRendererReceiveShadows)
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

void CMeshRendererInspectorFormView::OnBnClickedMeshRendererCastShadows()
{
	// TODO: Add your control notification handler code here
	m_castShadows.UpdateData(TRUE);

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	pMeshRenderer->SetCastShadows(m_castShadows.GetCheck());
}

void CMeshRendererInspectorFormView::OnBnClickedMeshRendererReceiveShadows()
{
	// TODO: Add your control notification handler code here
	m_receiveShadows.UpdateData(TRUE);

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	pMeshRenderer->SetReceiveShadows(m_receiveShadows.GetCheck());
}

#include "CLVItemMeshRenderer.h"
#include "..\MainFrm.h"
#include "..\MeshRendererInspectorFormView.h"

void CLVItemMeshRenderer::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. �ν����� �� �� ��ü
	CMeshRendererInspectorFormView* pInspector =
		static_cast<CMeshRendererInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CMeshRendererInspectorFormView)));

	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� �ν����� �� �信 �ڽ��� ���� ����
	pInspector->m_castShadows.SetCheck(m_pMeshRenderer->GetCastShadows());
	pInspector->m_receiveShadows.SetCheck(m_pMeshRenderer->GetReceiveShadows());

	// ���ε��Ǿ� �ִ� ���ҽ����� �ν����Ϳ� ǥ���ؾ� �� (Not implemented)
	// pInspector->m_comboSelectMesh = ;
	// pInspector->m_comboSelectSubsetIndex = ;
	// pInspector->m_comboSelectMaterial = ;

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

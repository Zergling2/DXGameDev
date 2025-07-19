#include "CLVItemMeshRenderer.h"
#include "..\MainFrm.h"
#include "..\MeshRendererInspectorFormView.h"

void CLVItemMeshRenderer::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. ������Ʈ �ν����� �� �並 Transform �ν����ͷ� ��ü
	CMeshRendererInspectorFormView* pInspector =
		static_cast<CMeshRendererInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CMeshRendererInspectorFormView)));




	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� ������Ʈ �ν����� �� �信 �ڽ��� ���� ����
	pInspector->m_castShadows.SetCheck(m_pMeshRenderer->GetCastShadows());
	pInspector->m_receiveShadows.SetCheck(m_pMeshRenderer->GetReceiveShadows());

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

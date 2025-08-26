#include "CLVItemMeshRenderer.h"
#include "..\MainFrm.h"
#include "..\MeshRendererInspectorFormView.h"

void CLVItemMeshRenderer::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 인스펙터 폼 뷰 교체
	CMeshRendererInspectorFormView* pInspector =
		static_cast<CMeshRendererInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CMeshRendererInspectorFormView)));

	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 인스펙터 폼 뷰에 자신의 정보 세팅
	pInspector->m_castShadows.SetCheck(m_pMeshRenderer->GetCastShadows());
	pInspector->m_receiveShadows.SetCheck(m_pMeshRenderer->GetReceiveShadows());

	// 바인딩되어 있던 리소스들을 인스펙터에 표시해야 함 (Not implemented)
	// pInspector->m_comboSelectMesh = ;
	// pInspector->m_comboSelectSubsetIndex = ;
	// pInspector->m_comboSelectMaterial = ;

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

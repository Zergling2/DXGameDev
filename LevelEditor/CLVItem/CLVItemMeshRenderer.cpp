#include "CLVItemMeshRenderer.h"
#include "..\MainFrm.h"
#include "..\MeshRendererInspectorFormView.h"

void CLVItemMeshRenderer::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 컴포넌트 인스펙터 폼 뷰를 Transform 인스펙터로 교체
	CMeshRendererInspectorFormView* pInspector =
		static_cast<CMeshRendererInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CMeshRendererInspectorFormView)));




	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 컴포넌트 인스펙터 폼 뷰에 자신의 정보 세팅
	pInspector->m_castShadows.SetCheck(m_pMeshRenderer->GetCastShadows());
	pInspector->m_receiveShadows.SetCheck(m_pMeshRenderer->GetReceiveShadows());

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

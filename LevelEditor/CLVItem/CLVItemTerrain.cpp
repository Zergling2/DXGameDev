#include "CLVItemTerrain.h"
#include "..\MainFrm.h"
#include "..\TerrainInspectorFormView.h"

void CLVItemTerrain::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 컴포넌트 인스펙터 폼 뷰 교체
	CTerrainInspectorFormView* pInspector =
		static_cast<CTerrainInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CTerrainInspectorFormView)));



	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 컴포넌트 인스펙터 폼 뷰에 자신의 정보 세팅

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

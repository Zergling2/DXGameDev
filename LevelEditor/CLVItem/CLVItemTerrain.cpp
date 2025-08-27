#include "CLVItemTerrain.h"
#include "..\MainFrm.h"
#include "..\TerrainInspectorFormView.h"

void CLVItemTerrain::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 인스펙터 폼 뷰 교체
	CTerrainInspectorFormView* pInspector =
		static_cast<CTerrainInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CTerrainInspectorFormView)));

	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 인스펙터 폼 뷰에 Terrain 정보 보여주기
	ze::Terrain* pTerrain = this->GetTerrain();
	D3D11_TEXTURE2D_DESC heightMapDesc;
	ID3D11Texture2D* pHeightMap = pTerrain->GetHeightMap().GetTex2DComInterface();
	if (pHeightMap)
	{
		pHeightMap->GetDesc(&heightMapDesc);
		TCHAR str[100];
		StringCbPrintf(
			str, sizeof(str),
			_T("Height Map: %u x %u \nCell Size: %f \nHeight Scale: %f"),
			heightMapDesc.Width, heightMapDesc.Height,
			pTerrain->GetCellSize(),
			pTerrain->GetHeightScale()
		);
		pInspector->m_staticTerrainInfo.SetWindowText(str);
	}

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

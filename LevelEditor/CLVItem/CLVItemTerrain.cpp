#include "CLVItemTerrain.h"
#include "..\MainFrm.h"
#include "..\TerrainInspectorFormView.h"

void CLVItemTerrain::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. �ν����� �� �� ��ü
	CTerrainInspectorFormView* pInspector =
		static_cast<CTerrainInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CTerrainInspectorFormView)));

	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� �ν����� �� �信 Terrain ���� �����ֱ�
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

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

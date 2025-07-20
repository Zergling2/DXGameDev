#include "CLVItemTerrain.h"
#include "..\MainFrm.h"
#include "..\TerrainInspectorFormView.h"

void CLVItemTerrain::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. ������Ʈ �ν����� �� �� ��ü
	CTerrainInspectorFormView* pInspector =
		static_cast<CTerrainInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CTerrainInspectorFormView)));



	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� ������Ʈ �ν����� �� �信 �ڽ��� ���� ����

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

#include "CLVItemSpotLight.h"
#include "..\MainFrm.h"
#include "..\SpotLightInspectorFormView.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>

void CLVItemSpotLight::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 인스펙터 폼 뷰 교체
	CSpotLightInspectorFormView* pInspector =
		static_cast<CSpotLightInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CSpotLightInspectorFormView)));


	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 인스펙터 폼 뷰에 정보 세팅
	ze::SpotLight* pSpotLight = this->GetSpotLight();
	TCHAR buf[32];

	// Ambient
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_ambient.x);
	pInspector->m_editAmbientR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_ambient.y);
	pInspector->m_editAmbientG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_ambient.z);
	pInspector->m_editAmbientB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_ambient.w);
	pInspector->m_editAmbientA.SetWindowText(buf);

	// Diffuse
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_diffuse.x);
	pInspector->m_editDiffuseR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_diffuse.y);
	pInspector->m_editDiffuseG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_diffuse.z);
	pInspector->m_editDiffuseB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_diffuse.w);
	pInspector->m_editDiffuseA.SetWindowText(buf);

	// Specular
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_specular.x);
	pInspector->m_editSpecularR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_specular.y);
	pInspector->m_editSpecularG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_specular.z);
	pInspector->m_editSpecularB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_specular.w);
	pInspector->m_editSpecularExp.SetWindowText(buf);

	// Spot Exp
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_spotExp);
	pInspector->m_editSpotExp.SetWindowText(buf);

	// Range
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_range);
	pInspector->m_editRange.SetWindowText(buf);

	// Att
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_att.x);
	pInspector->m_editAttX.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_att.y);
	pInspector->m_editAttY.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->m_att.z);
	pInspector->m_editAttZ.SetWindowText(buf);

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

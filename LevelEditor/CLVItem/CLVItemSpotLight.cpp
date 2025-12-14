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

	// Inner Cone Angle
	StringCbPrintf(buf, sizeof(buf), _T("%f"), XMConvertToDegrees(pSpotLight->GetInnerConeAngle()));	// 에디터에서는 편의상 육십분법 지원
	pInspector->m_editInnerConeAngle.SetWindowText(buf);

	// Outer Cone Angle
	StringCbPrintf(buf, sizeof(buf), _T("%f"), XMConvertToDegrees(pSpotLight->GetOuterConeAngle()));	// 에디터에서는 편의상 육십분법 지원
	pInspector->m_editOuterConeAngle.SetWindowText(buf);

	// Range
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pSpotLight->GetRange());
	pInspector->m_editRange.SetWindowText(buf);

	// Att
	const XMFLOAT3 att = pSpotLight->GetDistAtt();
	StringCbPrintf(buf, sizeof(buf), _T("%f"), att.x);
	pInspector->m_editAttX.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), att.y);
	pInspector->m_editAttY.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), att.z);
	pInspector->m_editAttZ.SetWindowText(buf);

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}

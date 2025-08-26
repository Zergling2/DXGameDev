#include "CLVItemDirectionalLight.h"
#include "..\MainFrm.h"
#include "..\DirectionalLightInspectorFormView.h"

void CLVItemDirectionalLight::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. �ν����� �� �� ��ü
	CDirectionalLightInspectorFormView* pInspector =
		static_cast<CDirectionalLightInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CDirectionalLightInspectorFormView)));


	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� �ν����� �� �信 ���� ����
	ze::DirectionalLight* pDirectionalLight = this->GetDirectionalLight();
	TCHAR buf[32];

	// Ambient
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_ambient.x);
	pInspector->m_editAmbientR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_ambient.y);
	pInspector->m_editAmbientG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_ambient.z);
	pInspector->m_editAmbientB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_ambient.w);
	pInspector->m_editAmbientA.SetWindowText(buf);

	// Diffuse
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_diffuse.x);
	pInspector->m_editDiffuseR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_diffuse.y);
	pInspector->m_editDiffuseG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_diffuse.z);
	pInspector->m_editDiffuseB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_diffuse.w);
	pInspector->m_editDiffuseA.SetWindowText(buf);

	// Specular
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_specular.x);
	pInspector->m_editSpecularR.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_specular.y);
	pInspector->m_editSpecularG.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_specular.z);
	pInspector->m_editSpecularB.SetWindowText(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), pDirectionalLight->m_specular.w);
	pInspector->m_editSpecularExp.SetWindowText(buf);

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

#include "ATVItemMaterial.h"
#include "..\MainFrm.h"
#include "..\MaterialInspectorFormView.h"
#include <ZergEngine\CoreSystem\Resource\Material.h>

void ATVItemMaterial::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. �ν����� �� �� ��ü
	CMaterialInspectorFormView* pInspector =
		static_cast<CMaterialInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CMaterialInspectorFormView)));


	pInspector->SetATVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� �ν����� �� �信 Material ���� �����ֱ�
	TCHAR str[32];

	// Ambient
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_ambient.x);
	pInspector->m_editAmbientR.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_ambient.y);
	pInspector->m_editAmbientG.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_ambient.z);
	pInspector->m_editAmbientB.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_ambient.w);
	pInspector->m_editAmbientA.SetWindowText(str);

	// Diffuse
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_diffuse.x);
	pInspector->m_editDiffuseR.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_diffuse.y);
	pInspector->m_editDiffuseG.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_diffuse.z);
	pInspector->m_editDiffuseB.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_diffuse.w);
	pInspector->m_editDiffuseA.SetWindowText(str);

	// Specular
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_specular.x);
	pInspector->m_editSpecularR.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_specular.y);
	pInspector->m_editSpecularG.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_specular.z);
	pInspector->m_editSpecularB.SetWindowText(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), m_spMaterial->m_specular.w);
	pInspector->m_editSpecularExp.SetWindowText(str);


	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}

#include "LevelEditor.h"
#include "SpotLightInspectorFormView.h"
#include "CLVItem\CLVItemSpotLight.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>

// CSpotLightInspectorFormView

IMPLEMENT_DYNCREATE(CSpotLightInspectorFormView, CFormView)

CSpotLightInspectorFormView::CSpotLightInspectorFormView()
	: CFormView(IDD_SPOT_LIGHT_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{
}

CSpotLightInspectorFormView::~CSpotLightInspectorFormView()
{
}

void CSpotLightInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_R, m_editAmbientR);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_G, m_editAmbientG);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_B, m_editAmbientB);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_A, m_editAmbientA);
	DDX_Control(pDX, IDC_EDIT_DIFFUSE_R, m_editDiffuseR);
	DDX_Control(pDX, IDC_EDIT_DIFFUSE_G, m_editDiffuseG);
	DDX_Control(pDX, IDC_EDIT_DIFFUSE_B, m_editDiffuseB);
	DDX_Control(pDX, IDC_EDIT_DIFFUSE_A, m_editDiffuseA);
	DDX_Control(pDX, IDC_EDIT_SPECULAR_R, m_editSpecularR);
	DDX_Control(pDX, IDC_EDIT_SPECULAR_G, m_editSpecularG);
	DDX_Control(pDX, IDC_EDIT_SPECULAR_B, m_editSpecularB);
	DDX_Control(pDX, IDC_EDIT_SPECULAR_EXPONENT, m_editSpecularExp);
	DDX_Control(pDX, IDC_EDIT_SPOT_LIGHT_SPOT_EXP, m_editSpotExp);
	DDX_Control(pDX, IDC_EDIT_SPOT_LIGHT_RANGE, m_editRange);
	DDX_Control(pDX, IDC_EDIT_SPOT_LIGHT_ATT_X, m_editAttX);
	DDX_Control(pDX, IDC_EDIT_SPOT_LIGHT_ATT_Y, m_editAttY);
	DDX_Control(pDX, IDC_EDIT_SPOT_LIGHT_ATT_Z, m_editAttZ);
}

BEGIN_MESSAGE_MAP(CSpotLightInspectorFormView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_R, &CSpotLightInspectorFormView::OnEnChangeEditAmbientR)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_G, &CSpotLightInspectorFormView::OnEnChangeEditAmbientG)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_B, &CSpotLightInspectorFormView::OnEnChangeEditAmbientB)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_A, &CSpotLightInspectorFormView::OnEnChangeEditAmbientA)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_R, &CSpotLightInspectorFormView::OnEnChangeEditDiffuseR)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_G, &CSpotLightInspectorFormView::OnEnChangeEditDiffuseG)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_B, &CSpotLightInspectorFormView::OnEnChangeEditDiffuseB)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_A, &CSpotLightInspectorFormView::OnEnChangeEditDiffuseA)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_R, &CSpotLightInspectorFormView::OnEnChangeEditSpecularR)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_G, &CSpotLightInspectorFormView::OnEnChangeEditSpecularG)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_B, &CSpotLightInspectorFormView::OnEnChangeEditSpecularB)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_EXPONENT, &CSpotLightInspectorFormView::OnEnChangeEditSpecularExponent)
	ON_EN_CHANGE(IDC_EDIT_SPOT_LIGHT_SPOT_EXP, &CSpotLightInspectorFormView::OnEnChangeEditSpotLightSpotExp)
	ON_EN_CHANGE(IDC_EDIT_SPOT_LIGHT_RANGE, &CSpotLightInspectorFormView::OnEnChangeEditSpotLightRange)
	ON_EN_CHANGE(IDC_EDIT_SPOT_LIGHT_ATT_X, &CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttX)
	ON_EN_CHANGE(IDC_EDIT_SPOT_LIGHT_ATT_Y, &CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttY)
	ON_EN_CHANGE(IDC_EDIT_SPOT_LIGHT_ATT_Z, &CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttZ)
END_MESSAGE_MAP()


// CSpotLightInspectorFormView diagnostics

#ifdef _DEBUG
void CSpotLightInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSpotLightInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSpotLightInspectorFormView message handlers

void CSpotLightInspectorFormView::OnEnChangeEditAmbientR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_ambient.x = m_editAmbientR.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditAmbientG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_ambient.y = m_editAmbientG.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditAmbientB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_ambient.z = m_editAmbientB.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditAmbientA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_ambient.w = m_editAmbientA.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditDiffuseR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_diffuse.x = m_editDiffuseR.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditDiffuseG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_diffuse.y = m_editDiffuseG.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditDiffuseB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_diffuse.z = m_editDiffuseB.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditDiffuseA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_diffuse.w = m_editDiffuseA.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpecularR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_specular.x = m_editSpecularR.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpecularG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_specular.y = m_editSpecularG.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpecularB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_specular.z = m_editSpecularB.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpecularExponent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->m_specular.w = m_editSpecularExp.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpotLightSpotExp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	// pSpotLight->m_spotExp = m_editSpotExp.GetValue();
}

void CSpotLightInspectorFormView::OnEnChangeEditSpotLightRange()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->SetRange(m_editRange.GetValue());
}

void CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->SetDistAttConstant(m_editAttX.GetValue());
}

void CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->SetDistAttLinear(m_editAttY.GetValue());
}

void CSpotLightInspectorFormView::OnEnChangeEditSpotLightAttZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::SpotLight* pSpotLight = this->GetCLVItemToModify()->GetSpotLight();
	pSpotLight->SetDistAttSquare(m_editAttZ.GetValue());
}

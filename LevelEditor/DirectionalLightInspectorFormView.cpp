#include "LevelEditor.h"
#include "DirectionalLightInspectorFormView.h"
#include "CLVItem\CLVItemDirectionalLight.h"

// CDirectionalLightInspectorFormView

IMPLEMENT_DYNCREATE(CDirectionalLightInspectorFormView, CFormView)

CDirectionalLightInspectorFormView::CDirectionalLightInspectorFormView()
	: CFormView(IDD_DIRECTIONAL_LIGHT_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{

}

CDirectionalLightInspectorFormView::~CDirectionalLightInspectorFormView()
{
}

void CDirectionalLightInspectorFormView::DoDataExchange(CDataExchange* pDX)
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
}

BEGIN_MESSAGE_MAP(CDirectionalLightInspectorFormView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_R, &CDirectionalLightInspectorFormView::OnEnChangeEditAmbientR)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_G, &CDirectionalLightInspectorFormView::OnEnChangeEditAmbientG)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_B, &CDirectionalLightInspectorFormView::OnEnChangeEditAmbientB)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_A, &CDirectionalLightInspectorFormView::OnEnChangeEditAmbientA)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_R, &CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseR)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_G, &CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseG)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_B, &CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseB)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_A, &CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseA)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_R, &CDirectionalLightInspectorFormView::OnEnChangeEditSpecularR)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_G, &CDirectionalLightInspectorFormView::OnEnChangeEditSpecularG)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_B, &CDirectionalLightInspectorFormView::OnEnChangeEditSpecularB)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_EXPONENT, &CDirectionalLightInspectorFormView::OnEnChangeEditSpecularExponent)
END_MESSAGE_MAP()


// CDirectionalLightInspectorFormView diagnostics

#ifdef _DEBUG
void CDirectionalLightInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDirectionalLightInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDirectionalLightInspectorFormView message handlers

void CDirectionalLightInspectorFormView::OnEnChangeEditAmbientR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_ambient.x = m_editAmbientR.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditAmbientG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_ambient.y = m_editAmbientG.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditAmbientB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_ambient.z = m_editAmbientB.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditAmbientA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_ambient.w = m_editAmbientA.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_diffuse.x = m_editDiffuseR.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_diffuse.y = m_editDiffuseG.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_diffuse.z = m_editDiffuseB.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditDiffuseA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_diffuse.w = m_editDiffuseA.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditSpecularR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_specular.x = m_editSpecularR.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditSpecularG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_specular.y = m_editSpecularG.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditSpecularB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_specular.z = m_editSpecularB.GetValue();
}

void CDirectionalLightInspectorFormView::OnEnChangeEditSpecularExponent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::DirectionalLight* pDirectionalLight = this->GetCLVItemToModify()->GetDirectionalLight();
	pDirectionalLight->m_specular.w = m_editSpecularExp.GetValue();
}

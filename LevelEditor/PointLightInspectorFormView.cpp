#include "LevelEditor.h"
#include "PointLightInspectorFormView.h"
#include "CLVItem\CLVItemPointLight.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>

// CPointLightInspectorFormView

IMPLEMENT_DYNCREATE(CPointLightInspectorFormView, CFormView)

CPointLightInspectorFormView::CPointLightInspectorFormView()
	: CFormView(IDD_POINT_LIGHT_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{
}

CPointLightInspectorFormView::~CPointLightInspectorFormView()
{
}

void CPointLightInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_DIFFUSE_R, m_editDiffuseR);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_DIFFUSE_G, m_editDiffuseG);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_DIFFUSE_B, m_editDiffuseB);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_DIFFUSE_A, m_editDiffuseA);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_SPECULAR_R, m_editSpecularR);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_SPECULAR_G, m_editSpecularG);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_SPECULAR_B, m_editSpecularB);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_SPECULAR_EXPONENT, m_editSpecularExp);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_RANGE, m_editRange);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_ATT_X, m_editAttX);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_ATT_Y, m_editAttY);
	DDX_Control(pDX, IDC_EDIT_POINT_LIGHT_ATT_Z, m_editAttZ);
}

BEGIN_MESSAGE_MAP(CPointLightInspectorFormView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_DIFFUSE_R, &CPointLightInspectorFormView::OnEnChangeEditDiffuseR)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_DIFFUSE_G, &CPointLightInspectorFormView::OnEnChangeEditDiffuseG)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_DIFFUSE_B, &CPointLightInspectorFormView::OnEnChangeEditDiffuseB)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_DIFFUSE_A, &CPointLightInspectorFormView::OnEnChangeEditDiffuseA)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_SPECULAR_R, &CPointLightInspectorFormView::OnEnChangeEditSpecularR)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_SPECULAR_G, &CPointLightInspectorFormView::OnEnChangeEditSpecularG)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_SPECULAR_B, &CPointLightInspectorFormView::OnEnChangeEditSpecularB)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_SPECULAR_EXPONENT, &CPointLightInspectorFormView::OnEnChangeEditSpecularExponent)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_RANGE, &CPointLightInspectorFormView::OnEnChangeEditPointLightRange)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_ATT_X, &CPointLightInspectorFormView::OnEnChangeEditPointLightAttX)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_ATT_Y, &CPointLightInspectorFormView::OnEnChangeEditPointLightAttY)
	ON_EN_CHANGE(IDC_EDIT_POINT_LIGHT_ATT_Z, &CPointLightInspectorFormView::OnEnChangeEditPointLightAttZ)
END_MESSAGE_MAP()


// CPointLightInspectorFormView diagnostics

#ifdef _DEBUG
void CPointLightInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPointLightInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPointLightInspectorFormView message handlers

void CPointLightInspectorFormView::OnEnChangeEditDiffuseR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_diffuse.x = m_editDiffuseR.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditDiffuseG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_diffuse.y = m_editDiffuseG.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditDiffuseB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_diffuse.z = m_editDiffuseB.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditDiffuseA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_diffuse.w = m_editDiffuseA.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditSpecularR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_specular.x = m_editSpecularR.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditSpecularG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_specular.y = m_editSpecularG.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditSpecularB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_specular.z = m_editSpecularB.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditSpecularExponent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->m_specular.w = m_editSpecularExp.GetValue();
}

void CPointLightInspectorFormView::OnEnChangeEditPointLightRange()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->SetRange(m_editRange.GetValue());
}

void CPointLightInspectorFormView::OnEnChangeEditPointLightAttX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->SetDistAttConstant(m_editAttX.GetValue());
}

void CPointLightInspectorFormView::OnEnChangeEditPointLightAttY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->SetDistAttLinear(m_editAttY.GetValue());
}

void CPointLightInspectorFormView::OnEnChangeEditPointLightAttZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	ze::PointLight* pPointLight = this->GetCLVItemToModify()->GetPointLight();
	pPointLight->SetDistAttSquare(m_editAttZ.GetValue());
}

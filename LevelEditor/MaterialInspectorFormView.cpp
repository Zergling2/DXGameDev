#include "LevelEditor.h"
#include "MaterialInspectorFormView.h"
#include "MainFrm.h"
#include "AssetTreeView.h"
#include "ATVItem\ATVItemMaterial.h"
#include "ATVItem\ATVItemTexture.h"
#include <ZergEngine\CoreSystem\Resource\Material.h>

// CMaterialInspectorFormView

IMPLEMENT_DYNCREATE(CMaterialInspectorFormView, CFormView)

CMaterialInspectorFormView::CMaterialInspectorFormView()
	: CFormView(IDD_MATERIAL_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{
}

CMaterialInspectorFormView::~CMaterialInspectorFormView()
{
}

void CMaterialInspectorFormView::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_COMBO_DIFFUSE_MAP, m_comboDiffuseMap);
	DDX_Control(pDX, IDC_COMBO_NORMAL_MAP, m_comboNormalMap);
	DDX_Control(pDX, IDC_COMBO_SPECULAR_MAP, m_comboSpecularMap);
}

BEGIN_MESSAGE_MAP(CMaterialInspectorFormView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_R, &CMaterialInspectorFormView::OnEnChangeEditAmbientR)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_G, &CMaterialInspectorFormView::OnEnChangeEditAmbientG)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_B, &CMaterialInspectorFormView::OnEnChangeEditAmbientB)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_A, &CMaterialInspectorFormView::OnEnChangeEditAmbientA)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_R, &CMaterialInspectorFormView::OnEnChangeEditDiffuseR)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_G, &CMaterialInspectorFormView::OnEnChangeEditDiffuseG)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_B, &CMaterialInspectorFormView::OnEnChangeEditDiffuseB)
	ON_EN_CHANGE(IDC_EDIT_DIFFUSE_A, &CMaterialInspectorFormView::OnEnChangeEditDiffuseA)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_R, &CMaterialInspectorFormView::OnEnChangeEditSpecularR)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_G, &CMaterialInspectorFormView::OnEnChangeEditSpecularG)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_B, &CMaterialInspectorFormView::OnEnChangeEditSpecularB)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR_EXPONENT, &CMaterialInspectorFormView::OnEnChangeEditSpecularExponent)
	ON_CBN_SELCHANGE(IDC_COMBO_DIFFUSE_MAP, &CMaterialInspectorFormView::OnCbnSelchangeComboDiffuseMap)
	ON_CBN_DROPDOWN(IDC_COMBO_DIFFUSE_MAP, &CMaterialInspectorFormView::OnCbnDropdownComboDiffuseMap)
	ON_CBN_SELCHANGE(IDC_COMBO_NORMAL_MAP, &CMaterialInspectorFormView::OnCbnSelchangeComboNormalMap)
	ON_CBN_DROPDOWN(IDC_COMBO_NORMAL_MAP, &CMaterialInspectorFormView::OnCbnDropdownComboNormalMap)
END_MESSAGE_MAP()


// CMaterialInspectorFormView diagnostics

#ifdef _DEBUG
void CMaterialInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMaterialInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMaterialInspectorFormView message handlers

void CMaterialInspectorFormView::OnEnChangeEditAmbientR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editAmbientR.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_ambient.x = m_editAmbientR.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditAmbientG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editAmbientG.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_ambient.y = m_editAmbientG.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditAmbientB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editAmbientB.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_ambient.z = m_editAmbientB.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditAmbientA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editAmbientA.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_ambient.w = m_editAmbientA.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditDiffuseR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editDiffuseR.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_diffuse.x = m_editDiffuseR.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditDiffuseG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editDiffuseG.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_diffuse.y = m_editDiffuseG.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditDiffuseB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editDiffuseB.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_diffuse.z = m_editDiffuseB.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditDiffuseA()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editDiffuseA.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_diffuse.w = m_editDiffuseA.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditSpecularR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editSpecularR.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_specular.x = m_editSpecularR.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditSpecularG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editSpecularG.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_specular.y = m_editSpecularG.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditSpecularB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editSpecularB.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_specular.z = m_editSpecularB.GetValue();
}

void CMaterialInspectorFormView::OnEnChangeEditSpecularExponent()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editSpecularExp.UpdateData(TRUE);

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_specular.w = m_editSpecularExp.GetValue();
}

void CMaterialInspectorFormView::OnCbnSelchangeComboDiffuseMap()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboDiffuseMap.GetCurSel();
	if (sel == CB_ERR)
		return;

	ATVItemTexture* pATVItemTexture = reinterpret_cast<ATVItemTexture*>(m_comboDiffuseMap.GetItemData(sel));
	
	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_diffuseMap = pATVItemTexture->m_texture;
}

void CMaterialInspectorFormView::OnCbnDropdownComboDiffuseMap()
{
	// TODO: Add your control notification handler code here
	m_comboDiffuseMap.ResetContent();
	auto textureSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetTextureSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = textureSet.cend();
	for (auto iter = textureSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		ASSERT(ret != FALSE);

		int index = m_comboDiffuseMap.AddString(text);
		m_comboDiffuseMap.SetItemData(index, qi.lParam);
	}
}

void CMaterialInspectorFormView::OnCbnSelchangeComboNormalMap()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboNormalMap.GetCurSel();
	if (sel == CB_ERR)
		return;

	ATVItemTexture* pATVItemTexture = reinterpret_cast<ATVItemTexture*>(m_comboNormalMap.GetItemData(sel));

	ze::Material* pMaterial = this->GetATVItemToModify()->GetMaterialPtr();
	pMaterial->m_normalMap = pATVItemTexture->m_texture;
}

void CMaterialInspectorFormView::OnCbnDropdownComboNormalMap()
{
	// TODO: Add your control notification handler code here
	m_comboNormalMap.ResetContent();
	auto textureSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetTextureSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = textureSet.cend();
	for (auto iter = textureSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		ASSERT(ret != FALSE);

		int index = m_comboNormalMap.AddString(text);
		m_comboNormalMap.SetItemData(index, qi.lParam);
	}
}


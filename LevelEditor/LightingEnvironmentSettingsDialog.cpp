#include "LevelEditor.h"
#include "LightingEnvironmentSettingsDialog.h"
#include "MainFrm.h"
#include "AssetTreeView.h"
#include "ATVItem\ATVItemTexture.h"
#include <ZergEngine\CoreSystem\RenderSettings.h>

// CLightingEnvironmentSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLightingEnvironmentSettingsDialog, CDialog)

CLightingEnvironmentSettingsDialog::CLightingEnvironmentSettingsDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHTING_ENVIRONMENT_SETTINGS_DIALOG, pParent)
{

}

CLightingEnvironmentSettingsDialog::~CLightingEnvironmentSettingsDialog()
{
}

void CLightingEnvironmentSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ENVIRONMENT_SKYBOX_TEXTURE, m_comboEnvironmentSkyboxTexture);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_LIGHT_COLOR_R, m_editAmbientLightColorR);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_LIGHT_COLOR_G, m_editAmbientLightColorG);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_LIGHT_COLOR_B, m_editAmbientLightColorB);
	DDX_Control(pDX, IDC_EDIT_AMBIENT_LIGHT_INTENSITY, m_editAmbientLightIntensity);
}


BEGIN_MESSAGE_MAP(CLightingEnvironmentSettingsDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ENVIRONMENT_SKYBOX_TEXTURE, &CLightingEnvironmentSettingsDialog::OnCbnSelchangeComboEnvironmentSkyboxTexture)
	ON_CBN_DROPDOWN(IDC_COMBO_ENVIRONMENT_SKYBOX_TEXTURE, &CLightingEnvironmentSettingsDialog::OnCbnDropdownComboEnvironmentSkyboxTexture)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_LIGHT_COLOR_R, &CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorR)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_LIGHT_COLOR_G, &CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorG)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_LIGHT_COLOR_B, &CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorB)
	ON_EN_CHANGE(IDC_EDIT_AMBIENT_LIGHT_INTENSITY, &CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightIntensity)
END_MESSAGE_MAP()


// CLightingEnvironmentSettingsDialog message handlers

BOOL CLightingEnvironmentSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	const XMFLOAT3 ambientLightColor = ze::RenderSettings::GetInstance()->GetAmbientLightColor();
	const FLOAT ambientLightIntensity = ze::RenderSettings::GetInstance()->GetAmbientLightIntensity();

	TCHAR str[32];

	// Ambient Light Color
	StringCbPrintf(str, sizeof(str), _T("%f"), ambientLightColor.x);
	m_editAmbientLightColorR.SetWindowTextW(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), ambientLightColor.y);
	m_editAmbientLightColorG.SetWindowTextW(str);
	StringCbPrintf(str, sizeof(str), _T("%f"), ambientLightColor.z);
	m_editAmbientLightColorB.SetWindowTextW(str);

	// Ambient Light Intensity
	StringCbPrintf(str, sizeof(str), _T("%f"), ambientLightIntensity);
	m_editAmbientLightIntensity.SetWindowTextW(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLightingEnvironmentSettingsDialog::OnCbnSelchangeComboEnvironmentSkyboxTexture()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboEnvironmentSkyboxTexture.GetCurSel();
	if (sel == CB_ERR)
		return;

	ATVItemTexture* pATVItemTexture = reinterpret_cast<ATVItemTexture*>(m_comboEnvironmentSkyboxTexture.GetItemData(sel));
	ze::RenderSettings::GetInstance()->SetSkybox(pATVItemTexture->m_texture);
}

void CLightingEnvironmentSettingsDialog::OnCbnDropdownComboEnvironmentSkyboxTexture()
{
	// TODO: Add your control notification handler code here
	m_comboEnvironmentSkyboxTexture.ResetContent();
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

		int index = m_comboEnvironmentSkyboxTexture.AddString(text);
		m_comboEnvironmentSkyboxTexture.SetItemData(index, qi.lParam);
	}
}

void CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorR()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	// m_editAmbientLightColorR.UpdateData(TRUE);	// 컨트롤 타입은 최신 값 들어있어서 UpdateData 호출 불필요

	XMFLOAT3 ambientLightColor = ze::RenderSettings::GetInstance()->GetAmbientLightColor();
	ambientLightColor.x = m_editAmbientLightColorR.GetValue();
	ze::RenderSettings::GetInstance()->SetAmbientLightColor(ambientLightColor);
}

void CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorG()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	// m_editAmbientLightColorG.UpdateData(TRUE);	// 컨트롤 타입은 최신 값 들어있어서 UpdateData 호출 불필요

	XMFLOAT3 ambientLightColor = ze::RenderSettings::GetInstance()->GetAmbientLightColor();
	ambientLightColor.y = m_editAmbientLightColorG.GetValue();
	ze::RenderSettings::GetInstance()->SetAmbientLightColor(ambientLightColor);
}

void CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightColorB()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	XMFLOAT3 ambientLightColor = ze::RenderSettings::GetInstance()->GetAmbientLightColor();
	ambientLightColor.z = m_editAmbientLightColorB.GetValue();
	ze::RenderSettings::GetInstance()->SetAmbientLightColor(ambientLightColor);
}

void CLightingEnvironmentSettingsDialog::OnEnChangeEditAmbientLightIntensity()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	ze::RenderSettings::GetInstance()->SetAmbientLightIntensity(m_editAmbientLightIntensity.GetValue());
}

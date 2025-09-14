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
}


BEGIN_MESSAGE_MAP(CLightingEnvironmentSettingsDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ENVIRONMENT_SKYBOX_TEXTURE, &CLightingEnvironmentSettingsDialog::OnCbnSelchangeComboEnvironmentSkyboxTexture)
	ON_CBN_DROPDOWN(IDC_COMBO_ENVIRONMENT_SKYBOX_TEXTURE, &CLightingEnvironmentSettingsDialog::OnCbnDropdownComboEnvironmentSkyboxTexture)
END_MESSAGE_MAP()


// CLightingEnvironmentSettingsDialog message handlers

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

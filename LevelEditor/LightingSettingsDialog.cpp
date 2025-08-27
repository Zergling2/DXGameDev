#include "LevelEditor.h"
#include "LightingSettingsDialog.h"


// CLightingSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLightingSettingsDialog, CDialog)

CLightingSettingsDialog::CLightingSettingsDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHTING_SETTINGS_DIALOG, pParent)
	, m_pTabChildShow(nullptr)
{
}

CLightingSettingsDialog::~CLightingSettingsDialog()
{
}

void CLightingSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_LIGHTING_MAIN, m_tabMain);
}


BEGIN_MESSAGE_MAP(CLightingSettingsDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_LIGHTING_MAIN, &CLightingSettingsDialog::OnTcnSelchangeTabLightingMain)
END_MESSAGE_MAP()


// CLightingSettingsDialog message handlers

BOOL CLightingSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	PCTSTR tabName[] = { _T("Scene"), _T("Environment"), _T("Realtime Lightmaps"), _T("Baked Lightmaps") };
	for (int i = 0; i < _countof(tabName); ++i)
	{
		m_tabMain.InsertItem(i, tabName[i]);
	}


	// 자식 탭 항목 윈도우들 생성
	CRect rect;
	m_tabMain.GetClientRect(&rect);

	m_sceneSettingsDlg.Create(IDD_LIGHTING_SCENE_SETTINGS_DIALOG, &m_tabMain);
	m_sceneSettingsDlg.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_SHOWWINDOW | SWP_NOZORDER);
	m_pTabChildShow = &m_sceneSettingsDlg;

	m_environmentSettingsDlg.Create(IDD_LIGHTING_ENVIRONMENT_SETTINGS_DIALOG, &m_tabMain);
	m_environmentSettingsDlg.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	m_realtimeLightmapsSettingsDlg.Create(IDD_LIGHTING_REALTIME_LIGHTMAPS_SETTINGS_DIALOG, &m_tabMain);
	m_realtimeLightmapsSettingsDlg.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	m_bakedLightmapsSettingsDlg.Create(IDD_LIGHTING_BAKED_LIGHTMAPS_SETTINGS_DIALOG, &m_tabMain);
	m_bakedLightmapsSettingsDlg.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLightingSettingsDialog::OnTcnSelchangeTabLightingMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	if (m_pTabChildShow != nullptr)
	{
		m_pTabChildShow->ShowWindow(SW_HIDE);
		m_pTabChildShow = nullptr;
	}

	const int tabIndex = m_tabMain.GetCurSel();
	switch (tabIndex)
	{
	case 0:
		m_sceneSettingsDlg.ShowWindow(SW_SHOW);
		m_pTabChildShow = &m_sceneSettingsDlg;
		break;
	case 1:
		m_environmentSettingsDlg.ShowWindow(SW_SHOW);
		m_pTabChildShow = &m_environmentSettingsDlg;
		break;
	case 2:
		m_realtimeLightmapsSettingsDlg.ShowWindow(SW_SHOW);
		m_pTabChildShow = &m_realtimeLightmapsSettingsDlg;
		break;
	case 3:
		m_bakedLightmapsSettingsDlg.ShowWindow(SW_SHOW);
		m_pTabChildShow = &m_bakedLightmapsSettingsDlg;
		break;
	default:
		break;
	}

	*pResult = 0;
}

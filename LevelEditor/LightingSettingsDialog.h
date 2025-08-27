#pragma once
#include "afxdialogex.h"
#include "LightingSceneSettingsDialog.h"
#include "LightingEnvironmentSettingsDialog.h"
#include "LightingRealtimeLightmapsSettingsDialog.h"
#include "LightingBakedLightmapsSettingsDialog.h"

// CLightingSettingsDialog dialog

class CLightingSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightingSettingsDialog)

public:
	CLightingSettingsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightingSettingsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTING_SETTINGS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabMain;

	CWnd* m_pTabChildShow;
	CLightingSceneSettingsDialog m_sceneSettingsDlg;
	CLightingEnvironmentSettingsDialog m_environmentSettingsDlg;
	CLightingRealtimeLightmapsSettingsDialog m_realtimeLightmapsSettingsDlg;;
	CLightingBakedLightmapsSettingsDialog m_bakedLightmapsSettingsDlg;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabLightingMain(NMHDR* pNMHDR, LRESULT* pResult);
};

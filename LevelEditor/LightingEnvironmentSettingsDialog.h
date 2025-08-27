#pragma once
#include "afxdialogex.h"


// CLightingEnvironmentSettingsDialog dialog

class CLightingEnvironmentSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightingEnvironmentSettingsDialog)

public:
	CLightingEnvironmentSettingsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightingEnvironmentSettingsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTING_ENVIRONMENT_SETTINGS_DIALOG };
#endif
public:
	CComboBox m_comboEnvironmentSkyboxTexture;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboEnvironmentSkyboxMaterial();
	afx_msg void OnCbnDropdownComboEnvironmentSkyboxMaterial();
};

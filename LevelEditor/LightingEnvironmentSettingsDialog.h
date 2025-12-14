#pragma once

#include "afxdialogex.h"
#include "Edit\FloatEdit.h"

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
	CFloatEdit m_editAmbientLightColorR;
	CFloatEdit m_editAmbientLightColorG;
	CFloatEdit m_editAmbientLightColorB;
	CFloatEdit m_editAmbientLightIntensity;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboEnvironmentSkyboxTexture();
	afx_msg void OnCbnDropdownComboEnvironmentSkyboxTexture();
	afx_msg void OnEnChangeEditAmbientLightColorR();
	afx_msg void OnEnChangeEditAmbientLightColorG();
	afx_msg void OnEnChangeEditAmbientLightColorB();
	afx_msg void OnEnChangeEditAmbientLightIntensity();
};

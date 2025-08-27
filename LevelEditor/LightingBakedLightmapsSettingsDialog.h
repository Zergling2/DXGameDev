#pragma once
#include "afxdialogex.h"

// CLightingBakedLightmapsSettingsDialog dialog

class CLightingBakedLightmapsSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightingBakedLightmapsSettingsDialog)

public:
	CLightingBakedLightmapsSettingsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightingBakedLightmapsSettingsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTING_BAKED_LIGHTMAPS_SETTINGS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

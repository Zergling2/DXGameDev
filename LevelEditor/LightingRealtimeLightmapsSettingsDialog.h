#pragma once
#include "afxdialogex.h"


// CLightingRealtimeLightmapsSettingsDialog dialog

class CLightingRealtimeLightmapsSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightingRealtimeLightmapsSettingsDialog)

public:
	CLightingRealtimeLightmapsSettingsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightingRealtimeLightmapsSettingsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTING_REALTIME_LIGHTMAPS_SETTINGS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

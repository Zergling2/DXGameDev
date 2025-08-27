#pragma once
#include "afxdialogex.h"

// CLightingSceneSettingsDialog dialog

class CLightingSceneSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightingSceneSettingsDialog)

public:
	CLightingSceneSettingsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLightingSceneSettingsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTING_SCENE_SETTINGS_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

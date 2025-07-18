#pragma once

#include "framework.h"

/*
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
*/

// CLevelEditorApp:
// See LevelEditor.cpp for the implementation of this class
//

class CLevelEditorApp : public CWinApp
{
private:
	ze::GameObjectHandle m_hEditorCameraObject;
public:
	CLevelEditorApp() noexcept;

// Overrides
public:
	DECLARE_MESSAGE_MAP()
public:
	// Implementation
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	afx_msg void OnAppAbout();
	virtual BOOL OnIdle(LONG lCount);
};

extern CLevelEditorApp theApp;

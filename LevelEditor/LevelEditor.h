// LevelEditor.h : main header file for the LevelEditor application
//
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
public:
	CLevelEditorApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLevelEditorApp theApp;

#pragma once

#include "framework.h"

// CEmptyInspectorFormView form view

class CEmptyInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CEmptyInspectorFormView)

protected:
	CEmptyInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CEmptyInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EMPTY_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};



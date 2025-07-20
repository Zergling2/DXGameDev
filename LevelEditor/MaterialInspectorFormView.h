#pragma once

#include "framework.h"

// CMaterialInspectorFormView form view

class CMaterialInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CMaterialInspectorFormView)

protected:
	CMaterialInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CMaterialInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATERIAL_INSPECTOR_FORMVIEW };
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

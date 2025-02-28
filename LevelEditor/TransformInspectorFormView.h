#pragma once

#include "framework.h"

// CTransformInspectorFormView form view

class CTransformInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CTransformInspectorFormView)

protected:
	CTransformInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CTransformInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTransformInspectorFormView };
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



#pragma once

#include "framework.h"
#include "Edit\FloatEdit.h"

class CLVItemDirectionalLight;

// CDirectionalLightInspectorFormView form view

class CDirectionalLightInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CDirectionalLightInspectorFormView)

protected:
	CDirectionalLightInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CDirectionalLightInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTIONAL_LIGHT_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetCLVItemToModify(CLVItemDirectionalLight* pItem) { m_pItem = pItem; }
	CLVItemDirectionalLight* GetCLVItemToModify() const { return m_pItem; }
private:
	CLVItemDirectionalLight* m_pItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFloatEdit m_editDiffuseR;
	CFloatEdit m_editDiffuseG;
	CFloatEdit m_editDiffuseB;
	CFloatEdit m_editDiffuseA;
	CFloatEdit m_editSpecularR;
	CFloatEdit m_editSpecularG;
	CFloatEdit m_editSpecularB;
	CFloatEdit m_editSpecularExp;
public:
	afx_msg void OnEnChangeEditDiffuseR();
	afx_msg void OnEnChangeEditDiffuseG();
	afx_msg void OnEnChangeEditDiffuseB();
	afx_msg void OnEnChangeEditDiffuseA();
	afx_msg void OnEnChangeEditSpecularR();
	afx_msg void OnEnChangeEditSpecularG();
	afx_msg void OnEnChangeEditSpecularB();
	afx_msg void OnEnChangeEditSpecularExponent();
};

#pragma once

#include "framework.h"
#include "Edit\FloatEdit.h"

class CLVItemPointLight;

// CPointLightInspectorFormView form view

class CPointLightInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CPointLightInspectorFormView)

protected:
	CPointLightInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CPointLightInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POINT_LIGHT_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetCLVItemToModify(CLVItemPointLight* pItem) { m_pItem = pItem; }
	CLVItemPointLight* GetCLVItemToModify() const { return m_pItem; }
private:
	CLVItemPointLight* m_pItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFloatEdit m_editAmbientR;
	CFloatEdit m_editAmbientG;
	CFloatEdit m_editAmbientB;
	CFloatEdit m_editAmbientA;
	CFloatEdit m_editDiffuseR;
	CFloatEdit m_editDiffuseG;
	CFloatEdit m_editDiffuseB;
	CFloatEdit m_editDiffuseA;
	CFloatEdit m_editSpecularR;
	CFloatEdit m_editSpecularG;
	CFloatEdit m_editSpecularB;
	CFloatEdit m_editSpecularExp;
	CFloatEdit m_editRange;
	CFloatEdit m_editAttX;
	CFloatEdit m_editAttY;
	CFloatEdit m_editAttZ;
public:
	afx_msg void OnEnChangeEditAmbientR();
	afx_msg void OnEnChangeEditAmbientG();
	afx_msg void OnEnChangeEditAmbientB();
	afx_msg void OnEnChangeEditAmbientA();
	afx_msg void OnEnChangeEditDiffuseR();
	afx_msg void OnEnChangeEditDiffuseG();
	afx_msg void OnEnChangeEditDiffuseB();
	afx_msg void OnEnChangeEditDiffuseA();
	afx_msg void OnEnChangeEditSpecularR();
	afx_msg void OnEnChangeEditSpecularG();
	afx_msg void OnEnChangeEditSpecularB();
	afx_msg void OnEnChangeEditSpecularExponent();
	afx_msg void OnEnChangeEditPointLightRange();
	afx_msg void OnEnChangeEditPointLightAttX();
	afx_msg void OnEnChangeEditPointLightAttY();
	afx_msg void OnEnChangeEditPointLightAttZ();
};

#pragma once

#include "framework.h"
#include "Edit\FloatEdit.h"

class CLVItemSpotLight;

// CSpotLightInspectorFormView form view

class CSpotLightInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CSpotLightInspectorFormView)

protected:
	CSpotLightInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CSpotLightInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPOT_LIGHT_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetCLVItemToModify(CLVItemSpotLight* pItem) { m_pItem = pItem; }
	CLVItemSpotLight* GetCLVItemToModify() const { return m_pItem; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CLVItemSpotLight* m_pItem;
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
	CFloatEdit m_editSpotExp;
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
	afx_msg void OnEnChangeEditSpotLightSpotExp();
	afx_msg void OnEnChangeEditSpotLightRange();
	afx_msg void OnEnChangeEditSpotLightAttX();
	afx_msg void OnEnChangeEditSpotLightAttY();
	afx_msg void OnEnChangeEditSpotLightAttZ();
};

#pragma once

#include "framework.h"
#include "Edit\FloatEdit.h"

class ATVItemMaterial;

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
	void SetATVItemToModify(ATVItemMaterial* pItem) { m_pItem = pItem; }
	ATVItemMaterial* GetATVItemToModify() const { return m_pItem; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	ATVItemMaterial* m_pItem;
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
	CComboBox m_comboDiffuseMap;
	CComboBox m_comboNormalMap;
	CComboBox m_comboSpecularMap;
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
};

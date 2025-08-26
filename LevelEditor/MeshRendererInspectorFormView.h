#pragma once

#include "framework.h"

class CLVItemMeshRenderer;

// CMeshRendererInspectorFormView form view

class CMeshRendererInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CMeshRendererInspectorFormView)

protected:
	CMeshRendererInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CMeshRendererInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESH_RENDERER_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
private:
	CLVItemMeshRenderer* m_pItem;
public:
	CButton m_castShadows;
	CButton m_receiveShadows;
	CComboBox m_comboSelectMesh;
	CComboBox m_comboSelectSubsetIndex;
	CComboBox m_comboSelectMaterial;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetCLVItemToModify(CLVItemMeshRenderer* pItem) { m_pItem = pItem; }
	CLVItemMeshRenderer* GetCLVItemToModify() const { return m_pItem; }

	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedMeshRendererCastShadows();
	afx_msg void OnBnClickedMeshRendererReceiveShadows();
	afx_msg void OnCbnSelchangeComboSelectMesh();
	afx_msg void OnCbnDropdownComboSelectMesh();
	afx_msg void OnCbnCloseupComboSelectMesh();
	afx_msg void OnBnClickedButtonRemoveMesh();
	afx_msg void OnCbnSelchangeComboSelectSubsetIndex();
	afx_msg void OnCbnSelchangeComboSelectMaterial();
	afx_msg void OnCbnDropdownComboSelectMaterial();
};



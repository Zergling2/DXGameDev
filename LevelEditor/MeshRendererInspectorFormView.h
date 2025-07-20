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
	void SetCLVItemToModify(CLVItemMeshRenderer* pItem) { m_pItem = pItem; }
	CLVItemMeshRenderer* GetCLVItemToModify() const { return m_pItem; }
public:
	CButton m_castShadows;
	CButton m_receiveShadows;
private:
	CLVItemMeshRenderer* m_pItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMeshRendererCastShadows();
	afx_msg void OnBnClickedMeshRendererReceiveShadows();
};



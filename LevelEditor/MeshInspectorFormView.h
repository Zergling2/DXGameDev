#pragma once

#include "framework.h"

class ATVItemStaticMesh;

// CMeshInspectorFormView form view

class CMeshInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CMeshInspectorFormView)

protected:
	CMeshInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CMeshInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESH_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetATVItemToModify(ATVItemStaticMesh* pItem) { m_pItem = pItem; }
	ATVItemStaticMesh* GetATVItemToModify() const { return m_pItem; }
private:
	ATVItemStaticMesh* m_pItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_staticMeshSubsetCount;
	CString m_staticMeshName;
	int m_staticMeshVertexCount;
	CString m_staticMeshVertexFormatType;
};

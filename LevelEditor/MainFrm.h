// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "framework.h"

class CHierarchyTreeView;
class CComponentListView;
class CLevelEditorView;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	bool m_splitterCreated;
	CSplitterWnd m_wndSplitter[5];
// Operations
public:
// Overrides
public:
	CHierarchyTreeView* GetHierarchyTreeView() const;
	CComponentListView* GetComponentListView() const;
	CLevelEditorView* GetLevelEditorView() const;
	CFormView* GetComponentInspectorFormView() const;
	CFormView* SwitchComponentInspectorFormView(CRuntimeClass* pRtClass);

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void On3DObjectTerrain();
	afx_msg void OnGameObjectCreateEmpty();
	afx_msg void OnGameObjectRename();
	afx_msg void OnComponentMeshRenderer();
};

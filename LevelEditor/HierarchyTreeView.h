#pragma once

#include "framework.h"

// CHierarchyTreeView view

class CHierarchyTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CHierarchyTreeView)
protected:
	CHierarchyTreeView();           // protected constructor used by dynamic creation
	virtual ~CHierarchyTreeView();
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()
private:
	bool m_initialized;
private:
	void DeleteTreeItemDataRecursive(HTREEITEM hItem);
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On3DObjectTerrain();
	afx_msg void OnGameObjectCreateEmpty();
	afx_msg void OnGameObjectRename();
	afx_msg void OnDestroy();
};

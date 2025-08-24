#pragma once

#include "framework.h"

// CAssetTreeView view

class CAssetTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CAssetTreeView)

protected:
	CAssetTreeView();           // protected constructor used by dynamic creation
	virtual ~CAssetTreeView();

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
	void DeleteTreeItemDataRecursive(CTreeCtrl& tc, HTREEITEM hItem);
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
};

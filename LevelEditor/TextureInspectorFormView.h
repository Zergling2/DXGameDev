#pragma once

#include "framework.h"

class ATVItemTexture;

// CTextureInspectorFormView form view

class CTextureInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CTextureInspectorFormView)

protected:
	CTextureInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CTextureInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXTURE_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetATVItemToModify(ATVItemTexture* pItem) { m_pItem = pItem; }
	ATVItemTexture* GetATVItemToModify() const { return m_pItem; }
private:
	ATVItemTexture* m_pItem;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_staticTextureArraySize;
	int m_staticTextureMipLevels;
	CString m_staticTextureType;
	CString m_staticTextureFormat;
	CString m_staticTextureSize;
};

#pragma once

#include "framework.h"

class CLevelEditorDoc;

class CLevelEditorView : public CView
{
protected: // create from serialization only
	CLevelEditorView() noexcept;
	DECLARE_DYNCREATE(CLevelEditorView)

// Attributes
public:
	CLevelEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// Implementation
public:
	virtual ~CLevelEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void Render();
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pSwapChainRTV;
};

#ifndef _DEBUG  // debug version in LevelEditorView.cpp
inline CLevelEditorDoc* CLevelEditorView::GetDocument() const
{
	return reinterpret_cast<CLevelEditorDoc*>(m_pDocument);
}
#endif

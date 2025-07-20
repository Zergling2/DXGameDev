#pragma once

#include "framework.h"
#include "Edit\FloatEdit.h"

class CLVItemTransform;

// CTransformInspectorFormView form view

class CTransformInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CTransformInspectorFormView)

protected:
	CTransformInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CTransformInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSFORM_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void SetCLVItemToModify(CLVItemTransform* pItem) { m_pItem = pItem; }
	CLVItemTransform* GetCLVItemToModify() const { return m_pItem; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CLVItemTransform* m_pItem;
public:
	CFloatEdit m_positionX;
	CFloatEdit m_positionY;
	CFloatEdit m_positionZ;
	CFloatEdit m_rotationX;
	CFloatEdit m_rotationY;
	CFloatEdit m_rotationZ;
	CFloatEdit m_scaleX;
	CFloatEdit m_scaleY;
	CFloatEdit m_scaleZ;
	afx_msg void OnEnChangeEditPositionX();
	afx_msg void OnEnChangeEditPositionY();
	afx_msg void OnEnChangeEditPositionZ();
	afx_msg void OnEnChangeEditRotationX();
	afx_msg void OnEnChangeEditRotationY();
	afx_msg void OnEnChangeEditRotationZ();
	afx_msg void OnEnChangeEditScaleX();
	afx_msg void OnEnChangeEditScaleY();
	afx_msg void OnEnChangeEditScaleZ();
};



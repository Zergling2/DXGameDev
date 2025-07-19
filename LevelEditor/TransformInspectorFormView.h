#pragma once

#include "framework.h"
#include "TransformEdit\TransformEdit.h"

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
	CTransformEdit m_positionX;
	CTransformEdit m_positionY;
	CTransformEdit m_positionZ;
	CTransformEdit m_rotationX;
	CTransformEdit m_rotationY;
	CTransformEdit m_rotationZ;
	CTransformEdit m_scaleX;
	CTransformEdit m_scaleY;
	CTransformEdit m_scaleZ;
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



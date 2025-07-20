#pragma once

#include "framework.h"

class CLVItemTerrain;

// CTerrainInspectorFormView form view

enum class TERRAIN_BRUSH_TYPE
{
	BRUSH_TERRAIN,
	BRUSH_TREE,
	BRUSH_GRASS
};

class CTerrainInspectorFormView : public CFormView
{
	DECLARE_DYNCREATE(CTerrainInspectorFormView)

protected:
	CTerrainInspectorFormView();           // protected constructor used by dynamic creation
	virtual ~CTerrainInspectorFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAIN_INSPECTOR_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	void SetCLVItemToModify(CLVItemTerrain* pItem) { m_pItem = pItem; }
	CLVItemTerrain* GetCLVItemToModify() const { return m_pItem; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	void HideTerrainBrushUITools();
	void ShowTerrainBrushUITools();
private:
	CLVItemTerrain* m_pItem;
	TERRAIN_BRUSH_TYPE m_currentTerrainBrushType;
public:
	CButton m_radioBrushTerrain;
	CComboBox m_elevationModification;
	CStatic m_staticTerrainBrushSize;
	CStatic m_staticTerrainBrushOpacity;
	CSliderCtrl m_sliderTerrainBrushSize;
	CSliderCtrl m_sliderTerrainBrushOpacity;
	CEdit m_editTerrainBrushSize;
	CEdit m_editTerrainBrushOpacity;
	CStatic m_staticTerrainLayers;
	CStatic m_imgTerrainLayer0;
	CStatic m_imgTerrainLayer1;
	CStatic m_imgTerrainLayer2;
	CStatic m_imgTerrainLayer3;
	CStatic m_imgTerrainLayer4;
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedTerrainBrushType(UINT id);
};

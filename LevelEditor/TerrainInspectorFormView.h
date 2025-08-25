#pragma once

#include "framework.h"
#include "TerrainDataEditor.h"

class CLVItemTerrain;

// CTerrainInspectorFormView form view

enum class TERRAIN_BRUSH_TYPE
{
	BRUSH_TERRAIN,
	BRUSH_TREE,
	BRUSH_GRASS
};

enum class TERRAIN_EDIT_MODE
{
	RAISE_OR_LOWER_TERRAIN = 0,
	PAINT_TEXTURE = 1,
	SET_HEIGHT = 2,
	SMOOTH_HEIGHT = 3
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
	void SetCLVItemToModify(CLVItemTerrain* pItem);
	CLVItemTerrain* GetCLVItemToModify() const { return m_pItem; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	void OnHideTerrainBrushUI();
	void OnShowTerrainBrushUI();
private:
	TerrainDataEditor m_tde;
	CLVItemTerrain* m_pItem;
	TERRAIN_BRUSH_TYPE m_currentTerrainBrushType;
	TERRAIN_EDIT_MODE m_currentTerrainEditMode;
public:
	CButton m_radioBrushTerrain;
	// °øÅë
	CComboBox m_comboTerrainEditMode;
	CStatic m_staticTerrainBrushSize;
	CStatic m_staticTerrainBrushOpacity;
	CSliderCtrl m_sliderTerrainBrushSize;
	CSliderCtrl m_sliderTerrainBrushOpacity;
	CEdit m_editTerrainBrushSize;
	CEdit m_editTerrainBrushOpacity;
	// Raise or Lower Terrain
	
	// Paint Texture
	CStatic m_staticTerrainLayers;
	CStatic m_staticTerrainDiffuseLayer;
	CStatic m_staticTerrainNormalLayer;
	CComboBox m_comboTerrainDiffuseLayer;
	CComboBox m_comboTerrainNormalLayer;
	CButton m_buttonRemoveTerrainDiffuseLayer;
	CButton m_buttonRemoveTerrainNormalLayer;
	// Set Height
	CStatic m_staticSetTerrainHeightValue;
	CEdit m_editSetTerrainHeightValue;
	CStatic m_staticSetTerrainHeightValueHelp;
	// Smooth Height
	CStatic m_staticSmoothTerrainHeightBlurDirection;
	CSliderCtrl m_sliderSmoothTerrainHeightBlurDirection;
	CEdit m_editSmoothTerrainHeightBlurDirection;
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedTerrainBrushType(UINT id);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeComboTerrainEditMode();
	afx_msg void OnEnUpdateEditSetTerrainHeightValue();
	CStatic m_staticTerrainInfo;
};

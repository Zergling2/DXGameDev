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

	TerrainDataEditor& GetTerrainDataEditor() { return m_tde; }
	TERRAIN_BRUSH_TYPE GetTerrainBrushType() const { return m_currentTerrainBrushType; }
	TERRAIN_EDIT_MODE GetTerrainEditMode() const { return m_currentTerrainEditMode; }
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
	CStatic m_staticLayerModifying;
	CButton m_radioLayerModifying0;
	CButton m_radioLayerModifying1;
	CButton m_radioLayerModifying2;
	CButton m_radioLayerModifying3;
	CButton m_radioLayerModifying4;
	CButton m_buttonRemoveTerrainDiffuseLayer;
	CButton m_buttonRemoveTerrainNormalLayer;
	size_t m_modifyingLayerIndex;
	// Set Height
	CStatic m_staticSetTerrainHeightValue;
	CEdit m_editSetTerrainHeightValue;
	CStatic m_staticSetTerrainHeightValueHelp;
	// Smooth Height
	CStatic m_staticSmoothTerrainHeightBlurDirection;
	CSliderCtrl m_sliderSmoothTerrainHeightBlurDirection;
	CEdit m_editSmoothTerrainHeightBlurDirection;
	CStatic m_staticTerrainInfo;
public:
	virtual void OnInitialUpdate();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedTerrainBrushType(UINT id);
	afx_msg void OnBnClickedTerrainLayerToModify(UINT id);
	afx_msg void OnCbnSelchangeComboTerrainEditMode();
	afx_msg void OnEnUpdateEditSetTerrainHeightValue();
	afx_msg void OnCbnSelchangeComboTerrainDiffuseLayer();
	afx_msg void OnCbnDropdownComboTerrainDiffuseLayer();
	afx_msg void OnBnClickedButtonRemoveTerrainDiffuseLayer();
	afx_msg void OnCbnSelchangeComboTerrainNormalLayer();
	afx_msg void OnCbnDropdownComboTerrainNormalLayer();
	afx_msg void OnBnClickedButtonRemoveTerrainNormalLayer();
};

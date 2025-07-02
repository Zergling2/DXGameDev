#pragma once
#include "afxdialogex.h"


// CTerrainGenerationDialog dialog

class CTerrainGenerationDialog : public CDialog
{
	DECLARE_DYNAMIC(CTerrainGenerationDialog)
public:
	CTerrainGenerationDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTerrainGenerationDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TERRAIN_GENERATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	unsigned int m_patchCountRow;
	unsigned int m_patchCountColumn;
	float m_cellSize;
	float m_heightScale;
};

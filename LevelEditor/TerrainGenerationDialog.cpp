#include "LevelEditor.h"
#include "afxdialogex.h"
#include "TerrainGenerationDialog.h"

// CTerrainGenerationDialog dialog

IMPLEMENT_DYNAMIC(CTerrainGenerationDialog, CDialog)

CTerrainGenerationDialog::CTerrainGenerationDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TERRAIN_GENERATION_DIALOG, pParent)
	, m_patchCountRow(0)
	, m_patchCountColumn(0)
	, m_cellSize(0)
	, m_heightScale(0)
{

}

CTerrainGenerationDialog::~CTerrainGenerationDialog()
{
}

void CTerrainGenerationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TERRAIN_GENERATION_PATCH_COUNT_ROW, m_patchCountRow);
	DDV_MinMaxUInt(pDX, m_patchCountRow, 1U, 128U);
	DDX_Text(pDX, IDC_EDIT_TERRAIN_GENERATION_PATCH_COUNT_COLUMN, m_patchCountColumn);
	DDV_MinMaxUInt(pDX, m_patchCountColumn, 1U, 128U);
	DDX_Text(pDX, IDC_EDIT_TERRAIN_GENERATION_CELL_SIZE, m_cellSize);
	DDV_MinMaxFloat(pDX, m_cellSize, 0.1f, 1.0f);
	DDX_Text(pDX, IDC_EDIT_TERRAIN_GENERATION_HEIGHT_SCALE, m_heightScale);
	DDV_MinMaxFloat(pDX, m_heightScale, 0.001f, 1.0f);
}


BEGIN_MESSAGE_MAP(CTerrainGenerationDialog, CDialog)
END_MESSAGE_MAP()


// CTerrainGenerationDialog message handlers

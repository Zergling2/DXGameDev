#include "LevelEditor.h"
#include "LightingBakedLightmapsSettingsDialog.h"


// CLightingBakedLightmapsSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLightingBakedLightmapsSettingsDialog, CDialog)

CLightingBakedLightmapsSettingsDialog::CLightingBakedLightmapsSettingsDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHTING_BAKED_LIGHTMAPS_SETTINGS_DIALOG, pParent)
{

}

CLightingBakedLightmapsSettingsDialog::~CLightingBakedLightmapsSettingsDialog()
{
}

void CLightingBakedLightmapsSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLightingBakedLightmapsSettingsDialog, CDialog)
END_MESSAGE_MAP()


// CLightingBakedLightmapsSettingsDialog message handlers

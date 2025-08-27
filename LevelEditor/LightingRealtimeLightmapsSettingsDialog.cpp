#include "LevelEditor.h"
#include "LightingRealtimeLightmapsSettingsDialog.h"


// CLightingRealtimeLightmapsSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLightingRealtimeLightmapsSettingsDialog, CDialog)

CLightingRealtimeLightmapsSettingsDialog::CLightingRealtimeLightmapsSettingsDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHTING_REALTIME_LIGHTMAPS_SETTINGS_DIALOG, pParent)
{

}

CLightingRealtimeLightmapsSettingsDialog::~CLightingRealtimeLightmapsSettingsDialog()
{
}

void CLightingRealtimeLightmapsSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLightingRealtimeLightmapsSettingsDialog, CDialog)
END_MESSAGE_MAP()


// CLightingRealtimeLightmapsSettingsDialog message handlers

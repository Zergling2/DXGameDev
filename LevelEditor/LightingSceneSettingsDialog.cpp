#include "LevelEditor.h"
#include "LightingSceneSettingsDialog.h"


// CLightingSceneSettingsDialog dialog

IMPLEMENT_DYNAMIC(CLightingSceneSettingsDialog, CDialog)

CLightingSceneSettingsDialog::CLightingSceneSettingsDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LIGHTING_SCENE_SETTINGS_DIALOG, pParent)
{

}

CLightingSceneSettingsDialog::~CLightingSceneSettingsDialog()
{
}

void CLightingSceneSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLightingSceneSettingsDialog, CDialog)
END_MESSAGE_MAP()


// CLightingSceneSettingsDialog message handlers

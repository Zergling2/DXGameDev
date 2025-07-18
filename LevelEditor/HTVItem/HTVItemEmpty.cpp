#include "HTVItemEmpty.h"
#include "..\Settings.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void HTVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// �� �׸��� ���õǾ��� ���� Rename �޴� ���� ��Ȱ��ȭ
	CMenu* pMainFrameMenu = pMainFrame->GetMenu();
	CMenu* pSubMenu = pMainFrameMenu->GetSubMenu(GAMEOBJECT_MENU_INDEX);
	pSubMenu->EnableMenuItem(ID_GAMEOBJECT_RENAME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// CLV �׸� ��� ����
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// CIFV�� �� �� �� ����
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

#include "HTVItemEmpty.h"
#include "..\Settings.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void HTVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 이 항목이 선택되었을 때는 Rename 메뉴 선택 비활성화
	CMenu* pMainFrameMenu = pMainFrame->GetMenu();
	CMenu* pSubMenu = pMainFrameMenu->GetSubMenu(GAMEOBJECT_MENU_INDEX);
	pSubMenu->EnableMenuItem(ID_GAMEOBJECT_RENAME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// CLV 항목 모두 제거
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// CIFV에 빈 폼 뷰 설정
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

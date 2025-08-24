#include "ATVItemFolder.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void ATVItemFolder::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// CLV Ç×¸ñ ¸ðµÎ Á¦°Å
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// CIFV¿¡ ºó Æû ºä ¼³Á¤
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

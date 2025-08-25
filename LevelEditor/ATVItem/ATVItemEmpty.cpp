#include "ATVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void ATVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// CLV Ç×¸ñ ¸ðµÎ Á¦°Å
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// ÀÎ½ºÆåÅÍ Æû ºä¿¡ ºó Æû ºä ¼³Á¤
	pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

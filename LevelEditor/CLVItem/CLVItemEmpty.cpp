#include "CLVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\EmptyInspectorFormView.h"

void CLVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// ÀÎ½ºÆåÅÍ Æû ºä ±³Ã¼
	pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

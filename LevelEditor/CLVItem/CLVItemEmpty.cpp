#include "CLVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\EmptyInspectorFormView.h"

void CLVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// ÄÄÆ÷³ÍÆ® ÀÎ½ºÆåÅÍ Æû ºä ±³Ã¼
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}

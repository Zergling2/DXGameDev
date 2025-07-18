#include "HTVItemRoot.h"
#include "HTVItemEmpty.h"
#include "..\Settings.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"

void HTVItemRoot::OnSelect()
{
	HTVItemEmpty item;
	item.OnSelect();
}

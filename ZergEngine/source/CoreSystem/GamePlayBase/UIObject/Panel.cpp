#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>

using namespace ze;

Panel::Panel(UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(flag, name, UIOBJECT_TYPE::PANEL)
{
}

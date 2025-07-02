#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>

using namespace ze;

Text::Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name, UIOBJECT_TYPE::TEXT)
{
}

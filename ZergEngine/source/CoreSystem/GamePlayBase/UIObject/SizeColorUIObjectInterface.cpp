#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

using namespace ze;

ISizeColorUIObject::ISizeColorUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeUIObject(id, flag, name)
	, m_color(Colors::LightGray)
{
}

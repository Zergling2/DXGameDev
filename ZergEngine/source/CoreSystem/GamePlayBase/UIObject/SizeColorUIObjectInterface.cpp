#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>

using namespace ze;

ISizeColorUIObject::ISizeColorUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type)
	: ISizeUIObject(id, flag, name, type)
	, m_color(Colors::LightGray)
{
}

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>

using namespace ze;

InputField::InputField(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name, UIOBJECT_TYPE::INPUT_FIELD)
	, m_bkColor(Colors::White)
{
}

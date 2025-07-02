#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>

using namespace ze;

constexpr float ROUNDED_PANEL_DEFAULT_RADIUS = 15.0f;

Panel::Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name, UIOBJECT_TYPE::PANEL)
	, m_shape(PANEL_SHAPE::RECTANGLE)
	, m_radius(ROUNDED_PANEL_DEFAULT_RADIUS, ROUNDED_PANEL_DEFAULT_RADIUS)
{
}

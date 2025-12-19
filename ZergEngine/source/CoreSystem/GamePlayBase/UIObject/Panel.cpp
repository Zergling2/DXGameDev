#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>

using namespace ze;

constexpr float ROUNDED_PANEL_DEFAULT_RADIUS = 10.0f;

Panel::Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name)
	, m_shape(PanelShape::RoundedRectangle)
	, m_radius(ROUNDED_PANEL_DEFAULT_RADIUS, ROUNDED_PANEL_DEFAULT_RADIUS)
{
}

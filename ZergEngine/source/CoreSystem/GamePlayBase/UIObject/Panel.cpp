#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>

using namespace ze;

constexpr float PANEL_DEFAULT_SIZE = 400.0f;
constexpr float ROUNDED_PANEL_DEFAULT_RADIUS = 10.0f;

Panel::Panel(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_size(PANEL_DEFAULT_SIZE, PANEL_DEFAULT_SIZE)
	, m_radius(ROUNDED_PANEL_DEFAULT_RADIUS, ROUNDED_PANEL_DEFAULT_RADIUS)
	, m_shape(PanelShape::RoundedRectangle)
{
}

bool Panel::HitTest(const XMFLOAT2& mousePos) const
{
	XMFLOAT2 wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(mousePos, wcp);
}

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeUIObjectInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

using namespace ze;

constexpr uint16_t MIN_UI_WIDTH = 16;
constexpr uint16_t MIN_UI_HEIGHT = 16;

ISizeUIObject::ISizeUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type)
	: IUIObject(id, flag, name, type)
	, m_size(static_cast<FLOAT>(MIN_UI_WIDTH), static_cast<FLOAT>(MIN_UI_HEIGHT))
	, m_halfSize(m_size.x * 0.5f, m_size.y * 0.5f)
{
}

void XM_CALLCONV ISizeUIObject::SetSize(FXMVECTOR size)
{
	XMStoreFloat2(&m_size, size);
	XMStoreFloat2(&m_halfSize, XMVectorMultiply(size, g_XMOneHalf));
}

bool XM_CALLCONV ISizeUIObject::HitTest(FXMVECTOR mousePosition) const
{
	// 현재는 스케일링 없는 모드에서만 동작!!!
	// 스케일링이 일어나는 전체화면 모드에서는 아직 제대로 동작 X

	// 마우스 포인터를 UI의 로컬 스페이스 공간으로 이동시킨 뒤 인바운드 검사를 한다
	XMVECTOR mousePosInButtonLocalSpace = XMVectorSubtract(mousePosition, m_transform.GetWindowsScreenPosition());

	XMVECTOR bound = XMLoadFloat2(&m_halfSize);

	// XMVector2InBounds
	// (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	return XMVector2InBounds(mousePosInButtonLocalSpace, bound);
}

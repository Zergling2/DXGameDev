#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

RectTransform::RectTransform(IUIObject* pUIObject)
    : m_pUIObject(pUIObject)
    , m_pParent(nullptr)
    , m_children()
    , m_position(0.0f, 0.0f)
    , m_ha(HorizontalAnchor::Center)
    , m_va(VerticalAnchor::VCenter)
{
}

bool RectTransform::SetParent(RectTransform* pTransform)
{
    return UIObjectManager::GetInstance()->SetParent(this, pTransform);
}

bool RectTransform::IsDescendantOf(RectTransform* pTransform) const
{
    const RectTransform* pCursor = m_pParent;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParent;
    }

    return false;
}

void RectTransform::GetHCSPosition(XMFLOAT2* pOut) const
{
    XMFLOAT2 base;

    switch (m_ha)
    {
    case HorizontalAnchor::Left:
        base.x = -GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HorizontalAnchor::Center:
        base.x = 0.0f;
        break;
    case HorizontalAnchor::Right:
        base.x = +GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VerticalAnchor::Top:
        base.y = +GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VerticalAnchor::VCenter:
        base.y = 0.0f;
        break;
    case VerticalAnchor::Bottom:
        base.y = -GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    }

    pOut->x = base.x + m_position.x;
    pOut->y = base.y + m_position.y;
}

void RectTransform::GetWinCoordPosition(POINT* pOut) const
{
    POINT base;

    switch (m_ha)
    {
    case HorizontalAnchor::Left:
        base.x = 0;
        break;
    case HorizontalAnchor::Center:
        base.x = GraphicDevice::GetInstance()->GetSwapChainWidth() / 2U;
        break;
    case HorizontalAnchor::Right:
        base.x = GraphicDevice::GetInstance()->GetSwapChainWidth();
        break;
    }

    switch (m_va)
    {
    case VerticalAnchor::Top:
        base.y = 0;
        break;
    case VerticalAnchor::VCenter:
        base.y = GraphicDevice::GetInstance()->GetSwapChainHeight() / 2U;
        break;
    case VerticalAnchor::Bottom:
        base.y = GraphicDevice::GetInstance()->GetSwapChainHeight();
        break;
    }

    pOut->x = base.x + static_cast<LONG>(m_position.x);
    pOut->y = base.y - static_cast<LONG>(m_position.y);    // Windows 좌표계는 모니터 위쪽으로 갈수록 음수 값
}

void RectTransform::GetWinCoordPosition(XMFLOAT2* pOut) const
{
    XMFLOAT2 base;

    switch (m_ha)
    {
    case HorizontalAnchor::Left:
        base.x = 0.0f;
        break;
    case HorizontalAnchor::Center:
        base.x = GraphicDevice::GetInstance()->GetSwapChainHalfWidthFlt();
        break;
    case HorizontalAnchor::Right:
        base.x = GraphicDevice::GetInstance()->GetSwapChainWidthFlt();
        break;
    }

    switch (m_va)
    {
    case VerticalAnchor::Top:
        base.y = 0.0f;
        break;
    case VerticalAnchor::VCenter:
        base.y = GraphicDevice::GetInstance()->GetSwapChainHalfHeightFlt();
        break;
    case VerticalAnchor::Bottom:
        base.y = GraphicDevice::GetInstance()->GetSwapChainHeightFlt();
        break;
    }

    pOut->x = base.x + m_position.x;
    pOut->y = base.y - m_position.y;    // Windows 좌표계는 모니터 위쪽으로 갈수록 음수 값
}

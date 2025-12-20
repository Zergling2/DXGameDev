#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

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

bool RectTransform::SetParent(RectTransform* pTransform)
{
    return UIObjectManager::GetInstance()->SetParent(this, pTransform);
}

bool RectTransform::IsDescendantOf(RectTransform* pTransform) const
{
    const RectTransform* pCursor = m_pParentTransform;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParentTransform;
    }

    return false;
}

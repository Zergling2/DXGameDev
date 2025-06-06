#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

XMVECTOR XM_CALLCONV RectTransform::GetPreNDCPosition() const
{
    XMFLOAT2A basis;

    switch (m_ha)
    {
    case HORIZONTAL_ANCHOR::LEFT:
        basis.x = -Window.GetHalfWidthFloat();
        break;
    case HORIZONTAL_ANCHOR::CENTER:
        basis.x = 0.0f;
        break;
    case HORIZONTAL_ANCHOR::RIGHT:
        basis.x = +Window.GetHalfWidthFloat();
        break;
    }

    switch (m_va)
    {
    case VERTICAL_ANCHOR::TOP:
        basis.y = +Window.GetHalfHeightFloat();
        break;
    case VERTICAL_ANCHOR::VCENTER:
        basis.y = 0.0f;
        break;
    case VERTICAL_ANCHOR::BOTTOM:
        basis.y = -Window.GetHalfHeightFloat();
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2(&m_position));
}

XMVECTOR XM_CALLCONV RectTransform::GetUnityScreenPosition() const
{
    XMFLOAT2A basis;

    switch (m_ha)
    {
    case HORIZONTAL_ANCHOR::LEFT:
        basis.x = 0.0f;
        break;
    case HORIZONTAL_ANCHOR::CENTER:
        basis.x = Window.GetHalfWidthFloat();
        break;
    case HORIZONTAL_ANCHOR::RIGHT:
        basis.x = Window.GetWidthFloat();
        break;
    }

    switch (m_va)
    {
    case VERTICAL_ANCHOR::TOP:
        basis.y = Window.GetHeightFloat();
        break;
    case VERTICAL_ANCHOR::VCENTER:
        basis.y = Window.GetHalfHeightFloat();;
        break;
    case VERTICAL_ANCHOR::BOTTOM:
        basis.y = 0.0f;
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2(&m_position));
}

XMVECTOR XM_CALLCONV RectTransform::GetWindowsScreenPosition() const
{
    XMFLOAT2A basis;
    XMFLOAT2A windowsPosition(m_position.x, -m_position.y);

    switch (m_ha)
    {
    case HORIZONTAL_ANCHOR::LEFT:
        basis.x = 0.0f;
        break;
    case HORIZONTAL_ANCHOR::CENTER:
        basis.x = Window.GetHalfWidthFloat();
        break;
    case HORIZONTAL_ANCHOR::RIGHT:
        basis.x = Window.GetWidthFloat();
        break;
    }

    switch (m_va)
    {
    case VERTICAL_ANCHOR::TOP:
        basis.y = 0.0f;
        break;
    case VERTICAL_ANCHOR::VCENTER:
        basis.y = Window.GetHalfHeightFloat();
        break;
    case VERTICAL_ANCHOR::BOTTOM:
        basis.y = Window.GetHeightFloat();
        break;
    }

    return XMVectorAdd(XMLoadFloat2A(&basis), XMLoadFloat2A(&windowsPosition));
}

bool RectTransform::SetParent(RectTransform* pTransform)
{
    if (m_pUIObject->IsDeferred() || m_pUIObject->IsOnTheDestroyQueue())
        return false;

    // 자기 자신을 부모로 설정하려는 경우
    if (pTransform == this)
        return false;

    // 이미 부모인 경우
    if (m_pParentTransform == pTransform)
        return false;

    // Check cycle
    // pTransform이 이미 나를 조상으로 하고 있는 경우 or 파괴 예정인 오브젝트를 부모로 설정하려는 경우
    // 예외처리 해주지 않으면 RuntimeImpl::Destroy()에서 자식 오브젝트들까지 모두 Destroy 큐에 넣어주는 정책과 싱크가 맞지 않게 된다.
    if (pTransform != nullptr)
        if (pTransform->IsDescendantOf(this) || pTransform->m_pUIObject->IsOnTheDestroyQueue())
            return false;

    // 루트 노드 처리 경우의 수
    // 1. m_pParentTransform != nullptr && pTransform == nullptr (기존에 루트 노드가 아니었고 이제 루트 노드가 되려는 경우)
    // 2. m_pParentTransform != nullptr && pTransform != nullptr (기존에 루트 노드가 아니었고 지금도 루트 노드가 되는 것은 아닌 경우)
    // 3. m_pParentTransform == nullptr && pTransform == nullptr (기존에 루트 노드였고 지금도 루트 노드가 되려는 경우) (이 경우는 함수 진입 직후 예외처리됨)
    // 4. m_pParentTransform == nullptr && pTransform != nullptr (기존에 루트 노드였고 이제 루트 노드가 아니게 되는 경우)

    // 부모가 nullptr이 아니었던 경우에는 기존 부모에서 떠나기 위해 자신의 포인터를 찾아 제거
    if (m_pParentTransform != nullptr)
    {
#if defined(DEBUG) || defined(_DEBUG)
        bool find = false;
#endif
        std::vector<RectTransform*>::const_iterator end = m_pParentTransform->m_children.cend();
        std::vector<RectTransform*>::const_iterator iter = m_pParentTransform->m_children.cbegin();
        while (iter != end)
        {
            if (*iter == this)
            {
                m_pParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
                find = true;
#endif
                break;
            }
            ++iter;
        }
        assert(find == true);

        // 1. m_pParentTransform != nullptr && pTransform == nullptr (기존에 루트 노드가 아니었고 이제 루트 노드가 되려는 경우)
        if (pTransform == nullptr)
        {
            m_pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);
            UIObjectManager.AddPtrToRootVector(m_pUIObject);
        }
        /*
        // 2. m_pParentTransform != nullptr && pTransform != nullptr (기존에 루트 노드가 아니었고 지금도 루트 노드가 되는 것은 아닌 경우)
        else
        {
            // Nothing to do.
        }
        */
    }
    else    // 부모가 nullptr이었던 경우
    {
        // 4. m_pParentTransform == nullptr && pTransform != nullptr 
        // (기존에 루트 노드였고 이제 루트 노드가 아니게 되는 경우)
        if (pTransform != nullptr)
        {
            UIObjectManager.RemovePtrFromRootVector(m_pUIObject);
            m_pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
        }
        /*
        else
        {
            // 함수 진입부에서 m_pParentTransform == m_pTransform 검사에 의해 예외처리됨
        }
        */
    }

    // 부모의 자식 목록을 업데이트
    if (pTransform != nullptr)
        pTransform->m_children.push_back(this);

    // 자식의 부모 포인터를 새로운 부모로 업데이트
    m_pParentTransform = pTransform;

    return true;
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

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

    // �ڱ� �ڽ��� �θ�� �����Ϸ��� ���
    if (pTransform == this)
        return false;

    // �̹� �θ��� ���
    if (m_pParentTransform == pTransform)
        return false;

    // Check cycle
    // pTransform�� �̹� ���� �������� �ϰ� �ִ� ��� or �ı� ������ ������Ʈ�� �θ�� �����Ϸ��� ���
    // ����ó�� ������ ������ RuntimeImpl::Destroy()���� �ڽ� ������Ʈ����� ��� Destroy ť�� �־��ִ� ��å�� ��ũ�� ���� �ʰ� �ȴ�.
    if (pTransform != nullptr)
        if (pTransform->IsDescendantOf(this) || pTransform->m_pUIObject->IsOnTheDestroyQueue())
            return false;

    // ��Ʈ ��� ó�� ����� ��
    // 1. m_pParentTransform != nullptr && pTransform == nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���� ��Ʈ ��尡 �Ƿ��� ���)
    // 2. m_pParentTransform != nullptr && pTransform != nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���ݵ� ��Ʈ ��尡 �Ǵ� ���� �ƴ� ���)
    // 3. m_pParentTransform == nullptr && pTransform == nullptr (������ ��Ʈ ��忴�� ���ݵ� ��Ʈ ��尡 �Ƿ��� ���) (�� ���� �Լ� ���� ���� ����ó����)
    // 4. m_pParentTransform == nullptr && pTransform != nullptr (������ ��Ʈ ��忴�� ���� ��Ʈ ��尡 �ƴϰ� �Ǵ� ���)

    // �θ� nullptr�� �ƴϾ��� ��쿡�� ���� �θ𿡼� ������ ���� �ڽ��� �����͸� ã�� ����
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

        // 1. m_pParentTransform != nullptr && pTransform == nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���� ��Ʈ ��尡 �Ƿ��� ���)
        if (pTransform == nullptr)
        {
            m_pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);
            UIObjectManager.AddPtrToRootVector(m_pUIObject);
        }
        /*
        // 2. m_pParentTransform != nullptr && pTransform != nullptr (������ ��Ʈ ��尡 �ƴϾ��� ���ݵ� ��Ʈ ��尡 �Ǵ� ���� �ƴ� ���)
        else
        {
            // Nothing to do.
        }
        */
    }
    else    // �θ� nullptr�̾��� ���
    {
        // 4. m_pParentTransform == nullptr && pTransform != nullptr 
        // (������ ��Ʈ ��忴�� ���� ��Ʈ ��尡 �ƴϰ� �Ǵ� ���)
        if (pTransform != nullptr)
        {
            UIObjectManager.RemovePtrFromRootVector(m_pUIObject);
            m_pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
        }
        /*
        else
        {
            // �Լ� ���Ժο��� m_pParentTransform == m_pTransform �˻翡 ���� ����ó����
        }
        */
    }

    // �θ��� �ڽ� ����� ������Ʈ
    if (pTransform != nullptr)
        pTransform->m_children.push_back(this);

    // �ڽ��� �θ� �����͸� ���ο� �θ�� ������Ʈ
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

#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TransformManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

Transform::Transform() noexcept
	: IComponent(TransformManager.AssignUniqueId())
    , m_pParentTransform(nullptr)
    , m_children()
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

XMMATRIX Transform::GetWorldTransformMatrix() const noexcept
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return m_pParentTransform->GetWorldTransformMatrix() * this->GetLocalTransformMatrix();
    else
        return this->GetLocalTransformMatrix();
}

XMVECTOR XM_CALLCONV Transform::GetWorldRotation() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return XMQuaternionMultiply(m_pParentTransform->GetWorldRotation(), this->GetLocalRotation());
    else
        return this->GetLocalRotation();
}

XMVECTOR XM_CALLCONV Transform::GetWorldPosition() const
{
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
        return this->GetWorldTransformMatrix().r[3];
    else
        return this->GetLocalPosition();
}

void XM_CALLCONV Transform::RotateAround(XMVECTOR point, XMVECTOR axis, FLOAT angle)
{
    // 1. 회전 후의 새로운 world position 계산
    // 현재 Transform의 월드 위치와 회전 얻기
    XMVECTOR oldWorldPos = GetWorldPosition();

    // point 기준 axis를 회전축으로 회전
    XMVECTOR offset = oldWorldPos - point;

    // 회전 행렬 생성
    XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, angle);

    // 오프셋 회전
    XMVECTOR rotatedOffset = XMVector3Transform(offset, rotationMatrix);

    // 새로운 world position 계산
    XMVECTOR newWorldPosition = point + rotatedOffset;

    // 2. 회전 후의 새로운 world rotation 계산
    // 기존의 world 기준 회전 상태에 RotateAround 함수로 인한 추가적인 회전을 누적시키면 된다.
    
    // 기존 world rotation 얻기 (worldMatrix에서 추출하거나, 직접 계산)
    XMVECTOR oldWorldRotation = this->GetWorldRotation();

    // 회전축 기준으로 회전하는 쿼터니언 생성
    XMVECTOR deltaRotation = XMQuaternionRotationAxis(axis, angle);
    
    // 새로운 world rotation 계산
    XMVECTOR newWorldRotation = XMQuaternionMultiply(deltaRotation, oldWorldRotation);

    // 4. newWorldPos, newWorldRotation을 newLocalPos, newLocalRotation으로 변환
    assert(m_pGameObject != nullptr);

    if (m_pParentTransform != nullptr)
    {
        Transform* pParentTransform = m_pParentTransform;

        // 부모의 월드 매트릭스
        XMMATRIX parentWorldMatrix = pParentTransform->GetWorldTransformMatrix();

        // 부모의 월드 회전 (쿼터니언)
        XMVECTOR parentWorldRotation = pParentTransform->GetWorldRotation();

        // 부모 space 기준으로 변환
        // 위치 변환
        XMMATRIX parentWorldMatrixInv = XMMatrixInverse(nullptr, parentWorldMatrix);
        XMVECTOR newLocalPosition = XMVector3TransformCoord(newWorldPosition, parentWorldMatrixInv);

        // 회전 변환
        XMVECTOR parentWorldRotationInv = XMQuaternionInverse(parentWorldRotation);
        XMVECTOR newLocalRotation = XMQuaternionMultiply(newWorldRotation, parentWorldRotationInv);

        XMStoreFloat3A(&m_position, newLocalPosition);
        XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newLocalRotation));
    }
    else
    {
        // 부모 오브젝트가 없다면 world == local
        XMStoreFloat3A(&m_position, newWorldPosition);
        XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newWorldRotation));
    }
}

bool Transform::SetParent(Transform* pTransform)
{
    // 자기 자신을 부모로 설정하려는 경우
    if (pTransform == this)
        return false;

    // 이미 부모인 경우
    if (m_pParentTransform == pTransform)
        return false;

    // Check cycle
    // pTransform이 이미 나를 조상으로 하고 있는 경우
    if (pTransform != nullptr && pTransform->IsDescendantOf(this))
        return false;

    // 기존 부모에서 떠나는 자식 포인터를 찾아 제거
    if (m_pParentTransform != nullptr)
    {
#if defined(DEBUG) || defined(_DEBUG)
        bool find = false;
#endif
        std::vector<Transform*>::const_iterator end = m_pParentTransform->m_children.cend();
        std::vector<Transform*>::const_iterator iter = m_pParentTransform->m_children.cbegin();
        while (iter != end)
        {
            if ((*iter) == this)
            {
                m_pParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
                find = true;
#endif
                break;
            }
        }
        assert(find == true);
    }

    // 부모의 자식 목록을 업데이트
    if (pTransform != nullptr)
        pTransform->m_children.push_back(this);

    // 자식의 부모 포인터를 업데이트
    m_pParentTransform = pTransform;

    return true;
}

bool Transform::IsDescendantOf(Transform* pTransform) const
{
    const Transform* pCursor = m_pParentTransform;

    while (pCursor != nullptr)
    {
        if (pCursor == pTransform)
            return true;

        pCursor = pCursor->m_pParentTransform;
    }

    return false;
}

IComponentManager* Transform::GetComponentManager() const
{
	return &TransformManager;
}

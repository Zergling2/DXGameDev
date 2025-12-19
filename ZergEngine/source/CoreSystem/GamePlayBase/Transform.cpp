#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>

using namespace ze;

Transform::Transform(GameObject* pGameObject)
    : m_pGameObject(pGameObject)
    , m_pParentTransform(nullptr)
    , m_children()
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

XMMATRIX Transform::GetWorldTransformMatrix() const
{
    XMVECTOR s;
    XMVECTOR r;
    XMVECTOR t;

    this->GetWorldTransform(&s, &r, &t);

    XMMATRIX m = XMMatrixMultiply(XMMatrixScalingFromVector(s), XMMatrixRotationQuaternion(r));
    m.r[3] = XMVectorSetW(t, 1.0f);

    return m;
}

void Transform::GetWorldTransform(XMFLOAT3* pScale, XMFLOAT4* pRotation, XMFLOAT3* pPosition) const
{
    assert(pScale != nullptr);
    assert(pRotation != nullptr);
    assert(pPosition != nullptr);

    XMVECTOR scale;
    XMVECTOR rotation;	// Quaternion
    XMVECTOR translation;
    this->GetWorldTransform(&scale, &rotation, &translation);

    XMStoreFloat3(pScale, scale);
    XMStoreFloat4(pRotation, rotation);
    XMStoreFloat3(pPosition, translation);
}

void Transform::GetWorldTransform(XMFLOAT3A* pScale, XMFLOAT4A* pRotation, XMFLOAT3A* pPosition) const
{
    assert(pScale != nullptr);
    assert(pRotation != nullptr);
    assert(pPosition != nullptr);

    XMVECTOR scale;
    XMVECTOR rotation;	// Quaternion
    XMVECTOR translation;
    this->GetWorldTransform(&scale, &rotation, &translation);

    XMStoreFloat3A(pScale, scale);
    XMStoreFloat4A(pRotation, rotation);
    XMStoreFloat3A(pPosition, translation);
}

void Transform::GetWorldTransform(XMVECTOR* pScale, XMVECTOR* pRotation, XMVECTOR* pPosition) const
{
    assert(pScale != nullptr);
    assert(pRotation != nullptr);
    assert(pPosition != nullptr);

    XMVECTOR scaleL = this->GetScale();
    XMVECTOR rotationL = this->GetRotation();
    XMVECTOR translationL = XMVectorSetW(this->GetPosition(), 1.0f);    // 매우 중요

    if (!m_pParentTransform)
    {
        *pScale = scaleL;
        *pRotation = rotationL;
        *pPosition = translationL;
    }
    else
    {
        // 부모가 있는 경우 부모의 스케일, 회전, 이동을 누적

        // 중요 포인트!
        // 자식의 Translation 성분은 부모의 스케일 및 회전에 영향을 받는다.
        // 자기 자신 로컬 스케일과 회전은 영향을 받지 않는다. (부모의 스케일 및 회전만 적용해야 함!!!)
        // 부모의 스케일 값에 자식의 회전을 곱한 뒤 

        // 자식의 스케일에는 부모의

        XMVECTOR parentScaleW;
        XMVECTOR parentRotationW;
        XMVECTOR parentTranslationW;
        m_pParentTransform->GetWorldTransform(&parentScaleW, &parentRotationW, &parentTranslationW);



        XMVECTOR scaleW = XMVectorMultiply(scaleL, parentScaleW);

        XMVECTOR rotationW = XMQuaternionMultiply(rotationL, parentRotationW);

        // 1. 자식의 이동 성분이 부모의 스케일에 영향을 받는다.
        XMVECTOR scaledTranslationL = XMVectorMultiply(translationL, parentScaleW);
        // 2. 자식의 이동 성분은 부모의 회전에 영향을 받는다.
        XMVECTOR rotatedTranslationL = XMVector3Rotate(scaledTranslationL, parentRotationW);
        // 3. 계산된 자식의 Translation 성분은 부모의 월드 위치 기준 Translation 성분이다. 따라서 더해준다.
        XMVECTOR translationW = XMVectorAdd(rotatedTranslationL, parentTranslationW);

        *pScale = scaleW;
        *pRotation = rotationW;
        *pPosition = translationW;
    }
}

XMVECTOR Transform::GetWorldScale() const
{
    XMVECTOR scaleW;
    XMVECTOR scaleL = this->GetScale();

    if (m_pParentTransform)
        scaleW = XMVectorMultiply(scaleL, m_pParentTransform->GetWorldScale());
    else
        scaleW = scaleL;

    return scaleW;
}

XMVECTOR Transform::GetWorldRotation() const
{
    XMVECTOR rotationW;
    XMVECTOR rotationL = this->GetRotation();

    if (m_pParentTransform)
        rotationW = XMQuaternionMultiply(rotationL, m_pParentTransform->GetWorldRotation());
    else
        rotationW = rotationL;

    return rotationW;
}

XMVECTOR Transform::GetWorldPosition() const
{
    XMVECTOR scaleW;
    XMVECTOR rotationW;
    XMVECTOR positionW;

    this->GetWorldTransform(&scaleW, &rotationW, &positionW);

    return positionW;
}

// void XM_CALLCONV Transform::RotateAround(FXMVECTOR point, FXMVECTOR axis, FLOAT angle)
// {
//     // 1. 회전 후의 새로운 world position 계산
//     // 현재 Transform의 월드 위치와 회전 얻기
//     XMVECTOR oldWorldPos = GetWorldPosition();
// 
//     // point 기준 axis를 회전축으로 회전
//     XMVECTOR offset = oldWorldPos - point;
// 
//     // 회전 행렬 생성
//     XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, angle);
// 
//     // 오프셋 회전
//     XMVECTOR rotatedOffset = XMVector3Transform(offset, rotationMatrix);
// 
//     // 새로운 world position 계산
//     XMVECTOR newWorldPosition = point + rotatedOffset;
// 
//     // 2. 회전 후의 새로운 world rotation 계산
//     // 기존의 world 기준 회전 상태에 RotateAround 함수로 인한 추가적인 회전을 누적시키면 된다.
//     
//     // 기존 world rotation 얻기 (worldMatrix에서 추출하거나, 직접 계산)
//     XMVECTOR oldWorldRotation = this->GetWorldRotation();
// 
//     // 회전축 기준으로 회전하는 쿼터니언 생성
//     XMVECTOR deltaRotation = XMQuaternionRotationAxis(axis, angle);
//     
//     // 새로운 world rotation 계산
//     XMVECTOR newWorldRotation = XMQuaternionMultiply(deltaRotation, oldWorldRotation);
// 
//     // 4. newWorldPos, newWorldRotation을 newLocalPos, newLocalRotation으로 변환
//     assert(m_pGameObject != nullptr);
// 
//     if (m_pParentTransform != nullptr)
//     {
//         const Transform* pParentTransform = m_pParentTransform;
// 
//         // 부모의 월드 매트릭스
//         XMMATRIX parentWorldMatrix = pParentTransform->GetWorldTransformMatrix();
// 
//         // 부모의 월드 회전 (쿼터니언)
//         XMVECTOR parentWorldRotation = pParentTransform->GetWorldRotation();
// 
//         // 부모 space 기준으로 변환
//         // 위치 변환
//         XMMATRIX parentWorldMatrixInv = XMMatrixInverse(nullptr, parentWorldMatrix);
//         XMVECTOR newLocalPosition = XMVector3TransformCoord(newWorldPosition, parentWorldMatrixInv);
//         
//         // 회전 변환
//         XMVECTOR parentWorldRotationInv = XMQuaternionInverse(parentWorldRotation);
//         XMVECTOR newLocalRotation = XMQuaternionMultiply(newWorldRotation, parentWorldRotationInv);
// 
//         XMStoreFloat3A(&m_position, newLocalPosition);
//         XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newLocalRotation));
//     }
//     else
//     {
//         // 부모 오브젝트가 없다면 world == local
//         XMStoreFloat3A(&m_position, newWorldPosition);
//         XMStoreFloat4A(&m_rotation, XMQuaternionNormalize(newWorldRotation));
//     }
// 
// 
// }

bool Transform::SetParent(Transform* pTransform)
{
    return GameObjectManager::GetInstance()->SetParent(this, pTransform);
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

GameObjectHandle Transform::GetChild(uint32_t index)
{
    if (index >= static_cast<uint32_t>(m_children.size()))
        return GameObjectHandle();
    else
        return m_children[index]->m_pGameObject->ToHandle();
}

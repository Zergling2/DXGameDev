#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>

namespace ze
{
    CameraManagerImpl CameraManager;
}

using namespace ze;

CameraManagerImpl::CameraManagerImpl()
{
}

CameraManagerImpl::~CameraManagerImpl()
{
}

void CameraManagerImpl::Init(void* pDesc)
{
}

void CameraManagerImpl::Release()
{
}

void CameraManagerImpl::Update()
{
    // Depth 기준 재정렬
    std::sort(
        m_activeComponents.begin(), m_activeComponents.end(),
        [](const IComponent* pA, const IComponent* pB)
        {
            return static_cast<const Camera*>(pA)->GetDepth() < static_cast<const Camera*>(pB)->GetDepth();
        }
    );
}

void CameraManagerImpl::OnResize()
{
    for (auto pComponent : m_activeComponents)
    {
        Camera* pCamera = static_cast<Camera*>(pComponent);

        pCamera->CreateBufferAndView();
        pCamera->UpdateProjectionMatrix();
        pCamera->UpdateFullbufferViewport();
    }
}

void CameraManagerImpl::AddPtrToActiveVector(IComponent* pComponent)
{
    IComponentManager::AddPtrToActiveVector(pComponent);

    this->Update();
}

void CameraManagerImpl::RemoveDestroyedComponents()
{
    const size_t activeSizeBefore = m_activeComponents.size();
    IComponentManager::RemoveDestroyedComponents();
    const size_t activeSizeAfter = m_activeComponents.size();

    // 파괴된 카메라가 있는 경우
    if (activeSizeAfter < activeSizeBefore)
        this->Update();
}

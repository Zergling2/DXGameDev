#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>

using namespace ze;

CameraManager* CameraManager::s_pInstance = nullptr;

void CameraManager::CreateInstance()
{
    assert(s_pInstance == nullptr);

    s_pInstance = new CameraManager();
}

void CameraManager::DestroyInstance()
{
    assert(s_pInstance != nullptr);

    delete s_pInstance;
    s_pInstance = nullptr;
}

void CameraManager::Update()
{
    // Depth 기준 재정렬
    std::sort(
        m_directAccessGroup.begin(), m_directAccessGroup.end(),
        [](const IComponent* pA, const IComponent* pB)
        {
            return static_cast<const Camera*>(pA)->GetDepth() < static_cast<const Camera*>(pB)->GetDepth();
        }
    );
}

void CameraManager::ResizeBuffer(uint32_t width, uint32_t height)
{
    for (auto pComponent : m_directAccessGroup)
    {
        Camera* pCamera = static_cast<Camera*>(pComponent);

        pCamera->CreateBuffer(width, height);
        pCamera->UpdateProjectionMatrix(width, height);
    }
}

void CameraManager::AddToDirectAccessGroup(IComponent* pComponent)
{
    IComponentManager::AddToDirectAccessGroup(pComponent);

    this->Update();
}

void CameraManager::RemoveDestroyedComponents()
{
    const size_t activeSizeBefore = m_directAccessGroup.size();
    IComponentManager::RemoveDestroyedComponents();
    const size_t activeSizeAfter = m_directAccessGroup.size();

    // 파괴된 카메라가 있는 경우
    if (activeSizeAfter < activeSizeBefore)
        this->Update();
}

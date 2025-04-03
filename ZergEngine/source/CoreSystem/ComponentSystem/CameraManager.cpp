#include <ZergEngine\CoreSystem\ComponentSystem\CameraManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(CameraManager);

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::Init(void* pDesc)
{
}

void CameraManager::Release()
{
}

void CameraManager::Update()
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

void CameraManager::OnResize()
{
    for (auto pComponent : m_activeComponents)
    {
        Camera* pCamera = static_cast<Camera*>(pComponent);

        pCamera->CreateBufferAndView();
        pCamera->UpdateProjectionMatrix();
        pCamera->UpdateFullbufferViewport();
    }
}

ComponentHandle CameraManager::Register(IComponent* pComponent)
{
    ComponentHandle hComponent = IComponentManager::Register(pComponent);

    this->Update();

    return hComponent;
}

void CameraManager::Unregister(IComponent* pComponent)
{
    IComponentManager::Unregister(pComponent);

    this->Update();
}

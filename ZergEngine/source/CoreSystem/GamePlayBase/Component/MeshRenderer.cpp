#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>

using namespace ze;

MeshRenderer::MeshRenderer() noexcept
	: IComponent(MeshRendererManager::GetInstance()->AssignUniqueId())
	, m_castShadows(false)
	, m_receiveShadows(false)
{
}

IComponentManager* MeshRenderer::GetComponentManager() const
{
	return MeshRendererManager::GetInstance();
}

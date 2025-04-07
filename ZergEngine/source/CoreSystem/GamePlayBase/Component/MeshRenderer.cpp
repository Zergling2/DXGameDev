#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\ComponentSystem\MeshRendererManager.h>

using namespace ze;

MeshRenderer::MeshRenderer()
{
	this->SetId(MeshRendererManager.AssignUniqueId());
}

IComponentManager* MeshRenderer::GetComponentManager() const
{
	return &MeshRendererManager;
}

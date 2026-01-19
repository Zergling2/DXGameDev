#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Resource\StaticMesh.h>

using namespace ze;

MeshRenderer::MeshRenderer(GameObject& owner) noexcept
	: IComponent(owner, MeshRendererManager::GetInstance()->AssignUniqueId())
	, m_spMesh()
	, m_materials()
	, m_castShadows(false)
	, m_receiveShadows(false)
{
}

void MeshRenderer::SetMesh(std::shared_ptr<StaticMesh> spMesh)
{
	if (m_spMesh == spMesh)
		return;

	// 1. 새로운 메시 포인터 이동
	m_spMesh = std::move(spMesh);

	// 2. 재질 슬롯을 새로운 메시의 서브셋 개수만큼 할당
	m_materials.clear();
	if (m_spMesh != nullptr)	// 그러나 새로운 메시가 nullptr일 수 있으므로 체크 후 수행
		m_materials.resize(m_spMesh->m_subsets.size());	// subset 개수만큼 재질 슬롯 할당
}

const Material* MeshRenderer::GetMaterialPtr(size_t subsetIndex) const
{
	assert(subsetIndex < m_materials.size());
	return m_materials[subsetIndex].get();
}

std::shared_ptr<Material> MeshRenderer::GetMaterial(size_t subsetIndex) const
{
	if (subsetIndex < m_materials.size())
		return m_materials[subsetIndex];
	else
		return nullptr;
}

bool MeshRenderer::SetMaterial(size_t subsetIndex, std::shared_ptr<Material> spMaterial)
{
	if (m_materials.size() <= subsetIndex)
		return false;

	m_materials[subsetIndex] = std::move(spMaterial);

	return true;
}

IComponentManager* MeshRenderer::GetComponentManager() const
{
	return MeshRendererManager::GetInstance();
}

#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>

using namespace ze;

MeshRenderer::MeshRenderer() noexcept
	: IComponent(MeshRendererManager::GetInstance()->AssignUniqueId())
	, m_spMesh()
	, m_castShadows(false)
	, m_receiveShadows(false)
{
}

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> spMesh)
{
	if (m_spMesh == spMesh)
		return;

	// 1. ���ο� �޽� ������ �̵�
	m_spMesh = std::move(spMesh);

	// 2. ���� ������ ���ο� �޽��� ����� ������ŭ �Ҵ�
	m_materials.clear();
	if (m_spMesh != nullptr)	// �׷��� ���ο� �޽ð� nullptr�� �� �����Ƿ� üũ �� ����
		m_materials.resize(m_spMesh->m_subsets.size());	// subset ������ŭ ���� ���� �Ҵ�
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

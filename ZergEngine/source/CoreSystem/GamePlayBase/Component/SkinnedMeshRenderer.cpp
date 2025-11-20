#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>

using namespace ze;

SkinnedMeshRenderer::SkinnedMeshRenderer() noexcept
	: IComponent(SkinnedMeshRendererManager::GetInstance()->AssignUniqueId())
	, m_spMesh()
	, m_spArmature()
	, m_pCurrAnim(nullptr)
	, m_animSpeed(1.0f)
	, m_animLoop(true)
	, m_animTimeCursor(0.0f)
	, m_materials()
	, m_castShadows(false)
	, m_receiveShadows(false)
{
}

void SkinnedMeshRenderer::SetMesh(std::shared_ptr<SkinnedMesh> spMesh)
{
	m_spMesh = std::move(spMesh);

	if (m_spMesh == nullptr)
		this->StopAnimation();

	// 재질 슬롯을 새로운 메시의 서브셋 개수만큼 할당
	m_materials.clear();
	if (m_spMesh != nullptr)	// 그러나 새로운 메시가 nullptr일 수 있으므로 체크 후 수행
		m_materials.resize(m_spMesh->m_subsets.size());	// subset 개수만큼 재질 슬롯 할당
}

void SkinnedMeshRenderer::SetArmature(std::shared_ptr<Armature> spArmature)
{
	m_spArmature = std::move(spArmature);

	this->StopAnimation();
}

Material* SkinnedMeshRenderer::GetMaterialPtr(size_t subsetIndex) const
{
	assert(subsetIndex < m_materials.size());
	return m_materials[subsetIndex].get();
}

std::shared_ptr<Material> SkinnedMeshRenderer::GetMaterial(size_t subsetIndex) const
{
	if (subsetIndex < m_materials.size())
		return m_materials[subsetIndex];
	else
		return nullptr;
}

bool SkinnedMeshRenderer::SetMaterial(size_t subsetIndex, std::shared_ptr<Material> spMaterial)
{
	if (m_materials.size() <= subsetIndex)
		return false;

	m_materials[subsetIndex] = std::move(spMaterial);

	return true;
}

bool SkinnedMeshRenderer::PlayAnimation(const char* animName, float speed, bool loop, float timeCursor)
{
	if (m_spArmature == nullptr)
		return false;

	const Animation* pAnim = m_spArmature->GetAnimation(animName);
	if (pAnim == nullptr)
		return false;

	const float duration = pAnim->GetDuration();
	if (loop)
		timeCursor = Math::WrapFloat(timeCursor, duration);
	else
		timeCursor = Math::Clamp(timeCursor, 0.0f, duration);

	m_pCurrAnim = pAnim;
	m_animSpeed = speed;
	m_animLoop = loop;
	m_animTimeCursor = timeCursor;

	return true;
}

void SkinnedMeshRenderer::StopAnimation()
{
	m_pCurrAnim = nullptr;
	m_animSpeed = 1.0f;
	m_animLoop = false;
	m_animTimeCursor = 0.0f;
}

IComponentManager* SkinnedMeshRenderer::GetComponentManager() const
{
	return SkinnedMeshRendererManager::GetInstance();
}

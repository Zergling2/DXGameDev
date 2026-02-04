#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>

using namespace ze;

SkinnedMeshRenderer::SkinnedMeshRenderer(GameObject& owner) noexcept
	: IComponent(owner, SkinnedMeshRendererManager::GetInstance()->AssignUniqueId())
	, m_materials()
	, m_castShadows(false)
	, m_receiveShadows(false)
	, m_spMesh()
	, m_spArmature()
	, m_additiveTransform()
	, m_currAnims()
{
}

const Material* SkinnedMeshRenderer::GetMaterialPtr(size_t subsetIndex) const
{
	if (subsetIndex < m_materials.size())
		return m_materials[subsetIndex].get();
	else
		return nullptr;
}

std::shared_ptr<Material> SkinnedMeshRenderer::GetMaterial(size_t subsetIndex) const
{
	if (subsetIndex < m_materials.size())
		return m_materials[subsetIndex];
	else
		return nullptr;
}

bool SkinnedMeshRenderer::SetMaterial(size_t subsetIndex, std::shared_ptr<Material> material)
{
	if (m_materials.size() <= subsetIndex)
		return false;

	m_materials[subsetIndex] = std::move(material);

	return true;
}

void SkinnedMeshRenderer::SetMesh(std::shared_ptr<SkinnedMesh> mesh)
{
	this->StopAnimation();

	m_spMesh = std::move(mesh);

	// 재질 슬롯을 새로운 메시의 서브셋 개수만큼 할당
	m_materials.clear();
	if (m_spMesh != nullptr)	// 그러나 새로운 메시가 nullptr일 수 있으므로 체크 후 수행
		m_materials.resize(m_spMesh->m_subsets.size());	// subset 개수만큼 재질 슬롯 할당
}

void SkinnedMeshRenderer::SetArmature(std::shared_ptr<Armature> armature)
{
	this->StopAnimation();

	m_currAnims.clear();
	m_spArmature = std::move(armature);

	if (m_spArmature)
		m_additiveTransform.resize(m_spArmature->GetBoneCount());
	else
		std::vector<XMFLOAT4X4A> tmp(std::move(m_additiveTransform));	// 행렬 팔레트용 메모리 버퍼 해제
}

bool SkinnedMeshRenderer::PlayAnimation(const std::string& animName, bool loop, float playbackSpeed, float timeCursor)
{
	if (!m_spArmature)
		return false;

	// 지정한 이름을 가진 애니메이션이 없는 경우
	const Animation* pAnim = m_spArmature->GetAnimation(animName);
	if (pAnim == nullptr)
		return false;

	// 본 그룹이 없는 경우
	if (m_spArmature->GetBoneGroups().size() == 0)
		return false;

	const float animDuration = pAnim->GetDuration();
	if (loop)
		timeCursor = Math::WrapFloat(timeCursor, animDuration);
	else
		timeCursor = Math::Clamp(timeCursor, 0.0f, animDuration);

	const auto& boneGroups = m_spArmature->GetBoneGroups();

	PlayingAnimation pa(pAnim, loop, playbackSpeed, timeCursor);
	for (const auto& iter : boneGroups)
		m_currAnims[iter.first] = pa;
	     
	return true;
}

bool SkinnedMeshRenderer::PlayGroupAnimation(const std::string& animName, const std::string& groupName, bool loop, float playbackSpeed, float timeCursor)
{
	if (!m_spArmature)
		return false;

	// 지정한 이름을 가진 애니메이션이 없는 경우
	const Animation* pAnim = m_spArmature->GetAnimation(animName);
	if (pAnim == nullptr)
		return false;

	// 지정한 이름을 가진 그룹이 존재하지 않는 경우
	if (!m_spArmature->GetBoneGroup(groupName))
		return false;

	const float animDuration = pAnim->GetDuration();
	if (loop)
		timeCursor = Math::WrapFloat(timeCursor, animDuration);
	else
		timeCursor = Math::Clamp(timeCursor, 0.0f, animDuration);

	PlayingAnimation pa(pAnim, loop, playbackSpeed, timeCursor);
	m_currAnims[groupName] = pa;

	return true;
}

bool SkinnedMeshRenderer::SetGroupAnimationSpeed(const std::string& groupName, float playbackSpeed)
{
	if (!m_spArmature)
		return false;

	// 지정한 이름을 가진 그룹이 존재하지 않는 경우
	if (!m_spArmature->GetBoneGroup(groupName))
		return false;

	const auto iter = m_currAnims.find(groupName);
	if (iter == m_currAnims.cend())
		return false;

	iter->second.m_playbackSpeed = playbackSpeed;

	return true;
}

bool SkinnedMeshRenderer::SetGroupAnimationTimeCursor(const std::string& groupName, float timeCursor)
{
	if (!m_spArmature)
		return false;

	// 지정한 이름을 가진 그룹이 존재하지 않는 경우
	if (!m_spArmature->GetBoneGroup(groupName))
		return false;

	const auto iter = m_currAnims.find(groupName);
	if (iter == m_currAnims.cend())
		return false;

	const float animDuration = iter->second.m_pAnim->GetDuration();

	if (iter->second.m_loop)
		timeCursor = Math::WrapFloat(timeCursor, animDuration);
	else
		timeCursor = Math::Clamp(timeCursor, 0.0f, animDuration);

	iter->second.m_timeCursor = timeCursor;

	return true;
}

void SkinnedMeshRenderer::StopAnimation()
{
	m_currAnims.clear();
}

void SkinnedMeshRenderer::StopGroupAnimation(const std::string groupName)
{
	m_currAnims.erase(groupName);
}

IComponentManager* SkinnedMeshRenderer::GetComponentManager() const
{
	return SkinnedMeshRendererManager::GetInstance();
}

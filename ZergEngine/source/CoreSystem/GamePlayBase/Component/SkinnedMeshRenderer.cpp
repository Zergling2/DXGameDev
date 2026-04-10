#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\EngineConstants.h>

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

bool SkinnedMeshRenderer::GetBoneTransform(const std::string& boneName, XMFLOAT3& scale, XMFLOAT4& rot, XMFLOAT3& translation) const
{
	if (!m_spArmature)
		return false;

	const auto& pair = m_spArmature->GetBoneIndexMap().find(boneName);
	if (pair == m_spArmature->GetBoneIndexMap().end())
		return false;

	const bone_index_type targetBoneIndex = pair->second;

	BoneTransform boneTransforms[MAX_BONE_COUNT];
	GetBoneTransforms(boneTransforms, _countof(boneTransforms));

	assert(targetBoneIndex < MAX_BONE_COUNT);

	scale = boneTransforms[targetBoneIndex].m_scale;
	rot = boneTransforms[targetBoneIndex].m_rot;
	translation = boneTransforms[targetBoneIndex].m_translation;

	return true;
}

void SkinnedMeshRenderer::GetBoneTransforms(BoneTransform* pArr, size_t len) const
{
	XMFLOAT4X4A MlMp[MAX_BONE_COUNT];
	XMFLOAT4X4A Ma[MAX_BONE_COUNT];
	// 2패스 구조
	// Pass 1.
	// 모든 뼈 각각에 대해 현재 재생중인 애니메이션에 대한 로컬 변환을 구해둔다. 이때 키 프레임의 로컬 변환에 Additive Transform도 성분을 추가해줄 수 있을듯.

	// Assimp로 로드한 키 프레임 = Ml * Mp (리깅 시 애니메이션 프레임에서 뼈의 스케일링 & 회전 & 이동은 Ml * Mp이다.)

	for (size_t i = 0; i < MAX_BONE_COUNT; ++i)	// 애니메이션을 재생중이지 않은 뼈들은 Identity 변환행렬 사용.
		XMStoreFloat4x4A(&MlMp[i], XMMatrixIdentity());

	// 각 본마다 자신이 재생중인 애니메이션 키 프레임을 보간 샘플링 및 변환행렬 저장.
	for (const auto& pair : m_currAnims)
	{
		const BoneAnimation* const pBoneAnimations = pair.second.m_pAnim->GetBoneAnimations();
		const std::vector<bone_index_type>* const pBoneGroup = m_spArmature->GetBoneGroup(pair.first);

		for (const bone_index_type boneIndex : *pBoneGroup)
		{
			XMFLOAT3A scale;
			XMFLOAT4A rotation;
			XMFLOAT3A translation;
			pBoneAnimations[boneIndex].Interpolate(pair.second.m_timeCursor, &scale, &rotation, &translation);

			XMStoreFloat4x4A(
				&MlMp[boneIndex],
				XMMatrixScalingFromVector(XMLoadFloat3A(&scale)) *
				XMMatrixRotationQuaternion(XMLoadFloat4A(&rotation)) *
				XMMatrixTranslationFromVector(XMLoadFloat3A(&translation))
			);
		}
	}

	// Pass 2.
	// 0번 뼈부터 높은 인덱스로 가며 Ma 행렬을 계산한다.
	const BYTE* const pBoneHierarchy = m_spArmature->GetBoneHierarchy();
	const size_t boneCount = m_spArmature->GetBoneCount();

	// Ma 계산
	if (boneCount > 0)	// 먼저 Ma 배열의 0번 인덱스를 채우고 (Ma(0) = Ml(0) * Mp(0))
		XMStoreFloat4x4A(&Ma[0], XMLoadFloat4x4A(&MlMp[0]));
	for (size_t boneIndex = 1; boneIndex < boneCount; ++boneIndex)	// Ma(i) = Ml(i) * Mp(i) * Ma(i - 1)
		XMStoreFloat4x4A(&Ma[boneIndex], XMLoadFloat4x4A(&MlMp[boneIndex]) * XMLoadFloat4x4A(&Ma[pBoneHierarchy[boneIndex]]));

	for (size_t i = 0; i < boneCount && i < len; ++i)
	{
		XMMATRIX finalMatrix = XMLoadFloat4x4A(&Ma[i]);
		XMVECTOR s;
		XMVECTOR r;
		XMVECTOR t;
		XMMatrixDecompose(&s, &r, &t, finalMatrix);

		XMStoreFloat3(&pArr[i].m_scale, s);
		XMStoreFloat4(&pArr[i].m_rot, r);
		XMStoreFloat3(&pArr[i].m_translation, t);
	}
}

void SkinnedMeshRenderer::ComputeFinalTransform(XMFLOAT4X4A* pOut, size_t len) const
{
	XMFLOAT4X4A MlMp[MAX_BONE_COUNT];
	XMFLOAT4X4A Ma[MAX_BONE_COUNT];
	// 2패스 구조
	// Pass 1.
	// 모든 뼈 각각에 대해 현재 재생중인 애니메이션에 대한 로컬 변환을 구해둔다. 이때 키 프레임의 로컬 변환에 Additive Transform도 성분을 추가해줄 수 있을듯.

	// Assimp로 로드한 키 프레임 = Ml * Mp (리깅 시 애니메이션 프레임에서 뼈의 스케일링 & 회전 & 이동은 Ml * Mp이다.)

	for (size_t i = 0; i < MAX_BONE_COUNT; ++i)	// 애니메이션을 재생중이지 않은 뼈들은 Identity 변환행렬 사용.
		XMStoreFloat4x4A(&MlMp[i], XMMatrixIdentity());

	// 각 본마다 자신이 재생중인 애니메이션 키 프레임을 보간 샘플링 및 변환행렬 저장.
	for (const auto& pair : m_currAnims)
	{
		const BoneAnimation* const pBoneAnimations = pair.second.m_pAnim->GetBoneAnimations();
		const std::vector<bone_index_type>* const pBoneGroup = m_spArmature->GetBoneGroup(pair.first);

		for (const bone_index_type boneIndex : *pBoneGroup)
		{
			XMFLOAT3A scale;
			XMFLOAT4A rotation;
			XMFLOAT3A translation;
			pBoneAnimations[boneIndex].Interpolate(pair.second.m_timeCursor, &scale, &rotation, &translation);

			XMStoreFloat4x4A(
				&MlMp[boneIndex],
				XMMatrixScalingFromVector(XMLoadFloat3A(&scale)) *
				XMMatrixRotationQuaternion(XMLoadFloat4A(&rotation)) *
				XMMatrixTranslationFromVector(XMLoadFloat3A(&translation))
			);
		}
	}

	// Pass 2.
	// 0번 뼈부터 높은 인덱스로 가며 Ma 행렬을 계산하고 최종적으로 Mf(i) = MdInv(i) * Ml(i) * Mp(i) * Ma(i-1)를 계산한다.
	const BYTE* const pBoneHierarchy = m_spArmature->GetBoneHierarchy();
	const size_t boneCount = m_spArmature->GetBoneCount();

	// Ma 계산
	if (boneCount > 0)	// 먼저 Ma 배열의 0번 인덱스를 채우고 (Ma(0) = Ml(0) * Mp(0))
		XMStoreFloat4x4A(&Ma[0], XMLoadFloat4x4A(&MlMp[0]));
	for (size_t boneIndex = 1; boneIndex < boneCount; ++boneIndex)	// Ma(i) = Ml(i) * Mp(i) * Ma(i - 1)
		XMStoreFloat4x4A(&Ma[boneIndex], XMLoadFloat4x4A(&MlMp[boneIndex]) * XMLoadFloat4x4A(&Ma[pBoneHierarchy[boneIndex]]));



	// Mf (Final transform) 계산
	const XMFLOAT4X4A* pMdInvArray = m_spArmature->GetMdInvArray();
	for (size_t boneIndex = 0; boneIndex < boneCount && boneIndex < len; ++boneIndex)
		XMStoreFloat4x4A(&pOut[boneIndex], XMLoadFloat4x4A(&pMdInvArray[boneIndex]) * XMLoadFloat4x4A(&Ma[boneIndex]));
}

IComponentManager* SkinnedMeshRenderer::GetComponentManager() const
{
	return SkinnedMeshRendererManager::GetInstance();
}

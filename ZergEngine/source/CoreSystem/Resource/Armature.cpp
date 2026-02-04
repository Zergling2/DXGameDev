#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>

using namespace ze;

Armature::Armature(std::unordered_map<std::string, bone_index_type> boneIndexMap,
	std::unique_ptr<bone_index_type[]> upBoneHierarchy,
	std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray)
	: m_boneIndexMap(std::move(boneIndexMap))
	, m_boneGroup()
	, m_upBoneHierarchy(std::move(upBoneHierarchy))
	, m_upMdInvArray(std::move(upMdInvArray))
	, m_anims()
{
}

bool Armature::CreateBoneGroupByRootBoneName(std::string groupName, const std::string& rootBoneName)
{
	// 이미 동일한 이름의 그룹이 존재하는 경우 실패
	if (m_boneGroup.find(groupName) != m_boneGroup.cend())
		return false;

	std::vector<bone_index_type> group;
	auto found = m_boneIndexMap.find(rootBoneName);
	if (found == m_boneIndexMap.cend())	// 루트 본 검색에 실패한 경우
		return false;

	const bone_index_type rootBoneIndex = found->second;
	group.push_back(rootBoneIndex);

	const uint32_t boneCount = this->GetBoneCount();
	for (uint32_t i = rootBoneIndex + 1; i < boneCount; ++i)
	{
		if (IsDescendantOf(rootBoneIndex, i))
			group.push_back(i);
	}

	// 새 그룹을 추가
	m_boneGroup.insert(std::make_pair(std::move(groupName), std::move(group)));

	return true;
}

bool Armature::CreateBoneGroupByExcludeGroup(std::string groupName, const std::string& excludeGroupName)
{
	auto found = m_boneGroup.find(excludeGroupName);
	if (found == m_boneGroup.cend())
		return false;

	const std::vector<bone_index_type>& excludeBoneGroup = found->second;
	const uint32_t boneCount = this->GetBoneCount();
	std::vector<bone_index_type> group;

	for (bone_index_type i = 0; i < boneCount; ++i)
	{
		bool exclude = false;
		for (bone_index_type bi : excludeBoneGroup)
		{
			if (i == bi)	// 현재 뼈 i가 제외 그룹에 속해있으면
			{
				exclude = true;
				break;
			}
		}

		if (!exclude)
			group.push_back(i);
	}	

	// 새 그룹을 추가
	m_boneGroup.insert(std::make_pair(std::move(groupName), std::move(group)));

	return true;
}

const std::vector<bone_index_type>* Armature::GetBoneGroup(const std::string& groupName)
{
	auto iter = m_boneGroup.find(groupName);
	if (iter == m_boneGroup.cend())
		return nullptr;
	else
		return &iter->second;
}

const Animation* Armature::GetAnimation(const std::string& animName) const
{
	auto iter = m_anims.find(animName);

	if (iter == m_anims.end())
		return nullptr;

	const Animation* pFoundAnimation = iter->second.get();

	return pFoundAnimation;
}

Animation* Armature::AddNewAnimationNode(std::string animName, float duration)
{
	std::unique_ptr<Animation> upNewAnim = std::make_unique<Animation>(*this, duration);

	Animation* pNewAnim = upNewAnim.get();

	auto pair = m_anims.insert(std::make_pair(std::move(animName), std::move(upNewAnim)));
	assert(pair.second == true);		// 중복 키로 인한 삽입 실패 체크

	return pNewAnim;
}

bool Armature::IsDescendantOf(bone_index_type ancestor, bone_index_type descendant) const
{
	if (descendant <= ancestor)	// 동일 뼈 or 조상이 오히려 높은 인덱스를 갖는 경우 (저장 시 높은 인덱스로 갈수록 leaf 노드를 저장했으므로)
		return false;

	bone_index_type cursor = m_upBoneHierarchy[descendant];
	while (cursor > ancestor)
		cursor = m_upBoneHierarchy[cursor];

	return cursor == ancestor;
}

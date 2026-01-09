#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>

using namespace ze;

Armature::Armature(std::unordered_map<std::string, bone_index_type> boneIndexMap,
	std::unique_ptr<bone_index_type[]> upBoneHierarchy,
	std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray)
	: m_boneIndexMap(std::move(boneIndexMap))
	, m_upBoneHierarchy(std::move(upBoneHierarchy))
	, m_upMdInvArray(std::move(upMdInvArray))
	, m_anims()
{
}

const Animation* Armature::GetAnimation(const char* animName) const
{
	auto iter = m_anims.find(animName);

	if (iter == m_anims.end())
		return nullptr;

	const Animation* pFoundAnimation = iter->second.get();

	return pFoundAnimation;
}

Animation* Armature::AddNewAnimationNode(const char* animName, float duration)
{
	std::unique_ptr<Animation> upNewAnim = std::make_unique<Animation>(*this, duration);

	Animation* pNewAnim = upNewAnim.get();

	auto pair = m_anims.insert(std::make_pair(std::string(animName), std::move(upNewAnim)));
	assert(pair.second == true);		// 중복 키로 인한 삽입 실패 체크

	return pNewAnim;
}

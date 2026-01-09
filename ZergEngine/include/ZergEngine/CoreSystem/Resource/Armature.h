#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\BoneIndexType.h>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>

namespace ze
{
	class Animation;

	class Armature
	{
		friend class ResourceLoader;
	public:
		Armature(std::unordered_map<std::string, bone_index_type> boneIndexMap,
			std::unique_ptr<bone_index_type[]> upBoneHierarchy,
			std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray);
		~Armature() = default;

		// 애니메이션 블렌딩을 위해 본을 그룹화합니다.
		// groupName은 그룹 식별 이름을 전달받고 groupRootBoneName은 그룹 내 루트 본 노드의 이름을 전달받습니다.
		bool CreateBoneGroup(const char* groupName, const char* groupRootBoneName);

		uint32_t GetBoneCount() const { return static_cast<uint32_t>(m_boneIndexMap.size()); }

		const std::unordered_map<std::string, bone_index_type>& GetBoneIndexMap() const { return m_boneIndexMap; }

		const bone_index_type* GetBoneHierarchy() const { return m_upBoneHierarchy.get(); }

		const XMFLOAT4X4A* GetMdInvArray() const { return m_upMdInvArray.get(); }

		uint32_t GetAnimationCount() const { return static_cast<uint32_t>(m_anims.size()); }
		const Animation* GetAnimation(const char* animName) const;
	private:
		Animation* AddNewAnimationNode(const char* animName, float duration);
	private:
		std::unordered_map<std::string, bone_index_type> m_boneIndexMap;
		std::unique_ptr<bone_index_type[]> m_upBoneHierarchy;	// 부모 뼈의 인덱스를 담고있는 배열
		std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> m_upMdInvArray;	// Character space -> Bone space 변환 행렬 배열
		std::unordered_map<std::string, std::unique_ptr<Animation>> m_anims;
	};
}

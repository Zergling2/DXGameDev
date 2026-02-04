#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>

namespace ze
{
	using bone_index_type = uint8_t;

	class Animation;

	class Armature
	{
		friend class ResourceLoader;
	public:
		Armature(std::unordered_map<std::string, bone_index_type> boneIndexMap,
			std::unique_ptr<bone_index_type[]> upBoneHierarchy,
			std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray);
		~Armature() = default;

		/**
		* @brief 애니메이션 블렌딩을 위해 본을 그룹화합니다. 이미 동일한 이름의 그룹이 존재할 경우 함수는 실패합니다.
		* @param groupName 생성할 그룹의 이름입니다.
		* @param rootBoneName 그룹을 지을 본들 중에서 최상위 루트 본을 지정합니다.
		* @return 성공 시 true, 실패 시 false를 반환합니다.
		*/
		bool CreateBoneGroupByRootBoneName(std::string groupName, const std::string& rootBoneName);

		/**
		* @brief 애니메이션 블렌딩을 위해 본을 그룹화합니다. 이미 동일한 이름의 그룹이 존재할 경우 함수는 실패합니다.
		* 이 함수는 특히 캐릭터 메쉬에서 상체, 하체를 그룹지을 때 유용합니다. 예를 들면 CreateBoneGroupByRootBoneName 함수로 일반적으로 골반 위의 첫 번째
		* 척추뼈를 지정하여 상체로 그룹화하고, 이 함수를 이용하여 상체 그룹을 제외 그룹으로 지정하여 나머지 뼈들을 하체로 지정하는데 사용할 수 있습니다.
		* @param groupName 생성할 그룹의 이름입니다.
		* @param excludeGroupName 제외할 그룹의 이름입니다.
		* @return 성공 시 true, 실패 시 false를 반환합니다.
		*/
		bool CreateBoneGroupByExcludeGroup(std::string groupName, const std::string& excludeGroupName);

		/**
		* @brief 이 뼈대의 본 갯수를 반환합니다.
		* @return 뼈대에 포함된 본의 갯수.
		*/
		uint32_t GetBoneCount() const { return static_cast<uint32_t>(m_boneIndexMap.size()); }

		const std::unordered_map<std::string, std::vector<bone_index_type>>& GetBoneGroups() const { return m_boneGroup; }

		/**
		* @brief 본 그룹에 포함된 본 인덱스 벡터를 반환.
		* @param groupName 본 그룹 이름
		* @return 본 인덱스 벡터.
		*/
		const std::vector<bone_index_type>* GetBoneGroup(const std::string& groupName);

		/**
		* @brief 이 뼈대의 포함된 애니메이션의 갯수를 반환합니다.
		* @return 뼈대에 포함된 애니메이션의 갯수.
		*/
		uint32_t GetAnimationCount() const { return static_cast<uint32_t>(m_anims.size()); }

		const std::unordered_map<std::string, bone_index_type>& GetBoneIndexMap() const { return m_boneIndexMap; }
		const bone_index_type* GetBoneHierarchy() const { return m_upBoneHierarchy.get(); }
		const XMFLOAT4X4A* GetMdInvArray() const { return m_upMdInvArray.get(); }
		const Animation* GetAnimation(const std::string& animName) const;
	private:
		Animation* AddNewAnimationNode(std::string animName, float duration);
		bool IsDescendantOf(bone_index_type ancestor, bone_index_type descendant) const;
	private:
		std::unordered_map<std::string, bone_index_type> m_boneIndexMap;
		std::unordered_map<std::string, std::vector<bone_index_type>> m_boneGroup;
		std::unique_ptr<bone_index_type[]> m_upBoneHierarchy;	// 부모 뼈의 인덱스를 담고있는 배열
		std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> m_upMdInvArray;	// Character space -> Bone space 변환 행렬 배열
		std::unordered_map<std::string, std::unique_ptr<Animation>> m_anims;
	};
}

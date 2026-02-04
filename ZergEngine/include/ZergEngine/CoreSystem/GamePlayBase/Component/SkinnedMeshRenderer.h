#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace ze
{
	class SkinnedMesh;
	class Armature;
	class Material;
	class Animation;

	struct PlayingAnimation
	{
	public:
		PlayingAnimation()
			: m_pAnim(nullptr)
			, m_loop(false)
			, m_playbackSpeed(1.0f)
			, m_timeCursor(0.0f)
		{
		}
		PlayingAnimation(const Animation* pAnim, bool loop, float playbackSpeed, float timeCursor)
			: m_pAnim(pAnim)
			, m_loop(loop)
			, m_playbackSpeed(playbackSpeed)
			, m_timeCursor(timeCursor)
		{
		}
		PlayingAnimation(const PlayingAnimation&) = default;
	public:
		const Animation* m_pAnim;
		bool m_loop;
		float m_playbackSpeed;
		float m_timeCursor;
	};

	class SkinnedMeshRenderer : public IComponent
	{
		friend class Runtime;
	public:
		static constexpr ComponentType TYPE = ComponentType::SkinnedMeshRenderer;
		static constexpr bool IsCreatable() { return true; }

		SkinnedMeshRenderer(GameObject& owner) noexcept;
		virtual ~SkinnedMeshRenderer() = default;

		virtual ComponentType GetType() const override { return ComponentType::SkinnedMeshRenderer; }

		size_t GetSubsetCount() const { return m_materials.size(); }
		// 범위검사 생략 (엔진 렌더러에서 사용, subsetIndex에 유효하지 않은 인덱스 전달하지 않도록 주의)
		const Material* GetMaterialPtr(size_t subsetIndex) const;

		// 범위검사 포함 (컨텐츠 로직에서 사용 권장)
		std::shared_ptr<Material> GetMaterial(size_t subsetIndex) const;
		bool SetMaterial(size_t subsetIndex, std::shared_ptr<Material> material);

		bool GetCastShadows() const { return m_castShadows; }
		void SetCastShadows(bool b) { m_castShadows = b; }
		bool GetReceiveShadows() const { return m_receiveShadows; }
		void SetReceiveShadows(bool b) { m_receiveShadows = b; }

		const SkinnedMesh* GetMeshPtr() const { return m_spMesh.get(); }
		std::shared_ptr<SkinnedMesh> GetMesh() const { return m_spMesh; }
		void SetMesh(std::shared_ptr<SkinnedMesh> mesh);

		const Armature* GetArmaturePtr() const { return m_spArmature.get(); }
		std::shared_ptr<Armature> GetArmature() const { return m_spArmature; }

		/**
		* @brief 뼈대를 설정합니다. 이 함수를 호출하면 재생중이던 애니메이션이 취소됩니다.
		* 
		* @param armature 설정할 뼈대를 전달합니다.
		*/
		void SetArmature(std::shared_ptr<Armature> armature);

		/**
		* @brief 모든 본 그룹에 대해 지정한 애니메이션을 재생합니다.
		*
		* @param animName 현재 뼈대에 포함된 애니메이션 클립의 이름을 전달합니다.
		* @param loop true를 전달할 시 애니메이션을 반복 재생합니다.
		* @param playbackSpeed 애니메이션 재생 속도 (1.0f = original speed)
		* @param timeCursor 애니메이션의 클립을 시작할 타임라인 시각을 지정합니다.
		* @return 현재 뼈대에 animName으로 전달한 애니메이션 클립이 존재하지 않을 경우 false를 반환합니다.
		*/
		bool PlayAnimation(const std::string& animName, bool loop, float playbackSpeed = 1.0f, float timeCursor = 0.0f);

		/**
		* @brief 특정 본 그룹에 대해 애니메이션을 재생합니다.
		* 
		* @param animName 현재 뼈대에 포함된 애니메이션 클립의 이름을 지정합니다.
		* @param groupName 애니메이션을 재생할 본 그룹의 이름을 지정합니다.
		* @param loop true를 전달할 시 애니메이션을 반복 재생합니다.
		* @param playbackSpeed 애니메이션 재생 속도 (1.0f = original speed)
		* @param timeCursor 애니메이션의 클립을 시작할 타임라인 시각을 지정합니다.
		* @return 현재 뼈대에 animName으로 전달한 애니메이션 클립이 존재하지 않을 경우 false를 반환합니다.
		*/
		bool PlayGroupAnimation(const std::string& animName, const std::string& groupName, bool loop, float playbackSpeed = 1.0f, float timeCursor = 0.0f);

		/**
		* @brief 본 그룹 애니메이션의 재생 속도를 설정합니다.
		*
		* @param groupName 본 그룹의 이름입니다.
		* @param playbackSpeed 새로운 애니메이션 재생 속도입니다.
		* @return 지정한 본 그룹이 애니메이션을 재생 중이지 않을 경우 false를 반환합니다. 재생 속도 변경에 성공하면 true를 반환합니다.
		*/
		bool SetGroupAnimationSpeed(const std::string& groupName, float playbackSpeed);

		/**
		* @brief 본 그룹 애니메이션의 타임라인 커서를 설정합니다.
		*
		* @param groupName 본 그룹의 이름입니다.
		* @param timeCursor 애니메이션 타임라인 커서의 새로운 값입니다.
		* @return 지정한 본 그룹이 애니메이션을 재생 중이지 않을 경우 false를 반환합니다. 타임라인 커서 변경에 성공하면 true를 반환합니다.
		*/
		bool SetGroupAnimationTimeCursor(const std::string& groupName, float timeCursor);

		/**
		* @brief 모든 본 그룹에 대한 현재 재생중인 애니메이션을 중단합니다.
		*/
		void StopAnimation();

		/**
		* @brief 본 그룹에 대한 애니메이션을 중단합니다.
		* 
		* @param groupName 애니메이션을 중단할 본 그룹의 이름을 지정합니다.
		*/
		void StopGroupAnimation(const std::string groupName);

		const std::unordered_map<std::string, PlayingAnimation>& GetCurrentAnims() const { return m_currAnims; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		std::vector<std::shared_ptr<Material>> m_materials;
		bool m_castShadows;
		bool m_receiveShadows;
		std::shared_ptr<SkinnedMesh> m_spMesh;
		std::shared_ptr<Armature> m_spArmature;
		std::vector<XMFLOAT4X4A> m_additiveTransform;	// 추가 변환용 행렬 팔레트
		std::unordered_map<std::string, PlayingAnimation> m_currAnims;	// <본 그룹, PlayingAnimation> Pair
	};
}

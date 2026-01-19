#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>
#include <vector>

namespace ze
{
	class SkinnedMesh;
	class Armature;
	class Material;
	class Animation;

	class SkinnedMeshRenderer : public IComponent
	{
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
		bool SetMaterial(size_t subsetIndex, std::shared_ptr<Material> spMaterial);

		bool GetCastShadows() const { return m_castShadows; }
		void SetCastShadows(bool b) { m_castShadows = b; }
		bool GetReceiveShadows() const { return m_receiveShadows; }
		void SetReceiveShadows(bool b) { m_receiveShadows = b; }

		const SkinnedMesh* GetMeshPtr() const { return m_spMesh.get(); }
		std::shared_ptr<SkinnedMesh> GetMesh() const { return m_spMesh; }
		void SetMesh(std::shared_ptr<SkinnedMesh> spMesh);

		const Armature* GetArmaturePtr() const { return m_spArmature.get(); }
		std::shared_ptr<Armature> GetArmature() const { return m_spArmature; }
		void SetArmature(std::shared_ptr<Armature> spArmature);

		bool PlayAnimation(const char* animName, float speed, bool loop, float timeCursor = 0.0f);

		void StopAnimation();
		void PauseAnimation() { m_pauseAnim = true; }
		void ResumeAnimation() { m_pauseAnim = false; }
		bool IsAnimationPaused() const { return m_pauseAnim; }

		const Animation* GetCurrentAnimationPtr() const { return m_pCurrAnim; }

		float GetAnimationTimeCursor() const { return m_animTimeCursor; }
		void SetAnimationTimeCursor(float cursor) { m_animTimeCursor = cursor; }
		float GetAnimationSpeed() const { return m_animSpeed; }
		void SetAnimationSpeed(float speed) { m_animSpeed = speed; }
		void SetLoopAnimation(bool b) { m_animLoop = b; }
		bool IsLoopAnimation() const { return m_animLoop; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		std::vector<std::shared_ptr<Material>> m_materials;
		bool m_castShadows;
		bool m_receiveShadows;
		bool m_animLoop;
		bool m_pauseAnim;
		std::shared_ptr<SkinnedMesh> m_spMesh;
		std::shared_ptr<Armature> m_spArmature;
		std::vector<XMFLOAT4X4A> m_additiveTransform;	// 추가 변환용 행렬 팔레트
		
		const Animation* m_pCurrAnim;
		float m_animTimeCursor;
		float m_animSpeed;
	};
}

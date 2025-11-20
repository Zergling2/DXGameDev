#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <cstdint>
#include <vector>

namespace ze
{
	class Armature;

	struct ScaleKeyFrame
	{
		float m_time;
		XMFLOAT3 m_value;
	};

	struct RotationKeyFrame
	{
		float m_time;
		XMFLOAT4 m_value;	// Quaternion
	};

	struct PositionKeyFrame
	{
		float m_time;
		XMFLOAT3 m_value;
	};

	class BoneAnimation
	{
	public:
		BoneAnimation() = default;
		~BoneAnimation() = default;

		std::vector<ScaleKeyFrame>& GetScaleKeyFrames() { return m_scaleKeyFrames; }
		std::vector<RotationKeyFrame>& GetRotationKeyFrames() { return m_rotationKeyFrames; }
		std::vector<PositionKeyFrame>& GetPositionKeyFrames() { return m_positionKeyFrames; }

		void Interpolate(float time, XMFLOAT3* pOutScale, XMFLOAT4* pOutRotation, XMFLOAT3* pOutTranslate) const;
	private:
		std::vector<ScaleKeyFrame> m_scaleKeyFrames;
		std::vector<RotationKeyFrame> m_rotationKeyFrames;
		std::vector<PositionKeyFrame> m_positionKeyFrames;
	};

	class Animation
	{
	public:
		Animation(const Armature& armature, float duration);
		~Animation() = default;

		float GetDuration() const { return m_duration; }
		std::vector<BoneAnimation>& GetBoneAnimations() { return m_boneAnimations; }

		// 출력 행렬은 HLSL 전달 시 전치해야 함
		void ComputeFinalTransform(float time, XMFLOAT4X4A* pOutFinalTransform, size_t count) const;
	private:
		const Armature& m_armature;
		float m_duration;		// sec
		std::vector<BoneAnimation> m_boneAnimations;
	};
}

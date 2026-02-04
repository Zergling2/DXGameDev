#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\EngineConstants.h>

using namespace ze;

Animation::Animation(const Armature& armature, float duration)
	: m_armature(armature)
	, m_duration(duration)
	, m_upBoneAnims(std::make_unique<BoneAnimation[]>(armature.GetBoneCount()))
{
}

void BoneAnimation::Interpolate(float time, XMFLOAT3* pOutScale, XMFLOAT4* pOutRotation, XMFLOAT3* pOutTranslate) const
{
	size_t kfc;	// Key frame count
	
	// Scale 보간
	kfc = m_scaleKeyFrames.size();
	if (kfc == 0)
	{
		// 키 프레임이 없는 경우
		*pOutScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}
	else if (kfc == 1)
	{
		*pOutScale = m_scaleKeyFrames[0].m_value;
	}
	else
	{
		size_t intervalIndex = (std::numeric_limits<size_t>::max)();

		for (size_t i = 0; i < kfc; ++i)
		{
			if (time <= m_scaleKeyFrames[i].m_time)
			{
				intervalIndex = i;
				break;
			}
		}

		// 가장 마지막 키 프레임보다도 time 값이 큰 경우 
		if (intervalIndex == (std::numeric_limits<size_t>::max)())
			intervalIndex = kfc;

		if (intervalIndex == 0)
		{
			// 첫 키 프레임 값을 그대로 출력
			*pOutScale = m_scaleKeyFrames[0].m_value;
		}
		else if (intervalIndex == kfc)
		{
			// 마지막 키 프레임 값을 그대로 출력
			*pOutScale = m_scaleKeyFrames[kfc - 1].m_value;
		}
		else
		{
			// 보간
			const ScaleKeyFrame& kfL = m_scaleKeyFrames[intervalIndex - 1];
			const ScaleKeyFrame& kfR = m_scaleKeyFrames[intervalIndex];

			float t = (time - kfL.m_time) / (kfR.m_time - kfL.m_time);
			XMStoreFloat3(pOutScale, XMVectorLerp(XMLoadFloat3(&kfL.m_value), XMLoadFloat3(&kfR.m_value), t));
		}
	}



	// Rotation 보간
	kfc = m_rotationKeyFrames.size();
	if (kfc == 0)
	{
		// 키 프레임이 없는 경우
		*pOutRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (kfc == 1)
	{
		*pOutRotation = m_rotationKeyFrames[0].m_value;
	}
	else
	{
		size_t intervalIndex = (std::numeric_limits<size_t>::max)();

		for (size_t i = 0; i < kfc; ++i)
		{
			if (time <= m_rotationKeyFrames[i].m_time)
			{
				intervalIndex = i;
				break;
			}
		}

		// 가장 마지막 키 프레임보다도 time 값이 큰 경우 
		if (intervalIndex == (std::numeric_limits<size_t>::max)())
			intervalIndex = kfc;

		if (intervalIndex == 0)
		{
			// 첫 키 프레임 값을 그대로 출력
			*pOutRotation = m_rotationKeyFrames[0].m_value;
		}
		else if (intervalIndex == kfc)
		{
			// 마지막 키 프레임 값을 그대로 출력
			*pOutRotation = m_rotationKeyFrames[kfc - 1].m_value;
		}
		else
		{
			// 보간
			const RotationKeyFrame& kfL = m_rotationKeyFrames[intervalIndex - 1];
			const RotationKeyFrame& kfR = m_rotationKeyFrames[intervalIndex];

			float t = (time - kfL.m_time) / (kfR.m_time - kfL.m_time);
			XMStoreFloat4(pOutRotation, XMQuaternionSlerp(XMLoadFloat4(&kfL.m_value), XMLoadFloat4(&kfR.m_value), t));
		}
	}



	// Position 보간
	kfc = m_positionKeyFrames.size();
	if (kfc == 0)
	{
		// 키 프레임이 없는 경우
		// 일반적으로 좋지 않은 시각적 효과를 얻음. 본의 부모 공간 기준 Translation 성분이 Zero이므로 뼈가 부모 공간에 그대로 위치한 셈이 된다.
		// 하지만 값을 출력하지 않을 수는 없으므로 XMFLOAT3 all zero를 내보낸다.
		*pOutTranslate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	else if (kfc == 1)
	{
		*pOutTranslate = m_positionKeyFrames[0].m_value;
	}
	else
	{
		size_t intervalIndex = (std::numeric_limits<size_t>::max)();

		for (size_t i = 0; i < kfc; ++i)
		{
			if (time <= m_positionKeyFrames[i].m_time)
			{
				intervalIndex = i;
				break;
			}
		}

		// 가장 마지막 키 프레임보다도 time 값이 큰 경우 
		if (intervalIndex == (std::numeric_limits<size_t>::max)())
			intervalIndex = kfc;

		if (intervalIndex == 0)
		{
			// 첫 키 프레임 값을 그대로 출력
			*pOutTranslate = m_positionKeyFrames[0].m_value;
		}
		else if (intervalIndex == kfc)
		{
			// 마지막 키 프레임 값을 그대로 출력
			*pOutTranslate = m_positionKeyFrames[kfc - 1].m_value;
		}
		else
		{
			// 보간
			const PositionKeyFrame& kfL = m_positionKeyFrames[intervalIndex - 1];
			const PositionKeyFrame& kfR = m_positionKeyFrames[intervalIndex];

			float t = (time - kfL.m_time) / (kfR.m_time - kfL.m_time);
			XMStoreFloat3(pOutTranslate, XMVectorLerp(XMLoadFloat3(&kfL.m_value), XMLoadFloat3(&kfR.m_value), t));
		}
	}
}

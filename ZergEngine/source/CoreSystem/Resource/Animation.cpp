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

void Animation::ComputeFinalTransform(float time, XMFLOAT4X4A* pOutFinalTransform, size_t count) const
{
	const XMFLOAT4X4A* pMdInvArray = m_armature.GetMdInvArray();

	// Assimp로 로드한 키 프레임 = Ml * Mp (리깅 시 애니메이션 프레임에서 뼈의 스케일링 & 회전 & 이동은 Ml * Mp이다.)
	// const XMFLOAT4X4A* pMpArray = m_armature.GetMpArray();

	const BYTE* pBoneHierarchy = m_armature.GetBoneHierarchy();

	// Ma(i) = Ml(i) * Mp(i) * Ma(i - 1)

	// 1. Ma 팔레트 계산
	XMFLOAT4X4A MaArray[MAX_BONE_COUNT];
	for (size_t currBoneIndex = 0; currBoneIndex < count; ++currBoneIndex)
	{
		const BYTE parentBoneIndex = pBoneHierarchy[currBoneIndex];

		assert(parentBoneIndex <= currBoneIndex);	// 부모 뼈의 인덱스를 담아두었으므로 반드시 충족해야 함

		// 애니메이션 스케일 행렬
		XMFLOAT3A scale;
		XMFLOAT4A rotation;
		XMFLOAT3A translation;
		m_upBoneAnims[currBoneIndex].Interpolate(time, &scale, &rotation, &translation);

		XMMATRIX MlMp = 
			XMMatrixScalingFromVector(XMLoadFloat3A(&scale)) *
			XMMatrixRotationQuaternion(XMLoadFloat4A(&rotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3A(&translation));
		
		XMMATRIX Ma;
		if (parentBoneIndex == currBoneIndex)
		{
			assert(currBoneIndex == 0);
			Ma = MlMp;
		}
		else
		{
			Ma = MlMp * XMLoadFloat4x4A(&MaArray[parentBoneIndex]);
		}

		XMStoreFloat4x4A(&MaArray[currBoneIndex], Ma);
	}

	// 2. Mf 팔레트 계산	(Final transform 행렬)
	// Mf = MdInv(i) * Ma(i)
	for (size_t currBoneIndex = 0; currBoneIndex < count; ++currBoneIndex)
	{
		XMMATRIX Mf = XMLoadFloat4x4A(&pMdInvArray[currBoneIndex]) * XMLoadFloat4x4A(&MaArray[currBoneIndex]);
		XMStoreFloat4x4A(&pOutFinalTransform[currBoneIndex], Mf);
	}
}

void BoneAnimation::Interpolate(float time, XMFLOAT3* pOutScale, XMFLOAT4* pOutRotation, XMFLOAT3* pOutTranslate) const
{
	size_t kfc;	// Key frame count
	
	// Scale 보간
	kfc = m_scaleKeyFrames.size();
	if (kfc == 0)
	{
		// 키 프레임이 0개이면 기본 값을 내보낸다.
		XMStoreFloat3(pOutScale, Vector3::One3());
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
		// 키 프레임이 0개이면 기본 값을 내보낸다.
		XMStoreFloat4(pOutRotation, Quaternion::Identity());
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
		// 키 프레임이 0개이면 기본 값을 내보낸다.
		XMStoreFloat3(pOutTranslate, Vector3::Zero());
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

#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>

namespace ze
{
	class Animation;

	class Armature
	{
	public:
		Armature(BYTE boneCount);
		~Armature();

		Animation* AddNewAnimation(const char* animName, float duration);

		uint32_t GetBoneCount() const { return m_boneCount; }
		void SetBoneHierarchy(const BYTE* pBoneHierarchy);	// Copy data, not reference.
		BYTE* GetBoneHierarchy() const { return m_pBoneHierarchy; }

		XMFLOAT4X4A* GetMdInvArray() const { return m_pMdInvArray; }
		void SetMdInvArray(const XMFLOAT4X4* pMdInvArray);		// Copy data, not reference.
		uint32_t GetAnimationCount() const { return static_cast<uint32_t>(m_anims.size()); }
		const Animation* GetAnimation(const char* animName) const;
	private:
		uint32_t m_boneCount;
		BYTE* m_pBoneHierarchy;
		XMFLOAT4X4A* m_pMdInvArray;		// Character space -> Bone space 변환 행렬 배열
		// XMFLOAT4X4A* m_pMpArray;		// 드레스 포즈에서의 to parent transform 행렬 배열
		std::unordered_map<std::string, std::unique_ptr<Animation>> m_anims;
	};
}

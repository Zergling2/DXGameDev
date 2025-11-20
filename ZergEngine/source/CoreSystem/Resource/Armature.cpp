#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>

using namespace ze;

Armature::Armature(BYTE boneCount)
	: m_boneCount(boneCount)
	, m_pBoneHierarchy(nullptr)
	, m_pMdInvArray(nullptr)
	, m_anims()
{
	assert(boneCount > 0);

	// Bone hierarchy 할당 및 초기화
	m_pBoneHierarchy = new BYTE[m_boneCount];
	memset(m_pBoneHierarchy, 0, sizeof(BYTE) * m_boneCount);


	// 행렬 배열 메모리 공간 할당
	constexpr size_t XMFLOAT4X4A_ALIGNMENT = 16;
	m_pMdInvArray = reinterpret_cast<XMFLOAT4X4A*>(_aligned_malloc(sizeof(XMFLOAT4X4A) * m_boneCount, XMFLOAT4X4A_ALIGNMENT));
}

Armature::~Armature()
{
	delete[] m_pBoneHierarchy;

	/*
	// MdInv & Mp 배열 메모리 해제
	if (m_pMatrixArray)
		_aligned_free(m_pMatrixArray);
	*/

	if (m_pMdInvArray)
		_aligned_free(m_pMdInvArray);
}

Animation* Armature::AddNewAnimation(const char* animName, float duration)
{
	std::unique_ptr<Animation> upNewAnim = std::make_unique<Animation>(*this, duration);

	Animation* pNewAnim = upNewAnim.get();

	auto pair = m_anims.insert(std::make_pair(std::string(animName), std::move(upNewAnim)));
	assert(pair.second == true);		// 중복 키로 인한 삽입 실패 체크

	return pNewAnim;
}

void Armature::SetBoneHierarchy(const BYTE* pBoneHierarchy)
{
	memcpy(m_pBoneHierarchy, pBoneHierarchy, sizeof(BYTE) * m_boneCount);
}

void Armature::SetMdInvArray(const XMFLOAT4X4* pMdInvArray)
{
	memcpy(m_pMdInvArray, pMdInvArray, sizeof(XMFLOAT4X4) * m_boneCount);
}

const Animation* Armature::GetAnimation(const char* animName) const
{
	auto iter = m_anims.find(animName);
	assert(iter != m_anims.end());

	const Animation* pFoundAnimation = iter->second.get();

	return pFoundAnimation;
}

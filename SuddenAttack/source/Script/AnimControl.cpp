#include "AnimControl.h"

using namespace ze;

AnimControl::AnimControl(ze::GameObject& owner)
	: MonoBehaviour(owner)
	, m_hArmsSkinnedMeshRenderer()
	, m_currAnimLifeTime(0.0f)
	, m_currAnimClipName()
{
}

bool AnimControl::PlayAnim(const char* clipName, bool loop, float playbackSpeed, float timeCursor)
{
	SkinnedMeshRenderer* pArmsSkinnedMeshRenderer = m_hArmsSkinnedMeshRenderer.ToPtr();
	if (!pArmsSkinnedMeshRenderer)
		return false;

	/*
	* 
	* 
	* 
	*/
	
	return false;
}

#pragma once

#include <ZergEngine\ZergEngine.h>

class AnimControl : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	AnimControl(ze::GameObject& owner);
	virtual ~AnimControl() = default;

	bool PlayAnim(const char* clipName, bool loop, float playbackSpeed = 1.0f, float timeCursor = 0.0f);
	bool PlayGroupAnim(const char* clipName, const char* groupName, bool loop);
	void SetDefaultAnimClip(const char* clipName) { m_defaultAnimClipName = clipName; }
	const char* GetDefaultAnimClipName() const { return m_defaultAnimClipName.c_str(); }
public:
	ze::ComponentHandle<ze::SkinnedMeshRenderer> m_hArmsSkinnedMeshRenderer;
private:
	float m_currAnimLifeTime;
	std::string m_currAnimClipName;
	std::string m_defaultAnimClipName;
};

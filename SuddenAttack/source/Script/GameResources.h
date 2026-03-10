#pragma once

#include <ZergEngine\ZergEngine.h>

class GameResources : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	GameResources(ze::GameObject& owner);
	virtual ~GameResources() = default;
public:
	ze::Texture2D m_texLoginBgr;
	ze::Texture2D m_texGameListBgr;
};

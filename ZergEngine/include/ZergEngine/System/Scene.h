#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class GameObject;
	class Skybox;
	class Terrain;

	// Scene base.
	class IScene abstract
	{
		friend class SceneManager;
		friend class Graphics;
	public:
		IScene();
		virtual ~IScene();
		std::shared_ptr<GameObject> CreateEmpty(const wchar_t* name);
		void SetSkybox(const std::shared_ptr<Skybox>& skybox);
		bool SetTerrain(const std::shared_ptr<Terrain>& terrain);
		inline sceneId GetId() const { return m_id; }
	private:
		virtual void LoadScene() = 0;
	private:
		sceneId m_id;
		std::shared_ptr<Skybox> m_skybox;
		std::shared_ptr<Terrain> m_terrain;
		std::vector<std::shared_ptr<GameObject>> m_deferredGameObjects;
	};
}

#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class IComponent;
	class Camera;
	class IScript;
	class MeshRenderer;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	class ComponentSystem
	{
		friend class Engine;
		friend class Graphics;
		friend class GameObject;
		friend class SceneManager;
		friend void Destroy(std::shared_ptr<IComponent>& component);
	private:
		static void AddComponentToSystem(std::shared_ptr<IComponent> component);
		static void RemoveComponentFromSystemAndComponentList(std::shared_ptr<IComponent>& component);
		static void RemoveComponentFromSystemOnly(std::shared_ptr<IComponent>& component);
		// RemoveComponentFromSystemAndComponentList와 RemoveComponentFromSystemOnly에서 사용되는 공통 루틴
		static void RemoveComponentFromSystemCommon(std::shared_ptr<IComponent>& component);
	public:
		class CameraManager
		{
			friend class Engine;
			friend class Graphics;
			friend class Camera;
			friend class ComponentSystem;
		private:
			static void Update();		// 파괴된 카메라 컴포넌트 처리, 변경된 뷰포트 처리 등등...
			// RSSetViewport 갱신 등을 위해서...
			static inline void RequestUpdate() { ComponentSystem::CameraManager::s_updateRequired = true; }
			static void OnBeforeResize();
			static void OnResize();
		private:
			static bool s_updateRequired;
			static std::list<std::shared_ptr<Camera>> s_cameras;
		};

		class ScriptManager
		{
			friend class Engine;
			friend class SceneManager;
			friend class IScript;
			friend class ComponentSystem;
		private:
			ScriptManager();
			static void FixedUpdateScripts();
			static void UpdateScripts();
			static void LateUpdateScripts();
		private:
			static std::vector<std::shared_ptr<IScript>> s_scripts;
		};

		class MeshRendererManager
		{
			friend class Graphics;
			friend class MeshRenderer;
			friend class ComponentSystem;
		private:
			static std::list<std::shared_ptr<MeshRenderer>> s_meshRenderers;
		};

		class LightManager
		{
			friend class ComponentSystem;
			friend class Graphics;
		public:
			static size_t GetDirectionalLightCount() { return LightManager::s_dl.size(); }
			static size_t GetPointLightCount() { return LightManager::s_pl.size(); }
			static size_t GetSpotLightCount() { return LightManager::s_sl.size(); }
		private:
			static std::vector<std::shared_ptr<DirectionalLight>> s_dl;
			static std::vector<std::shared_ptr<PointLight>> s_pl;
			static std::vector<std::shared_ptr<SpotLight>> s_sl;
		};
	};
}

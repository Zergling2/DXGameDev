#pragma once

#include <ZergEngine\Common\EngineHeaders.h>
#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\System\Component\Transform.h>

namespace zergengine
{
	class IComponent;

	// GameObject는 반드시 소유중인 컴포넌트의 링크를 끊지 않고 먼저 사라지면 안됨!
	class GameObject
	{
		friend void DontDestroyOnLoad(std::shared_ptr<GameObject>& gameObject);
		friend void Destroy(std::shared_ptr<GameObject>& gameObject);
		friend void Destroy(std::shared_ptr<IComponent>& component);
		friend class IScene;
		friend class SceneManager;
		friend class Graphics;
		friend class Camera;
		friend class ComponentSystem;
	public:
		GameObject(bool isDeferred, bool isTerrain, const wchar_t* name);
		inline bool IsDontDestroyOnLoad() const { return m_dontDestroyOnLoad; }
		inline bool IsDestroyed() const { return m_destroyed; }

		// AddComponent 템플릿 함수
		// template<class ComponentType, typename... Args>
		// typename std::enable_if<!std::is_same<ComponentType, Transform>::value, std::shared_ptr<ComponentType>>::type AddComponent(Args&& ...args);
		template<class ComponentType, typename... Args>
		std::shared_ptr<ComponentType> AddComponent(Args&& ...args);

		inline Transform& GetTransform() { return m_transform; }

		template<class ComponentType>
		std::shared_ptr<ComponentType> GetComponent();
		/*
		template<>
		inline std::shared_ptr<Transform> GetComponent() { return m_transform; }
		*/

		template<class ComponentType>
		std::vector<std::shared_ptr<ComponentType>> GetComponents();
		template<>
		std::vector<std::shared_ptr<Transform>> GetComponents<Transform>() = delete;

		// Return number of components
		template<class ComponentType>
		uint32_t GetComponents(std::shared_ptr<ComponentType> componentArr[], uint32_t len);
		template<>
		uint32_t GetComponents<Transform>(std::shared_ptr<Transform>[], uint32_t) = delete;
	private:
		void DestroyAllComponents();
	private:
		wchar_t m_name[22];
		bool m_isDeferred;
		bool m_dontDestroyOnLoad;
		bool m_destroyed;
		const bool m_isTerrain;
		Transform m_transform;
		std::list<std::shared_ptr<IComponent>> m_components;
	};

	template<class ComponentType, typename ...Args>
	std::shared_ptr<ComponentType> GameObject::AddComponent(Args && ...args)
	{
		if (IsDestroyed())
			return nullptr;

		if (m_isTerrain)
			return nullptr;

		if (!ComponentType::IsCreatable())
			return nullptr;

		std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>(std::forward<Args>(args)...);
		m_components.emplace_back(std::static_pointer_cast<IComponent>(component));
		component->m_pGameObject = this;

		if (!m_isDeferred)
		{
			// 컴포넌트가 std::shared_from_this를 사용하지 않는 방법
			ComponentSystem::AddComponentToSystem(std::static_pointer_cast<IComponent>(component));
		}

		return component;
	}

	template<class ComponentType>
	std::shared_ptr<ComponentType> GameObject::GetComponent()
	{
		if (IsDestroyed())
			return nullptr;

		for (auto& component : m_components)
			if (dynamic_cast<ComponentType*>(component.get()) && !component->IsDestroyed())
				return std::static_pointer_cast<ComponentType>(component);

		return nullptr;
	}

	template<class ComponentType>
	std::vector<std::shared_ptr<ComponentType>> GameObject::GetComponents()
	{
		std::vector<std::shared_ptr<ComponentType>> cv;

		if (IsDestroyed())
			return cv;

		for (auto& component : m_components)
			if (dynamic_cast<ComponentType*>(component.get()) && !component->IsDestroyed())
				cv.emplace_back(std::static_pointer_cast<ComponentType>(component));

		return cv;
	}

	template<class ComponentType>
	uint32_t GameObject::GetComponents(std::shared_ptr<ComponentType> componentArr[], uint32_t len)
	{
		if (IsDestroyed() || len == 0)
			return 0;

		uint32_t count = 0;

		for (auto& component : m_components)
		{
			if (dynamic_cast<ComponentType*>(component.get()) && !component->IsDestroyed())
			{
				componentArr[count++] = std::static_pointer_cast<ComponentType>(component);
				if (count >= len)
					break;
			}
		}

		return count;
	}
}

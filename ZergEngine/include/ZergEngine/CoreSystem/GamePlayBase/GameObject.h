#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;

	enum GAMEOBJECT_FLAG : uint16_t
	{
		GOF_NONE = 0x0000,
		GOF_DONT_DESTROY_ON_LOAD = 0x0001,
		GOF_DEFERRED = 0x0002
	};

	class GameObject
	{
		friend class RuntimeImpl;
		friend class IScene;
		friend class GameObjectManagerImpl;
		friend class SceneManagerImpl;
	private:
		GameObject(bool deferred, PCWSTR name);
	public:
		inline bool IsDontDestroyOnLoad() const { return m_flag & GOF_DONT_DESTROY_ON_LOAD; }

		inline PCWSTR GetName() const { return m_name; }
		inline uint64_t GetId() const { return m_id; }

		inline Transform& GetTransform() { return m_transform; }
		inline const Transform& GetTransform() const { return m_transform; }

		// IScene::OnLoadScene() �Լ� �ȿ����� AddComponent �Լ��� ������Ʈ�� �߰��ϸ� �����մϴ�. ��� �� �Լ��� ����Ͽ��� �մϴ�.
		// �� �Լ��� ��ȯ�� ��ü �����Ϳ� ����� delete�� ȣ���ؼ��� �ȵ˴ϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad�� ȣ����� �ʾҴ� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		template<class ComponentType, typename... Args>
		ComponentType* AddDeferredComponent(Args&& ...args);

		// ��ũ��Ʈ �������� �� �Լ��� ������Ʈ�� �߰��ؾ� �մϴ�. AddDeferredComponent �Լ��� ������Ʈ�� �߰��ϸ� �����մϴ�.
		template<class ComponentType, typename... Args>
		ComponentHandle AddComponent(Args&& ...args);

		template<class ComponentType>
		ComponentHandle GetComponent();
		template<>
		ComponentHandle GetComponent<Transform>() = delete;

		template<class ComponentType>
		std::vector<ComponentHandle> GetComponents();
		template<>
		std::vector<ComponentHandle> GetComponents<Transform>() = delete;

		// Return number of components
		template<class ComponentType>
		uint32_t GetComponents(ComponentHandle componentArr[], uint32_t len);
		template<>
		uint32_t GetComponents<Transform>(ComponentHandle[], uint32_t) = delete;
	private:
		inline bool IsDeferred() const { return m_flag & GOF_DEFERRED; }
		GameObjectHandle ToHandle();
	private:
		Transform m_transform;
		std::list<ComponentHandle> m_components;
		uint64_t m_id;
		uint32_t m_index;
		uint32_t m_activeIndex;
		GAMEOBJECT_FLAG m_flag;
		WCHAR m_name[19];
	};

	template<class ComponentType, typename ...Args>
	ComponentType* GameObject::AddDeferredComponent(Args && ...args)
	{
		ComponentType* pComponent = nullptr;
		
		do
		{
			if (!IsDeferred())
				break;

			pComponent = new(std::nothrow) ComponentType(std::forward<Args>(args)...);
			if (!pComponent)
				break;

			// Scene�� ���۵� �� ��¥ ������Ʈ �ڵ���� ��¥ �ڵ�� �ٲ��ش�.
			ComponentHandle hFakeHandle = ComponentHandle::MakeFakeHandle(pComponent);
			m_components.push_back(hFakeHandle);
		} while (false);

		return pComponent;
	}

	template<class ComponentType, typename ...Args>
	ComponentHandle GameObject::AddComponent(Args && ...args)
	{
		ComponentHandle hComponent;	// Invalid handle

		do
		{
			if (IsDeferred())
				break;

			GameObjectHandle hMyHandle = this->ToHandle();
			assert(hMyHandle.IsValid());

			ComponentType* pComponent = new(std::nothrow) ComponentType(std::forward<Args>(args)...);
			if (!pComponent)
				break;

			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			hComponent = pComponentManager->Register(pComponent);

			if (!hComponent.IsValid())
			{
				delete pComponent;
				break;
			}

			pComponent->m_hGameObject = hMyHandle;
			m_components.push_back(hComponent);

		} while (false);

		return hComponent;
	}

	template<class ComponentType>
	ComponentHandle GameObject::GetComponent()
	{
		ComponentHandle hComponent;

		do
		{
			if (IsDeferred())
				break;

			for (auto& h : m_components)
			{
				IComponent* pComponent = h.ToPtr();
				assert(pComponent != nullptr);

				if (dynamic_cast<ComponentType*>(pComponent))
				{
					hComponent = h;
					break;
				}
			}
		} while (false);

		return hComponent;
	}

	template<class ComponentType>
	std::vector<ComponentHandle> GameObject::GetComponents()
	{
		std::vector<ComponentHandle> hv;

		do
		{
			if (IsDeferred())
				break;

			for (auto& h : m_components)
			{
				IComponent* pComponent = h.ToPtr();
				assert(pComponent != nullptr);

				if (dynamic_cast<ComponentType*>(pComponent))
					hv.push_back(h);
			}
		} while (false);

		return hv;
	}

	template<class ComponentType>
	uint32_t GameObject::GetComponents(ComponentHandle componentArr[], uint32_t len)
	{
		uint32_t count = 0;

		do
		{
			if (this->IsDeferred() || len == 0)
				break;

			for (auto& h : m_components)
			{
				IComponent* pComponent = h.ToPtr();
				assert(pComponent != nullptr);

				if (dynamic_cast<ComponentType*>(pComponent))
				{
					componentArr[count++] = h;
					if (count >= len)
						break;
				}
			}
		} while (false);

		return count;
	}
}

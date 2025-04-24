#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;
	class Transform;

	enum GAMEOBJECT_FLAG : uint16_t
	{
		GOF_NONE					= 0,
		GOF_STATIC					= 1 << 0,
		GOF_DONT_DESTROY_ON_LOAD	= 1 << 1,
		GOF_DEFERRED				= 1 << 2,
		GOF_ACTIVE					= 1 << 3,
		GOF_ON_DESTROY_QUEUE		= 1 << 4,
	};

	class GameObject
	{
		friend class RuntimeImpl;
		friend class IScene;
		friend class GameObjectManagerImpl;
		friend class SceneManagerImpl;
		friend class IComponentManager;
		friend class RendererImpl;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class SkyboxEffect;
	private:
		GameObject(GAMEOBJECT_FLAG flag, PCWSTR name);
	public:
		bool IsDontDestroyOnLoad() const { return m_flag & GOF_DONT_DESTROY_ON_LOAD; }

		PCWSTR GetName() const { return m_name; }
		uint64_t GetId() const { return m_id; }

		void SetActive(bool active);
		bool IsActive() { return m_flag & GOF_ACTIVE; }

		// IScene::OnLoadScene() �Լ� �ȿ����� AddComponent �Լ��� ������Ʈ�� �߰��ϸ� �����մϴ�. ��� �� �Լ��� ����Ͽ��� �մϴ�.
		// �� �Լ��� ��ȯ�� ��ü �����Ϳ� ����� delete�� ȣ���ؼ��� �ȵ˴ϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad�� ȣ����� �ʾҴ� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		template<class ComponentType, typename... Args>
		ComponentType* AddDeferredComponent(Args&& ...args);

		// ��ũ��Ʈ �������� �� �Լ��� ������Ʈ�� �߰��ؾ� �մϴ�. AddDeferredComponent �Լ��� ������Ʈ�� �߰��ϸ� �����մϴ�.
		template<class ComponentType, typename... Args>
		ComponentHandle<ComponentType> AddComponent(Args&& ...args);

		// IScene Ŭ������ OnLoadScene() �Լ��� �������̵��� �Լ� �������� ����ؾ� �մϴ�. �׷��� ������ nullptr�� ��ȯ�մϴ�.
		template<class ComponentType>
		ComponentType* GetComponentRawPtr();

		template<class ComponentType>
		ComponentHandle<ComponentType> GetComponent();

		template<class ComponentType>
		std::vector<ComponentHandle<ComponentType>> GetComponents();

		// Return number of components
		template<class ComponentType>
		uint32_t GetComponents(ComponentHandle<ComponentType> componentArr[], uint32_t len);

		GameObjectHandle ToHandle() const;

		bool SetParent(GameObject* pGameObject);
		bool IsDescendant(GameObject* pGameObject) const;
		bool IsDeferred() const { return m_flag & GOF_DEFERRED; }
		GAMEOBJECT_FLAG GetFlag() const { return m_flag; }

		XMMATRIX CalcWorldTransformMatrix() const noexcept;
		XMMATRIX CalcLocalTransformMatrix() const noexcept;
	private:
		void SetOnDestroyQueueFlag() { m_flag = static_cast<GAMEOBJECT_FLAG>(m_flag | GOF_ON_DESTROY_QUEUE); }
		bool IsOnTheDestroyQueue() const { return m_flag & GOF_ON_DESTROY_QUEUE; }
	private:
		GameObject* m_pParent;
		std::list<GameObject*> m_children;
		std::list<IComponent*> m_components;
		Transform* m_pTransform;
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_activeIndex;
		GAMEOBJECT_FLAG m_flag;
		WCHAR m_name[23];
	};

	template<class ComponentType, typename ...Args>
	ComponentType* GameObject::AddDeferredComponent(Args && ...args)
	{
		ComponentType* pComponent = nullptr;
		
		do
		{
			if (!IsDeferred())
				break;

			if (!ComponentType::IsCreatable())
				break;

			pComponent = new ComponentType(std::forward<Args>(args)...);

			pComponent->m_pGameObject = this;
			m_components.push_back(pComponent);
		} while (false);

		return pComponent;
	}

	template<class ComponentType, typename ...Args>
	ComponentHandle<ComponentType> GameObject::AddComponent(Args && ...args)
	{
		ComponentHandle<ComponentType> hComponent;	// Invalid handle

		do
		{
			if (IsDeferred())
				break;

			if (!ComponentType::IsCreatable())
				break;

			ComponentType* pComponent = new ComponentType(std::forward<Args>(args)...);

			pComponent->m_pGameObject = this;
			m_components.push_back(pComponent);

			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			assert(pComponentManager != nullptr);

			hComponent = pComponentManager->Register(pComponent);
			assert(hComponent.IsValid() == true);
		} while (false);

		return hComponent;
	}

	template<class ComponentType>
	ComponentType* GameObject::GetComponentRawPtr()
	{
		ComponentType* pFind = nullptr;

		do
		{
			if (!this->IsDeferred())
				break;

			for (auto pComponent : m_components)
			{
				assert(pComponent != nullptr);
				if (dynamic_cast<ComponentType*>(pComponent))
				{
					pFind = static_cast<ComponentType*>(pComponent);
					break;
				}
			}
		} while (false);

		return pFind;
	}

	template<class ComponentType>
	ComponentHandle<ComponentType> GameObject::GetComponent()
	{
		ComponentHandle<ComponentType> hComponent;

		for (auto pComponent : m_components)
		{
			assert(pComponent != nullptr);
			if (dynamic_cast<ComponentType*>(pComponent))
			{
				hComponent = pComponent->ToHandleBase();
				assert(hComponent.IsValid() == true);
				break;
			}
		}

		return hComponent;
	}

	template<class ComponentType>
	std::vector<ComponentHandle<ComponentType>> GameObject::GetComponents()
	{
		std::vector<ComponentHandle<ComponentType>> hv;

		for (auto pComponent : m_components)
		{
			assert(pComponent != nullptr);
			assert(pComponent->ToHandleBase().IsValid());
			if (dynamic_cast<ComponentType*>(pComponent))
			{
				hv.emplace_back(pComponent->ToHandleBase());
				assert(hv[hv.size() - 1].IsValid() == true);
			}
		}

		return hv;
	}

	template<class ComponentType>
	uint32_t GameObject::GetComponents(ComponentHandle<ComponentType> componentArr[], uint32_t len)
	{
		if (len == 0)
			break;

		uint32_t count = 0;

		for (auto pComponent : m_components)
		{
			assert(pComponent != nullptr);
			if (dynamic_cast<ComponentType*>(pComponent))
			{
				componentArr[count++] = pComponent->ToHandleBase();
				assert(componentArr[count - 1].IsValid() == true);
				if (count >= len)
					break;
			}
		}

		return count;
	}
}

#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	using GameObjectFlagType = uint16_t;

	enum GAMEOBJECT_FLAG : GameObjectFlagType
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
		friend class Transform;
		friend class Camera;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class SkyboxEffect;
		friend class IComponent;
	private:
		GameObject(GameObjectFlagType flag, PCWSTR name);
	public:
		// ���� �����ϴ� Ȱ��ȭ�� ���� ������Ʈ�� �̸����� �˻��Ͽ� �ڵ��� ��ȯ�մϴ�.
		static GameObjectHandle Find(PCWSTR name);
		static GameObjectHandle Find(const std::wstring& name) { GameObject::Find(name.c_str()); }
	public:
		bool IsDontDestroyOnLoad() const { return m_flag & GOF_DONT_DESTROY_ON_LOAD; }

		PCWSTR GetName() const { return m_name; }
		uint64_t GetId() const { return m_id; }

		void SetActive(bool active);
		bool IsActive() { return m_flag & GOF_ACTIVE; }

		// IScene::OnLoadScene() �Լ� �������� GameObject �� Component�� ������ �����ϸ� Destroy�� ������ �ʽ��ϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad�� ȣ����� �ʾҴ� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		template<class ComponentType, typename... Args>
		ComponentHandle<ComponentType> AddComponent(Args&& ...args);

		template<class ComponentType>
		ComponentHandle<ComponentType> GetComponent();

		template<class ComponentType>
		std::vector<ComponentHandle<ComponentType>> GetComponents();

		// Return number of components
		template<class ComponentType>
		uint32_t GetComponents(ComponentHandle<ComponentType> componentArr[], uint32_t len);

		GameObjectHandle ToHandle() const;
	private:
		bool IsDeferred() const { return m_flag & GOF_DEFERRED; }
		void OnFlag(GameObjectFlagType flag) { m_flag |= flag; }
		void OffFlag(GameObjectFlagType flag) { m_flag &= ~flag; }
		bool IsOnTheDestroyQueue() const { return m_flag & GOF_ON_DESTROY_QUEUE; }
	public:
		Transform m_transform;
	private:
		std::list<IComponent*> m_components;

		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_activeIndex;
		GameObjectFlagType m_flag;
		WCHAR m_name[23];
	};

	template<class ComponentType, typename ...Args>
	ComponentHandle<ComponentType> GameObject::AddComponent(Args && ...args)
	{
		ComponentHandle<ComponentType> hComponent;	// Invalid handle

		do
		{
			if (!ComponentType::IsCreatable())
				break;

			IComponent* pComponent = new ComponentType(std::forward<Args>(args)...);
			pComponent->m_pGameObject = this;
			m_components.push_back(pComponent);

			if (!this->IsActive())
				pComponent->OffFlag(CF_ENABLED);

			IComponentManager* pComponentManager = pComponent->GetComponentManager();

			hComponent = pComponentManager->RegisterToHandleTable(pComponent);
			assert(hComponent.IsValid());

			// ������ ���ӿ�����Ʈ�� �ƴ� ��쿡�� �ٷ� ������ Ȱ��ȭ
			if (!this->IsDeferred())
				pComponentManager->AddPtrToActiveVector(pComponent);
		} while (false);

		return hComponent;
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

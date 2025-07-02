#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\ComponentManagerInterface.h>

namespace ze
{
	using GameObjectFlagType = uint16_t;
	using goft = GameObjectFlagType;

	enum class GAMEOBJECT_FLAG : goft
	{
		NONE					= 0,

		STATIC					= 1 << 0,
		DONT_DESTROY_ON_LOAD	= 1 << 1,
		PENDING					= 1 << 2,
		ACTIVE					= 1 << 3,
		ON_DESTROY_QUEUE		= 1 << 4,
	};

	class GameObject
	{
		friend class Runtime;
		friend class GameObjectManager;
		friend class SceneManager;
		friend class IScene;
		friend class IComponentManager;
		friend class IComponent;
		friend class Renderer;
		friend class Transform;
		friend class Camera;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class SkyboxEffect;
	private:
		GameObject(uint64_t id, GAMEOBJECT_FLAG flag, PCWSTR name);
	public:
		// ���� �����ϴ� Ȱ��ȭ�� ���� ������Ʈ�� �̸����� �˻��Ͽ� �ڵ��� ��ȯ�մϴ�.
		static GameObjectHandle Find(PCWSTR name);
		static GameObjectHandle Find(const std::wstring& name) { GameObject::Find(name.c_str()); }
	public:
		void DontDestroyOnLoad();
		void Destroy();
		void Destroy(float delay);
		bool IsDontDestroyOnLoad() const { return static_cast<goft>(m_flag) & static_cast<goft>(GAMEOBJECT_FLAG::DONT_DESTROY_ON_LOAD); }
		bool IsActive() const { return static_cast<goft>(m_flag) & static_cast<goft>(GAMEOBJECT_FLAG::ACTIVE); }

		PCWSTR GetName() const { return m_name; }
		uint64_t GetId() const { return m_id; }

		void SetActive(bool active);

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

		const GameObjectHandle ToHandle() const;
		bool IsPending() const { return static_cast<goft>(m_flag) & static_cast<goft>(GAMEOBJECT_FLAG::PENDING); }
		bool IsOnTheDestroyQueue() const { return static_cast<goft>(m_flag) & static_cast<goft>(GAMEOBJECT_FLAG::ON_DESTROY_QUEUE); }
	private:
		void OnFlag(GAMEOBJECT_FLAG flag) { m_flag = static_cast<GAMEOBJECT_FLAG>(static_cast<goft>(m_flag) | static_cast<goft>(flag)); }
		void OffFlag(GAMEOBJECT_FLAG flag) { m_flag = static_cast<GAMEOBJECT_FLAG>(static_cast<goft>(m_flag) & ~static_cast<goft>(flag)); }

		ComponentHandleBase AddComponentImpl(IComponent* pComponent);
	public:
		Transform m_transform;
	private:
		std::list<IComponent*> m_components;

		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_groupIndex;
		GAMEOBJECT_FLAG m_flag;
		WCHAR m_name[27];
	};

	template<class ComponentType, typename ...Args>
	ComponentHandle<ComponentType> GameObject::AddComponent(Args && ...args)
	{
		ComponentHandle<ComponentType> hComponent;	// Invalid handle

		do
		{
			if (!ComponentType::IsCreatable())
				break;

			if (this->IsOnTheDestroyQueue())
				break;

			IComponent* pComponent = new ComponentType(std::forward<Args>(args)...);
			return this->AddComponentImpl(pComponent);
		} while (false);

		return hComponent;
	}

	template<class ComponentType>
	ComponentHandle<ComponentType> GameObject::GetComponent()
	{
		ComponentHandle<ComponentType> hComponent;

		do
		{
			if (this->IsOnTheDestroyQueue())
				break;

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

		} while (false);

		return hComponent;
	}

	template<class ComponentType>
	std::vector<ComponentHandle<ComponentType>> GameObject::GetComponents()
	{
		std::vector<ComponentHandle<ComponentType>> hv;

		do
		{
			if (this->IsOnTheDestroyQueue())
				break;

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
		} while (false);

		return hv;
	}

	template<class ComponentType>
	uint32_t GameObject::GetComponents(ComponentHandle<ComponentType> componentArr[], uint32_t len)
	{
		if (len == 0)
			return;

		if (this->IsOnTheDestroyQueue())
			return;

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

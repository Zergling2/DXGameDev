#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentType.h>

namespace ze
{
	class GameObject;
	class IComponent;
	class IComponentManager;
	class IUIObject;

	constexpr uint64_t INVALID_ID = std::numeric_limits<uint64_t>::max();

	class ToPtrHelper
	{
	public:
		static IComponent* ToComponentPtrImpl(COMPONENT_TYPE type, uint32_t tableIndex, uint64_t id);
	};

	class GameObjectHandle
	{
		friend class Runtime;
		friend class GameObjectManager;
		friend class GameObject;
	public:
		GameObjectHandle() noexcept
			: m_tableIndex(0)	// Ŀ�Ե��� ���� ���� ������ �����ϱ� ���� 0�� �ε����� �ʱ�ȭ
			, m_id(INVALID_ID)
		{
		}
	private:
		GameObjectHandle(uint32_t index, uint64_t id) noexcept
			: m_tableIndex(index)
			, m_id(id)
		{
		}
	public:
		GameObjectHandle(const GameObjectHandle& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
		}
		GameObjectHandle(GameObjectHandle&& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
			other.m_tableIndex = 0;
			other.m_id = INVALID_ID;
		}
		GameObjectHandle& operator=(const GameObjectHandle& other) noexcept
		{
			m_tableIndex = other.m_tableIndex;
			m_id = other.m_id;

			return *this;
		}
		~GameObjectHandle() = default;

		GameObject* ToPtr() const;	// index�� ���� nullptr�̸� �ı��� ��, �ƴ϶�� m_id�� ��ü�� id ��
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		bool IsValid() const { return ToPtr() != nullptr; }

		uint32_t GetIndex() const { return m_tableIndex; }
		uint64_t GetId() const { return m_id; }
	private:
		uint32_t m_tableIndex;
		// uint32_t m_reserved;
		uint64_t m_id;
	};

	class UIObjectHandle
	{
		friend class UIObjectManager;
		friend class IUIObject;
	public:
		UIObjectHandle() noexcept
			: m_tableIndex(0)
			, m_id(INVALID_ID)
		{
		}
	private:
		UIObjectHandle(uint32_t index, uint64_t id) noexcept
			: m_tableIndex(index)
			, m_id(id)
		{
		}
	public:
		UIObjectHandle(const UIObjectHandle& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
		}
		UIObjectHandle(UIObjectHandle&& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
			other.m_tableIndex = 0;
			other.m_id = INVALID_ID;
		}
		UIObjectHandle& operator=(const UIObjectHandle& other) noexcept
		{
			m_tableIndex = other.m_tableIndex;
			m_id = other.m_id;

			return *this;
		}
		~UIObjectHandle() = default;

		uint32_t GetIndex() const { return m_tableIndex; }
		uint64_t GetId() const { return m_id; }

		IUIObject* ToPtr() const;
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		bool IsValid() const { return ToPtr() != nullptr; }
	private:
		uint32_t m_tableIndex;
		// uint32_t m_reserved;
		uint64_t m_id;
	};

	class ComponentHandleBase
	{
		friend class IComponentManager;
		friend class IComponent;
	public:
		ComponentHandleBase() noexcept
			: m_tableIndex(0)
			, m_id(INVALID_ID)
		{
		}
	private:
		ComponentHandleBase(uint32_t index, uint64_t id) noexcept
			: m_tableIndex(index)
			, m_id(id)
		{
		}
	public:
		ComponentHandleBase(const ComponentHandleBase& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
		}
		ComponentHandleBase(ComponentHandleBase&& other) noexcept
			: m_tableIndex(other.m_tableIndex)
			, m_id(other.m_id)
		{
			other.m_tableIndex = 0;
			other.m_id = INVALID_ID;
		}
		ComponentHandleBase& operator=(const ComponentHandleBase& other) noexcept
		{
			m_tableIndex = other.m_tableIndex;
			m_id = other.m_id;

			return *this;
		}
		~ComponentHandleBase() = default;	// �����Լ��� ���� X. ��ü ���� ���� (ComponentHandleBase*�� ������ �� ����)

		uint32_t GetIndex() const { return m_tableIndex; }
		uint64_t GetId() const { return m_id; }
	protected:
		uint32_t m_tableIndex;
		// uint32_t m_reserved;
		uint64_t m_id;
	};

	template<typename T>
	class ComponentHandle : public ComponentHandleBase
	{
	public:
		ComponentHandle() noexcept
			: ComponentHandleBase()
		{
		}
		ComponentHandle(const ComponentHandleBase& base) noexcept
			: ComponentHandleBase(base)
		{
		}
		ComponentHandle(ComponentHandleBase&& base) noexcept
			: ComponentHandleBase(base)
		{
		}
		ComponentHandle& operator=(const ComponentHandleBase& other) noexcept
		{
			ComponentHandleBase::operator=(other);

			return *this;
		}
		~ComponentHandle() = default;

		T* ToPtr() const;
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		bool IsValid() const { return ToPtr() != nullptr; }
	};

	template<typename T>
	T* ComponentHandle<T>::ToPtr() const
	{
		return static_cast<T*>(ToPtrHelper::ToComponentPtrImpl(T::TYPE, m_tableIndex, m_id));
	}

	inline bool operator==(const GameObjectHandle& hA, const GameObjectHandle& hB)
	{
		return
			hA.GetIndex() == hB.GetIndex() &&
			hA.GetId() == hB.GetId();
	}

	template<typename T>
	inline bool operator==(const ComponentHandle<T>& hA, const ComponentHandle<T>& hB)
	{
		return
			hA.GetIndex() == hB.GetIndex() &&
			hA.GetId() == hB.GetId();
	}
}

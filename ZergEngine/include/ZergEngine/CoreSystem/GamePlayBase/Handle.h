#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentType.h>

namespace ze
{
	class GameObject;
	class IComponent;
	class Canvas;
	class ICanvasItem;

	constexpr uint64_t INVALID_ID = std::numeric_limits<uint64_t>::max();

	class HandleHelper
	{
	public:
		static IComponent* ToPtrImpl(COMPONENT_TYPE type, uint32_t tableIndex, uint64_t id);
	};

	class GameObjectHandle
	{
		friend class RuntimeImpl;
		friend class GameObjectManagerImpl;
		friend class GameObject;
	public:
		GameObjectHandle() noexcept
			: m_tableIndex(0)
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

		GameObject* ToPtr() const;	// index에 가서 nullptr이면 파괴된 것, 아니라면 m_id와 객체의 id 비교
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		bool IsValid() const { return ToPtr() != nullptr; }

		uint32_t GetIndex() const { return m_tableIndex; }
		uint64_t GetId() const { return m_id; }
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
		~ComponentHandleBase() = default;	// 가상함수로 지정 X. 객체 공간 절약 (ComponentHandleBase*로 조작할 일 없음)

		uint32_t GetIndex() const { return m_tableIndex; }
		uint64_t GetId() const { return m_id; }
	protected:
		uint32_t m_tableIndex;
		// uint32_t m_reserved;
		uint64_t m_id;
	};

	template<typename _T>
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
			*static_cast<ComponentHandleBase*>(this) = other;

			return *this;
		}
		~ComponentHandle() = default;

		_T* ToPtr() const;
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		bool IsValid() const { return ToPtr() != nullptr; }
	};

	inline bool operator==(const GameObjectHandle& hA, const GameObjectHandle& hB)
	{
		return
			hA.GetIndex() == hB.GetIndex() &&
			hA.GetId() == hB.GetId();
	}

	template<typename _T>
	inline bool operator==(const ComponentHandle<_T>& hA, const ComponentHandle<_T>& hB)
	{
		return
			hA.GetIndex() == hB.GetIndex() &&
			hA.GetId() == hB.GetId();
	}

	template<typename _T>
	_T* ComponentHandle<_T>::ToPtr() const
	{
		return static_cast<_T*>(HandleHelper::ToPtrImpl(_T::TYPE, m_tableIndex, m_id));
	}
}

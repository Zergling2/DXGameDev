#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentType.h>

namespace ze
{
	class GameObject;
	class IComponent;

	constexpr uint64_t INVALID_ID = std::numeric_limits<uint64_t>::max();

	class GameObjectHandle
	{
		friend class RuntimeImpl;
		friend class GameObjectManagerImpl;
		friend class GameObject;
	public:
		GameObjectHandle() noexcept
			: m_index(0)
			, m_reserved(0)
			, m_id(INVALID_ID)
		{
		}
	private:
		GameObjectHandle(uint32_t index, uint64_t id) noexcept
			: m_index(index)
			, m_reserved(0)
			, m_id(id)
		{
		}
	public:
		GameObjectHandle(const GameObjectHandle& other) noexcept
			: m_index(other.m_index)
			, m_reserved(other.m_reserved)
			, m_id(other.m_id)
		{
		}
		GameObjectHandle(GameObjectHandle&& other) noexcept
			: m_index(other.m_index)
			, m_reserved(other.m_reserved)
			, m_id(other.m_id)
		{
			other.m_index = 0;
			other.m_reserved = 0;
			other.m_id = INVALID_ID;
		}
		GameObjectHandle& operator=(const GameObjectHandle& other)
		{
			m_index = other.m_index;
			m_reserved = other.m_reserved;
			m_id = other.m_id;

			return *this;
		}
		~GameObjectHandle() = default;

		GameObject* ToPtr() const;	// index에 가서 nullptr이면 파괴된 것, 아니라면 m_id와 객체의 id 비교
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		inline bool IsValid() const { return ToPtr() != nullptr; }

		inline uint32_t GetIndex() const { return m_index; }
		inline uint64_t GetId() const { return m_id; }
	private:
		uint32_t m_index;
		uint32_t m_reserved;
		uint64_t m_id;
	};

	class ComponentHandle
	{
		friend class IComponentManager;
		friend class SceneManagerImpl;
	public:
		static ComponentHandle MakeFakeHandle(IComponent* pComponent);
	public:
		ComponentHandle() noexcept
			: m_index(0)
			, m_type(COMPONENT_TYPE::UNKNOWN)
			, m_id(INVALID_ID)
		{
		}
	private:
		ComponentHandle(uint32_t index, COMPONENT_TYPE type, uint64_t id) noexcept
			: m_index(index)
			, m_type(type)
			, m_id(id)
		{
		}
	public:
		ComponentHandle(const ComponentHandle& other) noexcept
			: m_index(other.m_index)
			, m_type(other.m_type)
			, m_id(other.m_id)
		{
		}
		ComponentHandle(ComponentHandle&& other) noexcept
			: m_index(other.m_index)
			, m_type(other.m_type)
			, m_id(other.m_id)
		{
			other.m_index = 0;
			other.m_type = COMPONENT_TYPE::UNKNOWN;
			other.m_id = INVALID_ID;
		}
		ComponentHandle& operator=(const ComponentHandle& other)
		{
			m_index = other.m_index;
			m_type = other.m_type;
			m_id = other.m_id;

			return *this;
		}
		~ComponentHandle() = default;

		IComponent* ToPtr() const;	// index에 가서 nullptr이면 파괴된 것, 아니라면 m_id와 객체의 id 비교
		bool IsDestroyed() const { return ToPtr() == nullptr; }
		inline bool IsValid() const { return ToPtr() != nullptr; }

		inline uint32_t GetIndex() const { return m_index; }
		inline COMPONENT_TYPE GetType() const { return m_type; }
		inline uint64_t GetId() const { return m_id; }
	private:
		uint32_t m_index;
		COMPONENT_TYPE m_type;
		union
		{
			uint64_t m_id;
			IComponent* m_pComponent;
		};
	};

	bool operator==(const GameObjectHandle& hA, const GameObjectHandle& hB);
	bool operator==(const ComponentHandle& hA, const ComponentHandle& hB);
}

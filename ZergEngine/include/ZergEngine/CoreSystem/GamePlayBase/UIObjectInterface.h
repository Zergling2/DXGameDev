#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObjectType.h>

namespace ze
{
	using UIObjectFlagType = uint8_t;
	using uioft = UIObjectFlagType;

	enum class UIOBJECT_FLAG : uioft
	{
		NONE = 0,

		STATIC					= 1 << 0,
		DONT_DESTROY_ON_LOAD	= 1 << 1,
		DEFERRED				= 1 << 2,
		ACTIVE					= 1 << 3,
		ON_DESTROY_QUEUE		= 1 << 4,
		REAL_ROOT				= 1 << 5
	};

	/*
	* Scene 로드 함수 내에서 UI 오브젝트 계층 구조의 수정은 허용되지 않으며 RectTransformL::SetParent() 함수는 실패합니다.
	* 예시: UI 오브젝트의 부모 변경, 루트가 아닌 오브젝트를 루트로 변경하는 등
	*/
	class IUIObject
	{
		friend class RuntimeImpl;
		friend class UIObjectManagerImpl;
		friend class SceneManagerImpl;
		friend class RendererImpl;
		friend class RectTransform;
	protected:
		IUIObject(UIOBJECT_FLAG flag, PCWSTR name, uint64_t id, UIOBJECT_TYPE type);
		virtual ~IUIObject() = default;
	public:
		bool IsDontDestroyOnLoad() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::DONT_DESTROY_ON_LOAD); }
		bool IsActive() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::ACTIVE); }

		PCWSTR GetName() const { return m_name; }
		uint64_t GetId() const { return m_id; }
		UIOBJECT_TYPE GetType() const { return m_type; }

		void SetActive(bool active);

		template<class UIObjectType, typename... Args>
		UIObjectHandle CreateChildUIObject(Args&& ...args);
	private:
		const UIObjectHandle ToHandleBase() const;

		bool IsDeferred() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::DEFERRED); }
		void OnFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) | static_cast<uioft>(flag)); }
		void OffFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) & ~static_cast<uioft>(flag)); }
		bool IsOnTheDestroyQueue() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::ON_DESTROY_QUEUE); }
		bool IsRootObject() const {return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::REAL_ROOT); }

		virtual bool HitTest(const XMFLOAT2A mousePos) const = 0;
		static UIObjectHandle CreateChildUIObjectImpl(IUIObject* pUIObject);
	public:
		RectTransform m_transform;
	private:
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_activeIndex;
		UIOBJECT_FLAG m_flag;
		UIOBJECT_TYPE m_type;
		WCHAR m_name[16];
	};

	template<class UIObjectType, typename ...Args>
	UIObjectHandle IUIObject::CreateChildUIObject(Args && ...args)
	{
		const UIOBJECT_FLAG flag = this->IsDeferred() ?
			static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE)) :
			static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE);

		UIObjectType* pNewUIObject = new UIObjectType(flag,	std::forward<Args>(args)...);

		m_transform.m_children.push_back(&pNewUIObject->m_transform);
		pNewUIObject->m_transform.m_pParentTransform = &this->m_transform;

		return IUIObject::CreateChildUIObjectImpl(pNewUIObject);
	}
}

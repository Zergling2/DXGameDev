#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectType.h>

namespace ze
{
	using UIObjectFlagType = uint8_t;
	using uioft = UIObjectFlagType;

	enum class UIOBJECT_FLAG : uioft
	{
		NONE = 0,

		STATIC					= 1 << 0,
		DONT_DESTROY_ON_LOAD	= 1 << 1,
		PENDING					= 1 << 2,
		ACTIVE					= 1 << 3,
		ON_DESTROY_QUEUE		= 1 << 4,
		REAL_ROOT				= 1 << 5		// ������Ʈ ���� �������� ��¥ ��Ʈ�������� �Ǻ��ϴ� �뵵�� ���ȴ�.
	};

	/*
	* Scene �ε� �Լ� ������ UI ������Ʈ ���� ������ ������ ������ ������ RectTransformL::SetParent() �Լ��� �����մϴ�.
	* ����: UI ������Ʈ�� �θ� ����, ��Ʈ�� �ƴ� ������Ʈ�� ��Ʈ�� �����ϴ� ��
	*/
	class IUIObject
	{
		friend class UIObjectManager;
		friend class SceneManager;
		friend class Renderer;
		friend class RectTransform;
	protected:
		IUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~IUIObject() = default;
	public:
		void DontDestroyOnLoad();
		void Destroy();
		void Destroy(float delay);
		bool IsDontDestroyOnLoad() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::DONT_DESTROY_ON_LOAD); }
		bool IsActive() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::ACTIVE); }

		PCWSTR GetName() const { return m_name; }
		uint64_t GetId() const { return m_id; }
		virtual UIOBJECT_TYPE GetType() const = 0;

		void SetActive(bool active);
	private:
		const UIObjectHandle ToHandleBase() const;

		bool IsPending() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::PENDING); }
		void OnFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) | static_cast<uioft>(flag)); }
		void OffFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) & ~static_cast<uioft>(flag)); }
		bool IsOnTheDestroyQueue() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::ON_DESTROY_QUEUE); }
		bool IsRoot() const {return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::REAL_ROOT); }

		// UI ��ȣ�ۿ��� ��ҵǴ°�� �� �� ����
		virtual void OnDetachUIInteraction() {}
		virtual bool XM_CALLCONV HitTest(FXMVECTOR mousePosition) const = 0;

		virtual void OnLButtonDown() {}
		virtual void OnLButtonUp() {}
		virtual void OnLClick() {}
		virtual void OnRButtonDown() {}
		virtual void OnRButtonUp() {}
		virtual void OnRClick() {}
		virtual void OnMButtonDown() {}
		virtual void OnMButtonUp() {}
		virtual void OnMClick() {}
	public:
		RectTransform m_transform;
	private:
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_groupIndex;
		UIOBJECT_FLAG m_flag;
		WCHAR m_name[16];
	};
}

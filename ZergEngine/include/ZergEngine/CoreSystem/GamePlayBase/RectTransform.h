#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IUIObject;

	enum class HORIZONTAL_ANCHOR : uint8_t
	{
		LEFT,
		CENTER,
		RIGHT
	};

	enum class VERTICAL_ANCHOR : uint8_t
	{
		TOP,
		VCENTER,
		BOTTOM
	};

	class RectTransform
	{
		friend class Runtime;
		friend class IUIObject;
		friend class UIObjectManager;
		friend class SceneManager;
		friend class Renderer;
		friend class ISizeUIObject;
	public:
		RectTransform(IUIObject* pUIObject)
			: m_pUIObject(pUIObject)
			, m_pParentTransform(nullptr)
			, m_children()
			, m_position(0.0f, 0.0f)
			, m_ha(HORIZONTAL_ANCHOR::CENTER)
			, m_va(VERTICAL_ANCHOR::VCENTER)
		{
		}
		RectTransform(const RectTransform&) = delete;
		RectTransform(RectTransform&&) = delete;
		~RectTransform() = default;
		RectTransform& operator=(const RectTransform&) = delete;

		HORIZONTAL_ANCHOR GetHorizontalAnchor() const { return m_ha; }
		VERTICAL_ANCHOR GetVerticalAnchor() const { return m_va; }
		void SetHorizontalAnchor(HORIZONTAL_ANCHOR ha) { m_ha = ha; }
		void SetVerticalAnchor(VERTICAL_ANCHOR va) { m_va = va; }
		XMVECTOR GetLocalPosition() const { return XMLoadFloat2(&m_position); }

		void XM_CALLCONV Translate(FXMVECTOR translation) { XMStoreFloat2(&m_position, XMVectorAdd(XMLoadFloat2(&m_position), translation)); }
		void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat2(&m_position, position); }
		void SetPosition(const XMFLOAT2& position) { m_position = position; }
		void SetPosition(const XMFLOAT2A& position) { m_position = position; }

		// 부모로 설정할 오브젝트의 Transform을 인수로 전달받습니다.
		bool SetParent(RectTransform* pTransform);
		bool IsDescendantOf(RectTransform* pTransform) const;

		// UIObjectHandle GetChild(uint32_t index);
	private:
		XMVECTOR GetPreNDCPosition() const;
		XMVECTOR GetWindowsScreenPosition() const;
	private:
		IUIObject* m_pUIObject;
		RectTransform* m_pParentTransform;
		std::vector<RectTransform*> m_children;
	public:
		XMFLOAT2 m_position;
		HORIZONTAL_ANCHOR m_ha;
		VERTICAL_ANCHOR m_va;
	};
}

#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\Platform.h>
#include <vector>

namespace ze
{
	class IUIObject;

	enum class HorizontalAnchor : uint8_t
	{
		Left,
		Center,
		Right
	};

	enum class VerticalAnchor : uint8_t
	{
		Top,
		VCenter,
		Bottom
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
		RectTransform(IUIObject* pUIObject);
		RectTransform(const RectTransform&) = delete;
		RectTransform(RectTransform&&) = delete;
		~RectTransform() = default;
		RectTransform& operator=(const RectTransform&) = delete;

		HorizontalAnchor GetHorizontalAnchor() const { return m_ha; }
		VerticalAnchor GetVerticalAnchor() const { return m_va; }
		void SetHorizontalAnchor(HorizontalAnchor ha) { m_ha = ha; }
		void SetVerticalAnchor(VerticalAnchor va) { m_va = va; }
		XMVECTOR GetLocalPosition() const { return XMLoadFloat2(&m_position); }

		const XMFLOAT2& GetPosition() const { return m_position; }
		FLOAT GetPositionX() const { return m_position.x; }
		FLOAT GetPositionY() const { return m_position.y; }

		void XM_CALLCONV Translate(FXMVECTOR translation) { XMStoreFloat2(&m_position, XMVectorAdd(XMLoadFloat2(&m_position), translation)); }
		void Translate(const XMFLOAT2& translation) { m_position.x += translation.x; m_position.y += translation.y; }
		void Translate(FLOAT tx, FLOAT ty) { m_position.x += tx; m_position.y += ty; }
		void TranslateX(FLOAT tx) { m_position.x += tx; }
		void TranslateY(FLOAT ty) { m_position.y += ty; }
		void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat2(&m_position, position); }
		void SetPosition(const XMFLOAT2& position) { m_position = position; }
		void SetPosition(const XMFLOAT2A& position) { m_position = position; }
		void SetPosition(FLOAT x, FLOAT y) { m_position.x = x;	m_position.y = y; }
		void SetPositionX(FLOAT x) { m_position.x = x; }
		void SetPositionY(FLOAT y) { m_position.y = y; }

		RectTransform* GetParent() const { return m_pParent; }
		bool SetParent(RectTransform* pTransform);	// 부모로 설정할 오브젝트의 Transform을 인수로 전달받습니다.
		bool IsDescendantOf(RectTransform* pTransform) const;

		// UIObjectHandle GetChild(uint32_t index);

		void GetHCSPosition(XMFLOAT2* pOut) const;
		void GetWinCoordPosition(POINT* pOut) const;
		void GetWinCoordPosition(XMFLOAT2* pOut) const;
	private:
		IUIObject* m_pUIObject;
		RectTransform* m_pParent;
		std::vector<RectTransform*> m_children;

		XMFLOAT2 m_position;
		HorizontalAnchor m_ha;
		VerticalAnchor m_va;
	};
}

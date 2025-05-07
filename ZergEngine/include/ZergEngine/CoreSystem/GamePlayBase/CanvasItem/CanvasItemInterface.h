#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\CanvasItem\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\CanvasItem\UIHitbox.h>

namespace ze
{
	class ICanvasItem abstract
	{
	public:
		ICanvasItem() noexcept
			: m_width(64.0f)
			, m_height(64.0f)
			, m_hitbox()
		{
		}
		virtual ~ICanvasItem() = default;

		ANCHOR GetAnchor() const { return m_transform.m_anchor; }
		void SetAnchor(ANCHOR anchor);	// 히트박스 재계산 필요

		float GetWidth() const { return m_width; }
		void SetWidth(float width);		// 히트박스 재계산 필요
		float GetHeight() const { return m_height; }
		void SetHeight(float height);	// 히트박스 재계산 필요
	private:
		virtual void UpdateHitbox() = 0;	// 부모 Canvas의 스크린 영역 너비를 통해 자신의 히트박스 업데이트
	private:

		RectTransform m_transform;
		float m_width;
		float m_height;
		UIHitbox m_hitbox;
	};
}

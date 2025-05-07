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
		void SetAnchor(ANCHOR anchor);	// ��Ʈ�ڽ� ���� �ʿ�

		float GetWidth() const { return m_width; }
		void SetWidth(float width);		// ��Ʈ�ڽ� ���� �ʿ�
		float GetHeight() const { return m_height; }
		void SetHeight(float height);	// ��Ʈ�ڽ� ���� �ʿ�
	private:
		virtual void UpdateHitbox() = 0;	// �θ� Canvas�� ��ũ�� ���� �ʺ� ���� �ڽ��� ��Ʈ�ڽ� ������Ʈ
	private:

		RectTransform m_transform;
		float m_width;
		float m_height;
		UIHitbox m_hitbox;
	};
}

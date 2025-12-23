#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class Text : public IUIObject
	{
		friend class Renderer;
	public:
		Text(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Text() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::Text; }

		const XMFLOAT2& GetSize() const { return m_size.GetSize(); }
		FLOAT GetSizeX() const { return m_size.GetSizeX(); }
		FLOAT GetSizeY() const { return m_size.GetSizeY(); }
		FLOAT GetWidth() const { return GetSizeX(); }
		FLOAT GetHeight() const { return GetSizeY(); }
		FLOAT GetHalfSizeX() const { return m_size.GetHalfSizeX(); }
		FLOAT GetHalfSizeY() const { return m_size.GetHalfSizeY(); }
		FLOAT GetHalfWidth() const { return GetHalfSizeX(); }
		FLOAT GetHalfHeight() const { return GetHalfSizeY(); }
		void SetSize(FLOAT width, FLOAT height) { m_size.SetSize(width, height); }
		void SetSize(const XMFLOAT2& size) { m_size.SetSize(size); }
		void XM_CALLCONV SetSize(FXMVECTOR size) { m_size.SetSize(size); }

		std::wstring& GetText() { return m_text.GetText(); }
		const std::wstring& GetText() const { return m_text.GetText(); }
		void SetText(PCWSTR text) { m_text.SetText(text); }
		void SetText(std::wstring text) { m_text.SetText(std::move(text)); }
		TextFormat& GetTextFormat() { return m_text.GetTextFormat(); }
		const TextFormat& GetTextFormat() const { return m_text.GetTextFormat(); }
		DWRITE_TEXT_ALIGNMENT GetTextAlignment() const { return m_text.GetTextAlignment(); }
		void SetTextAlignment(DWRITE_TEXT_ALIGNMENT ta) { m_text.SetTextAlignment(ta); }
		DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() const { return m_text.GetParagraphAlignment(); }
		void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT pa) { m_text.SetParagraphAlignment(pa); }
		IDWriteTextFormat* GetDWriteTextFormatComInterface() const { return m_text.GetDWriteTextFormatComInterface(); }
		void ApplyTextFormat() { m_text.ApplyTextFormat(); }		// TextFormat을 변경한 후 이 함수를 호출해야 새로운 폰트가 적용됩니다.

		XMVECTOR GetColorVector() const { return m_color.GetColorVector(); }
		const XMFLOAT4& GetColor() const { return m_color.GetColor(); }
		void XM_CALLCONV SetColor(FXMVECTOR color) { m_color.SetColor(color); }
		void SetColor(const XMFLOAT4& color) { m_color.SetColor(color); }
		void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_color.SetColor(r, g, b, a); }
		void XM_CALLCONV SetColorRGB(FXMVECTOR rgb) { m_color.SetColorRGB(rgb); }
		void SetColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_color.SetColorRGB(r, g, b); }
		void SetColorA(FLOAT a) { m_color.SetColorA(a); }

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override { return false; }
	private:
		UISize m_size;	// Text box size
		UIText m_text;
		UIColor m_color;
	};
}

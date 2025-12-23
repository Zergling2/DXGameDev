#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	enum class InputFieldShape : uint8_t
	{
		ClientEdge,
		Rectangle,
		RoundedRectangle
	};

	enum INPUT_FIELD_FLAG : uint32_t
	{
		IFF_NONE		= 0,
		IFF_RETURN		= 0x00000001,	// 엔터 허용
		IFF_SPACE		= 0x00000002,	// 공백 허용
		IFF_DIGIT		= 0x00000004,	// 숫자만 허용
		IFF_PASSWORD	= 0x00000008	// 입력을 *로 표시
	};

	class InputField : public IUIObject
	{
		friend class UIObjectManager;
	public:
		InputField(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~InputField() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::InputField; }

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

		const XMFLOAT4& GetBkColor() const { return m_bkColor.GetColor(); }
		XMVECTOR GetBkColorVector() const { return m_bkColor.GetColorVector(); }
		void XM_CALLCONV SetBkColor(FXMVECTOR color) { m_bkColor.SetColor(color); }
		void SetBkColor(const XMFLOAT4& color) { m_bkColor.SetColor(color); }
		void SetBkColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_bkColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetBkColorRGB(FXMVECTOR rgb) { m_bkColor.SetColorRGB(rgb); }
		void SetBkColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_bkColor.SetColorRGB(r, g, b); }
		void SetBkColorA(FLOAT a) { m_bkColor.SetColorA(a); }

		const XMFLOAT4& GetTextColor() const { return m_textColor.GetColor(); }
		XMVECTOR GetTextColorVector() const { return m_textColor.GetColorVector(); }
		void XM_CALLCONV SetTextColor(FXMVECTOR color) { m_textColor.SetColor(color); }
		void SetTextColor(const XMFLOAT4& color) { m_textColor.SetColor(color); }
		void SetTextColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_textColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetTextColorRGB(FXMVECTOR rgb) { m_textColor.SetColorRGB(rgb); }
		void SetTextColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_textColor.SetColorRGB(r, g, b); }
		void SetTextColorA(FLOAT a) { m_textColor.SetColorA(a); }

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

		void SetShape(InputFieldShape shape) { m_shape = shape; }
		InputFieldShape GetShape() const { return m_shape; }

		void SetRadiusX(float rx) { m_radius.x = rx; }
		void SetRadiusY(float ry) { m_radius.y = ry; }
		float GetRadiusX() const { return m_radius.x; }
		float GetRadiusY() const { return m_radius.y; }

		void AllowReturn(bool b);
		bool IsReturnAllowed() const { return m_iff & IFF_RETURN; }

		void AllowSpace(bool b);
		bool IsSpaceAllowed() const { return m_iff & IFF_SPACE; }

		void SetDigitOnly(bool b);
		bool IsDigitOnly() const { return m_iff & IFF_DIGIT; }

		// 문자열의 내용을 *문자로 대체하여 출력합니다.
		void SetPassword(bool b);
		bool IsPassword() const { return m_iff & IFF_PASSWORD; }

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override;
	protected:
		virtual void OnChar(TCHAR ch) override;
		virtual void OnLButtonClick(POINT pt) override;
	private:
		UISize m_size;
		UIColor m_bkColor;
		UIText m_text;
		UIColor m_textColor;
		XMFLOAT2 m_radius;	// Shape가 InputFieldShape::RoundedRectangle이 아닌 경우 의미를 갖지 않습니다.
		uint32_t m_iff;	// FLAG
		InputFieldShape m_shape;
	};
}

#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>

namespace ze
{
	enum class INPUT_FIELD_SHAPE : uint8_t
	{
		RECTANGLE,
		ROUNDED_RECTANGLE
	};

	enum INPUT_FIELD_FLAG : uint32_t
	{
		IFF_NONE		= 0,
		IFF_RETURN		= 0x00000001,	// 엔터 허용
		IFF_SPACE		= 0x00000002,	// 공백 허용
		IFF_DIGIT		= 0x00000004,	// 숫자만 허용
		IFF_PASSWORD	= 0x00000008	// 입력을 *로 표시
	};

	class InputField : public Text
	{
		friend class UIObjectManager;
	public:
		InputField(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~InputField() = default;

		virtual UIOBJECT_TYPE GetType() const override { return UIOBJECT_TYPE::INPUT_FIELD; }

		void AllowReturn(bool b);
		bool IsReturnAllowed() const { return m_iff & IFF_RETURN; }

		void AllowSpace(bool b);
		bool IsSpaceAllowed() const { return m_iff & IFF_SPACE; }

		void SetDigitOnly(bool b);
		bool IsDigitOnly() const { return m_iff & IFF_DIGIT; }

		// 문자열의 내용을 *문자로 대체하여 출력합니다.
		void SetPassword(bool b);
		bool IsPassword() const { return m_iff & IFF_PASSWORD; }

		// Input Field 박스의 배경 색상을 가져옵니다.
		const XMFLOAT4& GetBkColor() const { return m_bkColor; }
		XMVECTOR XM_CALLCONV GetBkColorVector() const { return XMLoadFloat4(&m_bkColor); }

		// Input Field 박스의 배경 색상을 설정합니다.
		void XM_CALLCONV SetBkColor(FXMVECTOR color) { XMStoreFloat4(&m_bkColor, color); }
		void SetBkColor(const XMFLOAT4A& color) { m_bkColor = color; }
		void SetBkColor(const XMFLOAT4& color) { m_bkColor = color; }

		void SetShape(INPUT_FIELD_SHAPE shape) { m_shape = shape; }
		INPUT_FIELD_SHAPE GetShape() const { return m_shape; }

		void SetRadiusX(float rx) { m_radius.x = rx; }
		void SetRadiusY(float ry) { m_radius.y = ry; }
		float GetRadiusX() const { return m_radius.x; }
		float GetRadiusY() const { return m_radius.y; }
	protected:
		virtual void OnDetachedFromUIInteraction() override;
		virtual void OnLButtonDown() override;
		virtual void OnLButtonClick() override;
		virtual void OnChar(TCHAR ch) override;
	private:
		uint32_t m_iff;	// FLAG
		INPUT_FIELD_SHAPE m_shape;
		// The x, y radius for the quarter ellipse that is drawn to replace every corner of the rectangle.
		// 패널의 모양이 INPUT_FIELD_SHAPE::RECTANGLE로 설정된 경우에는 의미를 갖지 않습니다.
		XMFLOAT2 m_radius;
		XMFLOAT4 m_bkColor;
	};
}

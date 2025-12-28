#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectType.h>
#include <ZergEngine\CoreSystem\Resource\Font.h>
#include <memory>
#include <functional>

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
		REAL_ROOT				= 1 << 5		// 오브젝트 제거 과정에서 진짜 루트였는지를 판별하는 용도로 사용된다.
	};

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
		void SetActive(bool active);

		virtual UIObjectType GetType() const = 0;

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const = 0;
	private:
		virtual void OnChar(TCHAR ch) {}
		virtual void OnMouseMove(POINT pt) {}
		virtual void OnLButtonDown(POINT pt) {}
		virtual void OnLButtonUp(POINT pt) {}
		virtual void OnLButtonClick(POINT pt) {}
		virtual void OnMButtonDown(POINT pt) {}
		virtual void OnMButtonUp(POINT pt) {}
		virtual void OnMButtonClick(POINT pt) {}
		virtual void OnRButtonDown(POINT pt) {}
		virtual void OnRButtonUp(POINT pt) {}
		virtual void OnRButtonClick(POINT pt) {}

		virtual void OnDestroy() {}
		

		const UIObjectHandle ToHandle() const;

		void OnFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) | static_cast<uioft>(flag)); }
		void OffFlag(UIOBJECT_FLAG flag) { m_flag = static_cast<UIOBJECT_FLAG>(static_cast<uioft>(m_flag) & ~static_cast<uioft>(flag)); }
	protected:
		bool IsPending() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::PENDING); }
		bool IsOnTheDestroyQueue() const { return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::ON_DESTROY_QUEUE); }
		bool IsRoot() const {return static_cast<uioft>(m_flag) & static_cast<uioft>(UIOBJECT_FLAG::REAL_ROOT); }
	public:
		RectTransform m_transform;
	private:
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_groupIndex;
		UIOBJECT_FLAG m_flag;
		WCHAR m_name[16];
	};

	class UISize
	{
	public:
		UISize();
		UISize(FLOAT width, FLOAT height);

		bool HitTest(POINT pt, POINT pos) const;

		XMVECTOR GetSizeVector() const { return XMLoadFloat2(&m_size); }
		const XMFLOAT2& GetSize() const { return m_size; }
		FLOAT GetSizeX() const { return m_size.x; }
		FLOAT GetSizeY() const { return m_size.y; }
		FLOAT GetHalfSizeX() const { return m_halfSize.x; }
		FLOAT GetHalfSizeY() const { return m_halfSize.y; }
		const XMFLOAT2& GetHalfSize() const { return m_halfSize; }
		void XM_CALLCONV SetSize(FXMVECTOR size);
		void SetSize(FLOAT width, FLOAT height);
		void SetSize(const XMFLOAT2& size) { SetSize(size.x, size.y); }
	public:
		XMFLOAT2 m_size;
	private:
		XMFLOAT2 m_halfSize;
	};

	class UIColor
	{
	public:
		UIColor();
		UIColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		UIColor(FXMVECTOR color);

		const XMFLOAT4& GetColor() const { return m_color; }
		XMVECTOR GetColorVector() const { return XMLoadFloat4(&m_color); }
		void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_color.x = r; m_color.y = g; m_color.z = b; m_color.w = a; }
		void XM_CALLCONV SetColor(FXMVECTOR color) { XMStoreFloat4(&m_color, color); }
		void SetColor(const XMFLOAT4& color) { m_color = color; }
		void XM_CALLCONV SetColorRGB(FXMVECTOR rgb) { XMStoreFloat4(&m_color, XMVectorSetW(rgb, m_color.w)); }
		void SetColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_color.x = r; m_color.y = g; m_color.z = b; }
		void SetColorA(FLOAT a) { m_color.w = a; }
	private:
		XMFLOAT4 m_color;
	};

	class UIText
	{
	public:
		UIText();
		UIText(std::wstring text);
		~UIText() = default;

		std::wstring& GetText() { return m_text; }
		const std::wstring& GetText() const { return m_text; }
		void SetText(PCWSTR text) { m_text = text; }
		void SetText(std::wstring text) { m_text = std::move(text); }
		TextFormat& GetTextFormat() { return m_tf; }
		const TextFormat& GetTextFormat() const { return m_tf; }
		DWRITE_TEXT_ALIGNMENT GetTextAlignment() const { return m_textAlignment; }
		void SetTextAlignment(DWRITE_TEXT_ALIGNMENT ta) { m_textAlignment = ta; }
		DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() const { return m_paragraphAlignment; }
		void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT pa) { m_paragraphAlignment = pa; }
		void ApplyTextFormat();		// TextFormat을 변경한 후 이 함수를 호출해야 새로운 폰트가 적용됩니다.

		IDWriteTextFormat* GetDWriteTextFormatComInterface() const { return m_spDWriteTextFormatWrapper->GetDWriteTextFormatComInterface(); }
	protected:
		std::wstring m_text;
		TextFormat m_tf;
		DWRITE_TEXT_ALIGNMENT m_textAlignment;
		DWRITE_PARAGRAPH_ALIGNMENT m_paragraphAlignment;
		std::shared_ptr<DWriteTextFormatWrapper> m_spDWriteTextFormatWrapper;
	};
}

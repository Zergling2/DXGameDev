#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class TextFormat
	{
	public:
		TextFormat(std::wstring fontFamilyName, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch, uint32_t size)
			: m_familyName(std::move(fontFamilyName))
			, m_weight(weight)
			, m_style(style)
			, m_stretch(stretch)
			, m_size(size)
		{
		}
		bool operator==(const TextFormat& other) const
		{
			return
				(this->GetFontFamilyName() == other.GetFontFamilyName()) &&
				(this->GetWeight() == other.GetWeight()) &&
				(this->GetStyle() == other.GetStyle()) &&
				(this->GetStretch() == other.GetStretch()) &&
				(this->GetSize() == other.GetSize());
		}

		const std::wstring& GetFontFamilyName() const { return m_familyName; }
		void SetFontFamilyName(std::wstring familyName) { m_familyName = std::move(familyName); }
		DWRITE_FONT_WEIGHT GetWeight() const { return m_weight; };
		void SetWeight(DWRITE_FONT_WEIGHT weight) { m_weight = weight; }
		DWRITE_FONT_STYLE GetStyle() const { return m_style; };
		void SetStyle(DWRITE_FONT_STYLE style) { m_style = style; };
		DWRITE_FONT_STRETCH GetStretch() const { return m_stretch; };
		void SetStretch(DWRITE_FONT_STRETCH stretch) { m_stretch = stretch; };
		uint32_t GetSize() const { return m_size; }
		void SetSize(uint32_t size) { m_size = size; }
	private:
		std::wstring m_familyName;
		DWRITE_FONT_WEIGHT m_weight;
		DWRITE_FONT_STYLE m_style;
		DWRITE_FONT_STRETCH m_stretch;
		uint32_t m_size;	// float·Î º¯È¯
	};

	struct TextFormatHasher
	{
	public:
		std::size_t operator()(const TextFormat& tf) const noexcept
		{
			std::size_t h[5];
			h[0] = std::hash<std::wstring>()(tf.GetFontFamilyName());
			h[1] = std::hash<int>()(tf.GetWeight());
			h[2] = std::hash<int>()(tf.GetStyle());
			h[3] = std::hash<int>()(tf.GetStretch());
			h[4] = std::hash<uint32_t>()(tf.GetSize());

			return h[0] ^ h[1] ^ h[2] ^ h[3] ^ h[4];	// o_o
		}
	};

	class DWriteTextFormatWrapper
	{
	public:
		DWriteTextFormatWrapper(IDWriteTextFormat* pTextFormat) noexcept
			: m_pTextFormat(pTextFormat)
		{
		}
		~DWriteTextFormatWrapper()
		{
			if (m_pTextFormat)
				m_pTextFormat->Release();
		}
		IDWriteTextFormat* GetDWriteTextFormatComInterface() const { return m_pTextFormat; }
	private:
		IDWriteTextFormat* m_pTextFormat;
	};
}

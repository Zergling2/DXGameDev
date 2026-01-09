#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <string>
#include <utility>

namespace ze
{
	class TextFormat
	{
	public:
		TextFormat(std::wstring fontFamilyName, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style,
			DWRITE_FONT_STRETCH stretch, uint32_t size)
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
		size_t operator()(const TextFormat& tf) const
		{
			size_t h = std::hash<std::wstring>()(tf.GetFontFamilyName());
			h = CombineHash(h, std::hash<int>()(tf.GetWeight()));
			h = CombineHash(h, std::hash<int>()(tf.GetStyle()));
			h = CombineHash(h, std::hash<int>()(tf.GetStretch()));
			h = CombineHash(h, std::hash<uint32_t>()(tf.GetSize()));

			return h;
		}
		static size_t CombineHash(size_t lhs, size_t rhs)
		{
			lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
			return lhs;
		}
	};

	class DWriteTextFormatWrapper
	{
	public:
		DWriteTextFormatWrapper(IDWriteTextFormat* pTextFormat)
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

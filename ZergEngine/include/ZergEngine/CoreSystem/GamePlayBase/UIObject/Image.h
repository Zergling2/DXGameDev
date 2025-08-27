#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SizeColorUIObjectInterface.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Image : public ISizeColorUIObject
	{
	public:
		Image(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Image() = default;

		virtual UIOBJECT_TYPE GetType() const override { return UIOBJECT_TYPE::IMAGE; }

		void SetTexture(const Texture2D& texture);
		Texture2D& GetTexture() { return m_texture; }
		const Texture2D& GetTexture() const { return m_texture; }

		// 이미지의 크기를 텍스처의 크기와 일치시킵니다.
		void SetNativeSize(bool b);
	private:
		void UpdateToNativeSize();
	private:
		bool m_nativeSize;
		Texture2D m_texture;
	};
}

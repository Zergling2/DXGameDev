#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

namespace ze
{
	class Image : public IUIObject
	{
	public:
		Image(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~Image() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::Image; }

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

		void SetTexture(const Texture2D& texture);
		Texture2D& GetTexture() { return m_texture; }
		const Texture2D& GetTexture() const { return m_texture; }

		// 이미지의 크기를 텍스처의 크기와 일치시킵니다.
		void SetNativeSize(bool b);

		// Windows 좌표계 마우스 위치와 충돌 테스트 수행
		virtual bool HitTest(POINT pt) const override;
	private:
		void UpdateToNativeSize();
	private:
		UISize m_size;
		Texture2D m_texture;
		bool m_nativeSize;
	};
}

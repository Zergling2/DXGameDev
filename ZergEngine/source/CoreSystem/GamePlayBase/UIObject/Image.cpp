#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>

using namespace ze;

constexpr float IMAGE_DEFAULT_SIZE = 256.0f;

Image::Image(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_size(IMAGE_DEFAULT_SIZE, IMAGE_DEFAULT_SIZE)
	, m_texture()
	, m_nativeSize(false)
{
}

void Image::SetTexture(const Texture2D& texture)
{
	m_texture = texture;

	if (m_nativeSize)
		this->UpdateToNativeSize();
}

void Image::SetNativeSize(bool b)
{
	m_nativeSize = b;

	if (m_nativeSize)
		this->UpdateToNativeSize();
}

bool Image::HitTest(const XMFLOAT2& mousePos) const
{
	XMFLOAT2 wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(mousePos, wcp);
}

void Image::UpdateToNativeSize()
{
	if (m_texture.GetTex2DComInterface())
	{
		D3D11_TEXTURE2D_DESC desc;
		m_texture.GetTexture2DDesc(&desc);

		this->SetSize(XMFLOAT2A(static_cast<FLOAT>(desc.Width), static_cast<FLOAT>(desc.Height)));
	}
}

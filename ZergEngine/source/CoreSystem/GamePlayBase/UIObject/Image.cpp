#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>

using namespace ze;

constexpr float IMAGE_DEFAULT_SIZE = 400.0f;

Image::Image(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_size(IMAGE_DEFAULT_SIZE, IMAGE_DEFAULT_SIZE)
	, m_uvScale(1.0f, 1.0f)
	, m_uvOffset(0.0f, 0.0f)
	, m_texture()
	, m_nativeSize(false)
	, m_handlerOnClick()
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

bool Image::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(pt, wcp);
}

void Image::OnLButtonClick(POINT pt)
{
	// 지연 객체(예시: OnLoadScene)인 경우 콜백 호출 방지
	if (this->IsPending())
		return;

	// UI Event Callback
	if (m_handlerOnClick)
	{
		bool success = m_handlerOnClick();
		if (!success)	// 객체가 파괴된 경우
			m_handlerOnClick = nullptr;
	}
}

void Image::UpdateToNativeSize()
{
	if (m_texture.GetTexture2D())
	{
		D3D11_TEXTURE2D_DESC desc;
		m_texture.GetTexture2DDesc(&desc);

		this->SetSize(XMFLOAT2A(static_cast<FLOAT>(desc.Width), static_cast<FLOAT>(desc.Height)));
	}
}

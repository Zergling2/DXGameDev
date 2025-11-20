#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

void ImageEffect::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = nullptr;
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::TransformPTQuadToHCS);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::ColorPositionTexCoordFragmentWithSingleTexture);

	m_cbPerUIRender.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerImage.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
}

void ImageEffect::Release()
{
	m_cbPerUIRender.Release();
	m_cbPerImage.Release();
}

void ImageEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cbPerUIRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_UI_RENDER;
}

void XM_CALLCONV ImageEffect::SetSize(FXMVECTOR size) noexcept
{
	XMStoreFloat2(&m_cbPerImageCache.size, size);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_IMAGE;
}

void XM_CALLCONV ImageEffect::SetPreNDCPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat2(&m_cbPerImageCache.position, position);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_IMAGE;
}

void ImageEffect::SetImageTexture(const Texture2D& image) noexcept
{
	m_pTextureSRVArray[0] = image.GetSRVComInterface();
}

void ImageEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DIRTY_FLAG::PRIMITIVE_TOPOLOGY:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		case DIRTY_FLAG::INPUT_LAYOUT:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DIRTY_FLAG::SHADER:
			ApplyShader(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_UI_RENDER:
			ApplyPerUIRenderConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_IMAGE:
			ApplyPerImageConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	/*
	Texture2D tex2d_image : register(t0);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void ImageEffect::KickedOutOfDeviceContext() noexcept
{
	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void ImageEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void ImageEffect::ApplyPerUIRenderConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerUIRender.Update(pDeviceContext, &m_cbPerUIRenderCache);
	ID3D11Buffer* const cbs[] = { m_cbPerUIRender.GetComInterface() };

	// PerUIRender 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void ImageEffect::ApplyPerImageConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerImage.Update(pDeviceContext, &m_cbPerImageCache);
	ID3D11Buffer* const cbs[] = { m_cbPerImage.GetComInterface() };

	// PerImage 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

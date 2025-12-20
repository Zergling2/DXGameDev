#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

// Image Effect
// VertexShader:
// - ToHcs2DQuad
// 
// PixelShader:
// - UnlitPT1Tex

void ImageEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcs2DQuad);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPT1Tex);

	m_cbPerUIRender.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPer2DQuad.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
}

void ImageEffect::Release()
{
	m_cbPerUIRender.Release();
	m_cbPer2DQuad.Release();
}

void ImageEffect::SetScreenToNDCSpaceRatio(const XMFLOAT2& ratio) noexcept
{
	m_cbPerUIRenderCache.toNDCSpaceRatio = ratio;

	m_dirtyFlag |= DirtyFlag::CBPerUIRender;
}

void ImageEffect::SetSize(FLOAT width, FLOAT height) noexcept
{
	m_cbPer2DQuadCache.size.x = width;
	m_cbPer2DQuadCache.size.y = height;

	m_dirtyFlag |= DirtyFlag::CBPer2DQuad;
}

void ImageEffect::SetHCSPosition(const XMFLOAT2& pos) noexcept
{
	m_cbPer2DQuadCache.position = pos;

	m_dirtyFlag |= DirtyFlag::CBPer2DQuad;
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
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(nullptr);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::CBPerUIRender:
			ApplyPerUIRenderConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPer2DQuad:
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
	m_dirtyFlag = DirtyFlag::ALL;
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
	m_cbPer2DQuad.Update(pDeviceContext, &m_cbPer2DQuadCache);
	ID3D11Buffer* const cbs[] = { m_cbPer2DQuad.GetComInterface() };

	// Per2DQuad 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

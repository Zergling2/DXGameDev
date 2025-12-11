#include <ZergEngine\CoreSystem\Effect\DrawScreenRatioQuadWithTextureEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

// MSCameraMerge Effect
// 1. VertexShader: VSTransformNDCRatioQuad
// 2. PixelShader: PSColorPTFragmentSingleMSTexture

void DrawScreenRatioQuadWithTextureEffect::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = nullptr;
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::TransformScreenRatioQuad);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::ColorPositionTexCoordFragmentWithSingleTexture);

	m_cbPerScreenRatioQuad.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());

	ClearTextureSRVArray();
}

void DrawScreenRatioQuadWithTextureEffect::Release()
{
	m_cbPerScreenRatioQuad.Release();
}

void DrawScreenRatioQuadWithTextureEffect::SetQuadParameters(float width, float height, float topLeftX, float topLeftY) noexcept
{
	m_cbPerScreenRatioQuadCache.width = width;
	m_cbPerScreenRatioQuadCache.height = height;
	m_cbPerScreenRatioQuadCache.topLeftX = topLeftX;
	m_cbPerScreenRatioQuadCache.topLeftY = topLeftY;

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SCREEN_RATIO_QUAD;
}

void DrawScreenRatioQuadWithTextureEffect::SetTexture(ID3D11ShaderResourceView* pTextureSRV)
{
	m_pTextureSRVArray[0] = pTextureSRV;
}

void DrawScreenRatioQuadWithTextureEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DIRTY_FLAG::PRIMITIVE_TOPOLOGY:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// Triangle Strip
			break;
		case DIRTY_FLAG::INPUT_LAYOUT:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DIRTY_FLAG::SHADER:
			ApplyShader(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_SCREEN_RATIO_QUAD:
			ApplyPerDrawQuadConstantBuffer(pDeviceContext);
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
	Texture2DMS<float4> tex2dms_texture : register(t0);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void DrawScreenRatioQuadWithTextureEffect::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void DrawScreenRatioQuadWithTextureEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void DrawScreenRatioQuadWithTextureEffect::ApplyPerDrawQuadConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerScreenRatioQuad.Update(pDeviceContext, &m_cbPerScreenRatioQuadCache);
	ID3D11Buffer* const cbs[] = { m_cbPerScreenRatioQuad.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void DrawScreenRatioQuadWithTextureEffect::ClearTextureSRVArray()
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}

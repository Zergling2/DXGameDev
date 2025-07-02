#include <ZergEngine\CoreSystem\Effect\MSCameraMergeEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

// MSCameraMerge Effect
// 1. VertexShader: VSTransformCameraMergeQuad
// 2. PixelShader: PSColorPTFragmentSingleMSTexture

void MSCameraMergeEffect::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = nullptr;
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_CAMERA_MERGE_QUAD);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE);

	m_cbPerMSCameraMerge.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());

	ClearTextureSRVArray();
}

void MSCameraMergeEffect::Release()
{
	m_cbPerMSCameraMerge.Release();
}

void MSCameraMergeEffect::SetMergeParameters(float width, float height, float topLeftX, float topLeftY) noexcept
{
	m_cbPerMSCameraMergeCache.width = width;
	m_cbPerMSCameraMergeCache.height = height;
	m_cbPerMSCameraMergeCache.topLeftX = topLeftX;
	m_cbPerMSCameraMergeCache.topLeftY = topLeftY;

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MSCAMERA_MERGE;
}

void MSCameraMergeEffect::SetMergeTexture(ID3D11ShaderResourceView* pMergeTextureSRV)
{
	m_pTextureSRVArray[0] = pMergeTextureSRV;
}

void MSCameraMergeEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DIRTY_FLAG::CONSTANTBUFFER_PER_MSCAMERA_MERGE:
			ApplyPerMSCameraMergeConstantBuffer(pDeviceContext);
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

void MSCameraMergeEffect::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void MSCameraMergeEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void MSCameraMergeEffect::ApplyPerMSCameraMergeConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMSCameraMerge.Update(pDeviceContext, &m_cbPerMSCameraMergeCache);
	ID3D11Buffer* const cbs[] = { m_cbPerMSCameraMerge.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void MSCameraMergeEffect::ClearTextureSRVArray()
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}
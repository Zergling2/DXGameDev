#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

// BasicPT Effect
// 1. VertexShader: VSTransformPTToHCS
// 2. PixelShader: PSColorPTFragment

void BasicEffectPT::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_PT_TO_HCS);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_PT_FRAGMENT);

	m_cbPerCamera.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerMesh.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());

	ClearTextureSRVArray();
}

void BasicEffectPT::Release()
{
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
}

void BasicEffectPT::SetCamera(const Camera* pCamera) noexcept
{
	const GameObject* pCameraOwner = pCamera->m_pGameObject;
	assert(pCameraOwner != nullptr);

	XMMATRIX vp = pCamera->GetViewMatrix() * pCamera->GetProjMatrix();
	Math::CalcWorldFrustumFromViewProjMatrix(vp, m_cbPerCameraCache.frustumW);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_transform.GetWorldPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinDistanceTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaxDistanceTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, ConvertToHLSLMatrix(vp));

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA;
}

void XM_CALLCONV BasicEffectPT::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL ��ġ
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// ������� ��ġ�� HLSL ��ġ

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MESH;
}

void BasicEffectPT::SetTexture(ID3D11ShaderResourceView* pTexture) noexcept
{
	m_pTextureSRVArray[0] = pTexture;
}

void BasicEffectPT::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DIRTY_FLAG::PRIMITIVE_TOPOLOGY:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case DIRTY_FLAG::INPUT_LAYOUT:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DIRTY_FLAG::SHADER:
			ApplyShader(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_MESH:
			ApplyPerMeshConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// ������Ʈ�� �׸��� dirty flag ����
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	/*
	// [Texture]
	Texture2D tex2d_diffuseMap : register(t0);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);
}

void BasicEffectPT::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void BasicEffectPT::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void BasicEffectPT::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera ������� ��� ���̴�
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPT::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh ������� ��� ���̴�
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPT::ClearTextureSRVArray()
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}
#include <ZergEngine\CoreSystem\Effect\SkyboxEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>

using namespace ze;

// Skybox Effect
// 1. VertexShader: VSTransformSkyboxToHCS
// 2. PixelShader: PSColorSkyboxFragment

void SkyboxEffect::Init()
{
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();

	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = nullptr;
	m_pVertexShader = GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_SKYBOX_TO_HCS);
	m_pPixelShader = GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_SKYBOX_FRAGMENT);

	m_cbPerCamera.Init(pDevice);

	ClearTextureSRVArray();
}

void SkyboxEffect::Release()
{
	m_cbPerCamera.Release();
}

void SkyboxEffect::SetCamera(const Camera* pCamera) noexcept
{
	const GameObject* pCameraOwner = pCamera->m_pGameObject;
	assert(pCameraOwner != nullptr);

	XMFLOAT4A frustumPlanes[6];
	XMFLOAT4X4A vp;
	XMStoreFloat4x4A(&vp, pCamera->GetViewMatrix() * pCamera->GetProjMatrix());
	Math::ExtractFrustumPlanesInWorldSpace(&vp, frustumPlanes);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_pTransform->GetPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinimumDistanceForTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaximumDistanceForTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();
	m_cbPerCameraCache.frustumPlane[0] = frustumPlanes[0];
	m_cbPerCameraCache.frustumPlane[1] = frustumPlanes[1];
	m_cbPerCameraCache.frustumPlane[2] = frustumPlanes[2];
	m_cbPerCameraCache.frustumPlane[3] = frustumPlanes[3];
	m_cbPerCameraCache.frustumPlane[4] = frustumPlanes[4];
	m_cbPerCameraCache.frustumPlane[5] = frustumPlanes[5];

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, XMMatrixTranspose(XMLoadFloat4x4A(&vp)));

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA;
}

void SkyboxEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	/*
	TextureCube texcube_skyboxCubeMap : register(t0);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void SkyboxEffect::KickedFromDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void SkyboxEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void SkyboxEffect::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbArr[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbArr);
}

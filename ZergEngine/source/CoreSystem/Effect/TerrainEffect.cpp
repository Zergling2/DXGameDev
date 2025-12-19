#include <ZergEngine\CoreSystem\Effect\TerrainEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>

using namespace ze;

// Terrain Effect
// VertexShader:
// - TerrainPatchCtrlPt
// 
// Hull Shader:
// - CalcTerrainTessFactor
// 
// Domain Shader:
// - SampleTerrainHeightMap
// 
// PixelShader:
// - LitTerrainFragment

void TerrainEffect::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::TerrainPatchCtrlPt);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::TerrainPatchCtrlPt);
	m_pHullShader = GraphicDevice::GetInstance()->GetHSComInterface(HullShaderType::CalcTerrainTessFactor);
	m_pDomainShader = GraphicDevice::GetInstance()->GetDSComInterface(DomainShaderType::SampleTerrainHeightMap);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitTerrainFragment);

	m_cbPerFrame.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerCamera.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerTerrain.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
}

void TerrainEffect::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerTerrain.Release();
}

void XM_CALLCONV TerrainEffect::SetAmbientLight(FXMVECTOR ambientLight) noexcept
{
	XMStoreFloat3(&m_cbPerFrameCache.ambientLight, ambientLight);

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void TerrainEffect::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void TerrainEffect::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void TerrainEffect::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void TerrainEffect::SetCamera(const Camera* pCamera) noexcept
{
	const GameObject* pCameraOwner = pCamera->m_pGameObject;
	assert(pCameraOwner != nullptr);

	XMMATRIX vp = pCamera->GetViewMatrix() * pCamera->GetProjMatrix();
	Math::ComputeFrustumPlanesFromViewProjMatrix(vp, m_cbPerCameraCache.worldSpaceFrustumPlane);

	XMStoreFloat3(&m_cbPerCameraCache.cameraPosW, pCameraOwner->m_transform.GetWorldPosition());
	m_cbPerCameraCache.tessMinDist = pCamera->GetMinDistanceTessellationToStart();
	m_cbPerCameraCache.tessMaxDist = pCamera->GetMaxDistanceTessellationToStart();
	m_cbPerCameraCache.minTessExponent = pCamera->GetMinimumTessellationExponent();
	m_cbPerCameraCache.maxTessExponent = pCamera->GetMaximumTessellationExponent();

	XMStoreFloat4x4A(&m_cbPerCameraCache.vp, ConvertToHLSLMatrix(vp));

	m_dirtyFlag |= DirtyFlag::CBPerCamera;
}

void TerrainEffect::SetMaxHeight(float maxHeight) noexcept
{
	m_cbPerTerrainCache.maxHeight = maxHeight;

	m_dirtyFlag |= DirtyFlag::CBPerTerrain;
}

void TerrainEffect::SetTilingScale(float tilingScale) noexcept
{
	m_cbPerTerrainCache.tilingScale = tilingScale;

	m_dirtyFlag |= DirtyFlag::CBPerTerrain;
}

void TerrainEffect::SetFieldMap(ID3D11ShaderResourceView* pHeightMapSRV, ID3D11ShaderResourceView* pNormalMapSRV) noexcept
{
	assert(pHeightMapSRV != nullptr);
	assert(pNormalMapSRV != nullptr);

	m_pHeightMapSRV = pHeightMapSRV;
	m_pNormalMapSRV = pNormalMapSRV;
}

void TerrainEffect::SetLayerTexture(
	ID3D11ShaderResourceView* pDiffuseMapLayerSRV,
	ID3D11ShaderResourceView* pNormalMapLayerSRV,
	ID3D11ShaderResourceView* pBlendMapSRV) noexcept
{
	if (pDiffuseMapLayerSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC layerDesc;
		pDiffuseMapLayerSRV->GetDesc(&layerDesc);
		m_cbPerTerrainCache.layerArraySize = layerDesc.Texture2DArray.ArraySize;
	}
	else
	{
		m_cbPerTerrainCache.layerArraySize = 0;
	}

	m_pDiffuseMapLayerSRV = pDiffuseMapLayerSRV;
	m_pNormalMapLayerSRV = pNormalMapLayerSRV;
	m_pBlendMapSRV = pBlendMapSRV;

	/*
	if (pDiffuseMapLayerSRV)
		m_cbPerTerrainCache.layerFlag |= static_cast<uint32_t>(TerrainLayerFlag::UseDiffuseLayer);
	else
		m_cbPerTerrainCache.layerFlag &= ~static_cast<uint32_t>(TerrainLayerFlag::UseDiffuseLayer);

	if (pNormalMapLayerSRV)
		m_cbPerTerrainCache.layerFlag |= static_cast<uint32_t>(TerrainLayerFlag::UseNormalLayer);
	else
		m_cbPerTerrainCache.layerFlag &= ~static_cast<uint32_t>(TerrainLayerFlag::UseNormalLayer);
	*/

	m_dirtyFlag |= DirtyFlag::CBPerTerrain;
}

void TerrainEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::CBPerFrame:
			ApplyPerFrameConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPerCamera:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPerTerrain:
			ApplyPerTerrainConstantBuffer(pDeviceContext);
			break;
		default:
			assert(false);
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	// 도메인 셰이더
	// 사용 텍스쳐
	// Texture2D tex2d_heightMap : register(t0);
	// Texture2D tex2d_normalMap : register(t1);
	// 픽셀 셰이더
	// 사용 텍스쳐
	// Texture2DArray tex2d_diffuseMapLayer : register(t0);
	// Texture2DArray tex2d_specularMapLayer : register(t1);
	// Texture2DArray tex2d_normalMapLayer : register(t2);
	// Texture2D tex2d_blendMap : register(t3);
	ID3D11ShaderResourceView* dsSRVs[] = { m_pHeightMapSRV, m_pNormalMapSRV };
	ID3D11ShaderResourceView* psSRVs[] = { m_pDiffuseMapLayerSRV, m_pSpecularMapLayerSRV, m_pNormalMapLayerSRV,	m_pBlendMapSRV };

	pDeviceContext->DSSetShaderResources(0, _countof(dsSRVs), dsSRVs);
	pDeviceContext->PSSetShaderResources(0, _countof(psSRVs), psSRVs);
}

void TerrainEffect::KickedOutOfDeviceContext() noexcept
{
	m_dirtyFlag = DirtyFlag::ALL;
}

void TerrainEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(m_pHullShader, nullptr, 0);
	pDeviceContext->DSSetShader(m_pDomainShader, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void TerrainEffect::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더		 Start Slot		Num Buffers
	pDeviceContext->PSSetConstantBuffers(0,				1,				cbs);
}

void TerrainEffect::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더		 Start Slot		Num Buffers
	pDeviceContext->HSSetConstantBuffers(0,				1,				cbs);
	pDeviceContext->DSSetConstantBuffers(0,				1,				cbs);
	pDeviceContext->PSSetConstantBuffers(1,				1,				cbs);
}

void TerrainEffect::ApplyPerTerrainConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerTerrain.Update(pDeviceContext, &m_cbPerTerrainCache);
	ID3D11Buffer* const cbs[] = { m_cbPerTerrain.GetComInterface() };

	// PerTerrain 상수버퍼 사용 셰이더	 Start Slot		Num Buffers
	pDeviceContext->DSSetConstantBuffers(1,				1,				cbs);
	pDeviceContext->PSSetConstantBuffers(2,				1,				cbs);
}

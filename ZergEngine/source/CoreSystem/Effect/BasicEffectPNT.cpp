#include <ZergEngine\CoreSystem\Effect\BasicEffectPNT.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

// BasicPNT Effect
// 1. VertexShader: VSTransformPNTToHCS
// 2. PixelShader: PSColorPNTFragment

void BasicEffectPNT::Init()
{
	m_dirtyFlag = DIRTY_FLAG::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormalTexCoord);
	m_pVertexShader = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::TRANSFORM_PNT_TO_HCS);
	m_pPixelShader = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::ColorPositionNormalTexCoordFragment);

	m_cbPerFrame.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerCamera.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerMesh.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());
	m_cbPerSubset.Init(GraphicDevice::GetInstance()->GetDeviceComInterface());

	ClearTextureSRVArray();
}

void BasicEffectPNT::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
	m_cbPerSubset.Release();
}

void BasicEffectPNT::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPNT::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPNT::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME;
}

void BasicEffectPNT::SetCamera(const Camera* pCamera) noexcept
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

void XM_CALLCONV BasicEffectPNT::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_MESH;
}

void BasicEffectPNT::UseMaterial(bool b) noexcept
{
	if (b)
		m_cbPerSubsetCache.mtl.mtlFlag |= static_cast<uint32_t>(MATERIAL_FLAG::UseMaterial);
	else
		m_cbPerSubsetCache.mtl.mtlFlag = static_cast<uint32_t>(MATERIAL_FLAG::None);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPNT::SetAmbientColor(FXMVECTOR ambient) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.ambient, ambient);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPNT::SetDiffuseColor(FXMVECTOR diffuse) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.diffuse, diffuse);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPNT::SetSpecularColor(FXMVECTOR specular) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.specular, specular);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void XM_CALLCONV BasicEffectPNT::SetReflection(FXMVECTOR reflect) noexcept
{
	XMStoreFloat4A(&m_cbPerSubsetCache.mtl.reflect, reflect);

	m_dirtyFlag |= DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET;
}

void BasicEffectPNT::SetDiffuseMap(ID3D11ShaderResourceView* pDiffuseMap) noexcept
{
	m_pTextureSRVArray[0] = pDiffuseMap;

	if (pDiffuseMap)
		m_cbPerSubsetCache.mtl.mtlFlag |= static_cast<uint32_t>(MATERIAL_FLAG::UseDiffuseMap);
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~static_cast<uint32_t>(MATERIAL_FLAG::UseDiffuseMap);
}

void BasicEffectPNT::SetSpecularMap(ID3D11ShaderResourceView* pSpecularMap) noexcept
{
	m_pTextureSRVArray[1] = pSpecularMap;

	if (pSpecularMap)
		m_cbPerSubsetCache.mtl.mtlFlag |= static_cast<uint32_t>(MATERIAL_FLAG::UseSpecularMap);
	else
		m_cbPerSubsetCache.mtl.mtlFlag &= ~static_cast<uint32_t>(MATERIAL_FLAG::UseSpecularMap);
}

void BasicEffectPNT::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DIRTY_FLAG::CONSTANTBUFFER_PER_FRAME:
			ApplyPerFrameConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_CAMERA:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_MESH:
			ApplyPerMeshConstantBuffer(pDeviceContext);
			break;
		case DIRTY_FLAG::CONSTANTBUFFER_PER_SUBSET:
			ApplyPerSubsetConstantBuffer(pDeviceContext);
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
	Texture2D tex2d_diffuseMap : register(t0);
	Texture2D tex2d_specularMap : register(t1);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void BasicEffectPNT::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DIRTY_FLAG::ALL;
}

void BasicEffectPNT::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
}

void BasicEffectPNT::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPNT::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT vsStartSlot = 0;
	constexpr UINT psStartSlot = 1;
	pDeviceContext->VSSetConstantBuffers(vsStartSlot, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(psStartSlot, 1, cbs);
}

void BasicEffectPNT::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 1;
	pDeviceContext->VSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPNT::ApplyPerSubsetConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerSubset.Update(pDeviceContext, &m_cbPerSubsetCache);
	ID3D11Buffer* const cbs[] = { m_cbPerSubset.GetComInterface() };

	// PerSubset 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 2;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPNT::ClearTextureSRVArray() noexcept
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}
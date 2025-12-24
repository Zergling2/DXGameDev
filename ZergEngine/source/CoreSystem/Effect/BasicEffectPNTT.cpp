#include <ZergEngine\CoreSystem\Effect\BasicEffectPNTT.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

// BasicPNTT Effect
// VertexShader:
// - ToHcsPNTT
// 
// PixelShader:
// - UnlitPNTTNoMtl
// - LitPNTT
// - LitPNTTDiffMapping
// - LitPNTTSpecMapping
// - LitPNTTNormMapping
// - LitPNTTDiffSpecMapping
// - LitPNTTDiffNormMapping
// - LitPNTTSpecNormMapping
// - LitPNTTDiffSpecNormMapping

static inline int PtrToZeroOrOne(const void* p)
{
	return p != nullptr;
}

void BasicEffectPNTT::Init()
{
	m_dirtyFlag = DirtyFlag::ALL;

	m_pInputLayout = GraphicDevice::GetInstance()->GetILComInterface(VertexFormatType::PositionNormalTangentTexCoord);
	m_pVS = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsPNTT);

	m_pPSUnlitPNTTNoMtl = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::UnlitPNTTNoMtl);
	ID3D11PixelShader* pPSLitPNTT = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTT);
	ID3D11PixelShader* pPSLitPNTTDiffMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTDiffMapping);
	ID3D11PixelShader* pPSLitPNTTSpecMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTSpecMapping);
	ID3D11PixelShader* pPSLitPNTTNormMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTNormMapping);
	ID3D11PixelShader* pPSLitPNTTDiffSpecMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTDiffSpecMapping);
	ID3D11PixelShader* pPSLitPNTTDiffNormMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTDiffNormMapping);
	ID3D11PixelShader* pPSLitPNTTSpecNormMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTSpecNormMapping);
	ID3D11PixelShader* pPSLitPNTTDiffSpecNormMapping = GraphicDevice::GetInstance()->GetPSComInterface(PixelShaderType::LitPNTTDiffSpecNormMapping);

	// Pixel Shader 테이블 세팅
	m_psTable[0][0][0] = pPSLitPNTT;
	m_psTable[0][0][1] = pPSLitPNTTDiffMapping;
	m_psTable[0][1][0] = pPSLitPNTTSpecMapping;
	m_psTable[0][1][1] = pPSLitPNTTDiffSpecMapping;
	m_psTable[1][0][0] = pPSLitPNTTNormMapping;
	m_psTable[1][0][1] = pPSLitPNTTDiffNormMapping;
	m_psTable[1][1][0] = pPSLitPNTTSpecNormMapping;
	m_psTable[1][1][1] = pPSLitPNTTDiffSpecNormMapping;

	m_pCurrPS = m_pPSUnlitPNTTNoMtl;

	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();
	m_cbPerFrame.Init(pDevice);
	m_cbPerCamera.Init(pDevice);
	m_cbPerMesh.Init(pDevice);
	m_cbMaterial.Init(pDevice);

	ClearTextureSRVArray();
}

void BasicEffectPNTT::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerMesh.Release();
	m_cbMaterial.Release();
}

void XM_CALLCONV BasicEffectPNTT::SetAmbientLight(FXMVECTOR ambientLight) noexcept
{
	XMStoreFloat3(&m_cbPerFrameCache.ambientLight, ambientLight);

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPNTT::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPNTT::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPNTT::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::UpdateCBPerFrame;
}

void BasicEffectPNTT::SetCamera(const Camera* pCamera) noexcept
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

	m_dirtyFlag |= DirtyFlag::UpdateCBPerCamera;
}

void XM_CALLCONV BasicEffectPNTT::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerMeshCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerMeshCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DirtyFlag::UpdateCBPerMesh;
}

void BasicEffectPNTT::SetMaterial(const Material* pMaterial)
{
	ID3D11PixelShader* pPS;

	if (!pMaterial)
	{
		pPS = m_pPSUnlitPNTTNoMtl;
	}
	else
	{
		ID3D11ShaderResourceView* pDiffuseMapSRV = pMaterial->m_diffuseMap.GetSRVComInterface();
		ID3D11ShaderResourceView* pSpecularMapSRV = pMaterial->m_specularMap.GetSRVComInterface();
		ID3D11ShaderResourceView* pNormalMapSRV = pMaterial->m_normalMap.GetSRVComInterface();

		m_pTextureSRVArray[0] = pDiffuseMapSRV;
		m_pTextureSRVArray[1] = pSpecularMapSRV;
		m_pTextureSRVArray[2] = pNormalMapSRV;

		// 텍스쳐 매핑 기반 픽셀 셰이더 선택
		const int useDiffuseMap = PtrToZeroOrOne(pDiffuseMapSRV);
		const int useSpecularMap = PtrToZeroOrOne(pSpecularMapSRV);
		const int useNormalMap = PtrToZeroOrOne(pNormalMapSRV);

		pPS = m_psTable[useNormalMap][useSpecularMap][useDiffuseMap];

		// 재질 값 설정
		m_cbMaterialCache.mtl.diffuse = pMaterial->m_diffuse;
		m_cbMaterialCache.mtl.specular = pMaterial->m_specular;
		m_cbMaterialCache.mtl.reflect = pMaterial->m_reflect;

		m_dirtyFlag |= DirtyFlag::UpdateCBMaterial;
	}

	if (m_pCurrPS != pPS)
	{
		m_pCurrPS = pPS;
		m_dirtyFlag |= DirtyFlag::PixelShader;
	}
}

void BasicEffectPNTT::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
{
	DWORD index;

	while (BitScanForward(&index, m_dirtyFlag))
	{
		switch (static_cast<DWORD>(1) << index)
		{
		case DirtyFlag::PrimitiveTopology:
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case DirtyFlag::InputLayout:
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			break;
		case DirtyFlag::Shader:
			ApplyShader(pDeviceContext);
			break;
		case DirtyFlag::PixelShader:
			ApplyPixelShader(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerFrame:
			ApplyPerFrameConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerCamera:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBPerMesh:
			ApplyPerMeshConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::ApplyCBMaterial:
			ApplyMaterialConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::UpdateCBPerFrame:
			m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
			break;
		case DirtyFlag::UpdateCBPerCamera:
			m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
			break;
		case DirtyFlag::UpdateCBPerMesh:
			m_cbPerMesh.Update(pDeviceContext, &m_cbPerMeshCache);
			break;
		case DirtyFlag::UpdateCBMaterial:
			m_cbMaterial.Update(pDeviceContext, &m_cbMaterialCache);
			break;
		default:
			*reinterpret_cast<int*>(0) = 0;	// Force crash;
			break;
		}

		// 업데이트된 항목은 dirty flag 끄기
		m_dirtyFlag &= ~(static_cast<DWORD>(1) << index);
	}

	// 텍스처 리소스는 항상 재설정 (포인터를 들고 있어봤자 메모리 재사용 등으로 인해 이전 리소스 그대로인지 확신할 수 없음..)
	/*
	Texture2D tex2d_diffuseMap : register(t0);
	Texture2D tex2d_specularMap : register(t1);
	Texture2D tex2d_normalMap : register(t2);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void BasicEffectPNTT::OnUnbindFromDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DirtyFlag::ALL;

	const DWORD except =
		DirtyFlag::UpdateCBPerFrame |
		DirtyFlag::UpdateCBPerCamera |
		DirtyFlag::UpdateCBPerMesh |
		DirtyFlag::UpdateCBMaterial;

	m_dirtyFlag = m_dirtyFlag & ~except;
}

void BasicEffectPNTT::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BasicEffectPNTT::ApplyPixelShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BasicEffectPNTT::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT startSlot = 0;
	pDeviceContext->PSSetConstantBuffers(startSlot, 1, cbs);
}

void BasicEffectPNTT::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	constexpr UINT PS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BasicEffectPNTT::ApplyPerMeshConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbPerMesh.GetComInterface() };

	// PerMesh 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void BasicEffectPNTT::ApplyMaterialConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	ID3D11Buffer* const cbs[] = { m_cbMaterial.GetComInterface() };

	// Material 상수버퍼 사용 셰이더
	constexpr UINT PS_SLOT = 2;
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BasicEffectPNTT::ClearTextureSRVArray() noexcept
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}

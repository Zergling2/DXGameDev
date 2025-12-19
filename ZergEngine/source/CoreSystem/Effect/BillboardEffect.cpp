#include <ZergEngine\CoreSystem\Effect\BillboardEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>

using namespace ze;

// Billboard Effect
// VertexShader:
// - ToHcsBillboardQuad
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

void BillboardEffect::Init()
{
	m_pVS = GraphicDevice::GetInstance()->GetVSComInterface(VertexShaderType::ToHcsBillboardQuad);

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
	m_cbPerBillboard.Init(pDevice);
	m_cbMaterial.Init(pDevice);

	ClearTextureSRVArray();
}

void BillboardEffect::Release()
{
	m_cbPerFrame.Release();
	m_cbPerCamera.Release();
	m_cbPerBillboard.Release();
	m_cbMaterial.Release();
}

void XM_CALLCONV BillboardEffect::SetAmbientLight(FXMVECTOR ambientLight) noexcept
{
	XMStoreFloat3(&m_cbPerFrameCache.ambientLight, ambientLight);

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void BillboardEffect::SetDirectionalLight(const DirectionalLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.dlCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.dl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void BillboardEffect::SetPointLight(const PointLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.plCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.pl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void BillboardEffect::SetSpotLight(const SpotLightData* pLights, uint32_t count) noexcept
{
	assert(count <= 4);

	m_cbPerFrameCache.slCount = count;
	for (uint32_t i = 0; i < count; ++i)
		m_cbPerFrameCache.sl[i] = pLights[i];

	m_dirtyFlag |= DirtyFlag::CBPerFrame;
}

void BillboardEffect::SetCamera(const Camera* pCamera) noexcept
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

void XM_CALLCONV BillboardEffect::SetWorldMatrix(FXMMATRIX w) noexcept
{
	XMStoreFloat4x4A(&m_cbPerBillboardCache.w, ConvertToHLSLMatrix(w));			// HLSL 전치
	XMStoreFloat4x4A(&m_cbPerBillboardCache.wInvTr, XMMatrixInverse(nullptr, w));	// 역행렬의 전치의 HLSL 전치

	m_dirtyFlag |= DirtyFlag::CBPerBillboard;
}

void BillboardEffect::SetMaterial(const Material* pMaterial)
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

		m_dirtyFlag |= DirtyFlag::CBMaterial;
	}

	if (m_pCurrPS != pPS)
	{
		m_pCurrPS = pPS;
		m_dirtyFlag |= DirtyFlag::PixelShader;
	}
}

void BillboardEffect::ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept
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
		case DirtyFlag::PixelShader:
			ApplyPixelShader(pDeviceContext);
			break;
		case DirtyFlag::CBPerFrame:
			ApplyPerFrameConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPerCamera:
			ApplyPerCameraConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBPerBillboard:
			ApplyPerBillboardConstantBuffer(pDeviceContext);
			break;
		case DirtyFlag::CBMaterial:
			ApplyMaterialConstantBuffer(pDeviceContext);
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
	Texture2D tex2d_normalMap : register(t2);
	*/
	pDeviceContext->PSSetShaderResources(0, _countof(m_pTextureSRVArray), m_pTextureSRVArray);

	ClearTextureSRVArray();
}

void BillboardEffect::KickedOutOfDeviceContext() noexcept
{
	ClearTextureSRVArray();

	m_dirtyFlag = DirtyFlag::ALL;
}

void BillboardEffect::ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BillboardEffect::ApplyPixelShader(ID3D11DeviceContext* pDeviceContext) noexcept
{
	pDeviceContext->PSSetShader(m_pCurrPS, nullptr, 0);
}

void BillboardEffect::ApplyPerFrameConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerFrame.Update(pDeviceContext, &m_cbPerFrameCache);
	ID3D11Buffer* const cbs[] = { m_cbPerFrame.GetComInterface() };

	// PerFrame 상수버퍼 사용 셰이더
	constexpr UINT PS_SLOT = 0;
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BillboardEffect::ApplyPerCameraConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerCamera.Update(pDeviceContext, &m_cbPerCameraCache);
	ID3D11Buffer* const cbs[] = { m_cbPerCamera.GetComInterface() };

	// PerCamera 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 0;
	constexpr UINT PS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BillboardEffect::ApplyPerBillboardConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbPerBillboard.Update(pDeviceContext, &m_cbPerBillboardCache);
	ID3D11Buffer* const cbs[] = { m_cbPerBillboard.GetComInterface() };

	// PerBillboard 상수버퍼 사용 셰이더
	constexpr UINT VS_SLOT = 1;
	pDeviceContext->VSSetConstantBuffers(VS_SLOT, 1, cbs);
}

void BillboardEffect::ApplyMaterialConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_cbMaterial.Update(pDeviceContext, &m_cbMaterialCache);
	ID3D11Buffer* const cbs[] = { m_cbMaterial.GetComInterface() };

	// Material 상수버퍼 사용 셰이더
	constexpr UINT PS_SLOT = 2;
	pDeviceContext->PSSetConstantBuffers(PS_SLOT, 1, cbs);
}

void BillboardEffect::ClearTextureSRVArray() noexcept
{
	for (size_t i = 0; i < _countof(m_pTextureSRVArray); ++i)
		m_pTextureSRVArray[i] = nullptr;
}

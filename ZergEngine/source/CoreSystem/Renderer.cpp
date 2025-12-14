#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\RenderSettings.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\Resource\StaticMesh.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>


using namespace ze;

Renderer* Renderer::s_pInstance = nullptr;

Renderer::Renderer()
	: m_pCullBackRS(nullptr)
	, m_pCullNoneRS(nullptr)
	, m_pWireFrameRS(nullptr)
	, m_pDefaultDSS(nullptr)
	, m_pSkyboxDSS(nullptr)
	, m_pDepthReadOnlyDSS(nullptr)
	, m_pNoDepthStencilTestDSS(nullptr)
	, m_pOpaqueBS(nullptr)
	, m_pNoColorWriteBS(nullptr)
	, m_pButtonVB(nullptr)
	, m_effectImmediateContext()
	, m_pAnimFinalTransformIdentity(nullptr)
	, m_pAnimFinalTransformBuffer(nullptr)
	// , m_basicEffectP()
	// , m_basicEffectPC()
	// , m_basicEffectPN()
	// , m_basicEffectPT()
	// , m_basicEffectPNT()
	, m_basicEffectPNTT()
	, m_basicEffectPNTTSkinned()
	, m_terrainEffect()
	, m_skyboxEffect()
	, m_drawScreenQuadTex()
	, m_drawScreenQuadMSTex()
	, m_buttonEffect()
	, m_imageEffect()
	, m_uiRenderQueue()
{
	m_uiRenderQueue.reserve(256);

	constexpr size_t XMFLOAT4X4A_ALIGNMENT = 16;

	m_pAnimFinalTransformBufferSpace = reinterpret_cast<XMFLOAT4X4A*>(_aligned_malloc_dbg(sizeof(XMFLOAT4X4A) * MAX_BONE_COUNT * 2, XMFLOAT4X4A_ALIGNMENT, __FILE__, __LINE__));
	m_pAnimFinalTransformIdentity = m_pAnimFinalTransformBufferSpace;
	m_pAnimFinalTransformBuffer = m_pAnimFinalTransformIdentity + MAX_BONE_COUNT;

	for (size_t i = 0; i < MAX_BONE_COUNT; ++i)
		XMStoreFloat4x4A(&m_pAnimFinalTransformIdentity[i], XMMatrixIdentity());
}

Renderer::~Renderer()
{
	_aligned_free_dbg(m_pAnimFinalTransformBufferSpace);
}

void Renderer::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new Renderer();
}

void Renderer::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void Renderer::Init()
{
	m_pCullBackRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerMode::SolidCullBackMultisample);
	m_pCullNoneRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerMode::SolidCullNoneMultisample);
	m_pWireFrameRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerMode::WireframeMultisample);
	m_pDefaultDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::Default);
	m_pSkyboxDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::Skybox);
	m_pDepthReadOnlyDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::DepthReadOnlyLess);
	m_pNoDepthStencilTestDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::NoDepthStencilTest);
	m_pOpaqueBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::Opaque);
	m_pAlphaBlendBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::AlphaBlend);
	m_pNoColorWriteBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::NoColorWrite);
	m_pButtonVB = GraphicDevice::GetInstance()->GetButtonMeshVB();	// Read only vertex buffer

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ INITIALIZE EFFECTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// m_basicEffectP.Init();
	// m_basicEffectPC.Init();
	// m_basicEffectPN.Init();
	// m_basicEffectPT.Init();
	// m_basicEffectPNT.Init();
	m_basicEffectPNTT.Init();
	m_basicEffectPNTTSkinned.Init();
	m_terrainEffect.Init();
	m_skyboxEffect.Init();
	m_drawScreenQuadTex.Init();
	m_drawScreenQuadMSTex.Init();
	m_buttonEffect.Init();
	m_imageEffect.Init();

	// effect context ÁØºñ
	assert(GraphicDevice::GetInstance()->GetImmediateContextComInterface() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(GraphicDevice::GetInstance()->GetImmediateContextComInterface());
}

void Renderer::UnInit()
{
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE EFFECTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// m_basicEffectP.Release();
	// m_basicEffectPC.Release();
	// m_basicEffectPN.Release();
	// m_basicEffectPT.Release();
	// m_basicEffectPNT.Release();
	m_basicEffectPNTT.Release();
	m_basicEffectPNTTSkinned.Release();
	m_terrainEffect.Release();
	m_skyboxEffect.Release();
	m_drawScreenQuadTex.Release();
	m_drawScreenQuadMSTex.Release();
	m_buttonEffect.Release();
	m_imageEffect.Release();
}

void Renderer::RenderFrame()
{
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContextComInterface();

	ID3D11SamplerState* const ssArr[] =
	{
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Anisotropic4x),	// s0 common sampler
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Trilinear),		// s1 normal map sampler
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Bilinear),		// s2 bilinear sampler
	};
	pImmContext->DSSetSamplers(0, _countof(ssArr), ssArr);
	pImmContext->PSSetSamplers(0, _countof(ssArr), ssArr);

	// Rasterizer State
	pImmContext->RSSetState(m_pCullBackRS);		// ÈÄ¸é ÄÃ¸µ ¼³Á¤

	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);

	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);

	// PerFrame »ó¼ö¹öÆÛ ¾÷µ¥ÀÌÆ® ¹× ¹ÙÀÎµù
	{
		DirectionalLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(DirectionalLightManager::GetInstance()->m_directAccessGroup.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : DirectionalLightManager::GetInstance()->m_directAccessGroup)
		{
			if (index >= MAX_GLOBAL_LIGHT_COUNT)
				break;

			const DirectionalLight* pLight = static_cast<const DirectionalLight*>(pLightComponent);
			const GameObject* pGameObject = pLight->m_pGameObject;
			assert(pGameObject != nullptr);

			const XMMATRIX w = pGameObject->m_transform.GetWorldTransformMatrix();
			XMVECTOR scale;
			XMVECTOR rotation;	// Quaternion
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &rotation, &translation, w);

			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;
			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Normalize(XMVector3Rotate(Math::Vector3::Forward(), rotation))
			);

			++index;
		}

		// m_basicEffectP.SetDirectionalLight(light, lightCount);
		// m_basicEffectPC.SetDirectionalLight(light, lightCount);
		// m_basicEffectPN.SetDirectionalLight(light, lightCount);
		// m_basicEffectPT.SetDirectionalLight(light, lightCount);
		// m_basicEffectPNT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNTT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNTTSkinned.SetDirectionalLight(light, lightCount);
		// m_skyboxEffect.SetDirectionalLight(light, lightCount);
		m_terrainEffect.SetDirectionalLight(light, lightCount);
	}

	{
		PointLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(PointLightManager::GetInstance()->m_directAccessGroup.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : PointLightManager::GetInstance()->m_directAccessGroup)
		{
			if (index >= MAX_GLOBAL_LIGHT_COUNT)
				break;

			const PointLight* pLight = static_cast<const PointLight*>(pLightComponent);
			const GameObject* pGameObject = pLight->m_pGameObject;
			assert(pGameObject != nullptr);

			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, pGameObject->m_transform.GetWorldPosition());
			light[index].range = pLight->GetRange();

			light[index].att = pLight->GetDistAtt();

			++index;
		}

		// m_basicEffectP.SetPointLight(light, lightCount);
		// m_basicEffectPC.SetPointLight(light, lightCount);
		// m_basicEffectPN.SetPointLight(light, lightCount);
		// m_basicEffectPT.SetPointLight(light, lightCount);
		// m_basicEffectPNT.SetPointLight(light, lightCount);
		m_basicEffectPNTT.SetPointLight(light, lightCount);
		m_basicEffectPNTTSkinned.SetPointLight(light, lightCount);
		// m_skyboxEffect.SetPointLight(light, lightCount);
		m_terrainEffect.SetPointLight(light, lightCount);
	}

	{
		SpotLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(SpotLightManager::GetInstance()->m_directAccessGroup.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : SpotLightManager::GetInstance()->m_directAccessGroup)
		{
			if (index >= MAX_GLOBAL_LIGHT_COUNT)
				break;

			const SpotLight* pLight = static_cast<const SpotLight*>(pLightComponent);
			const GameObject* pGameObject = pLight->m_pGameObject;
			assert(pGameObject != nullptr);

			const XMMATRIX w = pGameObject->m_transform.GetWorldTransformMatrix();
			XMVECTOR scale;
			XMVECTOR rotation;	// Quaternion
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &rotation, &translation, w);

			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, translation);
			light[index].range = pLight->GetRange();

			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Normalize(XMVector3Rotate(Math::Vector3::Forward(), rotation))
			);

			light[index].att = pLight->GetDistAtt();
			light[index].innerConeCos = std::cos(pLight->GetInnerConeAngle());
			light[index].outerConeCos = std::cos(pLight->GetOuterConeAngle());

			++index;
		}

		// m_basicEffectP.SetSpotLight(light, lightCount);
		// m_basicEffectPC.SetSpotLight(light, lightCount);
		// m_basicEffectPN.SetSpotLight(light, lightCount);
		// m_basicEffectPT.SetSpotLight(light, lightCount);
		// m_basicEffectPNT.SetSpotLight(light, lightCount);
		m_basicEffectPNTT.SetSpotLight(light, lightCount);
		m_basicEffectPNTTSkinned.SetSpotLight(light, lightCount);
		// m_skyboxEffect.SetSpotLight(light, lightCount);
		m_terrainEffect.SetSpotLight(light, lightCount);
	}

	XMVECTOR ambientLight = XMVectorScale(
		XMLoadFloat3(&RenderSettings::GetInstance()->GetAmbientLightColor()),
		RenderSettings::GetInstance()->GetAmbientLightIntensity()
	);
	// m_basicEffectP.SetAmbientLight(ambientLight);
	// m_basicEffectPC.SetAmbientLight(ambientLight);
	// m_basicEffectPN.SetAmbientLight(ambientLight);
	// m_basicEffectPT.SetAmbientLight(ambientLight);
	// m_basicEffectPNT.SetAmbientLight(ambientLight);
	m_basicEffectPNTT.SetAmbientLight(ambientLight);
	m_basicEffectPNTTSkinned.SetAmbientLight(ambientLight);
	// m_skyboxEffect.SetAmbientLight(ambientLightt);
	m_terrainEffect.SetAmbientLight(ambientLight);

	// Camera¸¶´Ù ÇÁ·¹ÀÓ ·»´õ¸µ
	for (IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		pCamera->UpdateViewMatrix();	// ºä º¯È¯ Çà·Ä ¾÷µ¥ÀÌÆ®
		Frustum worldSpaceFrustum;
		Frustum(pCamera->GetProjMatrix(), false).Transform(worldSpaceFrustum, XMMatrixInverse(nullptr, pCamera->GetViewMatrix()));
		// Frustum cameraFrustumW;			// ÇÁ·¯½ºÅÒ ÄÃ¸µ¿ë
		// Math::CalcWorldFrustumFromViewProjMatrix(pCamera->GetViewMatrix()* pCamera->GetProjMatrix(), cameraFrustumW);

		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// ºäÆ÷Æ® ¹ÙÀÎµù
		pImmContext->RSSetViewports(1, &pCamera->GetEntireBufferViewport());
		// ÄÃ·¯ ¹öÆÛ ¹× µª½º½ºÅÙ½Ç ¹öÆÛ ¹ÙÀÎµù
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// PerCamera »ó¼ö¹öÆÛ ¾÷µ¥ÀÌÆ® ¹× ¹ÙÀÎµù
		// m_basicEffectP.SetCamera(pCamera);
		// m_basicEffectPC.SetCamera(pCamera);
		// m_basicEffectPN.SetCamera(pCamera);
		// m_basicEffectPT.SetCamera(pCamera);
		// m_basicEffectPNT.SetCamera(pCamera);
		m_basicEffectPNTT.SetCamera(pCamera);
		m_basicEffectPNTTSkinned.SetCamera(pCamera);
		m_skyboxEffect.SetCamera(pCamera);
		m_terrainEffect.SetCamera(pCamera);

		for (const IComponent* pComponent : MeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;
			
			// ÇÁ·¯½ºÅÒ ÄÃ¸µ
			Aabb worldSpaceAabb;
			pMesh->GetAabb().Transform(worldSpaceAabb, pMeshRenderer->m_pGameObject->m_transform.GetWorldTransformMatrix());
			if (!Math::TestFrustumAabbCollision(worldSpaceFrustum, worldSpaceAabb))
				continue;

			/*
			switch (pMesh->GetVertexFormatType())
			{
			case VertexFormatType::Position:
				RenderVFPositionMesh(pMeshRenderer);
				break;
			case VertexFormatType::PositionColor:
				RenderVFPositionColorMesh(pMeshRenderer);
				break;
			case VertexFormatType::PositionNormal:
				RenderVFPositionNormalMesh(pMeshRenderer);
				break;
			case VertexFormatType::PositionTexCoord:
				RenderVFPositionTexCoordMesh(pMeshRenderer);
				break;
			case VertexFormatType::PositionNormalTexCoord:
				RenderVFPositionNormalTexCoordMesh(pMeshRenderer);
				break;
			case VertexFormatType::PositionNormalTangentTexCoord:
				RenderVFPositionNormalTangentTexCoordMesh(pMeshRenderer);
				break;
			case VertexFormatType::COUNT:
				__fallthrough;
			case VertexFormatType::UNKNOWN:
				*reinterpret_cast<int*>(0) = 0;
				break;
			}
			*/

			RenderVFPositionNormalTangentTexCoordMesh(pMeshRenderer);
		}

		for (const IComponent* pComponent : SkinnedMeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const SkinnedMeshRenderer* pMeshRenderer = static_cast<const SkinnedMeshRenderer*>(pComponent);
			const SkinnedMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;

			// ÇÁ·¯½ºÅÒ ÄÃ¸µ
			// Aabb worldSpaceAabb;
			// pMesh->GetAabb().Transform(worldSpaceAabb, pMeshRenderer->m_pGameObject->m_transform.GetWorldTransformMatrix());
			// if (!Math::TestFrustumAabbCollision(worldSpaceFrustum, worldSpaceAabb))
			// 	continue;

			RenderVFPositionNormalTangentTexCoordSkinnedMesh(pMeshRenderer);
		}

		// ÁöÇü ·»´õ¸µ
		for (const IComponent* pComponent : TerrainManager::GetInstance()->m_directAccessGroup)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			RenderTerrain(pTerrain);
		}

		// ½ºÄ«ÀÌ¹Ú½º ·»´õ¸µ
		ID3D11ShaderResourceView* pSkyboxCubeMap = RenderSettings::GetInstance()->m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pSkyboxDSS, 0);
			RenderSkybox(pSkyboxCubeMap);
			pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);
		}
	}

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Ä«¸Þ¶ó ·»´õ¸µ °á°ú º´ÇÕ ¹× ½º¿ÒÃ¼ÀÎ ¹öÆÛ¿¡ UI ·»´õ¸µ
	// Rasterizer State
	pImmContext->RSSetState(m_pCullNoneRS);	// Quad ·»´õ¸µ »ÓÀÌ¹Ç·Î ÈÄ¸é ÄÃ¸µ ²ô±â
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);
	
	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);
	
	// ÀüÃ¼ ¹é¹öÆÛ¿¡ ´ëÇÑ ºäÆ÷Æ® ¼³Á¤
	pImmContext->RSSetViewports(1, &GraphicDevice::GetInstance()->GetEntireSwapChainViewport());
	// ·»´õÅ¸°Ù ¹ÙÀÎµù
	ID3D11RenderTargetView* pColorBufferRTV = GraphicDevice::GetInstance()->GetSwapChainRTVComInterface();
	ID3D11DepthStencilView* pDepthStencilBufferDSV = GraphicDevice::GetInstance()->GetSwapChainDSVComInterface();
	pImmContext->ClearRenderTargetView(pColorBufferRTV, DirectX::ColorsLinear::Blue);
	pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
	
	for (const IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		const Camera* pCamera = static_cast<const Camera*>(pComponent);
		if (!pCamera->IsEnabled())
			continue;

		if (pCamera->m_msaaMode != MSAAMode::Off)
		{
			m_drawScreenQuadMSTex.SetQuadParameters(
				pCamera->m_viewportRect.m_width,
				pCamera->m_viewportRect.m_height,
				pCamera->m_viewportRect.m_x,
				pCamera->m_viewportRect.m_y
			);
			m_drawScreenQuadMSTex.SetTexture(pCamera->GetColorBufferSRVComInterface());

			m_effectImmediateContext.Apply(&m_drawScreenQuadMSTex);

			m_effectImmediateContext.Draw(4, 0);
		}
		else
		{
			m_drawScreenQuadTex.SetQuadParameters(
				pCamera->m_viewportRect.m_width,
				pCamera->m_viewportRect.m_height,
				pCamera->m_viewportRect.m_x,
				pCamera->m_viewportRect.m_y
			);
			m_drawScreenQuadTex.SetTexture(pCamera->GetColorBufferSRVComInterface());

			m_effectImmediateContext.Apply(&m_drawScreenQuadTex);

			m_effectImmediateContext.Draw(4, 0);
		}
	}

	{
		// ´ÜÀÏ Ä«¸Þ¶ó »ç¿ëÇÏ´Â ¾À¿¡¼­ ´ÙÀ½ ÇÁ·¹ÀÓ¿¡ µ¿ÀÏ Ä«¸Þ¶ó ¹öÆÛ°¡ Input/OutputÀ¸·Î µ¿½Ã¿¡ ¹ÙÀÎµù µÇ´Â°ÍÀ» ¹æÁö
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_effectImmediateContext.GetDeviceContextComInterface()->PSSetShaderResources(0, 1, srvs);
	}
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// UI ·»´õ¸µ
	{
		// Rasterizer State
		pImmContext->RSSetState(m_pCullNoneRS);	// Quad ·»´õ¸µ »ÓÀÌ¹Ç·Î ÈÄ¸é ÄÃ¸µ ²ô±â

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);

		// D3D11_BLEND_BLEND_FACTOR ¶Ç´Â D3D11_BLEND_INV_BLEND_FACTOR ¹Ì»ç¿ë (blend factor·Î nullptr Àü´Þ)
		pImmContext->OMSetBlendState(m_pAlphaBlendBS, nullptr, 0xFFFFFFFF);

		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// ¸ðµç UI Effectµé¿¡ ´ëÇÑ ScreenToNDCSpaceRatio ¼³Á¤
		const XMFLOAT2 screenToNDCSpaceRatio = XMFLOAT2(
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Height)
		);
		m_buttonEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_imageEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

		// ¸ðµç ·çÆ® UI¿ÀºêÁ§Æ®µéºÎÅÍ ¼øÈ¸ÇÏ¸ç ÀÚ½Ä UI±îÁö ·»´õ¸µ
		for (const IUIObject* pRootUIObject : UIObjectManager::GetInstance()->m_roots)
		{
			assert(pRootUIObject->IsRoot());

			size_t index = 0;

			// Å¥´Â ³ëµå ÇÒ´çÀÌ ÀÏ¾î³ª¹Ç·Î ¹è¿­À» »ç¿ë.

			if (pRootUIObject->IsActive())
				m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// ÀÚ½ÄµéÀ» ¸ðµÎ Å¥¿¡ ³Ö´Â´Ù.
				for (const RectTransform* pChildTransform : pUIObject->m_transform.m_children)
				{
					const IUIObject* pChildUIObject = pChildTransform->m_pUIObject;
					assert(pChildUIObject != nullptr);

					if (pChildUIObject->IsActive())
						m_uiRenderQueue.push_back(pChildUIObject);
				}

				switch (pUIObject->GetType())
				{
				case UIOBJECT_TYPE::PANEL:
					this->RenderPanel(static_cast<const Panel*>(pUIObject));
					break;
				case UIOBJECT_TYPE::IMAGE:
					this->RenderImage(static_cast<const Image*>(pUIObject));
					break;
				case UIOBJECT_TYPE::TEXT:
					this->RenderText(static_cast<const Text*>(pUIObject));
					break;
				case UIOBJECT_TYPE::INPUT_FIELD:
					this->RenderInputField(static_cast<const InputField*>(pUIObject));
					break;
				case UIOBJECT_TYPE::BUTTON:
					this->RenderButton(static_cast<const Button*>(pUIObject));
					break;
				case UIOBJECT_TYPE::IMAGE_BUTTON:
					break;
				default:
					break;
				}

				++index;
			}

			// ·»´õ¸µÀÌ ¿Ï·áµÈ ÈÄ Å¥ Å¬¸®¾î
			m_uiRenderQueue.clear();
		}
	}

	HRESULT hr = GraphicDevice::GetInstance()->GetSwapChainComInterface()->Present(1, 0);
}

/*
void Renderer::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::Position);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectP.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::Position) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Â ¼øÈ¸ ½Ã º¯°æµÇ´Â »çÇ×ÀÌ ¾øÀ¸¹Ç·Î ·çÇÁ ¹Û¿¡¼­ Apply ÇÑ ¹ø¸¸ È£Ãâ
	m_effectImmediateContext.Apply(&m_basicEffectP);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const MeshSubset& subset : pMesh->m_subsets)
	{
		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}
*/

/*
void Renderer::RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionColor);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPC.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionColor) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Â ¼øÈ¸ ½Ã º¯°æµÇ´Â »çÇ×ÀÌ ¾øÀ¸¹Ç·Î ·çÇÁ ¹Û¿¡¼­ Apply ÇÑ ¹ø¸¸ È£Ãâ
	m_effectImmediateContext.Apply(&m_basicEffectPC);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const MeshSubset& subset : pMesh->m_subsets)
	{
		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}
*/

/*
void Renderer::RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormal);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPN.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormal) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPN.UseMaterial(true);
			m_basicEffectPN.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPN.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
		}
		else
		{
			m_basicEffectPN.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPN);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}
*/

/*
void Renderer::RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionTexCoord);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
			m_basicEffectPT.SetTexture(pMaterial->m_diffuseMap.GetSRVComInterface());
		else
			m_basicEffectPT.SetTexture(nullptr);

		m_effectImmediateContext.Apply(&m_basicEffectPT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}
*/

/*
void Renderer::RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTexCoord);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPNT.UseMaterial(true);
			m_basicEffectPNT.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPNT.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
			m_basicEffectPNT.SetDiffuseMap(pMaterial->m_diffuseMap.GetSRVComInterface());
			m_basicEffectPNT.SetSpecularMap(pMaterial->m_specularMap.GetSRVComInterface());
		}
		else
		{
			m_basicEffectPNT.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPNT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}
*/

void Renderer::RenderVFPositionNormalTangentTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTangentTexCoord);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNTT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTangentTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPNTT.UseMaterial(true);
			m_basicEffectPNTT.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPNTT.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
			m_basicEffectPNTT.SetDiffuseMap(pMaterial->m_diffuseMap.GetSRVComInterface());
			m_basicEffectPNTT.SetSpecularMap(pMaterial->m_specularMap.GetSRVComInterface());
			m_basicEffectPNTT.SetNormalMap(pMaterial->m_normalMap.GetSRVComInterface());
		}
		else
		{
			m_basicEffectPNTT.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPNTT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

#include <ZergEngine\CoreSystem\Input.h>
void Renderer::RenderVFPositionNormalTangentTexCoordSkinnedMesh(const SkinnedMeshRenderer* pSkinnedMeshRenderer)
{
	const SkinnedMesh* pMesh = pSkinnedMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	const GameObject* pGameObject = pSkinnedMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNTTSkinned.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());
	
	const Animation* pCurrAnim = pSkinnedMeshRenderer->GetCurrentAnimation();
	const Armature* pArmature = pSkinnedMeshRenderer->GetArmaturePtr();
	if (pCurrAnim)
	{
		pCurrAnim->ComputeFinalTransform(
			pSkinnedMeshRenderer->GetAnimationTimeCursor(),
			m_pAnimFinalTransformBuffer,
			pArmature->GetBoneCount()
		);

		for (size_t i = 0; i < pArmature->GetBoneCount(); ++i)
		{
			XMMATRIX m = XMLoadFloat4x4A(&m_pAnimFinalTransformBuffer[i]);
			m = ConvertToHLSLMatrix(m);
			XMStoreFloat4x4A(&m_pAnimFinalTransformBuffer[i], m);
		}
		m_basicEffectPNTTSkinned.SetArmatureFinalTransform(m_pAnimFinalTransformBuffer, pArmature->GetBoneCount());
	}
	else
	{
		m_basicEffectPNTTSkinned.SetArmatureFinalTransform(m_pAnimFinalTransformIdentity, pArmature->GetBoneCount());
	}

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTangentTexCoordSkinned) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	assert(pMesh->m_subsets.size() == pSkinnedMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pSkinnedMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPNTTSkinned.UseMaterial(true);
			m_basicEffectPNTTSkinned.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPNTTSkinned.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
			m_basicEffectPNTTSkinned.SetDiffuseMap(pMaterial->m_diffuseMap.GetSRVComInterface());
			m_basicEffectPNTTSkinned.SetSpecularMap(pMaterial->m_specularMap.GetSRVComInterface());
			m_basicEffectPNTTSkinned.SetNormalMap(pMaterial->m_normalMap.GetSRVComInterface());
		}
		else
		{
			m_basicEffectPNTTSkinned.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPNTTSkinned);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderTerrain(const Terrain* pTerrain)
{
	m_terrainEffect.SetMaxHeight(pTerrain->GetMaxHeight());
	m_terrainEffect.SetTilingScale(pTerrain->GetTilingScale());
	m_terrainEffect.SetFieldMap(pTerrain->m_heightMap.GetSRVComInterface(), pTerrain->m_normalMap.GetSRVComInterface());
	m_terrainEffect.SetLayerTexture(
		pTerrain->m_diffuseMapLayer.GetSRVComInterface(),
		pTerrain->m_normalMapLayer.GetSRVComInterface(),
		pTerrain->m_blendMap.GetSRVComInterface()
	);

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::TerrainPatchCtrlPt) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pTerrain->GetPatchControlPointBufferComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferComInterface(), DXGI_FORMAT_R32_UINT, 0);

	m_effectImmediateContext.Apply(&m_terrainEffect);
	m_effectImmediateContext.DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}

void Renderer::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkybox(pSkyboxCubeMapSRV);

	m_effectImmediateContext.Apply(&m_skyboxEffect);

	// µå·Î¿ì
	// ¼ÎÀÌ´õ Áö¿ªº¯¼ö Á¤Á¡µé »ç¿ë (ÃÑ 36°³)
	m_effectImmediateContext.Draw(36, 0);
}

void Renderer::RenderPanel(const Panel* pPanel)
{
	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
	ID2D1SolidColorBrush* pSolidColorBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();
	const RectTransform& transform = pPanel->m_transform;
	XMFLOAT2A windowsRectCenter;
	XMStoreFloat2A(&windowsRectCenter, transform.GetWindowsScreenPosition());

	D2D1_ROUNDED_RECT shapeInfo;
	shapeInfo.rect.left = windowsRectCenter.x - pPanel->m_halfSize.x;
	shapeInfo.rect.right = shapeInfo.rect.left + pPanel->m_size.x;
	shapeInfo.rect.top = windowsRectCenter.y - pPanel->m_halfSize.y;
	shapeInfo.rect.bottom = shapeInfo.rect.top + pPanel->m_size.y;

	pSolidColorBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pPanel->GetColor()));


	// ################ BEGIN DRAW ################
	pD2DRenderTarget->BeginDraw();

	switch (pPanel->GetShape())
	{
	case PANEL_SHAPE::RECTANGLE:
		pD2DRenderTarget->FillRectangle(&shapeInfo.rect, pSolidColorBrush);
		break;
	case PANEL_SHAPE::ROUNDED_RECTANGLE:
		shapeInfo.radiusX = pPanel->GetRadiusX();
		shapeInfo.radiusY = pPanel->GetRadiusY();
		pD2DRenderTarget->FillRoundedRectangle(&shapeInfo, pSolidColorBrush);
		break;
	default:
		break;
	}

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderImage(const Image* pImage)
{
	m_imageEffect.SetPreNDCPosition(pImage->m_transform.GetPreNDCPosition());
	m_imageEffect.SetSize(pImage->GetSizeVector());
	m_imageEffect.SetImageTexture(pImage->GetTexture());

	m_effectImmediateContext.Apply(&m_imageEffect);
	m_effectImmediateContext.Draw(4, 0);
}

void Renderer::RenderText(const Text* pText)
{
	UINT32 textLength = static_cast<UINT32>(pText->GetTextLength());
	if (textLength == 0)
		return;

	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
	ID2D1SolidColorBrush* pBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();
	IDWriteTextFormat* pDWriteTextFormat = pText->GetDWriteTextFormatComInterface();

	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pText->GetColor()));
	pDWriteTextFormat->SetTextAlignment(pText->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pText->GetParagraphAlignment());

	const RectTransform& transform = pText->m_transform;
	D2D1_RECT_F layoutRect;
	XMFLOAT2A windowsButtonCenter;
	XMStoreFloat2A(&windowsButtonCenter, transform.GetWindowsScreenPosition());
	if (pText->GetType() == UIOBJECT_TYPE::BUTTON)
	{
		if (static_cast<const Button*>(pText)->IsPressed())	// ¹öÆ°ÀÌ ´­¸° »óÅÂÀÌ¸é ÅØ½ºÆ® À§Ä¡µµ »ìÂ¦ ¿ìÇÏ´ÜÀ¸·Î ³»·Á¼­ ÀÔÃ¼°¨ ºÎ¿©
		{
			windowsButtonCenter.x += 1.0f;
			windowsButtonCenter.y += 1.0f;
		}
	}

	layoutRect.left = windowsButtonCenter.x - pText->m_halfSize.x;
	layoutRect.right = layoutRect.left + pText->m_size.x;
	layoutRect.top = windowsButtonCenter.y - pText->m_halfSize.y;
	layoutRect.bottom = layoutRect.top + pText->m_size.y;


	// ################ BEGIN DRAW ################
	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->DrawTextW(
		pText->GetText(),
		textLength,
		pDWriteTextFormat,
		layoutRect,
		pBrush
	);

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderButton(const Button* pButton)
{
	// 1. ¹öÆ° ÇÁ·¹ÀÓ ·»´õ¸µ
	ID3D11Buffer* vbs[] = { m_pButtonVB };
	UINT strides[] = { sizeof(VFButton) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_buttonEffect.SetPressed(pButton->IsPressed());
	m_buttonEffect.SetColor(pButton->GetButtonColorVector());
	m_buttonEffect.SetPreNDCPosition(pButton->m_transform.GetPreNDCPosition());
	m_buttonEffect.SetSize(pButton->GetSizeVector());

	m_effectImmediateContext.Apply(&m_buttonEffect);
	m_effectImmediateContext.Draw(30, 0);

	// 2. ¹öÆ° ÅØ½ºÆ® ·»´õ¸µ
	this->RenderText(static_cast<const Text*>(pButton));
}

void Renderer::RenderInputField(const InputField* pInputField)
{
	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
	ID2D1SolidColorBrush* pBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();

	// 1. Input Field ¹è°æ ·»´õ¸µ
	const RectTransform& transform = pInputField->m_transform;
	XMFLOAT2A windowsRectCenter;
	XMStoreFloat2A(&windowsRectCenter, transform.GetWindowsScreenPosition());

	D2D1_ROUNDED_RECT shapeInfo;
	shapeInfo.rect.left = windowsRectCenter.x - pInputField->m_halfSize.x;
	shapeInfo.rect.right = shapeInfo.rect.left + pInputField->m_size.x;
	shapeInfo.rect.top = windowsRectCenter.y - pInputField->m_halfSize.y;
	shapeInfo.rect.bottom = shapeInfo.rect.top + pInputField->m_size.y;

	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pInputField->GetBkColor()));

	// ################ BEGIN DRAW ################
	pD2DRenderTarget->BeginDraw();
	do
	{
		switch (pInputField->GetShape())
		{
		case INPUT_FIELD_SHAPE::RECTANGLE:
			pD2DRenderTarget->FillRectangle(&shapeInfo.rect, pBrush);
			break;
		case INPUT_FIELD_SHAPE::ROUNDED_RECTANGLE:
			shapeInfo.radiusX = pInputField->GetRadiusX();
			shapeInfo.radiusY = pInputField->GetRadiusY();
			pD2DRenderTarget->FillRoundedRectangle(&shapeInfo, pBrush);
			break;
		default:
			break;
		}

		// 2. Text ·»´õ¸µ
		UINT32 textLength = static_cast<UINT32>(pInputField->GetTextLength());
		if (textLength == 0)
			break;

		IDWriteTextFormat* pDWriteTextFormat = pInputField->GetDWriteTextFormatComInterface();

		pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pInputField->GetColor()));
		pDWriteTextFormat->SetTextAlignment(pInputField->GetTextAlignment());
		pDWriteTextFormat->SetParagraphAlignment(pInputField->GetParagraphAlignment());
		const D2D1_RECT_F& layoutRect = shapeInfo.rect;
		
		static PCWSTR pwc = L"********************************";
		constexpr size_t pwcl = 32;
		PCTSTR pContent;
		PWSTR pBuffer = nullptr;
		if (pInputField->IsPassword())
		{
			if (textLength <= pwcl)	// ÃÖ´ë 32°³ÀÇ *±îÁö´Â µ¿ÀûÇÒ´ç X
				pContent = pwc;
			else
			{
				pBuffer = new WCHAR[textLength + 1];
				for (size_t i = 0; i < textLength; ++i)
					pBuffer[i] = L'*';
				pBuffer[textLength] = L'\0';
				pContent = pBuffer;
			}
		}
		else
		{
			pContent = pInputField->GetText();
		}
		pD2DRenderTarget->DrawTextW(
			pContent,
			textLength,
			pDWriteTextFormat,
			layoutRect,
			pBrush
		);

		if (pBuffer != nullptr)
			delete[] pBuffer;
	} while (false);

	pD2DRenderTarget->EndDraw();
}

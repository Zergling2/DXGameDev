#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>
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
	, m_basicEffectP()
	, m_basicEffectPC()
	, m_basicEffectPN()
	, m_basicEffectPT()
	, m_basicEffectPNT()
	// , m_terrainEffect()
	, m_skyboxEffect()
	, m_msCameraMergeEffect()
	, m_buttonEffect()
	, m_imageEffect()
	, m_uiRenderQueue()
{
	m_uiRenderQueue.reserve(256);
}

Renderer::~Renderer()
{
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
	m_pCullBackRS = GraphicDevice::GetInstance()->GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK);
	m_pCullNoneRS = GraphicDevice::GetInstance()->GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::NONE);
	m_pWireFrameRS = GraphicDevice::GetInstance()->GetRSComInterface(RASTERIZER_FILL_MODE::WIREFRAME, RASTERIZER_CULL_MODE::NONE);
	m_pDefaultDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEFAULT);
	m_pSkyboxDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DEPTH_STENCIL_STATETYPE::SKYBOX);
	m_pDepthReadOnlyDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEPTH_READ_ONLY);
	m_pNoDepthStencilTestDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DEPTH_STENCIL_STATETYPE::NO_DEPTH_STENCILTEST);
	m_pOpaqueBS = GraphicDevice::GetInstance()->GetBSComInterface(BLEND_STATETYPE::OPAQUE_);
	m_pAlphaBlendBS = GraphicDevice::GetInstance()->GetBSComInterface(BLEND_STATETYPE::ALPHABLEND);
	m_pNoColorWriteBS = GraphicDevice::GetInstance()->GetBSComInterface(BLEND_STATETYPE::NO_COLOR_WRITE);
	m_pButtonVB = GraphicDevice::GetInstance()->GetVBComInterface(VERTEX_BUFFER_TYPE::BUTTON);	// Read only vertex buffer

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ INITIALIZE EFFECTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->InitializeEffects();

	// effect context ÁØºñ
	assert(GraphicDevice::GetInstance()->GetImmediateContextComInterface() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(GraphicDevice::GetInstance()->GetImmediateContextComInterface());

	/*
	m_passTerrain.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_passTerrain.SetInputLayout(GraphicDevice::GetInstance()->GetILComInterface(VERTEX_FORMAT_TYPE::TERRAIN_PATCH_CTRL_PT));
	m_passTerrain.SetVertexShader(GraphicDevice::GetInstance()->GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT));
	m_passTerrain.SetHullShader(GraphicDevice::GetInstance()->GetHSComInterface(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR));
	m_passTerrain.SetDomainShader(GraphicDevice::GetInstance()->GetDSComInterface(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP));
	m_passTerrain.SetPixelShader(GraphicDevice::GetInstance()->GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT));
	m_passTerrain.SetRasterizerState(GraphicDevice::GetInstance()->GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passTerrain.SetDepthStencilState(GraphicDevice::GetInstance()->GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEFAULT), 0);
	*/
}

void Renderer::UnInit()
{
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬ RELEASE EFFECTS ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	this->ReleaseEffects();
}

void Renderer::InitializeEffects()
{
	m_basicEffectP.Init();
	m_basicEffectPC.Init();
	m_basicEffectPN.Init();
	m_basicEffectPT.Init();
	m_basicEffectPNT.Init();
	m_skyboxEffect.Init();
	// m_terrainEffect.Init();
	m_msCameraMergeEffect.Init();
	m_buttonEffect.Init();
	m_imageEffect.Init();
}

void Renderer::ReleaseEffects()
{
	m_basicEffectP.Release();
	m_basicEffectPC.Release();
	m_basicEffectPN.Release();
	m_basicEffectPT.Release();
	m_basicEffectPNT.Release();
	m_skyboxEffect.Release();
	// m_terrainEffect.Release();
	m_msCameraMergeEffect.Release();
	m_buttonEffect.Release();
	m_imageEffect.Release();
}

void Renderer::RenderFrame()
{
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContextComInterface();

	// BasicEffect¿¡¼­ »ç¿ëÇÏ´Â ·»´õ »óÅÂ ¼³Á¤
	// ÃßÈÄ ·±Å¸ÀÓ¿¡ º¯°æ °¡´ÉÇÏ°Ô ¼öÁ¤
	// Sampler State
	constexpr TEXTURE_FILTERING_OPTION meshTexFilter = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;
	constexpr TEXTURE_FILTERING_OPTION terrainTexFilter = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;
	ID3D11SamplerState* const ssArr[] =
	{
		GraphicDevice::GetInstance()->GetSSComInterface(meshTexFilter),		// (s0) mesh texture sampler
		GraphicDevice::GetInstance()->GetSkyboxSamplerComInterface(),		// (s1)
		GraphicDevice::GetInstance()->GetSSComInterface(terrainTexFilter),	// (s2) terrain color texture sampler
		GraphicDevice::GetInstance()->GetHeightmapSamplerComInterface()		// (s3)
	};

	pImmContext->VSSetSamplers(0, _countof(ssArr), ssArr);	// ÅÍ·¹ÀÎ ÀÌÆåÆ®¿¡¼­
	pImmContext->DSSetSamplers(0, _countof(ssArr), ssArr);	// ÅÍ·¹ÀÎ ÀÌÆåÆ®¿¡¼­
	pImmContext->PSSetSamplers(0, _countof(ssArr), ssArr);	// ´ëºÎºÐ ÀÌÆåÆ®¿¡¼­

	// Rasterizer State
	pImmContext->RSSetState(m_pCullBackRS);					// ÈÄ¸é ÄÃ¸µ ¼³Á¤
	
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
			XMVECTOR transation;
			XMMatrixDecompose(&scale, &rotation, &transation, w);

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;
			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Rotate(Math::Vector3::FORWARD, rotation)
			);

			++index;
		}

		// m_basicEffectP.SetDirectionalLight(light, lightCount);
		// m_basicEffectPC.SetDirectionalLight(light, lightCount);
		m_basicEffectPN.SetDirectionalLight(light, lightCount);
		// m_basicEffectPT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNT.SetDirectionalLight(light, lightCount);
		// m_skyboxEffect.SetDirectionalLight(light, lightCount);
		// m_terrainEffect.SetDirectionalLight(light, lightCount);
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

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, pGameObject->m_transform.GetWorldPosition());
			light[index].range = pLight->GetRange();

			light[index].att = pLight->GetAtt();

			++index;
		}

		// m_basicEffectP.SetPointLight(light, lightCount);
		// m_basicEffectPC.SetPointLight(light, lightCount);
		m_basicEffectPN.SetPointLight(light, lightCount);
		// m_basicEffectPT.SetPointLight(light, lightCount);
		m_basicEffectPNT.SetPointLight(light, lightCount);
		// m_skyboxEffect.SetPointLight(light, lightCount);
		// m_terrainEffect.SetPointLight(light, lightCount);
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
			XMVECTOR transation;
			XMMatrixDecompose(&scale, &rotation, &transation, w);

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, transation);
			light[index].range = pLight->m_range;

			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Rotate(Math::Vector3::FORWARD, rotation)
			);
			light[index].spotExp = pLight->m_spotExp;

			light[index].att = pLight->m_att;

			++index;
		}

		// m_basicEffectP.SetSpotLight(light, lightCount);
		// m_basicEffectPC.SetSpotLight(light, lightCount);
		m_basicEffectPN.SetSpotLight(light, lightCount);
		// m_basicEffectPT.SetSpotLight(light, lightCount);
		m_basicEffectPNT.SetSpotLight(light, lightCount);
		// m_skyboxEffect.SetSpotLight(light, lightCount);
		// m_terrainEffect.SetSpotLight(light, lightCount);
	}

	// Camera¸¶´Ù ÇÁ·¹ÀÓ ·»´õ¸µ
	for (IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		pCamera->UpdateViewMatrix();	// ºä º¯È¯ Çà·Ä ¾÷µ¥ÀÌÆ®

		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// ºäÆ÷Æ® ¹ÙÀÎµù
		pImmContext->RSSetViewports(1, &pCamera->GetEntireBufferViewport());
		// ÄÃ·¯ ¹öÆÛ ¹× µª½º½ºÅÙ½Ç ¹öÆÛ ¹ÙÀÎµù
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		// pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearRenderTargetView(pColorBufferRTV, Colors::Red);
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		OutputDebugString(_T("Scene Rendering OMSetRenderTargets\n"));
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// PerCamera »ó¼ö¹öÆÛ ¾÷µ¥ÀÌÆ® ¹× ¹ÙÀÎµù
		m_basicEffectP.SetCamera(pCamera);
		m_basicEffectPC.SetCamera(pCamera);
		m_basicEffectPN.SetCamera(pCamera);
		m_basicEffectPT.SetCamera(pCamera);
		m_basicEffectPNT.SetCamera(pCamera);
		m_skyboxEffect.SetCamera(pCamera);
		// m_terrainEffect.SetCamera(pCamera);

		for (const IComponent* pComponent : MeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			if (!pMeshRenderer->IsEnabled() || pMeshRenderer->m_mesh == nullptr)
				continue;

			switch (pMeshRenderer->m_mesh->GetVertexFormatType())
			{
			case VERTEX_FORMAT_TYPE::POSITION:
				RenderVFPositionMesh(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_COLOR:
				RenderVFPositionColorMesh(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
				RenderVFPositionNormalMesh(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
				RenderVFPositionTexCoordMesh(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
				RenderVFPositionNormalTexCoordMesh(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::COUNT:
				__fallthrough;
			case VERTEX_FORMAT_TYPE::UNKNOWN:
				*reinterpret_cast<int*>(0) = 0;
				break;
			}
		}

		// ÁöÇü ·»´õ¸µ
		for (const IComponent* pComponent : TerrainManager::GetInstance()->m_directAccessGroup)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			// RenderTerrain(pTerrain);
		}

		// ½ºÄ«ÀÌ¹Ú½º ·»´õ¸µ
		ID3D11ShaderResourceView* pSkyboxCubeMap = Environment::GetInstance()->m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pSkyboxDSS, 0);
			RenderSkybox(pSkyboxCubeMap);
		}
	}

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Ä«¸Þ¶ó ·»´õ¸µ °á°ú º´ÇÕ ¹× UI ·»´õ¸µ
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
	pImmContext->ClearRenderTargetView(pColorBufferRTV, DirectX::Colors::Blue);
	pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
	OutputDebugString(_T("Camera Merge OMSetRenderTargets\n"));
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
	
	for (const IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		const Camera* pCamera = static_cast<const Camera*>(pComponent);
		if (!pCamera->IsEnabled())
			continue;
	
		m_msCameraMergeEffect.SetMergeParameters(
			pCamera->m_viewportRect.m_width,
			pCamera->m_viewportRect.m_height,
			pCamera->m_viewportRect.m_x,
			pCamera->m_viewportRect.m_y
		);
		m_msCameraMergeEffect.SetMergeTexture(pCamera->GetColorBufferSRVComInterface());
	
		m_effectImmediateContext.Apply(&m_msCameraMergeEffect);
	
		m_effectImmediateContext.Draw(4, 0);
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

			// Å¥´Â ³ëµå ÇÒ´çÀÌ ÀÏ¾î³ª¹Ç·Î ¹è¿­À» »ç¿ë
			m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// ÀÚ½ÄµéÀ» ¸ðµÎ Å¥¿¡ ³Ö´Â´Ù.
				for (const RectTransform* pChildTransform : pUIObject->m_transform.m_children)
				{
					const IUIObject* pChildUIObject = pChildTransform->m_pUIObject;
					assert(pChildUIObject != nullptr);

					m_uiRenderQueue.push_back(pChildUIObject);
				}

				switch (pUIObject->GetType())
				{
				case UIOBJECT_TYPE::PANEL:
					this->RenderPanel(static_cast<const Panel*>(pUIObject));
					break;
				case UIOBJECT_TYPE::BUTTON:
					this->RenderButton(static_cast<const Button*>(pUIObject));
					break;
				case UIOBJECT_TYPE::IMAGE_BUTTON:
					break;
				case UIOBJECT_TYPE::IMAGE:
					this->RenderImage(static_cast<const Image*>(pUIObject));
					break;
				case UIOBJECT_TYPE::INPUT_FIELD:
					break;
				case UIOBJECT_TYPE::TEXT:
					break;
				default:
					break;
				}
				++index;
			}

			// ´ÙÀ½ ÇÁ·¹ÀÓÀ» À§ÇØ Å¥ Å¬¸®¾î
			m_uiRenderQueue.clear();
		}
	}

	HRESULT hr = GraphicDevice::GetInstance()->GetSwapChainComInterface()->Present(0, 0);
}

void Renderer::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectP.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Â ¼øÈ¸ ½Ã º¯°æµÇ´Â »çÇ×ÀÌ ¾øÀ¸¹Ç·Î ·çÇÁ ¹Û¿¡¼­ Apply ÇÑ ¹ø¸¸ È£Ãâ
	m_effectImmediateContext.Apply(&m_basicEffectP);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const Subset& subset : pMesh->m_subsets)
	{
		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_COLOR);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPC.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Â ¼øÈ¸ ½Ã º¯°æµÇ´Â »çÇ×ÀÌ ¾øÀ¸¹Ç·Î ·çÇÁ ¹Û¿¡¼­ Apply ÇÑ ¹ø¸¸ È£Ãâ
	m_effectImmediateContext.Apply(&m_basicEffectPC);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const Subset& subset : pMesh->m_subsets)
	{
		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPN.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const Subset& subset : pMesh->m_subsets)
	{
		const Material* pMaterial = subset.m_material.get();
		if (pMaterial != nullptr)
		{
			m_basicEffectPN.UseMaterial(true);
			m_basicEffectPN.SetAmbientColor(XMLoadFloat4A(&pMaterial->m_ambient));
			m_basicEffectPN.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPN.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
		}
		else
		{
			m_basicEffectPN.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPN);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const Subset& subset : pMesh->m_subsets)
	{
		const Material* pMaterial = subset.m_material.get();
		if (pMaterial != nullptr)
		{
			m_basicEffectPT.UseMaterial(true);
			m_basicEffectPT.SetLightMap(pMaterial->m_lightMap);
			m_basicEffectPT.SetDiffuseMap(pMaterial->m_diffuseMap);
			m_basicEffectPT.SetNormalMap(pMaterial->m_normalMap);
			m_basicEffectPT.SetSpecularMap(pMaterial->m_specularMap);
		}
		else
		{
			m_basicEffectPT.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ¼­ºê¼Âµé ¼øÈ¸ÇÏ¸ç ·»´õ¸µ
	for (const Subset& subset : pMesh->m_subsets)
	{
		const Material* pMaterial = subset.m_material.get();
		if (pMaterial != nullptr)
		{
			m_basicEffectPNT.UseMaterial(true);
			m_basicEffectPNT.SetAmbientColor(XMLoadFloat4A(&pMaterial->m_ambient));
			m_basicEffectPNT.SetDiffuseColor(XMLoadFloat4A(&pMaterial->m_diffuse));
			m_basicEffectPNT.SetSpecularColor(XMLoadFloat4A(&pMaterial->m_specular));
			m_basicEffectPNT.SetLightMap(pMaterial->m_lightMap);
			m_basicEffectPNT.SetDiffuseMap(pMaterial->m_diffuseMap);
			m_basicEffectPNT.SetNormalMap(pMaterial->m_normalMap);
			m_basicEffectPNT.SetSpecularMap(pMaterial->m_specularMap);
		}
		else
		{
			m_basicEffectPNT.UseMaterial(false);
		}
		
		m_effectImmediateContext.Apply(&m_basicEffectPNT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkyboxCubeMap(pSkyboxCubeMapSRV);

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

void Renderer::RenderButton(const Button* pButton)
{
	// 1. ¹öÆ° ÇÁ·¹ÀÓ ·»´õ¸µ
	ID3D11Buffer* vbs[] = { m_pButtonVB };
	UINT strides[] = { sizeof(VFButton) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_buttonEffect.SetPressed(pButton->IsPressed());
	m_buttonEffect.SetColor(pButton->GetColorVector());
	m_buttonEffect.SetPreNDCPosition(pButton->m_transform.GetPreNDCPosition());
	m_buttonEffect.SetSize(pButton->GetSizeVector());

	m_effectImmediateContext.Apply(&m_buttonEffect);
	m_effectImmediateContext.Draw(30, 0);

	// 2. ¹öÆ° ÅØ½ºÆ® ·»´õ¸µ
	UINT32 textLength = static_cast<UINT32>(pButton->GetTextLength());
	if (textLength == 0)
		return;

	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
	ID2D1SolidColorBrush* pSolidColorBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();
	const RectTransform& transform = pButton->m_transform;
	D2D1_RECT_F layoutRect;
	XMFLOAT2A windowsButtonCenter;
	XMStoreFloat2A(&windowsButtonCenter, transform.GetWindowsScreenPosition());
	if (pButton->IsPressed())
	{
		windowsButtonCenter.x += 1.0f;
		windowsButtonCenter.y += 1.0f;
	}

	layoutRect.left = windowsButtonCenter.x - pButton->m_halfSize.x;
	layoutRect.right = layoutRect.left + pButton->m_size.x;
	layoutRect.top = windowsButtonCenter.y - pButton->m_halfSize.y;
	layoutRect.bottom = layoutRect.top + pButton->m_size.y;

	pSolidColorBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pButton->GetTextColor()));
	pD2DRenderTarget->BeginDraw();
	pD2DRenderTarget->DrawTextW(
		pButton->GetText(),
		textLength,
		GraphicDevice::GetInstance()->GetDefaultDWriteTextFormat(),
		layoutRect,
		pSolidColorBrush
	);

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

/*
void Renderer::RenderTerrain(ID3D11DeviceContext* pDeviceContext, const Terrain* pTerrain)
{
	assert(pDeviceContext != nullptr && pTerrain != nullptr);

	assert(m_pCbPerTerrain != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// ¹öÅØ½º ¹öÆÛ ¼³Á¤
	const UINT strides[] = { sizeof(VertexFormat::TerrainControlPoint) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vb[] = { pTerrain->GetPatchControlPointBufferInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, 1, vb, stride, offset);

	// ÀÎµ¦½º ¹öÆÛ ¼³Á¤
	this->GetDeviceContext()->IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerTerrain »ó¼ö¹öÆÛ ¾÷µ¥ÀÌÆ® ¹× ¹ÙÀÎµù
	ConstantBuffer::PerTerrain cbPerTerrain;

	// const XMMATRIX w = XMMatrixTranslationFromVector(camPos);
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_w, ConvertToHLSLMatrix(w));		// ¼ÎÀÌ´õ¿¡¼­ ¹Ì»ç¿ë
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// ¼ÎÀÌ´õ¿¡¼­ ¹Ì»ç¿ë
	XMStoreFloat4x4A(&cbPerTerrain.cbpt_wvp, ConvertToHLSLMatrix(XMLoadFloat4x4A(&m_vp)));	// ¿ùµåº¯È¯Àº »ý·« (ÁöÇüÀÌ ÀÌ¹Ì ¿ùµå °ø°£¿¡¼­ Á¤ÀÇµÇ¾ú´Ù°í °¡Á¤ÇÏ¹Ç·Î)
	cbPerTerrain.cbptTerrainTextureTiling = pTerrain->GetTextureScale();
	cbPerTerrain.cbptTerrainCellSpacing = pTerrain->GetCellSpacing();
	cbPerTerrain.cbptTerrainTexelSpacingU = pTerrain->GetTexelSpacingU();
	cbPerTerrain.cbptTerrainTexelSpacingV = pTerrain->GetTexelSpacingV();
	hr = this->GetDeviceContext()->Map(
		m_pCbPerTerrain,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerTerrain, sizeof(cbPerTerrain));
	this->GetDeviceContext()->Unmap(m_pCbPerTerrain, 0);

	ID3D11Buffer* const cbs[] = { m_pCbPerTerrain };
	this->GetDeviceContext()->DSSetConstantBuffers(ConstantBuffer::PerTerrain::GetSlotNumber(), _countof(cbs), cbs);
	this->GetDeviceContext()->PSSetConstantBuffers(ConstantBuffer::PerTerrain::GetSlotNumber(), _countof(cbs), cbs);

	// ¼ÎÀÌ´õ ¸®¼Ò½º ¼³Á¤
	ID3D11ShaderResourceView* const srvs[] = { pTerrain->GetHeightMapSRV() };
	this->GetDeviceContext()->VSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->DSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->PSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// µå·Î¿ì
	this->GetDeviceContext()->DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}
*/

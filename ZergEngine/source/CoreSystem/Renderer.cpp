#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Manager\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>

namespace ze
{
	RendererImpl Renderer;
}

using namespace ze;

RendererImpl::RendererImpl()
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
	// , mTerrainEffect()
	, m_skyboxEffect()
	, m_msCameraMergeEffect()
	, m_buttonEffect()
	, m_imageEffect()
	, m_uiRenderQueue()
{
	m_uiRenderQueue.reserve(256);
}

RendererImpl::~RendererImpl()
{
}

void RendererImpl::Init(void* pDesc)
{
	m_pCullBackRS = GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK);
	m_pCullNoneRS = GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::NONE);
	m_pWireFrameRS = GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::WIREFRAME, RASTERIZER_CULL_MODE::NONE);
	m_pDefaultDSS = GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEFAULT);
	m_pSkyboxDSS = GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATETYPE::SKYBOX);
	m_pDepthReadOnlyDSS = GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEPTH_READ_ONLY);
	m_pNoDepthStencilTestDSS = GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATETYPE::NO_DEPTH_STENCILTEST);
	m_pOpaqueBS = GraphicDevice.GetBSComInterface(BLEND_STATETYPE::OPAQUE_);
	m_pAlphaBlendBS = GraphicDevice.GetBSComInterface(BLEND_STATETYPE::ALPHABLEND);
	m_pNoColorWriteBS = GraphicDevice.GetBSComInterface(BLEND_STATETYPE::NO_COLOR_WRITE);
	m_pButtonVB = GraphicDevice.GetVBComInterface(VERTEX_BUFFER_TYPE::BUTTON);	// Read only vertex buffer

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 INITIALIZE EFFECTS 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->InitializeEffects();

	// effect context 遽綠
	assert(GraphicDevice.GetImmediateContextComInterface() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(GraphicDevice.GetImmediateContextComInterface());

	/*
	m_passTerrain.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_passTerrain.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::TERRAIN_PATCH_CTRL_PT));
	m_passTerrain.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TRANSFORM_TERRAIN_PATCH_CTRL_PT));
	m_passTerrain.SetHullShader(GraphicDevice.GetHSComInterface(HULL_SHADER_TYPE::CALC_TERRAIN_TESS_FACTOR));
	m_passTerrain.SetDomainShader(GraphicDevice.GetDSComInterface(DOMAIN_SHADER_TYPE::SAMPLE_TERRAIN_HEIGHT_MAP));
	m_passTerrain.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::COLOR_TERRAIN_FRAGMENT));
	m_passTerrain.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passTerrain.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATETYPE::DEFAULT), 0);
	*/
}

void RendererImpl::InitializeEffects()
{
	m_basicEffectP.Init();
	m_basicEffectPC.Init();
	m_basicEffectPN.Init();
	m_basicEffectPT.Init();
	m_basicEffectPNT.Init();
	m_skyboxEffect.Init();
	// mTerrainEffect.Init();
	m_msCameraMergeEffect.Init();
	m_buttonEffect.Init();
	m_imageEffect.Init();
}

void RendererImpl::ReleaseEffects()
{
	m_basicEffectP.Release();
	m_basicEffectPC.Release();
	m_basicEffectPN.Release();
	m_basicEffectPT.Release();
	m_basicEffectPNT.Release();
	m_skyboxEffect.Release();
	// mTerrainEffect.Release();
	m_msCameraMergeEffect.Release();
	m_buttonEffect.Release();
	m_imageEffect.Release();
}

void RendererImpl::Release()
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 RELEASE EFFECTS 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	this->ReleaseEffects();
}

void RendererImpl::RenderFrame()
{
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContextComInterface();

	// BasicEffect縑憮 餌辨ж朝 溶渦 鼻鷓 撲薑
	// 蹺�� 楛顫歜縑 滲唳 陛棟ж啪 熱薑
	// Sampler State
	constexpr TEXTURE_FILTERING_OPTION meshTexFilter = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;
	constexpr TEXTURE_FILTERING_OPTION terrainTexFilter = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;
	ID3D11SamplerState* const ssArr[] =
	{
		GraphicDevice.GetSSComInterface(meshTexFilter),		// (s0) mesh texture sampler
		GraphicDevice.GetSkyboxSamplerComInterface(),		// (s1)
		GraphicDevice.GetSSComInterface(terrainTexFilter),	// (s2) terrain color texture sampler
		GraphicDevice.GetHeightmapSamplerComInterface()		// (s3)
	};

	pImmContext->VSSetSamplers(0, _countof(ssArr), ssArr);	// 攪溯檣 檜めお縑憮
	pImmContext->DSSetSamplers(0, _countof(ssArr), ssArr);	// 攪溯檣 檜めお縑憮
	pImmContext->PSSetSamplers(0, _countof(ssArr), ssArr);	// 渠睡碟 檜めお縑憮

	// Rasterizer State
	pImmContext->RSSetState(m_pCullBackRS);					// �譫� 鏽葭 撲薑
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);

	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);

	// PerFrame 鼻熱幗ぷ 機等檜お 塽 夥檣註
	{
		DirectionalLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(DirectionalLightManager.m_activeComponents.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : DirectionalLightManager.m_activeComponents)
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
				XMVector3Rotate(LIGHT_DIRECTION_LOCAL_AXIS, rotation)
			);

			++index;
		}

		// m_basicEffectP.SetDirectionalLight(light, lightCount);
		// m_basicEffectPC.SetDirectionalLight(light, lightCount);
		m_basicEffectPN.SetDirectionalLight(light, lightCount);
		// m_basicEffectPT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNT.SetDirectionalLight(light, lightCount);
		// m_skyboxEffect.SetDirectionalLight(light, lightCount);
		// mTerrainEffect.SetDirectionalLight(light, lightCount);
	}

	{
		PointLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(PointLightManager.m_activeComponents.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : PointLightManager.m_activeComponents)
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
		// mTerrainEffect.SetPointLight(light, lightCount);
	}

	{
		SpotLightData light[MAX_GLOBAL_LIGHT_COUNT];
		const uint32_t lightCount = static_cast<uint32_t>(SpotLightManager.m_activeComponents.size());

		uint32_t index = 0;
		for (const IComponent* pLightComponent : SpotLightManager.m_activeComponents)
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
				XMVector3Rotate(LIGHT_DIRECTION_LOCAL_AXIS, rotation)
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
		// mTerrainEffect.SetSpotLight(light, lightCount);
	}

	// Camera葆棻 Щ溯歜 溶渦葭
	for (IComponent* pComponent : CameraManager.m_activeComponents)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		if (!pCamera->GetColorBufferRTVComInterface() ||
			!pCamera->GetColorBufferSRVComInterface() ||
			!pCamera->GetDepthStencilBufferDSVComInterface())
		{
			if (FAILED(pCamera->CreateBufferAndView()))
				continue;

			pCamera->UpdateProjectionMatrix();
			pCamera->UpdateFullbufferViewport();
		}
		pCamera->UpdateViewMatrix();	// 箔 滲�� ч溺 機等檜お

		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		// 箔んお 夥檣註
		pImmContext->RSSetViewports(1, &pCamera->GetFullBufferViewport());
		// 鏽楝 幗ぷ 塽 答蝶蝶蘸褒 幗ぷ 夥檣註
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		// PerCamera 鼻熱幗ぷ 機等檜お 塽 夥檣註
		m_basicEffectP.SetCamera(pCamera);
		m_basicEffectPC.SetCamera(pCamera);
		m_basicEffectPN.SetCamera(pCamera);
		m_basicEffectPT.SetCamera(pCamera);
		m_basicEffectPNT.SetCamera(pCamera);
		m_skyboxEffect.SetCamera(pCamera);
		// mTerrainEffect.SetCamera(pCamera);

		for (const IComponent* pComponent : MeshRendererManager.m_activeComponents)
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
				Debug::ForceCrashWithMessageBox(L"RendererImpl error", L"Unknown vertex format type.");
				break;
			}
		}

		// 雖⑽ 溶渦葭
		for (const IComponent* pComponent : TerrainManager.m_activeComponents)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			// RenderTerrain(pTerrain);
		}

		// 蝶蘋檜夢蝶 溶渦葭
		ID3D11ShaderResourceView* pSkyboxCubeMap = Environment.m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pSkyboxDSS, 0);
			RenderSkybox(pSkyboxCubeMap);
		}
	}

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 蘋詭塭 溶渦葭 唸婁 煽м 塽 UI 溶渦葭
	// Rasterizer State
	pImmContext->RSSetState(m_pCullNoneRS);	// Quad 溶渦葭 閨檜嘎煎 �譫� 鏽葭 莖晦
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);
	
	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);
	
	// 瞪羹 寥幗ぷ縑 渠и 箔んお 撲薑
	pImmContext->RSSetViewports(1, &GraphicDevice.GetFullSwapChainViewport());
	// 溶渦顫啃 夥檣註
	ID3D11RenderTargetView* pColorBufferRTV = GraphicDevice.GetSwapChainRTVComInterface();
	ID3D11DepthStencilView* pDepthStencilBufferDSV = GraphicDevice.GetSwapChainDSVComInterface();
	pImmContext->ClearRenderTargetView(pColorBufferRTV, DirectX::Colors::Red);
	pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
	
	for (const IComponent* pComponent : CameraManager.m_activeComponents)
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
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

	// UI 溶渦葭
	{
		// Rasterizer State
		pImmContext->RSSetState(m_pCullNoneRS);	// Quad 溶渦葭 閨檜嘎煎 �譫� 鏽葭 莖晦

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);

		// D3D11_BLEND_BLEND_FACTOR 傳朝 D3D11_BLEND_INV_BLEND_FACTOR 嘐餌辨 (blend factor煎 nullptr 瞪殖)
		pImmContext->OMSetBlendState(m_pAlphaBlendBS, nullptr, 0xFFFFFFFF);

		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		// 賅萇 UI Effect菟縑 渠и ScreenToNDCSpaceRatio 撲薑
		const XMFLOAT2 screenToNDCSpaceRatio = XMFLOAT2(
			2.0f / static_cast<float>(GraphicDevice.GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice.GetSwapChainDesc().BufferDesc.Height)
		);
		m_buttonEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_imageEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

		// 賅萇 瑞お UI螃粽薛お菟睡攪 牖�裔牉� 濠衝 UI梱雖 溶渦葭
		for (const IUIObject* pRootUIObject : UIObjectManager.m_rootUIObjects)
		{
			assert(pRootUIObject->IsRootObject());

			size_t index = 0;

			// 聽朝 喻萄 й渡檜 橾橫釭嘎煎 漱攪蒂 餌辨
			m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// 濠衝菟擊 賅舒 聽縑 厥朝棻.
				for (const RectTransform* pChildTransform : pUIObject->m_transform.m_children)
				{
					const IUIObject* pChildUIObject = pChildTransform->m_pUIObject;
					assert(pChildUIObject != nullptr);

					m_uiRenderQueue.push_back(pChildUIObject);
				}

				switch (pUIObject->GetType())
				{
				case UIOBJECT_TYPE::PANEL:
					if (this->RenderPanel(static_cast<const Panel*>(pUIObject)) == D2DERR_RECREATE_TARGET)
						GraphicDevice.RequestRecreateTarget();
					break;
				case UIOBJECT_TYPE::BUTTON:
					this->RenderButton(static_cast<const Button*>(pUIObject));
					break;
				case UIOBJECT_TYPE::IMAGE_BUTTON:
					break;
				case UIOBJECT_TYPE::IMAGE:
					this->RenderImage(static_cast<const Image*>(pUIObject));
					break;
				case UIOBJECT_TYPE::LABEL:
					break;
				case UIOBJECT_TYPE::TEXT_BOX:
					break;
				default:
					break;
				}
				++index;
			}

			// 棻擠 Щ溯歜擊 嬪п 聽 贗葬橫
			m_uiRenderQueue.clear();
		}
	}

	GraphicDevice.GetSwapChainComInterface()->Present(0, 0);
}

void RendererImpl::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectP.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 檣策蝶 幗ぷ 撲薑
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// 憮粽撢 牖�� 衛 滲唳腎朝 餌о檜 橈戲嘎煎 瑞Щ 夤縑憮 Apply и 廓虜 ��轎
	m_effectImmediateContext.Apply(&m_basicEffectP);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const Subset& subset : pMesh->m_subsets)
	{
		// 萄煎辦
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_COLOR);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPC.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 檣策蝶 幗ぷ 撲薑
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// 憮粽撢 牖�� 衛 滲唳腎朝 餌о檜 橈戲嘎煎 瑞Щ 夤縑憮 Apply и 廓虜 ��轎
	m_effectImmediateContext.Apply(&m_basicEffectPC);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const Subset& subset : pMesh->m_subsets)
	{
		// 萄煎辦
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPN.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 檣策蝶 幗ぷ 撲薑
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
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

		// 萄煎辦
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 檣策蝶 幗ぷ 撲薑
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
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

		// 萄煎辦
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 檣策蝶 幗ぷ 撲薑
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
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

		// 萄煎辦
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkyboxCubeMap(pSkyboxCubeMapSRV);

	m_effectImmediateContext.Apply(&m_skyboxEffect);

	// 萄煎辦
	// 樁檜渦 雖羲滲熱 薑薄菟 餌辨 (識 36偃)
	m_effectImmediateContext.Draw(36, 0);
}

HRESULT RendererImpl::RenderPanel(const Panel* pPanel)
{
	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice.GetD2DRenderTarget();
	ID2D1SolidColorBrush* pSolidColorBrush = GraphicDevice.GetD2DSolidColorBrush();
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

	return pD2DRenderTarget->EndDraw();
}

HRESULT RendererImpl::RenderButton(const Button* pButton)
{
	// 1. 幗が Щ溯歜 溶渦葭
	ID3D11Buffer* vbs[] = { m_pButtonVB };
	UINT strides[] = { sizeof(VFButton) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_buttonEffect.SetPressed(pButton->IsPressed());
	m_buttonEffect.SetColor(pButton->GetColorVector());
	m_buttonEffect.SetPreNDCPosition(pButton->m_transform.GetPreNDCPosition());
	m_buttonEffect.SetSize(pButton->GetSize());

	m_effectImmediateContext.Apply(&m_buttonEffect);
	m_effectImmediateContext.Draw(30, 0);

	// 2. 幗が 臢蝶お 溶渦葭
	UINT32 textLength = static_cast<UINT32>(wcslen(pButton->GetText()));
	if (textLength == 0)
		return S_OK;

	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice.GetD2DRenderTarget();
	ID2D1SolidColorBrush* pSolidColorBrush = GraphicDevice.GetD2DSolidColorBrush();
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
		GraphicDevice.GetDefaultDWriteTextFormat(),
		layoutRect,
		pSolidColorBrush
	);

	return pD2DRenderTarget->EndDraw();
}

void RendererImpl::RenderImage(const Image* pImage)
{
	m_imageEffect.SetPreNDCPosition(pImage->m_transform.GetPreNDCPosition());
	m_imageEffect.SetSize(pImage->GetSize());
	m_imageEffect.SetImageTexture(pImage->GetTexture());

	m_effectImmediateContext.Apply(&m_imageEffect);
	m_effectImmediateContext.Draw(4, 0);
}

/*
void RendererImpl::RenderTerrain(ID3D11DeviceContext* pDeviceContext, const Terrain* pTerrain)
{
	assert(pDeviceContext != nullptr && pTerrain != nullptr);

	assert(m_pCbPerTerrain != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT strides[] = { sizeof(VertexFormat::TerrainControlPoint) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vb[] = { pTerrain->GetPatchControlPointBufferInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, 1, vb, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	this->GetDeviceContext()->IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerTerrain 鼻熱幗ぷ 機等檜お 塽 夥檣註
	ConstantBuffer::PerTerrain cbPerTerrain;

	// const XMMATRIX w = XMMatrixTranslationFromVector(camPos);
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_w, ConvertToHLSLMatrix(w));		// 樁檜渦縑憮 嘐餌辨
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 樁檜渦縑憮 嘐餌辨
	XMStoreFloat4x4A(&cbPerTerrain.cbpt_wvp, ConvertToHLSLMatrix(XMLoadFloat4x4A(&m_vp)));	// 錯萄滲�素� 儅楞 (雖⑽檜 檜嘐 錯萄 奢除縑憮 薑曖腎歷棻堅 陛薑ж嘎煎)
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

	// 樁檜渦 葬模蝶 撲薑
	ID3D11ShaderResourceView* const srvs[] = { pTerrain->GetHeightMapSRV() };
	this->GetDeviceContext()->VSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->DSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->PSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// 萄煎辦
	this->GetDeviceContext()->DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}
*/

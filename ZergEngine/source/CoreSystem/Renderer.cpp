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
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
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

	// ���������������������������������������������������������������������� INITIALIZE EFFECTS ������������������������������������������������������������������
	this->InitializeEffects();

	// effect context �غ�
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
}

void RendererImpl::Release()
{
	// �������������������������������������������������������������������� RELEASE EFFECTS ������������������������������������������������������������������
	this->ReleaseEffects();
}

void RendererImpl::RenderFrame()
{
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContextComInterface();

	// BasicEffect���� ����ϴ� ���� ���� ����
	// ���� ��Ÿ�ӿ� ���� �����ϰ� ����
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

	pImmContext->VSSetSamplers(0, _countof(ssArr), ssArr);	// �ͷ��� ����Ʈ����
	pImmContext->DSSetSamplers(0, _countof(ssArr), ssArr);	// �ͷ��� ����Ʈ����
	pImmContext->PSSetSamplers(0, _countof(ssArr), ssArr);	// ��κ� ����Ʈ����

	// Rasterizer State
	pImmContext->RSSetState(m_pCullBackRS);					// �ĸ� �ø� ����
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);

	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);

	// PerFrame ������� ������Ʈ �� ���ε�
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

	// Camera���� ������ ������
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
		pCamera->UpdateViewMatrix();	// �� ��ȯ ��� ������Ʈ

		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// ����Ʈ ���ε�
		pImmContext->RSSetViewports(1, &pCamera->GetFullBufferViewport());
		// �÷� ���� �� �������ٽ� ���� ���ε�
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// PerCamera ������� ������Ʈ �� ���ε�
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

		// ���� ������
		for (const IComponent* pComponent : TerrainManager.m_activeComponents)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			// RenderTerrain(pTerrain);
		}

		// ��ī�̹ڽ� ������
		ID3D11ShaderResourceView* pSkyboxCubeMap = Environment.m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pSkyboxDSS, 0);
			RenderSkybox(pSkyboxCubeMap);
		}
	}

	// ��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// ī�޶� ������ ��� ���� �� UI ������
	// Rasterizer State
	pImmContext->RSSetState(m_pCullNoneRS);	// Quad ������ ���̹Ƿ� �ĸ� �ø� ����
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);
	
	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);
	
	// ��ü ����ۿ� ���� ����Ʈ ����
	pImmContext->RSSetViewports(1, &GraphicDevice.GetFullSwapChainViewport());
	// ����Ÿ�� ���ε�
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
	// ��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// UI ������
	{
		// Rasterizer State
		pImmContext->RSSetState(m_pCullNoneRS);	// Quad ������ ���̹Ƿ� �ĸ� �ø� ����

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pNoDepthStencilTestDSS, 0);

		// D3D11_BLEND_BLEND_FACTOR �Ǵ� D3D11_BLEND_INV_BLEND_FACTOR �̻�� (blend factor�� nullptr ����)
		pImmContext->OMSetBlendState(m_pAlphaBlendBS, nullptr, 0xFFFFFFFF);

		// ������������������������������������������������������������������������������������������������������������������������������������������������������������
		// ��� UI Effect�鿡 ���� ScreenToNDCSpaceRatio ����
		const XMFLOAT2 screenToNDCSpaceRatio = XMFLOAT2(
			2.0f / static_cast<float>(GraphicDevice.GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice.GetSwapChainDesc().BufferDesc.Height)
		);
		// m_panelEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_buttonEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// m_imageButtonEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// ������������������������������������������������������������������������������������������������������������������������������������������������������������

		for (const IUIObject* pRootUIObject : UIObjectManager.m_rootUIObjects)
		{
			assert(pRootUIObject->IsRootObject());

			size_t index = 0;

			// ť�� ��� �Ҵ��� �Ͼ�Ƿ� ���͸� ���
			m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// �ڽĵ��� ��� ť�� �ִ´�.
				for (const RectTransform* pChildTransform : pUIObject->m_transform.m_children)
				{
					const IUIObject* pChildUIObject = pChildTransform->m_pUIObject;
					assert(pChildUIObject != nullptr);

					m_uiRenderQueue.push_back(pChildUIObject);
				}

				switch (pUIObject->GetType())
				{
				case UIOBJECT_TYPE::PANEL:
					break;
				case UIOBJECT_TYPE::BUTTON:
					this->RenderButton(static_cast<const Button*>(pUIObject));
					break;
				case UIOBJECT_TYPE::IMAGE_BUTTON:
					break;
				case UIOBJECT_TYPE::IMAGE:
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����� ��ȸ �� ����Ǵ� ������ �����Ƿ� ���� �ۿ��� Apply �� ���� ȣ��
	m_effectImmediateContext.Apply(&m_basicEffectP);

	// ����µ� ��ȸ�ϸ� ������
	for (const Subset& subset : pMesh->m_subsets)
	{
		// ��ο�
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����� ��ȸ �� ����Ǵ� ������ �����Ƿ� ���� �ۿ��� Apply �� ���� ȣ��
	m_effectImmediateContext.Apply(&m_basicEffectPC);

	// ����µ� ��ȸ�ϸ� ������
	for (const Subset& subset : pMesh->m_subsets)
	{
		// ��ο�
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����µ� ��ȸ�ϸ� ������
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

		// ��ο�
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����µ� ��ȸ�ϸ� ������
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

		// ��ο�
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����µ� ��ȸ�ϸ� ������
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

		// ��ο�
		m_effectImmediateContext.DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkyboxCubeMap(pSkyboxCubeMapSRV);

	m_effectImmediateContext.Apply(&m_skyboxEffect);

	// ��ο�
	// ���̴� �������� ������ ��� (�� 36��)
	m_effectImmediateContext.Draw(36, 0);
}

void RendererImpl::RenderButton(const Button* pButton)
{
	ID3D11Buffer* vbs[] = { m_pButtonVB };
	UINT strides[] = { sizeof(VFButton) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_buttonEffect.SetPressed(pButton->IsPressed());
	m_buttonEffect.SetColor(pButton->GetColor());
	m_buttonEffect.SetScreenPosition(pButton->m_transform.GetScreenPosition());
	m_buttonEffect.SetSize(pButton->GetSize());

	m_effectImmediateContext.Apply(&m_buttonEffect);
	m_effectImmediateContext.Draw(30, 0);
}

/*
void RendererImpl::RenderTerrain(ID3D11DeviceContext* pDeviceContext, const Terrain* pTerrain)
{
	assert(pDeviceContext != nullptr && pTerrain != nullptr);

	assert(m_pCbPerTerrain != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// ���ؽ� ���� ����
	const UINT strides[] = { sizeof(VertexFormat::TerrainControlPoint) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vb[] = { pTerrain->GetPatchControlPointBufferInterface() };
	this->GetDeviceContext()->IASetVertexBuffers(0, 1, vb, stride, offset);

	// �ε��� ���� ����
	this->GetDeviceContext()->IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerTerrain ������� ������Ʈ �� ���ε�
	ConstantBuffer::PerTerrain cbPerTerrain;

	// const XMMATRIX w = XMMatrixTranslationFromVector(camPos);
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_w, ConvertToHLSLMatrix(w));		// ���̴����� �̻��
	// XMStoreFloat4x4A(&cbPerTerrain.cbpt_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// ���̴����� �̻��
	XMStoreFloat4x4A(&cbPerTerrain.cbpt_wvp, ConvertToHLSLMatrix(XMLoadFloat4x4A(&m_vp)));	// ���庯ȯ�� ���� (������ �̹� ���� �������� ���ǵǾ��ٰ� �����ϹǷ�)
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

	// ���̴� ���ҽ� ����
	ID3D11ShaderResourceView* const srvs[] = { pTerrain->GetHeightMapSRV() };
	this->GetDeviceContext()->VSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->DSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);
	this->GetDeviceContext()->PSSetShaderResources(ShaderResourceSlot::HeightmapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// ��ο�
	this->GetDeviceContext()->DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}
*/

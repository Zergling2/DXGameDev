#include <ZergEngine\CoreSystem\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\RenderSettings.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\PointLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\SpotLightManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MeshRendererManager.h>
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
	, m_basicEffectPNTT()
	, m_terrainEffect()
	, m_skyboxEffect()
	, m_drawQuadWithMSTextureEffect()
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
	m_pCullBackRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerFillMode::Solid, RasterizerCullMode::Back);
	m_pCullNoneRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerFillMode::Solid, RasterizerCullMode::None);
	m_pWireFrameRS = GraphicDevice::GetInstance()->GetRSComInterface(RasterizerFillMode::Wireframe, RasterizerCullMode::None);
	m_pDefaultDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::Default);
	m_pSkyboxDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::Skybox);
	m_pDepthReadOnlyDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::DepthReadOnly);
	m_pNoDepthStencilTestDSS = GraphicDevice::GetInstance()->GetDSSComInterface(DepthStencilStateType::NoDepthStencilTest);
	m_pOpaqueBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::Opaque);
	m_pAlphaBlendBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::AlphaBlend);
	m_pNoColorWriteBS = GraphicDevice::GetInstance()->GetBSComInterface(BlendStateType::NoColorWrite);
	m_pButtonVB = GraphicDevice::GetInstance()->GetVBComInterface(VertexBufferType::ButtonPt);	// Read only vertex buffer

	// ���������������������������������������������������������������������� INITIALIZE EFFECTS ������������������������������������������������������������������
	m_basicEffectP.Init();
	m_basicEffectPC.Init();
	m_basicEffectPN.Init();
	m_basicEffectPT.Init();
	m_basicEffectPNT.Init();
	m_basicEffectPNTT.Init();
	m_terrainEffect.Init();
	m_skyboxEffect.Init();
	m_drawQuadWithMSTextureEffect.Init();
	m_buttonEffect.Init();
	m_imageEffect.Init();

	// effect context �غ�
	assert(GraphicDevice::GetInstance()->GetImmediateContextComInterface() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(GraphicDevice::GetInstance()->GetImmediateContextComInterface());
}

void Renderer::UnInit()
{
	// �������������������������������������������������������������������� RELEASE EFFECTS ������������������������������������������������������������������
	m_basicEffectP.Release();
	m_basicEffectPC.Release();
	m_basicEffectPN.Release();
	m_basicEffectPT.Release();
	m_basicEffectPNT.Release();
	m_basicEffectPNTT.Release();
	m_terrainEffect.Release();
	m_skyboxEffect.Release();
	m_drawQuadWithMSTextureEffect.Release();
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
	pImmContext->RSSetState(m_pCullBackRS);		// �ĸ� �ø� ����

	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);

	// BasicState
	pImmContext->OMSetBlendState(m_pOpaqueBS, nullptr, 0xFFFFFFFF);

	// PerFrame ������� ������Ʈ �� ���ε�
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

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;
			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Rotate(Math::Vector3::Forward(), rotation)
			);

			++index;
		}

		// m_basicEffectP.SetDirectionalLight(light, lightCount);
		// m_basicEffectPC.SetDirectionalLight(light, lightCount);
		m_basicEffectPN.SetDirectionalLight(light, lightCount);
		// m_basicEffectPT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNT.SetDirectionalLight(light, lightCount);
		m_basicEffectPNTT.SetDirectionalLight(light, lightCount);
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

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, pGameObject->m_transform.GetWorldPosition());
			light[index].range = pLight->m_range;

			light[index].att = pLight->m_att;

			++index;
		}

		// m_basicEffectP.SetPointLight(light, lightCount);
		// m_basicEffectPC.SetPointLight(light, lightCount);
		m_basicEffectPN.SetPointLight(light, lightCount);
		// m_basicEffectPT.SetPointLight(light, lightCount);
		m_basicEffectPNT.SetPointLight(light, lightCount);
		m_basicEffectPNTT.SetPointLight(light, lightCount);
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

			light[index].ambient = pLight->m_ambient;
			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, translation);
			light[index].range = pLight->m_range;

			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Rotate(Math::Vector3::Forward(), rotation)
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
		m_basicEffectPNTT.SetSpotLight(light, lightCount);
		// m_skyboxEffect.SetSpotLight(light, lightCount);
		m_terrainEffect.SetSpotLight(light, lightCount);
	}

	// Camera���� ������ ������
	for (IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		pCamera->UpdateViewMatrix();	// �� ��ȯ ��� ������Ʈ
		Frustum cameraFrustumW;			// �������� �ø���
		Math::CalcWorldFrustumFromViewProjMatrix(pCamera->GetViewMatrix()* pCamera->GetProjMatrix(), cameraFrustumW);

		// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// ����Ʈ ���ε�
		pImmContext->RSSetViewports(1, &pCamera->GetEntireBufferViewport());
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
		m_basicEffectPNTT.SetCamera(pCamera);
		m_skyboxEffect.SetCamera(pCamera);
		m_terrainEffect.SetCamera(pCamera);

		for (const IComponent* pComponent : MeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			const Mesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;

			// �������� �ø�
			Aabb aabbW;
			pMesh->GetAabb().Transform(aabbW, pMeshRenderer->m_pGameObject->m_transform.GetWorldTransformMatrix());
			if (!Math::TestFrustumAabbCollision(cameraFrustumW, aabbW))
				continue;

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
		}

		// ���� ������
		for (const IComponent* pComponent : TerrainManager::GetInstance()->m_directAccessGroup)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			RenderTerrain(pTerrain);
		}

		// ��ī�̹ڽ� ������
		ID3D11ShaderResourceView* pSkyboxCubeMap = RenderSettings::GetInstance()->m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pSkyboxDSS, 0);
			RenderSkybox(pSkyboxCubeMap);
			pImmContext->OMSetDepthStencilState(m_pDefaultDSS, 0);
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
	pImmContext->RSSetViewports(1, &GraphicDevice::GetInstance()->GetEntireSwapChainViewport());
	// ����Ÿ�� ���ε�
	ID3D11RenderTargetView* pColorBufferRTV = GraphicDevice::GetInstance()->GetSwapChainRTVComInterface();
	ID3D11DepthStencilView* pDepthStencilBufferDSV = GraphicDevice::GetInstance()->GetSwapChainDSVComInterface();
	pImmContext->ClearRenderTargetView(pColorBufferRTV, DirectX::Colors::Blue);
	pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
	
	for (const IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		const Camera* pCamera = static_cast<const Camera*>(pComponent);
		if (!pCamera->IsEnabled())
			continue;
	
		m_drawQuadWithMSTextureEffect.SetQuadParameters(
			pCamera->m_viewportRect.m_width,
			pCamera->m_viewportRect.m_height,
			pCamera->m_viewportRect.m_x,
			pCamera->m_viewportRect.m_y
		);
		m_drawQuadWithMSTextureEffect.SetTexture(pCamera->GetColorBufferSRVComInterface());
	
		m_effectImmediateContext.Apply(&m_drawQuadWithMSTextureEffect);
	
		m_effectImmediateContext.Draw(4, 0);
	}

	{
		// ���� ī�޶� ����ϴ� ������ ���� �����ӿ� ���� ī�޶� ���۰� Input/Output���� ���ÿ� ���ε� �Ǵ°��� ����
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_effectImmediateContext.GetDeviceContextComInterface()->PSSetShaderResources(0, 1, srvs);
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
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Height)
		);
		m_buttonEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_imageEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// ������������������������������������������������������������������������������������������������������������������������������������������������������������

		// ��� ��Ʈ UI������Ʈ����� ��ȸ�ϸ� �ڽ� UI���� ������
		for (const IUIObject* pRootUIObject : UIObjectManager::GetInstance()->m_roots)
		{
			assert(pRootUIObject->IsRoot());

			size_t index = 0;

			// ť�� ��� �Ҵ��� �Ͼ�Ƿ� �迭�� ���.

			if (pRootUIObject->IsActive())
				m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// �ڽĵ��� ��� ť�� �ִ´�.
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

			// �������� �Ϸ�� �� ť Ŭ����
			m_uiRenderQueue.clear();
		}
	}

	HRESULT hr = GraphicDevice::GetInstance()->GetSwapChainComInterface()->Present(1, 0);
}

void Renderer::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::Position);

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

void Renderer::RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionColor);

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

void Renderer::RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormal);

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
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const Subset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
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
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionTexCoord);

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
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const Subset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
			m_basicEffectPT.SetTexture(pMaterial->m_diffuseMap.GetSRVComInterface());
		else
			m_basicEffectPT.SetTexture(nullptr);

		m_effectImmediateContext.Apply(&m_basicEffectPT);

		// ��ο�
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTexCoord);

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
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const Subset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPNT.UseMaterial(true);
			m_basicEffectPNT.SetAmbientColor(XMLoadFloat4A(&pMaterial->m_ambient));
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

		// ��ο�
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderVFPositionNormalTangentTexCoordMesh(const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTangentTexCoord);

	const GameObject* pGameObject = pMeshRenderer->m_pGameObject;
	assert(pGameObject != nullptr);

	m_basicEffectPNTT.SetWorldMatrix(pGameObject->m_transform.GetWorldTransformMatrix());

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(pMesh->GetVertexFormatType()) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// ����µ� ��ȸ�ϸ� ������
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const Subset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
		{
			m_basicEffectPNTT.UseMaterial(true);
			m_basicEffectPNTT.SetAmbientColor(XMLoadFloat4A(&pMaterial->m_ambient));
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

		// ��ο�
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

	// ���ؽ� ���� ����
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::TerrainPatchCtrlPt) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pTerrain->GetPatchControlPointBufferComInterface() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// �ε��� ���� ����
	m_effectImmediateContext.IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBufferComInterface(), DXGI_FORMAT_R32_UINT, 0);

	m_effectImmediateContext.Apply(&m_terrainEffect);
	m_effectImmediateContext.DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}

void Renderer::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkybox(pSkyboxCubeMapSRV);

	m_effectImmediateContext.Apply(&m_skyboxEffect);

	// ��ο�
	// ���̴� �������� ������ ��� (�� 36��)
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
		if (static_cast<const Button*>(pText)->IsPressed())	// ��ư�� ���� �����̸� �ؽ�Ʈ ��ġ�� ��¦ ���ϴ����� ������ ��ü�� �ο�
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
	// 1. ��ư ������ ������
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

	// 2. ��ư �ؽ�Ʈ ������
	this->RenderText(static_cast<const Text*>(pButton));
}

void Renderer::RenderInputField(const InputField* pInputField)
{
	ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
	ID2D1SolidColorBrush* pBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();

	// 1. Input Field ��� ������
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

		// 2. Text ������
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
			if (textLength <= pwcl)	// �ִ� 32���� *������ �����Ҵ� X
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

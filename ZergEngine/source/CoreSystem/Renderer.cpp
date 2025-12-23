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
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SliderControl.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Checkbox.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Billboard.h>
#include <ZergEngine\CoreSystem\Resource\StaticMesh.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <algorithm>

using namespace ze;

Renderer* Renderer::s_pInstance = nullptr;

// Ä«¸Þ¶ó·ÎºÎÅÍÀÇ °Å¸® Á¦°ö ³»¸²Â÷¼ø Á¤·Ä
struct BillboardComparator
{
	bool operator()(const std::pair<const Billboard*, float>& a, const std::pair<const Billboard*, float>& b) const
	{
		return a.second > b.second; // ³»¸²Â÷¼ø
	}
};

Renderer::Renderer()
	: m_pRSSolidCullBack(nullptr)
	, m_pRSMultisampleSolidCullBack(nullptr)
	, m_pRSSolidCullNone(nullptr)
	, m_pRSMultisampleSolidCullNone(nullptr)
	, m_pRSWireframe(nullptr)
	, m_pRSMultisampleWireframe(nullptr)
	, m_pDSSDefault(nullptr)
	, m_pDSSSkybox(nullptr)
	, m_pDSSDepthReadOnlyLess(nullptr)
	, m_pDSSNoDepthStencilTest(nullptr)
	, m_pBSOpaque(nullptr)
	, m_pBSAlphaBlend(nullptr)
	, m_pBSNoColorWrite(nullptr)
	, m_pVBShaded2DQuad(nullptr)
	, m_pVBCheckbox(nullptr)
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
	, m_billboardEffect()
	, m_drawScreenQuadTex()
	, m_drawScreenQuadMSTex()
	, m_shaded2DQuadEffect()
	, m_checkboxEffect()
	, m_imageEffect()
	, m_asteriskStr(L"********************************")	// L'*' x 32
	, m_billboardRenderQueue()
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
	const GraphicDevice* pGraphicDevice = GraphicDevice::GetInstance();

	m_pRSSolidCullBack = pGraphicDevice->GetRSComInterface(RasterizerMode::SolidCullBack);
	m_pRSMultisampleSolidCullBack = pGraphicDevice->GetRSComInterface(RasterizerMode::MultisampleSolidCullBack);
	m_pRSSolidCullNone = pGraphicDevice->GetRSComInterface(RasterizerMode::SolidCullNone);
	m_pRSMultisampleSolidCullNone = pGraphicDevice->GetRSComInterface(RasterizerMode::MultisampleSolidCullNone);
	m_pRSWireframe = pGraphicDevice->GetRSComInterface(RasterizerMode::Wireframe);
	m_pRSMultisampleWireframe = pGraphicDevice->GetRSComInterface(RasterizerMode::MultisampleWireframe);
	m_pDSSDefault = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::Default);
	m_pDSSSkybox = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::Skybox);
	m_pDSSDepthReadOnlyLess = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::DepthReadOnlyLess);
	m_pDSSNoDepthStencilTest = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::NoDepthStencilTest);
	m_pBSOpaque = pGraphicDevice->GetBSComInterface(BlendStateType::Opaque);
	m_pBSAlphaBlend = pGraphicDevice->GetBSComInterface(BlendStateType::AlphaBlend);
	m_pBSNoColorWrite = pGraphicDevice->GetBSComInterface(BlendStateType::NoColorWrite);
	m_pVBShaded2DQuad = pGraphicDevice->GetVBShaded2DQuad();	// Read only vertex buffer
	m_pVBCheckbox = pGraphicDevice->GetVBCheckbox();

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
	m_billboardEffect.Init();
	m_drawScreenQuadTex.Init();
	m_drawScreenQuadMSTex.Init();
	m_shaded2DQuadEffect.Init();
	m_checkboxEffect.Init();
	m_imageEffect.Init();

	// effect context ÁØºñ
	assert(pGraphicDevice->GetImmediateContextComInterface() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(pGraphicDevice->GetImmediateContextComInterface());
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
	m_billboardEffect.Release();
	m_drawScreenQuadTex.Release();
	m_drawScreenQuadMSTex.Release();
	m_shaded2DQuadEffect.Release();
	m_checkboxEffect.Release();
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

	// PerFrame »ó¼ö¹öÆÛ ¾÷µ¥ÀÌÆ® ¹× ¹ÙÀÎµù
	{
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
		m_billboardEffect.SetAmbientLight(ambientLight);
	}

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

			XMVECTOR scale;
			XMVECTOR rotationQuat;
			XMVECTOR position;
			pGameObject->m_transform.GetWorldTransform(&scale, &rotationQuat, &position);

			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;
			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Normalize(XMVector3Rotate(Vector3::Forward(), rotationQuat))
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
		m_billboardEffect.SetDirectionalLight(light, lightCount);
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
		m_billboardEffect.SetPointLight(light, lightCount);
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

			XMVECTOR scale;
			XMVECTOR rotationQuat;
			XMVECTOR position;
			pGameObject->m_transform.GetWorldTransform(&scale, &rotationQuat, &position);

			light[index].diffuse = pLight->m_diffuse;
			light[index].specular = pLight->m_specular;

			XMStoreFloat3(&light[index].positionW, position);
			light[index].range = pLight->GetRange();

			XMStoreFloat3(
				&light[index].directionW,
				XMVector3Normalize(XMVector3Rotate(Vector3::Forward(), rotationQuat))
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
		m_billboardEffect.SetSpotLight(light, lightCount);
	}





	// Camera¸¶´Ù ÇÁ·¹ÀÓ ·»´õ¸µ

	// DepthStencil State
	for (IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		// ºÒÅõ¸í ÆÐ½º
		// Rasterizer State
		if (pCamera->GetMSAAMode() == MSAAMode::Off)
			pImmContext->RSSetState(m_pRSSolidCullBack);
		else
			pImmContext->RSSetState(m_pRSMultisampleSolidCullBack);	// ÈÄ¸é ÄÃ¸µ ¼³Á¤

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSDefault, 0);

		// BasicState
		pImmContext->OMSetBlendState(m_pBSOpaque, nullptr, 0xFFFFFFFF);

		pCamera->UpdateViewMatrix();	// ºä º¯È¯ Çà·Ä ¾÷µ¥ÀÌÆ®

		// ¿ùµå ½ºÆäÀÌ½º ÀýµÎÃ¼ »ý¼º
		Frustum frustumW(pCamera->GetProjMatrix(), false);	// ¿ø±Ù Åõ¿µ ±â¹Ý ÀýµÎÃ¼
		frustumW.Transform(frustumW, XMMatrixInverse(nullptr, pCamera->GetViewMatrix()));

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
		m_billboardEffect.SetCamera(pCamera);

		// <Mesh ·»´õ¸µ>
		for (const IComponent* pComponent : MeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;
			
			// ÇÁ·¯½ºÅÒ ÄÃ¸µ
			const Aabb& aabbL = pMesh->GetAabb();
			Obb obbW;
			obbW.Center = aabbL.Center;
			obbW.Extents = aabbL.Extents;
			XMStoreFloat4(&obbW.Orientation, Quaternion::Identity());

			XMMATRIX worldMatrix = pMeshRenderer->m_pGameObject->m_transform.GetWorldTransformMatrix();

			obbW.Transform(obbW, worldMatrix);
			if (!Math::TestFrustumObbCollision(frustumW, obbW))
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
				RenderPNTTMesh(pMeshRenderer);
				break;
			case VertexFormatType::COUNT:
				__fallthrough;
			case VertexFormatType::UNKNOWN:
				*reinterpret_cast<int*>(0) = 0;		// Force crash
				break;
			}
			*/

			RenderPNTTMesh(pMeshRenderer, worldMatrix);
		}

		// <SkinnedMesh ·»´õ¸µ>
		for (const IComponent* pComponent : SkinnedMeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const SkinnedMeshRenderer* pMeshRenderer = static_cast<const SkinnedMeshRenderer*>(pComponent);
			const SkinnedMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;

			// ÇÁ·¯½ºÅÒ ÄÃ¸µ
			// const Aabb& aabbL = pMesh->GetAabb();
			// Obb obbW;
			// obbW.Center = aabbL.Center;
			// obbW.Extents = aabbL.Extents;
			// XMStoreFloat4(&obbW.Orientation, Quaternion::Identity());

			XMMATRIX worldMatrix = pMeshRenderer->m_pGameObject->m_transform.GetWorldTransformMatrix();

			// obbW.Transform(obbW, worldMatrix);
			// if (!Math::TestFrustumObbCollision(frustumW, obbW))
			// 	continue;

			RenderPNTTSkinnedMesh(pMeshRenderer, worldMatrix);
		}

		// ÁöÇü ·»´õ¸µ
		for (const IComponent* pComponent : TerrainManager::GetInstance()->m_directAccessGroup)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			RenderTerrain(pTerrain);
		}

		// <½ºÄ«ÀÌ¹Ú½º ·»´õ¸µ>
		ID3D11ShaderResourceView* pSkyboxCubeMap = RenderSettings::GetInstance()->m_skyboxCubeMap.GetSRVComInterface();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pDSSSkybox, 0);
			RenderSkybox(pSkyboxCubeMap);
			pImmContext->OMSetDepthStencilState(m_pDSSDefault, 0);
		}


		// Åõ¸í ÆÐ½º
		// 
		// <ºôº¸µå ·»´õ¸µ>
		// ÆÄÀÌÇÁ¶óÀÎ »óÅÂ ¼³Á¤
		// Rasterizer State
		// ºôº¸µå´Â Ç×»ó Ä«¸Þ¶ó¸¦ ÇâÇÏ¹Ç·Î ÄÃ¸µÀÌ ÇÊ¿ä¾øÀ½.
		if (pCamera->GetMSAAMode() == MSAAMode::Off)
			pImmContext->RSSetState(m_pRSSolidCullNone);
		else
			pImmContext->RSSetState(m_pRSMultisampleSolidCullNone);

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSDepthReadOnlyLess, 0);

		// Blend State
		// D3D11_BLEND_BLEND_FACTOR ¶Ç´Â D3D11_BLEND_INV_BLEND_FACTOR ¹Ì»ç¿ë (blend factor·Î nullptr Àü´Þ)
		pImmContext->OMSetBlendState(m_pBSAlphaBlend, nullptr, 0xFFFFFFFF);



		XMVECTOR cameraUpW;
		XMVECTOR cameraForwardW;
		XMVECTOR cameraPosW;
		{
			XMVECTOR cameraScaleW;
			XMVECTOR cameraRotW;		// Quaternion
			pCamera->m_pGameObject->m_transform.GetWorldTransform(&cameraScaleW, &cameraRotW, &cameraPosW);
			cameraUpW = XMVector3Rotate(Vector3::Up(), cameraRotW);
			cameraForwardW = XMVector3Rotate(Vector3::Forward(), cameraRotW);
		}

		XMMATRIX screenAlignedBillboardRotationW;
		{
			// ºôº¸µåÀÇ ±âÀú º¤ÅÍ °è»ê
			XMVECTOR up = cameraUpW;	// up º¤ÅÍ´Â Ä«¸Þ¶óÀÇ up º¤ÅÍ¿Í µ¿ÀÏ
			XMVECTOR forward = XMVectorNegate(cameraForwardW);	// ºôº¸µåÀÇ forward´Â cameraÀÇ forward¿Í ¹Ý´ë¹æÇâ
			XMVECTOR right = XMVector3Cross(up, forward);	// right º¤ÅÍ´Â up, forward·Î µµÃâ

			screenAlignedBillboardRotationW.r[0] = right;
			screenAlignedBillboardRotationW.r[1] = up;
			screenAlignedBillboardRotationW.r[2] = forward;
			screenAlignedBillboardRotationW.r[3] = Math::IdentityR3();
		}

		// È°¼ºÈ­µÈ ºôº¸µå ÄÄÆ÷³ÍÆ® °Ë»ö ¹× Á¤·Ä (Å¥¿¡ Æ÷ÀÎÅÍ ÀÌµ¿ ÈÄ Á¤·Ä, Manager ¹è¿­¿¡¼­ Á¤·ÄÇÏ¸é groupIndex ¾÷µ¥ÀÌÆ® µî º¹ÀâÇØÁü)
		m_billboardRenderQueue.clear();
		for (IComponent* pComponent : BillboardManager::GetInstance()->m_directAccessGroup)
		{
			Billboard* pBillboard = static_cast<Billboard*>(pComponent);
			if (!pBillboard->IsEnabled() || pBillboard == nullptr)
				continue;

			// ÇÁ·¯½ºÅÒ ÄÃ¸µ
			XMVECTOR billboardScaleW = XMVectorMultiply(
				XMVectorSet(pBillboard->GetBillboardQuadWidth(), pBillboard->GetBillboardQuadHeight(), 1.0f, 0.0f),
				pBillboard->m_pGameObject->m_transform.GetWorldScale()
			);

			Obb obbW;
			XMStoreFloat3(&obbW.Center, XMVectorZero());
			XMStoreFloat3(&obbW.Extents, billboardScaleW);
			obbW.Extents.z = BOUNDING_BOX_MIN_EXTENT;	// z º¼·ý 0.01f
			XMStoreFloat4(&obbW.Orientation, Quaternion::Identity());

			XMVECTOR billboardPosW = pBillboard->m_pGameObject->m_transform.GetWorldPosition();
			XMMATRIX worldMatrix;
			switch (pBillboard->GetBillboardType())
			{
			case BillboardType::Spherical:
				worldMatrix = Math::ComputeBillboardSphericalMatrix(billboardPosW, billboardScaleW, cameraPosW, cameraUpW);
				break;
			case BillboardType::CylindricalY:
				worldMatrix = Math::ComputeBillboardCylindricalYMatrix(billboardPosW, billboardScaleW, cameraPosW);
				break;
			case BillboardType::ScreenAligned:
				worldMatrix = Math::ComputeBillboardScreenAlignedMatrix(screenAlignedBillboardRotationW, billboardPosW, billboardScaleW);
				break;
			default:
				*reinterpret_cast<int*>(0) = 0;	// Force crash
				break;
			}
			obbW.Transform(obbW, worldMatrix);

			if (!Math::TestFrustumObbCollision(frustumW, obbW))
				continue;

			// ÇÁ·¯½ºÅÒ ÄÃ¸µ¿¡ »ç¿ëÇÑ Çà·ÄÀÌ ·»´õ¸µ ½Ã¿¡µµ ÇÊ¿äÇÏ¹Ç·Î Ä³½Ì
			pBillboard->CacheWorldMatrix(worldMatrix);

			// Ä«¸Þ¶ó·ÎºÎÅÍÀÇ °Å¸®ÀÇ Á¦°ö °è»ê
			XMVECTOR lengthSquared = XMVector3LengthSq(XMVectorSubtract(billboardPosW, cameraPosW));
			m_billboardRenderQueue.push_back(std::make_pair(pBillboard, XMVectorGetX(lengthSquared)));	// ºôº¸µå Æ÷ÀÎÅÍ, °Å¸® Á¦°ö pair
		}

		// ºôº¸µå Á¤·Ä ¹× ·»´õ¸µ
		std::sort(m_billboardRenderQueue.begin(), m_billboardRenderQueue.end(), BillboardComparator());
		for (const auto& pair : m_billboardRenderQueue)
			RenderBillboard(pair.first);
	}

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Ä«¸Þ¶ó ·»´õ¸µ °á°ú º´ÇÕ ¹× ½º¿ÒÃ¼ÀÎ ¹öÆÛ¿¡ UI ·»´õ¸µ
	// Rasterizer State
	pImmContext->RSSetState(m_pRSSolidCullNone);	// Quad ·»´õ¸µ »ÓÀÌ¹Ç·Î ÈÄ¸é ÄÃ¸µ ²ô±â
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDSSNoDepthStencilTest, 0);
	
	// Blend State
	pImmContext->OMSetBlendState(m_pBSOpaque, nullptr, 0xFFFFFFFF);
	
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
			m_drawScreenQuadMSTex.SetScreenRatioQuadParam(
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
			m_drawScreenQuadTex.SetScreenRatioQuadParam(
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
		pImmContext->RSSetState(m_pRSSolidCullNone);	// Quad ·»´õ¸µ »ÓÀÌ¹Ç·Î ÈÄ¸é ÄÃ¸µ ²ô±â

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSNoDepthStencilTest, 0);

		// Blend State
		// D3D11_BLEND_BLEND_FACTOR ¶Ç´Â D3D11_BLEND_INV_BLEND_FACTOR ¹Ì»ç¿ë (blend factor·Î nullptr Àü´Þ)
		pImmContext->OMSetBlendState(m_pBSAlphaBlend, nullptr, 0xFFFFFFFF);

		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
		// ¸ðµç UI Effectµé¿¡ ´ëÇÑ ScreenToNDCSpaceRatio ¼³Á¤
		const XMFLOAT2 screenToNDCSpaceRatio = XMFLOAT2(
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Height)
		);
		m_shaded2DQuadEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_checkboxEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_imageEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

		ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetD2DRenderTarget();
		ID2D1SolidColorBrush* pBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();
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
				case UIObjectType::Panel:
					this->RenderPanel(pD2DRenderTarget, pBrush, static_cast<const Panel*>(pUIObject));
					break;
				case UIObjectType::Image:
					this->RenderImage(static_cast<const Image*>(pUIObject));
					break;
				case UIObjectType::Text:
					this->RenderText(pD2DRenderTarget, pBrush, static_cast<const Text*>(pUIObject));
					break;
				case UIObjectType::InputField:
					this->RenderInputField(pD2DRenderTarget, pBrush, static_cast<const InputField*>(pUIObject));
					break;
				case UIObjectType::Button:
					this->RenderButton(pD2DRenderTarget, pBrush, static_cast<const Button*>(pUIObject));
					break;
				case UIObjectType::ImageButton:
					break;
				case UIObjectType::SliderControl:
					this->RenderSliderControl(static_cast<const SliderControl*>(pUIObject));
					break;
				case UIObjectType::Checkbox:
					this->RenderCheckbox(pD2DRenderTarget, pBrush, static_cast<const Checkbox*>(pUIObject));
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
void XM_CALLCONV Renderer::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::Position);

	m_basicEffectP.SetWorldMatrix(worldMatrix);

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
void XM_CALLCONV Renderer::RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionColor);

	m_basicEffectPC.SetWorldMatrix(worldMatrix);

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
void XM_CALLCONV Renderer::RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormal);

	m_basicEffectPN.SetWorldMatrix(worldMatrix);

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
void XM_CALLCONV Renderer::RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionTexCoord);

	m_basicEffectPT.SetWorldMatrix(worldMatrix);

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
void XM_CALLCONV Renderer::RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTexCoord);

	m_basicEffectPNT.SetWorldMatrix(worldMatrix);

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

void XM_CALLCONV Renderer::RenderPNTTMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::PositionNormalTangentTexCoord);

	m_basicEffectPNTT.SetWorldMatrix(worldMatrix);

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
	for (size_t subsetIndex = 0; subsetIndex < subsetCount; ++subsetIndex)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[subsetIndex];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(subsetIndex);
		m_basicEffectPNTT.SetMaterial(pMaterial);

		m_effectImmediateContext.Apply(&m_basicEffectPNTT);

		// µå·Î¿ì
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void XM_CALLCONV Renderer::RenderPNTTSkinnedMesh(const SkinnedMeshRenderer* pSkinnedMeshRenderer, FXMMATRIX worldMatrix)
{
	const SkinnedMesh* pMesh = pSkinnedMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	m_basicEffectPNTTSkinned.SetWorldMatrix(worldMatrix);
	
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
	for (size_t subsetIndex = 0; subsetIndex < subsetCount; ++subsetIndex)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[subsetIndex];
		const Material* pMaterial = pSkinnedMeshRenderer->GetMaterialPtr(subsetIndex);
		m_basicEffectPNTTSkinned.SetMaterial(pMaterial);

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

void Renderer::RenderBillboard(const Billboard* pBillboard)
{
	m_billboardEffect.SetWorldMatrix(pBillboard->GetWorldMatrixCache());
	m_billboardEffect.SetMaterial(pBillboard->GetMaterialPtr());

	m_effectImmediateContext.Apply(&m_billboardEffect);
	m_effectImmediateContext.Draw(4, 0);
}

void Renderer::RenderPanel(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Panel* pPanel)
{
	XMFLOAT2 wcp;
	pPanel->m_transform.GetWinCoordPosition(&wcp);

	D2D1_ROUNDED_RECT rect;
	rect.rect.left = wcp.x - pPanel->GetHalfSizeX();
	rect.rect.right = wcp.x + pPanel->GetHalfSizeX();
	rect.rect.top = wcp.y - pPanel->GetHalfSizeY();
	rect.rect.bottom = wcp.y + pPanel->GetHalfSizeY();
	
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pPanel->GetColor()));

	// BeginDraw
	pD2DRenderTarget->BeginDraw();

	switch (pPanel->GetShape())
	{
	case PanelShape::Rectangle:
		pD2DRenderTarget->FillRectangle(&rect.rect, pBrush);
		break;
	case PanelShape::RoundedRectangle:
		rect.radiusX = pPanel->GetRadiusX();
		rect.radiusY = pPanel->GetRadiusY();
		pD2DRenderTarget->FillRoundedRectangle(&rect, pBrush);
		break;
	default:
		break;
	}

	// EndDraw
	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderImage(const Image* pImage)
{
	XMFLOAT2 hcsp;
	pImage->m_transform.GetHCSPosition(&hcsp);
	m_imageEffect.SetHCSPosition(hcsp);
	m_imageEffect.SetSize(pImage->GetSizeX(), pImage->GetSizeY());
	m_imageEffect.SetImageTexture(pImage->GetTexture());

	m_effectImmediateContext.Apply(&m_imageEffect);
	m_effectImmediateContext.Draw(4, 0);
}

void Renderer::RenderText(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Text* pText)
{
	UINT32 textLength = static_cast<UINT32>(pText->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pText->GetDWriteTextFormatComInterface();
	pDWriteTextFormat->SetTextAlignment(pText->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pText->GetParagraphAlignment());
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pText->GetColor()));

	XMFLOAT2 wcp;
	pText->m_transform.GetWinCoordPosition(&wcp);
	D2D1_RECT_F layout;
	layout.left = wcp.x - pText->GetHalfSizeX();
	layout.right = wcp.x + pText->GetHalfSizeX();
	layout.top = wcp.y - pText->GetHalfSizeY();
	layout.bottom = wcp.y + pText->GetHalfSizeY();

	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->DrawTextW(
		pText->GetText().c_str(),
		textLength,
		pDWriteTextFormat,
		&layout,
		pBrush
	);

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderButton(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Button* pButton)
{
	// 1. ¹öÆ° ÇÁ·¹ÀÓ ·»´õ¸µ
	ID3D11Buffer* vbs[] = { m_pVBShaded2DQuad };
	UINT strides[] = { sizeof(VFShaded2DQuad) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	if (pButton->IsPressed())
		m_shaded2DQuadEffect.SetColorWeightIndex(SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONCAVE);
	else
		m_shaded2DQuadEffect.SetColorWeightIndex(SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONVEX);

	m_shaded2DQuadEffect.SetColor(pButton->GetButtonColorVector());
	XMFLOAT2 hcsp;
	pButton->m_transform.GetHCSPosition(&hcsp);
	m_shaded2DQuadEffect.SetHCSPosition(hcsp);
	m_shaded2DQuadEffect.SetSize(pButton->GetSizeX(), pButton->GetSizeY());

	m_effectImmediateContext.Apply(&m_shaded2DQuadEffect);
	m_effectImmediateContext.Draw(SHADED_2DQUAD_VERTEX_COUNT, 0);

	// 2. ¹öÆ° ÅØ½ºÆ® ·»´õ¸µ
	UINT32 textLength = static_cast<UINT32>(pButton->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pButton->GetDWriteTextFormatComInterface();
	pDWriteTextFormat->SetTextAlignment(pButton->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pButton->GetParagraphAlignment());
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pButton->GetTextColor()));

	XMFLOAT2 wcp;
	pButton->m_transform.GetWinCoordPosition(&wcp);
	D2D1_RECT_F layout;
	layout.left = wcp.x - pButton->GetHalfSizeX();
	layout.right = wcp.x + pButton->GetHalfSizeX();
	layout.top = wcp.y - pButton->GetHalfSizeY();
	layout.bottom = wcp.y + pButton->GetHalfSizeY();

	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->DrawTextW(
		pButton->GetText().c_str(),
		textLength,
		pDWriteTextFormat,
		&layout,
		pBrush
	);

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderInputField(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const InputField* pInputField)
{
	// Input Field ¹è°æ ·»´õ¸µ
	const InputFieldShape ifs = pInputField->GetShape();
	if (ifs == InputFieldShape::ClientEdge)
	{
		ID3D11Buffer* vbs[] = { m_pVBShaded2DQuad };
		UINT strides[] = { sizeof(VFShaded2DQuad) };
		UINT offsets[] = { 0 };

		m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

		m_shaded2DQuadEffect.SetColorWeightIndex(SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONCAVE);
		m_shaded2DQuadEffect.SetColor(pInputField->GetBkColorVector());
		XMFLOAT2 hcsp;
		pInputField->m_transform.GetHCSPosition(&hcsp);
		m_shaded2DQuadEffect.SetHCSPosition(hcsp);
		m_shaded2DQuadEffect.SetSize(pInputField->GetSizeX(), pInputField->GetSizeY());

		m_effectImmediateContext.Apply(&m_shaded2DQuadEffect);
		m_effectImmediateContext.Draw(SHADED_2DQUAD_VERTEX_COUNT, 0);
	}

	XMFLOAT2 wcp;
	pInputField->m_transform.GetWinCoordPosition(&wcp);
	// Text ·»´õ¸µ & D2D ÇÁ·¹ÀÓ Å¸ÀÔ ·»´õ¸µ½Ã ÇÊ¿ä
	D2D1_ROUNDED_RECT layout;
	layout.rect.left = wcp.x - pInputField->GetHalfSizeX();
	layout.rect.right = wcp.x + pInputField->GetHalfSizeX();
	layout.rect.top = wcp.y - pInputField->GetHalfSizeY();
	layout.rect.bottom = wcp.y + pInputField->GetHalfSizeY();

	// BeginDraw
	pD2DRenderTarget->BeginDraw();

	if (ifs != InputFieldShape::ClientEdge)
	{
		pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pInputField->GetBkColor()));

		if (ifs == InputFieldShape::Rectangle)
		{
			pD2DRenderTarget->FillRectangle(&layout.rect, pBrush);
		}
		else
		{
			layout.radiusX = pInputField->GetRadiusX();
			layout.radiusY = pInputField->GetRadiusY();
			pD2DRenderTarget->FillRoundedRectangle(&layout, pBrush);
		}
	}

	// do-while(false) for safe EndDraw
	do
	{
		// Text ·»´õ¸µ
		UINT32 textLength = static_cast<UINT32>(pInputField->GetText().length());
		if (textLength == 0)
			break;

		// ÅØ½ºÆ® ·¹ÀÌ¾Æ¿ô ¿©¹é
		layout.rect.left += 2.0f;
		layout.rect.right -= 2.0f;

		IDWriteTextFormat* pDWriteTextFormat = pInputField->GetDWriteTextFormatComInterface();

		pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pInputField->GetTextColor()));
		pDWriteTextFormat->SetTextAlignment(pInputField->GetTextAlignment());
		pDWriteTextFormat->SetParagraphAlignment(pInputField->GetParagraphAlignment());

		PCTSTR pContent;

		if (pInputField->IsPassword())
		{
			const INT32 diff = static_cast<INT32>(textLength) - static_cast<INT32>(m_asteriskStr.length());
			if (diff > 0)
				m_asteriskStr.append(diff, L'*');

			pContent = m_asteriskStr.c_str();
		}
		else
		{
			pContent = pInputField->GetText().c_str();
		}

		pD2DRenderTarget->DrawTextW(
			pContent,
			textLength,
			pDWriteTextFormat,
			&layout.rect,
			pBrush
		);
	} while (false);

	// EndDraw
	pD2DRenderTarget->EndDraw();
}

void Renderer::RenderSliderControl(const SliderControl* pSliderControl)
{
	// °øÅë»çÇ× ¼³Á¤
	ID3D11Buffer* vbs[] = { m_pVBShaded2DQuad };
	UINT strides[] = { sizeof(VFShaded2DQuad) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	// Track ·»´õ¸µ
	XMFLOAT2 hcsp;
	pSliderControl->m_transform.GetHCSPosition(&hcsp);
	m_shaded2DQuadEffect.SetHCSPosition(hcsp);

	m_shaded2DQuadEffect.SetColorWeightIndex(SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONCAVE);
	m_shaded2DQuadEffect.SetColor(pSliderControl->GetTrackColorVector());

	if (pSliderControl->GetSliderControlType() == SliderControlType::Horizontal)
		m_shaded2DQuadEffect.SetSize(pSliderControl->GetTrackLength(), pSliderControl->GetTrackThickness());
	else
		m_shaded2DQuadEffect.SetSize(pSliderControl->GetTrackThickness(), pSliderControl->GetTrackLength());

	m_effectImmediateContext.Apply(&m_shaded2DQuadEffect);
	m_effectImmediateContext.Draw(SHADED_2DQUAD_VERTEX_COUNT, 0);

	// Thumb ·»´õ¸µ
	XMFLOAT2 thumbHcsp;
	XMFLOAT2 thumbOffset;
	pSliderControl->ComputeHCSCoordThumbOffset(&thumbOffset);
	thumbHcsp.x = hcsp.x + thumbOffset.x;
	thumbHcsp.y = hcsp.y + thumbOffset.y;
	m_shaded2DQuadEffect.SetHCSPosition(thumbHcsp);

	m_shaded2DQuadEffect.SetColorWeightIndex(SHADED_2DQUAD_COLOR_WEIGHT_INDEX_CONVEX);
	m_shaded2DQuadEffect.SetColor(pSliderControl->GetThumbColorVector());

	m_shaded2DQuadEffect.SetSize(pSliderControl->GetThumbSizeX(), pSliderControl->GetThumbSizeY());

	m_effectImmediateContext.Apply(&m_shaded2DQuadEffect);
	m_effectImmediateContext.Draw(SHADED_2DQUAD_VERTEX_COUNT, 0);
}

void Renderer::RenderCheckbox(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Checkbox* pCheckbox)
{
	// Ã¼Å©¹Ú½º ·»´õ¸µ
	ID3D11Buffer* vbs[] = { m_pVBCheckbox };
	UINT strides[] = { sizeof(VFCheckbox) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_checkboxEffect.SetBoxColor(pCheckbox->GetBoxColorVector());
	if (pCheckbox->IsChecked())
		m_checkboxEffect.SetCheckColor(pCheckbox->GetCheckColorVector());
	else
		m_checkboxEffect.SetCheckColor(pCheckbox->GetBoxColorVector());	// Ã¼Å©µÇÁö ¾ÊÀº »óÅÂÀÏ °æ¿ì Ã¼Å© »ö»ó°ú ¹Ú½º »ö»óÀ» ÀÏÄ¡

	XMFLOAT2 hcsp;
	pCheckbox->m_transform.GetHCSPosition(&hcsp);
	m_checkboxEffect.SetHCSPosition(hcsp);
	m_checkboxEffect.SetSize(pCheckbox->GetCheckboxSizeX(), pCheckbox->GetCheckboxSizeY());

	m_effectImmediateContext.Apply(&m_checkboxEffect);
	m_effectImmediateContext.Draw(CHECKBOX_VERTEX_COUNT, 0);

	// ÅØ½ºÆ® ·»´õ¸µ
	UINT32 textLength = static_cast<UINT32>(pCheckbox->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pCheckbox->GetDWriteTextFormatComInterface();
	pDWriteTextFormat->SetTextAlignment(pCheckbox->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pCheckbox->GetParagraphAlignment());
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pCheckbox->GetTextColor()));

	constexpr FLOAT SPACE_BETWEEN_CHECKBOX_AND_TEXTBOX = 5.0f;
	XMFLOAT2 wcp;
	pCheckbox->m_transform.GetWinCoordPosition(&wcp);
	FLOAT textboxOffsetX = pCheckbox->GetCheckboxHalfSizeX() + pCheckbox->GetTextboxHalfSizeX() + SPACE_BETWEEN_CHECKBOX_AND_TEXTBOX;
	if (pCheckbox->IsLeftText())
		textboxOffsetX = -textboxOffsetX;

	wcp.x += textboxOffsetX;
	D2D1_RECT_F layout;
	layout.left = wcp.x - pCheckbox->GetTextboxHalfSizeX();
	layout.right = wcp.x + pCheckbox->GetTextboxHalfSizeX();
	layout.top = wcp.y - pCheckbox->GetTextboxHalfSizeY();
	layout.bottom = wcp.y + pCheckbox->GetTextboxHalfSizeY();

	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->DrawTextW(
		pCheckbox->GetText().c_str(),
		textLength,
		pDWriteTextFormat,
		&layout,
		pBrush
	);

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

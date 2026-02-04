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
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\RadioButton.h>
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
#include <ZergEngine\CoreSystem\Physics.h>
#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>
#include <algorithm>

using namespace ze;

Renderer* Renderer::s_pInstance = nullptr;

// 카메라로부터의 거리 제곱 내림차순 정렬
struct BillboardComparator
{
	bool operator()(const std::pair<const Billboard*, float>& a, const std::pair<const Billboard*, float>& b) const
	{
		return a.second > b.second; // 내림차순
	}
};

Renderer::Renderer()
	: m_pRSSolidCullBack(nullptr)
	, m_pRSMultisampleSolidCullBack(nullptr)
	, m_pRSSolidCullNone(nullptr)
	, m_pRSMultisampleSolidCullNone(nullptr)
	, m_pRSWireframe(nullptr)
	, m_pRSMultisampleWireframe(nullptr)
	, m_pDSSDepthReadWrite(nullptr)
	, m_pDSSDepthReadOnly(nullptr)
	, m_pDSSSkybox(nullptr)
	, m_pDSSNoDepthStencilTest(nullptr)
	, m_pBSOpaque(nullptr)
	, m_pBSAlphaBlend(nullptr)
	, m_pBSNoColorWrite(nullptr)
	, m_pVBPNTTQuad(nullptr)
	, m_pVBCheckbox(nullptr)
	, m_effectImmediateContext()
	, m_pAnimFinalTransformIdentity(nullptr)
	, m_debugLinesEffect()
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
	, m_shadedEdgeQuadEffect()
	, m_shadedEdgeCircleEffect()
	, m_checkboxEffect()
	, m_imageEffect()
	, m_asteriskStr(L"********************************")	// L'*' x 32
	, m_billboardRenderQueue()
	, m_uiRenderQueue(256)
{
	m_uiRenderQueue.clear();

	m_pAnimFinalTransformIdentity = reinterpret_cast<XMFLOAT4X4A*>(_aligned_malloc_dbg(sizeof(XMFLOAT4X4A) * MAX_BONE_COUNT, alignof(XMFLOAT4X4A), __FILE__, __LINE__));;

	for (size_t i = 0; i < MAX_BONE_COUNT; ++i)
		XMStoreFloat4x4A(&m_pAnimFinalTransformIdentity[i], XMMatrixIdentity());
}

Renderer::~Renderer()
{
	_aligned_free_dbg(m_pAnimFinalTransformIdentity);
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
	m_pDSSDepthReadWrite = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::DepthReadWrite);
	m_pDSSDepthReadOnly = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::DepthReadOnly);
	m_pDSSSkybox = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::Skybox);
	m_pDSSNoDepthStencilTest = pGraphicDevice->GetDSSComInterface(DepthStencilStateType::NoDepthStencilTest);
	m_pBSOpaque = pGraphicDevice->GetBSComInterface(BlendStateType::Opaque);
	m_pBSAlphaBlend = pGraphicDevice->GetBSComInterface(BlendStateType::AlphaBlend);
	m_pBSNoColorWrite = pGraphicDevice->GetBSComInterface(BlendStateType::NoColorWrite);
	m_pVBPNTTQuad = pGraphicDevice->GetVBPNTTQuad();
	m_pVBCheckbox = pGraphicDevice->GetVBCheckbox();


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ INITIALIZE EFFECTS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	m_debugLinesEffect.Init();
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
	m_shadedEdgeQuadEffect.Init();
	m_shadedEdgeCircleEffect.Init();
	m_checkboxEffect.Init();
	m_imageEffect.Init();

	// effect context 준비
	assert(pGraphicDevice->GetImmediateContext() != nullptr);
	m_effectImmediateContext.AttachDeviceContext(pGraphicDevice->GetImmediateContext());
}

void Renderer::Shutdown()
{
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ RELEASE EFFECTS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	m_debugLinesEffect.Release();
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
	m_shadedEdgeQuadEffect.Release();
	m_shadedEdgeCircleEffect.Release();
	m_checkboxEffect.Release();
	m_imageEffect.Release();
}

void Renderer::RenderFrame()
{
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContext();

	ID3D11SamplerState* const ssArr[] =
	{
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Anisotropic4x),	// s0 common sampler
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Trilinear),		// s1 normal map sampler
		GraphicDevice::GetInstance()->GetSSComInterface(TextureFilteringMode::Bilinear),		// s2 bilinear sampler
	};
	pImmContext->DSSetSamplers(0, _countof(ssArr), ssArr);
	pImmContext->PSSetSamplers(0, _countof(ssArr), ssArr);

	// PerFrame 상수버퍼 업데이트 및 바인딩
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

	// Camera마다 프레임 렌더링

	// DepthStencil State
	for (IComponent* pComponent : CameraManager::GetInstance()->m_directAccessGroup)
	{
		Camera* pCamera = static_cast<Camera*>(pComponent);

		if (!pCamera->IsEnabled())
			continue;

		// 불투명 패스
		// Rasterizer State
		if (pCamera->GetMSAAMode() == MSAAMode::Off)
			pImmContext->RSSetState(m_pRSSolidCullBack);
		else
			pImmContext->RSSetState(m_pRSMultisampleSolidCullBack);	// 후면 컬링 설정

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSDepthReadWrite, 0);

		// BasicState
		pImmContext->OMSetBlendState(m_pBSOpaque, nullptr, 0xFFFFFFFF);


		// 버퍼 재생성 필요시 생성
		if (pCamera->GetColorBufferRTV() == nullptr)
			if (!pCamera->CreateViews())
				continue;

		pCamera->UpdateViewMatrix();	// 뷰 변환 행렬 업데이트

		// 월드 스페이스 절두체 생성
		Frustum frustumW(pCamera->GetProjMatrix(), false);	// 원근 투영 기반 절두체
		frustumW.Transform(frustumW, XMMatrixInverse(nullptr, pCamera->GetViewMatrix()));

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// 뷰포트 바인딩
		pImmContext->RSSetViewports(1, &pCamera->GetEntireBufferViewport());
		// 컬러 버퍼 및 뎁스스텐실 버퍼 바인딩
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// PerCamera 상수버퍼 업데이트 및 바인딩
		m_debugLinesEffect.SetCamera(pCamera);
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

		// <Mesh 렌더링>
		for (const IComponent* pComponent : MeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;
			
			// 프러스텀 컬링
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

		// <SkinnedMesh 렌더링>
		for (const IComponent* pComponent : SkinnedMeshRendererManager::GetInstance()->m_directAccessGroup)
		{
			const SkinnedMeshRenderer* pMeshRenderer = static_cast<const SkinnedMeshRenderer*>(pComponent);
			const SkinnedMesh* pMesh = pMeshRenderer->GetMeshPtr();

			if (!pMeshRenderer->IsEnabled() || pMesh == nullptr)
				continue;

			// 프러스텀 컬링
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

		// 지형 렌더링
		for (const IComponent* pComponent : TerrainManager::GetInstance()->m_directAccessGroup)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);
			RenderTerrain(pTerrain);
		}

		// <스카이박스 렌더링>
		ID3D11ShaderResourceView* pSkyboxCubeMap = RenderSettings::GetInstance()->m_skyboxCubeMap.GetSRV();
		if (pSkyboxCubeMap)
		{
			pImmContext->OMSetDepthStencilState(m_pDSSSkybox, 0);
			RenderSkybox(pSkyboxCubeMap);
			pImmContext->OMSetDepthStencilState(m_pDSSDepthReadWrite, 0);
		}


		// 투명 패스
		// 
		// <빌보드 렌더링>
		// 파이프라인 상태 설정
		// Rasterizer State
		// 빌보드는 항상 카메라를 향하므로 컬링이 필요없음.
		if (pCamera->GetMSAAMode() == MSAAMode::Off)
			pImmContext->RSSetState(m_pRSSolidCullNone);
		else
			pImmContext->RSSetState(m_pRSMultisampleSolidCullNone);

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSDepthReadOnly, 0);

		// Blend State
		// D3D11_BLEND_BLEND_FACTOR 또는 D3D11_BLEND_INV_BLEND_FACTOR 미사용 (blend factor로 nullptr 전달)
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
			// 빌보드의 기저 벡터 계산
			XMVECTOR up = cameraUpW;	// up 벡터는 카메라의 up 벡터와 동일
			XMVECTOR forward = XMVectorNegate(cameraForwardW);	// 빌보드의 forward는 camera의 forward와 반대방향
			XMVECTOR right = XMVector3Cross(up, forward);	// right 벡터는 up, forward로 도출

			screenAlignedBillboardRotationW.r[0] = right;
			screenAlignedBillboardRotationW.r[1] = up;
			screenAlignedBillboardRotationW.r[2] = forward;
			screenAlignedBillboardRotationW.r[3] = Math::IdentityR3();
		}

		// 활성화된 빌보드 컴포넌트 검색 및 정렬 (큐에 포인터 이동 후 정렬, Manager 배열에서 정렬하면 groupIndex 업데이트 등 복잡해짐)
		m_billboardRenderQueue.clear();
		for (IComponent* pComponent : BillboardManager::GetInstance()->m_directAccessGroup)
		{
			Billboard* pBillboard = static_cast<Billboard*>(pComponent);
			if (!pBillboard->IsEnabled() || pBillboard == nullptr)
				continue;

			// 프러스텀 컬링
			XMVECTOR billboardScaleW = XMVectorMultiply(
				XMVectorSet(pBillboard->GetBillboardQuadWidth(), pBillboard->GetBillboardQuadHeight(), 1.0f, 0.0f),
				pBillboard->m_pGameObject->m_transform.GetWorldScale()
			);

			Obb obbW;
			XMStoreFloat3(&obbW.Center, XMVectorZero());
			XMStoreFloat3(&obbW.Extents, billboardScaleW);
			obbW.Extents.z = BOUNDING_BOX_MIN_EXTENT;	// z 볼륨 0.01f
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

			// 프러스텀 컬링에 사용한 행렬이 렌더링 시에도 필요하므로 캐싱
			pBillboard->CacheWorldMatrix(worldMatrix);

			// 카메라로부터의 거리의 제곱 계산
			XMVECTOR lengthSquared = XMVector3LengthSq(XMVectorSubtract(billboardPosW, cameraPosW));
			m_billboardRenderQueue.push_back(std::make_pair(pBillboard, XMVectorGetX(lengthSquared)));	// 빌보드 포인터, 거리 제곱 pair
		}

		// 빌보드 정렬 및 렌더링
		std::sort(m_billboardRenderQueue.begin(), m_billboardRenderQueue.end(), BillboardComparator());
		for (const auto& pair : m_billboardRenderQueue)
			RenderBillboard(pair.first);


		// 디버그 시각 정보 렌더링 (루프 밖에서 업데이트하고 모든 카메라가 공유)
		if (Physics::GetInstance()->m_drawDebugInfo)
			RenderDebugInfo();
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 카메라 렌더링 결과 병합 및 스왑체인 버퍼에 UI 렌더링
	// Rasterizer State
	pImmContext->RSSetState(m_pRSSolidCullNone);	// Quad 렌더링 뿐이므로 후면 컬링 끄기
	
	// DepthStencil State
	pImmContext->OMSetDepthStencilState(m_pDSSNoDepthStencilTest, 0);
	
	// Blend State
	pImmContext->OMSetBlendState(m_pBSOpaque, nullptr, 0xFFFFFFFF);
	
	// 전체 백버퍼에 대한 뷰포트 설정
	pImmContext->RSSetViewports(1, &GraphicDevice::GetInstance()->GetEntireSwapChainViewport());
	// 렌더타겟 바인딩
	ID3D11RenderTargetView* pSwapChainRTV = GraphicDevice::GetInstance()->GetSwapChainRTV();
	ID3D11DepthStencilView* pSwapChainDSV = GraphicDevice::GetInstance()->GetSwapChainDSV();
	pImmContext->ClearRenderTargetView(pSwapChainRTV, DirectX::ColorsLinear::Blue);
	pImmContext->ClearDepthStencilView(pSwapChainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pSwapChainRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pSwapChainDSV);
	
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
			m_drawScreenQuadMSTex.SetTexture(pCamera->GetColorBufferSRV());

			m_effectImmediateContext.Apply(&m_drawScreenQuadMSTex);
			m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
		}
		else
		{
			m_drawScreenQuadTex.SetScreenRatioQuadParam(
				pCamera->m_viewportRect.m_width,
				pCamera->m_viewportRect.m_height,
				pCamera->m_viewportRect.m_x,
				pCamera->m_viewportRect.m_y
			);
			m_drawScreenQuadTex.SetTexture(pCamera->GetColorBufferSRV());

			m_effectImmediateContext.Apply(&m_drawScreenQuadTex);
			m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
		}
	}

	{
		// 단일 카메라 사용하는 씬에서 다음 프레임에 동일 카메라 버퍼가 Input/Output으로 동시에 바인딩 되는것을 방지
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_effectImmediateContext.GetDeviceContext()->PSSetShaderResources(0, 1, srvs);
	}
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// UI 렌더링
	{
		// Rasterizer State
		pImmContext->RSSetState(m_pRSSolidCullNone);	// Quad 렌더링 뿐이므로 후면 컬링 끄기

		// DepthStencil State
		pImmContext->OMSetDepthStencilState(m_pDSSNoDepthStencilTest, 0);

		// Blend State
		// D3D11_BLEND_BLEND_FACTOR 또는 D3D11_BLEND_INV_BLEND_FACTOR 미사용 (blend factor로 nullptr 전달)
		pImmContext->OMSetBlendState(m_pBSAlphaBlend, nullptr, 0xFFFFFFFF);

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// 모든 UI Effect들에 대한 ScreenToNDCSpaceRatio 설정
		const XMFLOAT2 screenToNDCSpaceRatio = XMFLOAT2(
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Width),
			2.0f / static_cast<float>(GraphicDevice::GetInstance()->GetSwapChainDesc().BufferDesc.Height)
		);
		m_shadedEdgeQuadEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_shadedEdgeCircleEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_checkboxEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		m_imageEffect.SetScreenToNDCSpaceRatio(screenToNDCSpaceRatio);
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

		ID2D1RenderTarget* pD2DRenderTarget = GraphicDevice::GetInstance()->GetSwapChainD2DRT();
		ID2D1SolidColorBrush* pBrush = GraphicDevice::GetInstance()->GetD2DSolidColorBrush();
		// 모든 루트 UI오브젝트들부터 순회하며 자식 UI까지 렌더링
		for (const IUIObject* pRootUIObject : UIObjectManager::GetInstance()->m_roots)
		{
			assert(pRootUIObject->IsRoot());

			size_t index = 0;

			// 큐는 노드 할당이 일어나므로 배열을 사용.

			if (pRootUIObject->IsActive())
				m_uiRenderQueue.push_back(pRootUIObject);

			while (index < m_uiRenderQueue.size())
			{
				const IUIObject* pUIObject = m_uiRenderQueue[index];

				// 자식들을 모두 큐에 넣는다.
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
				case UIObjectType::RadioButton:
					this->RenderRadioButton(pD2DRenderTarget, pBrush, static_cast<const RadioButton*>(pUIObject));
					break;
				default:
					break;
				}

				++index;
			}

			// 렌더링이 완료된 후 큐 클리어
			m_uiRenderQueue.clear();
		}
	}

	HRESULT hr = GraphicDevice::GetInstance()->GetSwapChain()->Present(1, 0);
}

/*
void XM_CALLCONV Renderer::RenderVFPositionMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix)
{
	const StaticMesh* pMesh = pMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	// assert(pMesh->GetVertexFormatType() == VertexFormatType::Position);

	m_basicEffectP.SetWorldMatrix(worldMatrix);

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::Position) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋 순회 시 변경되는 사항이 없으므로 루프 밖에서 Apply 한 번만 호출
	m_effectImmediateContext.Apply(&m_basicEffectP);

	// 서브셋들 순회하며 렌더링
	for (const MeshSubset& subset : pMesh->m_subsets)
	{
		// 드로우
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

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionColor) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋 순회 시 변경되는 사항이 없으므로 루프 밖에서 Apply 한 번만 호출
	m_effectImmediateContext.Apply(&m_basicEffectPC);

	// 서브셋들 순회하며 렌더링
	for (const MeshSubset& subset : pMesh->m_subsets)
	{
		// 드로우
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

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormal) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋들 순회하며 렌더링
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

		// 드로우
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

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋들 순회하며 렌더링
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t i = 0; i < subsetCount; ++i)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[i];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(i);
		if (pMaterial != nullptr)
			m_basicEffectPT.SetTexture(pMaterial->m_diffuseMap.GetSRV());
		else
			m_basicEffectPT.SetTexture(nullptr);

		m_effectImmediateContext.Apply(&m_basicEffectPT);

		// 드로우
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

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋들 순회하며 렌더링
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
			m_basicEffectPNT.SetDiffuseMap(pMaterial->m_diffuseMap.GetSRV());
			m_basicEffectPNT.SetSpecularMap(pMaterial->m_specularMap.GetSRV());
		}
		else
		{
			m_basicEffectPNT.UseMaterial(false);
		}

		m_effectImmediateContext.Apply(&m_basicEffectPNT);

		// 드로우
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

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTangentTexCoord) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋들 순회하며 렌더링
	assert(pMesh->m_subsets.size() == pMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t subsetIndex = 0; subsetIndex < subsetCount; ++subsetIndex)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[subsetIndex];
		const Material* pMaterial = pMeshRenderer->GetMaterialPtr(subsetIndex);
		m_basicEffectPNTT.SetMaterial(pMaterial);

		m_effectImmediateContext.Apply(&m_basicEffectPNTT);

		// 드로우
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void XM_CALLCONV Renderer::RenderPNTTSkinnedMesh(const SkinnedMeshRenderer* pSkinnedMeshRenderer, FXMMATRIX worldMatrix)
{
	const SkinnedMesh* pMesh = pSkinnedMeshRenderer->GetMeshPtr();
	if (!pMesh)
		return;

	m_basicEffectPNTTSkinned.SetWorldMatrix(worldMatrix);

	const Armature* const pArmature = pSkinnedMeshRenderer->GetArmaturePtr();

	if (!pArmature || pSkinnedMeshRenderer->GetCurrentAnims().size() == 0)
	{
		m_basicEffectPNTTSkinned.SetArmatureFinalTransform(m_pAnimFinalTransformIdentity, MAX_BONE_COUNT);
	}
	else
	{
		XMFLOAT4X4A transformBuffer1[MAX_BONE_COUNT];
		XMFLOAT4X4A transformBuffer2[MAX_BONE_COUNT];
		// 2패스 구조
		// Pass 1.
		// 모든 뼈 각각에 대해 현재 재생중인 애니메이션에 대한 로컬 변환을 구해둔다. 이때 키 프레임의 로컬 변환에 Additive Transform도 성분을 추가해줄 수 있을듯.

		// Assimp로 로드한 키 프레임 = Ml * Mp (리깅 시 애니메이션 프레임에서 뼈의 스케일링 & 회전 & 이동은 Ml * Mp이다.)

		XMFLOAT4X4A* pMlMp = transformBuffer1;
		for (size_t i = 0; i < MAX_BONE_COUNT; ++i)	// 애니메이션을 재생중이지 않은 뼈들은 Identity 변환행렬 사용.
			XMStoreFloat4x4A(&pMlMp[i], XMMatrixIdentity());

		// 각 본마다 자신이 재생중인 애니메이션 키 프레임을 보간 샘플링 및 변환행렬 저장.
		const auto& currAnims = pSkinnedMeshRenderer->GetCurrentAnims();
		for (const auto& pair : currAnims)
		{
			const BoneAnimation* const pBoneAnimations = pair.second.m_pAnim->GetBoneAnimations();
			const std::vector<bone_index_type>* const pBoneGroup = pSkinnedMeshRenderer->GetArmature()->GetBoneGroup(pair.first);

			for (const bone_index_type boneIndex : *pBoneGroup)
			{
				XMFLOAT3A scale;
				XMFLOAT4A rotation;
				XMFLOAT3A translation;
				pBoneAnimations[boneIndex].Interpolate(pair.second.m_timeCursor, &scale, &rotation, &translation);

				XMStoreFloat4x4A(
					&pMlMp[boneIndex],
					XMMatrixScalingFromVector(XMLoadFloat3A(&scale)) *
					XMMatrixRotationQuaternion(XMLoadFloat4A(&rotation)) *
					XMMatrixTranslationFromVector(XMLoadFloat3A(&translation))
				);
			}
		}

		// Pass 2.
		// 0번 뼈부터 높은 인덱스로 가며 Ma 행렬을 계산하고 최종적으로 Mf(i) = MdInv(i) * Ml(i) * Mp(i) * Ma(i-1)를 계산한다.
		const Armature* const pArmature = pSkinnedMeshRenderer->GetArmaturePtr();
		const BYTE* const pBoneHierarchy = pArmature->GetBoneHierarchy();
		const size_t boneCount = pArmature->GetBoneCount();

		// Ma 계산
		XMFLOAT4X4A* pMa = transformBuffer2;
		if (boneCount > 0)	// 먼저 Ma 배열의 0번 인덱스를 채우고 (Ma(0) = Ml(0) * Mp(0))
			XMStoreFloat4x4A(&pMa[0], XMLoadFloat4x4A(&pMlMp[0]));

		for (size_t boneIndex = 1; boneIndex < boneCount; ++boneIndex)	// Ma(i) = Ml(i) * Mp(i) * Ma(i - 1)
			XMStoreFloat4x4A(&pMa[boneIndex], XMLoadFloat4x4A(&pMlMp[boneIndex]) * XMLoadFloat4x4A(&pMa[pBoneHierarchy[boneIndex]]));



		// Mf (Final transform) 계산
		XMFLOAT4X4A* Mf = transformBuffer1;	// MlMp 저장에 사용하던 임시 버퍼를 재활용(이 시점에서는 MlMp 버퍼 내용은 필요 없음)
		const XMFLOAT4X4A* pMdInvArray = pArmature->GetMdInvArray();
		for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
			XMStoreFloat4x4A(&Mf[boneIndex], XMLoadFloat4x4A(&pMdInvArray[boneIndex]) * XMLoadFloat4x4A(&pMa[boneIndex]));


		// HLSL 전달을 위해 Mf 전치
		for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
			XMStoreFloat4x4A(&Mf[boneIndex], ConvertToHLSLMatrix(XMLoadFloat4x4A(&Mf[boneIndex])));

		m_basicEffectPNTTSkinned.SetArmatureFinalTransform(Mf, boneCount);
	}

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTangentTexCoordSkinned) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// 서브셋들 순회하며 렌더링
	assert(pMesh->m_subsets.size() == pSkinnedMeshRenderer->GetMeshPtr()->m_subsets.size());
	const size_t subsetCount = pMesh->m_subsets.size();
	for (size_t subsetIndex = 0; subsetIndex < subsetCount; ++subsetIndex)
	{
		const MeshSubset& currentSubset = pMesh->m_subsets[subsetIndex];
		const Material* pMaterial = pSkinnedMeshRenderer->GetMaterialPtr(subsetIndex);
		m_basicEffectPNTTSkinned.SetMaterial(pMaterial);

		m_effectImmediateContext.Apply(&m_basicEffectPNTTSkinned);

		// 드로우
		m_effectImmediateContext.DrawIndexed(currentSubset.GetIndexCount(), currentSubset.GetStartIndexLocation(), 0);
	}
}

void Renderer::RenderTerrain(const Terrain* pTerrain)
{
	m_terrainEffect.SetMaxHeight(pTerrain->GetMaxHeight());
	m_terrainEffect.SetTilingScale(pTerrain->GetTilingScale());
	m_terrainEffect.SetFieldMap(pTerrain->m_heightMap.GetSRV(), pTerrain->m_normalMap.GetSRV());
	m_terrainEffect.SetLayerTexture(
		pTerrain->m_diffuseMapLayer.GetSRV(),
		pTerrain->m_normalMapLayer.GetSRV(),
		pTerrain->m_blendMap.GetSRV()
	);

	// 버텍스 버퍼 설정
	const UINT strides[] = { InputLayoutHelper::GetStructureByteStride(VertexFormatType::TerrainPatchCtrlPt) };
	const UINT offsets[] = { 0 };
	ID3D11Buffer* const vbs[] = { pTerrain->GetPatchControlPointVertexBuffer() };
	m_effectImmediateContext.IASetVertexBuffers(0, _countof(vbs), vbs, strides, offsets);

	// 인덱스 버퍼 설정
	m_effectImmediateContext.IASetIndexBuffer(pTerrain->GetPatchControlPointIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	m_effectImmediateContext.Apply(&m_terrainEffect);
	m_effectImmediateContext.DrawIndexed(pTerrain->GetPatchControlPointIndexCount(), 0, 0);
}

void Renderer::RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV)
{
	assert(pSkyboxCubeMapSRV != nullptr);

	m_skyboxEffect.SetSkybox(pSkyboxCubeMapSRV);

	m_effectImmediateContext.Apply(&m_skyboxEffect);

	// 드로우
	// 셰이더 지역변수 정점들 사용 (총 36개)
	m_effectImmediateContext.Draw(36, 0);
}

void Renderer::RenderBillboard(const Billboard* pBillboard)
{
	ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
	UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_billboardEffect.SetWorldMatrix(pBillboard->GetWorldMatrixCache());
	m_billboardEffect.SetUVScale(pBillboard->GetUVScaleX(), pBillboard->GetUVScaleY());
	m_billboardEffect.SetUVOffset(pBillboard->GetUVOffsetX(), pBillboard->GetUVOffsetY());
	m_billboardEffect.SetMaterial(pBillboard->GetMaterialPtr());

	m_effectImmediateContext.Apply(&m_billboardEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
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
	ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
	UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	XMFLOAT2 hcsp;
	pImage->m_transform.GetHCSPosition(&hcsp);
	m_imageEffect.SetHCSPosition(hcsp);
	m_imageEffect.SetSize(pImage->GetSizeX(), pImage->GetSizeY());
	m_imageEffect.SetUVScale(pImage->GetUVScaleX(), pImage->GetUVScaleY());
	m_imageEffect.SetUVOffset(pImage->GetUVOffsetX(), pImage->GetUVOffsetY());
	m_imageEffect.SetImageTexture(pImage->GetTexture());

	m_effectImmediateContext.Apply(&m_imageEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
}

void Renderer::RenderText(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Text* pText)
{
	UINT32 textLength = static_cast<UINT32>(pText->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pText->GetDWriteTextFormat();
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
	// 1. 버튼 프레임 렌더링
	ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
	UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	XMFLOAT2 hcsp;
	pButton->m_transform.GetHCSPosition(&hcsp);
	m_shadedEdgeQuadEffect.SetHCSPosition(hcsp);
	m_shadedEdgeQuadEffect.SetSize(pButton->GetSizeX(), pButton->GetSizeY());

	if (pButton->IsPressed())
		m_shadedEdgeQuadEffect.SetColorWeight(-0.5f, +0.5f);
	else
		m_shadedEdgeQuadEffect.SetColorWeight(+0.5f, -0.5f);

	m_shadedEdgeQuadEffect.SetColor(pButton->GetButtonColorVector());

	m_effectImmediateContext.Apply(&m_shadedEdgeQuadEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);

	// 2. 버튼 텍스트 렌더링
	UINT32 textLength = static_cast<UINT32>(pButton->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pButton->GetDWriteTextFormat();
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
	// Input Field 배경 렌더링
	const InputFieldShape ifs = pInputField->GetShape();
	if (ifs == InputFieldShape::ClientEdge)
	{
		ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
		UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
		UINT offsets[] = { 0 };

		m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

		XMFLOAT2 hcsp;
		pInputField->m_transform.GetHCSPosition(&hcsp);
		m_shadedEdgeQuadEffect.SetHCSPosition(hcsp);
		m_shadedEdgeQuadEffect.SetSize(pInputField->GetSizeX(), pInputField->GetSizeY());

		m_shadedEdgeQuadEffect.SetColorWeight(-0.5f, +0.5f);	// concave input field
		m_shadedEdgeQuadEffect.SetColor(pInputField->GetBkColorVector());

		m_effectImmediateContext.Apply(&m_shadedEdgeQuadEffect);
		m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
	}

	XMFLOAT2 wcp;
	pInputField->m_transform.GetWinCoordPosition(&wcp);
	// Text 렌더링 & D2D 프레임 타입 렌더링시 필요
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
		// Text 렌더링
		UINT32 textLength = static_cast<UINT32>(pInputField->GetText().length());
		if (textLength == 0)
			break;

		// 텍스트 레이아웃 여백
		layout.rect.left += 2.0f;
		layout.rect.right -= 2.0f;

		IDWriteTextFormat* pDWriteTextFormat = pInputField->GetDWriteTextFormat();

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
	// 공통사항 설정
	ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
	UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);


	// 파츠별 렌더링
	// Track
	XMFLOAT2 hcsp;
	pSliderControl->m_transform.GetHCSPosition(&hcsp);
	m_shadedEdgeQuadEffect.SetHCSPosition(hcsp);
	if (pSliderControl->GetSliderControlType() == SliderControlType::Horizontal)
		m_shadedEdgeQuadEffect.SetSize(pSliderControl->GetTrackLength(), pSliderControl->GetTrackThickness());
	else
		m_shadedEdgeQuadEffect.SetSize(pSliderControl->GetTrackThickness(), pSliderControl->GetTrackLength());

	m_shadedEdgeQuadEffect.SetColorWeight(-0.5f, +0.5f);	// concave track
	m_shadedEdgeQuadEffect.SetColor(pSliderControl->GetTrackColorVector());

	m_effectImmediateContext.Apply(&m_shadedEdgeQuadEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);

	// Thumb 렌더링
	XMFLOAT2 thumbHcsp;
	XMFLOAT2 thumbOffset;
	pSliderControl->ComputeHCSCoordThumbOffset(&thumbOffset);
	thumbHcsp.x = hcsp.x + thumbOffset.x;
	thumbHcsp.y = hcsp.y + thumbOffset.y;
	m_shadedEdgeQuadEffect.SetHCSPosition(thumbHcsp);
	m_shadedEdgeQuadEffect.SetSize(pSliderControl->GetThumbSizeX(), pSliderControl->GetThumbSizeY());

	m_shadedEdgeQuadEffect.SetColorWeight(+0.5f, -0.5f);	// convex thumb
	m_shadedEdgeQuadEffect.SetColor(pSliderControl->GetThumbColorVector());

	m_effectImmediateContext.Apply(&m_shadedEdgeQuadEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
}

void Renderer::RenderCheckbox(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Checkbox* pCheckbox)
{
	// 체크박스 렌더링
	ID3D11Buffer* vbs[] = { m_pVBCheckbox };
	UINT strides[] = { sizeof(VFCheckbox) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_checkboxEffect.SetBoxColor(pCheckbox->GetBoxColorVector());
	if (pCheckbox->GetCheck())
		m_checkboxEffect.SetCheckColor(pCheckbox->GetCheckColorVector());
	else
		m_checkboxEffect.SetCheckColor(pCheckbox->GetBoxColorVector());	// 체크되지 않은 상태일 경우 체크 색상과 박스 색상을 일치

	XMFLOAT2 hcsp;
	pCheckbox->m_transform.GetHCSPosition(&hcsp);
	m_checkboxEffect.SetHCSPosition(hcsp);
	m_checkboxEffect.SetSize(pCheckbox->GetCheckboxSizeX(), pCheckbox->GetCheckboxSizeY());

	m_effectImmediateContext.Apply(&m_checkboxEffect);
	m_effectImmediateContext.Draw(CHECKBOX_VERTEX_COUNT, 0);

	// 텍스트 렌더링
	UINT32 textLength = static_cast<UINT32>(pCheckbox->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pCheckbox->GetDWriteTextFormat();
	pDWriteTextFormat->SetTextAlignment(pCheckbox->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pCheckbox->GetParagraphAlignment());
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pCheckbox->GetTextColor()));

	XMFLOAT2 wcp;
	pCheckbox->m_transform.GetWinCoordPosition(&wcp);
	FLOAT textboxOffsetX = pCheckbox->GetCheckboxHalfSizeX() + SPACING_BETWEEN_CHECKBOX_AND_TEXTBOX + pCheckbox->GetTextboxHalfSizeX();
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

void Renderer::RenderRadioButton(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const RadioButton* pRadioButton)
{
	// 1. 버튼 렌더링
	ID3D11Buffer* vbs[] = { m_pVBPNTTQuad };
	UINT strides[] = { sizeof(VFPositionNormalTangentTexCoord) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_shadedEdgeCircleEffect.SetColorWeight(-0.5f, +0.5f);	// concave
	m_shadedEdgeCircleEffect.SetColor(pRadioButton->GetButtonColorVector());
	m_shadedEdgeCircleEffect.SetRadius(pRadioButton->GetRadius());

	XMFLOAT2 hcsp;
	pRadioButton->m_transform.GetHCSPosition(&hcsp);

	m_shadedEdgeCircleEffect.SetHCSPosition(hcsp);
	m_effectImmediateContext.Apply(&m_shadedEdgeCircleEffect);
	m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);

	// 버튼이 선택되어 있으면 작은 원을 추가로 렌더링
	if (pRadioButton->GetCheck())
	{
		m_shadedEdgeCircleEffect.SetColorWeight(0.0f, 0.0f);	// flat
		m_shadedEdgeCircleEffect.SetColor(pRadioButton->GetDotColorVector());
		m_shadedEdgeCircleEffect.SetRadius(pRadioButton->GetRadius() * 0.4f);

		// XMFLOAT2 hcsp;
		// pRadioButton->m_transform.GetHCSPosition(&hcsp);	// 위에서 설정됨

		// m_shadedEdgeCircleEffect.SetHCSPosition(hcsp);	// 위에서 설정됨
		m_effectImmediateContext.Apply(&m_shadedEdgeCircleEffect);
		m_effectImmediateContext.Draw(TRIANGLESTRIP_QUAD_VERTEX_COUNT, 0);
	}
	
	// 2. 텍스트 렌더링
	UINT32 textLength = static_cast<UINT32>(pRadioButton->GetText().length());
	if (textLength == 0)
		return;

	IDWriteTextFormat* pDWriteTextFormat = pRadioButton->GetDWriteTextFormat();
	pDWriteTextFormat->SetTextAlignment(pRadioButton->GetTextAlignment());
	pDWriteTextFormat->SetParagraphAlignment(pRadioButton->GetParagraphAlignment());
	pBrush->SetColor(reinterpret_cast<const D2D1_COLOR_F&>(pRadioButton->GetTextColor()));

	XMFLOAT2 wcp;
	pRadioButton->m_transform.GetWinCoordPosition(&wcp);
	FLOAT textboxOffsetX = pRadioButton->GetRadius() + SPACING_BETWEEN_RADIOBUTTON_AND_TEXTBOX + pRadioButton->GetTextboxHalfSizeX();
	if (pRadioButton->IsLeftText())
		textboxOffsetX = -textboxOffsetX;

	wcp.x += textboxOffsetX;
	D2D1_RECT_F layout;
	layout.left = wcp.x - pRadioButton->GetTextboxHalfSizeX();
	layout.right = wcp.x + pRadioButton->GetTextboxHalfSizeX();
	layout.top = wcp.y - pRadioButton->GetTextboxHalfSizeY();
	layout.bottom = wcp.y + pRadioButton->GetTextboxHalfSizeY();

	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->DrawTextW(
		pRadioButton->GetText().c_str(),
		textLength,
		pDWriteTextFormat,
		&layout,
		pBrush
	);

	HRESULT hr = pD2DRenderTarget->EndDraw();
}

void Renderer::RenderDebugInfo()
{
	ID3D11Buffer* const pDebugLineVertexBuffer = Physics::GetInstance()->GetPhysicsDebugDrawer()->GetDebugLineVertexBuffer();
	const UINT debugLinesVertexCount = Physics::GetInstance()->GetPhysicsDebugDrawer()->GetDebugLineVertexCountToDraw();

	// Rasterizer

	// 래스터라이저 디스크립터에서
	// 뎁스 바이어스 및 슬로프 스케일드 뎁스 바이어스 등 설정한거 사용해야함.
	ID3D11DeviceContext* pImmContext = m_effectImmediateContext.GetDeviceContext();

	// 파이프라인 상태 업데이트
	ID3D11RasterizerState* pRSDebugLineDrawing = Physics::GetInstance()->GetPhysicsDebugDrawer()->GetRSDebugLineDrawing();
	pImmContext->RSSetState(pRSDebugLineDrawing);
	pImmContext->OMSetDepthStencilState(m_pDSSDepthReadOnly, 0);


	// 렌더링
	ID3D11Buffer* vbs[] = { pDebugLineVertexBuffer };
	UINT strides[] = { sizeof(DebugLineVertexFormat) };
	UINT offsets[] = { 0 };

	m_effectImmediateContext.IASetVertexBuffers(0, 1, vbs, strides, offsets);

	m_effectImmediateContext.Apply(&m_debugLinesEffect);
	m_effectImmediateContext.Draw(debugLinesVertexCount, 0);
}

#include <ZergEngine\CoreSystem\LowLevelRenderer\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\CameraManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\DirectionalLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\PointLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\SpotLightManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\MeshRendererManager.h>
#include <ZergEngine\CoreSystem\ComponentSystem\TerrainManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Light.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Renderer);

Renderer::Renderer()
	: m_techStandardP()
	, m_techStandardPC()
	, m_techStandardPN()
	, m_techStandardPT()
	, m_techStandardPNT()
	, m_techSkybox()
	, m_techTerrain()
{
}

Renderer::~Renderer()
{
}

void Renderer::Init(void* pDesc)
{
	HRESULT hr;
	ID3D11Device* pDevice = GraphicDevice::GetInstance().GetDeviceComInterface();
	ID3D11DeviceContext* pImmContext = GraphicDevice::GetInstance().GetImmContextComInterface();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ CREATE CONSTANT BUFFERS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	D3D11_BUFFER_DESC descBuffer;
	ZeroMemory(&descBuffer, sizeof(descBuffer));

	// 상수 버퍼 공통 속성 세팅
	descBuffer.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	descBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descBuffer.MiscFlags = 0;
	descBuffer.StructureByteStride = 0;

	do
	{
		// 1.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerFrame);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerFrame.GetAddressOf()
		);
		if (FAILED(hr))
			break;

		// 2.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerCamera);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerCamera.GetAddressOf()
		);
		if (FAILED(hr))
			break;

		// 3.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerMesh);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerMesh.GetAddressOf()
		);
		if (FAILED(hr))
			break;

		// 4.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerSubset);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerSubset.GetAddressOf()
		);
		if (FAILED(hr))
			break;

		// 5.
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerRenderResultMerge);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerRenderResultMerge.GetAddressOf()
		);
		if (FAILED(hr))
			break;
	} while (false);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Constant Buffer Creation Failed", hr);
	

	m_techStandardP.SetDeviceContext(pImmContext);
	m_techStandardP.SetConstantBuffer(m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get());

	m_techStandardPC.SetDeviceContext(pImmContext);
	m_techStandardPC.SetConstantBuffer(m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get());

	m_techStandardPN.SetDeviceContext(pImmContext);
	m_techStandardPN.SetConstantBuffer(m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get());

	m_techStandardPT.SetDeviceContext(pImmContext);
	m_techStandardPT.SetConstantBuffer(m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get());

	m_techStandardPNT.SetDeviceContext(pImmContext);
	m_techStandardPNT.SetConstantBuffer(m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get());

	m_techSkybox.SetDeviceContext(pImmContext);
	m_techSkybox.SetConstantBuffer(m_cpCbPerMesh.Get());

	m_techTerrain.SetDeviceContext(pImmContext);
	m_techTerrain.SetConstantBuffer(m_cpCbPerTerrain.Get());
}

void Renderer::Release()
{
	m_cpCbPerFrame.Reset();
	m_cpCbPerCamera.Reset();
	m_cpCbPerMesh.Reset();
	m_cpCbPerSubset.Reset();
	m_cpCbPerTerrain.Reset();
	m_cpCbPerRenderResultMerge.Reset();
}

void Renderer::RenderFrame()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	ID3D11DeviceContext* pImmContext = GraphicDevice::GetInstance().GetImmContextComInterface();
	const RASTERIZER_FILL_MODE rfm = RASTERIZER_FILL_MODE::SOLID;
	const RASTERIZER_CULL_MODE rcm = RASTERIZER_CULL_MODE::BACK;
	const TEXTURE_FILTERING_OPTION tfo = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;

	// PerFrame 상수버퍼 업데이트 및 바인딩
	{
		ConstantBuffer::PerFrame cbPerFrame;

		const uint32_t dlCount = static_cast<uint32_t>(DirectionalLightManager::GetInstance().m_activeComponents.size());
		const uint32_t plCount = static_cast<uint32_t>(PointLightManager::GetInstance().m_activeComponents.size());
		const uint32_t slCount = static_cast<uint32_t>(SpotLightManager::GetInstance().m_activeComponents.size());
		cbPerFrame.cbpf_dlCount = dlCount <= MAX_GLOBAL_LIGHT_COUNT ? dlCount : MAX_GLOBAL_LIGHT_COUNT;
		cbPerFrame.cbpf_plCount = plCount <= MAX_GLOBAL_LIGHT_COUNT ? plCount : MAX_GLOBAL_LIGHT_COUNT;
		cbPerFrame.cbpf_slCount = slCount <= MAX_GLOBAL_LIGHT_COUNT ? slCount : MAX_GLOBAL_LIGHT_COUNT;

		assert(
			dlCount <= MAX_GLOBAL_LIGHT_COUNT &&
			plCount <= MAX_GLOBAL_LIGHT_COUNT &&
			slCount <= MAX_GLOBAL_LIGHT_COUNT
		);

		for (uint32_t i = 0; i < dlCount; ++i)
		{
			const DirectionalLight* pDL = static_cast<DirectionalLight*>(DirectionalLightManager::GetInstance().m_activeComponents[i]);
			const GameObject* pGameObject = pDL->GetGameObjectHandle().ToPtr();

			cbPerFrame.cbpf_dl[i].diffuse = pDL->m_diffuse;
			cbPerFrame.cbpf_dl[i].ambient = pDL->m_ambient;
			cbPerFrame.cbpf_dl[i].specular = pDL->m_specular;
			XMStoreFloat3(
				&cbPerFrame.cbpf_dl[i].directionW,
				XMVector3Rotate(
					LIGHT_DIRECTION_LOCAL_AXIS,
					XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&pGameObject->GetTransform().m_rotation))
				)
			);
		}

		for (uint32_t i = 0; i < plCount; ++i)
		{
			const PointLight* pPL = static_cast<PointLight*>(PointLightManager::GetInstance().m_activeComponents[i]);
			const GameObject* pGameObject = pPL->GetGameObjectHandle().ToPtr();

			cbPerFrame.cbpf_pl[i].diffuse = pPL->m_diffuse;
			cbPerFrame.cbpf_pl[i].ambient = pPL->m_ambient;
			cbPerFrame.cbpf_pl[i].specular = pPL->m_specular;
			cbPerFrame.cbpf_pl[i].positionW = pGameObject->GetTransform().m_position;
			cbPerFrame.cbpf_pl[i].range = pPL->m_range;
			cbPerFrame.cbpf_pl[i].att = pPL->m_att;
		}

		for (uint32_t i = 0; i < slCount; ++i)
		{
			const SpotLight* pSL = static_cast<SpotLight*>(SpotLightManager::GetInstance().m_activeComponents[i]);
			const GameObject* pGameObject = pSL->GetGameObjectHandle().ToPtr();

			cbPerFrame.cbpf_sl[i].diffuse = pSL->m_diffuse;
			cbPerFrame.cbpf_sl[i].ambient = pSL->m_ambient;
			cbPerFrame.cbpf_sl[i].specular = pSL->m_specular;
			cbPerFrame.cbpf_sl[i].positionW = pGameObject->GetTransform().m_position;
			cbPerFrame.cbpf_sl[i].range = pSL->m_range;
			XMStoreFloat3(
				&cbPerFrame.cbpf_sl[i].directionW,
				XMVector3Rotate(LIGHT_DIRECTION_LOCAL_AXIS,	XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&pGameObject->GetTransform().m_rotation)))
			);
			cbPerFrame.cbpf_sl[i].spotExp = pSL->m_spotExp;
			cbPerFrame.cbpf_sl[i].att = pSL->m_att;
		}
		
		hr = pImmContext->Map(
			m_cpCbPerFrame.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			return;
		memcpy(mapped.pData, &cbPerFrame, sizeof(cbPerFrame));
		pImmContext->Unmap(m_cpCbPerFrame.Get(), 0);

		ID3D11Buffer* const cbs[] = { m_cpCbPerFrame.Get() };
		// PerFrame 상수버퍼는 픽셀 셰이더에서만 참조
		pImmContext->PSSetConstantBuffers(ConstantBuffer::PerFrame::GetSlotNumber(), _countof(cbs), cbs);
	}

	// Camera마다 프레임 렌더링
	for (IComponent* pComponent : CameraManager::GetInstance().m_activeComponents)
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

		const GameObject* pCameraOwner = pCamera->GetGameObjectHandle().ToPtr();
		assert(pCameraOwner != nullptr);

		pCamera->UpdateViewMatrix();
		// pCamera->UpdateProjectionMatrix(); // 굳이 매 프레임 해줄 필요는 없음
		const XMMATRIX vp = XMLoadFloat4x4A(&pCamera->m_viewMatrix) * XMLoadFloat4x4A(&pCamera->m_projMatrix);
		m_techStandardP.SetViewProjMatrix(vp);
		m_techStandardPC.SetViewProjMatrix(vp);
		m_techStandardPN.SetViewProjMatrix(vp);
		m_techStandardPT.SetViewProjMatrix(vp);
		m_techStandardPNT.SetViewProjMatrix(vp);

		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		const D3D11_VIEWPORT* pViewport = &pCamera->m_fullbufferViewport;

		// 컬러 버퍼 및 뎁스스텐실 버퍼 초기화
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// PerCamera 상수버퍼 업데이트 및 바인딩
		{
			XMFLOAT4A frustumPlanes[6];
			Math::ExtractFrustumPlanesInWorldSpace(vp, frustumPlanes);	// Map 시간 줄이기 위해 미리 계산

			ConstantBuffer::PerCamera cbPerCamera;
			cbPerCamera.cbpc_cameraPosW = pCameraOwner->GetTransform().m_position;
			cbPerCamera.cbpc_tessMinDist = pCamera->GetMinimumDistanceForTessellationToStart();
			cbPerCamera.cbpc_tessMaxDist = pCamera->GetMaximumDistanceForTessellationToStart();
			cbPerCamera.cbpc_minTessExponent = pCamera->GetMinimumTessellationExponent();
			cbPerCamera.cbpc_maxTessExponent = pCamera->GetMaximumTessellationExponent();
			cbPerCamera.cbpc_frustumPlane[0] = frustumPlanes[0];
			cbPerCamera.cbpc_frustumPlane[1] = frustumPlanes[1];
			cbPerCamera.cbpc_frustumPlane[2] = frustumPlanes[2];
			cbPerCamera.cbpc_frustumPlane[3] = frustumPlanes[3];
			cbPerCamera.cbpc_frustumPlane[4] = frustumPlanes[4];
			cbPerCamera.cbpc_frustumPlane[5] = frustumPlanes[5];

			hr = pImmContext->Map(
				m_cpCbPerCamera.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mapped
			);
			if (FAILED(hr))
				continue;
			memcpy(mapped.pData, &cbPerCamera, sizeof(cbPerCamera));
			pImmContext->Unmap(m_cpCbPerCamera.Get(), 0);

			ID3D11Buffer* const cbs[] = { m_cpCbPerCamera.Get() };
			pImmContext->HSSetConstantBuffers(ConstantBuffer::PerCamera::GetSlotNumber(), _countof(cbs), cbs);
			pImmContext->PSSetConstantBuffers(ConstantBuffer::PerCamera::GetSlotNumber(), _countof(cbs), cbs);
		}

		for (const IComponent* pComponent : MeshRendererManager::GetInstance().m_activeComponents)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);

			if (!pMeshRenderer->IsEnabled() || pMeshRenderer->m_mesh == nullptr)
				continue;

			switch (pMeshRenderer->m_mesh->GetVertexFormatType())
			{
			case VERTEX_FORMAT_TYPE::POSITION:
				m_techStandardP.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
				m_techStandardP.Render(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_COLOR:
				m_techStandardPC.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
				m_techStandardPC.Render(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
				m_techStandardPN.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
				m_techStandardPN.Render(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
				m_techStandardPT.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
				m_techStandardPT.Render(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
				m_techStandardPNT.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
				m_techStandardPNT.Render(pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::COUNT:
				__fallthrough;
			case VERTEX_FORMAT_TYPE::UNKNOWN:
				Debug::ForceCrashWithMessageBox(L"Renderer error", L"Unknown vertex format type.");
				break;
			}
		}

		// 지형 렌더링
		for (const IComponent* pComponent : TerrainManager::GetInstance().m_activeComponents)
		{
			const Terrain* pTerrain = static_cast<const Terrain*>(pComponent);

			m_techTerrain.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, rfm, rcm, tfo);
			m_techTerrain.SetViewProjMatrix(vp);
			m_techTerrain.Render(pTerrain);
		}

		// 스카이박스 렌더링
		const Skybox* pSkybox = Environment::GetInstance().m_spSkybox.get();
		if (pSkybox)
		{
			const XMVECTOR cameraPos = XMLoadFloat3A(&pCameraOwner->GetTransform().m_position);
			m_techSkybox.Apply(pViewport, pColorBufferRTV, pDepthStencilBufferDSV, tfo);
			m_techSkybox.SetViewProjMatrix(vp);
			m_techSkybox.Render(cameraPos, pSkybox);
		}
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 카메라 렌더링 결과 병합
	// 인풋 레이아웃 필요없음
	// 정점 입력 필요없음
	ID3D11RenderTargetView* pSwapChainRTV = GraphicDevice::GetInstance().GetSwapChainRTVComInterface();
	ID3D11DepthStencilView* pSwapChainDSV = GraphicDevice::GetInstance().GetSwapChainDSVComInterface();
	pImmContext->ClearRenderTargetView(pSwapChainRTV, DirectX::Colors::Black);
	pImmContext->ClearDepthStencilView(pSwapChainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	pImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// TRIANGLESTRIP

	pImmContext->OMSetDepthStencilState(GraphicDevice::GetInstance().GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::RENDER_RESULT_MERGE), 0);
	ID3D11RenderTargetView* const rtvs[] = { pSwapChainRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pSwapChainDSV);

	pImmContext->RSSetViewports(1, &Window::GetInstance().GetFullClientViewport());	// 전체 백버퍼에 대한 뷰포트 설정

	// 셰이더 설정
	pImmContext->CSSetShader(nullptr, nullptr, 0);
	pImmContext->VSSetShader(GraphicDevice::GetInstance().GetVSComInterface(VERTEX_SHADER_TYPE::RENDER_RESULT_MERGE), nullptr, 0);
	pImmContext->HSSetShader(nullptr, nullptr, 0);
	pImmContext->DSSetShader(nullptr, nullptr, 0);
	pImmContext->GSSetShader(nullptr, nullptr, 0);
	// ┌─카메라 컴포넌트 설정에 따라서 픽셀셰이더 달라져야 함! (MSAA 쓰는 카메라인지 아닌지)
	pImmContext->PSSetShader(GraphicDevice::GetInstance().GetPSComInterface(PIXEL_SHADER_TYPE::MSRENDER_RESULT_MERGE), nullptr, 0);
	for (const IComponent* pComponent : CameraManager::GetInstance().m_activeComponents)
	{
		const Camera* pCamera = static_cast<const Camera*>(pComponent);
		if (!pCamera->IsEnabled())
			continue;

		// PerRenderResultMerge 상수버퍼 업데이트 및 바인딩
		ConstantBuffer::PerRenderResultMerge cbPerRenderResultMerge;

		cbPerRenderResultMerge.cbprrm_width = pCamera->m_nzdVp.w;
		cbPerRenderResultMerge.cbprrm_height = pCamera->m_nzdVp.h;
		cbPerRenderResultMerge.cbprrm_topLeftX = pCamera->m_nzdVp.x;
		cbPerRenderResultMerge.cbprrm_topLeftY = pCamera->m_nzdVp.y;

		hr = pImmContext->Map(
			m_cpCbPerRenderResultMerge.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerRenderResultMerge, sizeof(cbPerRenderResultMerge));
		pImmContext->Unmap(m_cpCbPerRenderResultMerge.Get(), 0);

		ID3D11Buffer* const cbs[] = { m_cpCbPerRenderResultMerge.Get() };
		pImmContext->VSSetConstantBuffers(ConstantBuffer::PerRenderResultMerge::GetSlotNumber(), _countof(cbs), cbs);

		ID3D11ShaderResourceView* const srvs[] = { pCamera->GetColorBufferSRVComInterface()};
		pImmContext->PSSetShaderResources(ShaderResourceSlot::RenderResultSlot::GetSlotNumber(), _countof(srvs), srvs);

		pImmContext->Draw(4, 0);
	}
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	GraphicDevice::GetInstance().GetSwapChainComInterface()->Present(0, 0);
}

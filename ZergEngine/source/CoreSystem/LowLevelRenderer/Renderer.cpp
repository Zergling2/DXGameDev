#include <ZergEngine\CoreSystem\LowLevelRenderer\Renderer.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\EnvironmentManager.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\Resource\Skybox.h>
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

namespace ze
{
	RendererImpl Renderer;
}

using namespace ze;

RendererImpl::RendererImpl()
	: m_pCurrentPass(nullptr)
	, m_passStandardP()
	, m_passStandardPC()
	, m_passStandardPN()
	, m_passStandardPT()
	, m_passStandardPNT()
	, m_passSkybox()
	, m_passTerrain()
	, m_passMSCameraMerge()
{
}

RendererImpl::~RendererImpl()
{
}

void RendererImpl::Init(void* pDesc)
{
	HRESULT hr;
	ID3D11Device* pDevice = GraphicDevice.GetDeviceComInterface();
	ID3D11DeviceContext* pImmContext = GraphicDevice.GetImmContextComInterface();

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收 CREATE CONSTANT BUFFERS 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	D3D11_BUFFER_DESC descBuffer;
	ZeroMemory(&descBuffer, sizeof(descBuffer));

	// 鼻熱 幗ぷ 奢鱔 樓撩 撮た
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
		descBuffer.ByteWidth = sizeof(ConstantBuffer::PerCameraMerge);
		hr = pDevice->CreateBuffer(
			&descBuffer,
			nullptr,
			m_cpCbPerCameraMerge.GetAddressOf()
		);
		if (FAILED(hr))
			break;
	} while (false);

	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Constant Buffer Creation Failed", hr);

	m_passStandardP.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_passStandardP.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION));
	m_passStandardP.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_P));
	m_passStandardP.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_P));
	m_passStandardP.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passStandardP.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passStandardPC.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_passStandardPC.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_COLOR));
	m_passStandardPC.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PC));
	m_passStandardPC.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_PC));
	m_passStandardPC.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passStandardPC.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passStandardPN.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_passStandardPN.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_NORMAL));
	m_passStandardPN.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PN));
	m_passStandardPN.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_PN));
	m_passStandardPN.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passStandardPN.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passStandardPT.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_passStandardPT.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD));
	m_passStandardPT.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PT));
	m_passStandardPT.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_PT));
	m_passStandardPT.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passStandardPT.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passStandardPNT.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_passStandardPNT.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD));
	m_passStandardPNT.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::STANDARD_TRANSFORM_PNT));
	m_passStandardPNT.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::STANDARD_COLORING_PNT));
	m_passStandardPNT.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passStandardPNT.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passSkybox.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// m_passSkybox.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD));	// в蹂 橈擠
	m_passSkybox.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::SKYBOX_TRANSFORM));
	m_passSkybox.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::SKYBOX_COLORING));
	m_passSkybox.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passSkybox.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::SKYBOX), 0);

	m_passTerrain.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_passTerrain.SetInputLayout(GraphicDevice.GetILComInterface(VERTEX_FORMAT_TYPE::TERRAIN_CONTROL_POINT));
	m_passTerrain.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::TERRAIN_TRANSFORM));
	m_passTerrain.SetHullShader(GraphicDevice.GetHSComInterface(HULL_SHADER_TYPE::TERRAIN_RENDERING));
	m_passTerrain.SetDomainShader(GraphicDevice.GetDSComInterface(DOMAIN_SHADER_TYPE::TERRAIN_RENDERING));
	m_passTerrain.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::TERRAIN_COLORING));
	m_passTerrain.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passTerrain.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::STANDARD), 0);

	m_passMSCameraMerge.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// Triangle strip
	// m_passMSCameraMerge.SetInputLayout(nullptr);
	m_passMSCameraMerge.SetVertexShader(GraphicDevice.GetVSComInterface(VERTEX_SHADER_TYPE::CAMERA_MERGE));
	m_passMSCameraMerge.SetPixelShader(GraphicDevice.GetPSComInterface(PIXEL_SHADER_TYPE::MSCAMERA_MERGE));
	m_passMSCameraMerge.SetRasterizerState(GraphicDevice.GetRSComInterface(RASTERIZER_FILL_MODE::SOLID, RASTERIZER_CULL_MODE::BACK));
	m_passMSCameraMerge.SetDepthStencilState(GraphicDevice.GetDSSComInterface(DEPTH_STENCIL_STATE_TYPE::CAMERA_MERGE), 0);

	// SamplerState 撲薑
	TEXTURE_FILTERING_OPTION tfo = TEXTURE_FILTERING_OPTION::ANISOTROPIC_4X;
	ID3D11SamplerState* const ss[] =
	{
		GraphicDevice.GetSSComInterface(tfo),			// (s0) mesh texture sampler
		GraphicDevice.GetSkyboxSamplerComInterface(),	// (s1)
		GraphicDevice.GetSSComInterface(tfo),			// (s2) terrain color texture sampler
		GraphicDevice.GetHeightmapSamplerComInterface()	// (s3)
	};
	pImmContext->VSSetSamplers(0, _countof(ss), ss);
	pImmContext->DSSetSamplers(0, _countof(ss), ss);
	pImmContext->PSSetSamplers(0, _countof(ss), ss);
}

void RendererImpl::Release()
{
	m_cpCbPerFrame.Reset();
	m_cpCbPerCamera.Reset();
	m_cpCbPerMesh.Reset();
	m_cpCbPerSubset.Reset();
	m_cpCbPerTerrain.Reset();
	m_cpCbPerCameraMerge.Reset();
}

void RendererImpl::RenderFrame()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	ID3D11DeviceContext* pImmContext = GraphicDevice.GetImmContextComInterface();

	// PerFrame 鼻熱幗ぷ 機等檜お 塽 夥檣註
	{
		ConstantBuffer::PerFrame cbPerFrame;

		const uint32_t dlCount = static_cast<uint32_t>(DirectionalLightManager.m_activeComponents.size());
		const uint32_t plCount = static_cast<uint32_t>(PointLightManager.m_activeComponents.size());
		const uint32_t slCount = static_cast<uint32_t>(SpotLightManager.m_activeComponents.size());
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
			const DirectionalLight* pDL = static_cast<DirectionalLight*>(DirectionalLightManager.m_activeComponents[i]);
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
			const PointLight* pPL = static_cast<PointLight*>(PointLightManager.m_activeComponents[i]);
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
			const SpotLight* pSL = static_cast<SpotLight*>(SpotLightManager.m_activeComponents[i]);
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
		// PerFrame 鼻熱幗ぷ朝 а撚 樁檜渦縑憮虜 霤褻
		pImmContext->PSSetConstantBuffers(ConstantBuffer::PerFrame::GetSlotNumber(), _countof(cbs), cbs);
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

		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		// 箔んお 夥檣註
		pImmContext->RSSetViewports(1, &pCamera->m_fullbufferViewport);
		// 鏽楝 幗ぷ 塽 答蝶蝶蘸褒 幗ぷ 夥檣註
		ID3D11RenderTargetView* pColorBufferRTV = pCamera->m_cpColorBufferRTV.Get();
		ID3D11DepthStencilView* pDepthStencilBufferDSV = pCamera->m_cpDepthStencilBufferDSV.Get();
		pImmContext->ClearRenderTargetView(pColorBufferRTV, reinterpret_cast<const FLOAT*>(&pCamera->GetBackgroundColor()));
		pImmContext->ClearDepthStencilView(pDepthStencilBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* const rtvs[] = { pColorBufferRTV };
		pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pDepthStencilBufferDSV);
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

		const GameObject* pCameraOwner = pCamera->GetGameObjectHandle().ToPtr();
		assert(pCameraOwner != nullptr);

		pCamera->UpdateViewMatrix();	// 箔 滲�� ч溺 機等檜お
		// pCamera->UpdateProjectionMatrix(); // 衙 Щ溯歜 п還 в蹂朝 橈擠
		XMFLOAT4X4A vp;
		XMStoreFloat4x4A(&vp, XMLoadFloat4x4A(&pCamera->m_viewMatrix)* XMLoadFloat4x4A(&pCamera->m_projMatrix));

		// PerCamera 鼻熱幗ぷ 機等檜お 塽 夥檣註
		{
			XMFLOAT4A frustumPlanes[6];
			Math::ExtractFrustumPlanesInWorldSpace(&vp, frustumPlanes);	// Map 衛除 還檜晦 嬪п 嘐葬 啗骯

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

		for (const IComponent* pComponent : MeshRendererManager.m_activeComponents)
		{
			const MeshRenderer* pMeshRenderer = static_cast<const MeshRenderer*>(pComponent);
			if (!pMeshRenderer->IsEnabled() || pMeshRenderer->m_mesh == nullptr)
				continue;

			switch (pMeshRenderer->m_mesh->GetVertexFormatType())
			{
			case VERTEX_FORMAT_TYPE::POSITION:
				RenderPMesh(pImmContext, m_cpCbPerMesh.Get(), &vp, pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_COLOR:
				RenderPCMesh(pImmContext, m_cpCbPerMesh.Get(), &vp, pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
				RenderPNMesh(pImmContext, m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get(), &vp, pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
				RenderPTMesh(pImmContext, m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get(), &vp, pMeshRenderer);
				break;
			case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
				RenderPNTMesh(pImmContext, m_cpCbPerMesh.Get(), m_cpCbPerSubset.Get(), &vp, pMeshRenderer);
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
			m_passTerrain.Apply(pImmContext);
			m_pCurrentPass = &m_passTerrain;
			RenderTerrain(pImmContext, pTerrain);
		}

		// 蝶蘋檜夢蝶 溶渦葭
		const Skybox* pSkybox = Environment.m_spSkybox.get();
		if (pSkybox)
		{
			m_passSkybox.Apply(pImmContext);
			m_pCurrentPass = &m_passSkybox;
			RenderSkybox(pImmContext, m_cpCbPerMesh.Get(), &vp, &pCameraOwner->GetTransform().m_position, pSkybox);
		}
	}

	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 蘋詭塭 溶渦葭 唸婁 煽м
	m_passMSCameraMerge.Apply(pImmContext);
	m_pCurrentPass = &m_passMSCameraMerge;

	// 瞪羹 寥幗ぷ縑 渠и 箔んお 撲薑
	pImmContext->RSSetViewports(1, &Window.GetFullClientViewport());
	// 溶渦顫啃 夥檣註
	ID3D11RenderTargetView* pSwapChainRTV = GraphicDevice.GetSwapChainRTVComInterface();
	ID3D11DepthStencilView* pSwapChainDSV = GraphicDevice.GetSwapChainDSVComInterface();
	pImmContext->ClearRenderTargetView(pSwapChainRTV, DirectX::Colors::Blue);
	pImmContext->ClearDepthStencilView(pSwapChainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* const rtvs[] = { pSwapChainRTV };
	pImmContext->OMSetRenderTargets(_countof(rtvs), rtvs, pSwapChainDSV);

	for (const IComponent* pComponent : CameraManager.m_activeComponents)
	{
		const Camera* pCamera = static_cast<const Camera*>(pComponent);
		if (!pCamera->IsEnabled())
			continue;

		// PerMSCameraMerge 鼻熱幗ぷ 機等檜お 塽 夥檣註
		ConstantBuffer::PerMSCameraMerge cbPerCameraMerge;
		cbPerCameraMerge.cbpcm_width = pCamera->m_nzdVp.w;
		cbPerCameraMerge.cbpcm_height = pCamera->m_nzdVp.h;
		cbPerCameraMerge.cbpcm_topLeftX = pCamera->m_nzdVp.x;
		cbPerCameraMerge.cbpcm_topLeftY = pCamera->m_nzdVp.y;

		hr = pImmContext->Map(
			m_cpCbPerCameraMerge.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerCameraMerge, sizeof(cbPerCameraMerge));
		pImmContext->Unmap(m_cpCbPerCameraMerge.Get(), 0);

		ID3D11Buffer* const cbs[] = { m_cpCbPerCameraMerge.Get() };
		pImmContext->VSSetConstantBuffers(ConstantBuffer::PerCameraMerge::GetSlotNumber(), _countof(cbs), cbs);

		ID3D11ShaderResourceView* const srvs[] = { pCamera->GetColorBufferSRVComInterface()};
		pImmContext->PSSetShaderResources(ShaderResourceSlot::RenderResultSlot::GetSlotNumber(), _countof(srvs), srvs);

		pImmContext->Draw(4, 0);

		ID3D11ShaderResourceView* const clrsrvs[] = { nullptr };
		pImmContext->PSSetShaderResources(ShaderResourceSlot::RenderResultSlot::GetSlotNumber(), _countof(clrsrvs), clrsrvs);
	}
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收

	GraphicDevice.GetSwapChainComInterface()->Present(0, 0);
}

void RendererImpl::RenderPMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh,
	const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION);
	assert(pDeviceContext != nullptr && pMeshRenderer != nullptr);

	if (m_pCurrentPass != &m_passStandardP)
	{
		m_passStandardP.Apply(pDeviceContext);
		m_pCurrentPass = &m_passStandardP;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::Position) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	pDeviceContext->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	pDeviceContext->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 鼻熱幗ぷ 機等檜お 塽 夥檣註
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	ConstantBuffer::PerMesh cbPerMesh;

	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 牖憮 �蛻�
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));

	hr = pDeviceContext->Map(
		pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	pDeviceContext->Unmap(pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { pCbPerMesh };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const auto& subset : pMesh->m_subsets)
		pDeviceContext->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
}

void RendererImpl::RenderPCMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh,
	const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_COLOR);
	assert(pDeviceContext != nullptr && pMeshRenderer != nullptr);

	if (m_pCurrentPass != &m_passStandardPC)
	{
		m_passStandardPC.Apply(pDeviceContext);
		m_pCurrentPass = &m_passStandardPC;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::PositionColor) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	pDeviceContext->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	pDeviceContext->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 鼻熱幗ぷ 機等檜お 塽 夥檣註
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	ConstantBuffer::PerMesh cbPerMesh;

	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 牖憮 �蛻�
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));
	hr = pDeviceContext->Map(
		pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	pDeviceContext->Unmap(pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { pCbPerMesh };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const auto& subset : pMesh->m_subsets)
		pDeviceContext->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
}

void RendererImpl::RenderPNMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
	const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL);
	assert(pDeviceContext != nullptr && pMeshRenderer != nullptr);

	if (m_pCurrentPass != &m_passStandardPN)
	{
		m_passStandardPN.Apply(pDeviceContext);
		m_pCurrentPass = &m_passStandardPN;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::PositionNormal) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	pDeviceContext->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	pDeviceContext->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 鼻熱幗ぷ 機等檜お 塽 夥檣註
	ConstantBuffer::PerMesh cbPerMesh;
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 牖憮 �蛻�
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));


	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));
	hr = pDeviceContext->Map(
		pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	pDeviceContext->Unmap(pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { pCbPerMesh };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const auto& subset : pMesh->m_subsets)
	{
		// PerSubset 鼻熱幗ぷ 機等檜お 塽 夥檣註
		ConstantBuffer::PerSubset cbPerSubset;
		const Material* pMaterial = subset.m_material.get();

		cbPerSubset.cbps_subsetMtl.Initialize();
		if (pMaterial != nullptr)
		{
			cbPerSubset.cbps_subsetMtl.UseMaterial();
			cbPerSubset.cbps_subsetMtl.ambient = pMaterial->m_ambient;
			cbPerSubset.cbps_subsetMtl.diffuse = pMaterial->m_diffuse;
			cbPerSubset.cbps_subsetMtl.specular = pMaterial->m_specular;
		}
		hr = pDeviceContext->Map(
			pCbPerSubset,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerSubset, sizeof(cbPerSubset));
		pDeviceContext->Unmap(pCbPerSubset, 0);

		ID3D11Buffer* const cbs[] = { pCbPerSubset };
		pDeviceContext->PSSetConstantBuffers(ConstantBuffer::PerSubset::GetSlotNumber(), _countof(cbs), cbs);

		// 萄煎辦
		pDeviceContext->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderPTMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
	const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);
	assert(pDeviceContext != nullptr && pMeshRenderer != nullptr);

	if (m_pCurrentPass != &m_passStandardPT)
	{
		m_passStandardPT.Apply(pDeviceContext);
		m_pCurrentPass = &m_passStandardPT;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::PositionTexCoord) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	pDeviceContext->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	pDeviceContext->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 鼻熱幗ぷ 機等檜お 塽 夥檣註
	ConstantBuffer::PerMesh cbPerMesh;
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 牖憮 �蛻�
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));
	hr = pDeviceContext->Map(
		pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	pDeviceContext->Unmap(pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { pCbPerMesh };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const auto& subset : pMesh->m_subsets)
	{
		// PerSubset 鼻熱幗ぷ 機等檜お 塽 夥檣註
		ConstantBuffer::PerSubset cbPerSubset;
		const Material* pMaterial = subset.m_material.get();

		cbPerSubset.cbps_subsetMtl.Initialize();
		if (pMaterial != nullptr)
		{
			cbPerSubset.cbps_subsetMtl.UseMaterial();
			cbPerSubset.cbps_subsetMtl.ambient = pMaterial->m_ambient;
			cbPerSubset.cbps_subsetMtl.diffuse = pMaterial->m_diffuse;
			cbPerSubset.cbps_subsetMtl.specular = pMaterial->m_specular;
			if (pMaterial->m_baseMap)
				cbPerSubset.cbps_subsetMtl.UseBaseMap();
		}
		hr = pDeviceContext->Map(
			pCbPerSubset,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerSubset, sizeof(cbPerSubset));
		pDeviceContext->Unmap(pCbPerSubset, 0);

		ID3D11Buffer* const cbs[] = { pCbPerSubset };
		pDeviceContext->PSSetConstantBuffers(ConstantBuffer::PerSubset::GetSlotNumber(), _countof(cbs), cbs);

		// 樁檜渦 葬模蝶 箔 撲薑
		if (pMaterial != nullptr && pMaterial->m_baseMap)
		{
			ID3D11ShaderResourceView* const srvs[] = { subset.m_material->m_baseMap->GetSRVComInterface() };
			pDeviceContext->PSSetShaderResources(ShaderResourceSlot::BasemapSlot::GetSlotNumber(), _countof(srvs), srvs);
		}

		// 萄煎辦
		pDeviceContext->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderPNTMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
	const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer)
{
	const Mesh* pMesh = pMeshRenderer->m_mesh.get();
	if (!pMesh)
		return;

	assert(pMesh->GetVertexFormatType() == VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD);
	assert(pDeviceContext != nullptr && pMeshRenderer != nullptr);

	if (m_pCurrentPass != &m_passStandardPNT)
	{
		m_passStandardPNT.Apply(pDeviceContext);
		m_pCurrentPass = &m_passStandardPNT;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::PositionNormalTexCoord) };
	const UINT offset[] = { 0 };
	ID3D11Buffer* const vbs[] = { pMesh->GetVBComInterface() };
	pDeviceContext->IASetVertexBuffers(0, _countof(vbs), vbs, stride, offset);

	// 檣策蝶 幗ぷ 撲薑
	pDeviceContext->IASetIndexBuffer(pMesh->GetIBComInterface(), DXGI_FORMAT_R32_UINT, 0);

	// PerMesh 鼻熱幗ぷ 機等檜お 塽 夥檣註
	ConstantBuffer::PerMesh cbPerMesh;
	const GameObject* pGameObject = pMeshRenderer->GetGameObjectHandle().ToPtr();
	const Transform& transform = pGameObject->GetTransform();
	const XMMATRIX w = XMMatrixScalingFromVector(XMLoadFloat3A(&transform.m_scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4A(&transform.m_rotation)) *	// Yaw(y) Pitch(x) Roll(z) 牖憮 �蛻�
		XMMatrixTranslationFromVector(XMLoadFloat3A(&transform.m_position));

	XMStoreFloat4x4A(&cbPerMesh.cbpm_w, ConvertToHLSLMatrix(w));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(nullptr, w)));
	XMStoreFloat4x4A(&cbPerMesh.cbpm_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));

	hr = pDeviceContext->Map(
		pCbPerMesh,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerMesh, sizeof(cbPerMesh));
	pDeviceContext->Unmap(pCbPerMesh, 0);

	ID3D11Buffer* cbs[] = { pCbPerMesh };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerMesh::GetSlotNumber(), _countof(cbs), cbs);

	// 憮粽撢菟 牖�裔牉� 溶渦葭
	for (const auto& subset : pMesh->m_subsets)
	{
		// PerSubset 鼻熱幗ぷ 機等檜お 塽 夥檣註
		ConstantBuffer::PerSubset cbPerSubset;
		const Material* pMaterial = subset.m_material.get();

		cbPerSubset.cbps_subsetMtl.Initialize();
		if (pMaterial != nullptr)
		{
			cbPerSubset.cbps_subsetMtl.UseMaterial();
			cbPerSubset.cbps_subsetMtl.ambient = pMaterial->m_ambient;
			cbPerSubset.cbps_subsetMtl.diffuse = pMaterial->m_diffuse;
			cbPerSubset.cbps_subsetMtl.specular = pMaterial->m_specular;
			if (pMaterial->m_baseMap)
				cbPerSubset.cbps_subsetMtl.UseBaseMap();
		}
		hr = pDeviceContext->Map(
			pCbPerSubset,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		);
		if (FAILED(hr))
			continue;
		memcpy(mapped.pData, &cbPerSubset, sizeof(cbPerSubset));
		pDeviceContext->Unmap(pCbPerSubset, 0);

		ID3D11Buffer* const cbs[] = { pCbPerSubset };
		pDeviceContext->PSSetConstantBuffers(ConstantBuffer::PerSubset::GetSlotNumber(), _countof(cbs), cbs);

		// 樁檜渦 葬模蝶 箔 撲薑
		if (pMaterial != nullptr && pMaterial->m_baseMap)
		{
			ID3D11ShaderResourceView* const srvs[] = { subset.m_material->m_baseMap->GetSRVComInterface() };
			pDeviceContext->PSSetShaderResources(ShaderResourceSlot::BasemapSlot::GetSlotNumber(), _countof(srvs), srvs);
		}
		/*
		else	// D3D11 ERROR 螃盟 詭衛雖 寞雖
		{
			ID3D11ShaderResourceView* srvs[] = { nullptr };
			pDeviceContext->PSSetShaderResources(ShaderResourceSlot::BasemapSlot::GetSlotNumber(), 1, srvs);
		}
		*/

		// 萄煎辦
		pDeviceContext->DrawIndexed(subset.GetIndexCount(), subset.GetStartIndexLocation(), 0);
	}
}

void RendererImpl::RenderTerrain(ID3D11DeviceContext* pDeviceContext, const Terrain* pTerrain)
{
	assert(pDeviceContext != nullptr && pTerrain != nullptr);

	/*
	assert(m_pCbPerTerrain != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// 幗臢蝶 幗ぷ 撲薑
	const UINT stride[] = { sizeof(VertexFormat::TerrainControlPoint) };
	const UINT offset[] = { 0 };
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
	cbPerTerrain.cbpt_terrainTextureTiling = pTerrain->GetTextureScale();
	cbPerTerrain.cbpt_terrainCellSpacing = pTerrain->GetCellSpacing();
	cbPerTerrain.cbpt_terrainTexelSpacingU = pTerrain->GetTexelSpacingU();
	cbPerTerrain.cbpt_terrainTexelSpacingV = pTerrain->GetTexelSpacingV();
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
	*/
}

void RendererImpl::RenderSkybox(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerSkybox,
	const XMFLOAT4X4A* pViewProjMatrix, const XMFLOAT3A* pCameraPos, const Skybox* pSkybox)
{
	assert(pDeviceContext != nullptr && pSkybox != nullptr);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;

	// PerSkybox 鼻熱幗ぷ 機等檜お 塽 夥檣註
	ConstantBuffer::PerSkybox cbPerSkybox;

	const XMMATRIX w = XMMatrixTranslationFromVector(XMLoadFloat3A(pCameraPos));	// 蝶蘋檜夢蝶蒂 о鼻 蘋詭塭 嬪纂煎 檜翕
	// XMStoreFloat4x4A(&cbPerSkybox.cbpsky_w, ConvertToHLSLMatrix(w));				// 樁檜渦縑憮 嘐餌辨
	// XMStoreFloat4x4A(&cbPerSkybox.cbpsky_wInvTr, TransposeAndConvertToHLSLMatrix(XMMatrixInverse(&det, w)));	// 樁檜渦縑憮 嘐餌辨
	XMStoreFloat4x4A(&cbPerSkybox.cbpsky_wvp, ConvertToHLSLMatrix(w * XMLoadFloat4x4A(pViewProjMatrix)));

	hr = pDeviceContext->Map(
		pCbPerSkybox,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);
	if (FAILED(hr))
		return;
	memcpy(mapped.pData, &cbPerSkybox, sizeof(cbPerSkybox));
	pDeviceContext->Unmap(pCbPerSkybox, 0);

	ID3D11Buffer* const cbs[] = { pCbPerSkybox };
	pDeviceContext->VSSetConstantBuffers(ConstantBuffer::PerSkybox::GetSlotNumber(), _countof(cbs), cbs);

	// 樁檜渦 葬模蝶 撲薑
	ID3D11ShaderResourceView* const srvs[] = { pSkybox->GetSRVComInterface() };
	pDeviceContext->PSSetShaderResources(ShaderResourceSlot::SkyboxCubemapSlot::GetSlotNumber(), _countof(srvs), srvs);

	// 萄煎辦
	pDeviceContext->Draw(36, 0);
}

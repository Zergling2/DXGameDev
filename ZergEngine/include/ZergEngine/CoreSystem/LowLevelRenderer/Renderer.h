#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\Pass.h>

namespace ze
{
	class MeshRenderer;
	class Terrain;
	class Skybox;

	class RendererImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(RendererImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void RenderFrame();

		void RenderPMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, 
			const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer);
		void RenderPCMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh,
			const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer);
		void RenderPNMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
			const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer);
		void RenderPTMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
			const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer);
		void RenderPNTMesh(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerMesh, ID3D11Buffer* pCbPerSubset,
			const XMFLOAT4X4A* pViewProjMatrix, const MeshRenderer* pMeshRenderer);
		void RenderTerrain(ID3D11DeviceContext* pDeviceContext, const Terrain* pTerrain);
		void RenderSkybox(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pCbPerSkybox, 
			const XMFLOAT4X4A* pViewProjMatrix, const XMFLOAT3A* pCameraPos, const Skybox* pSkybox);
	private:
		Pass* m_pCurrentPass;

		Pass m_passStandardP;
		Pass m_passStandardPC;
		Pass m_passStandardPN;
		Pass m_passStandardPT;
		Pass m_passStandardPNT;
		Pass m_passSkybox;
		Pass m_passTerrain;

		Pass m_passMSCameraMerge;

		ComPtr<ID3D11Buffer> m_cpCbPerFrame;
		ComPtr<ID3D11Buffer> m_cpCbPerCamera;
		ComPtr<ID3D11Buffer> m_cpCbPerMesh;
		ComPtr<ID3D11Buffer> m_cpCbPerSubset;
		ComPtr<ID3D11Buffer> m_cpCbPerTerrain;
		ComPtr<ID3D11Buffer> m_cpCbPerCameraMerge;
	};

	extern RendererImpl Renderer;
}

#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPCTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPNTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPTTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardPNTTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\SkyboxTechnique.h>
#include <ZergEngine\CoreSystem\LowLevelRenderer\TerrainTechnique.h>

namespace ze
{
	class Renderer : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(Renderer);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void RenderFrame();
	private:
		StandardPTechnique m_techStandardP;
		StandardPCTechnique m_techStandardPC;
		StandardPNTechnique m_techStandardPN;
		StandardPTTechnique m_techStandardPT;
		StandardPNTTechnique m_techStandardPNT;
		SkyboxTechnique m_techSkybox;
		TerrainTechnique m_techTerrain;

		ComPtr<ID3D11Buffer> m_cpCbPerFrame;
		ComPtr<ID3D11Buffer> m_cpCbPerCamera;
		ComPtr<ID3D11Buffer> m_cpCbPerMesh;
		ComPtr<ID3D11Buffer> m_cpCbPerSubset;
		ComPtr<ID3D11Buffer> m_cpCbPerTerrain;
		ComPtr<ID3D11Buffer> m_cpCbPerRenderResultMerge;
	};
}

#pragma once

#include <ZergEngine\CoreSystem\RenderContext\EffectDeviceContext.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectP.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPC.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNTT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNTTSkinned.h>
#include <ZergEngine\CoreSystem\Effect\TerrainEffect.h>
#include <ZergEngine\CoreSystem\Effect\SkyboxEffect.h>
#include <ZergEngine\CoreSystem\Effect\DrawScreenRatioQuadWithTextureEffect.h>
#include <ZergEngine\CoreSystem\Effect\DrawScreenRatioQuadWithMSTextureEffect.h>
#include <ZergEngine\CoreSystem\Effect\ButtonEffect.h>
#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>
#include <vector>

namespace ze
{
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class Terrain;
	class IUIObject;
	class Panel;
	class Image;
	class Text;
	class Button;
	class InputField;

	class Renderer
	{
		friend class Runtime;
	public:
		static Renderer* GetInstance() { return s_pInstance; }
	private:
		Renderer();
		virtual ~Renderer();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();

		void RenderFrame();

		void RenderVFPositionMesh(const MeshRenderer* pMeshRenderer);
		// void RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer);
		// void RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer);
		// void RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer);
		// void RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalTangentTexCoordMesh(const MeshRenderer* pMeshRenderer);
		void RenderVFPositionNormalTangentTexCoordSkinnedMesh(const SkinnedMeshRenderer* pSkinnedMeshRenderer);
		void RenderTerrain(const Terrain* pTerrain);
		void RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV);
		void RenderPanel(const Panel* pPanel);
		void RenderImage(const Image* pImage);
		void RenderText(const Text* pText);
		void RenderButton(const Button* pButton);
		void RenderInputField(const InputField* pInputField);
	private:
		static Renderer* s_pInstance;

		ID3D11RasterizerState* m_pCullBackRS;
		ID3D11RasterizerState* m_pCullNoneRS;
		ID3D11RasterizerState* m_pWireFrameRS;
		ID3D11DepthStencilState* m_pDefaultDSS;
		ID3D11DepthStencilState* m_pSkyboxDSS;
		ID3D11DepthStencilState* m_pDepthReadOnlyDSS;
		ID3D11DepthStencilState* m_pNoDepthStencilTestDSS;
		ID3D11BlendState* m_pOpaqueBS;
		ID3D11BlendState* m_pAlphaBlendBS;
		ID3D11BlendState* m_pNoColorWriteBS;
		ID3D11Buffer* m_pButtonVB;

		EffectDeviceContext m_effectImmediateContext;

		XMFLOAT4X4A* m_pAnimFinalTransformBufferSpace;
		XMFLOAT4X4A* m_pAnimFinalTransformIdentity;
		XMFLOAT4X4A* m_pAnimFinalTransformBuffer;

		// Unused
		// BasicEffectP m_basicEffectP;
		// BasicEffectPC m_basicEffectPC;
		// BasicEffectPN m_basicEffectPN;
		// BasicEffectPT m_basicEffectPT;
		// BasicEffectPNT m_basicEffectPNT;
		BasicEffectPNTT m_basicEffectPNTT;
		BasicEffectPNTTSkinned m_basicEffectPNTTSkinned;
		TerrainEffect m_terrainEffect;
		SkyboxEffect m_skyboxEffect;
		DrawScreenRatioQuadWithTextureEffect m_drawScreenQuadTex;
		DrawScreenRatioQuadWithMSTextureEffect m_drawScreenQuadMSTex;
		ButtonEffect m_buttonEffect;
		ImageEffect m_imageEffect;
		std::vector<const IUIObject*> m_uiRenderQueue;
	};
}

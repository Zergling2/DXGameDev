#pragma once

#include <ZergEngine\CoreSystem\RenderContext\EffectDeviceContext.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectP.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPC.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPN.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNTT.h>
#include <ZergEngine\CoreSystem\Effect\BasicEffectPNTTSkinned.h>
#include <ZergEngine\CoreSystem\Effect\TerrainEffect.h>
#include <ZergEngine\CoreSystem\Effect\SkyboxEffect.h>
#include <ZergEngine\CoreSystem\Effect\BillboardEffect.h>
#include <ZergEngine\CoreSystem\Effect\DrawScreenRatioQuadWithTextureEffect.h>
#include <ZergEngine\CoreSystem\Effect\DrawScreenRatioQuadWithMSTextureEffect.h>
#include <ZergEngine\CoreSystem\Effect\ShadedEdgeQuadEffect.h>
#include <ZergEngine\CoreSystem\Effect\ShadedEdgeCircleEffect.h>
#include <ZergEngine\CoreSystem\Effect\CheckboxEffect.h>
#include <ZergEngine\CoreSystem\Effect\ImageEffect.h>
#include <vector>

namespace ze
{
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class Terrain;
	class Billboard;
	class Panel;
	class Image;
	class Text;
	class Button;
	class InputField;
	class SliderControl;
	class Checkbox;
	class RadioButton;
	class IUIObject;

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

		// void XM_CALLCONV RenderVFPositionMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		// void XM_CALLCONV RenderVFPositionColorMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		// void XM_CALLCONV RenderVFPositionNormalMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		// void XM_CALLCONV RenderVFPositionTexCoordMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		// void XM_CALLCONV RenderVFPositionNormalTexCoordMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		void XM_CALLCONV RenderPNTTMesh(const MeshRenderer* pMeshRenderer, FXMMATRIX worldMatrix);
		void XM_CALLCONV RenderPNTTSkinnedMesh(const SkinnedMeshRenderer* pSkinnedMeshRenderer, FXMMATRIX worldMatrix);
		void RenderTerrain(const Terrain* pTerrain);
		void RenderSkybox(ID3D11ShaderResourceView* pSkyboxCubeMapSRV);
		void RenderBillboard(const Billboard* pBillboard);

		// D2D 렌더링 요소가 필요한 UI는 필요 인터페이스를 추가적으로 전달
		void RenderPanel(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Panel* pPanel);
		void RenderImage(const Image* pImage);
		void RenderText(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Text* pText);
		void RenderButton(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Button* pButton);
		void RenderInputField(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const InputField* pInputField);
		void RenderSliderControl(const SliderControl* pSliderControl);
		void RenderCheckbox(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const Checkbox* pCheckbox);
		void RenderRadioButton(ID2D1RenderTarget* pD2DRenderTarget, ID2D1SolidColorBrush* pBrush, const RadioButton* pRadioButton);
	private:
		static Renderer* s_pInstance;

		ID3D11RasterizerState* m_pRSSolidCullBack;
		ID3D11RasterizerState* m_pRSMultisampleSolidCullBack;
		ID3D11RasterizerState* m_pRSSolidCullNone;
		ID3D11RasterizerState* m_pRSMultisampleSolidCullNone;
		ID3D11RasterizerState* m_pRSWireframe;
		ID3D11RasterizerState* m_pRSMultisampleWireframe;
		ID3D11DepthStencilState* m_pDSSDefault;
		ID3D11DepthStencilState* m_pDSSSkybox;
		ID3D11DepthStencilState* m_pDSSDepthReadOnlyLess;
		ID3D11DepthStencilState* m_pDSSNoDepthStencilTest;
		ID3D11BlendState* m_pBSOpaque;
		ID3D11BlendState* m_pBSAlphaBlend;
		ID3D11BlendState* m_pBSNoColorWrite;
		ID3D11Buffer* m_pVBPNTTQuad;
		ID3D11Buffer* m_pVBCheckbox;

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
		BillboardEffect m_billboardEffect;
		DrawScreenRatioQuadWithTextureEffect m_drawScreenQuadTex;
		DrawScreenRatioQuadWithMSTextureEffect m_drawScreenQuadMSTex;
		ShadedEdgeQuadEffect m_shadedEdgeQuadEffect;
		ShadedEdgeCircleEffect m_shadedEdgeCircleEffect;
		CheckboxEffect m_checkboxEffect;
		ImageEffect m_imageEffect;
		std::wstring m_asteriskStr;
		std::vector<std::pair<const Billboard*, float>> m_billboardRenderQueue;
		std::vector<const IUIObject*> m_uiRenderQueue;
	};
}

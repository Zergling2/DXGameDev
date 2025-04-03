#pragma once

#include <ZergEngine\CoreSystem\LowLevelRenderer\StandardTechniqueInterface.h>

namespace ze
{
	class StandardPTechnique : public IStandardTechnique
	{
	public:
		StandardPTechnique() = default;
		virtual ~StandardPTechnique() = default;

		virtual void Apply(const D3D11_VIEWPORT* pViewport, ID3D11RenderTargetView* pRTVColorBuffer, ID3D11DepthStencilView* pDSVColorBuffer,
			RASTERIZER_FILL_MODE rfm, RASTERIZER_CULL_MODE rcm, TEXTURE_FILTERING_OPTION tfo) override;
		virtual void Render(const MeshRenderer* pMeshRenderer) override;
	};
}

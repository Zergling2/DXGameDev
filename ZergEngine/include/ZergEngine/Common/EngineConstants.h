#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃          Constants          ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	constexpr uint32_t CELLS_PER_TERRAIN_PATCH = 64;
	constexpr uint32_t MAX_GLOBAL_LIGHT_COUNT = 4;
	constexpr uint16_t MAX_CAMERA_COUNT = 4;

	enum class RasterizerFillMode
	{
		Wireframe,
		Solid,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class RasterizerCullMode
	{
		None,
		Front,
		Back,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class TextureFilteringMode
	{
		Point,
		Bilinear,
		Trilinear,
		Anisotropic2x,
		Anisotropic4x,
		Anisotropic8x,
		Anisotropic16x,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class VertexShaderType
	{
		TRANSFORM_SKYBOX_TO_HCS,
		TRANSFORM_TERRAIN_PATCH_CTRL_PT,
		TRANSFORM_P_TO_HCS,
		TRANSFORM_PC_TO_HCS,
		TRANSFORM_PN_TO_HCS,
		TRANSFORM_PT_TO_HCS,
		TRANSFORM_CAMERA_MERGE_QUAD,
		TRANSFORM_PNT_TO_HCS,
		TRANSFORM_BUTTON_TO_HCS,
		TRANSFORM_PT_QUAD_TO_HCS,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class HullShaderType
	{
		CalcTerrainTessFactor,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class DomainShaderType
	{
		SampleTerrainHeightMap,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class PixelShaderType
	{
		ColorSkyboxFragment,
		ColorTerrainFragment,
		ColorPositionFragment,
		ColorPositionColorFragment,
		ColorPositionNormalFragment,
		ColorPositionTexCoordFragmentWithSingleTexture,
		ColorPositionTexCoordFragmentWithSingleMSTexture,
		ColorPositionNormalTexCoordFragment,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class VertexBufferType
	{
		ButtonPt,
		// ImageButtonPt,	// (셰이더 지역변수로 처리 가능)
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class MultisamplingAntiAliasingMode
	{
		Off	= 1,
		x2	= 2,
		x4	= 4,
		x8	= 8,
		x16	= 16,
		x32	= 32
	};

	enum class ProjectionMethod : int8_t
	{
		Perspective,
		Orthographic
	};

	enum class ClearFlag : int8_t
	{
		Skybox,
		SolidColor,
		DepthOnly,
		DontClear
	};

	enum class VertexFormatType : int32_t
	{
		Position,
		PositionColor,
		PositionNormal,
		PositionTexCoord,
		PositionNormalTexCoord,
		TerrainPatchCtrlPt,
		ButtonPt,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT,
		UNKNOWN
	};
}

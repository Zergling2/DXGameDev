#pragma once

#include <cstdint>

namespace ze
{
	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃          Constants          ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	constexpr uint32_t CELLS_PER_TERRAIN_PATCH = 64;
	constexpr uint32_t MAX_GLOBAL_LIGHT_COUNT = 4;
	constexpr uint16_t MAX_CAMERA_COUNT = 4;
	constexpr uint8_t MAX_BONE_COUNT = 96;

	enum class RasterizerMode
	{
		WireframeMultisample,
		WireframeNoMultisample,
		SolidCullNoneMultisample,
		SolidCullNoneNoMultisample,
		SolidCullFrontMultisample,
		SolidCullFrontNoMultisample,
		SolidCullBackMultisample,
		SolidCullBackNoMultisample,
		ShadowMap,	// No multisample

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
		ShadowMapSampling,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT
	};

	enum class VertexShaderType
	{
		TransformSkyboxToHCS,
		TransformTerrainPatchCtrlPt,
		TransformPToHCS,
		TransformPCToHCS,
		TransformPNToHCS,
		TransformPTToHCS,
		TransformPNTToHCS,
		TransformPNTTToHCS,
		TransformPNTTSkinnedToHCS,
		TransformButtonToHCS,
		TransformPTQuadToHCS,
		TransformCameraMergeQuad,
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
		ColorPositionNormalTangentTexCoordFragment,
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

	enum class VertexFormatType
	{
		Position,
		PositionColor,
		PositionNormal,
		PositionTexCoord,
		PositionNormalTexCoord,
		PositionNormalTangentTexCoord,
		PositionNormalTangentTexCoordSkinned,
		TerrainPatchCtrlPt,
		ButtonPt,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT,
		UNKNOWN
	};
}

#pragma once

#include <cstdint>

namespace ze
{
	// 旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬
	// 早          Constants          早
	// 曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭
	constexpr uint32_t FIXED_UPDATE_HZ = 60;
	constexpr float FIXED_DELTA_TIME = 1.0f / static_cast<float>(FIXED_UPDATE_HZ);
	constexpr uint32_t CELLS_PER_TERRAIN_PATCH = 64;
	constexpr uint32_t MAX_GLOBAL_LIGHT_COUNT = 4;
	constexpr uint16_t MAX_CAMERA_COUNT = 4;
	constexpr uint8_t MAX_BONE_COUNT = 96;
	constexpr float BOUNDING_BOX_MIN_EXTENT = 0.005f;
	constexpr uint32_t TRIANGLESTRIP_QUAD_VERTEX_COUNT = 4;
	constexpr uint32_t CHECKBOX_VERTEX_COUNT = 66;
	constexpr float SPACING_BETWEEN_CHECKBOX_AND_TEXTBOX = 4.0f;
	constexpr float SPACING_BETWEEN_RADIOBUTTON_AND_TEXTBOX = 4.0f;

	enum class RasterizerMode
	{
		MultisampleWireframe,

		Wireframe,

		MultisampleSolidCullNone,

		SolidCullNone,

		MultisampleSolidCullFront,

		SolidCullFront,

		MultisampleSolidCullBack,

		SolidCullBack,

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
		// 收收收收收收收收收收收收收收收收收收收收收收
		COUNT
	};

	enum class VertexShaderType
	{
		ToHcsDebugLines,
		ToHcsP,
		ToHcsPC,
		ToHcsPN,
		ToHcsPT,
		ToHcsPNT,
		ToHcsPNTT,
		ToHcsPNTTSkinned,
		TerrainPatchCtrlPt,
		ToHcsSkybox,
		ToHcsPNTTQuadForBillboard,
		ToHcsPNTTQuadForImage,
		ToHcsPNTTQuadForShadedEdgeQuad,
		ToHcsPNTTQuadForShadedEdgeCircle,
		ToHcsCheckbox,
		ToHcsScreenRatioQuad,
		// ToHcsShaded2DQuad,
		// 收收收收收收收收收收收收收收收收收收收收收收

		COUNT
	};

	enum class HullShaderType
	{
		CalcTerrainTessFactor,
		// 收收收收收收收收收收收收收收收收收收收收收收

		COUNT
	};

	enum class DomainShaderType
	{
		SampleTerrainHeightMap,
		// 收收收收收收收收收收收收收收收收收收收收收收

		COUNT
	};

	// P: Position
	// PC: Position, Color
	// PN: Position, Normal
	// PT: Position, TexCoord
	// PNT: Position, Normal, TexCoord
	// PNTT: Position, Normal, Tangent, TexCoord
	enum class PixelShaderType
	{
		UnlitP,
		UnlitPC,
		UnlitPNNoMtl,
		LitPN,
		UnlitPT1Tex,
		UnlitPT1MSTex,
		// LitPNT,		// deprecated
		UnlitPNTTNoMtl,
		LitPNTT,
		LitPNTTDiffMapping,
		LitPNTTSpecMapping,
		LitPNTTNormMapping,
		LitPNTTDiffSpecMapping,
		LitPNTTDiffNormMapping,
		LitPNTTSpecNormMapping,
		LitPNTTDiffSpecNormMapping,
		SkyboxFragment,
		LitTerrainFragment,
		ColorShadedEdgeQuad,
		ColorShadedEdgeCircle,
		// ShadowMapAlphaClipping,
		// 收收收收收收收收收收收收收收收收收收收收收收

		COUNT
	};

	enum class MSAAMode
	{
		Off	= 1,
		x2	= 2,
		x4	= 4,
		x8	= 8
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
		Shaded2DQuad,
		Checkbox,
		// 收收收收收收收收收收收收收收收收收收收收收收

		COUNT,
		Unknown
	};


}

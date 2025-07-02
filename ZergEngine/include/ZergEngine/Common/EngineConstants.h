#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	// ��������������������������������������������������������������
	// ��          Constants          ��
	// ��������������������������������������������������������������
	constexpr uint32_t MAX_GLOBAL_LIGHT_COUNT = 4;
	constexpr uint16_t MAX_CAMERA_COUNT = 4;

	enum class RASTERIZER_FILL_MODE
	{
		WIREFRAME,
		SOLID,
		COUNT
	};

	enum class RASTERIZER_CULL_MODE
	{
		NONE,
		FRONT,
		BACK,
		COUNT
	};

	enum class TEXTURE_FILTERING_OPTION
	{
		POINT,
		BILINEAR,
		TRILINEAR,
		ANISOTROPIC_2X,
		ANISOTROPIC_4X,
		ANISOTROPIC_8X,
		ANISOTROPIC_16X,
		COUNT
	};

	enum class VERTEX_SHADER_TYPE
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

		COUNT
	};

	enum class HULL_SHADER_TYPE
	{
		CALC_TERRAIN_TESS_FACTOR,

		COUNT
	};

	enum class DOMAIN_SHADER_TYPE
	{
		SAMPLE_TERRAIN_HEIGHT_MAP,

		COUNT
	};

	enum class PIXEL_SHADER_TYPE
	{
		COLOR_SKYBOX_FRAGMENT,
		COLOR_TERRAIN_FRAGMENT,
		COLOR_P_FRAGMENT,
		COLOR_PC_FRAGMENT,
		COLOR_PN_FRAGMENT,
		COLOR_PT_FRAGMENT,
		COLOR_PNT_FRAGMENT,
		COLOR_PT_FRAGMENT_SINGLE_TEXTURE,
		COLOR_PT_FRAGMENT_SINGLE_MSTEXTURE,
		COLOR_BUTTON_FRAGMENT,
		COLOR_PT_UI_QUAD,

		COUNT
	};

	enum class VERTEX_BUFFER_TYPE
	{
		BUTTON,
		// IMAGE_BUTTON,	// (���̴� ���������� ó�� ����)

		COUNT
	};

	enum class MSAA_SAMPLE_COUNT : uint32_t
	{
		OFF	= 1,
		X2	= 2,
		X4	= 4,
		X8	= 8,
		X16	= 16,
		X32	= 32
	};

	enum class PROJECTION_METHOD : int8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	enum class CLEAR_FLAG : int8_t
	{
		SKYBOX,
		SOLID_COLOR,
		DEPTH_ONLY,
		DONT_CLEAR
	};

	enum class VERTEX_FORMAT_TYPE : int32_t
	{
		POSITION,
		POSITION_COLOR,
		POSITION_NORMAL,
		POSITION_TEXCOORD,
		POSITION_NORMAL_TEXCOORD,
		TERRAIN_PATCH_CTRL_PT,
		BUTTON,
		IMAGE_BUTTON,
		// ��������������������������������������������
		COUNT,
		UNKNOWN
	};
}

#pragma once

// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
// ┃           Headers           ┃
// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#include <Windows.h>
#include <windowsx.h>
#include <WinSock2.h>
#include <strsafe.h>
#include <locale.h>
#include <wrl\client.h>

#include <thread>
#include <memory>
#include <cstdlib>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <limits>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <dinput.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using index_format = uint32_t;

#define FATAL_ERROR *reinterpret_cast<int*>(0) = 0

namespace zergengine
{
	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃       Type definition       ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	using sysresult = int32_t;
	using sceneId = LONG64;
	using SceneTableMap = std::unordered_map<std::wstring, class IScene* (*)()>;
	
	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃        Helper Macros        ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	#define ZEINFLOOP for (;;)
	#define cbuffer struct alignas(16)
	#define hlsl_struct struct alignas(16)

	// HLSL 셰이더 행렬로 변환
	#define ConvertToHLSLMatrix XMMatrixTranspose
	// 전치 후 HLSL 셰이더 행렬로 변환
	#define TransposeAndConvertToHLSLMatrix

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃       Helper functions      ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	template <typename T>
	constexpr bool IsPowerOfTwo(T n)
	{
		return n > 0 && (n & (n - 1)) == 0;
	}

	template<class T>
	inline void SafeDelete(T*& ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template<class T>
	inline void SafeDeleteArray(T*& ptr)
	{
		if (ptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}

	template<class T>
	inline void SafeReleaseCOM(T*& ptr)
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃            Types            ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	struct EngineSystemInitDescriptor
	{
		uint32_t width;
		uint32_t height;
		bool fullscreen;
		const SceneTableMap& stm;
	};

	struct Resolution
	{
		inline Resolution(bool fullscreen, uint32_t width, uint32_t height)
			: fullscreen(fullscreen)
			, width(width)
			, height(height)
		{
		}
		inline bool operator==(const Resolution& rhs)
		{
			return fullscreen == rhs.fullscreen && width == rhs.width && height == rhs.height;
		}
		bool fullscreen;
		uint32_t width;
		uint32_t height;
	};
	
	template<class T>
	struct Size2D
	{
		inline Size2D()
		{
		}
		inline Size2D(T x, T y)
			: x(x)
			, y(y)
		{
		}
		T x;
		T y;
	};

	enum class MSAACount : UINT
	{
		OFF = 1U,
		X2 = 2U,
		X4 = 4U,
		X8 = 8U,
		X16 = 16U,
		X32 = 32U
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
		TERRAIN,
		// ━━━━━━━━━━━━━━━━━━━━━━
		COUNT,
		UNKNOWN
	};

	struct ClippingPlanes
	{
		inline ClippingPlanes(float nearZ, float farZ)
			: nearZ(nearZ)
			, farZ(farZ)
		{
		}
		float nearZ;
		float farZ;
	};

	// All member values are normalized value
	struct NormalizedViewportRect
	{
		inline NormalizedViewportRect(float x, float y, float w, float h)
			: x(x)
			, y(y)
			, w(w)
			, h(h)
		{
		}
		float x;
		float y;
		float w;
		float h;
	};

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃          Constants          ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	// constexpr float allows only one floating point constant to exist in memory, even if it is not encoded in a x86 command.
	constexpr DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr uint32_t SWAP_CHAIN_FLAG = 0;
	constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f;
	constexpr uint8_t CAMERA_FIELD_OF_VIEW_DEFAULT = 92;
	constexpr int8_t CAMERA_DEPTH_DEFAULT = 0;
	constexpr PROJECTION_METHOD CAMERA_PROJECTION_METHOD_DEFAULT = PROJECTION_METHOD::PERSPECTIVE;
	constexpr CLEAR_FLAG CAMERA_CLEAR_FLAG_DEFAULT = CLEAR_FLAG::SOLID_COLOR;
	XMGLOBALCONST XMVECTORF32 CAMERA_BACKGROUND_COLOR_DEFAULT = Colors::DarkBlue;
	constexpr float CAMERA_CLIPPING_NEAR_PLANE_DEFAULT = 0.3f;
	constexpr float CAMERA_CLIPPING_FAR_PLANE_DEFAULT = 1000.0f;
	constexpr float CAMERA_VIEWPORT_X_DEFAULT = 0.0f;
	constexpr float CAMERA_VIEWPORT_Y_DEFAULT = 0.0f;
	constexpr float CAMERA_VIEWPORT_WIDTH_DEFAULT = 1.0f;
	constexpr float CAMERA_VIEWPORT_HEIGHT_DEFAULT = 1.0f;

	constexpr uint16_t CELLS_PER_TERRAIN_PATCH = 64;

	XMGLOBALCONST XMVECTORF32 CAMERA_FORWARD_VECTOR = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMGLOBALCONST XMVECTORF32 CAMERA_UP_VECTOR = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMGLOBALCONST XMVECTORF32 DEFAULT_SCALE = { 1.0f, 1.0f, 1.0f, 0.0f } ;
	XMGLOBALCONST XMVECTORF32 WORLD_UP = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMGLOBALCONST XMVECTORF32 LIGHT_DIRECTION_LOCAL_AXIS = { 0.0f, 0.0f, 1.0f, 0.0f };

	// #################################
	constexpr uint32_t BACKBUFFER_COUNT = 2;
	constexpr uint32_t MAX_GLOBAL_LIGHT_COUNT = 4;
	constexpr uint16_t MAX_CAMERA_COUNT = 4;
	// #################################
}

#pragma once

// Windows
#include <Windows.h>
#include <windowsx.h>
#include <winerror.h>
#include <process.h>
#include <intrin.h>
#include <strsafe.h>
#include <tchar.h>
#include <wrl.h>
#include <time.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// DirectX
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <dwrite.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;


// HLSL 셰이더 행렬로 변환
#define ConvertToHLSLMatrix XMMatrixTranspose
// 전치 후 HLSL 셰이더 행렬로 변환
#define TransposeAndConvertToHLSLMatrix
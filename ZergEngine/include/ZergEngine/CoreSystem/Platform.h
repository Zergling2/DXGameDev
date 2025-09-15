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

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// DirectX
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
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


// HLSL ���̴� ��ķ� ��ȯ
#define ConvertToHLSLMatrix XMMatrixTranspose
// ��ġ �� HLSL ���̴� ��ķ� ��ȯ
#define TransposeAndConvertToHLSLMatrix
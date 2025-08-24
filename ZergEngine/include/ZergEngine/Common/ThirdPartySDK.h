#pragma once

// Microsoft SDK
#define NOMINMAX

#include <Windows.h>
#include <windowsx.h>
#include <winerror.h>
#include <process.h>
#include <intrin.h>
#include <wrl\client.h>
#include <strsafe.h>
#include <tchar.h>

#include <cassert>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <memory>
#include <random>
#include <chrono>
#include <cstdint>
#include <type_traits>

// C++ STL
#include <utility>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>

// DirectX

#define DIRECTINPUT_VERSION 0x0800
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dinput.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

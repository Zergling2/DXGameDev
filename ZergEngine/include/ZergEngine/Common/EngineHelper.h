#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	/*
	XMVECTOR, XMMATRIX 전달 규칙
	https://learn.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-internals
	FXMVECTOR _1, FXMVECTOR _2, FXMVECTOR _3, GXMVECTOR _4, HXMVECTOR _5, HXMVECTOR _6, CXMVECTOR _7, CXMVECTOR _8, ...

	Use the FXMVECTOR alias to pass up to the first three instances of XMVECTOR used as arguments to a function.
	Use the GXMVECTOR alias to pass the 4th instance of an XMVECTOR used as an argument to a function.
	Use the HXMVECTOR alias to pass the 5th and 6th instances of an XMVECTOR used as an argument to a function. For info about additional considerations, see the __vectorcall documentation.
	Use the CXMVECTOR alias to pass any further instances of XMVECTOR used as arguments.

	FXMMATRIX = __m128 x 4
	Use the FXMMATRIX alias to pass the first XMMATRIX as an argument to the function.
	This assumes you don't have more than two FXMVECTOR arguments or more than two float, double,
	or FXMVECTOR arguments to the 'right' of the matrix. For info about additional considerations,
	see the __vectorcall documentation.
	Use the CXMMATRIX alias otherwise.
	*/

	/*
	In addition to the type aliases, you must also use the XM_CALLCONV annotation to make sure
	the function uses the appropriate calling convention (__fastcall versus __vectorcall) based
	on your compiler and architecture. Because of limitations with __vectorcall, we recommend
	that you not use XM_CALLCONV for C++ constructors.
	*/

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃        Helper Macros        ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	#define ZE_INF_FORLOOP ;;

	#define FILELINELOGFORMAT L"%s(%u) HRESULT=0x%x\n", __FILEW__, __LINE__, hr

	#define ZE_DECLARE_SINGLETON(ClassName)\
	public:\
		static inline ClassName& GetInstance() { return ClassName::s_instance; }\
		ClassName(const ClassName&) = delete;\
		ClassName& operator=(const ClassName&) = delete;\
		ClassName(ClassName&&) = delete;\
		ClassName& operator=(ClassName&&) = delete;\
	private:\
		ClassName();\
		~ClassName();\
	private:\
		static ClassName s_instance

	#define ZE_IMPLEMENT_SINGLETON(ClassName) ClassName ClassName::s_instance

	#define DECLARE_SLOT_NUMBER(Num)\
		private:\
			static constexpr uint32_t SLOT_NUM = Num;\
		public:\
			static constexpr uint32_t GetSlotNumber() { return SLOT_NUM; }

	// HLSL 셰이더 행렬로 변환
	#define ConvertToHLSLMatrix XMMatrixTranspose
	// 전치 후 HLSL 셰이더 행렬로 변환
	#define TransposeAndConvertToHLSLMatrix

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃       Helper functions      ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	template<class T>
	void SafeDelete(T*& ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template<class T>
	void SafeDeleteArray(T*& ptr)
	{
		if (ptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}

	template<class T>
	void SafeReleaseCOM(T*& ptr)
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}

	void SafeCloseCRTFile(FILE*& pFile);
	void SafeCloseWinThreadHandle(HANDLE& handle);

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃            Types            ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	#define cbuffer struct alignas(16)
	#define hlsl_struct struct alignas(16)

	struct WideStringComparator
	{
		bool operator()(const PCWSTR lhs, const PCWSTR rhs) const
		{
			return wcscmp(lhs, rhs) < 0;
		}
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
}

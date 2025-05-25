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

	#define ZE_DECLARE_SINGLETON(ClassName)\
	public:\
		ClassName();\
		~ClassName();\
		ClassName(const ClassName&) = delete;\
		ClassName& operator=(const ClassName&) = delete;\
		ClassName(ClassName&&) = delete;\
		ClassName& operator=(ClassName&&) = delete;\

	// HLSL 셰이더 행렬로 변환
	#define ConvertToHLSLMatrix XMMatrixTranspose
	// 전치 후 HLSL 셰이더 행렬로 변환
	#define TransposeAndConvertToHLSLMatrix

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃       Helper functions      ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	class Helper
	{
	public:
		class AutoCRTFileCloser
		{
		public:
			AutoCRTFileCloser() noexcept
				: m_pFile(nullptr)
			{
			}
			AutoCRTFileCloser(FILE* pFile) noexcept
				: m_pFile()
			{
			}
			~AutoCRTFileCloser()
			{
				this->Close();
			}

			void Set(FILE* pFile)
			{
				this->Close();
				m_pFile = pFile;
			}
			void Close();
		private:
			FILE* m_pFile;
		};

		template<class T>
		static void SafeDelete(T*& ptr)
		{
			if (ptr)
			{
				delete ptr;
				ptr = nullptr;
			}
		}

		template<class T>
		static void SafeDeleteArray(T*& ptr)
		{
			if (ptr)
			{
				delete[] ptr;
				ptr = nullptr;
			}
		}

		template<typename T>
		static void SafeReleaseCom(T*& cp)
		{
			if (cp)
			{
				cp->Release();
				cp = nullptr;
			}
		}

		static void SafeCloseWinThreadHandle(HANDLE& handle);
	};

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃            Types            ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	struct MultiByteStringComparator
	{
		bool operator()(const PCSTR lhs, const PCSTR rhs) const
		{
			return strcmp(lhs, rhs) < 0;
		}
	};
}

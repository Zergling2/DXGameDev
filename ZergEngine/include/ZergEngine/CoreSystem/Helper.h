#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃        Helper Macros        ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	#define ZE_INF_LOOP for (;;)

	#define ZE_DECLARE_SINGLETON(ClassName)\
	public:\
		ClassName();\
		~ClassName();\
		ClassName(const ClassName&) = delete;\
		ClassName& operator=(const ClassName&) = delete;\
		ClassName(ClassName&&) = delete;\
		ClassName& operator=(ClassName&&) = delete;\

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃       Helper functions      ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	class Helper
	{
	public:
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

		static int ConvertUTF8ToUTF16(const char* utf8Str, wchar_t* pBuffer, size_t cchWideChar);
		static int ConvertUTF16ToUTF8(const wchar_t* utf16Str, char* pBuffer, size_t cbMultiByte);
	};

	// ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	// ┃            Types            ┃
	// ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	struct MultiByteStringComparator
	{
		bool operator()(const char* lhs, const char* rhs) const
		{
			return strcmp(lhs, rhs) < 0;
		}
	};

	struct WideCharStringComparator
	{
		bool operator()(const wchar_t* lhs, const wchar_t* rhs) const
		{
			return wcscmp(lhs, rhs) < 0;
		}
	};

	class IndexConverter2DTo1D
	{
	public:
		IndexConverter2DTo1D(size_t columnSize = 1)
			: m_columnSize(columnSize)
		{
		}
		size_t GetColumnSize() const { return m_columnSize; }
		void SetColumnSize(size_t size) { m_columnSize = size; }
		size_t CalcIndex(size_t row, size_t col) { return row * m_columnSize + col; }
	private:
		size_t m_columnSize;
	};

	class IndexConverter3DTo1D
	{
	public:
		IndexConverter3DTo1D(size_t rowSize = 1, size_t columnSize = 1)
			: m_rowSize(rowSize)
			, m_2t1(columnSize)
		{
		}
		size_t GetRowSize() const { return m_rowSize; }
		void SetRowSize(size_t size) { m_rowSize = size; }
		size_t GetColumnSize() const { return m_2t1.GetColumnSize(); }
		void SetColumnSize(size_t size) { m_2t1.SetColumnSize(size); }
		size_t CalcIndex(size_t depth, size_t row, size_t col) { return depth * m_rowSize * m_2t1.GetColumnSize() + m_2t1.CalcIndex(row, col); }
	private:
		size_t m_rowSize;
		IndexConverter2DTo1D m_2t1;
	};

	// ID3D11DeviceContext::Map()으로 매핑한 리소스를 읽을 때 사용.
	template<class TexelFormat>
	class D3D11Mapped2DSubresourceReader
	{
	public:
		D3D11Mapped2DSubresourceReader(const D3D11_MAPPED_SUBRESOURCE& mapped)
			: m_mapped(mapped)
		{
		}

		TexelFormat GetTexel(size_t y, size_t x) const
		{
			uint8_t* pRow = static_cast<uint8_t*>(m_mapped.pData) + y * m_mapped.RowPitch;
			return reinterpret_cast<TexelFormat*>(pRow)[x];
		}
	private:
		const D3D11_MAPPED_SUBRESOURCE& m_mapped;
	};
}

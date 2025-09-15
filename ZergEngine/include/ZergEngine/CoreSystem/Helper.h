#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	

	// 旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬
	// 早        Helper Macros        早
	// 曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭
	#define ZE_INF_LOOP for (;;)

	#define ZE_DECLARE_SINGLETON(ClassName)\
	public:\
		ClassName();\
		~ClassName();\
		ClassName(const ClassName&) = delete;\
		ClassName& operator=(const ClassName&) = delete;\
		ClassName(ClassName&&) = delete;\
		ClassName& operator=(ClassName&&) = delete;\

	// 旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬
	// 早       Helper functions      早
	// 曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭
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

	// 旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬
	// 早            Types            早
	// 曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭
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
		void SetColumnSize(size_t size) { m_columnSize = size; }
		size_t GetColumnSize() const { return m_columnSize; }
		size_t GetIndex(size_t row, size_t col) { return row * m_columnSize + col; }
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
		void SetRowSize(size_t size) { m_rowSize = size; }
		size_t GetRowSize() const { return m_rowSize; }
		void SetColumnSize(size_t size) { m_2t1.SetColumnSize(size); }
		size_t GetColumnSize() const { return m_2t1.GetColumnSize(); }
		size_t GetIndex(size_t depth, size_t row, size_t col) { return depth * m_rowSize * m_2t1.GetColumnSize() + m_2t1.GetIndex(row, col); }
	private:
		size_t m_rowSize;
		IndexConverter2DTo1D m_2t1;
	};

	template<class TexelFormat>
	class D3D11Mapped2DSubresourceReader
	{
	public:
		D3D11Mapped2DSubresourceReader(const D3D11_MAPPED_SUBRESOURCE* pMapped)
			: m_pMapped(pMapped)
		{
		}

		TexelFormat GetTexel(size_t y, size_t x) const
		{
			byte* pRow = reinterpret_cast<byte*>(m_pMapped->pData) + y * m_pMapped->RowPitch;
			return reinterpret_cast<TexelFormat*>(pRow)[x];
		}
	private:
		const D3D11_MAPPED_SUBRESOURCE* m_pMapped;
	};
}

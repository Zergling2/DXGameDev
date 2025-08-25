#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\SlimRWLock.h>

namespace ze
{
	class Debug
	{
	public:
		static void ForceCrashWithMessageBox(PCWSTR title, PCWSTR format, ...);
		static void ForceCrashWithWin32ErrorMessageBox(PCWSTR message, DWORD code);
		static void ForceCrashWithHRESULTErrorMessageBox(PCWSTR message, HRESULT hr);
		static PCTSTR DXGIFormatToString(DXGI_FORMAT fmt);
		static PCTSTR SRVDimensionToString(D3D11_SRV_DIMENSION d);
		static PCTSTR VertexFormatToString(VERTEX_FORMAT_TYPE type);
	};

	enum class SEVERITY
	{
		SUCCESS = 0,
		INFO = 1,
		WARNING = 2,
		ERR0R = 3
	};
	enum class IDENTIFICATION_CODE
	{
		MICROSOFT_DEFINED = Math::PowerOf2(8) - 1,

		CREATETHREAD_ISSUE,
		FILE_IO_ISSUE,
		RELEASE_ISSUE,

		MAX = Math::PowerOf2(12) - 1
	};
	enum class EXCEPTION_CODE : DWORD
	{
		FAILEDTO_CREATETHREAD,
		FAILEDTO_OPEN_FILE,

		MAX = Math::PowerOf2(16) - 1
	};

	class ErrorCodeCreator
	{
	private:
		//                                                .   .   .   .   .   .   .   .
		static constexpr DWORD USER_DEFINED_CODE_BASE = 0b00100000000000000000000000000000;
		static constexpr DWORD Create(SEVERITY s, IDENTIFICATION_CODE ic, EXCEPTION_CODE ec)
		{
			return (static_cast<DWORD>(s) << 30) | USER_DEFINED_CODE_BASE | (static_cast<DWORD>(ic) << 16) | static_cast<DWORD>(ec);
		}
	};

	struct AsyncLogBuffer
	{
		static constexpr size_t BUFFER_SIZE = Math::PowerOf2<size_t>(8) - sizeof(AsyncLogBuffer*) - sizeof(void*);
		static constexpr size_t BUFFER_COUNT = BUFFER_SIZE / sizeof(wchar_t);
		wchar_t buffer[BUFFER_SIZE / sizeof(wchar_t)];
		void* m_pInstance;	// Logger Instance (AsyncConsoleLogger와 AsyncFileLogger의 WriteProc에서 각각 reinterpret_cast 하여 되돌려주면 된다.)
		AsyncLogBuffer* m_pNext;
	};

	static_assert(Math::IsPowerOf2(sizeof(AsyncLogBuffer)), "AsyncLogBuffer size must be a power of two");

	class AsyncConsoleLogger
	{
	public:
		AsyncConsoleLogger();
		~AsyncConsoleLogger();

		bool Init();
		void Release();

		HRESULT Write(PCWSTR str);
		HRESULT __cdecl WriteFormat(PCWSTR format, ...);
	private:
		AsyncLogBuffer* GetLogBuffer();
		AsyncLogBuffer* AllocLogBufferList();

		volatile LONG GetExitFlag() const { return m_exit; }
		static unsigned int __stdcall WorkerThreadEntry(void* arg);
		static VOID NTAPI WriteProc(ULONG_PTR parameter);

		void SafeReleaseWorkerThread();
	private:
		bool m_init;
		volatile LONG m_exit;
		SlimRWLock m_lock;
		HANDLE m_hWorker;
		AsyncLogBuffer* m_pTop;
		std::vector<LPVOID> m_bufferLargePages;
	};

	class SyncFileLogger
	{
	public:
		SyncFileLogger();
		~SyncFileLogger();

		bool Init(PCWSTR fileName);
		void Release();

		HRESULT Write(PCWSTR str);
		HRESULT __cdecl WriteFormat(PCWSTR format, ...);
	private:
		bool m_init;
		FILE* m_pFile;
	};

	class AsyncFileLogger
	{
	public:
		AsyncFileLogger();
		~AsyncFileLogger();

		bool Init(PCWSTR fileName);
		void Release();

		HRESULT Write(PCWSTR str);
		HRESULT __cdecl WriteFormat(PCWSTR format, ...);
	private:
		AsyncLogBuffer* GetLogBuffer();
		AsyncLogBuffer* AllocLogBufferList();

		volatile LONG GetExitFlag() const { return m_exit; }
		static UINT __stdcall WorkerThreadEntry(void* arg);
		static VOID NTAPI WriteProc(ULONG_PTR parameter);

		void SafeReleaseWorkerThread();
	private:
		bool m_init;
		volatile LONG m_exit;
		FILE* m_pFile;
		SlimRWLock m_lock;
		HANDLE m_hWorker;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		alignas(64) AsyncLogBuffer* m_pTop;
		uint32_t m_flushTimer;
		std::vector<LPVOID> m_bufferLargePages;
	};
}

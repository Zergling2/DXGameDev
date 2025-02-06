#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

LPCSTR GetD3D11ReturnCodeMessage(HRESULT hr);

namespace zergengine
{
	struct LogBuffer;

	LogBuffer* AllocLogBufferList(std::vector<LPVOID>& bufferLargePages);

	struct LogBuffer
	{
		friend class AsyncConsoleLogger;
		friend class AsyncFileLogger;
		friend LogBuffer* zergengine::AllocLogBufferList(std::vector<LPVOID>& bufferLargePages);
		static constexpr size_t BUFFER_SIZE = 256 - sizeof(LogBuffer*);
		wchar_t buffer[BUFFER_SIZE / sizeof(wchar_t)];
	private:
		LogBuffer* m_pNext;
	};
	static_assert(IsPowerOfTwo(sizeof(LogBuffer)), "LogBuffer size must be a power of two");

	class AsyncConsoleLogger
	{
		friend class Engine;
	public:
		AsyncConsoleLogger();
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();

		static LogBuffer* GetLogBuffer();
		static inline void PushLog(const LogBuffer* pBuf)
		{
			QueueUserAPC(AsyncConsoleLogger::JobProc, AsyncConsoleLogger::s_worker, reinterpret_cast<ULONG_PTR>(pBuf));
		}
		static inline volatile LONG GetExitFlag() { return AsyncConsoleLogger::s_exit; }
	private:
		static unsigned int __stdcall WorkerThreadEntry(void* arg);
		static VOID NTAPI JobProc(ULONG_PTR parameter);
	private:
		static volatile LONG s_exit;
		static SRWLOCK s_lock;
		static HANDLE s_worker;
		static LogBuffer* s_pTop;
		static std::vector<LPVOID> s_bufferLargePages;
	};

	class AsyncFileLogger
	{
		friend class Engine;
	public:
		AsyncFileLogger();
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();

		static LogBuffer* GetLogBuffer();
		static inline void PushLog(const LogBuffer* pBuf)
		{
			QueueUserAPC(AsyncFileLogger::JobProc, AsyncFileLogger::s_worker, reinterpret_cast<ULONG_PTR>(pBuf));
		}
		static inline volatile LONG GetExitFlag() { return AsyncFileLogger::s_exit; }
	private:
		static unsigned int __stdcall WorkerThreadEntry(void* arg);
		static VOID NTAPI JobProc(ULONG_PTR parameter);
		static LONG CALLBACK FlushLog(PEXCEPTION_POINTERS pExceptionPointer);
	private:
		static FILE* s_pFile;
		static volatile LONG s_exit;
		static SRWLOCK s_lock;
		static HANDLE s_worker;
		// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
		static alignas(64) LogBuffer* s_pTop;
		static uint32_t s_flushTimer;
		static std::vector<LPVOID> s_bufferLargePages;
	};

	void hrlog(const wchar_t* file, DWORD line, HRESULT hr);
}

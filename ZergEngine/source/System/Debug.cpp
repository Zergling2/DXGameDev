#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\Runtime.h>

using namespace zergengine;

volatile LONG AsyncConsoleLogger::s_exit = FALSE;
SRWLOCK AsyncConsoleLogger::s_lock;
HANDLE AsyncConsoleLogger::s_worker = NULL;
LogBuffer* AsyncConsoleLogger::s_pTop = nullptr;
std::vector<LPVOID> AsyncConsoleLogger::s_bufferLargePages;

FILE* AsyncFileLogger::s_pFile = nullptr;
volatile LONG AsyncFileLogger::s_exit = FALSE;
SRWLOCK AsyncFileLogger::s_lock;
HANDLE AsyncFileLogger::s_worker;
alignas(64) LogBuffer* AsyncFileLogger::s_pTop = nullptr;
uint32_t AsyncFileLogger::s_flushTimer = 0;
std::vector<LPVOID> AsyncFileLogger::s_bufferLargePages;

static VOID NTAPI DoNothing(ULONG_PTR parameter)
{
}

LPCSTR GetD3D11ReturnCodeMessage(HRESULT hr)
{
	switch (hr)
	{
	case D3D11_ERROR_FILE_NOT_FOUND:
		return "The file was not found.";
	case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
		return "There are too many unique instances of a particular type of state object.";
	case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
		return "There are too many unique instances of a particular type of view object.";
	case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
		return "The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext\
			or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD.";
	case DXGI_ERROR_INVALID_CALL:
		return "The method call is invalid. For example, a method's parameter may not be a valid pointer.";
	case DXGI_ERROR_WAS_STILL_DRAWING:
		return "The previous blit operation that is transferring information to or from this surface is incomplete.";
	case E_FAIL:
		return "Attempted to create a device with the debug layer enabled and the layer is not installed.";
	case E_INVALIDARG:
		return "An invalid parameter was passed to the returning function.";
	case E_OUTOFMEMORY:
		return "Direct3D could not allocate sufficient memory to complete the call.";
	case E_NOTIMPL:
		return "The method call isn't implemented with the passed parameter combination.";
	default:
		return "Unknown error.";
	}
}

LogBuffer* zergengine::AllocLogBufferList(std::vector<LPVOID>& bufferLargePages)
{
	// Protected by m_lock SRWLOCK.
	LPVOID pages = VirtualAlloc(nullptr, Engine::GetSystemInfo().dwAllocationGranularity,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	const size_t itercnt = Engine::GetSystemInfo().dwAllocationGranularity / sizeof(LogBuffer) - 1;
	LogBuffer* cursor = reinterpret_cast<LogBuffer*>(pages);
	for (size_t i = 0; i < itercnt; ++i)
		cursor[i].m_pNext = &cursor[i + 1];

	cursor[itercnt].m_pNext = nullptr;

	bufferLargePages.push_back(pages);

	return reinterpret_cast<LogBuffer*>(pages);
}

void zergengine::hrlog(const wchar_t* file, DWORD line, HRESULT hr)
{
	LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
	StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"File: %s, Line: %d. hr: 0x%X\n", file, line, hr);
	AsyncFileLogger::PushLog(pLogBuffer);
}

sysresult AsyncConsoleLogger::SysStartup(const EngineSystemInitDescriptor& desc)
{
	UNREFERENCED_PARAMETER(desc);

	InitializeSRWLock(&AsyncConsoleLogger::s_lock);

	AsyncConsoleLogger::s_worker = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, AsyncConsoleLogger::WorkerThreadEntry, nullptr, 0, nullptr));
	if (!AsyncConsoleLogger::s_worker)
		return -1;

	// Allocate log buffers
	AcquireSRWLockExclusive(&AsyncConsoleLogger::s_lock);
	AsyncConsoleLogger::s_pTop = AllocLogBufferList(AsyncConsoleLogger::s_bufferLargePages);
	ReleaseSRWLockExclusive(&AsyncConsoleLogger::s_lock);

	return 0;
}

void AsyncConsoleLogger::SysCleanup()
{
	if (AsyncConsoleLogger::s_worker)
	{
		InterlockedExchange(&AsyncConsoleLogger::s_exit, TRUE);
		QueueUserAPC(DoNothing, AsyncConsoleLogger::s_worker, NULL);
		WaitForSingleObject(AsyncConsoleLogger::s_worker, INFINITE);
		CloseHandle(AsyncConsoleLogger::s_worker);
		AsyncConsoleLogger::s_worker = NULL;
	}

	AcquireSRWLockExclusive(&AsyncConsoleLogger::s_lock);
	for (auto pages : AsyncConsoleLogger::s_bufferLargePages)
		VirtualFree(pages, 0, MEM_RELEASE);		// Free all log buffer pages

	AsyncConsoleLogger::s_pTop = nullptr;
	ReleaseSRWLockExclusive(&AsyncConsoleLogger::s_lock);
}

LogBuffer* AsyncConsoleLogger::GetLogBuffer()
{
	AcquireSRWLockExclusive(&AsyncConsoleLogger::s_lock);
	if (AsyncConsoleLogger::s_pTop == nullptr)
		AsyncConsoleLogger::s_pTop = AllocLogBufferList(AsyncConsoleLogger::s_bufferLargePages);

	LogBuffer* pReturn = AsyncConsoleLogger::s_pTop;
	AsyncConsoleLogger::s_pTop = pReturn->m_pNext;
	pReturn->m_pNext = nullptr;

	ReleaseSRWLockExclusive(&AsyncConsoleLogger::s_lock);

	return pReturn;
}

unsigned int __stdcall AsyncConsoleLogger::WorkerThreadEntry(void* arg)
{
	while (!AsyncConsoleLogger::GetExitFlag())
		SleepEx(INFINITE, TRUE);

	return 0;
}

VOID AsyncConsoleLogger::JobProc(ULONG_PTR parameter)
{
	LogBuffer* pLogBuffer = reinterpret_cast<LogBuffer*>(parameter);
	wprintf(pLogBuffer->buffer);

	AcquireSRWLockExclusive(&AsyncConsoleLogger::s_lock);
	pLogBuffer->m_pNext = AsyncConsoleLogger::s_pTop;
	AsyncConsoleLogger::s_pTop = pLogBuffer;
	ReleaseSRWLockExclusive(&AsyncConsoleLogger::s_lock);
}

sysresult AsyncFileLogger::SysStartup(const EngineSystemInitDescriptor& desc)
{
	UNREFERENCED_PARAMETER(desc);

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(now);
	tm tm;
	localtime_s(&tm, &time);

	wchar_t filename[64];
	StringCbPrintfW(filename, sizeof(filename),
		L"%d%02d%02d_%02d%02d%02d.log",
		1900 + tm.tm_year,
		1 + tm.tm_mon,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec
	);

	errno_t e = _wfopen_s(&AsyncFileLogger::s_pFile, filename, L"wt");
	if (e != 0)
		return -1;

	SetUnhandledExceptionFilter(AsyncFileLogger::FlushLog);

	InitializeSRWLock(&AsyncFileLogger::s_lock);

	AsyncFileLogger::s_worker = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, AsyncFileLogger::WorkerThreadEntry, nullptr, 0, nullptr));
	if (!AsyncFileLogger::s_worker)
		return -1;

	// Allocate log buffers
	AcquireSRWLockExclusive(&AsyncFileLogger::s_lock);
	AsyncFileLogger::s_pTop = AllocLogBufferList(AsyncFileLogger::s_bufferLargePages);
	ReleaseSRWLockExclusive(&AsyncFileLogger::s_lock);

	return 0;
}

void AsyncFileLogger::SysCleanup()
{
	if (AsyncFileLogger::s_pFile)
	{
		fclose(AsyncFileLogger::s_pFile);
		AsyncFileLogger::s_pFile = nullptr;
	}

	if (AsyncFileLogger::s_worker)
	{
		InterlockedExchange(&AsyncFileLogger::s_exit, TRUE);
		QueueUserAPC(DoNothing, AsyncFileLogger::s_worker, NULL);
		WaitForSingleObject(AsyncFileLogger::s_worker, INFINITE);
		CloseHandle(AsyncFileLogger::s_worker);
		AsyncFileLogger::s_worker = NULL;
	}

	AcquireSRWLockExclusive(&AsyncFileLogger::s_lock);
	for (auto pages : AsyncFileLogger::s_bufferLargePages)
		VirtualFree(pages, 0, MEM_RELEASE);		// Free all log buffer pages

	AsyncFileLogger::s_pTop = nullptr;
	ReleaseSRWLockExclusive(&AsyncFileLogger::s_lock);
}

LogBuffer* AsyncFileLogger::GetLogBuffer()
{
	AcquireSRWLockExclusive(&AsyncFileLogger::s_lock);
	if (AsyncFileLogger::s_pTop == nullptr)
		AsyncFileLogger::s_pTop = AllocLogBufferList(AsyncFileLogger::s_bufferLargePages);

	LogBuffer* pReturn = AsyncFileLogger::s_pTop;
	AsyncFileLogger::s_pTop = pReturn->m_pNext;
	pReturn->m_pNext = nullptr;

	ReleaseSRWLockExclusive(&AsyncFileLogger::s_lock);

	return pReturn;
}

unsigned int __stdcall AsyncFileLogger::WorkerThreadEntry(void* arg)
{
	while (!AsyncFileLogger::GetExitFlag())
		SleepEx(INFINITE, TRUE);

	return 0;
}

VOID AsyncFileLogger::JobProc(ULONG_PTR parameter)
{
	LogBuffer* pLogBuffer = reinterpret_cast<LogBuffer*>(parameter);
	fwprintf(AsyncFileLogger::s_pFile, pLogBuffer->buffer);

	++AsyncFileLogger::s_flushTimer;
	if (AsyncFileLogger::s_flushTimer & 0x00000010)
	{
		fflush(AsyncFileLogger::s_pFile);
		AsyncFileLogger::s_flushTimer = 0;
	}

	AcquireSRWLockExclusive(&AsyncFileLogger::s_lock);
	pLogBuffer->m_pNext = AsyncFileLogger::s_pTop;
	AsyncFileLogger::s_pTop = pLogBuffer;
	ReleaseSRWLockExclusive(&AsyncFileLogger::s_lock);
}

LONG CALLBACK AsyncFileLogger::FlushLog(PEXCEPTION_POINTERS pExceptionPointer)
{
	if (AsyncFileLogger::s_pFile)
	{
		fclose(AsyncFileLogger::s_pFile);
		AsyncFileLogger::s_pFile = nullptr;
	}
	
	return 0;
}

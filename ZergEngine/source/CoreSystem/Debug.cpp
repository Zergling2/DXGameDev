#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>
#include <ZergEngine\CoreSystem\SystemInfo.h>
#include <ZergEngine\Common\EngineHelper.h>

namespace ze
{
	SyncFileLogger FileLog;
}

using namespace ze;

static VOID NTAPI WakeUpThread(ULONG_PTR parameter)
{
	// Do nothing.
}

void Debug::ForceCrashWithMessageBox(PCWSTR title, PCWSTR format, ...)
{
	WCHAR buffer[256];

	va_list args;
	va_start(args, format);
	vswprintf_s(buffer, _countof(buffer), format, args);
	va_end(args);

	MessageBoxW(NULL, buffer, title, MB_OK);
	*reinterpret_cast<int*>(0) = 0;
}

void Debug::ForceCrashWithWin32ErrorMessageBox(PCWSTR message, DWORD code)
{
	WCHAR buffer[384];
	WCHAR em[128];	// FormatMessage buffer

	DWORD result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, code, 0, em, static_cast<DWORD>(sizeof(em)), nullptr);
	if (result == 0)
		StringCbPrintfW(buffer, sizeof(buffer), L"%s\nException from unknown win32 error: %d", message, code);
	else
		StringCbPrintfW(buffer, sizeof(buffer), L"%s\nException from win32 error: %d\n%s", message, code, em);

	MessageBoxW(NULL, buffer, L"Error", MB_OK);
	*reinterpret_cast<int*>(0) = 0;
}

void Debug::ForceCrashWithHRESULTErrorMessageBox(PCWSTR message, HRESULT hr)
{
	WCHAR buffer[256];
	StringCbPrintfW(buffer, sizeof(buffer), L"%s\nException from HRESULT: 0x%x", message, hr);

	MessageBoxW(NULL, buffer, L"Error", MB_OK);
	*reinterpret_cast<int*>(0) = 0;
}

AsyncConsoleLogger::AsyncConsoleLogger()
	: m_init(false)
	, m_exit(FALSE)
	, m_lock{}
	, m_hWorker(NULL)
	, m_pTop(nullptr)
	, m_bufferLargePages()
{
}

AsyncConsoleLogger::~AsyncConsoleLogger()
{
	this->Release();
}

bool AsyncConsoleLogger::Init()
{
	if (m_init)
		this->Release();

	bool result = false;

	do
	{
		m_lock.Init();

		assert(m_hWorker == NULL);
		uintptr_t hWorker = _beginthreadex(nullptr, 0, AsyncConsoleLogger::WorkerThreadEntry, this, 0, nullptr);
		if (hWorker == static_cast<uintptr_t>(0) || hWorker == static_cast<uintptr_t>(-1))
			break;
		m_hWorker = reinterpret_cast<HANDLE>(hWorker);

		// Allocate log buffers
		assert(m_pTop == nullptr);
		m_pTop = this->AllocLogBufferList();

		result = true;
	} while (false);

	if (result == false)
		this->Release();
	else
		m_init = true;

	return result;
}

void AsyncConsoleLogger::Release()
{
	this->SafeReleaseWorkerThread();

	m_pTop = nullptr;
	// AcquireSRWLockExclusive(&m_lock);		// 스레드 종료 후이므로 불필요
	for (auto pages : m_bufferLargePages)
		VirtualFree(pages, 0, MEM_RELEASE);		// Free all log buffer pages
	// ReleaseSRWLockExclusive(&m_lock);

	m_init = false;
}

HRESULT AsyncConsoleLogger::Write(PCWSTR str)
{
	HRESULT hr;

	m_lock.AcquireLockExclusive();
	AsyncLogBuffer* pLogBuffer = this->GetLogBuffer();
	m_lock.ReleaseLockExclusive();

	StringCbCopyW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, str);

	// Windows Server 2003 and Windows XP: There are no error values defined for this function that can be retrieved by calling GetLastError.
	if (!QueueUserAPC(AsyncConsoleLogger::WriteProc, m_hWorker, reinterpret_cast<ULONG_PTR>(pLogBuffer)))
		hr = HRESULT_FROM_WIN32(GetLastError());
	else
		hr = S_OK;

	return hr;
}

HRESULT __cdecl AsyncConsoleLogger::WriteFormat(PCWSTR format, ...)
{
	return E_NOTIMPL;
}

AsyncLogBuffer* AsyncConsoleLogger::GetLogBuffer()
{
	m_lock.AcquireLockExclusive();

	if (m_pTop == nullptr)
		m_pTop = this->AllocLogBufferList();

	AsyncLogBuffer* pReturn = m_pTop;
	m_pTop = pReturn->m_pNext;
	pReturn->m_pNext = nullptr;

	m_lock.ReleaseLockExclusive();

	return pReturn;
}

AsyncLogBuffer* AsyncConsoleLogger::AllocLogBufferList()
{
	// Protected by SRWLOCK
	LPVOID pages = MemoryAllocator.RequestSystemAllocGranularitySize();
	if (pages == nullptr)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"MemoryAllocatorImpl::RequestSystemAllocGranularitySize()", GetLastError());

	// 할당한 페이지들 전체를 연결한다.
	const size_t itercnt = SystemInfo.GetSystemInfo().dwAllocationGranularity / sizeof(AsyncLogBuffer) - 1;
	AsyncLogBuffer* cursor = reinterpret_cast<AsyncLogBuffer*>(pages);
	for (size_t i = 0; i < itercnt; ++i)
	{
		cursor[i].m_pInstance = this;
		cursor[i].m_pNext = &cursor[i + 1];
	}

	cursor[itercnt].m_pInstance = this;
	cursor[itercnt].m_pNext = nullptr;

	// 바깥 함수에서 SRWLOCK 잠근 상태이므로 필요 없음
	m_bufferLargePages.push_back(pages);

	return reinterpret_cast<AsyncLogBuffer*>(pages);
}

unsigned int __stdcall AsyncConsoleLogger::WorkerThreadEntry(void* arg)
{
	AsyncConsoleLogger* pInstance = reinterpret_cast<AsyncConsoleLogger*>(arg);

	while (!pInstance->GetExitFlag())
		SleepEx(INFINITE, TRUE);

	return 0;
}

VOID AsyncConsoleLogger::WriteProc(ULONG_PTR parameter)
{
	AsyncLogBuffer* pLogBuffer = reinterpret_cast<AsyncLogBuffer*>(parameter);
	fputws(pLogBuffer->buffer, stdout);

	AsyncConsoleLogger* pInstance = reinterpret_cast<AsyncConsoleLogger*>(pLogBuffer->m_pInstance);

	pInstance->m_lock.AcquireLockExclusive();
	pLogBuffer->m_pNext = pInstance->m_pTop;
	pInstance->m_pTop = pLogBuffer;
	pInstance->m_lock.ReleaseLockExclusive();
}

void AsyncConsoleLogger::SafeReleaseWorkerThread()
{
	if (m_hWorker)
	{
		InterlockedExchange(&m_exit, TRUE);
		QueueUserAPC(WakeUpThread, m_hWorker, NULL);
		WaitForSingleObject(m_hWorker, INFINITE);
		CloseHandle(m_hWorker);
		m_hWorker = NULL;
	}
}

SyncFileLogger::SyncFileLogger()
	: m_init(false)
	, m_pFile(NULL)
{
}

SyncFileLogger::~SyncFileLogger()
{
	this->Release();
}

bool SyncFileLogger::Init(PCWSTR fileName)
{
	if (m_init)
		this->Release();

	assert(m_pFile == NULL);
	errno_t e = _wfopen_s(&m_pFile, fileName, L"wt");
	if (e != 0)
		return false;

	m_init = true;

	return true;
}

void SyncFileLogger::Release()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	m_init = false;
}

HRESULT SyncFileLogger::Write(PCWSTR str)
{
	HRESULT hr;

	if (fputws(str, m_pFile) == EOF)
		hr = E_FAIL;
	else
	{
		fflush(m_pFile);
		hr = S_OK;
	}

	return hr;
}

HRESULT __cdecl ze::SyncFileLogger::WriteFormat(PCWSTR format, ...)
{
	HRESULT hr;

	va_list args;
	va_start(args, format);
	if (vfwprintf_s(m_pFile, format, args) < 0)
		hr = E_FAIL;
	else
		hr = S_OK;

	va_end(args);

	return hr;
}

AsyncFileLogger::AsyncFileLogger()
	: m_init(false)
	, m_exit(FALSE)
	, m_pFile(NULL)
	, m_lock{}
	, m_hWorker(NULL)
	, m_pTop(nullptr)
	, m_flushTimer(0)
	, m_bufferLargePages()
{
}

AsyncFileLogger::~AsyncFileLogger()
{
	this->Release();
}

bool AsyncFileLogger::Init(PCWSTR fileName)
{
	if (m_init)
		this->Release();

	bool result = false;

	do
	{
		m_lock.Init();

		assert(m_pFile == NULL);
		errno_t e = _wfopen_s(&m_pFile, fileName, L"wt");
		if (e != 0)
			break;

		// 비동기 로그 스레드 생성
		assert(m_hWorker == NULL);
		uintptr_t hWorker = _beginthreadex(nullptr, 0, AsyncFileLogger::WorkerThreadEntry, this, 0, nullptr);
		if (hWorker == static_cast<uintptr_t>(0) || hWorker == static_cast<uintptr_t>(-1))
			break;
		m_hWorker = reinterpret_cast<HANDLE>(hWorker);

		// Allocate log buffers
		assert(m_pTop == nullptr);
		m_pTop = this->AllocLogBufferList();

		result = true;
	} while (false);

	if (result == false)
		this->Release();
	else
		m_init = true;

	return result;
}

void AsyncFileLogger::Release()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	this->SafeReleaseWorkerThread();

	m_pTop = nullptr;
	// AcquireSRWLockExclusive(&m_lock);		// 스레드 종료 후이므로 불필요
	for (auto pages : m_bufferLargePages)
		VirtualFree(pages, 0, MEM_RELEASE);		// Free all log buffer pages
	// ReleaseSRWLockExclusive(&m_lock);

	m_init = false;
}

HRESULT AsyncFileLogger::Write(PCWSTR str)
{
	HRESULT hr;

	m_lock.AcquireLockExclusive();
	AsyncLogBuffer* pLogBuffer = this->GetLogBuffer();
	m_lock.ReleaseLockExclusive();

	StringCbCopyW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, str);

	// Windows Server 2003 and Windows XP: There are no error values defined for this function that can be retrieved by calling GetLastError.
	if (!QueueUserAPC(AsyncFileLogger::WriteProc, m_hWorker, reinterpret_cast<ULONG_PTR>(pLogBuffer)))
		hr = HRESULT_FROM_WIN32(GetLastError());
	else
		hr = S_OK;

	return hr;
}

HRESULT AsyncFileLogger::WriteFormat(PCWSTR format, ...)
{
	HRESULT hr;

	m_lock.AcquireLockExclusive();
	AsyncLogBuffer* pLogBuffer = this->GetLogBuffer();
	m_lock.ReleaseLockExclusive();

	va_list args;
	va_start(args, format);
	vswprintf_s(pLogBuffer->buffer, _countof(AsyncLogBuffer::buffer), format, args);
	va_end(args);

	// Windows Server 2003 and Windows XP: There are no error values defined for this function that can be retrieved by calling GetLastError.
	if (!QueueUserAPC(AsyncFileLogger::WriteProc, m_hWorker, reinterpret_cast<ULONG_PTR>(pLogBuffer)))
		hr = HRESULT_FROM_WIN32(GetLastError());
	else
		hr = S_OK;

	return hr;
}

AsyncLogBuffer* AsyncFileLogger::GetLogBuffer()
{
	m_lock.AcquireLockExclusive();

	if (m_pTop == nullptr)
		m_pTop = this->AllocLogBufferList();

	AsyncLogBuffer* pReturn = m_pTop;
	m_pTop = pReturn->m_pNext;
	pReturn->m_pNext = nullptr;

	m_lock.ReleaseLockExclusive();

	return pReturn;
}

AsyncLogBuffer* AsyncFileLogger::AllocLogBufferList()
{
	// Protected by SRWLOCK
	LPVOID pages = MemoryAllocator.RequestSystemAllocGranularitySize();
	if (pages == nullptr)
		return nullptr;

	// 할당한 페이지들 전체를 연결한다.
	const size_t itercnt = SystemInfo.GetSystemInfo().dwAllocationGranularity / sizeof(AsyncLogBuffer) - 1;
	AsyncLogBuffer* cursor = reinterpret_cast<AsyncLogBuffer*>(pages);
	for (size_t i = 0; i < itercnt; ++i)
	{
		cursor[i].m_pInstance = this;
		cursor[i].m_pNext = &cursor[i + 1];
	}

	cursor[itercnt].m_pInstance = this;
	cursor[itercnt].m_pNext = nullptr;

	// 바깥 함수에서 SRWLOCK 잠근 상태이므로 필요 없음
	m_bufferLargePages.push_back(pages);

	return reinterpret_cast<AsyncLogBuffer*>(pages);
}

UINT __stdcall AsyncFileLogger::WorkerThreadEntry(void* arg)
{
	AsyncFileLogger* pInstance = reinterpret_cast<AsyncFileLogger*>(arg);

	while (!pInstance->GetExitFlag())
		SleepEx(INFINITE, TRUE);

	return 0;
}

VOID AsyncFileLogger::WriteProc(ULONG_PTR parameter)
{
	AsyncLogBuffer* pLogBuffer = reinterpret_cast<AsyncLogBuffer*>(parameter);
	AsyncFileLogger* pInstance = reinterpret_cast<AsyncFileLogger*>(pLogBuffer->m_pInstance);

	fputws(pLogBuffer->buffer, pInstance->m_pFile);

	/*
	++pInstance->m_flushTimer;
	if (pInstance->m_flushTimer & 0x00000001)
	{
		fflush(pInstance->m_pFile);
		pInstance->m_flushTimer = 0;
	}
	*/
	fflush(pInstance->m_pFile);

	pInstance->m_lock.AcquireLockExclusive();
	pLogBuffer->m_pNext = pInstance->m_pTop;
	pInstance->m_pTop = pLogBuffer;
	pInstance->m_lock.ReleaseLockExclusive();
}

void AsyncFileLogger::SafeReleaseWorkerThread()
{
	if (m_hWorker)
	{
		InterlockedExchange(&m_exit, TRUE);
		QueueUserAPC(WakeUpThread, m_hWorker, NULL);
		WaitForSingleObject(m_hWorker, INFINITE);
		CloseHandle(m_hWorker);
		m_hWorker = NULL;
	}
}

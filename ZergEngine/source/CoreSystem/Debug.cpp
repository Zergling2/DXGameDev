#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\MemoryAllocator.h>

using namespace ze;

const wchar_t* ze::HRESULT_ERROR_LOG_FMT = L"%s failed. HRESULT: 0x%x\n";

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

void Debug::ForceCrashWithMessageBox(PCSTR title, PCSTR format, ...)
{
	CHAR buffer[256];

	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, _countof(buffer), format, args);
	va_end(args);

	MessageBoxA(NULL, buffer, title, MB_OK);
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

void Debug::HRESULTMessageBox(PCWSTR message, HRESULT hr)
{
	WCHAR buffer[256];
	StringCbPrintfW(buffer, sizeof(buffer), L"%s\nException from HRESULT: 0x%x", message, hr);

	MessageBoxW(NULL, buffer, L"Error", MB_OK);
}

void Debug::ForceCrashWithHRESULTMessageBox(PCWSTR message, HRESULT hr)
{
	HRESULTMessageBox(message, hr);
	*reinterpret_cast<int*>(0) = 0;
}

PCTSTR Debug::DXGIFormatToString(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
		case DXGI_FORMAT_UNKNOWN:
			return _T("DXGI_FORMAT_UNKNOWN");
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			return _T("DXGI_FORMAT_R32G32B32A32_TYPELESS");
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return _T("DXGI_FORMAT_R32G32B32A32_FLOAT");
		case DXGI_FORMAT_R32G32B32A32_UINT:
			return _T("DXGI_FORMAT_R32G32B32A32_UINT");
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return _T("DXGI_FORMAT_R32G32B32A32_SINT");
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			return _T("DXGI_FORMAT_R32G32B32_TYPELESS");
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return _T("DXGI_FORMAT_R32G32B32_FLOAT");
		case DXGI_FORMAT_R32G32B32_UINT:
			return _T("DXGI_FORMAT_R32G32B32_UINT");
		case DXGI_FORMAT_R32G32B32_SINT:
			return _T("DXGI_FORMAT_R32G32B32_SINT");
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			return _T("DXGI_FORMAT_R16G16B16A16_TYPELESS");
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return _T("DXGI_FORMAT_R16G16B16A16_FLOAT");
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return _T("DXGI_FORMAT_R16G16B16A16_UNORM");
		case DXGI_FORMAT_R16G16B16A16_UINT:
			return _T("DXGI_FORMAT_R16G16B16A16_UINT");
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return _T("DXGI_FORMAT_R16G16B16A16_SNORM");
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return _T("DXGI_FORMAT_R16G16B16A16_SINT");
		case DXGI_FORMAT_R32G32_TYPELESS:
			return _T("DXGI_FORMAT_R32G32_TYPELESS");
		case DXGI_FORMAT_R32G32_FLOAT:
			return _T("DXGI_FORMAT_R32G32_FLOAT");
		case DXGI_FORMAT_R32G32_UINT:
			return _T("DXGI_FORMAT_R32G32_UINT");
		case DXGI_FORMAT_R32G32_SINT:
			return _T("DXGI_FORMAT_R32G32_SINT");
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			return _T("DXGI_FORMAT_R32G8X24_TYPELESS");
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			return _T("DXGI_FORMAT_D32_FLOAT_S8X24_UINT");
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return _T("DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS");
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return _T("DXGI_FORMAT_X32_TYPELESS_G8X24_UINT");
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			return _T("DXGI_FORMAT_R10G10B10A2_TYPELESS");
		case DXGI_FORMAT_R10G10B10A2_UNORM: 
			return _T("DXGI_FORMAT_R10G10B10A2_UNORM");
		case DXGI_FORMAT_R10G10B10A2_UINT:
			return _T("DXGI_FORMAT_R10G10B10A2_UINT");
		case DXGI_FORMAT_R11G11B10_FLOAT:
			return _T("DXGI_FORMAT_R11G11B10_FLOAT");
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return _T("DXGI_FORMAT_R8G8B8A8_TYPELESS");
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return _T("DXGI_FORMAT_R8G8B8A8_UNORM");
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return _T("DXGI_FORMAT_R8G8B8A8_UNORM_SRGB");
		case DXGI_FORMAT_R8G8B8A8_UINT:
			return _T("DXGI_FORMAT_R8G8B8A8_UINT");
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return _T("DXGI_FORMAT_R8G8B8A8_SNORM");
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return _T("DXGI_FORMAT_R8G8B8A8_SINT");
		case DXGI_FORMAT_R16G16_TYPELESS:
			return _T("DXGI_FORMAT_R16G16_TYPELESS");
		case DXGI_FORMAT_R16G16_FLOAT:
			return _T("DXGI_FORMAT_R16G16_FLOAT");
		case DXGI_FORMAT_R16G16_UNORM:
			return _T("DXGI_FORMAT_R16G16_UNORM");
		case DXGI_FORMAT_R16G16_UINT:
			return _T("DXGI_FORMAT_R16G16_UINT");
		case DXGI_FORMAT_R16G16_SNORM:
			return _T("DXGI_FORMAT_R16G16_SNORM");
		case DXGI_FORMAT_R16G16_SINT:
			return _T("DXGI_FORMAT_R16G16_SINT");
		case DXGI_FORMAT_R32_TYPELESS:
			return _T("DXGI_FORMAT_R32_TYPELESS");
		case DXGI_FORMAT_D32_FLOAT:
			return _T("DXGI_FORMAT_D32_FLOAT");
		case DXGI_FORMAT_R32_FLOAT:
			return _T("DXGI_FORMAT_R32_FLOAT");
		case DXGI_FORMAT_R32_UINT:
			return _T("DXGI_FORMAT_R32_UINT");
		case DXGI_FORMAT_R32_SINT:
			return _T("DXGI_FORMAT_R32_SINT");
		case DXGI_FORMAT_R24G8_TYPELESS:
			return _T("DXGI_FORMAT_R24G8_TYPELESS");
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return _T("DXGI_FORMAT_D24_UNORM_S8_UINT");
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			return _T("DXGI_FORMAT_R24_UNORM_X8_TYPELESS");
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return _T("DXGI_FORMAT_X24_TYPELESS_G8_UINT");
		case DXGI_FORMAT_R8G8_TYPELESS:
			return _T("DXGI_FORMAT_R8G8_TYPELESS");
		case DXGI_FORMAT_R8G8_UNORM:
			return _T("DXGI_FORMAT_R8G8_UNORM");
		case DXGI_FORMAT_R8G8_UINT:
			return _T("DXGI_FORMAT_R8G8_UINT");
		case DXGI_FORMAT_R8G8_SNORM:
			return _T("DXGI_FORMAT_R8G8_SNORM");
		case DXGI_FORMAT_R8G8_SINT:
			return _T("DXGI_FORMAT_R8G8_SINT");
		case DXGI_FORMAT_R16_TYPELESS:
			return _T("DXGI_FORMAT_R16_TYPELESS");
		case DXGI_FORMAT_R16_FLOAT:
			return _T("DXGI_FORMAT_R16_FLOAT");
		case DXGI_FORMAT_D16_UNORM:
			return _T("DXGI_FORMAT_D16_UNORM");
		case DXGI_FORMAT_R16_UNORM:
			return _T("DXGI_FORMAT_R16_UNORM");
		case DXGI_FORMAT_R16_UINT:
			return _T("DXGI_FORMAT_R16_UINT");
		case DXGI_FORMAT_R16_SNORM:
			return _T("DXGI_FORMAT_R16_SNORM");
		case DXGI_FORMAT_R16_SINT:
			return _T("DXGI_FORMAT_R16_SINT");
		case DXGI_FORMAT_R8_TYPELESS:
			return _T("DXGI_FORMAT_R8_TYPELESS");
		case DXGI_FORMAT_R8_UNORM:
			return _T("DXGI_FORMAT_R8_UNORM");
		case DXGI_FORMAT_R8_UINT:
			return _T("DXGI_FORMAT_R8_UINT");
		case DXGI_FORMAT_R8_SNORM:
			return _T("DXGI_FORMAT_R8_SNORM");
		case DXGI_FORMAT_R8_SINT:
			return _T("DXGI_FORMAT_R8_SINT");
		case DXGI_FORMAT_A8_UNORM:
			return _T("DXGI_FORMAT_A8_UNORM");
		case DXGI_FORMAT_R1_UNORM:
			return _T("DXGI_FORMAT_R1_UNORM");
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			return _T("DXGI_FORMAT_R9G9B9E5_SHAREDEXP");
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			return _T("DXGI_FORMAT_R8G8_B8G8_UNORM");
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return _T("DXGI_FORMAT_G8R8_G8B8_UNORM");
		case DXGI_FORMAT_BC1_TYPELESS:
			return _T("DXGI_FORMAT_BC1_TYPELESS");
		case DXGI_FORMAT_BC1_UNORM:
			return _T("DXGI_FORMAT_BC1_UNORM");
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return _T("DXGI_FORMAT_BC1_UNORM_SRGB");
		case DXGI_FORMAT_BC2_TYPELESS:
			return _T("DXGI_FORMAT_BC2_TYPELESS");
		case DXGI_FORMAT_BC2_UNORM:
			return _T("DXGI_FORMAT_BC2_UNORM");
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return _T("DXGI_FORMAT_BC2_UNORM_SRGB");
		case DXGI_FORMAT_BC3_TYPELESS:
			return _T("DXGI_FORMAT_BC3_TYPELESS");
		case DXGI_FORMAT_BC3_UNORM:
			return _T("DXGI_FORMAT_BC3_UNORM");
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return _T("DXGI_FORMAT_BC3_UNORM_SRGB");
		case DXGI_FORMAT_BC4_TYPELESS:
			return _T("DXGI_FORMAT_BC4_TYPELESS");
		case DXGI_FORMAT_BC4_UNORM:
			return _T("DXGI_FORMAT_BC4_UNORM");
		case DXGI_FORMAT_BC4_SNORM:
			return _T("DXGI_FORMAT_BC4_SNORM");
		case DXGI_FORMAT_BC5_TYPELESS:
			return _T("DXGI_FORMAT_BC5_TYPELESS");
		case DXGI_FORMAT_BC5_UNORM:
			return _T("DXGI_FORMAT_BC5_UNORM");
		case DXGI_FORMAT_BC5_SNORM:
			return _T("DXGI_FORMAT_BC5_SNORM");
		case DXGI_FORMAT_B5G6R5_UNORM:
			return _T("DXGI_FORMAT_B5G6R5_UNORM");
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return _T("DXGI_FORMAT_B5G5R5A1_UNORM");
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return _T("DXGI_FORMAT_B8G8R8A8_UNORM");
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return _T("DXGI_FORMAT_B8G8R8X8_UNORM");
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return _T("DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM");
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			return _T("DXGI_FORMAT_B8G8R8A8_TYPELESS");
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return _T("DXGI_FORMAT_B8G8R8A8_UNORM_SRGB");
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			return _T("DXGI_FORMAT_B8G8R8X8_TYPELESS");
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return _T("DXGI_FORMAT_B8G8R8X8_UNORM_SRGB");
		case DXGI_FORMAT_BC6H_TYPELESS:
			return _T("DXGI_FORMAT_BC6H_TYPELESS");
		case DXGI_FORMAT_BC6H_UF16:
			return _T("DXGI_FORMAT_BC6H_UF16");
		case DXGI_FORMAT_BC6H_SF16:
			return _T("DXGI_FORMAT_BC6H_SF16");
		case DXGI_FORMAT_BC7_TYPELESS:
			return _T("DXGI_FORMAT_BC7_TYPELESS");
		case DXGI_FORMAT_BC7_UNORM:
			return _T("DXGI_FORMAT_BC7_UNORM");
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return _T("DXGI_FORMAT_BC7_UNORM_SRGB");
		case DXGI_FORMAT_AYUV:
			return _T("DXGI_FORMAT_AYUV");
		case DXGI_FORMAT_Y410:
			return _T("DXGI_FORMAT_Y410");
		case DXGI_FORMAT_Y416:
			return _T("DXGI_FORMAT_Y416");
		case DXGI_FORMAT_NV12:
			return _T("DXGI_FORMAT_NV12");
		case DXGI_FORMAT_P010:
			return _T("DXGI_FORMAT_P010");
		case DXGI_FORMAT_P016:
			return _T("DXGI_FORMAT_P016");
		case DXGI_FORMAT_420_OPAQUE:
			return _T("DXGI_FORMAT_420_OPAQUE");
		case DXGI_FORMAT_YUY2:
			return _T("DXGI_FORMAT_YUY2");
		case DXGI_FORMAT_Y210:
			return _T("DXGI_FORMAT_Y210");
		case DXGI_FORMAT_Y216:
			return _T("DXGI_FORMAT_Y216");
		case DXGI_FORMAT_NV11:
			return _T("DXGI_FORMAT_NV11");
		case DXGI_FORMAT_AI44:
			return _T("DXGI_FORMAT_AI44");
		case DXGI_FORMAT_IA44:
			return _T("DXGI_FORMAT_IA44");
		case DXGI_FORMAT_P8:
			return _T("DXGI_FORMAT_P8");
		case DXGI_FORMAT_A8P8:
			return _T("DXGI_FORMAT_A8P8");
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			return _T("DXGI_FORMAT_B4G4R4A4_UNORM");
		case DXGI_FORMAT_P208:
			return _T("DXGI_FORMAT_P208");
		case DXGI_FORMAT_V208:
			return _T("DXGI_FORMAT_V208");
		case DXGI_FORMAT_V408:
			return _T("DXGI_FORMAT_V408");
		case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
			return _T("DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE");
		case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
			return _T("DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE");
		case DXGI_FORMAT_A4B4G4R4_UNORM:
			return _T("DXGI_FORMAT_A4B4G4R4_UNORM");
		case DXGI_FORMAT_FORCE_UINT:
			return _T("DXGI_FORMAT_FORCE_UINT");
		default:
			return _T("Unknown");
	}
}

PCTSTR Debug::SRVDimensionToString(D3D11_SRV_DIMENSION d)
{
	switch (d)
	{
	case D3D11_SRV_DIMENSION_UNKNOWN:
		return _T("Unknown");
	case D3D11_SRV_DIMENSION_BUFFER:
		return _T("Buffer");
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		return _T("1D Texture");
	case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
		return _T("1D Texture Array");
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		return _T("2D Texture");
	case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
		return _T("2D Texture Array");
	case D3D11_SRV_DIMENSION_TEXTURE2DMS:
		return _T("2D MS Texture");
	case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
		return _T("2D MS Texture Array");
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		return _T("3D Texture");
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		return _T("Cube Texture");
	case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
		return _T("Cube Texture Array");
	case D3D11_SRV_DIMENSION_BUFFEREX:
		return _T("Buffer Ex");
	default:
		return _T("");
	}
}

PCTSTR Debug::VertexFormatToString(VertexFormatType type)
{
	switch (type)
	{
	case VertexFormatType::Position:
		return _T("Position");
	case VertexFormatType::PositionColor:
		return _T("Position\nColor");
	case VertexFormatType::PositionNormal:
		return _T("Position\nNormal");
	case VertexFormatType::PositionTexCoord:
		return _T("Position\nTexCoord");
	case VertexFormatType::PositionNormalTexCoord:
		return _T("Position\nNormal\nTexCoord");
	case VertexFormatType::TerrainPatchCtrlPt:
		return _T("TerrainPatchCtrlPt");
	case VertexFormatType::ButtonPt:
		return _T("ButtonPt");
	default:
		return _T("Unknown");
	}
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
	assert(MemoryAllocator::GetInstance()->GetSystemAllocationGranularity() > sizeof(AsyncLogBuffer));

	// Protected by SRWLOCK
	LPVOID pages = MemoryAllocator::GetInstance()->RequestSystemAllocGranularitySize();
	if (pages == nullptr)
		Debug::ForceCrashWithWin32ErrorMessageBox(L"MemoryAllocator::RequestSystemAllocGranularitySize()", GetLastError());

	// 할당한 페이지들 전체를 연결한다.
	const size_t itercnt = MemoryAllocator::GetInstance()->GetSystemAllocationGranularity() / sizeof(AsyncLogBuffer) - 1;
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
	assert(MemoryAllocator::GetInstance()->GetSystemAllocationGranularity() > sizeof(AsyncLogBuffer));

	// Protected by SRWLOCK
	LPVOID pages = MemoryAllocator::GetInstance()->RequestSystemAllocGranularitySize();
	if (pages == nullptr)
		return nullptr;

	// 할당한 페이지들 전체를 연결한다.
	const size_t itercnt = MemoryAllocator::GetInstance()->GetSystemAllocationGranularity() / sizeof(AsyncLogBuffer) - 1;
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

#include <ZergEngine\CoreSystem\Helper.h>
#include <vector>

using namespace ze;

int Helper::ConvertUTF8ToUTF16(const char* utf8Str, wchar_t* pBuffer, size_t cchWideChar)
{
	if (!utf8Str || !pBuffer || cchWideChar == 0)
		return 0;

	const int cchRequired = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8Str,
		-1,			// -1로 설정하면 sizeNeeded에 Null 문자까지 포함한 필요 사이즈를 리턴해준다.
		nullptr,	// 필요 사이즈를 얻기 위해서
		0			// 필요 사이즈를 얻기 위해서
	);

	std::vector<WCHAR> tb(cchRequired);

	int result = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8Str,
		-1,
		tb.data(),
		cchRequired
	);

	if (result > 0)
		StringCchCopyW(pBuffer, cchWideChar, tb.data());
	
	return result;
}

int Helper::ConvertUTF16ToUTF8(const wchar_t* utf16Str, char* pBuffer, size_t cbMultiByte)
{
	if (!utf16Str || !pBuffer || cbMultiByte == 0)
		return 0;

	const int cbRequired = WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16Str,
		-1,			// -1로 설정하면 sizeNeeded에 Null 문자까지 포함한 필요 사이즈를 리턴해준다.
		nullptr,	// 필요 사이즈를 얻기 위해서
		0,			// 필요 사이즈를 얻기 위해서
		NULL,		// CP_UTF8을 사용하면 반드시 NULL을 전달해야 함.
		NULL		// CP_UTF8을 사용하면 반드시 NULL을 전달해야 함.
	);

	std::vector<CHAR> tb(cbRequired);

	int result = WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16Str,
		-1,
		tb.data(),
		cbRequired,
		NULL,
		NULL
	);

	if (result > 0)
		StringCbCopyA(pBuffer, cbMultiByte, tb.data());

	return result;
}

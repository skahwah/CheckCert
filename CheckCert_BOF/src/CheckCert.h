#include <windows.h>
#include <stdio.h>
#include <winhttp.h>
#include <Wincrypt.h>

//#pragma comment(lib, "winhttp.lib")
//#pragma comment(lib, "crypt32.lib")

/*
#define MSVCRT$wcscpy_s wcscpy_s
*/

//MSVCRT
WINBASEAPI void* WINAPI MSVCRT$malloc(SIZE_T);
WINBASEAPI int __cdecl MSVCRT$strcmp(const char* _Str1, const char* _Str2);
WINBASEAPI SIZE_T WINAPI MSVCRT$strlen(const char* str);
WINBASEAPI errno_t __cdecl MSVCRT$mbstowcs_s(size_t* pReturnValue, wchar_t* wcstr, size_t sizeInWords, const char* mbstr, size_t count);
DECLSPEC_IMPORT int __cdecl MSVCRT$atoi(const char *str);
WINBASEAPI void* WINAPI MSVCRT$strtok(char* str, const char* delim);

//KERNEL32
WINBASEAPI HANDLE WINAPI KERNEL32$GetProcessHeap();
WINBASEAPI void* WINAPI KERNEL32$HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
WINBASEAPI BOOL WINAPI KERNEL32$HeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem);
WINBASEAPI FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

//winhttp.dll
typedef HINTERNET(WINAPI* WinHttpOpen_t)(
	IN LPCWSTR pszAgentW,
	IN DWORD dwAccessType,
	IN LPCWSTR pszProxyW,
	IN LPCWSTR pszProxyBypassW,
	IN DWORD dwFlags
	);

typedef BOOL(WINAPI* WinHttpSetTimeouts_t)(
 	IN HINTERNET hInternet,
 	IN int nResolveTimeout,
 	IN int nConnectTimeout,
 	IN int nSendTimeout,
 	IN int nReceiveTimeout
	);

typedef HINTERNET(WINAPI* WinHttpConnect_t)(
	IN HINTERNET hSession,
	IN LPCWSTR pswzServerName,
	IN INTERNET_PORT nServerPort,
	IN DWORD dwReserved
	);

typedef HINTERNET(WINAPI* WinHttpOpenRequest_t)(
	IN HINTERNET hConnect,
	IN LPCWSTR pwszVerb,
	IN LPCWSTR pwszObjectName,
	IN LPCWSTR pwszVersion,
	IN LPCWSTR pwszReferrer,
	IN LPCWSTR* ppwszAcceptTypes,
	IN DWORD dwFlags
	);

typedef BOOL(WINAPI* WinHttpSendRequest_t)(
	IN HINTERNET hRequest,
	IN LPCWSTR lpszHeaders,
	IN DWORD dwHeadersLength,
	IN LPVOID lpOptional,
	IN DWORD dwOptionalLength,
	IN DWORD dwTotalLength,
	IN DWORD_PTR dwContext
	);

typedef BOOL(WINAPI* WinHttpQueryOption_t)(
	IN HINTERNET hInternet,
	IN DWORD dwOption,
	OUT LPVOID lpBuffer,
	OUT LPDWORD lpdwBufferLength
	);

typedef BOOL(WINAPI* WinHttpCloseHandle_t)(
	IN HINTERNET hInternet
	);

//Crypt32.dll
typedef DWORD(WINAPI* CertNameToStrA_t)(
	IN  DWORD dwCertEncodingType,
	IN  PCERT_NAME_BLOB pName,
	IN  DWORD dwStrType,
	OUT LPSTR psz,
	IN  DWORD csz
	);

//Kernel32.dll
typedef BOOL(WINAPI* FileTimeToSystemTime_t)(
	IN const FILETIME* lpFileTime,
	OUT LPSYSTEMTIME lpSystemTime
	);
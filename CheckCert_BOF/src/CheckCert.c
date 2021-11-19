#include <windows.h>
#include "beacon.h"
#include "CheckCert.h"

//Initiate a web request
void HTTPRequest(LPCWSTR http, INTERNET_PORT port, LPCWSTR referrer, LPCWSTR agent) {

    //Variables
	BOOL hResults = FALSE;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    FILETIME expiryFt, effectiveDateFt;
	SYSTEMTIME expirySt, effectiveSt;
	CERT_CONTEXT* pCert = { 0 };
	DWORD dwLen, len;
	char* name = NULL;
	char* issuer = NULL;
	
	//winhttp - define pointers
	HINSTANCE hinst = LoadLibrary("winhttp.dll");
	WinHttpOpen_t pWinHttpOpen = (WinHttpOpen_t)GetProcAddress(hinst, "WinHttpOpen");
	WinHttpSetTimeouts_t pWinHttpSetTimeouts = (WinHttpSetTimeouts_t)GetProcAddress(hinst, "WinHttpSetTimeouts");
	WinHttpConnect_t pWinHttpConnect = (WinHttpConnect_t)GetProcAddress(hinst, "WinHttpConnect");
	WinHttpOpenRequest_t pWinHttpOpenRequest = (WinHttpOpenRequest_t)GetProcAddress(hinst, "WinHttpOpenRequest");
	WinHttpSendRequest_t pWinHttpSendRequest = (WinHttpSendRequest_t)GetProcAddress(hinst, "WinHttpSendRequest");
	WinHttpQueryOption_t pWinHttpQueryOption = (WinHttpQueryOption_t)GetProcAddress(hinst, "WinHttpQueryOption");
	WinHttpCloseHandle_t pWinHttpCloseHandle = (WinHttpCloseHandle_t)GetProcAddress(hinst, "WinHttpCloseHandle");

	//crypt32 - define pointers
	hinst = LoadLibrary("crypt32.dll");
	CertNameToStrA_t pCertNameToStrA = (CertNameToStrA_t)GetProcAddress(hinst, "CertNameToStrA");

	//kernel32 - define pointers
	hinst = LoadLibrary("Kernel32.dll");
	FileTimeToSystemTime_t pFileTimeToSystemTime = (FileTimeToSystemTime_t)GetProcAddress(hinst, "FileTimeToSystemTime");

	//Begin HTTP request
	//Obtain a session handle
	hSession = pWinHttpOpen(agent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	//Set reasonable timout values
	pWinHttpSetTimeouts(hSession, 2000, 2000, 2000, 2000);

	//Specify an HTTP server
	if (hSession)
		hConnect = pWinHttpConnect(hSession, http, port, 0);

	//Create an request handle
	if (hConnect)
		//WINHTTP_FLAG_SECURE makes secure connection
		hRequest = pWinHttpOpenRequest(hConnect, L"GET", referrer, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE); 
	else 
		BeaconPrintf(CALLBACK_OUTPUT,"[!] Failed to create HTTP request handle.\n");

	//Connect to the server by sending a request
	if (hRequest)
		hResults = pWinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	else
		BeaconPrintf(CALLBACK_OUTPUT,"[!] Cannot connect to server.\n");

	//Obtain the SSL certificate using WINHTTP_OPTION_SERVER_CERT_CONTEXT
	if (hResults)
		hResults = pWinHttpQueryOption(hRequest, WINHTTP_OPTION_SERVER_CERT_CONTEXT, &pCert, &dwLen);
	else
		BeaconPrintf(CALLBACK_OUTPUT,"[!] Unable to get SSL certificate.\n");
        
    //Begin parsing the SSL certificate context
	if (hResults) {
        //Parse the SSL certificate context and obtain the CNAME/subject
		len = pCertNameToStrA(X509_ASN_ENCODING, &pCert->pCertInfo->Subject, CERT_X500_NAME_STR, NULL, 0);
		if (len) {
			name = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), 0, len);
			if (name) {
				pCertNameToStrA(X509_ASN_ENCODING, &pCert->pCertInfo->Subject, CERT_X500_NAME_STR, name, len);
			}
		}

        //Parse the SSL certificate context and obtain the issuer of the SSL certificate
		len = pCertNameToStrA(X509_ASN_ENCODING, &pCert->pCertInfo->Issuer, CERT_X500_NAME_STR, NULL, 0);
		if (len) {
		    issuer = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), 0, len);
			if (issuer) {
			    pCertNameToStrA(X509_ASN_ENCODING, &pCert->pCertInfo->Issuer, CERT_X500_NAME_STR, issuer, len);
			}
		}
        
        //Parse the SSL certificate context and obtain the expiry date
		expiryFt = pCert->pCertInfo->NotAfter;

        //Parse the SSL certificate context and obtain the issue/effective date
		effectiveDateFt = pCert->pCertInfo->NotBefore;

        //Convert date to readable time
		pFileTimeToSystemTime(&expiryFt, &expirySt);
		pFileTimeToSystemTime(&effectiveDateFt, &effectiveSt);
	}
	
    //Name and issuer will be NULL if unable to obtain a SSL certificate
    if (name != NULL)
        BeaconPrintf(CALLBACK_OUTPUT,"\nName: %s\nIssuer: %s\nExpiration: %02d/%02d/%d\nEffective Date: %02d/%02d/%d\n", name, issuer, expirySt.wMonth, expirySt.wDay, expirySt.wYear, effectiveSt.wMonth, effectiveSt.wDay, effectiveSt.wYear);
	
    //Free memory
    KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, name);
	KERNEL32$HeapFree(KERNEL32$GetProcessHeap(), 0, issuer);
	
	//Close open handles
	if (hRequest) pWinHttpCloseHandle(hRequest);
	if (hConnect) pWinHttpCloseHandle(hConnect);
	if (hSession) pWinHttpCloseHandle(hSession);
}

//BOF Entry Point
void go(char* args, int length) {
	
    //Variables
    char * target = MSVCRT$strtok(args, ",");
    char* url = "/";		//set a default referrer of / as a referrer is required
    char* port = "443";		//set a default port of 443
    LPCWSTR agent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.69 Safari/537.36";

    while (target != NULL) {

		//Converts arg to wide char
		size_t convertedChars = 0;
		size_t wideSize =  MSVCRT$strlen(target) + 1;

		//Convert domain to wide char and store in http variable
		wchar_t* http = (wchar_t*) MSVCRT$malloc(wideSize * sizeof(wchar_t));
		MSVCRT$mbstowcs_s(&convertedChars, http, wideSize, target, _TRUNCATE);

		//Convert url to wide char and store in referrer variable
		wideSize = MSVCRT$strlen(url) + 1;
		wchar_t* referrer = (wchar_t*)MSVCRT$malloc(1 * sizeof(wchar_t));
		MSVCRT$mbstowcs_s(&convertedChars, referrer, wideSize, url, _TRUNCATE);
			 
		BeaconPrintf(CALLBACK_OUTPUT, "\n[+] Getting SSL certificate details for https://%s:%s%s\n", target, port, url);

		//Connect to web application over HTTPS and grab the certificate
		HTTPRequest(http, MSVCRT$atoi(port), referrer, agent);
		
        //Clear current target
        target = MSVCRT$strtok(NULL, ",");
	}
}
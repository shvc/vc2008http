// test_curl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <curl/curl.h>
#include <windows.h>
#include <iostream>
#include <fstream>

size_t libcurl_read_callback(void * pBuffer, size_t size, size_t nmemb, void * hFile)
{
	DWORD dwNumberOfBytesRead = 0;
	BOOL bResult = ReadFile((HANDLE) hFile, pBuffer, size * nmemb, &dwNumberOfBytesRead, NULL);
	return dwNumberOfBytesRead;
}

int libcurl_progress_callback (void * clientp, double dltotal, double dlnow,double ultotal, double ulnow)
{
	printf("Uploaded: %d / %d\n", (int) ulnow, (int) ultotal);
	return 0;
}

void UploadFile(char * strFileName, char * strFilePath)
{
	char strBuffer[1024];
	CURL * hCurl;
	CURLcode ccCurlResult = CURL_LAST;
	curl_off_t cotFileSize;
	HANDLE hFile;
	LARGE_INTEGER liFileSize;
	// check parameters
	if((strFileName == NULL || strlen(strFileName) == 0) ||
			(strFilePath == NULL || strlen(strFilePath) == 0))
		return;
	// parse file path
	if(strFilePath[strlen(strFilePath) - 1] == '\\')
		sprintf_s(strBuffer, 1024, "%s%s", strFilePath, strFileName);
	else
		sprintf_s(strBuffer, 1024, "%s\\%s", strFilePath, strFileName);
	// create a handle to the file
	hFile = CreateFileA(strBuffer, // file to open
			GENERIC_READ, // open for reading
			FILE_SHARE_READ, // share for reading
			NULL, // default security
			OPEN_EXISTING, // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL); // no attr. template

	if(hFile != INVALID_HANDLE_VALUE)
	{
		// global libcurl initialisation
		ccCurlResult = curl_global_init(CURL_GLOBAL_WIN32);
		if(ccCurlResult == 0)
		{
			// start libcurl easy session
			hCurl = curl_easy_init();
			if(hCurl)
			{
				// enable verbose operation
				curl_easy_setopt(hCurl, CURLOPT_VERBOSE, TRUE);
				// enable uploading
				curl_easy_setopt(hCurl, CURLOPT_UPLOAD, TRUE);
				// inform libcurl of the file's size
				GetFileSizeEx(hFile, &liFileSize);
				cotFileSize = liFileSize.QuadPart;
				curl_easy_setopt(hCurl, CURLOPT_INFILESIZE_LARGE,cotFileSize);
				// enable progress report function
				curl_easy_setopt(hCurl, CURLOPT_NOPROGRESS, FALSE);
				curl_easy_setopt(hCurl, CURLOPT_PROGRESSFUNCTION,libcurl_progress_callback);
				// use custom read function
				curl_easy_setopt(hCurl, CURLOPT_READFUNCTION, libcurl_read_callback);
				// specify which file to upload
				curl_easy_setopt(hCurl, CURLOPT_READDATA, hFile);
				// specify full path of uploaded file (i.e. server
				// address plus remote path)
				sprintf_s(strBuffer, 1024, "http://192.168.1.12:9000/open/%s", strFileName);
				curl_easy_setopt(hCurl, CURLOPT_URL, strBuffer);
				// execute command
				ccCurlResult = curl_easy_perform(hCurl);
				// end libcurl easy session
				curl_easy_cleanup(hCurl);
			}
		}
		// release file handle
		CloseHandle(hFile);
		// global libcurl cleanup
		curl_global_cleanup();
		if (ccCurlResult == CURLE_OK)
			printf("File uploaded successfully.\n");
		else
			printf("File upload failed. Curl error: %d\n", ccCurlResult);
	}
	else
		printf("File upload failed! Could not open local file");
}



size_t libcurl_read_stream(void * pBuffer, size_t size, size_t nmemb, void * stream)
{
	std::streampos begin,end;
	//DWORD dwNumberOfBytesRead = 88;
	std::ifstream *is = (std::ifstream*)stream;
	begin = is->tellg();
    is->read((char*)pBuffer, size*nmemb);
	//*is >> (char*)pBuffer;
	end = is->tellg();
	//BOOL bResult = ReadFile((HANDLE) hFile, pBuffer, size * nmemb, &dwNumberOfBytesRead, NULL);
	return end - begin;
	//return 88;
}
void UploadFileStream(char * strFileName, char * strFilePath)
{
	char strBuffer[1024];
	CURL * hCurl;
	CURLcode ccCurlResult = CURL_LAST;
	curl_off_t cotFileSize;
	//HANDLE hFile;
	LARGE_INTEGER liFileSize;
	// check parameters
	if((strFileName == NULL || strlen(strFileName) == 0) ||
			(strFilePath == NULL || strlen(strFilePath) == 0))
		return;
	// parse file path
	if(strFilePath[strlen(strFilePath) - 1] == '\\')
		sprintf_s(strBuffer, 1024, "%s%s", strFilePath, strFileName);
	else
		sprintf_s(strBuffer, 1024, "%s\\%s", strFilePath, strFileName);

	std::ifstream filestream(strBuffer);

	if(filestream.is_open())
	{
		// global libcurl initialisation
		ccCurlResult = curl_global_init(CURL_GLOBAL_WIN32);
		if(ccCurlResult == 0)
		{
			// start libcurl easy session
			hCurl = curl_easy_init();
			if(hCurl)
			{
				// enable verbose operation
				curl_easy_setopt(hCurl, CURLOPT_VERBOSE, TRUE);
				// enable uploading
				curl_easy_setopt(hCurl, CURLOPT_UPLOAD, TRUE);
				curl_easy_setopt(hCurl, CURLOPT_CUSTOMREQUEST, "PUT");
				struct curl_slist *headers = NULL;
				headers = curl_slist_append(headers, "Content-Type:application/octet-stream");
				curl_easy_setopt(hCurl, CURLOPT_HTTPHEADER, headers);
				// inform libcurl of the file's size
				//GetFileSizeEx(hFile, &liFileSize);
				cotFileSize = 3814;
				curl_easy_setopt(hCurl, CURLOPT_INFILESIZE_LARGE,cotFileSize);
				// enable progress report function
				//curl_easy_setopt(hCurl, CURLOPT_NOPROGRESS, FALSE);
				//curl_easy_setopt(hCurl, CURLOPT_PROGRESSFUNCTION,libcurl_progress_callback);
				// use custom read function
				curl_easy_setopt(hCurl, CURLOPT_READFUNCTION, libcurl_read_stream);
				// specify which file to upload
				curl_easy_setopt(hCurl, CURLOPT_READDATA, &filestream);
				// specify full path of uploaded file (i.e. server
				// address plus remote path)
				sprintf_s(strBuffer, 1024, "http://192.168.1.12:9000/open/%s", strFileName);
				curl_easy_setopt(hCurl, CURLOPT_URL, strBuffer);
				// execute command
				ccCurlResult = curl_easy_perform(hCurl);
				// end libcurl easy session
				curl_easy_cleanup(hCurl);
			}
		}
		// release file handle
		//CloseHandle(hFile);
		// global libcurl cleanup
		curl_global_cleanup();
		if (ccCurlResult == CURLE_OK)
			printf("File uploaded successfully.\n");
		else
			printf("File upload failed. Curl error: %d\n", ccCurlResult);
	}
	else
		printf("File upload failed! Could not open local file");
}


int _tmain(int argc, _TCHAR* argv[])
{
	UploadFileStream("test.txt", "C:\\");
	printf("Press any key to continue...");
	_getch();
	return 0;

}
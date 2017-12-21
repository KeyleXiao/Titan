#include "StdAfx.h"
#include "EncryptUtil.h"

#include <atlcomcli.h>
#include <atlcrypt.h>
#include <atlenc.h>


ENCRYPTUTIL_NAMESPACE_DEF_START

// Hex To lower char
inline const string HexToCharL(
	BYTE* pIn,
	u_long nSize )
{
	string strOut = "";
	do 
	{
		if ( NULL == pIn ||
			0 == nSize )
		{
			break;
		}

		for ( u_long i = 0; i<nSize; ++i)
		{
			char szBuf[16] = {0};
			sprintf_s(szBuf, "%02x", pIn[i]);
			strOut += szBuf;
		}
	}
	while (0);	
	return strOut;
}

// Hex To upper char
inline const string HexToCharU(
	BYTE* pIn,
	u_long nSize )
{
	string strOut = "";
	do 
	{
		if ( NULL == pIn ||
			 0 == nSize )
		{
			break;
		}

		for ( u_long i = 0; i<nSize; ++i)
		{
			char szBuf[16] = {0};
			sprintf_s(szBuf, "%02X", pIn[i]);
			strOut += szBuf;
		}
	}
	while (0);	
	return strOut;
}

bool GetMD5(
	const char* pIn,
	string& strOut )
{
	if ( NULL == pIn )
	{
		return false;
	}

	strOut = "";
	bool bRc = false;
	do 
	{
		CCryptProv prov;
		HRESULT hr = prov.Initialize(
			PROV_DSS,
			NULL,
            MS_DEF_DSS_PROV_W,
			CRYPT_VERIFYCONTEXT );
		if(FAILED(hr))
		{
			break;
		}

		CCryptMD5Hash hash;
		hr = hash.Initialize(prov);
		if(FAILED(hr))
		{
			break;
		}

#ifdef RKT_UNICODE
        const TCHAR* pWIn = reinterpret_cast<const TCHAR*>(pIn);
        hr = hash.AddString(pWIn);
#else
        hr = hash.AddString(pIn);
#endif
		if(FAILED(hr))
		{
			break;
		}

		const DWORD dwMaxLen = 16;
		BYTE szMD5[dwMaxLen] = {0};
		DWORD dwLen = dwMaxLen;
		hr = hash.GetValue(szMD5 , &dwLen);
		if( FAILED(hr) )
		{
			break;
		}

		strOut = HexToCharL((BYTE*)szMD5, dwMaxLen);

		hash.Uninitialize();
		prov.Uninitialize();

		bRc = true;
	}
	while (0);

	return bRc;
}

bool Base64Encode(
	char* pSrc,
	string& strDes )
{
	if ( NULL == pSrc ||
		 '\0' == pSrc[0] )
	{
		return false;
	}
	
	size_t nSrcLen = strlen(pSrc);
	char* pBase64Buf = new char[nSrcLen+1];
	ZeroMemory(pBase64Buf, nSrcLen+1);
	BOOL bRc = ATL::Base64Encode(
		(BYTE*)pSrc, nSrcLen, pBase64Buf, (int*)&nSrcLen);
	if ( !bRc )
	{
		delete[] pBase64Buf;
		return false;
	}

	strDes = pBase64Buf;
	delete[] pBase64Buf;
	return true;
}

ENCRYPTUTIL_NAMESPACE_DEF_END
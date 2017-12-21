/********************************************************************
	created:	2009/07/06
	created:	6:7:2009   17:18
	filename: 	e:\Rocket\Server\LuaServer\Build\EncryptUtil.h
	file path:	e:\Rocket\Server\LuaServer\Build
	file base:	EncryptUtil
	file ext:	h
	author:		heliang
	
	purpose:	
*********************************************************************/
#pragma once

#include <string>
using namespace std;

namespace EncryptUtil
{
	typedef unsigned long u_long;

	// Hex To lower char
	inline const string HexToCharL(
		BYTE* pIn,
		u_long nSize );

	// Hex To upper char
	inline const string HexToCharU(
		BYTE* pIn,
		u_long nSize );

	bool GetMD5(
		const char* pIn,
		string& strOut );

	inline bool Base64Encode(
		char* pSrc,
		string& strDes );

};


#define ENCRYPTUTIL_NAMESPACE_DEF_START namespace EncryptUtil {
#define ENCRYPTUTIL_NAMESPACE_DEF_END  }

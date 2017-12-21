/*******************************************************************
** 文件名:	md5.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/19/2017
** 版  本:	1.0
** 描  述:	UTF8版本的MD5加密
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __MD5_H__
#define __MD5_H__

namespace rkt{


#include <wincrypt.h>
#include <string>
using namespace std;


static inline string  utf8_md5(const char* pbContent)
{
    HCRYPTPROV hCryptProv; 
    HCRYPTHASH hHash; 
    BYTE bHash[0x7f]; 
    DWORD dwHashLen= 16; // The MD5 algorithm always returns 16 bytes. 
    DWORD cbContent= strlen(pbContent);

    string strResult = "";

    //if(CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
    if(CryptAcquireContext(&hCryptProv, NULL, 
#ifdef _UNICODE
        MS_DEF_DSS_PROV_W,
#else
        MS_DEF_DSS_PROV_A,
#endif
        PROV_DSS, CRYPT_VERIFYCONTEXT)) 
    {
        if(CryptCreateHash(hCryptProv, 
            CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
            0, 0, &hHash)) 
        {
            if(CryptHashData(hHash, (BYTE*)pbContent, cbContent, 0))
            {
                if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0)) 
                {
                    // Make a string version of the numeric digest value
                    for (int i = 0; i<16; i++)
                    {
                        char szBuf[16] = {0};
                        sprintf_s(szBuf, "%02x", bHash[i]);
                        strResult += szBuf;
                    }

                }
                else{
                    strResult=("Error getting hash param"); 
                }
            }
            else {
                strResult="Error hashing data"; 
            }
        }
        else {
            strResult="Error creating hash"; 
        }
    }
    else {
        strResult=("Error acquiring context"); 
    }

    CryptDestroyHash(hHash); 
    CryptReleaseContext(hCryptProv, 0); 

    return strResult; 
}

};  // namespace rkt

#endif
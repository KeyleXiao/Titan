/*******************************************************************
** �ļ���:	md5.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	12/19/2017
** ��  ��:	1.0
** ��  ��:	UTF8�汾��MD5����
** Ӧ  ��:  

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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
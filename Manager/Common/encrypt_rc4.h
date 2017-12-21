// ===========================================================================
//	filename: 	encrypt_rc4.h
//
//	author:		CHENTAO
//
//	created:	2007/05/26
//	
//	purpose:	
//
// ===========================================================================

#ifndef  __encrypt_rc4_h__
#define  __encrypt_rc4_h__

#include "encryptor.h"

class encrypt_RC4 : public encrypt_algorithm 
{
public:
	virtual bool create_key( const unsigned char * data,ulong len,void ** key,ulong & key_len );

	virtual bool release_key( void * key );
	
	virtual bool decrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key );
	
	virtual bool encrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key);
};

// RC4 Encryption
//
struct RC4_Key_Struct{
	uint8 abyState[256];
	uint8 byX;
	uint8 byY;
};

RC4_Key_Struct* RC4CreateKey(const uchar* pachKeyData, ulong nLen, RC4_Key_Struct* key = NULL, bool bSkipDiscard = false);
void RC4Crypt(const uchar* pachIn, uchar* pachOut, ulong nLen, RC4_Key_Struct* key);

#endif// __encrypt_rc4_h__

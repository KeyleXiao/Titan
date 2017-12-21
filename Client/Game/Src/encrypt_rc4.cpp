// ===========================================================================
//	filename: 	encrypt_rc4.cpp
//
//	author:		CHENTAO
//
//	created:	2007/05/26
//	
//	purpose:	
//
// ===========================================================================

#include "stdafx.h"
#include "encrypt_rc4.h"

///////////////////////////////////////////////////////////////////////////////
/*************************** RC4加密算法 *************************************/
//////////////////////////////////////////////////////////////////////////


static void swap_byte (uint8* a, uint8* b)
{
	uint8 bySwap;
	bySwap = *a;
	*a = *b;
	*b = bySwap;
}

void RC4Crypt(const uchar* pachIn, uchar* pachOut, ulong nLen, RC4_Key_Struct* key){
	assert( key != NULL && nLen > 0 );
	if (key == NULL)
		return;
	
	uint8 byX = key->byX;;
	uint8 byY = key->byY;
	uint8* pabyState = &key->abyState[0];;
	uint8 byXorIndex;
	
	for (ulong i = 0; i < nLen; i++)
	{
		byX = (byX + 1) % 256;
		byY = (pabyState[byX] + byY) % 256;
		swap_byte(&pabyState[byX], &pabyState[byY]);
		byXorIndex = (pabyState[byX] + pabyState[byY]) % 256;
		
		if (pachIn != NULL)
			pachOut[i] = pachIn[i] ^ pabyState[byXorIndex];
    }
	key->byX = byX;
	key->byY = byY;
}

RC4_Key_Struct* RC4CreateKey(const uchar* pachKeyData, ulong nLen, RC4_Key_Struct* key, bool bSkipDiscard){
	uint8 index1;
	uint8 index2;
	uint8* pabyState;

	if (key == NULL)
		key = new RC4_Key_Struct;

	pabyState= &key->abyState[0];
	int i=0;
	for (i = 0; i < 256; i++)
		pabyState[i] = (uint8)i;

	key->byX = 0;
	key->byY = 0;
	index1 = 0;
	index2 = 0;
	for (i = 0; i < 256; i++){
		index2 = (pachKeyData[index1] + pabyState[i] + index2) % 256;
		swap_byte(&pabyState[i], &pabyState[index2]);
		index1 = (uint8)((index1 + 1) % nLen);
	}
	if (!bSkipDiscard)
		RC4Crypt(NULL, NULL, 1024, key);
	return key;
}


//////////////////////////////////////////////////////////////////////////
/****************************RC4加密接口 ********************************/
//////////////////////////////////////////////////////////////////////////

bool encrypt_RC4::create_key( const unsigned char * data,ulong len,void ** key,ulong & key_len )
{
	assert(key);

	RC4_Key_Struct * rc4_key = RC4CreateKey(data,len,0,false);
	*key = rc4_key;
	key_len = sizeof(RC4_Key_Struct);

	return true;
}

bool encrypt_RC4::release_key( void * key )
{
	if ( key==0)
		return false;
	delete key;
	return true;
}

bool encrypt_RC4::decrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key )
{
	RC4Crypt((const unsigned char *)in_buffer,(unsigned char *)in_buffer,in_len,(RC4_Key_Struct*)key);
	*out_buffer = in_buffer;
	out_len = in_len;
	return true;
}

bool encrypt_RC4::encrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key)
{
	RC4Crypt((const unsigned char *)in_buffer,(unsigned char *)in_buffer,in_len,(RC4_Key_Struct*)key);
	*out_buffer = in_buffer;
	out_len = in_len;
	return true;
}
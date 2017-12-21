// ===========================================================================
//	filename: 	encryptor.h
//
//	author:		CHENTAO
//
//	created:	2007/05/25
//	
//	purpose:	emule通讯协议加密接口
//
// ===========================================================================

#ifndef  __my_encryptor_interface_h__
#define  __my_encryptor_interface_h__
/*
#ifndef uint32
	typedef unsigned long   uint32;
#endif
#ifndef uint8
	typedef unsigned char   uint8;
#endif*/

//////////////////////////////////////////////////////////////////////////
/// 加密算法 (这里只能选用流加密算法)
struct encrypt_algorithm 
{
	/**
	@purpose    : 创建密钥
	@param	data: 用于创建密钥的数据
	@param	len : 用于创建密钥的数据的长度
	@param  key	: 返回密钥地址
	@param  key_len : 返回密钥长度
	@return		: 是否创建成功
	*/
	
	virtual bool create_key( const unsigned char * data,ulong len,void ** key,ulong & key_len ) = 0;

	/**
	@purpose    : 销毁密钥,释放内存
	@param	key	: 要释放的密钥 
	@return		:
	*/
	
	virtual bool release_key( void * key ) = 0;

	/**
	@purpose			: 将收到的数据解密
	@note				: 这个函数只能在握手之后才能调用
	@param	in_buffer	: 收到的数据 [输入]
	@param  in_len		: 收到的数据的长度 [输入]
	@param  out_buffer	: 解密后的数据 [输出] [这个指针指向的内存由加密类负责维护]
	@param  out_len		: 解密后的数据的长度 [输出]
	@param  key			: 解密密钥
	*/
	
	virtual bool decrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key ) =0;

	/**
	@purpose			: 加密需要发送的数据 
	@note				: 这个函数只能在握手之后才能调用
	@param	in_buffer	: 收到的数据 [输入]
	@param  in_len		: 收到的数据的长度 [输入]
	@param  out_buffer	: 加密后的数据 [输出] [这个指针指向的内存由加密类负责维护]
	@param  out_len		: 加密后的数据的长度 [输出]
	@param  key			: 加密密钥
	*/
	
	virtual bool encrypt( const char * in_buffer,ulong in_len,const char ** out_buffer,ulong &out_len,const void * key) = 0;

};

#endif// __emule_encryptor_h__

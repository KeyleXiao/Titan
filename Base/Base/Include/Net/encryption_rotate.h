// ===========================================================================
//	filename: 	encryption_rotate.h
//
//	author:		CHENTAO
//
//	created:	2007/12/03
//	
//	purpose:	
//
// ===========================================================================

#ifndef  __encrypt_rotate_h__
#define  __encrypt_rotate_h__

#include <map>

// ---------------------------------------------------------------------------
// Name	: 循环移位加密算法 [块加密]
// Desc : 选用4个字节的数据作为密钥
//        加密时:
//        用密钥对要加密的数据4字节4字节挨个异或
//        然后对异或过的数据进行移位操作,移位多少个字节根据密钥推断出来，最好每次都不一样       
//        现在的做法比较简单，依次取密钥中的字节进行移位 
//        效率:
//        初步测试这个算法比RC4快30倍左右
// ---------------------------------------------------------------------------

class RKT_EXPORT encryption_rotate
{
public:
	//////////////////////////////////////////////////////////////////////////
	virtual bool create_key( const unsigned char * data,unsigned len,void ** key,unsigned & key_len );
	
	virtual bool release_key( void * key );
	
	virtual bool decrypt( const char * in_buffer,unsigned in_len,const char ** out_buffer,unsigned &out_len,const void * key );
	
	virtual bool encrypt( const char * in_buffer,unsigned in_len,const char ** out_buffer,unsigned &out_len,const void * key);

	virtual void release(){ delete this; }

	void    alloc_buffer( unsigned len );

	encryption_rotate() : _encrypt_buffer(0),_buffer_len(0){}

	virtual ~encryption_rotate();

private:
	//////////////////////////////////////////////////////////////////////////
	// 密钥结构
	struct key_info
	{
		unsigned long   _key_id;
		unsigned char   _key_data[4];
	};

	typedef std::map<unsigned long,key_info>    key_map;
	key_map             _key_list;
	char *              _encrypt_buffer;
	unsigned            _buffer_len;
};

// ---------------------------------------------------------------------------
// Name	: 循环移位加密算法 [流加密] 
// Desc : 上面的实现有个缺憾就是不是流加密,有些应用场合不太方便,
//        这里再实现一个流加密版本
//        流加密有个问题
//        1.不能4字节4字节异或了，因为不能保证收齐4字节
//        2.要记录上一次加解密时的状态
//        效率:
//        初步测试这个算法比RC4快20倍左右
// ---------------------------------------------------------------------------

class RKT_EXPORT encryption_rotate_stream
{
public:
	//////////////////////////////////////////////////////////////////////////
	virtual bool create_key( const unsigned char * data,unsigned len,void ** key,unsigned long & key_len );
	
	virtual bool release_key( void * key );
	
	virtual bool decrypt( const char * in_buffer,unsigned in_len,const char ** out_buffer,unsigned long &out_len,const void * key );
	
	virtual bool encrypt( const char * in_buffer,unsigned in_len,const char ** out_buffer,unsigned long &out_len,const void * key);
	
	virtual void release(){ delete this; }

	void    alloc_buffer( unsigned len );
	
	encryption_rotate_stream() : _encrypt_buffer(0),_buffer_len(0){}
	
	virtual ~encryption_rotate_stream();
	
private:
	//////////////////////////////////////////////////////////////////////////
	// 密钥结构
	struct key_info
	{
		unsigned long   _key_id;
		unsigned char   _key_data[4];
		unsigned long   _encrypt_status;
		unsigned long   _decrypt_status;
	};
	
	typedef std::map<unsigned long,key_info>    key_map;
	key_map             _key_list;
	char *              _encrypt_buffer;
	unsigned            _buffer_len;
};

#endif// __encrypt_rotate_h__

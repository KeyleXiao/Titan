//==========================================================================
/**
* @file	  : encrypt.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-1   11:29
* purpose : 加密相关
*/
//==========================================================================

#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include "Common.h"

#ifdef SUPPORT_STREAM


/** 加（解）密类型*/
#define CRYPT_TYPE_EMPTY		0		///< 不进行加（解）密
#define CRYPT_TYPE_1			0x2
#define CRYPT_TYPE_2			0x4
#define CRYPT_TYPE_3			0x8
#define MAX_CRYPT_TYPE_COUNT	8		///< 最大支持8组加（解）密算法（包括用户注册的4组在内）

#define CRYPT_TYPE_MAP			CRYPT_TYPE_1	/// 字节映射加密类型，对加密后进行的压缩损害小
#define CRYPT_TYPE_XOR			CRYPT_TYPE_2	/// 复杂异或加密类型，对加密后进行的压缩损害大

namespace rkt {

	/**  加（解）密函数原型*/
	typedef bool (*CRYPT_FUNS) (uchar* buffer, ulong size, ulong key);

	/** 加密操作
	@param buffer 要加密的缓冲区
	@param size 缓冲区大小
	@param key 加（解）密钥匙
	@param mask 加（解）密类型MASK
	@return 成功返回true，否则返回false
	@note 内部不对缓冲区建立副本，直接在缓冲区上加密，如果加密失败，缓冲区数据可能是无效的
	@see decrypt CryptType
	*/
	RKT_API bool encrypt(uchar* buffer, ulong size, ulong key, ulong mask);


	/** 解密操作
	@param buffer 要解密的缓冲区
	@param size 缓冲区大小
	@param key 加（解）密钥匙
	@param mask 加（解）密类型MASK
	@return 成功返回true，否则返回false
	@note 内部不对缓冲区建立副本，直接在缓冲区上解密，如果解密失败，缓冲区数据可能是无效的
	@see encrypt CryptType
	*/
	RKT_API bool decrypt(uchar* buffer, ulong size, ulong key, ulong mask);


	/** 注册加解密算法
	@param encryptFuns 加密函数指针
	@param decryptFuns 解密函数指针
	@return 成功返回注册的加解密算法对的类型MASK（用户可以通过该MASK进行该加解密算法的解除），否则返回 0
	@note 如果算法是对称的，则加解密函数可以是同一个函数，注册失败可能是注册数已经达到了最大值
	@see unregisterCrypt
	*/
	RKT_API ulong registerCrypt(CRYPT_FUNS encryptFuns, CRYPT_FUNS decryptFuns);


	/** 反注册（解除）加解密算法
	@param mask 要解除的加解密算法类型MASK
	@return 成功返回true，否则返回false
	@see registerCrypt
	*/
	RKT_API bool unregisterCrypt(ulong mask);

	/** 获取加解密算法对的总数
	@return 加解密算法对的总数
	*/
	RKT_API int getCryptCount();

} // namespace

#endif // #ifdef SUPPORT_STREAM

#endif // __ENCRYPT_H__

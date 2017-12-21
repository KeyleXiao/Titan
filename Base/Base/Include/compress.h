//==========================================================================
/**
* @file	  : compress.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-1   11:29
* purpose : 压缩相关
*/
//==========================================================================

/**
	log: 2008.12.22 将mask机制改成了索引，不支持注册和反注册
*/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "Common.h"

#define SUPPORT_LZMA
#define SUPPORT_BZIP2

#ifdef SUPPORT_STREAM


/** （解）压缩类型*/
enum CompressType
{
	COMP_TYPE_EMPTY,			/// 不进行（解）压缩
	COMP_TYPE_LZMA,				/// lzma压缩
	COMP_TYPE_BZIP2,			/// bzip2压缩

	MAX_COMP_TYPE_COUNT,		/// 最大支持8组（解）压缩算法（包括用户可注册的4组在内）
};


namespace rkt {

	/**  （解）压缩函数原型*/
	typedef bool (*COMPRESS_FUNS)(uchar* outBuffer, ulong* outSize, const uchar* inBuffer, ulong inSize);

	/** 压缩操作
	@param outBuffer 压缩后的缓冲区（注意：压缩后的缓冲区不一定比压缩前小）
	@param outSize 传入进压缩缓冲区大小，传出压缩后的大小
	@param inBuffer 压缩前的缓冲区
	@param inSize 压缩前的大小
	@param mask （解）压缩类型MASK
	@return 成功返回true，否则返回false
	@note 输出缓冲区大小必须可靠，否则可能会失败
	@see decompress CompressType
	*/
	RKT_API bool docompress(uchar* outBuffer, ulong* outSize, const uchar* inBuffer, ulong inSize, ulong mask);


	/** 解压缩操作
	@param outBuffer 解压缩后的缓冲区（注意：压缩后的缓冲区不一定比压缩前小）
	@param outSize 解压缩后的大小
	@param inBuffer 解压缩前的缓冲区
	@param inSize 解压缩前的大小
	@param mask （解）压缩类型MASK
	@return 成功返回true，否则返回false
	@note 输出缓冲区大小必须可靠，否则可能会失败
	@see compress CompressType
	*/
	RKT_API bool decompress(uchar* outBuffer, ulong* outSize, const uchar* inBuffer, ulong inSize, ulong mask);


	/** 注册（解）压缩算法
	@param encompressFuns 压缩函数指针
	@param decompressFuns 解压缩函数指针
	@return 成功返回注册的（解）压缩算法对的类型MASK（用户可以通过该MASK进行该加解密算法的解除），否则返回 0
	@see unregisterCompress
	*/
	//RKT_API ulong registerCompress(COMPRESS_FUNS encompressFuns, COMPRESS_FUNS decompressFuns);


	/** 反注册（解除）（解）压缩算法
	@param mask 要解除的加解密算法类型MASK
	@return 成功返回true，否则返回false
	@see registerCompress
	*/
	//RKT_API bool unregisterCompress(ulong mask);

	/** 获取（解）压缩算法对的总数
	@return （解）压缩算法对的总数
	*/
	//RKT_API int getCompressCount();

} // namespace

#endif // #ifdef SUPPORT_STREAM

#endif // __COMPRESS_H__

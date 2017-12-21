//==========================================================================
/**
* @file	  : PkgStruct.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-11-1   10:49
* purpose : PKG结构定义
*/
//==========================================================================

#ifndef __PkgStruct_h__
#define __PkgStruct_h__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include "Trace.h"
#include "StringHash.h"
#include "Stream.h"
#include "encrypt.h"
#include "compress.h"

#include <string>
#include <vector>

namespace rkt {

	#define KEY_BLOCK_TABLE					"BLOCK_TABLE"	/// 块表加密key
	#define KEY_HASH_TABLE					"HASH_TABLE"	/// hash表加密key
	#define KEY_BLOCK_INFO					"BLOCK_INFO"	/// 块信息加密key
	#define KEY_BLOCK_BUFFER				"BLOCK_BUFFER"	/// 缓冲区加密key

	#define PKG_ID							'\x1AKPM'		/// pkg文件标识
	#define PKG_VER							'0002'			/// pkg文件版本号
	#define PKG_LIST_FILE_NAME				"(listfile)"	/// pkg内置的记录文件列表的文件名

	// 块标志定义
	#define BLOCK_EXISTS					0x00000001		/// 块是否存在
	#define BLOCK_UPDATED					0x00000002		/// 块是否更新过
	#define BLOCK_RENAME					0x00000004		/// 块是否重命名过
	#define BLOCK_ISPACKAGE					0x00000008		/// 块是否是另一个包文件
	#define BLOCK_SINGLE_UNIT				0x00000010		/// 块是否是保存为一个整理（可以保存为根据设定大小的连续内存块）

	#define BLOCK_COMPRESSED				0x00000F00		/// 块是否进行了压缩（支持叠加压缩）
	#define BLOCK_COMPRESS_PKWARE			0x00000100		/// pkware压缩
	#define BLOCK_COMPRESS_ZLIB				0x00000200		/// zlib压缩
	#define BLOCK_COMPRESS_HUFF				0x00000400		/// huff压缩
	#define BLOCK_COMPRESS_WAVE				0x00000800		/// wave压缩

	#define BLOCK_ENCRYPTED					0x0000F000		/// 块是否进行了加密（支持叠加加密）
	#define BLOCK_ENCRYPT_1					0x00001000		/// 加密算法1
	#define BLOCK_ENCRYPT_2					0x00002000		/// 加密算法2
	#define BLOCK_ENCRYPT_3					0x00004000		/// 加密算法3
	#define BLOCK_ENCRYPT_4					0x00008000		/// 加密算法4

	#define BLOCK_VER_MAJOR					0xFF000000		/// 块的主版本号
	#define BLOCK_VER_MINOR					0x00FF0000		/// 块的从版本号
	#define BLOCK_INIT_VER					0x01000000		/// 块的初始版本号

	enum PkgStreamType
	{
		STREAM_FILE	= 0,
		STREAM_DIR = 1,
	};

	#pragma pack (push)
	#pragma pack (4)

	/// 包文件头
	typedef struct tagPkgHeader // 40 bytes
	{
		ulong		id;			/// 标识
		ulong		ver;		/// 版本
		ulong		pkgSize;	/// 包大小
		//ulong		blocks;		/// 块数
		ulong		blockOff;	/// 第一个块的偏移
		ulong		htOff;		/// hash table偏移
		ulong		htSize;		/// hash table入口数
		ulong		btOff;		/// block table偏移
		ulong		btSize;		/// block table入口数
		ulong		lstOff;		/// listfile文件的偏移
		ulong		reserved;	/// 扩充保留
	} PkgHeader, *PPkgHeader;


	/// 块表结构
	typedef struct tagPkgBlock // 20 bytes
	{
		ulong		flags;		/// 块标志
		ulong		fOffset;	/// 在包中的偏移位置
		ulong		fSize;		/// 未压缩的文件大小
		ulong		cSize;		/// 压缩后的大小
		ulong		seek;		/// 加解密种子
		//ulong		prevIndex;
		//ulong		nextIndex;
	} PkgBlock, *PPkgBlock;


	/// 块信息头（一个文件初次创建只有一个块，在更新后可能存在多个块）
	typedef struct tagBlockInfo // 12 bytes
	{
		ulong		size;		/// 块大小（包含BlockInfo结构的大小 >=sizeof(BlockInfo)）
		ulong		prevPos;	/// 前一个块在包文件中的位置（0表示没有前一个块）
		ulong		nextPos;	/// 后一个块在包文件中的位置（0表示没有后一个块）
	} BlockInfo, *PBlockInfo;


	// hash key
	typedef struct tagPkgHashKey // 12 bytes
	{
		ulong	name;	/// 流名的基本hash值
		ulong	name1;	/// 流名的第一辅助hash值
		ulong	name2;	/// 流名的第二辅助hash值

		tagPkgHashKey() { }
		tagPkgHashKey(const char* str)
		{
			calcHash(str);
		}
		inline void calcHash(const char* str)
		{
			name = hashString(str, 0);
			name1 = hashString(str, 1);
			name2 = hashString(str, 2);
		}
		bool operator == (const tagPkgHashKey& hash)
		{
			return (name == hash.name && name1 == hash.name1 && name2 == hash.name2);
		}
		bool operator < (const tagPkgHashKey& hash)
		{
			if (name == hash.name)
				if (name1 == hash.name1)
					if (name2 == hash.name2)
					{
						WarningLn("tagPkgHashKey::operator < () error, name2==hash.name2");
						debugBreak;
						return false;
					}
					else return name2 < hash.name2;
				else return name1 < hash.name1;
			else return name < hash.name;
		}
	}PkgHashKey, *PPkgHashKey;


	// Hash入口
	typedef struct tagPkgHash // 16 bytes
	{
		PkgHashKey	key;		/// 流名称的hash值
		ulong		blkIndex;	/// 在Block表中的索引

		inline tagPkgHash() {}
		inline tagPkgHash(PkgHashKey _key) 
			: key(_key) {}
		inline tagPkgHash(PkgHashKey _key, ulong _index) 
			: key(_key), blkIndex(_index)	{}
	} PkgHash, *PPkgHash;

	struct EncodeFileStreamHeader
	{
		ulong id;
		ulong ver;
		ulong flags;
		ulong fsize;
	};

	enum {EncodeFileID='HSFE', EncodeFileVer=1};

	#pragma pack (pop)


	/// 包文件相关处理时的回调过程(如：cleanPackage("Data\\scp.mpk", CRYPT_TYPE_1, COMP_TYPE_LZMA);)
	typedef void (*PKGPROC)(const std::string& streamName, ulong curStream, ulong maxStreamCount, const PkgBlock* block);

	/** 包操作接口 */
	struct IPkgOperation
	{
		typedef std::vector<std::string>	FileList;
		typedef FileList::iterator			FileListPtr;
		typedef FileList::const_iterator	FileListCPtr;

		/** 创建一个包文件
		 @param filename 包文件的文件名
		 @return 成功返回true，否则返回false
		 */
		virtual bool createPackage(const char* filename) = 0;

		/** 将某个磁盘目录创建为一个包文件
		 @param dir 要打包的目录
		 @param filename 打包后的文件名
		 @param cryptMask 加密Mask
		 @param compressMask 压缩Mask
		 @return 成功返回true，否则返回false
		 */
		virtual bool createPackage(const char* dir, const char* filename, ulong cryptMask = CRYPT_TYPE_EMPTY, ulong compressMask = COMP_TYPE_EMPTY) = 0;

		/** 打开一个已存在的包文件
		 @param filename 要打开的包文件名
		 @return 成功返回true，否则返回false
		 */
		virtual bool openPackage(const char* filename) = 0;

		/** 扩展包到一个指定的目录
		 @param path 包所扩展的目标路径，path为NULL时，扩展到当前目录
		 @return 成功返回true，否则返回false
		 */
		virtual bool extractPackage(const char* path = NULL) = 0;

		/** 关闭包文件
		 @return 总是返回true
		 */
		virtual bool closePackage() = 0;

		/** 添加内存流到包文件
		 @param name 流在包中的名字
		 @param buffer 流的数据地址
		 @param size 流的数据大小
		 @param existReplace 当存在该流时是否替换
		 @return 成功返回true，否则返回false
		 */
		virtual bool addStream(const char* name, uchar* buffer, ulong size, bool existReplace = true, bool refresh = true) = 0;

		/** 添加文件流到包文件
		 @param name 流在包中的名字
		 @param path 文件流的路径
		 @param existReplace 当存在该流时是否替换
		 @param cryptMask 加密Mask
		 @param compressMask 压缩Mask
		 @return 成功返回true，否则返回false
		 */
		virtual bool addStream(const char* name, const char* path, bool existReplace = true, bool refresh = true) = 0;

		/** 查找包文件中指定的流
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @return 成功返回在包文件中的索引位置，否则返回-1
		 */
		virtual int findStream(const char* name) const = 0;

		/** 获取包文件中指定流的信息
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @reval 流的信息结构
		 */
		virtual const PPkgBlock getStreamInfo(const char* name) const = 0;

		/** 获取包文件中指定的流
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @retval 指定的流对象
		 */
		virtual Stream* openStream(const char* name) = 0;

		/** 扩展指定的流到指定的路径
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @param filename 要存放流的拷贝的目标路径（含文件名）
		 @return 成功返回true，否则返回false
		 @note 如果filename为NULL,则释放到包的虚拟目录所在文件夹，如果是相对目<br>
			录则释放到当前目录
		 */
		virtual bool extractStream(const char* name, const char* filename = NULL, ulong cryptMask = 0, ulong compressMask = 0) = 0;

		/** 扩展文件到指定的目录
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @param dir 要存放文件的目录
		 @return 成功返回true，否则返回false
		 @note 如果dir为NULL,则释放到包的虚拟目录所在文件夹，如果是相对目录则在前面自动添加当前目录
		 */
		virtual bool extractStreamToDir(const char* name, const char* dir = NULL, ulong cryptMask = 0, ulong compressMask = 0) = 0;

		/** 重命名包文件中指定的流
		 @param name 流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @param newName 新的流名
		 @return 成功返回true，否则返回false
		 */
		virtual bool renameStream(const char* name, const char* newName) = 0;

		/** 删除包文件中指定的流
		 @param name 要删除的流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @return 成功返回true，否则返回false
		 */
		virtual bool removeStream(const char* name) = 0;

		/** 添加目录及其所有子目录下面的文件流到包文件
		 @param absolutePath 目录的绝对路径
		 @param relativePath 目录在包中的相对路径
		 @param existReplace 当存在该流时是否替换
		 @param cryptMask 加密Mask
		 @param compressMask 压缩Mask
		 @return 成功返回true，否则返回false
		 */
		virtual bool addDir(const Path& absolutePath, const Path& relativePath, bool existReplace = true) = 0;

		/** 添加一个包文件到另一个包中
		@param sourceFilename 要加入的包的路径名
		@param existReplace 当存在该流时是否替换
		@param proc 回调函数
		@return 成功返回true，否则返回false
		@note 为了确保玩家硬盘上面的数据不压缩，添加之前，需要设置一下压缩和加密Mask
		*/
		virtual bool addMpk(const char* sourceFilename, bool replaceExist, PKGPROC proc = 0) = 0;

		/** 扩展包文件中一个目录（及其所有子目录）中所有的流到指定的目录（保持原有目录结构）
		 @param dir 包中指定的目录
		 @param saveDir 要存放结果的目录
		 @return 成功返回true，否则返回false
		 @note 如果dir为NULL，则调用@c extractPackage
		 */
		virtual bool extractDir(const char* dir, const char* saveDir = NULL, ulong cryptMask = 0, ulong compressMask = 0) = 0;

		/** 重命名包文件中指定的目录
		@param dir 目录的名字（应该包括从包文件的根目录一直到当前目录所在的整个目录结构，如："test\first\data"）
		@param newDir 新的目录名，作为上面那个参数的替换，目录深度应该匹配，而且只能修改最后一个目录，如："test\first\abc"
		@return 成功返回true，否则返回false
		*/
		virtual bool renameDir(const char* dir, const char* newDir) = 0;

		/** 删除包文件中的一个目录
		 @param relativeDir 相对目录，包括包中的当前目录及其所有父目录，不支持空目录
		 @return 成功返回true，否则返回false
		 */
		virtual bool removeDir(const char* relativeDir) = 0;

		/** 获取包中的文件列表 */
		virtual const IPkgOperation::FileList& getFileList() const = 0;

		/** 添加加密Mask */
		virtual ulong addCryptMask(ulong cryptMask) = 0;

		/** 移除加密Mask */
		virtual ulong removeCryptMask(ulong cryptMask) = 0;

		/** 获得加密Mask */
		virtual ulong getCryptMask() const = 0;

		/** 设置加密Mask */
		virtual ulong setCryptMask(ulong cryptMask) = 0;

		/** 添加压缩Mask */
		virtual ulong addCompressMask(ulong compressMask) = 0;

		/** 移除压缩Mask */
		virtual ulong removeCompressMask(ulong compressMask) = 0;

		/** 获得压缩Mask */
		virtual ulong getCompressMask() const = 0;

		/** 设置压缩Mask */
		virtual ulong setCompressMask(ulong compressMask) = 0;

		/// 设置优化(当压缩后尺寸反而变大的情况，自动改为不压缩，默认优化)
		virtual void setOptimization(bool optimization) = 0;

		/** 释放 */
		virtual void release() = 0;
		//virtual bool compactPackage(COMPACTPROC compact, void* lpCallbackData) = 0;
		// virtual bool updateStream(const char* name); == addStream
		//virtual bool extractDir(...) // == extractFile

		/** 打开一个内存中的包文件
		@param data 内存缓冲区地址
		@param len 内存字节长度
		@return 成功返回true，否则返回false
		*/
		virtual bool openPackage(const void* data, size_t len) = 0;

		/** 添加一个内存包文件到另一个包中
		@param data 内存缓冲区地址
		@param len 内存字节长度
		@param existReplace 当存在该流时是否替换
		@param proc 回调函数
		@return 成功返回true，否则返回false
		@note 为了确保玩家硬盘上面的数据不压缩，添加之前，需要设置一下压缩和加密Mask
		*/
		virtual bool addMemoryMpk(const void* data, size_t len, bool replaceExist, PKGPROC proc = 0) = 0;

		virtual bool flushPackage() = 0;

		/// 计算包的碎片率
		virtual float calcSegmentRatio() const = 0;

		/// 是否是压缩优化模式
		virtual bool isOptimization() const = 0;

		/// 设置压缩比水位线，取值范围为(0,1]之间的浮点数据
		virtual void setRatioWaterLine(float ratioWaterLine) = 0;

		/// 获取压缩比水位线
		virtual float getRatioWaterLine() const = 0;

		/** 取消删除包文件中的一个目录
		 @param relativeDir 相对目录，包括包中的当前目录及其所有父目录，不支持空目录
		 @return 成功返回true，否则返回false
		 */
		virtual bool cancelremoveDir(const char* relativeDir) = 0;

		/** 取消删除包文件中指定的流
		 @param name 要删除的流的名字（在包文件中流的名字可能含有相对路径名，忽略大小写）
		 @return 成功返回true，否则返回false
		 */
		virtual bool cancelremoveStream(const char* name) = 0;
	};


	/// 创建包文件操作对象
	RKT_API IPkgOperation* createPkgOperation();





	/// 包文件清理结果
	enum PkgResult
	{
		PR_OK					= 1,
		PR_ERR_UNKNOW			= 0,
		PR_ERR_PARAM			= -1,
		PR_ERR_MEMORY			= -2,
		PR_ERR_OPEN_PACKAGE		= -3,
		PR_ERR_CREATE_PACKAGE	= -4,
		PR_ERR_OPEN_STREAM		= -5,
		PR_ERR_READ_STREAM		= -6,
		PR_ERR_WRITE_STREAM		= -7,
		PR_ERR_DELETE_FILE		= -8,
		PR_ERR_RENAME_FILE		= -9
	};


	/** 清理包文件(可用于碎片整理，对块进行新的加密、压缩整理等)
	@param filename		包文件名
	@param cryptMask	新的加密类型，如果填写-1表示保持先前的加密类型
	@param compressMask	新的压缩类型，如果填写-1表示保持先前的压缩类型
	@param proc			清理过程回调函数
	@param ratioWaterLine 压缩比水位线
	@param ignoreCompExtList 指定忽略的文件扩展名列表，形如".jpg.mp3.wav"
	*/
	RKT_API PkgResult cleanPackage(const char* filename, 
							int cryptMask = -1,
							int compressMask = -1,
							PKGPROC proc = 0,
							float ratioWaterLine = 1.f,
							const char* ignoreCompExtList = 0
							);


} // namespace

#endif // #ifdef SUPPORT_STREAM

#endif // __PkgStruct_h__
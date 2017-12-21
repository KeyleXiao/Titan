#ifndef __I_MPK_MANIP_H__
#define __I_MPK_MANIP_H__

#include <string>

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/** Mpk file操作接口 */
struct IMpkFileManip
{
	/**释放自己
	*/
	virtual void release() = 0;

	/**读取数据
	@param pBuffer 数据缓冲区
	@param size 要读取的数据长度
	@return 是否读取成功
	@remarks
	*/
	virtual bool read(void *pBuffer,uint size) = 0;

	/**定位文件位置
	@param length 偏移量
	@param from 从哪里开始,SEEK_SET等
	@return 是否成功
	@remarks
	*/
	virtual bool seek(uint length,uint from) = 0;

	/**获得当前文件指针
	*/
	virtual uint getPos() = 0;

	/**文件大小
	@return 文件大小
	@remarks
	*/
	virtual uint size() = 0;

	/**获得原始文件大小
	@return 原始文件大小
	@remarks
	*/
	virtual uint getOriginSize() = 0;

	/**获得压缩后的文件大小
	@return 压缩后的文件大小
	@remarks
	*/
	virtual uint getCompressedSize() = 0;

	/**是否加密
	@return 是否加密
	@remarks
	*/
	virtual bool isEncrypted() = 0;
};

struct IMpkManipCallback
{
	/**Mpk操作的回调
	@param strFilename 文件名
	@param p 自定义参数
	@return
	@remarks
	@see IMpkManip
	*/
	virtual void onProcessFile(const std::string& strFilename,void *p) = 0;
};

/** Mpk操作接口 */
struct IMpkManip
{
	/**释放自己
	*/
	virtual void release() = 0;

	/**打开一个包文件
	@param mpkFilename 文件名
	@param bCreateNew 是否创建新文件
	@return 是否成功打开文件
	@remarks
	*/
	virtual bool open(const char* mpkFilename,bool bCreateNew) = 0;

	/**关闭文件
	@return 是否成功
	@remarks
	*/
	virtual bool close() = 0;

	/**是否有个文件叫filename
	@return 文件是否存在
	@remarks
	*/
	virtual bool hasFile(const char* filename) = 0;

	/**打开一个文件
	@param filename 文件名
	@return 文件操作对象指针
	@remarks
	*/
	virtual IMpkFileManip* openFile(const char* filename) = 0;

	/**解压目录
	@param source 源路径 "Data\\Creatures\\char"
	@param targetDir 目标路径 "C:\\Creatures"
	@param replaceExist 是否替换已经存在的文件
	@param pCallback 回调接口，让上层有机会做点事情
	@param p 自定义数据
	@return 是否成功
	@remarks
	*/
	virtual bool extractDir(const char* source,const char* targetDir,bool replaceExist,IMpkManipCallback *pCallback,void *p) = 0;

	/**解压文件
	@param source 源路径 "Data\\Creatures\\char\\1.m2"
	@param targetFilename 目标文件名 "C:\\Creatures\\1.m2"
	@param replaceExist 是否替换已经存在的文件
	@return 是否成功
	@remarks
	*/
	virtual bool extractFile(const char* source,const char* targetFilename,bool replaceExist) = 0;

	/**删除目录
	@param dir 路径 "Data\\Creatures\\char"
	@return 是否成功
	@remarks
	*/
	virtual bool removeDir(const char* dir) = 0;

	/**删除文件
	@param filename 文件名
	@return 是否成功
	@remarks
	*/
	virtual bool removeFile(const char* filename) = 0;

	/**重命名文件,如果已经有个dest文件，那么会失败
	@param source 源文件名
	@param dest 目标文件名
	@return 是否成功
	@remarks
	*/
	virtual bool renameFile(const char* source,const char* dest) = 0;

	/**添加一个文件到包内
	@param sourceFilename 文件名 "C:\\1.mpk"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@return 是否成功
	@remarks
	*/
	virtual bool addMpk(const char* sourceFilename,bool replaceExist = true,bool compress = true,bool encrypt =false) = 0;

	/**添加一个文件到包内
	@param sourceFilename 文件名 "C:\\1.txt"
	@param targetFilename 目标文件 "Data\\1.txt"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@return 是否成功
	@remarks
	*/
	virtual bool addFile(const char* sourceFilename,const char* targetFilename,bool replaceExist = true,bool compress = true,bool encrypt =false) = 0;

	/**添加一个内存数据文件到包内
	@param data 文件数据
	@param len 文件数据长度
	@param targetFilename 目标文件 "Data\\1.txt"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@return 是否成功
	@remarks
	*/
	virtual bool addFile(unsigned char* data,ulong len,const char* targetFilename,bool replaceExist = true,bool compress = true,bool encrypt =false) = 0;

	/**添加一个目录到包内
	@param sourceDir 目录名 "C:\\Data"
	@param targetDir 目标目录 "Data\\char"
	@param replaceExist 是否替换现有文件
	@param compress 是否压缩
	@param encrypt 是否加密
	@param pCallback 回调，对于每个文件回调一次
	@param p 自定义回调参数
	@return 是否成功
	@remarks
	*/
	virtual bool addDir(const char* sourceDir,const char* targetDir,bool replaceExist = true,bool compress = true,bool encrypt =false,IMpkManipCallback *pCallback = 0,void *p = 0) = 0;

	/**定位到第一个文件
	@param szMask 通配符"*.txt"
	@return 文件名
	*/
	virtual const char* findFirstFile(const char* szMask) = 0;

	/**定位到下一个文件
	@return 文件名
	*/
	virtual const char* findNextFile() = 0;

	/**关闭查找文件
	*/
	virtual void findClose() = 0;

	/**Dump所有文件
	@param
	@param
	@return
	@remarks
	*/
	virtual void dumpListFile() = 0;

	/**取得包使用率，包里面可能存在碎片
	@param mpkFilename 包文件名
	@return 百分比，88
	@remarks
	*/
	//virtual uint getUseRate(const char* mpkFilename) = 0;

	/**清除包文件碎片
	@param mpkFilename 包文件名
	@return 是否成功
	@remarks
	*/
	virtual bool cleanupFragment(const char* mpkFilename) = 0;

	/**取得列表文件名，用于打包工具
	@return 列表文件名(listfile)
	@remarks用于打包工具
	*/
	virtual const std::string& getListFileName() = 0;
};

/// 创建Mpk文件操作对象
RKT_API IMpkManip* createMpkManip();

	/** @} */
}

#endif
#ifndef __ListStream_H__
#define __ListStream_H__

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


/// 枚举流Callback
struct IListFileCallback
{
	/**枚举所有文件的回调
	@param strFilename 文件名
	@param p 自定义参数
	@see IMpkManip
	*/
	virtual void onGetStreamName(const char* strName,void *p) = 0;
};

	/** @} */
}

#endif
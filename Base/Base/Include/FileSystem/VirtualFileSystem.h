#ifndef __VirtualFileSystem_H__
#define __VirtualFileSystem_H__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include <vector>
#include "Thread.h"
#include "IFileSystem.h"
#include "MemStream.h"
#include "Api.h"

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


	struct IListFileCallback;
	class Stream;

	/**文件系统
	@param
	@param
	@return
	@remarks
	*/
	class RKT_EXPORT FileSystem
	{
		typedef std::vector<IFileSystem*>		StreamSystemVector;
	public:

		/**添加查找路径
		@param pPath 路径
		@param isEncode 表面文件系统中文件是否采取了编码措施，只适用于目录态的文件系统
		@return 文件系统
		@remarks
		*/
		IFileSystem*		addFindPath(const char* pPath, bool isEncode = false);

		/// 添加内存pkg文件系统(该内存由外部new出来，内部不进行内存拷贝，直接对传入的内存虚拟成文件系统)
		IFileSystem* addMemoryPkg(const void* data, size_t len);

#ifdef SUPPORT_ASYNC_STREAM
		/** 异步请求一个流对象
		@param streamName	文件名
		@param sink			请求结束后的回调接口
		@return 成功返回true，流不存在，或者请求出错，返回false
		*/
		bool request(const char* streamName, IAsynStreamSink* sink, ulong userdata = 0, ulong priority = 0);
#endif

		/**打开一个文件
		@param pFileName 文件名
		@param mode 打开方式
		@return 流指针
		@remarks
		*/
		Stream*	open(const char* pFileName, uint flags = Stream::modeRead|Stream::shareRead|Stream::modeExist);

		/**通过一个文件系统打开一个文件
		@param pStreamSystem 文件系统
		@param pFileName 文件名
		@param mode 打开方式
		@return 流指针
		@remarks
		*/
		Stream*	open(IFileSystem* pStreamSystem, const char* pFileName, uint flags = Stream::modeRead|Stream::shareRead|Stream::modeExist);

		/**是否能找到某个文件
		@param pFileName 文件名
		@return 文件是否存在于当前查找路径
		@remarks
		*/
		bool		isExist(const char* pFileName);

		/**列举当前查找路径下的所有文件
		@param pCallback 回调
		@param p 自定义参数
		@return
		@remarks
		*/
		void		list(IListFileCallback *pCallback,void *p);

		void		removeFileSystem(IFileSystem* fs);

		void		insertFileSystem(IFileSystem* fs,int index);

		void        setFileVerify( IFileVerify * pVerify );

		void		release(){delete this;}

	private:
		StreamSystemVector		m_vStreamSystem;
		Mutex	m_mutex;
	public:
		FileSystem(){}
		~FileSystem();
	};

		RKT_API FileSystem*	getFileSystem();
		RKT_API IFileSystem*	openFileSystem(const char* szPath, bool isEncode = false);

		/// 从内存打开文件系统(目前只用于内存Pkg)
		RKT_API IFileSystem*	openFileSystemFromMemory(const void* data, size_t len);

		/// 读取文本文件到内存流（考虑了所有编码细节）
		RKT_API MemoryStream* readTextFileToMemoryStream(const char* filename, bool add_terminating_char = false, EncodingType encodingType = EncodingType_Unknow);

		RKT_API bool get_enableTickCount();
		RKT_API float get_pTickCount();
		RKT_API void add_pTickCount(float tick);


	class RKT_EXPORT CStreamHelper
	{
	public:
		CStreamHelper();
		CStreamHelper(Stream*);
		~CStreamHelper();
		Stream* operator->();
		operator Stream*();
		bool isNull();
		const CStreamHelper& operator=(Stream* pStream);
		void close();
		//operator bool();
	private:
		Stream*	m_pStream;
	};


	/** @} */
}

#endif // #ifdef SUPPORT_STREAM

#endif
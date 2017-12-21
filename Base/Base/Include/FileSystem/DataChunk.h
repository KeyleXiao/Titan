#ifndef __DataChunk_H__
#define __DataChunk_H__

#include "Common.h"

#ifdef SUPPORT_STREAM
#include "MemStream.h"

#include <vector>

#define CHUNK_MIN_SIZE	8

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


	/**
	Represents a DataChunk ID-LENGTH-DATA.out terrain data format is based on DataChunk.
	@remarks derived class can inherit this class to support DataChunk format data.use registerChunkOperation to bind ChunkID with Operation.
	*/
	class RKT_EXPORT DataChunk
	{
	public:
		struct RKT_EXPORT stChunk
		{
			friend class DataChunk;
		public:
			uint GetSize();
		public:
			stChunk(uint ui32Type);
			stChunk(uint ui32Type,uint ui32ChunkSize,uint ui32DataSize,void *pData,stChunk *pChild,stChunk *pSibling);
			~stChunk();
		public:
			uint		m_ui32Type;	//类型

			// Helper Member,not saved in DataChunk-based files.
			uint		m_ui32ChunkSize;

			uint		m_ui32DataSize;
			void		*m_pData;	//数据

		private:
			MemoryStream	m_data;
		public:
			stChunk	*m_pSibling;
			stChunk	*m_pChild;
		};
	public:
		DataChunk();
		~DataChunk();
	public:
		/**
		public interface that can deal with chunks.
		*/

		// read DataChunk data from DataStream,DataStream can be MemoryDataStream or FileDataStream
		void SerializeIn(Stream* data,uint size);

		/** read DataChunk data from file
		@param bLoadAll true for load the Data of the DataChunk,false for only build the DataChunk structure.
		*/
		void SerializeIn(char szFileName[],bool bLoadAll = true);

		/** write DataChunk data to file
		@param szFileName the file to write to
		@remarks if szFileName is existed,the file will be overwritten.
		*/
		void save(const char* szFileName);
		
		/** write DataChunk data to Stream
		@param pStream the stream to write to
		*/
		void save(Stream* pStream);

		/** destroy the DataChunk tree,this function is called by destructor automaticly.
		*/
		void Close();

	private:
		/** fill all DataChunk's m_ui32ChunkSize field,then the DataChunk tree is perfectly represented.
		*/
		uint BuildChunkSize(stChunk *pChunk);
		stChunk *SerializeIn_Helper(Stream* data,uint ui32FPBegin,uint ui32FPEnd,stChunk *pParent,stChunk *pBrother);
		typedef void (DataChunk::*pRecursiveFunc)(stChunk *,void*);
		void	RecursiveFunc(pRecursiveFunc pFunc,stChunk *pChunk,void*);
		stChunk *SerializeIn_Helper(FILE *fp,uint ui32FPBegin,uint ui32FPEnd,bool bLoadAll,stChunk *pParent,stChunk *pBrother);
		void	Close_Helper(stChunk *pChunk,void *p);
		void	SerializeOut_Helper(FILE *fp,stChunk *p);
		void	SerializeOut_Helper(Stream *pStream,stChunk *p);
	private:
		stChunk	*m_pChunk;
	protected:
		/** operate DataChunk via strID
		@remarks base class just return,derived class should deal with this.
		*/
		virtual void operateChunk(uint ui32ID,Stream* pData,uint size);
		//-------------------------------------------------------------------------------------------------------
	public:
		//Read

		/**开始读取一个Chunk，通过流
		@param pData 数据指针
		@return 返回一个stChunk
		*/
		stChunk* beginChunk(Stream* pData,bool bWowFormat = false);

		/**开始读取一个Chunk，通过内存指针
		@param pData 数据指针
		@param size 内存大小
		@return 返回一个stChunk
		*/
		stChunk* beginChunk(uchar* pData,uint size,bool bWowFormat = false);


		/**读取下一个Chunk，通过流
		@param pData 数据指针
		@return 返回一个stChunk
		*/
		stChunk* nextChunk(Stream* pData,bool bWowFormat = false);

		/**读取下一个Chunk
		@return 返回一个stChunk
		*/
		stChunk* nextChunk(bool bWowFormat = false);


		/**获得当前流指针在流中的绝对偏移量
		@return 偏移量
		@remarks 用于一些工具
		*/
		uchar* getAbsoluteDataPointer();

		//Write

		/**开始保存一个Chunk
		@param ui32Type Chunk类型 'MVER'
		@param pDataStream 返回一个数据流供写入
		@return 返回一个Chunk
		@remarks
		*/
		stChunk* beginChunk(uint ui32Type,Stream** pDataStream);

		/**结束写一个Chunk，确保beginChunk要对应一个endChunk
		*/
		void endChunk();

		/**获得当前数据指针在这个文件中的偏移值
		@return 偏移值
		@remarks
		*/
		uint getOffset();
	private:
		stChunk* m_pCurrentChunk;
		stChunk* readChunk(Stream* pData,bool bWowFormat);
		MemoryStream m_data;
		//Write
		std::vector<stChunk*>	m_vChunks;
		uint	m_ui32Offset;
	};

	/** @} */
}

#endif // #ifdef SUPPORT_STREAM

#endif
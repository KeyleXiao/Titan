#ifndef __DataStream_H__
#define __DataStream_H__

#include "Common.h"

#ifdef SUPPORT_STREAM

#include <string>
#include <assert.h>

namespace rkt{
	/*! \addtogroup FileSystem
	*  文件系统
	*  @{
	*/


	/**可以自动增长的数据流
	*/
	class DataStream
	{	
	public:
		DataStream();
		DataStream(const void* pBuffer, size_t nSize);
		DataStream(const DataStream& rhs);
		DataStream& operator=(const DataStream& rhs);
		~DataStream();

	public:	// Get/Set field methods

		// Get methods
		RKT_EXPORT const void* data() const;
		const void* dataEnd() const;
		RKT_EXPORT size_t size() const;
		size_t capacity() const;

		bool atEnd() const;

		// Set methods
		void empty();


	public:	// append data operators
		DataStream& operator<<(bool v);
		DataStream& operator<<(short v);
		DataStream& operator<<(ushort v);
		DataStream& operator<<(int v);
		DataStream& operator<<(uint v);
		DataStream& operator<<(long v);
		DataStream& operator<<(ulong v);
		DataStream& operator<<(const char* p);
		DataStream& operator<<(const wchar_t* p);		
		template<class Traits>
		DataStream& operator<<(const std::basic_string<Traits>& s);
		DataStream& operator<<(const DataStream& rhs);
		RKT_EXPORT DataStream& append(const void* pBuffer, size_t nSize);
		RKT_EXPORT DataStream& write(const void* pBuffer, size_t nSize);
	public:	// extract data operators
		DataStream& operator>>(bool& v);
		DataStream& operator>>(short& v);
		DataStream& operator>>(ushort& v);
		DataStream& operator>>(int& v);
		DataStream& operator>>(uint& v);
		DataStream& operator>>(long& v);
		DataStream& operator>>(ulong& v);
	
		template<class Traits>
		DataStream& operator>>(std::basic_string<Traits>& s);
		DataStream& operator>>(DataStream& rhs);

	//-------------------------------------------------------------------------
	//	the following code is private
	//-------------------------------------------------------------------------
	private:
		template<class T>
		DataStream& _Append(T v)
		{
			m_impl.Append(&v, sizeof(T));
			return *this;
		}

		template<class T>
		DataStream& _Read(T& v)
		{
			m_impl.ReadData(&v, sizeof(T));
			return *this;
		}

		void _AppendWithLen(const void* p, size_t nLen)
		{
			// invoke PrepareSize explicitly, to avoid adding size twice
			m_impl.PrepareSize(m_impl.m_nSize + sizeof(size_t) + nLen);

			m_impl.Append(&nLen,	sizeof(size_t));
			m_impl.Append(p,		nLen);
		}

	private:

		class noncopyable
		{
		protected:

			noncopyable() { }
			~noncopyable() { } ///< May not be virtual! Classes without virtual operations also derive from noncopyable.

		private:

			noncopyable(const noncopyable&);
			const noncopyable& operator=(const noncopyable&);
		};

		// Support the class to simplify the memory management,
		//	and let the class DataStream be Exception-Safe
		class DataStreamImpl : private noncopyable
		{
		public:
			enum { INITIAL_CAPACITY = 256 };

			DataStreamImpl()
				: m_pData(NULL)
				, m_nSize(0)
				, m_nCapacity(0)
				, m_nCurrent(0)
			{
			}

			DataStreamImpl(const void* pBuffer, size_t nSize, size_t nCapacity, size_t nCurrent = 0)
				: m_nCurrent(nCurrent)
			{
				assert(nSize <= nCapacity);
				assert(nSize >= 0);
				assert(nCapacity >= 0);
				
				m_pData = new uchar[nCapacity];
				memset(m_pData, 0, nCapacity);
				
				if (pBuffer && nSize) memcpy(m_pData, pBuffer, nSize);
				m_nSize = nSize;
				m_nCapacity = nCapacity;
			}

			~DataStreamImpl()
			{
				safeDeleteArray(m_pData);
			}

			void Swap(DataStreamImpl& other)	//throw()
			{
				std::swap(m_pData,		other.m_pData);
				std::swap(m_nSize,		other.m_nSize);
				std::swap(m_nCapacity,	other.m_nCapacity);
				std::swap(m_nCurrent,	other.m_nCurrent);
			}

			void Empty()
			{
				safeDelete(m_pData);
				m_nSize = m_nCapacity = m_nCurrent = 0;
			}

			void Append(const void* pBuffer, size_t nSize)
			{
				PrepareSize(m_nSize + nSize);

				if (pBuffer && nSize) memcpy(m_pData + m_nSize, pBuffer, nSize);
				m_nSize += nSize;
			}


			void PrepareSize(size_t nSize)
			{
				assert(nSize > 0);

				if (m_nCapacity < nSize)
				{
					size_t nCapacity = _CalculateCapacity(nSize);
					
					DataStreamImpl temp(m_pData, m_nSize, nCapacity);
					Swap(temp);
				}
			}

			template<class T>
			void ReadData(T* pBuffer, size_t nSize)
			{
				assert(pBuffer != NULL);
				assert(m_nCurrent + nSize <= m_nSize);

				memcpy(pBuffer, m_pData + m_nCurrent, nSize);
				m_nCurrent += nSize;
			}

			const uchar* GetCurrentPtr(size_t nSize = 0)
			{
				assert(m_nCurrent + nSize <= m_nSize);

				return m_pData + m_nCurrent;
			}

			void MovePtr(size_t nSize)
			{
				assert(m_nCurrent + nSize <= m_nSize);

				m_nCurrent += nSize;
			}

		public:
			uchar* m_pData;
			size_t m_nSize, m_nCapacity;
			size_t m_nCurrent;	// used for ReadData

		private:
			size_t _CalculateCapacity(size_t nSize)
			{
				size_t nCapacity = (m_nCapacity > 0) ? m_nCapacity : INITIAL_CAPACITY;
				while(nCapacity < nSize) nCapacity <<= 1;

				return nCapacity;
			}
		};

		DataStreamImpl m_impl;
	};

	/** @} */
}

#endif // #ifdef SUPPORT_STREAM

#endif
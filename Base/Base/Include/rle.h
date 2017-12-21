//==========================================================================
/**
* @file	  : rle.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-7-22   17:38
* purpose : RLE(解)压缩模板
*/
//==========================================================================

#ifndef __RLE_H__
#define __RLE_H__

namespace rkt {

	/// RLE(解)压缩,Size为数据单元的最小长度
	template <size_t Size = 1>
	class Rle
	{
		unsigned char*	_buf;		/// 临时缓冲区
		size_t			_cap;		/// 缓冲区容量(字节数)
	protected:
		enum { MaxRepeatCount = 0x80 };
	public:
		Rle() : _buf(0), _cap(0) { STATIC_ASSERT(Size > 0); }
		~Rle() { if (_buf) delete[] _buf; }

	public:
		/// 获取操作后的目标缓冲区
		const unsigned char* data() const						{ return _buf; }

		/// 获取缓冲区容量
		size_t capacity() const									{ return _cap; }

		/** 压缩(标志数据与传入的数据的字节数相同)
		@param src		源缓冲区
		@param bytes	源缓冲区数据长度(字节数)
		@retval			成功返回压缩后的数据长度(字节数)，否则返回0
		@remark			存储格式：...;n-1;n个不同值a1;a2;...;an;k+127;k个相同值;b;...
		*/
		size_t compress(const void* src, size_t bytes)
		{
			if (!src || bytes == 0 || (bytes % Size) != 0) // 参数校验
				return 0;

			size_t sameDataRepeatCount = 0;			// 相同数据连续出现的次数
			size_t differentDataRepeatCount = 0;	// 不同数据连续出现的次数
			size_t pos = 0;							// 写入rle缓冲的位置
			size_t count = bytes / Size;			// 源数据的个数
			unsigned char* cur = (unsigned char*)src;
			unsigned char* from = cur;

			if (count == 1) // 只有一个数据
			{
				writeSameData(pos, 1, from);
			}
			else
			{
				for (size_t i = 1; i < count; i++)
				{
					if (ElemTraits<Size>::equalto(cur, cur + Size)) // 两个数据相同
					{
						if (differentDataRepeatCount > 0) // 写入前面连续不同的数据
						{
							writeDifferentData(pos, differentDataRepeatCount, from);
							from = cur; 
							differentDataRepeatCount = 0;
							sameDataRepeatCount = 1;
						}
						else // 连续相同的数据数目加1
						{
							sameDataRepeatCount ++;
						}
					}
					else // 两个数据不相同
					{
						if (sameDataRepeatCount > 0) // 写入前面连续相同的数据
						{
							writeSameData(pos, sameDataRepeatCount + 1, from);
							from = cur + Size;
							sameDataRepeatCount = 0;
							differentDataRepeatCount = 0;
						}
						else // 连续不同的数据数目加1
						{ 
							differentDataRepeatCount ++;
						}
					}

					// 对最大连续数目的判断
					if (sameDataRepeatCount >= MaxRepeatCount) // 连续相同的数据数目已经达到了最大值
					{
						writeSameData(pos, sameDataRepeatCount, from);
						from = cur + Size;
						differentDataRepeatCount = 0;
						sameDataRepeatCount = 0;
					}
					else if (differentDataRepeatCount >= MaxRepeatCount) // 连续不同的数据数目已经达到了最大值
					{
						writeDifferentData(pos, differentDataRepeatCount, from);
						from = cur + Size;
						differentDataRepeatCount = 0;
						sameDataRepeatCount = 0;
					}

					cur += Size;
				}

				// 尾部处理
				if (sameDataRepeatCount > 0) // 本行结束前有连续的相同数据
				{
					writeSameData(pos, sameDataRepeatCount + 1, from);
				}
				else if (differentDataRepeatCount > 0)  // 否则，本行结束前有不同的连续数据
				{
					writeDifferentData(pos, differentDataRepeatCount + 1, from);
				}
				else if (from < (unsigned char*)src + bytes) // 最后还剩一个元素？
				{
					writeSameData(pos, 1, from);
				}
			}

			return pos;
		}


		/** 解压缩
		@param src		源缓冲区
		@param bytes	源缓冲区数据长度(字节数)
		@retval			成功返回解压缩后的数据长度(字节数)，否则返回0
		*/
		size_t decompress(const void* src, size_t bytes)
		{
			if (!src || bytes == 0) return 0;

			size_t sameDataRepeatCount = 0;			// 相同数据连续出现的次数
			size_t differentDataRepeatCount = 0;	// 不同数据连续出现的次数
			size_t pos = 0;							// 写入缓冲的位置
			unsigned char* cur = (unsigned char*)src;

			for (size_t i=0; i<bytes;)
			{
				if (cur[i] >= MaxRepeatCount) // 连续相同的数据
				{
					sameDataRepeatCount = cur[i++] - MaxRepeatCount + 1;
					readSameData(pos, sameDataRepeatCount, &cur[i]);
					i += Size;
				}
				else // 连续不同的数据
				{
					differentDataRepeatCount = cur[i++] + 1;
					size_t n = Size * differentDataRepeatCount;
					readDifferentData(pos, n, &cur[i]);
					i += n;
				}
			}
			return pos;
		}

	private:
		inline void readSameData(size_t& pos, size_t count, unsigned char* from)
		{
			resize(pos + Size * count);
			while (count-- > 0)
			{
				ElemTraits<Size>::assign(&_buf[pos], from);
				pos += Size;
			}
		}
		inline void readDifferentData(size_t& pos, size_t bytes, unsigned char* from)
		{
			resize(pos + bytes);
			memcpy(&_buf[pos], from, bytes);
			pos += bytes;
		}
		inline void writeSameData(size_t& pos, size_t count, unsigned char* from)
		{
			resize(pos + 1 + Size);
			_buf[pos] = MaxRepeatCount + count - 1;
			pos++;

			ElemTraits<Size>::assign(&_buf[pos], from);
			pos += Size;
		}
		inline void writeDifferentData(size_t& pos, size_t count, unsigned char* from)
		{
			size_t n = Size * count;
			resize(pos + 1 + n);
			_buf[pos] = count - 1;
			pos++;

			memcpy(&_buf[pos], from, n);
			pos += n;
		}
		inline void resize(size_t new_cap)
		{
			if (new_cap >= _cap)
			{
				size_t good_cap = _cap ? _cap : 1024;
				while (good_cap <= new_cap)
					good_cap += good_cap >> 1;
				new_cap = (good_cap + 3) & ~3;

				unsigned char* buf = new unsigned char[new_cap];
				if (_buf)
				{
					memcpy(buf, _buf, _cap);
					delete[] _buf;
				}
				_buf = buf;
				_cap = new_cap;
			}
		}

		/// 特化和偏特化
		template<size_t _Size>
		struct ElemTraits
		{
			static bool equalto(const void* e1, const void* e2) { return memcmp(e1, e2, _Size) == 0; }
			static void assign(void* d, const void* s)			{ memcpy(d, s, _Size); }
		};
		template<>
		struct ElemTraits<1>
		{
			static bool equalto(const void* e1, const void* e2) { return *(const unsigned char*)e1 == *(const unsigned char*)e2; }
			static void assign(void* d, const void* s)			{ *(unsigned char*)d = *(const unsigned char*)s; }
		};
		template<>
		struct ElemTraits<2>
		{
			static bool equalto(const void* e1, const void* e2) { return *(const unsigned short*)e1 == *(const unsigned short*)e2; }
			static void assign(void* d, const void* s)			{ *(unsigned short*)d = *(const unsigned short*)s; }
		};
		template<>
		struct ElemTraits<4>
		{
			static bool equalto(const void* e1, const void* e2) { return *(const unsigned long*)e1 == *(const unsigned long*)e2; }
			static void assign(void* d, const void* s)			{ *(unsigned long*)d = *(const unsigned long*)s; }
		};
		template<>
		struct ElemTraits<3>
		{
			struct __rgb{ unsigned char r,g,b; };
			static bool equalto(const void* e1, const void* e2) { return ((const __rgb*)e1)->r == ((const __rgb*)e2)->r && ((const __rgb*)e1)->g == ((const __rgb*)e2)->g && ((const __rgb*)e1)->b == ((const __rgb*)e2)->b; }
			static void assign(void* d, const void* s)			{ *(__rgb*)d = *(const __rgb*)s; }
		};
		template<>
		struct ElemTraits<8>
		{
			static bool equalto(const void* e1, const void* e2) { return *(const unsigned __int64*)e1 == *(const unsigned __int64*)e2; }
			static void assign(void* d, const void* s)			{ *(unsigned __int64*)d = *(const unsigned __int64*)s; }
		};
	};
} // namespace

#endif // __RLE_H__

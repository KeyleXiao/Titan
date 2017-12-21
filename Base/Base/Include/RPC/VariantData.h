//==========================================================================
/**
* @file	  : DataChunk.h
* @author : CHEN.TAO! <101251683@qq.com>
* created : 2008-11-7   16:55
* purpose : 数据堆,根据一个键值找到一个数值
*/
//==========================================================================

#ifndef __DATA_CHUNK_H__
#define __DATA_CHUNK_H__

#include <string>
#include <map>

#include "Common.h"

#ifdef SUPPORT_RPC

#include "buffer.h"

namespace rkt
{
	class RKT_EXPORT Variant_Dic;

	//////////////////////////////////////////////////////////////////////////////////////
	/**
	@ name   : 复合类型数据
	@ breif  : 同时支持整型,字符串,浮点型,布尔型,64位长整型,字典等多种类型的符合数据
	@ note   : 该类型支持到各种类型的隐式转换,方便大家使用
	*/
	//////////////////////////////////////////////////////////////////////////////////////
	class RKT_EXPORT Variant_Data
	{
	public:
		enum
		{
			enType_Nil   = 0,   // 空类型
			enType_Int      ,   // 32位整型
			enType_String   ,   // 字符串类型
			enType_Bool     ,   // 布尔型
			enType_Float    ,   // 浮点型
			enType_Int64    ,   // 大整型
			enType_Dic      ,   // 字典类型
		};

		Variant_Data() : _type(enType_Nil){}

		Variant_Data( const int val ) : _type(enType_Int),_int_v(val){} 
		
		Variant_Data( const bool val) : _type(enType_Bool),_bool_v(val){} 

		Variant_Data( const float val) : _type(enType_Float),_float_v(val){} 

		Variant_Data( const __int64 val) : _type(enType_Int64),_int64_v(val){} 

		// 构造 : 字符串
		Variant_Data( const std::string & val ) ;

		// 构造 : 字典
		Variant_Data( const Variant_Dic & val );

		// 拷贝构造
		Variant_Data( const Variant_Data & val ) ;

		// 释构 : 销毁内存
		virtual ~Variant_Data();

		// 释放引用计数
		void ReleaseRef();

		// 流操作 : 输入
		ibuffer & operator << ( ibuffer & i );

		// 流操作 : 输出
		obuf & operator >> ( obuf & o );

		Variant_Data & operator = ( const Variant_Data & val );

		// 是否有效
		bool isValid() const { return _type!=enType_Nil; }

	public:
		unsigned char  _type;   // 数据类型
		struct custom_data      // 自定义类型数据
		{
			void *   _data;     // 数据内容
			int      _ref;      // 引用计数
		};

		union 
		{
			int     _int_v;		// 整型数据
			bool    _bool_v;	// 布尔型数据
			float   _float_v;	// 浮点型数据
			__int64 _int64_v;   // 大整型数据

			custom_data * _custom_v;  // 自定义数据,默认支持int64,字符串,字典类型
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////
	/**
	@ name  : 复合类型数据字典
	@ brief : 根据一个键值找到一个数值,数值的类型可以是整型,字符串,布尔型,浮点型等
	*/
	//////////////////////////////////////////////////////////////////////////////////////
	class RKT_EXPORT Variant_Dic
	{
	public:
		typedef   std::string					Key;
		typedef   Variant_Data                  Value;
		typedef   std::map<Key,Value>	        ValueTable;

		ValueTable                              _Table;           // 键-值 对

		Variant_Dic(){}

		Variant_Dic( const Variant_Dic & dic ) : _Table(dic._Table){}

		// 设置一个数值
		void SetValue( const Key & k,const Value & v )
		{
			_Table[k] = v;
		}

		// 获取一个数值
		const Variant_Data * GetValue( const Key & k ) 
		{
			ValueTable::iterator it = _Table.find(k);
			if ( it==_Table.end() )
				return 0;

			return &(it->second);
		}

		// 获取一个整型数值,当没找到时返回默认数值
		int GetInt( const Key & k,int def=0 ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_Int )
				return def;

			return v->_int_v;
		}

		// 获取一个布尔数值,当没找到时返回默认数值
		bool GetBool( const Key & k,bool def=false ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_Bool )
				return def;

			return v->_bool_v;
		}

		// 获取一个浮点数值,当没找到时返回默认数值
		float GetFloat( const Key & k,float def=0.0 ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_Float )
				return def;

			return v->_float_v;
		}

		// 获取一个大整型数值,当没找到时返回默认数值
		__int64 GetInt64( const Key & k,__int64 def=0 ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_Int64 )
				return def;

			return v->_int64_v;
		}

		// 获取一个字符串数值,当没找到时返回默认数值
		std::string GetString( const Key & k ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_String )
				return std::string();

			unsigned short * pBuf = (unsigned short *)(v->_custom_v->_data);
			unsigned short len = *pBuf;
			if ( len<=0 || len>10000000 )
				return std::string();

			return std::string((const char *)(pBuf+1),len);
		}

		// 获取一个字典数值,当没找到时返回0
		Variant_Dic * GetDic( const Key & k ) 
		{
			const Variant_Data * v = GetValue(k);
			if ( v==0 || v->_type!=Variant_Data::enType_Dic )
				return 0;	

			return (Variant_Dic*)(v->_custom_v->_data);
		}

		// 流操作 : 输入
		ibuffer & operator << ( ibuffer & i )
		{
			while(i.good() && !i.eof())
			{
				char * k = 0;
				i >> k;
				
				if ( k==0 || !i || i.eof() )
					break;

				Value v;
				v << i;

				if ( i.good() )
				{
					SetValue(k,v);
				}
			}

			return i;
		}

		// 流操作 : 输出
		obuf & operator >> ( obuf & o )
		{
			ValueTable::iterator it = _Table.begin();
			for ( ;it!=_Table.end();++it )
			{
				Key k = it->first;
				Value v = it->second;

				o << k;
				v >> o;
			}

			return o;
		}
	};
};

#endif // #ifdef SUPPORT_RPC

#endif//__DATA_CHUNK_H__

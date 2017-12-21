/*******************************************************************
** 文件名:	DataChunk.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			数据集合模版
********************************************************************/

#pragma once

#include "IDataChunk.h"
#include "TypeName.h"

struct DataChunkPropertyDeclare
{
	char             szType[16];	// 类型
	int              nDataOffset;	// 属性数据偏移
	int              len;			// 属性数据长度
	int              nMinLimit;		// 最小值限定
	int              nMaxLimit;		// 最大值限定
	int              nFlag;			// 序列化标签,参见ISerializable.h

	DataChunkPropertyDeclare() { memset( this,0,sizeof(DataChunkPropertyDeclare) ); }
};

// 定义属性的变量,因为没有类型反射功能,所以必须自己填写
#define		GET_OFFSET(C,member) (int)(&((C*)0)->member); 
#define     DATA_CHUNK_DECLARE(DATA)  DataChunkPropertyDeclare DChunk<DATA>::s_PropertyDeclare[];
#define		DECLARE_PROPERTY( DATA,id,member,minLimit,maxLimit,serializeFlag)     \
	if ( id<0 || id>=sizeof(DATA) || s_PropertyDeclare[id].len>0 ) {ErrorLn("DECLARE_PROPERTY Error!!!!!!!!!");} else{\
	s_PropertyDeclare[id].nDataOffset =GET_OFFSET(DATA,member);   \
	s_PropertyDeclare[id].len =sizeof(m_data.member);             \
	strcpy(s_PropertyDeclare[id].szType,TypeName(m_data.member)); \
	s_PropertyDeclare[id].nMinLimit =minLimit;                    \
	s_PropertyDeclare[id].nMaxLimit =maxLimit;					  \
	s_PropertyDeclare[id].nFlag = (serializeFlag); };


// 实体属性部件,使用时需要从这个模版继承
template<class DATA,int PROP_MAX=sizeof(DATA)>
class DChunk : public IDataChunk
{
public:
	static DataChunkPropertyDeclare  s_PropertyDeclare[PROP_MAX];

	DATA      m_data;

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	virtual int getPropertyCount()
	{
		for ( int i=PROP_MAX-1;i>=0;--i )
		{
			if ( s_PropertyDeclare[i].len>0 )
				return i+1;
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual const char * getPropertyType( int prop_id )
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		return s_PropertyDeclare[prop_id].szType;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual int getPropertyFlag( int prop_id )
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		return s_PropertyDeclare[prop_id].nFlag;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool getProperty_Bool( int prop_id)
	{
		void * data = 0;
		int len = 0;
		if ( !getProperty_custom(prop_id,data,len) )
		{
			return false;
		}

		if ( data==0 || len!=sizeof(bool) ) return false;
		return *(bool *)data;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_Bool( int prop_id,bool value)
	{
		return setProperty_custom(prop_id,&value,sizeof(value));
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual int getProperty_Integer( int prop_id)
	{
		void * data = 0;
		int len = 0;
		if ( !getProperty_custom(prop_id,data,len) )
		{
			return INVALID_INT_VALUE;
		}

		if ( data==0 ) return INVALID_INT_VALUE;

		switch(len)
		{
		case sizeof(BYTE):
			return *(BYTE *)data;
		case  sizeof(short):
			return *(unsigned short *)data;
		case sizeof(int):
			return *(int *)data;
		}

		return INVALID_INT_VALUE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_Integer( int prop_id,int value)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return false;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = s_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(m_data))
		{
			return false;
		}

		// 取值范围控制
		if ( value < declare.nMinLimit )
			value = declare.nMinLimit;

		if ( value>declare.nMaxLimit )
			value = declare.nMaxLimit;

		void * data = 0;

		switch(declare.len)
		{
		case sizeof(BYTE):
			{
				BYTE byte_value = (BYTE)value;
				data = &byte_value;
				break;
			}
		case  sizeof(short):
			{
				unsigned short short_value = (unsigned short)value;
				data = &short_value;
				break;
			}
		case sizeof(int):
			{
				data = &value;
				break;
			}
		default:
			return false;
		}

		memcpy( ((BYTE*)&m_data)+declare.nDataOffset,data,declare.len );
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool addProperty_Integer( int prop_id,int delta)
	{
		int value = getProperty_Integer(prop_id);
		if ( value==INVALID_INT_VALUE )
		{
			return false;
		}

		return setProperty_Integer(prop_id,value+delta);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual const char * getProperty_String( int prop_id )
	{
		void * data = 0;
		int len = 0;
		if ( !getProperty_custom(prop_id,data,len) )
		{
			return 0;
		}

		return (const char *)data;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_String( int prop_id,std::string value)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = s_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(m_data) )
		{
			return false;
		}

		// 太长就截断
		int len = min( value.size(),declare.len-1 );
		BYTE * data = ((BYTE*)&m_data)+declare.nDataOffset;
		memcpy( data,value.c_str(),len );
		data[len]=0;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool getProperty_custom( int prop_id,void *& data,int & len)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return false;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = s_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(m_data))
		{
			return false;
		}

		data = ((BYTE*)&m_data)+declare.nDataOffset;
		len  = declare.len;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_custom( int prop_id,void * data,int len)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX || data==0 )
		{
			return false;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = s_PropertyDeclare[prop_id];
		if ( declare.len==0 || len!=declare.len || declare.nDataOffset>=sizeof(m_data))
		{
			return false;
		}

		memcpy( ((BYTE*)&m_data)+declare.nDataOffset,data,len );
		return true;
	}
};
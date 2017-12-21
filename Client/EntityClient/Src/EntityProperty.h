/*******************************************************************
** 文件名:	EntityProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家属性集对象
********************************************************************/

#pragma once

#include "IEntity.h"
#include "IEntityProperty.h"
#include "EventDef.h"
#include "TypeName.h"
#include <string>
#include "Property.h"
#include "Smart_ClassT.h"
#include "ISchemeCenter.h"

// 实体属性部件,使用时需要从这个模版继承
template<class DATA,int PROP_MAX=PROPERTY_MAX>
class EntityProperty : public IEntityProperty
{
public:
	static DataChunkPropertyDeclare  s_PropertyDeclare[PROP_MAX];
	DataChunkPropertyDeclare  m_PropertyDeclare[PROP_MAX];
	DATA		m_data;
	IEntity*	m_pEntity;

public:
	EntityProperty() : m_pEntity(0) 
	{
	}

	virtual ~EntityProperty(){}

	//////////////////////////////**  IEntityPart **/////////////////////////////////////////////
	virtual short  getPartID() { return PART_PROPERTY; }

	virtual IEntity * getHost() { return m_pEntity; }

	virtual void release() { delete this; }

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len){ return true; }

	virtual bool onCommand( int cmdid,void * data,size_t len ){ return false; }
	
	virtual bool onLoad(IEntity* pEntity)
	{
		m_pEntity = pEntity;
		return true;
	}
    virtual void restore(){};
	//////////////////////////////**  IEntityProperty **/////////////////////////////////////////////
	// 取得属性数量
	virtual int getPropertyCount()
	{
		for ( int i=PROP_MAX-1;i>=0;--i )
		{
			if ( m_PropertyDeclare[i].len>0 )
				return i+1;
		}

		return 0;
	}
	// 取得某个属性的类型
	//////////////////////////////////////////////////////////////////////////////////////////
	virtual const char * getPropertyType( int prop_id )
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		return m_PropertyDeclare[prop_id].szType;
	}

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( int prop_id )
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		return m_PropertyDeclare[prop_id].nFlag;
	}

	virtual bool setProperty_custom(int prop_id, void* data, int len)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX || data==0 )
		{
			return false;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		if ( declare.len==0 || len!=declare.len || declare.nDataOffset>=sizeof(m_data))
		{
			return false;
		}

		memcpy( ((BYTE*)&m_data)+declare.nDataOffset,data,len );
        onPropertyChange( prop_id );

		return true;
	}

	virtual bool getProperty_custom(int prop_id, void*& data, int& len)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return false;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(m_data))
		{
			return false;
		}

		data = ((BYTE*)&m_data)+declare.nDataOffset;
		len  = declare.len;
		return true;
	}

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
			return *(/*unsigned*/ short *)data;
		case sizeof(int):
			return *(int *)data;
		}

		return INVALID_INT_VALUE;
	}

	virtual bool setProperty_Integer( int prop_id,int value)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX)
		{
			return false;
		}

		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];

		makesure_minmax(value, declare.nMinLimit, declare.nMaxLimit);

		BYTE byte_value = 0;
		/*unsigned*/ short short_value = 0;
		void* data = 0;

		switch (declare.len)
		{
		case sizeof(BYTE):
			{
				byte_value = (BYTE)value;
				data = &byte_value;
				break;
			}
		case  sizeof(short):
			{
				short_value = (/*unsigned*/ short)value;
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

		setProperty_custom(prop_id, data, declare.len);
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
		return setProperty_custom(prop_id, (void*)value.c_str(), value.length()+1);
	}

	//////////////////////////////**  ISerializable **/////////////////////////////////////////////
	virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag )
	{
		for ( int i=0;i<PROP_MAX;++i )
		{
			if ( m_PropertyDeclare[i].len>0 && m_PropertyDeclare[i].nFlag & flag )
			{
				in.pop( ((BYTE*)&m_data)+m_PropertyDeclare[i].nDataOffset,m_PropertyDeclare[i].len );
				if ( in.fail() )
				{
					return false;
				}
			}

#ifdef			ENTITY_PROP_OUTPUT
            PrintPropertyItemInfo(i, __FUNCTION__);
#endif
		}

        // todo 打印日志，以后去掉。角色的话，检测一下皮肤ID是否是对的
        if(m_pEntity != NULL && ((m_pEntity->getTypeMask() & MASK_ACTOR) == MASK_ACTOR))
        {
            int nSkin = *((int*)(((char*)&m_data) + m_PropertyDeclare[PROPERTY_SKIN].nDataOffset));
            if(!gClientGlobal->getSchemeCenter()->getSchemeSkinInfo()->getSkinShemeInfo(nSkin))
            {
                ErrorLn("onSerialize, error skin_id============" << nSkin);
            }
        }

		return true;
	}

	virtual bool deSerialize( rkt::obuf & out,int flag )
	{
		for ( int i=0;i<PROP_MAX;++i )
		{
			if ( m_PropertyDeclare[i].len>0 && m_PropertyDeclare[i].nFlag & flag )
			{
				out.push_back( ((BYTE*)&m_data)+m_PropertyDeclare[i].nDataOffset,m_PropertyDeclare[i].len );
				if ( out.fail() )
				{
					return false;
				}
			}

#ifdef			ENTITY_PROP_OUTPUT
				PrintPropertyItemInfo(i, __FUNCTION__);
#endif
		}

		return true;
	}

    virtual void onPropertyChange( int prop_id )
    {

    }

    // 服务器同步更新属性集
    virtual void synchronize_properties(void* data, int len)
    {
        if (data == NULL || len <= 0)
        {
            return;
        }

        // 解包
        int nInfoLen = sizeof(msg_entity_updateprop);
        while(len > nInfoLen)
        {
            msg_entity_updateprop* prop = (msg_entity_updateprop*)data;
            int nCurLen = nInfoLen + prop->nDataLen;
            if(len < nCurLen)
            {
                ErrorLn("sync property match failed!!!");
                break;
            }
            data = (char*)data + nCurLen;
            len -= nCurLen;

            setProperty_custom(prop->nPropertyID, (char*)prop+sizeof(*prop), prop->nDataLen);
        }
    }

    //////////////////////////////////////////////////////////////////////////
private:
#define CompareStringNoCase(str1, str2)\
		stricmp(str1,str2) == 0
#define OutputPropValue(buff, PtrType)\
		ErrorLn(szCall <<": nPropID="<<nPropID << ",value="<< (*((PtrType)buff)));

	// 输出单个属性信息
	void PrintPropertyItemInfo(int nPropID, char* szCall)
	{
		DataChunkPropertyDeclare & dataChunk = m_PropertyDeclare[nPropID];
		if(dataChunk.len < 0)
		{
			return;
		}

		BYTE * pData = ((BYTE*)&m_data)+ dataChunk.nDataOffset;

		// 输出信息		
		if( CompareStringNoCase(dataChunk.szType,"int") )
		{
			OutputPropValue(pData, int*);
		}
		else if( CompareStringNoCase(dataChunk.szType,"byte") )
		{
			OutputPropValue(pData, unsigned char*);
		}
		else if( CompareStringNoCase(dataChunk.szType,"short") )
		{
			OutputPropValue(pData, short*);
		}
		else if( CompareStringNoCase(dataChunk.szType,"bool") )
		{
			OutputPropValue(pData, bool*);
		}
		else if( CompareStringNoCase(dataChunk.szType,"Vector3") )
		{
			Vector3* pvet = (Vector3*)pData;
			Error(",x="<<pvet->x<<",y="<<pvet->y<<",z="<<pvet->z<< endl);
		}
		else if( CompareStringNoCase(dataChunk.szType,"unsigned long") )
		{
			OutputPropValue(pData, unsigned long*);
		}
		else
		{
			OutputPropValue(pData, char*);
		}
	}
};
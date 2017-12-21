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
#include "IServerGlobal.h"
#include "TypeName.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"
#include "IEntityAOI.h"
#include <string>
#include "ISchemeCenter.h"
#include "Property.h"
#include "Smart_ClassT.h"
#include "IPropertySet.h"

//#define ENTITY_PROP_OUTPUT
// 实体属性部件,使用时需要从这个模版继承
template<class DATA,int PROP_MAX=PROPERTY_MAX>
class EntityProperty : public __IEntityProperty, public IEventExecuteSink
{
public:
	static DataChunkPropertyDeclare  s_PropertyDeclare[PROP_MAX];
	DataChunkPropertyDeclare  m_PropertyDeclare[PROP_MAX];
	
	DATA        m_data;
	__IEntity*	m_pEntity;
	
	// 通过Framework创建的可安全跨线程访问的属性集
	IPropertySet * m_pPropertySet;

	// 是否创建实体(未创建实体时，不发属性改变事件)
	bool		m_bCreateEntity;

public:
	EntityProperty() : m_pEntity(0),m_pPropertySet(0),m_bCreateEntity(0) 
	{
		m_pPropertySet = gServerGlobal->getFramework()->create_propertyset( sizeof(DATA),&m_data );
		if ( m_pPropertySet==NULL )
		{
			ErrorLn("Property part onLoad failed!m_pPropertySet==NULL");
		}
	}

	virtual ~EntityProperty(){}

	//////////////////////////////**  IEntityPart **/////////////////////////////////////////////
	virtual short  getPartID() { return PART_PROPERTY; }

    virtual void restore()
    {
        // 释放前优先停止一些逻辑处理然后再释放
    }

	virtual __IEntity * getHost() { return m_pEntity; }

    virtual void destroy()
    {
        m_pEntity = NULL;

        // 有必要把m_pPropertySet备份然后置空吗? 
    }

	virtual void release() 
	{
		if ( m_pPropertySet != NULL )
		{
			m_pPropertySet->release();
			m_pPropertySet = NULL;
		}

		delete this; 
	}

	virtual bool onLoad(__IEntity* pEntity, int nLoginMode)
	{
		if (pEntity == NULL)
		{
			ErrorLn("Property part onLoad pEntity==NULL");
			return false;
		}
		m_pEntity = pEntity;

        onEntityLoadFinish();

        // 设置已创建
        m_bCreateEntity = true;

		return true;
	}

    // 切换英雄
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

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
		if ( len!=declare.len || declare.nDataOffset>=sizeof(DATA))
		{
			return false;
		}

		if ( m_pPropertySet==0 )
		{
			return false;
		}

		// 如果新老属性相等就直接返回
		if ( !m_pPropertySet->set_Property( declare.nDataOffset,declare.len,data,true) )
		{
			return false;
		}

		// 属性改变时的一些联动计算
		OnPropertyChange(prop_id,data,len);

		return true;
	}

	virtual int getProperty_custom(int prop_id, void* data, int len)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return 0;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(DATA))
		{
			return 0;
		}

		if ( m_pPropertySet==0 )
		{
			return 0;
		}

		if (! m_pPropertySet->get_Property( declare.nDataOffset,declare.len,data,len ) )
		{
			return 0;
		}

		return declare.len;
	}

	virtual bool getProperty_Bool( int prop_id)
	{
		bool bRet = false;
		int  nLen = sizeof(bRet);
		if ( getProperty_custom(prop_id,&bRet,nLen) != sizeof(bRet) )
		{
			return false;
		}

		return bRet;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_Bool( int prop_id,bool value)
	{
		return setProperty_custom(prop_id,&value,sizeof(value));
	}

	virtual int getProperty_Integer( int prop_id)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX )
		{
			return INVALID_INT_VALUE;
		}

		// 数据没有申明或者长度对不上
		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		if ( declare.len==0 || declare.nDataOffset>=sizeof(DATA))
		{
			return INVALID_INT_VALUE;
		}

		if ( m_pPropertySet==0 )
		{
			return INVALID_INT_VALUE;
		}

		int data = INVALID_INT_VALUE;
		if ( m_pPropertySet->get_Property( declare.nDataOffset,declare.len,&data,sizeof(data) )<=0 )
		{
			return INVALID_INT_VALUE;
		}

		switch(declare.len)
		{
		case sizeof(BYTE):
			return *(BYTE *)&data;
		case  sizeof(short):
			return *(/*unsigned*/ short *)&data;
		case sizeof(int):
			return data;
		}

		return INVALID_INT_VALUE;
	}

	virtual bool setProperty_Limit(int prop_id, int nMinLimit, int nMaxLimit)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX)
		{
			return false;
		}
		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		declare.nMinLimit=nMinLimit;
		declare.nMaxLimit=nMaxLimit;
		return true;
	}

    // 设置当前限制属性
    virtual void setCurLimitProperty(int prop_id, int max_prop_id)
    {
        setProperty_Limit(prop_id,0,getProperty_Integer(max_prop_id));
        setProperty_Integer(prop_id, getProperty_Integer(prop_id));
    }
	
	virtual bool setProperty_Integer( int prop_id,int value)
	{
		if ( prop_id<0 || prop_id>=PROP_MAX)
		{
			return false;
		}

		DataChunkPropertyDeclare & declare = m_PropertyDeclare[prop_id];
		makesure_minmax(value, declare.nMinLimit, declare.nMaxLimit);
		//只对移动速度，攻击速度，冷却时间，韧性，和暴击进行限定，修改值
		
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
	virtual std::string getProperty_String( int prop_id )
	{
		char data[MAX_PROPERTY_DATA_LEN];
		int len = getProperty_custom(prop_id,data,MAX_PROPERTY_DATA_LEN);
		if ( len<=0 )
		{
			return "";
		}

		return std::string(data,len);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	virtual bool setProperty_String( int prop_id,std::string value)
	{
		return setProperty_custom(prop_id, (void*)value.c_str(), 32);
	}

	//////////////////////////////**  ISerializable **/////////////////////////////////////////////
	virtual bool onSerialize( rkt::ibuffer & in,int flag )
	{
		if ( m_pPropertySet==0 )
		{
			return false;
		}

		for ( int i=0;i<PROP_MAX;++i )
		{
			if ( m_PropertyDeclare[i].len>0 && m_PropertyDeclare[i].nFlag & flag )
			{
				if ( in.space() < m_PropertyDeclare[i].len )
					return false;

				m_pPropertySet->set_Property( m_PropertyDeclare[i].nDataOffset,m_PropertyDeclare[i].len,in.current() );
				in.offset(m_PropertyDeclare[i].len);

				if ( in.fail() )
				{
					return false;
				}
#ifdef			ENTITY_PROP_OUTPUT
                PrintPropertyItemInfo(i, __FUNCTION__);
#endif
			}
		}
		
		return true;
	}

	virtual bool deSerialize( rkt::obuf & out,int flag )
	{
		if ( m_pPropertySet==0 )
		{
			return false;
		}

		for ( int i=0;i<PROP_MAX;++i )
		{
			if ( m_PropertyDeclare[i].len>0 && m_PropertyDeclare[i].nFlag & flag )
			{
				char * buff = (char *)&m_data;
				out.push_back( buff+m_PropertyDeclare[i].nDataOffset,m_PropertyDeclare[i].len );

#ifdef			ENTITY_PROP_OUTPUT
                PrintPropertyItemInfo(i, "deSerialize");
#endif
			}
		}

        // 校验皮肤ID是否合法(DEBUG版本)
#ifdef ENTITY_CHECK_SKIN
        if(m_pEntity != NULL && m_pEntity->getType() == TYPE_PLAYER_ROLE)
        {
            int nSkin = *((int*)(((char*)&m_data) + m_PropertyDeclare[PROPERTY_SKIN].nDataOffset));
            if(!gServerGlobal->getSchemeCenter()->getSchemeSkinInfo()->getSkinShemeInfo(nSkin))
            {
                ErrorLn(__FUNCTION__":["<< m_pEntity->getName() <<"] skin is invalid, pdbid="<< getProperty_Integer(PROPERTY_ID)
                    <<", heroid="<< getProperty_Integer(PROPERTY_VOCATION) <<", skin="<< nSkin << ", scene="<< getProperty_Integer(PROPERTY_SCENEID));

                __debugbreak();
            }
        }
#endif

		return true;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
	}

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len) { return false; }

	// 加载成功时回调
	virtual void onEntityLoadFinish(){}


	// 属性改变时的一些联动计算
	virtual void OnPropertyChange(int prop_id, void * data,int32 len){}


private:
#define CompareStringNoCase(str1, str2)\
		stricmp(str1,str2) == 0
#define OutputPropValue(buff, PtrType)\
		ErrorLn("["<<m_pEntity->getName().c_str()<<"]" <<szCall <<": nPropID="<<nPropID << ",value="<< (*((PtrType)buff)));

	// 输出单个属性信息
	void PrintPropertyItemInfo(int nPropID, char* szCall)
	{
		if(!m_pEntity)
		{
			return;
		}
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

/*******************************************************************
** 文件名:	TankService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据ITankService接口定义，自动生成的TankService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "ITankService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "TankService.h"

using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现ITankService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class TankService_Proxy : public ITankService
{
public:	
	IServiceContainer* m_pContainer;
	BYTE			m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE

	TankService_Proxy() : m_nServiceType(GAMELOGIC_SERVICE_TANK),m_pContainer(0)
	{
	}

	~TankService_Proxy(){
	}

	// 取得游戏逻辑服务类型
	BYTE getServiceType()
	{
		return m_nServiceType;
	}

    ////////////////////////////////////////////////////////////////////////////
    // 以下自动生成的消息打包函数，你需要检查
    ////////////////////////////////////////////////////////////////////////////
	
   //////////////////////////////////////////////////////////////////////////////////
   virtual UID  createTank( int id,Vector3 loc,void* context,int len )
    {
	    //OBUF(t_data);
	    //t_data << "createTank";
	    //t_data << id<<loc<<context<<len;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // UID vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return UID();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  destroyTank( UID uid )
    {
	    //OBUF(t_data);
	    //t_data << "destroyTank";
	    //t_data << uid;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getName( UID uid )
    {
	    //OBUF(t_data);
	    //t_data << "getName";
	    //t_data << uid;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // string vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getSceneID(  )
    {
	    //OBUF(t_data);
	    //t_data << "getSceneID";
	    ////t_data << ;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // int vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual Vector3  getLocation( UID uid )
    {
	    //OBUF(t_data);
	    //t_data << "getLocation";
	    //t_data << uid;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // Vector3 vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return Vector3();     // 请检查默认返回值是否正确
    };

   virtual void  setLocation( UID uid, Vector3 vLoc)
   {
	   //OBUF(t_data);
	   //t_data << "setLocation";
    //   string strFileInfo = (pFileInfo == NULL ? "" : pFileInfo);
	   //t_data << uid << vLoc << strFileInfo << nFileLine;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_NO_BLOCK);
   };

   virtual Vector3  getAngle( UID uid )
   {
	   //OBUF(t_data);
	   //t_data << "getAngle";
	   //t_data << uid;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // Vector3 vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}

	   return Vector3();     // 请检查默认返回值是否正确
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  handleMessage( UID uid,void *pData,int nLen )
    {
	    //OBUF(t_data);
	    //t_data << "handleMessage";
	    //t_data << uid<<msg;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_NO_BLOCK);
    };

   virtual void handleReplaceableMessage(UID uid, unsigned long ulKey,void * pMsg,int nLen)
   {
       //OBUF(t_data);
       //t_data << "handleMessage";
       //t_data << uid << msg;

       //m_pContainer->post_replaceable_message(ulKey, t_data.data(),t_data.size());
   }

   virtual void changeState(UID uidTank, int nState)
   {
	   //OBUF(t_data);
	   //t_data << "changeState";
	   //t_data << uidTank << nState;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   }

   virtual int getState(UID uidTank)
   {
	   //OBUF(t_data);
	   //t_data << "getState";
	   //t_data << uidTank;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // int vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return 0;     // 请检查默认返回值是否正确
   };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getPropertyType( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getPropertyType";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // string vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getPropertyFlag( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getPropertyFlag";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // int vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  getProperty_Bool( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getProperty_Bool";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_Bool( UID uid,int prop_id,bool value )
    {
	    //OBUF(t_data);
	    //t_data << "setProperty_Bool";
	    //t_data << uid<<prop_id<<value;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getProperty_Integer( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getProperty_Integer";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // int vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return int();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_Integer( UID uid,int prop_id,int value )
    {
	    //OBUF(t_data);
	    //t_data << "setProperty_Integer";
	    //t_data << uid<<prop_id<<value;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  addProperty_Integer( UID uid,int prop_id,int delta )
    {
	    //OBUF(t_data);
	    //t_data << "addProperty_Integer";
	    //t_data << uid<<prop_id<<delta;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getProperty_String( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getProperty_String";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // string vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_String( UID uid,int prop_id,string value )
    {
	    //OBUF(t_data);
	    //t_data << "setProperty_String";
	    //t_data << uid<<prop_id<<value;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual string  getProperty_custom( UID uid,int prop_id )
    {
	    //OBUF(t_data);
	    //t_data << "getProperty_custom";
	    //t_data << uid<<prop_id;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // string vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return string();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  setProperty_custom( UID uid,int prop_id,string value )
    {
	    //OBUF(t_data);
	    //t_data << "setProperty_custom";
	    //t_data << uid<<prop_id<<value;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual float  getSight( UID uid )
    {
	    //OBUF(t_data);
	    //t_data << "getSight";
	    //t_data << uid;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // float vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return float();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  setSight( UID uid,float radii )
    {
	    //OBUF(t_data);
	    //t_data << "setSight";
	    //t_data << uid<<radii;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual bool  isHide( UID uid )
    {
	    //OBUF(t_data);
	    //t_data << "isHide";
	    //t_data << uid;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	    //if ( resultBuf.good() && resultBuf.size() > 0  )
	    //{
		   // ibuffer t_in(resultBuf.data(),resultBuf.size());
		   // bool vReturn;
		   // t_in >> vReturn;
		   // if ( t_in.good() ) return vReturn;
	    //}

	    return bool();     // 请检查默认返回值是否正确
    };

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  setHide( UID uid,bool state )
    {
	    //OBUF(t_data);
	    //t_data << "setHide";
	    //t_data << uid<<state;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
    };

   virtual void AddEnmityValue(UID uidTank, UID uidDes, int nVal)
   {
	   //OBUF(t_data);
	   //t_data << "AddEnmityValue";
	   //t_data << uidTank << uidDes <<nVal;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   } 

   virtual void AddBroadEnmityValue(UID uidTank, UID uidDes, int nVal)
   {
	   //OBUF(t_data);
	   //t_data << "AddBroadEnmityValue";
	   //t_data << uidTank << uidDes <<nVal;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   } 

   /////////////////////// IBuffPart ///////////////////////////////////////////////
   virtual bool AddBuff(UID uidTank, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
   {
	   //OBUF(t_data);
	   //t_data << "AddBuff";
	   //t_data <<uidTank <<dwBuffID<<dwBuffLevel<<uidAdd<<dwTime<<nContextLen;
	   //if (pBuffContext && nContextLen > 0)
	   //{
		  // t_data.push_back(pBuffContext, nContextLen);
	   //}

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // bool vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return bool();
   }
   virtual bool RemoveBuff(UID uidTank,  DWORD dwBuffID, UID uidAdd, UID uidRemove)
   {
	   //OBUF(t_data);
	   //t_data << "RemoveBuff";
	   //t_data <<uidTank <<dwBuffID<<uidAdd<<uidRemove;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // bool vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return bool();
   }
   virtual bool IsExistBuff(UID uidTank, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
   {
	   //OBUF(t_data);
	   //t_data << "IsExistBuff";
	   //t_data << uidTank << dwBuffID << dwBuffLevel << uidAdd;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // bool vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return bool();
   }
   virtual bool IsExistBuffType(UID uidTank, DWORD dwBuffType)
   {
       //OBUF(t_data);
       //t_data << "IsExistBuffType";
       //t_data << uidTank << dwBuffType;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    bool vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}
       return bool();
   }
   virtual DWORD GetBuffLevel(UID uidTank, DWORD dwBuffID, UID uidAdd)
   {
	   //OBUF(t_data);
	   //t_data << "GetBuffLevel";
	   //t_data <<uidTank <<dwBuffID<<uidAdd;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // DWORD vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return 0;
   }
   virtual int GetLeftTime(UID uidTank, DWORD dwBuffID, UID uidAdd)
   {
	   //OBUF(t_data);
	   //t_data << "GetLeftTime";
	   //t_data <<uidTank <<dwBuffID<<uidAdd;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // int vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return -1;
   }
   virtual void SetAccumulateBuff(UID uidTank, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL ,int nContextLen=0)
   {
	   //OBUF(t_data);
	   //t_data << "SetAccumulateBuff";
	   //t_data <<uidTank <<dwBuffID<<uidAdd<<bOverlay;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   }
   virtual bool RemoveBuffByIndex(UID uidMonster,DWORD dwIndex)
   {
	   //OBUF(t_data);
	   //t_data << "RemoveBuffByIndex";
	   //t_data <<uidMonster <<dwIndex;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // bool vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return bool();
   }
   // 传送
   virtual bool Transport(UID uidTank, Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason)
   {
	   //OBUF(t_data);
    //   string strFileInfo = (pFileInfo == NULL ? "" : pFileInfo);
	   //t_data << "Transport";
	   //t_data <<uidTank <<newLoc<<bTransport<<nSceneID << strFileInfo << nFileLine;

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
	   //if ( resultBuf.good() && resultBuf.size() > 0  )
	   //{
		  // ibuffer t_in(resultBuf.data(),resultBuf.size());
		  // bool vReturn;
		  // t_in >> vReturn;
		  // if ( t_in.good() ) return vReturn;
	   //}
	   return bool();
   }

   // uid是否对camp可见
   virtual bool isInsight(UID uid, int camp)
   {
       //OBUF(t_data);
       //t_data << "isInsight";
       //t_data <<uid <<camp;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    bool vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}
       return bool();
   }

   virtual int getWarID(UID uid)
   {
       //obuf256 t_data;
       //t_data << "getWarID";
       //t_data << uid;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    int vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}

       return int();     // 请检查默认返回值是否正确
   }

   virtual bool chooseTarget( UID uid, int nTargetFilter, UID uidTarget )
   {
       //obuf256 t_data;
       //t_data << "chooseTarget";
       //t_data << uid << nTargetFilter << uidTarget;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    bool vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}

       return bool();     // 请检查默认返回值是否正确
   };

   virtual int getPKType( UID uid )
   {
       //obuf256 t_data;
       //t_data << "getPKType";
       //t_data << uid;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    int vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}

       return int();     // 请检查默认返回值是否正确
   };
   virtual bool MoveTo( UID uidTank,Vector3 newLoc ) {return false;}

   virtual int getPKState( UID uid )
   {
       //obuf256 t_data;
       //t_data << "getPKState";
       //t_data << uid;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    int vReturn;
       //    t_in >> vReturn;
       //    if ( t_in.good() ) return vReturn;
       //}

       return int();     // 请检查默认返回值是否正确
   };

   virtual void forceEndPK(UID uid)
   {
       //OBUF(t_data);
       //t_data << "AddEnmityValue";
       //t_data << uidTank << uidDes <<nVal;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   } 

   virtual int getPKCamp(UID uid)
   {
	   return int();
   }

   virtual BuffTypeList getBuff(UID uid, DWORD dwBuffType)
   {
       //obuf256 t_data;
       //t_data << "getBuff";
       //t_data << uid << dwBuffType;

       //rkt::obuf resultBuf;
       //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf,MSG_FLAG_READ_ONLY);
       //if ( resultBuf.good() && resultBuf.size() > 0  )
       //{
       //    ibuffer t_in(resultBuf.data(),resultBuf.size());
       //    BuffTypeList BuffType;
       //    t_in >> BuffType;
       //    if ( t_in.good() ) return BuffType;
       //}

       return BuffTypeList();     // 请检查默认返回值是否正确
   }

   // 移除所有Tank
   virtual void destroyAll()
   {
	   //OBUF(t_data);
	   //t_data << "destroyAll";

	   //rkt::obuf resultBuf;
	   //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual void  release(  )
    {
	    //OBUF(t_data);
	    //t_data << "release";
	    ////t_data << ;

	    //rkt::obuf resultBuf;
	    //m_pContainer->post_message( t_data.data(),t_data.size(),resultBuf);
    };

   // 添加效果 返回效果ID记录
   virtual	int mobaGoodsAddEffect(UID uid, int nEffectID, void * pAddContext, size_t len)
   {
       //if(m_pContainer == 0)
       //{
       //    WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
       //    return 0;
       //}

       //if (pAddContext == nullptr || len == 0)
       //    return 0;

       //OBUF(t_data);
       //t_data <<uid<<nEffectID << len;
       //t_data.push_back(pAddContext, len);
       //BUILD_MSG_BUFFER(INPCService::mobaGoodsAddEffect, t_data);

       //rkt::obuf resultBuf;
       //m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
       //RETURN_FROM_MSG_BUF(int);
       return int();     // 请检查默认返回值是否正确    
   }

   // 移除效果
   virtual	void mobaGoodsRemoveEffect(UID uid, int nID)
   {
       /*if(m_pContainer == 0)
       {
           WarningLn(__FUNCTION__ << ", failed, m_pContainer == 0");
           return;
       }

       BUILD_TYPE_MSG_CONTEXT_2( INPCService::mobaGoodsRemoveEffect, UID, uid, int ,nID);
       m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);*/
   }



   //////////////////////////////////////////////////////////////////////////////////
   // 实体通知主人
   virtual void entityNotifyMaster(UID uid, EntityBelongContext context)
   {

   }

   //////////////////////////////////////////////////////////////////////////////////
   virtual int  getMapID( UID uid )
   {
       return int();     // 请检查默认返回值是否正确
   };

   virtual void shift(UID uidMonster, void *pData,int nDataLen)
   {
   }
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class TankService_Stub
{
public:
	BYTE			  m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
	TankService_Proxy  m_proxy;
	TankService *      m_real_service;
     
public:
	TankService_Stub(TankService * pReal) : m_real_service(pReal), m_nServiceType(0){
	}

	// 启动
	virtual bool on_start( IServiceContainer* pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		int* p = (int*)data;
		int mapid = *p; p++;
		int scene = *p; p++;

		return m_real_service->on_start(mapid,scene);
	}

	// 停止
	virtual bool on_stop()
	{
        bool bReuslt = m_real_service->on_stop(); 
		return bReuslt;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		assertBreak;

////		if ( pMsg->context_len==0 || pMsg->context==0 )
////			return false;
////
////		ibuffer in(pMsg->context,pMsg->context_len);
////		BYTE byType;
////		const char * function_name;
////		in >> byType >> function_name;
////
////		// 校验一下类型，避免弄错
////		if (byType != m_proxy.getServiceType())
////		{
////			ErrorLn("TankService_Stub:handle_message() recieve error function name ["<< function_name <<"] , service type="<< byType);
////			return false;
////		}
////
////#ifdef SERVICE_STUB_FUNCTION_OUTPUT
////		TraceLn( __FUNCTION__ "() func="<< function_name);
////#endif
////        ////////////////////////////////////////////////////////////////////////////
////        // 以下自动生成的消息解包，你需要检查
////		
////		if ( TEST_SAME_FUNCTION(function_name,"createTank") )
////		{
////			int id;Vector3 loc;void* context;int len;
////			in >> id>>loc>>context>>len;
////			const UID & vReturn= m_real_service->createTank(id,loc,context,len);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"destroyTank") )
////		{
////			UID uid;
////			in >> uid;
////			const bool & vReturn= m_real_service->destroyTank(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getName") )
////		{
////			UID uid;
////			in >> uid;
////			const string & vReturn= m_real_service->getName(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getSceneID") )
////		{
////			;
////			//in >> ;
////			const int & vReturn= m_real_service->getSceneID();
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getLocation") )
////		{
////			UID uid;
////			in >> uid;
////			const Vector3 & vReturn= m_real_service->getLocation(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setLocation") )
////		{
////			UID uid;Vector3 vLoc;string strFileInfo;int nFileLine;
////			in >> uid >> vLoc >> strFileInfo >> nFileLine;
////			m_real_service->setLocation(uid, vLoc, strFileInfo.c_str(), nFileLine);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getAngle") )
////		{
////			UID uid;
////			in >> uid;
////			const Vector3 & vReturn= m_real_service->getAngle(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"handleMessage") )
////		{
////			UID uid;PACKAGE_PTR msg;
////			in >> uid>>msg;
////			m_real_service->handleMessage(uid,msg);
////			return true;
////		}
////
////		if ( TEST_SAME_FUNCTION(function_name,"getPropertyType") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const string & vReturn= m_real_service->getPropertyType(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getPropertyFlag") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const int & vReturn= m_real_service->getPropertyFlag(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getProperty_Bool") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const bool & vReturn= m_real_service->getProperty_Bool(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setProperty_Bool") )
////		{
////			UID uid;int prop_id;bool value;
////			in >> uid>>prop_id>>value;
////			const bool & vReturn= m_real_service->setProperty_Bool(uid,prop_id,value);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getProperty_Integer") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const int & vReturn= m_real_service->getProperty_Integer(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setProperty_Integer") )
////		{
////			UID uid;int prop_id;int value;
////			in >> uid>>prop_id>>value;
////			const bool & vReturn= m_real_service->setProperty_Integer(uid,prop_id,value);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"addProperty_Integer") )
////		{
////			UID uid;int prop_id;int delta;
////			in >> uid>>prop_id>>delta;
////			const bool & vReturn= m_real_service->addProperty_Integer(uid,prop_id,delta);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getProperty_String") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const string & vReturn= m_real_service->getProperty_String(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setProperty_String") )
////		{
////			UID uid;int prop_id;string value;
////			in >> uid>>prop_id>>value;
////			const bool & vReturn= m_real_service->setProperty_String(uid,prop_id,value);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getProperty_custom") )
////		{
////			UID uid;int prop_id;
////			in >> uid>>prop_id;
////			const string & vReturn= m_real_service->getProperty_custom(uid,prop_id);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setProperty_custom") )
////		{
////			UID uid;int prop_id;string value;
////			in >> uid>>prop_id>>value;
////			const bool & vReturn= m_real_service->setProperty_custom(uid,prop_id,value);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getSight") )
////		{
////			UID uid;
////			in >> uid;
////			const float & vReturn= m_real_service->getSight(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setSight") )
////		{
////			UID uid;float radii;
////			in >> uid>>radii;
////			m_real_service->setSight(uid,radii);
////			return true;
////		}
////
////		if ( TEST_SAME_FUNCTION(function_name,"isHide") )
////		{
////			UID uid;
////			in >> uid;
////			const bool & vReturn= m_real_service->isHide(uid);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"setHide") )
////		{
////			UID uid;bool state;
////			in >> uid>>state;
////			m_real_service->setHide(uid,state);
////			return true;
////		}		
////		if ( TEST_SAME_FUNCTION(function_name,"AddEnmityValue") )
////		{
////			UID uidTank, uidDes;int nVal;
////			in >> uidTank >> uidDes>>nVal;
////			m_real_service->AddEnmityValue(uidTank,uidDes,nVal);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"AddBroadEnmityValue") )
////		{
////			UID uidTank, uidDes;int nVal;
////			in >> uidTank >> uidDes>>nVal;
////			m_real_service->AddBroadEnmityValue(uidTank,uidDes,nVal);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"changeState") )
////		{
////			UID uidTank; int nState;
////			in >> uidTank >> nState;
////			m_real_service->changeState(uidTank,nState);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"getState") )
////		{
////			UID uidTank; 
////			in >> uidTank ;			
////			const int & vReturn= m_real_service->getState(uidTank);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"AddBuff") )
////		{
////			UID uidTank; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd; DWORD dwTime; int nContextLen = 0;
////			in >> uidTank >> dwBuffID>>dwBuffLevel>>uidAdd>>dwTime>>nContextLen;			
////			const bool & vReturn= m_real_service->AddBuff(uidTank,dwBuffID,dwBuffLevel,uidAdd,dwTime, in.current(), nContextLen);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"RemoveBuff") )
////		{
////			UID uidTank; DWORD dwBuffID; DWORD uidAdd; UID uidRemove;
////			in >> uidTank >> dwBuffID >> uidAdd >> uidRemove;
////			const bool & vReturn= m_real_service->RemoveBuff(uidTank,dwBuffID, uidAdd, uidRemove);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"IsExistBuff") )
////		{
////			UID uidTank; DWORD dwBuffID; DWORD dwBuffLevel; UID uidAdd;
////			in >> uidTank >> dwBuffID >> dwBuffLevel >> uidAdd;
////			const bool & vReturn= m_real_service->IsExistBuff(uidTank,dwBuffID,dwBuffLevel,uidAdd);
////			resultBuf << vReturn;
////			return true;
////		}
////        if ( TEST_SAME_FUNCTION(function_name,"IsExistBuffType") )
////        {
////            UID uidTank; DWORD dwBuffType;
////            in >> uidTank >> dwBuffType;
////            const bool & vReturn= m_real_service->IsExistBuffType(uidTank,dwBuffType);
////            resultBuf << vReturn;
////            return true;
////        }
////		if ( TEST_SAME_FUNCTION(function_name,"GetBuffLevel") )
////		{
////			UID uidTank; DWORD dwBuffID; UID uidAdd;
////			in >> uidTank >> dwBuffID>> uidAdd;
////			const DWORD & vReturn= m_real_service->GetBuffLevel(uidTank,dwBuffID,uidAdd);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"GetLeftTime") )
////		{
////			UID uidTank; DWORD dwBuffID; UID uidAdd;
////			in >> uidTank >> dwBuffID>> uidAdd;
////			const int & vReturn= m_real_service->GetLeftTime(uidTank,dwBuffID,uidAdd);
////			resultBuf << vReturn;
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"SetAccumulateBuff") )
////		{
////			UID uidTank; DWORD dwBuffID; UID uidAdd; bool bOverlay;
////			in >> uidTank >> dwBuffID >> uidAdd >> bOverlay ;
////			m_real_service->SetAccumulateBuff(uidTank,dwBuffID,uidAdd,bOverlay);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"RemoveBuffByIndex") )
////		{
////			UID uidMonster; DWORD dwIndex; 
////			in >> uidMonster >> dwIndex ;
////			m_real_service->RemoveBuffByIndex(uidMonster,dwIndex);
////			return true;
////		}
////		if ( TEST_SAME_FUNCTION(function_name,"Transport") )
////		{
////			UID uidTank;Vector3 newLoc;bool bTransport;int nSceneID;string strFileInfo; int nFileLine;
////			in >> uidTank >> newLoc >> bTransport >> nSceneID >> strFileInfo >> nFileLine;
////			const bool & vReturn= m_real_service->Transport(uidTank,newLoc,bTransport,nSceneID, strFileInfo.c_str(), nFileLine);
////			resultBuf << vReturn;
////			return true;
////		}
////
////        if ( TEST_SAME_FUNCTION(function_name,"isInsight") )
////        {
////            UID uidTank;int camp;
////            in >> uidTank >> camp;
////            const bool & vReturn= m_real_service->isInsight(uidTank,camp);
////            resultBuf << vReturn;
////            return true;
////        }
////
////        if(TEST_SAME_FUNCTION(function_name,"getWarID"))
////        {
////            UID uidTank;
////            in >> uidTank;
////            int vReturn = m_real_service->getWarID(uidTank);
////            resultBuf << vReturn;
////            return true;
////        }
////
////        if(TEST_SAME_FUNCTION(function_name,"getBuff"))
////        {
////            UID uid;
////            DWORD dwBuffType;
////            in >> uid >> dwBuffType;
////            BuffTypeList vReturn = m_real_service->getBuff(uid,dwBuffType);
////            resultBuf << vReturn;
////            return true;
////        }
////
////		if ( TEST_SAME_FUNCTION(function_name,"destroyAll") )
////		{
////			m_real_service->destroyAll();
////			return true;
////		}
////
////		if ( TEST_SAME_FUNCTION(function_name,"release") )
////		{
////			m_real_service->release();
////			return true;
////		}


//		ErrorLn("TankService_Stub::handle_message() has a function not handle, func="<< function_name);
		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
        m_proxy.m_pContainer = 0;

		if ( m_real_service )
		{
			m_real_service->release();
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//extern "C" __declspec(dllexport) IServiceStub * CreateTankService()
//{
//	return new TankService_Stub(new TankService);
//}
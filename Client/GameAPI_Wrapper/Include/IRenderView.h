/*******************************************************************
** 文件名:	IRenderView.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

			定义显示层和逻辑层通讯的游戏全局消息
********************************************************************/

#pragma once

#include "GameDef.h"
#include "Entity_ManagedDef.h"
#include "War_ManagedDef.h"
#include "ManagedCode.h"
namespace GameLogic
{



	// 发送命令给显示层
	// @param entityID   : 实体ID
	// @param cmdID      : 命令ID
	// @param nParam     : 整形参数
	// @param strParam   : 字符串参数
	// @param ptrParam   : 结构指针参数
	// @return           : 命令是否执行成功
	DELEGATE(bool, sendControllerCommand_ptr)(int cmdID, int nParam, sp_string strParam, sp_pointer ptrParam, int len, int nTickCount);

	// 创建实体
	// @param entityType : 实体类型ID
	// @return           : 实体序列号
	DELEGATE(int,createEntityView_ptr)(EntityViewItem entityInfo);

	// 加载模型
	// @param entityType : 模型信息
	// @return           : 是否加载成功
	DELEGATE(bool,LoadModel_ptr)(EntityViewItem entityInfo);

	// 删除实体
	// @param entityID   : 要删除的实体ID
	// @return           : 删除是否成功
	DELEGATE(bool,destroyEntityView_ptr)(int entityID);

	// 发送命令给实体
	// @param entityID   : 实体ID
	// @param cmdID      : 命令ID,具体定义见(EntityViewDef.h)
	// @param nParam     : 整形参数
	// @param strParam   : 字符串参数
	// @param ptrParam   : 结构指针参数
	// @return           : 命令是否执行成功
	DELEGATE(bool,sendEntityCommand_ptr)(int entityID, int cmdID, int nParam, sp_pointer strParam, sp_pointer ptrParam, int len, int nTickCount);

    // 加载预制体
    // @param entityType : 模型信息
    // @return           : 是否加载成功
    DELEGATE(bool,loadPrefab_ptr)(EntityViewItem entityInfo);

    // 加载皮肤
    // @param entityType : 模型信息
    // @return           : 是否加载成功
    DELEGATE(bool,loadSkin_ptr)(EntityViewItem entityInfo);

	STRUCT IRenderViewStruct
	{
	public:
		sendControllerCommand_ptr REF	sendControllerCommand_Handler;
		createEntityView_ptr REF		createEntityView_Handler;
		destroyEntityView_ptr REF		destroyEntityView_Handler;
		sendEntityCommand_ptr REF		sendEntityCommand_Handler;
		LoadModel_ptr REF				loadModel_Handler;
        loadPrefab_ptr REF              loadPrefab_Handler; 
        loadSkin_ptr REF                loadSkin_Handler;
	};

#ifndef _MANAGED
	struct IRenderViewProxy
	{
		virtual void  sendControllerCommand( int cmdID,int nParam,const char* strParam,void* ptrParam, int len) = 0;
		virtual void  loadModel( EntityViewItem& ite) = 0;
		virtual void  createEntityView( EntityViewItem& ite) = 0;
		virtual void destroyEntityView( int entityID, int nEntityType) = 0;
		virtual void  sendEntityCommand( int entityID, int nEntityType, int cmdID,int nParam,const char* strParam,void* ptrParam, int len) = 0;
        virtual void loadPrefab( EntityViewItem& ite) = 0;
        virtual void loadSkin( EntityViewItem& ite) = 0;
	};
#endif
}
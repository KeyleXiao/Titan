/*******************************************************************
** 文件名:	IStage_Manager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/3/23  21:15
** 版  本:	1.0
** 描  述:	游戏状态管理接口
** 应  用:  	

********************************************************************/

#include "Stdafx.h"
#include "Stage_ManagedDef.h"

using namespace Stage_ManagerDef;

struct IStage_Manager
{
    virtual bool gotoState( EStage_State nState,void * context=NULL, int len = 0 ) = 0;
    virtual EStage_State getPrevState() = 0 ;

    virtual EStage_State getCurState() = 0;
};

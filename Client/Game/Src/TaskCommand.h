/*******************************************************************
** 文件名: TaskCommand.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 5/24/2017
** 版  本: 1.0
** 描  述: command结构
            
********************************************************************/

#pragma once

#ifndef __TASK_COMMAND_H__
#define __TASK_COMMAND_H__

// TASK调试开关
// #define MOUDLE_TASK_DEBUG



// 命令类型
enum TASK_COMMAND_TYPE
{
    COMMAND_CONTROLLER,             // sendControllerCommand
    COMMAND_LOAD_MODEL,             // loadModel
    COMMAND_CREATE_ENTITY_VIEW,     // createEntityView
    COMMAND_DESTROY_ENTITY_VIEW,    // destroyEntityView
    COMMAND_SEND_ENTITY_COMMAND,    // sendEntityCommand
    COMMAND_LOAD_PREFAB,            // loadPrefab
    COMMAND_LOAD_SKIN,              // loadSkin

    COMMAND_ENTITYEVENT,            // 实体命令 OnEntityEvent
    COMMAND_VIEWEVENT,              // 显示命令 OnGameViewEvent
    COMMAND_TRACE,                  // LOG命令 traceLog    
};


// 命令结构
struct task_command
{
    int         nCmdType;           // 命令类别 TASK_COMMAND_TYPE
    int         nCmdID;             // 命令ID
    obuf256     context;            // 内容
};


#define post_task_message(MSG_ID, nCmd, ob)         postMessage(MSG_ID, nCmd, ob.data(), ob.size(), false)
// 投递VIP消息
#define post_task_vip_message(MSG_ID, nCmd, ob)     postMessage(MSG_ID, nCmd, ob.data(), ob.size(), true)

#endif // __TASK_COMMAND_H__ 
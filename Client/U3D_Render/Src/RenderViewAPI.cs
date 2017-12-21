using System;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;

using System.Threading;

using GameLogic;

namespace U3D_Render
{
    // 定义实体ID
    using ENTITY_ID = System.Int32;

    // 显示层接口实现
    public class RenderViewAPI
    {
        /// <summary>
        /// 发送GameView命令
        /// </summary>
        /// <param name="entityID">实体ID</param>
        /// <param name="cmdID">命令ID，参见GameViewDef.h</param>
        /// <param name="nParam">整形参数</param>
        /// <param name="strParam">字符串参数</param>
        /// <param name="ptrParam">指针参数</param>
        /// <returns></returns>
	    static public bool sendControllerCommand_Handler( int cmdID,int nParam,string strParam,IntPtr ptrParam, int len, int nTickCount)
        {
            return GameViewCommandHandler.onCommand(cmdID, nParam, strParam, ptrParam, len, nTickCount);
        }

        /// <summary>
        /// 加载模型
        /// </summary>
        /// <param name="SkinModelinfo">模型信息</param>
        /// <returns>是否成功</returns>
        static public bool LoadModel_Handler(EntityViewItem ite)
        {
            //Trace.LogWarning("LoadModel_Handler已经废弃，请调用loadPrefab_Handler");
            //ModelManager.LoadModel(ite.nHeroID, ite.szModelPath, ite.EntityType);
            return true;
        }

        /// <summary>
        /// 创建实体
        /// </summary>
        /// <param name="id">实体ID</param>
        /// <param name="SkinModelinfo">模型信息</param>
        /// <returns></returns> 
        static public ENTITY_ID createEntityView_Handler(EntityViewItem ite)
        {
            return EntityFactory.entry_createEntry(ite);
        }

        /// <summary>
        /// 删除实体
        /// </summary>
        /// <param name="id">实体ID</param>
        /// <returns>是否删除成功</returns>
	    static public bool destroyEntityView_Handler( ENTITY_ID entityID )
        {
            return EntityFactory.entry_destroyEntry(entityID);
        }


        /// <summary>
        /// 发送EntityView命令
        /// </summary>
        /// <param name="entityID">实体ID</param>
        /// <param name="cmdID">命令ID，参见EntityViewDef.h</param>
        /// <param name="nParam">整形参数</param>
        /// <param name="strParam">字符串参数</param>
        /// <param name="ptrParam">指针参数</param>
        /// <returns></returns>
        static public bool sendEntityCommand_Handler(ENTITY_ID entityID, int cmdID, int nParam, IntPtr strParam, IntPtr ptrParam, int len, int nTickCount)
        {
            return EntityFactory.entry_sendCommand(entityID, cmdID, nParam, Marshal.PtrToStringAnsi(strParam), ptrParam, len, nTickCount);
        }

        /// <summary>
        /// 加载预制体
        /// </summary>
        /// <param name="SkinModelinfo">模型信息</param>
        /// <returns>是否成功</returns>
        static public bool loadPrefab_Handler(EntityViewItem ite)
        {
            //Trace.Log("预制体ID=" + ite.nSkinID + ", 预制体路径=" + ite.szModelPath + ",英雄ID=" + ite.nHeroID + ", 名字=" + ite.szName);
            //return PrefabManager.LoadPrefab((ENTITY_TYPE)ite.EntityType,ite.szModelPath);
            return true;
        }

        /// <summary>
        /// 加载皮肤
        /// </summary>
        /// <param name="SkinModelinfo">模型信息</param>
        /// <returns>是否成功</returns>
        static public bool loadSkin_Handler(EntityViewItem ite)
        {
            return SkinManager.LoadSkin(ite);
        }

        // 定义成全局变量不然垃圾回收掉
        static public IntPtrHelper g_RenderViewHelper = new IntPtrHelper();
        static public IRenderViewStruct g_RenderView = new IRenderViewStruct();

        static public bool Start()
        {
            g_RenderView.sendControllerCommand_Handler = new sendControllerCommand_ptr(sendControllerCommand_Handler);
            g_RenderView.createEntityView_Handler = new createEntityView_ptr(createEntityView_Handler);
            g_RenderView.destroyEntityView_Handler = new destroyEntityView_ptr(destroyEntityView_Handler);
            g_RenderView.sendEntityCommand_Handler = new sendEntityCommand_ptr(sendEntityCommand_Handler);
            g_RenderView.loadModel_Handler = new LoadModel_ptr(LoadModel_Handler);
            g_RenderView.loadPrefab_Handler = new loadPrefab_ptr(loadPrefab_Handler);
            g_RenderView.loadSkin_Handler = new loadSkin_ptr(loadSkin_Handler);

            return true;
        }

        static public void Stop()
        {
            //发送下线消息
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_EXITGAME);
        }
    }
}
/*******************************************************************
** 文件名:	GFxLoadingWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/10/8
** 版  本:	1.0
** 描  述:	GFx 载入界面
** 应  用:  拆分自GFxLoadingWin，将在CS中执行AS方法的方法移到这里

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;


public partial class GFxLoadingWin
{


    #region 战场加载界面
    //添加玩家信息框
    //AttachWarPlayerFrame(_nCurIndex,_nCampPlayerCount,_nCamp,_nHeroID,_strVocationName,_strPlayerName,_nSkill1ID,_nSkill2ID)
    public void AttachWarPlayerFrame(int _nIndex,cmd_match_load_scene_playerinfo _data)
    {
        if(dicPlayerFrameTable == null)
        {
            dicPlayerFrameTable = new System.Collections.Generic.Dictionary<uint, FPlayerFrameData>();
        }
        //AttachWarPlayerFrame(_nCurIndex:int,_nCampPlayerCount:int,_nCamp:int,_nHeroID:int,_strVocationName:String,_strPlayerName:String,_nSkill1ID:int = 1,_nSkill2ID:int = 1)
        if(_data.nHeroID <= 0 || !Value.IsValueValid(vWarLoadingMgr))
        {
            Trace.LogWarning("进入战场加载界面添加英雄信息框时，英雄ID不能为0");
            return;
        }
        bool isComputer = _data.nIsComputer == 1 ? true:false;

        if (vWarLoadingMgr.Invoke("AttachWarPlayerFrame", _nIndex, _data.nCampPlayerCount, _data.nCamp, _data.nHeroID, _data.szVocationName, _data.szPlayerName, _data.nSupportSkill[0], _data.nSupportSkill[1], isComputer) == null)
        {
            Trace.LogError("error call AttachWarPlayerFrame failed");
            return;
        }
        if(!dicPlayerFrameTable.ContainsKey(_data.idActor))
            dicPlayerFrameTable.Add(_data.idActor, new FPlayerFrameData(_nIndex, _data.nCampPlayerCount, _data.nCamp));
    }

    //设置信息框延迟与加载进度
    //SetPlayerLoadingFrameInfo(_nCurIndex,_nCampPlayerCount,_nCamp,_nDelay,_nLoadProgress)
    public void SetPlayerLoadingFrameInfo(cmd_load_scene_rate _data)
    {
        if(dicPlayerFrameTable == null)
        {
            Trace.LogWarning("GVIEWCMD_MATCH_LOAD_SCENE_PLAYERINFO 这个消息比 GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE先来，或者没执行。");
            return;
        }
        if(Value.IsValueValid(vWarLoadingMgr) && dicPlayerFrameTable.ContainsKey(_data.idActor))
        {
            int nIndex = dicPlayerFrameTable[_data.idActor].nIndex;
            int nCampPlayerCount = dicPlayerFrameTable[_data.idActor].nCampPlayerCount;
            int nCamp = dicPlayerFrameTable[_data.idActor].nCamp;

            if (vWarLoadingMgr.Invoke("SetPlayerLoadingFrameInfo", nIndex, nCampPlayerCount, nCamp,_data.nPing,_data.nRate) == null)
            {
                Trace.LogError("error call SetPlayerLoadingFrameInfo failed");
                return;
            }

        }
    }

    //移除所有信息框
    //DisAttachWarPlayerFrame()
    public void DisAttachWarPlayerFrame()
    {
        if(Value.IsValueValid(vWarLoadingMgr))
        {
            if (vWarLoadingMgr.Invoke("DisAttachWarPlayerFrame") == null)
            {
                Trace.LogError("error call DisAttachWarPlayerFrame failed");
                return;
            }

        }
    }

    public void AttachWarLoadingMgr()
    {
        if(Value.IsValueValid(MyRoot))
        {
            if(MyRoot.Invoke("AttachWarLoadingMgr") == null)
            {
                Trace.LogError("call AttachWarLoadingMgr failed.");
                return;
            }
        }
    }

    public void DisAttachWarLoadingMgr()
    {
        if(Value.IsValueValid(MyRoot))
        {
            if(MyRoot.Invoke("DisAttachWarLoadingMgr") == null)
            {
                Trace.LogError("call DisAttachWarLoadingMgr failed.");
                return;
            }
        }
    }
#endregion

    #region 共用部分
    //加载界面背景图片
    public void SetLoadingWinBG(string bgName)
    {
        if (MyRoot.Invoke("AttachLoadingBg", bgName) == null)
        {
            Trace.LogError("call AttachLoadingBg  failed");
            return;
        }
    }

    public void DisAttachLoadingBg()
    {
        if (MyRoot.Invoke("DisAttachLoadingBg") == null)
        {
            Trace.LogError("call DisAttachLoadingBg failed");
            return;
        }
    }
    #endregion
}

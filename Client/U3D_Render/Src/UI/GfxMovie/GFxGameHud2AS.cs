///*******************************************************************
//** 文件名:	GFxGameHud2AS.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	吴欣伟
//** 日  期:	2015.9.28
//** 版  本:	1.0
//** 描  述:	1、拆分C#中与AS通讯部分逻辑
//** 应  用:  显示玩家当前状态和基本按钮	
// *          对应Flash Layout文件：GameHud.swf/gfx
//**************************** 修改记录 ******************************
//** 修改人: 
//** 日  期: 
//** 描  述: 
// *         
//********************************************************************/
//using UnityEngine;
//using System;
//using System.Collections;
//using System.Collections.Generic;
//using Scaleform;
//using GameLogic;

//public partial class GFxGameHud
//{
//    #region C#->AS
//    /// <summary>
//    /// 更新双方阵营的战斗统计
//    /// </summary>
//    /// <param name="_data"></param>
//    public void SetCamp(CreateMainHeroEntityEventArgs e)
//    {
//        if (!Value.IsValueValid(warTopRight))
//            return;

//        MainHeroCampID = EntityFactory.getEntityViewByID(e.MainHeroUID).Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
//    }

//    /// <summary>
//    /// 取得阵营信息
//    /// </summary>
//    /// <param name="_data"></param>
//    public int GetCamp()
//    {
//        return MainHeroCampID;
//    }

//    /// <summary>
//    /// 更新双方阵营的战斗统计
//    /// </summary>
//    /// <param name="_data"></param>
//    public void UpdateCampCombatStatistic(int OurKills, int OpponentKills)
//    {
//        if (!Value.IsValueValid(warTopRight))
//            return;

//        if (null == warTopRight.Invoke("SetKillNum_Our", OurKills))
//        {
//            Trace.LogError("Invoke SetKillNum_out failed!");
//        }

//        if (null == warTopRight.Invoke("SetKillNum_Opponent", OpponentKills))
//        {
//            Trace.LogError("Invoke SetKillNum_out failed!");
//        }

//    }

//    /// <summary>
//    /// 更新英雄的战斗统计
//    /// </summary>
//    public void UpdateHeroCombatStatisitc(int heroKills, int heroDeaths, int heroAssist)
//    {
//        if (!Value.IsValueValid(warTopRight))
//            return;

//        if (heroKills >= 100 || heroAssist >= 100 || heroDeaths >= 100)
//        {
//            Trace.LogWarning("当前英雄击杀数,助攻数,死亡数超过100，显示不完全");
//        }
//        warTopRight.Invoke("SetHeroCombatInfo", heroKills, heroDeaths, heroAssist);
//    }
//    #endregion

//}
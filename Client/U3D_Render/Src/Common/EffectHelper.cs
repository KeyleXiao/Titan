/*******************************************************************
** 文件名:	EffectHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李界华
** 日  期:	2016-3-17
** 版  本:	1.0
** 描  述:	播放效果辅助类

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;


/// <summary>
/// 缓存优先级
/// </summary>
public enum CachePriority
{
    PRIORITY_LOW = 1,           // 低
    PRIORITY_MEDIUM = 2,             // 中
    PRIORITY_HIGH = 4,            // 高
    PRIORITY_VERY_HIGH = 8,              // 非常高
}

public class EffectHelper
{    
     public static bool Play(ref U3D_Render.EntityView ev, int effectID, int src, int dest, Vector3 toPos, float distance = .0f
         , float range = .0f, float attackSpeed = 1.0f, int feedbackID = 0)
    {
        cmd_creature_skillview cmdInfo = new cmd_creature_skillview();

        cmdInfo.nSrcEntityID = src;		    // 攻击者EntityID
        cmdInfo.nTargetEntityID = dest;	    // 受击者EntityID
        cmdInfo.nFeedBackID = feedbackID;		// 反馈ID，当发生碰撞时返回此ID(客户端做一些碰撞效果使用，暂时无用，先留着)
        cmdInfo.fTargetPos_x = toPos.x;		// 目标坐标x
        cmdInfo.fTargetPos_y = toPos.y;		// 目标坐标y
        cmdInfo.fTargetPos_z = toPos.z;		// 目标坐标z

        cmdInfo.fDistance = distance;		    // 技能距离
        cmdInfo.fAttackRange = range;		    // 攻击范围
        cmdInfo.fPASD = attackSpeed;		    // 攻击速度

        return Play(ref ev, effectID, cmdInfo);
    }

    public static bool Play(ref U3D_Render.EntityView ev, int effectID, cmd_creature_skillview cmdInfo)
    {
        //光效开关
        if (!Initialize.Instance || !OptimitzedControlPlane.Instance.EnabledLightingEffect)
        {
            return true;
        }

        LightEffectParam param = new LightEffectParam();
        param.nFeedbackID = cmdInfo.nFeedBackID;
        param.bIsHero = ev.IsHero;
        param.nSrcEntityID = ev.ID;
        param.nTargetEntityID = cmdInfo.nTargetEntityID;
        param.srcObject = ev.gameObject;
        param.nCreatorSide = cmdInfo.nCreatorSide;
        if (cmdInfo.nSrcEntityID > 0)
        {
            param.nSrcEntityID = cmdInfo.nSrcEntityID;
            param.srcObject = EntityFactory.getEntityGameObjectByID(cmdInfo.nSrcEntityID);
        }
        param.fDistance = cmdInfo.fDistance;
        param.fAttackRange = cmdInfo.fAttackRange;
        param.speed = cmdInfo.fPASD;
        if (cmdInfo.nTargetEntityID != 0)
        {
            param.targetObject = EntityFactory.getEntityGameObjectByID(cmdInfo.nTargetEntityID);
        }
        param.targetPos = new Vector3(cmdInfo.fTargetPos_x, cmdInfo.fTargetPos_y, cmdInfo.fTargetPos_z);
        if(cmdInfo.bFlyToTargetPos != 0)
        {
            param.nEffectFlag |= (int)LightEffectParam.EFFECT_FLAG.FlyToTargetPos;
        }

        param.sourcePos = new Vector3(cmdInfo.fSrcPos_x,cmdInfo.fSrcPos_y,cmdInfo.fSrcPos_z);
        if(cmdInfo.bFromSrcPos != 0)
        {
            param.nEffectFlag |= (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos;
        }

        LightingEffectManager manager = ev.LightingEffectManager;
        if (manager == null)
            return false;

        // 开始光效
        manager.AddLighting(effectID, param);

        // 输出光效信息
        if (manager.showLog)
        {
            Trace.Log("Add Lighting" + effectID + ". Entity Id=" + ev.ID + " SrcID="+param.nSrcEntityID);
            Trace.Log("TargetID=" + cmdInfo.nTargetEntityID + ". TargetPos=(" + cmdInfo.fTargetPos_x + "," + cmdInfo.fTargetPos_y + "," + cmdInfo.fTargetPos_z + ").");
        }

        return true;
    }

    //使用这个接口会返回LightingEffect，可以通过强制转换控制光效的属性
    public static LightingEffectBehavior PlayAndReturnEffect(ref U3D_Render.EntityView ev, int effectID, int src, int dest, Vector3 toPos, float distance = .0f
         , float range = .0f, float attackSpeed = 1.0f, int feedbackID = 0)
    {
        cmd_creature_skillview cmdInfo = new cmd_creature_skillview();

        cmdInfo.nSrcEntityID = src;		    // 攻击者EntityID
        cmdInfo.nTargetEntityID = dest;	    // 受击者EntityID
        cmdInfo.nFeedBackID = feedbackID;		// 反馈ID，当发生碰撞时返回此ID(客户端做一些碰撞效果使用，暂时无用，先留着)
        cmdInfo.fTargetPos_x = toPos.x;		// 目标坐标x
        cmdInfo.fTargetPos_y = toPos.y;		// 目标坐标y
        cmdInfo.fTargetPos_z = toPos.z;		// 目标坐标z

        cmdInfo.fDistance = distance;		    // 技能距离
        cmdInfo.fAttackRange = range;		    // 攻击范围
        cmdInfo.fPASD = attackSpeed;		    // 攻击速度

        return PlayAndReturnEffect(ref ev, effectID, cmdInfo);
    }

    //使用这个接口会返回LightingEffect，可以通过强制转换控制光效的属性
    public static LightingEffectBehavior PlayAndReturnEffect(ref U3D_Render.EntityView ev, int effectID, cmd_creature_skillview cmdInfo)
    {
        //光效开关
        if (!Initialize.Instance || !OptimitzedControlPlane.Instance.EnabledLightingEffect)
        {
            return null;
        }

        LightEffectParam param = new LightEffectParam();
        param.nFeedbackID = cmdInfo.nFeedBackID;
        param.bIsHero = ev.IsHero;
        param.nSrcEntityID = ev.ID;
        param.nTargetEntityID = cmdInfo.nTargetEntityID;
        param.srcObject = ev.gameObject;
        if (cmdInfo.nSrcEntityID > 0)
        {
            param.nSrcEntityID = cmdInfo.nSrcEntityID;
            param.srcObject = EntityFactory.getEntityGameObjectByID(cmdInfo.nSrcEntityID);
        }
        param.fDistance = cmdInfo.fDistance;
        param.fAttackRange = cmdInfo.fAttackRange;
        param.speed = cmdInfo.fPASD;
        if (cmdInfo.nTargetEntityID != 0)
        {
            param.targetObject = EntityFactory.getEntityGameObjectByID(cmdInfo.nTargetEntityID);
        }
        param.targetPos = new Vector3(cmdInfo.fTargetPos_x, cmdInfo.fTargetPos_y, cmdInfo.fTargetPos_z);
        param.sourcePos = new Vector3(cmdInfo.fSrcPos_x, cmdInfo.fSrcPos_y, cmdInfo.fSrcPos_z);
        if (cmdInfo.bFromSrcPos != 0)
        {
            param.nEffectFlag |= (int)LightEffectParam.EFFECT_FLAG.CustomSourcePos;
        }
        param.nCreatorSide = cmdInfo.nCreatorSide;

        LightingEffectManager manager = ev.LightingEffectManager;
        if (manager == null)
            return null;

        // 开始光效
        LightingEffectBehavior eff = manager.AddLighting(effectID, param);

        // 输出光效信息
        if (manager.showLog)
        {
            Trace.Log("Add Lighting" + effectID + ". Entity Id=" + ev.ID + " SrcID=" + param.nSrcEntityID);
            Trace.Log("TargetID=" + cmdInfo.nTargetEntityID + ". TargetPos=(" + cmdInfo.fTargetPos_x + "," + cmdInfo.fTargetPos_y + "," + cmdInfo.fTargetPos_z + ").");
        }

        return eff;
    }

    /// <summary>
    /// 预加载某一类光效
    /// </summary>
    /// <param name="nType"></param>
    public static void LoadTypePreLight(ASpeedGame.Data.Scheme.LightType nType, CachePriority mask)
    {
        LoadTypePreLight(StageManager.Instance.getMapID(), nType, mask);
    }
    /// <summary>
    /// 预加载某一类光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    public static void LoadTypePreLight(int nMapID, ASpeedGame.Data.Scheme.LightType nType, CachePriority mask)
    {
        // 加载必须要加载的光效
        ASpeedGame.Data.Scheme.TypeLightNode typeLight = ASpeedGame.Data.Scheme.SchemePreLoadLight.Instance.get(nMapID, nType);
        if (typeLight == null || typeLight.Collection == null || typeLight.Collection.Count == 0)
        {
            return;
        }

        List<Pre_Load_Light_Info> ls = new List<Pre_Load_Light_Info>();

        foreach (var item in typeLight.Collection)
        {
            ASpeedGame.Data.Scheme.ObjectLightNode node = item.Value;

            Pre_Load_Light_Info info = new Pre_Load_Light_Info();
            info.nLightType = nType;        //光效类型
            info.nMask |= (int)mask;          //标志位
            info.effectIDList = node.Collection;

            ls.Add(info);
        }
        
        LightingEffectFactory.Instance.RecvPreLoadLinghtEffectInfo(ls);
    }

    /// <summary>
    /// 预加载光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    /// <param name="nObjID"></param>
    public static void LoadPreLight(ASpeedGame.Data.Scheme.LightType nType, int nObjID, CachePriority mask)
    {
        LoadPreLight(StageManager.Instance.getMapID(), nType, nObjID, mask);
    }

    /// <summary>
    /// 预加载光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    /// <param name="nObjID"></param>
    public static void LoadPreLight(int nMapID, ASpeedGame.Data.Scheme.LightType nType, int nObjID, CachePriority mask)
    {
        // 加载必须要加载的光效
        ASpeedGame.Data.Scheme.ObjectLightNode light = ASpeedGame.Data.Scheme.SchemePreLoadLight.Instance.get(nMapID, nType, nObjID);
        if (light == null)
        {
            return;
        }
        Pre_Load_Light_Info info = new Pre_Load_Light_Info();
        info.nLightType     = nType;            //光效类型
        info.nMask          |= (int)mask;        //标志位
        info.effectIDList   = light.Collection;

        List<Pre_Load_Light_Info> ls = new List<Pre_Load_Light_Info>();
        ls.Add(info);

        LightingEffectFactory.Instance.RecvPreLoadLinghtEffectInfo(ls);
    }


    /// <summary>
    /// 预加载光效 
    /// </summary>
    /// <param name="nType"></param>
    /// <param name="objs"></param>
    public static void LoadPreLights(ASpeedGame.Data.Scheme.LightType nType, System.Collections.Generic.IEnumerable<int> objs, CachePriority mask)
    {
        LoadPreLights(StageManager.Instance.getMapID(), nType, objs, mask);
    }

    /// <summary>
    /// 预加载光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    /// <param name="objs"></param>
    public static void LoadPreLights(int nMapID, ASpeedGame.Data.Scheme.LightType nType, System.Collections.Generic.IEnumerable<int> objs, CachePriority mask) 
    {
        if (objs == null)
        {
            return;
        }

        List<Pre_Load_Light_Info> ls = new List<Pre_Load_Light_Info>();
        foreach (int id in objs)
        {
            // 加载必须要加载的光效
            ASpeedGame.Data.Scheme.ObjectLightNode light = ASpeedGame.Data.Scheme.SchemePreLoadLight.Instance.get(nMapID, nType, id);
            if (light == null)
            {
                continue;
            }

            Pre_Load_Light_Info info = new Pre_Load_Light_Info();
            info.nLightType = nType;        //光效类型
            info.nMask |= (int)mask;         //标志位
            info.effectIDList = light.Collection;

            ls.Add(info);
        }

        if (ls.Count > 0)
        {
            LightingEffectFactory.Instance.RecvPreLoadLinghtEffectInfo(ls);
        }
    }

    /// <summary>
    /// 卸载已加载好的预加载光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    /// <param name="nObjID"></param>
    public static void UnloadPreLight(ASpeedGame.Data.Scheme.LightType nType, int nObjID)
    {
        UnloadPreLight(StageManager.Instance.getMapID(), nType, nObjID);
    }
    /// <summary>
    /// 卸载已加载好的预加载光效
    /// </summary>
    /// <param name="nMapID"></param>
    /// <param name="nType"></param>
    /// <param name="nObjID"></param>
    public static void UnloadPreLight(int nMapID, ASpeedGame.Data.Scheme.LightType nType, int nObjID)
    {
        ASpeedGame.Data.Scheme.ObjectLightNode light = ASpeedGame.Data.Scheme.SchemePreLoadLight.Instance.get(nMapID, nType, nObjID);
        if (light == null || light.Collection.Count == 0)
        {
            return;
        }

        LightingEffectFactory.Instance.ClearCache(light.Collection);
    }
}
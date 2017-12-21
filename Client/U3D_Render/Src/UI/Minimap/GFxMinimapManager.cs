/*******************************************************************
** 文件名:	GFxMinimapManager.cs
** 修改人: 陈俊涛
** 日  期: 2015/02/09
** 描  述: 添加事件管理
 * 1、接口 所有实体移动添加信息
 * 2、保存所有显示的元素
 * 3、元素是否进行修改后，进行更新
** 版  本:	1.0
** 应  用:  
 * 
*******************************************************************/
using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SPELL;
using U3D_Render;

using GameLogic;
// 定义实体ID
using ENTITY_ID = System.Int32;


// 处理逻辑层发过来的命令
public class GFxMinimapManager : Singleton<GFxMinimapManager>
{
    // 地图的信息
    private GFxMinimap m_Minimap = null;
    private int m_MainHeroCamp = -1;

    private bool m_bCreated = false;

    // 构造
    public GFxMinimapManager()
    {
        DestoryMinimap();
        //m_Minimap = SFGFxMovieManager.Instance.CreateWindow<GFxMinimap>("GFxMinimap", "GFx/Minimap.swf", SFGFxMovieManager.DefaultBGColor);

        //订阅更新数值属性消息
        EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);

        SoldierCamera.SwitchGodEyeModeEvent += OnSwitchGodEyeMode;

        m_bCreated = false;
    }

    private bool IsAllowShow(ENTITY_ID id)
    {
        EntityView ev = EntityFactory.getEntityViewByID(id);
        if (ev == null)
        {
            return false;
        }

        // 判断mask标记
        int nMask = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
        if (ev.IsHero)
        {
            if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_SELF_MINIMAP) != 0)
            {
                return false;
            }
        }
        else
        {
            if (ev.CampFlag == CampFlag.CampFlag_Friend)
            {
                if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_FRIEND_MINIMAP) != 0)
                {
                    return false;
                }
            }
            else if (ev.CampFlag == CampFlag.CampFlag_Enemy)
            {
                if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_ENEMY_MIMIMAP) != 0)
                {
                    return false;
                }
            }
        }
 
        // 死亡状态 不允许显示
        if (ev.StateMachine.GetState() == GameLogic.EntityState.Dead)
        {
            return false;
        }

        // 没视野 不允许显示
        if (!ev.StateMachine.m_OnViewVisible)
        {
            return false;
        }

        // 在黑暗的角落，不允许显示
        if (ev.m_bInDarkCorner)
        {
            return false;
        }

        return true;
    }

    //响应数值属性消息函数
    public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
    {
        return;
        if(!m_bCreated)
        {
            CreateMinimap();
        }

        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_HP:  
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                {
                    int nMaxHp = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                    int nCurHp = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                    if (nMaxHp == 0)
                    {
                        return;
                    }

                    if (m_Minimap != null)
                    {
                        m_Minimap.SetObjHpPercent(ref e.objev, e.objev.createinfo.nMinimapIcon, (int)(nCurHp * 100 / nMaxHp));
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_MASK:
                {
                    SetObjectVisible(e.objev.ID, e.objev.createinfo.nMinimapIcon, IsAllowShow(e.objev.ID) ? 1 : 0);
                }
                break;
            default:
                break;
        }
    }

    // 创建Minimap
    public void CreateMinimap()
    {
        return;
        if (m_Minimap == null && SFGFxMovieManager.GetWindow < GFxMinimap>() != null)
        {
            m_Minimap = SFGFxMovieManager.GetWindow<GFxMinimap>();
            m_Minimap.Init(SceneManager.Instance.m_curMapID);

            m_bCreated = true;
        }

    }

    // 销毁Minimap
    public void DestoryMinimap()
    {
        SFGFxMovieManager.Instance.DestroyWindow("GFxMinimap");
        m_Minimap = null;
        m_bCreated = false;
    }

    // 小地图添加物体
    // 参数 :唯一ID  职业类型(用来区分用什么图标，怪物的话是怪物ID)  阵营 
    public void MinimapAddObj(int nObjID, int nIconID, int nCamp, int nSight, byte entityType, byte isMaster, int data)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        bool bIsMainHero = (isMaster==1);
        if(bIsMainHero)
        {
            m_MainHeroCamp = nCamp; 
        }

        if (m_MainHeroCamp < 0)
        {
            Trace.LogError("MinimapAddObj failed: variable [mainHeroCamp] not ready!!!!");
            return;
        }

        // 默认中立
        int nRelation = 2;
        if(nCamp == m_MainHeroCamp)
        {
            nRelation = 0; // 友方
        }
        else if(nCamp == 0) // 中立
        {
            nRelation = 2;
        }
        else
        {
            nRelation = 1; // 敌方
        }

        if(bIsMainHero)
        {
            nRelation = 3; // 自己
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("MinimapAddObj called before CreateMinimap!!");
            return;
        }
        m_Minimap.AddObject(nObjID, nIconID, nCamp, nSight, entityType, nRelation, data);
    }

    // 小地图移除物体
    public void MinimapRemoveObj(int nObjID)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            //Trace.LogWarning("MinimapRemoveObj m_Minimap == null");
            return;
        }

        m_Minimap.RemoveObject(nObjID);
    }

    // 小地图更新物体位置、朝向
    private void MinimapUpdateObjPos(int nObjID, double dXpos, double dZpos, double dAngle)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            return;
        }
        m_Minimap.UpdateObjectPos(nObjID, dXpos, dZpos, dAngle);
    }

    // 小地图物体显示/隐藏
    public void SetObjectVisible(int nObjID, int nIconID, int nVisible)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            //Trace.LogWarning("SetObjectVisible m_Minimap == null");
            return;
        }

        if (nVisible != 0 && !IsAllowShow(nObjID))
        {
            return;
        }

        m_Minimap.SetObjectVisible(nObjID, nIconID, nVisible);
    }

    // 标记小地图
    public void MarkMinimap(float xPos, float zPos, int sigType)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("MarkMinimap m_Minimap == null");
            return;
        }

        m_Minimap.MarkMinimap(xPos, zPos, sigType);
    }

    // 通知小地图进入/退出 上帝视角模式
    public void OnSwitchGodEyeMode(bool isGodEyeMode, Vector3 vPoint)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("OnSwitchGodEyeMode m_Minimap == null");
            return;
        }

        m_Minimap.OnSwitchGodEyeMode(isGodEyeMode, vPoint);

        if (isGodEyeMode)
        {
            WarFogManager.Instance.Begin();
        }
        else
        {
            WarFogManager.Instance.End();
        }
    }

    // 实体加特技
    // nEffectType: 1--传送 2--进攻建筑物 3--防守建筑物 4--遭遇战斗
    public void MinimapEntityAttachEffect(int nObjID, int nEffectType)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("MinimapEntityAttachEffect m_Minimap == null");
            return;
        }

        m_Minimap.MinimapEntityAttachEffect(nObjID, nEffectType);
    }

    // 创建静态打点
    public void CreateStaticObj(int objIndex)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("CreateStaticObj m_Minimap == null");
            return;
        }

        m_Minimap.CreateStaticObj(objIndex);
    }

    // 销毁静态打点
    public void DestroyStaticObj(int objIndex)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("DestroyStaticObj m_Minimap == null");
            return;
        }

        m_Minimap.DestroyStaticObj(objIndex);
    }

    // 小地图移除物体
    public void DestroyAllStaticObj()
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("DestroyAllStaticObj m_Minimap == null");
            return;
        }

        m_Minimap.DestroyAllStaticObj();
    }

    // 创建野怪（静态打点）倒计时
    public void CreateCountDown(int objIndex, int countDown)
    {
        return;
        if (!m_bCreated)
        {
            CreateMinimap();
        }

        if (m_Minimap == null)
        {
            Trace.LogWarning("CreateCountDown m_Minimap == null");
            return;
        }

        m_Minimap.CreateCountDown(objIndex, countDown);
    }

    public void BatchUpdateEntityPos(Dictionary<int, EntityPosAngleInfo> hTable)
    {
        if (m_bCreated && m_Minimap != null)
        {
            m_Minimap.BatchUpdateEntityPos(hTable);
        }
    }
   
    public float SceneWidth
    {
        get
        {
            if (m_Minimap)
                return m_Minimap.SceneRect.width;
            else
                return -1;
        }
    }

    public float SceneHeight
    {
        get
        {
            if (m_Minimap)
                return m_Minimap.SceneRect.height ;
            else
                return -1;
        }
    }

    public float SceneLeft
    {
        get
        {
            if (m_Minimap)
                return m_Minimap.SceneRect.xMin;
            else
                return -1;
        }
    }

    public float SceneTop
    {
        get
        {
            if (m_Minimap)
                return m_Minimap.SceneRect.yMin;
            else
                return -1;
        }
    }

    public void LeaveGodEye()
    {
        return;
        PopupMovieManager.Instance.PopMovie(m_Minimap);
    }
}


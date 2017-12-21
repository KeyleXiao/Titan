/*******************************************************************
** 文件名:	MonsterStateMachine_Mask.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的Mask相关部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using U3D_Render;


public sealed  partial class MonsterStateMachine : BaseStateMachine
{
    /// <summary>
    /// 攻击时,不转向
    /// </summary>
    private bool bNotTurnWhenAttackMask = false;

    /// <summary>
    /// 第一次走跟visible相关的mask，就是创建时候的mask，屏蔽掉
    /// </summary>
    private bool bFirstVisibleMask = false;

    #region Mask相关

    /// <summary>
    /// 改变mask
    /// </summary>
    /// <param name="mas"></param>
    /// <param name="ev"></param>
    public override void ChangeMask(U3D_Render.EntityView ev)
    {
        if (null == ev)
        {
            return;
        }
        int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

        ////0不处理
        //if (mas == 0)
        //{
        //    return;
        //}
        if (!SkinConfig)
        {
            return;
        }
        Mask_Visible(ev);
        Mask_Collider(ev);
        Mask_Turn(ev);
        Mask_Not_Move(ev);
    }

    /// <summary>
    /// Mask--可见性
    /// </summary>
    /// <param name="ev"></param>
    /// <param name="bUseInitMask">使用初始化时候的Mask</param>
    /// <returns></returns>
    private void Mask_Visible(U3D_Render.EntityView ev, bool bUseInitMask = false)
    {
        int mas = 0;
        if (bUseInitMask)
        {
            mas = ev.InitMaskFlag;
        }
        else
        {
            mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
            if (!bFirstVisibleMask)
            {
                bFirstVisibleMask = true;
                return;
            }
        }

        bool bHideFriend = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_FRIEND) == (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_FRIEND;
        bool bHideEnemy = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_ENEMY) == (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_ENEMY;
        //两者都不隐藏
        if (!bHideFriend && !bHideEnemy)
        {
            // setAlpha(1.0f);

            m_MaskVisible = true;
        }
        //对友方隐藏
        else if (bHideFriend && !bHideEnemy)
        {
            if (ev.CampFlag == SPELL.CampFlag.CampFlag_Friend ||
                ev.CampFlag == SPELL.CampFlag.CampFlag_Self)
            {
                //setAlpha(0.5f);

                m_MaskVisible = false;
            }
        }
        //对非友方隐藏
        else if (!bHideFriend && bHideEnemy)
        {
            if (ev.CampFlag != SPELL.CampFlag.CampFlag_Friend &&
                ev.CampFlag != SPELL.CampFlag.CampFlag_Self)
            {
                //setAlpha(0.5f);

                m_MaskVisible = false;
            }
        }
        //对两者都隐藏
        else
        {
            //setAlpha(1.0f);
            m_MaskVisible = false;

        }
        UpdateVisible();
    }


    /// <summary>
    /// Mask--碰撞
    /// </summary>
    /// <param name="ev"></param>
    private void Mask_Collider(U3D_Render.EntityView ev)
    {

        if (SkinConfig.ColliderShape)
        {
            int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

            bool bHaveCollider = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HAVE_COLLIDER) == (int)ENTITY_MASKFLAG.MASKFLAG_HAVE_COLLIDER;
            SkinConfig.ColliderShape.gameObject.SetActive(bHaveCollider);

            if (!SkinConfig.ColliderShape.enabled)
            {
                SkinConfig.ColliderShape.enabled = true;
            }

            SkinConfig.ColliderShape.isTrigger = false;

            //TransparentFX层只和主角做检测
            SkinConfig.ColliderShape.gameObject.layer = LayerMask.NameToLayer(Config.LayerTransparentFX);

        }
    }


    /// <summary>
    /// Mask--转向
    /// </summary>
    /// <param name="ev"></param>
    private void Mask_Turn(U3D_Render.EntityView ev)
    {
        int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

        int flag = (int)ENTITY_MASKFLAG.MASKFLAG_NOT_TURN;
        bNotTurnWhenAttackMask = (mas & flag) == flag;

    }

    /// <summary>
    /// Mask-不移动
    /// </summary>
    /// <param name="ev"></param>
    private void Mask_Not_Move(U3D_Render.EntityView ev)
    {
        int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

        int flag = (int)ENTITY_MASKFLAG.MASKFLAG_NOT_MOVE;
        bNotMove = (mas & flag) == flag;

    }
    #endregion

}

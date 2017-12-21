/*******************************************************************
** 文件名:	EffectDragToMoveEX.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	10/23/2015
** 版  本:	1.0
** 描  述:	效果-目标移动瞬移
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"
#include "TimerAxis.h"

enum 
{
    DELAY_TO_TIMER,         // 延时拖动执行的TIMER；比如5秒后执行移动或者瞬移
    CHANGE_SPEED_TIMER,     //改变速度执行TIMER；比如原速度500，移动速度1000，移动到指定地点后还原500
    SKILL_DELAY_TIMER,      //技能延时500秒（暂时定的）；因为检测过来，不延时移动的话偶尔会有技能释放后摇导致移动距离过短的bug
};

class CEffectDragToMoveEX : public IEffectEx,public TimerHandler,IEventExecuteSink
{
public:
    typedef  EffectServer_DragToMoveEX    SCHEME_DATA;
    CEffectDragToMoveEX( SCHEME_DATA &data ) : m_data(data),m_uidTarget(INVALID_UID),
        m_vMouseLoc(0,0,0),m_nOldSpeed(0),m_vOldTargetLoc(0,0,0),m_pEntity(0),m_nLastSpellID(0)
    {
    }

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

    // 效果启用
    virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
    {
        if ( context==0 || context->pEntity==0 )
            return false;

        m_pEntity=context->pEntity;

        switch (m_data.nDragOperatorType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidTarget=context->uidOperator;
                m_vMouseLoc=context->ptTargetTile;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidTarget=context->uidTarget;
                m_vMouseLoc=context->ptTargetTile;
            }
            break;
        default:
            {
                break;
            }
        }

        if (isInvalidUID(m_uidTarget)|| equalto0(m_vMouseLoc.getLength()))
        {
            return false;
        }

        //如果移动或者瞬移移动类型是原地，那么记录原始位置,有需要瞬移返回原地的也记录下，同时再记录下二段技能的技能id
        if(m_data.nDirectionType==DIRECTION_INSITU || m_data.nIsDoubleSpellUseBack)
        {
            m_vOldTargetLoc=getLocation(m_uidTarget);
            if(m_data.nIsDoubleSpellUseBack)
            {
                m_nLastSpellID=context->nID;
            }
        }
        //获得原始速度
        m_nOldSpeed=getProperty_Integer(m_uidTarget , PROPERTY_SD);

        if(m_data.nTime!=0)
        {
            g_EHelper.SetTimer(DELAY_TO_TIMER, m_data.nTime, this, 1, "CEffectDragToMoveEX");
        }
        else
        {
            OnTimer(DELAY_TO_TIMER);
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
      
    }

    // 克隆一个新效果
    virtual IEffect *       Clone()
    {
        return new CEffectDragToMoveEX(m_data);
    }

    // 释放
    virtual void			Release()
    {
        // 先在release中处理下，后面方案会改进
        g_EHelper.KillTimer(DELAY_TO_TIMER,this);
        g_EHelper.KillTimer(CHANGE_SPEED_TIMER,this);
        g_EHelper.KillTimer(SKILL_DELAY_TIMER,this);

        // 为了安全，都杀一杀 
        if(m_pEntity != 0)
        {
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_SECONDARY_CAST_SPELL);

            m_pEntity=0;
        }
        Stop();
        delete this;
    }

    virtual void OnTimer( unsigned long dwTimerID )
    {
        switch(dwTimerID)
        {
            //还原速度
        case CHANGE_SPEED_TIMER:
            {
                setProperty_Integer(m_uidTarget, PROPERTY_SD, m_nOldSpeed);  
            }
            break;
            //移动延时，防止技能后摇，目前只用在自己移动
        case SKILL_DELAY_TIMER:
            {
                Vector3 vOldLoc=getLocation(m_uidTarget);
                Vector3 vNowLoc = vOldLoc+((m_vMouseLoc - vOldLoc).normalize())*m_data.fDistance;
                //如果改变自己的方向是原地的设定，那么将新位置设为原始位置
                if(m_data.nDirectionType==DIRECTION_INSITU)
                {
                    vNowLoc=m_vOldTargetLoc;
                }
                //改变速度
                setProperty_Integer(m_uidTarget , PROPERTY_SD, m_data.nMoveSpeed );  
                //移动
                MoveTo(m_uidTarget,vNowLoc);
                Vector3 spaceLocation = vNowLoc - vOldLoc;
                float fSpaceLocation = spaceLocation.getLength();
                if(m_data.nMoveSpeed == 0)
                {
                    return;
                }
                float fTime = fSpaceLocation*100/m_data.nMoveSpeed;
                if (fTime <= 0.01)
                {
                    fTime = 0.1f;
                }
                //时间到还原速度
                g_EHelper.SetTimer(CHANGE_SPEED_TIMER, (DWORD)(fTime*1000), this, 1, "CEffectDragToMoveEX");
            }
            break;
            //延时移动或者瞬移，针对自己和目标
        case DELAY_TO_TIMER:
            {
                switch (m_data.nDragOperatorType)
                {
                    //改变自己位置，瞬移或者移动
                case MA_TAGET_TO_SELF:
                    {
                        setSelfLoc();
                    }
                    break;
                    //目标改变位置
                case MA_TAGET_TO_TARGET:
                    {   
                        //原始位置（原地）的代码，传送或者移动到原地
                        if(m_data.nDirectionType==DIRECTION_INSITU)
                        {
                            if(m_data.nIsTeleport)//瞬移
                            {
                                Transport(m_uidTarget, m_vOldTargetLoc, Vector3(-1,-1,-1), true);
                            }
                            else
                            {
                                setProperty_Integer(m_uidTarget , PROPERTY_SD, m_data.nMoveSpeed );  
                                //移动
                                MoveTo(m_uidTarget,m_vOldTargetLoc);
                                Vector3 spaceLocation = getLocation(m_uidTarget) - m_vOldTargetLoc;
                                float fSpaceLocation = spaceLocation.getLength();
                                if(m_data.nMoveSpeed == 0)
                                {
                                    return;
                                }
                                float fTime = fSpaceLocation*100/m_data.nMoveSpeed;
                                if (fTime <= 0.01)
                                {
                                    fTime = 0.1f;
                                }
                                //移动到目的地之后变回原始速度
                                g_EHelper.SetTimer(CHANGE_SPEED_TIMER, (DWORD)(fTime*1000), this, 1, "CEffectDragToMoveEX");
                            }
                            break;
                        }
                        else
                        {
                            //其他方向的代码，前后左右方向，改变目标位置，瞬移或者移动
                            setEntityLoc();
                        }
                    }
                    break;
                default:
                    {
                        break;
                    }
                }
            }
            break;
        default:
            {
                break;
            }
        }	
    }

    //改变自己位置
    void setSelfLoc()
    {
        if (isInvalidUID(m_uidTarget))
        {
            return;
        }
        // 场景ID
        int nSceneID = UID_2_SCENE(m_uidTarget);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return;
        }

        Vector3 vOldLoc=getLocation(m_uidTarget);
        Vector3 vNowLoc;
        if(m_data.nIsUseMouseLocation&&(m_vMouseLoc.getDistance(vOldLoc)) < m_data.fDistance)
        {
            vNowLoc = m_vMouseLoc;
        }
        else
        {
            vNowLoc = vOldLoc-((vOldLoc-m_vMouseLoc).normalize())*m_data.fDistance;
        }
        // 找到地面的点
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        if(m_data.nIsTeleport)//瞬移
        {
            if(m_data.nDirectionType==DIRECTION_INSITU)
            {
                vNowLoc=m_vOldTargetLoc;
            }
            //传送
            Transport(m_uidTarget,vNowLoc,Vector3(-1,-1,-1),true);
            if(m_data.nIsDoubleSpellUseBack)
            {
                // 配置有配置二段技能的再注册
                g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_SECONDARY_CAST_SPELL, "CEffectDragToMoveEX");
            }
        }
        else//移动，目前针对英雄，小兵没做
        {
            g_EHelper.SetTimer(SKILL_DELAY_TIMER, 500, this, 1, "CEffectDragToMoveEX");
        }
    }

    //改变目标位置
    void setEntityLoc()
    {
        if (isInvalidUID(m_uidTarget))
        {
            return;
        }
        // 取得拖者实体角度
        Vector3 angle = getAngle(m_uidTarget);
        float fAngleY = (float)(angle.y*fPI/180);
        // 算出拖动者方向
        Vector3 vDragAngle;
        vDragAngle.x = sin(fAngleY);
        vDragAngle.y = 0;
        vDragAngle.z = cos(fAngleY);

        // 算出被拖动者方向
        Vector3 vByDragAngle;
        switch (m_data.nDirectionType)
        {
        case DIRECTION_FRONT:
            {
                vByDragAngle = vDragAngle;
            }
            break;
        case DIRECTION_BACK:
            {
                vByDragAngle = vDragAngle*(-1.0);
            }
            break;
        case DIRECTION_LEFT:
            {
                Vector3 vUp(0,1,0);
                vByDragAngle = vDragAngle.getCrossProduct(vUp);
            }
            break;
        case DIRECTION_RIGHT:
            {
                Vector3 vUp(0,1,0);
                vByDragAngle = vUp.getCrossProduct(vDragAngle);
            }
            break;
        default:
            {
                ErrorLn("CEffectDragToMoveEX error nDirectionType="<<m_data.nDirectionType);
                return;
            }
            break;
        }

        // 场景ID
        int nSceneID = UID_2_SCENE(m_uidTarget);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return;
        }
        Vector3 vNowLoc = pPathService->getRaycast(getLocation(m_uidTarget), vByDragAngle.normalize(), m_data.fDistance);
        // 找到地面的点
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        if (m_data.nIsTeleport)
        {
            // 传送
            Transport(m_uidTarget, vNowLoc, Vector3(-1,-1,-1),true);
        } 
        else
        {
            //移动，目前针对英雄，小兵没做
            if (UID_2_TYPE(m_uidTarget) == TYPE_PLAYER_ROLE)
            {
                setProperty_Integer(m_uidTarget, PROPERTY_SD, m_data.nMoveSpeed);  
                //移动
                MoveTo(m_uidTarget,vNowLoc);
                Vector3 spaceLocation = vNowLoc - getLocation(m_uidTarget);
                float fSpaceLocation = spaceLocation.getLength();
                if(m_data.nMoveSpeed == 0)
                {
                    return;
                }
                float fTime = fSpaceLocation*100/m_data.nMoveSpeed;
                if (fTime <= 0.01)
                {
                    fTime = 0.1f;
                }
                g_EHelper.SetTimer(CHANGE_SPEED_TIMER, (DWORD)(fTime*1000), this, 1, "CEffectDragToMoveEX");
            }
        }
    }

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
    /** 
    @param   wEventID ：事件ID
    @param   bSrcType ：发送源类型
    @param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
    @param   pszContext : 上下文
    @param   nLen : 上下文长度
    @return  
    @note     
    @warning 
    @retval buffer 
    */
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch (wEventID)
        {
        case EVENT_ENTITY_SECONDARY_CAST_SPELL:		// 二次施法技能事件
            {
                if(m_pEntity==NULL)
                {
                    return;
                }
                if (pszContext == NULL || nLen != sizeof(event_cast_spell))
                {
                    return;
                }
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;
                // 释放的技能id匹配
                if(pCastSpell->nID!=m_nLastSpellID)
                {
                    return;
                }
                // 瞬移回原地
                Transport(m_uidTarget, m_vOldTargetLoc, Vector3(-1,-1,-1), true);
                // 注销二段的技能释放事件
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_SECONDARY_CAST_SPELL);
            }
            break;
        default:
            break;
        }
    }
private:
    SCHEME_DATA             m_data;
    // 实体
    __IEntity*              m_pEntity;
    // 目标UID
    UID						m_uidTarget;
    // 鼠标位置
    Vector3                 m_vMouseLoc;
    // 原始位置
    Vector3                 m_vOldTargetLoc;
    // 原始速度
    int                     m_nOldSpeed;
    // 二段技能需要瞬移后回到原地，需要记录二段的技能ID
    int                     m_nLastSpellID;
};


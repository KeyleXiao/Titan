using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using SPELL;
using GameLogic;

public class EntityTimeSearchWatchOutTower : IEntityTimeSearch
{
    int m_nFirstPersonCamp = 0;
    int m_nFirstPersonEntityID = 0;
    bool m_bStartProgressing = false;
    IList<U3D_Render.EntityView> m_listAroundPlayer = new List<U3D_Render.EntityView>();

    public override void AddEntityTimeSearch(U3D_Render.EntityView objev)
    {
        SetWatchOutInNoCaptureState();
        entityViewInfo = objev;
    }

    public override void OnTimer(int nTimerID)
    {
        switch (nTimerID)
        {
            case (int)EN_TimeId.EN_ENTITYSEARCH:
                {
                    IList<U3D_Render.EntityView> WatchOutTowerAroundEntity = MapFinderManager.Nearest(entityViewInfo.StateMachine.transform.position, entityViewInfo.Sight, SPELL.CampFlag.CampFlag_Unknow);
    
                    if (WatchOutTowerAroundEntity.Count <= 0)
                    {
                        m_nFirstPersonCamp = 0;
                        m_nFirstPersonEntityID = 0;
                        return;
                    }

                    m_listAroundPlayer.Clear();
                    foreach (U3D_Render.EntityView Entity_Ev in WatchOutTowerAroundEntity)
                    {
                        if (Entity_Ev.gameObject == null)
                        {
                            continue;
                        }

                        if (Entity_Ev.Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            continue;
                        }

                        m_listAroundPlayer.Add(Entity_Ev);
                    }

                    bool bStartProgress = false;
                    foreach (U3D_Render.EntityView Entity_Ev in m_listAroundPlayer)
                    {
                        int nPersonCamp = Entity_Ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
                        if (m_nFirstPersonCamp == 0 || m_listAroundPlayer.Count == 1)
                        {
                            m_nFirstPersonCamp = nPersonCamp;
                            m_nFirstPersonEntityID = Entity_Ev.ID;
                            if (Entity_Ev.ID == EntityFactory.MainHeroID)
                            {
                                bStartProgress = true;
                            }

                            continue;
                        }
                        // 同阵营加速读条 敌对阵营打断读条
                        if (m_nFirstPersonCamp == nPersonCamp)
                        {
                            // 预留接口先不做 时间有点不够。。
                        }
                        else
                        {
                            bStartProgress = false;
                            if (m_nFirstPersonEntityID == EntityFactory.MainHeroID)
                            {
                                SendMainPlayerEndWatchOutProgress();
                            }
                            break;
                        }
                    }

                    if(bStartProgress)
                    {
                        SendMainPlayerStartWatchOutProgress();
                    }
                }
                break;
            default:
                break;
        }
    }

    private void SendMainPlayerStartWatchOutProgress()
    {
        if (m_bStartProgressing)
        {
            return;
        }

        m_bStartProgressing = true;
        cmd_Special_Entity_Range_Search data = new cmd_Special_Entity_Range_Search();
        data.uidOperator = (uint)entityViewInfo.ID;
        data.uidTarget = (uint)EntityFactory.MainHeroID;
        data.nFunctionType = 1;
        data.bSearchOrBreak = true;
        EntityEventHelper.Instance.SendCommand<cmd_Special_Entity_Range_Search>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_SPECIAL_SEARCH, ref data);
    }

    private void SendMainPlayerEndWatchOutProgress()
    {
       if(!m_bStartProgressing)
       {
           return;
       }

       m_bStartProgressing = false;
       cmd_Special_Entity_Range_Search data = new cmd_Special_Entity_Range_Search();
       data.uidOperator = (uint)entityViewInfo.ID;
       data.uidTarget = (uint)EntityFactory.MainHeroID;
       data.bSearchOrBreak = false;
       data.nFunctionType = 1;
       EntityEventHelper.Instance.SendCommand<cmd_Special_Entity_Range_Search>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_SPECIAL_SEARCH, ref data);
    }

    public override void ChangeEntityTimeSearchState(bool bSearch)
    {
        if(bSearch)
        {
            SetWatchOutInNoCaptureState();
        }
        else
        {
            SetWatchOutInCaptureState();
        }
    }

    public void SetWatchOutInCaptureState()
    {
        ResetData();
        TimerManager.KillTimer(this, (int)EN_TimeId.EN_ENTITYSEARCH);
    }

    public void SetWatchOutInNoCaptureState()
    {
        ResetData();
        TimerManager.SetTimer(this, (int)EN_TimeId.EN_ENTITYSEARCH, 1f);
    }

    public void ResetData()
    {
        m_nFirstPersonCamp = 0;
        m_nFirstPersonEntityID = 0;
        m_bStartProgressing = false;
    }

    public override void release()
    {
        TimerManager.KillTimer(this, (int)EN_TimeId.EN_ENTITYSEARCH);
        m_listAroundPlayer.Clear();
    }

    public EntityTimeSearchWatchOutTower()
    {
       
    }

    ~EntityTimeSearchWatchOutTower()
    {
        
    }
}
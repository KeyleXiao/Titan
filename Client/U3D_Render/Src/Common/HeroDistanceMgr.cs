using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using SPELL;
using GameLogic;
using System.Runtime.InteropServices;

public class HeroDistanceMgr : Singleton<HeroDistanceMgr>, ITimerHanlder
{

    // TODO 玩家离开时需时如果list里有的话需要移除
    List<U3D_Render.EntityView> listEntity = new List<U3D_Render.EntityView>();

    // 需要改变颜色的怪物id
    Dictionary<int, bool> dicMonster = new Dictionary<int, bool>();

    // 飞行时遭遇敌人半径
    int nFlyEncounterDistance = 0;

    public enum EN_TimeId
    {
        EN_HERO_BARRIER = 0,        // 检测英雄之间障碍物的定时器
        EN_MONSTER_BLOOD,           // 怪物血条是否改变的定时器
        EN_FLY_ENCOUNTER,           // 飞行检测遭遇敌人定时器
    }
    public HeroDistanceMgr()
    {
        TimerManager.SetTimer(this, (int)EN_TimeId.EN_HERO_BARRIER, 0.5f);
        //TimerManager.SetTimer(this, (int)EN_TimeId.EN_MONSTER_BLOOD, 0.2f);       //此功能目前关闭
    }
    ~HeroDistanceMgr()
    {
        TimerManager.KillTimer(this, (int)EN_TimeId.EN_HERO_BARRIER);
        //TimerManager.KillTimer(this, (int)EN_TimeId.EN_MONSTER_BLOOD);
        TimerManager.KillTimer(this, (int)EN_TimeId.EN_FLY_ENCOUNTER);
    }

    public void onFlyStateChange(bool bStart, int nScanRaddi)
    {
        nFlyEncounterDistance = nScanRaddi;
        if (bStart)
        {
            TimerManager.SetTimer(this, (int)EN_TimeId.EN_FLY_ENCOUNTER, 0.1f);
        }
        else
        {
            TimerManager.KillTimer(this, (int)EN_TimeId.EN_FLY_ENCOUNTER);
        }
    }

    public void OnTimer(int nTimerID)
    {
        switch (nTimerID)
        {
            case (int)EN_TimeId.EN_HERO_BARRIER:
                {
                    if (listEntity.Count != 0 && EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null && EntityFactory.MainHeroView.StateMachine.GetState() != EntityState.Dead)
                    {
                        CheckEntityOcclusions();
                    }
                }
                break;
            case (int)EN_TimeId.EN_MONSTER_BLOOD:
                {
                    if (Stage_ManagerDef.EStage_State.ESS_BATTLE == StageManager.Instance.getCurrentState())
                    {
                        monsterBloodIsChange();
                    }
                }
                break;
            case (int)EN_TimeId.EN_FLY_ENCOUNTER:
                {
                    if (Stage_ManagerDef.EStage_State.ESS_BATTLE == StageManager.Instance.getCurrentState())
                    {
                        checkFlyEncounter();
                    }
                }
                break;
            default:
                break;
        }
    }

    // 设置在小地图上已显示的敌人id
    public void setEnemyEntity(U3D_Render.EntityView entity, bool bIsSave)
    {
        if (this.listEntity == null)
        {
            listEntity = new List<U3D_Render.EntityView>();
        }

        if (bIsSave == true)
        {
            if (this.listEntity.Contains(entity))
                return;
            this.listEntity.Add(entity);
        }
        else
        {
            if (this.listEntity.Contains(entity))
            {
                this.listEntity.Remove(entity);
            }
        }
    }

    // 判断是否有障碍物，  true隐藏，  false代表显示
    public void CheckEntityOcclusions()
    {
        foreach (U3D_Render.EntityView ev in this.listEntity)
        {
            if (ev == null || ev.gameObject == null)
                continue;

            if (ev.StateMachine.GetState() == GameLogic.EntityState.Dead)
                continue;

            if (ev.Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                continue;
            }

            bool bInDarkCorner = false;
            int nMask = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
            if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_DISABLE_MINIMAP_SIGHT_CHECK) == 0)
            {
                Vector3 VecSourcePos = ev.gameObject.transform.position;
                VecSourcePos.y += 1;

                IList<U3D_Render.EntityView> EnemyAroundEntity = MapFinderManager.Nearest(ev.StateMachine.transform.position, ev.Sight, SPELL.CampFlag.CampFlag_Self);

                if (EnemyAroundEntity == null)
                {
                    return;
                }

                Vector3 VecTargetPos, RaycastDir;
                float RaycastDistance;
                RaycastHit raycastHitInfo;
                LayerMask raycastLayer = (1 << (LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding)));

                int nCount = 0;
                foreach (U3D_Render.EntityView Enemy_Ev in EnemyAroundEntity)
                {
                    if (Enemy_Ev.gameObject == null)
                        return;
                    VecTargetPos = Enemy_Ev.gameObject.transform.position;
                    VecTargetPos.y += 1;

                    RaycastDir = (VecTargetPos - VecSourcePos).normalized;
                    float tmpDis = (VecTargetPos - VecSourcePos).magnitude;
                    float tmpSight = ev.Sight;
                    RaycastDistance = tmpDis > tmpSight ? tmpSight : tmpDis;
                    Debug.DrawLine(VecSourcePos, VecSourcePos + RaycastDir * RaycastDistance, Color.blue);
                    Ray ray = new Ray(VecSourcePos, RaycastDir);
                    if (Physics.Raycast(ray, out raycastHitInfo, RaycastDistance, raycastLayer) == false)
                    {
                        break;
                    }
                    nCount++;
                }


                if (nCount == EnemyAroundEntity.Count())
                {
                    bInDarkCorner = true;
                }
            }

            // 变化了才发出事件
            if (bInDarkCorner != ev.m_bInDarkCorner)
            {
                ev.m_bInDarkCorner = bInDarkCorner; 

                U3D_Render.Utility.SEntityShowHero event_data = new U3D_Render.Utility.SEntityShowHero();
                event_data.ev = ev;
                event_data.bIsHide = bInDarkCorner;
                if (event_data.ev != null)
                {
                    U3D_Render.Utility.EventManager.FireExecute(U3D_Render.Utility.EVENT.ENTITY_SHOW_HERO, ev.ID, event_data);
                }
            }
        }
    }

    public static bool CheckDistance(U3D_Render.EntityView from, U3D_Render.EntityView to, int distance)
    {
        if (from == null || to == null || from.gameObject == null || to.gameObject == null)
            return false;
        float maxDistance = (float)(distance * distance);
        float diff = (from.gameObject.transform.position - to.gameObject.transform.position).sqrMagnitude;

        return (diff <= maxDistance);
    }
    Dictionary<U3D_Render.EntityView, bool> dicMonsterBlood = new Dictionary<U3D_Render.EntityView, bool>();  // true代表已经变色
    // 判断小怪血条颜色是否需要改变，此功能目前关闭
    public void monsterBloodIsChange()
    {
        if (GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
            return;

        U3D_Render.EntityView mainHero = EntityFactory.MainHeroView;
        if (mainHero == null)
            return;

        int nDistance = GameLogicConfigApi.MonsterDistance;
        int nTowerDisatance = GameLogicConfigApi.TowerDistance;

        IList<U3D_Render.EntityView> monster_Entities = MapFinderManager.Nearest(mainHero.StateMachine.transform.position, nDistance, SPELL.CampFlag.CampFlag_Enemy);
        if (monster_Entities == null)
            return;

        // 恢复那些变色
        recoverBloodColor(monster_Entities, mainHero);
        foreach (U3D_Render.EntityView ev in monster_Entities)
        {
            if (ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER || !ev.IsValid || ev.gameObject == null)
            {
                continue;
            }

            if (CheckDistance(ev, mainHero, nDistance) == false)
            {
                continue;
            }
            
            //IList<U3D_Render.EntityView> tower_Entities = MapFinderManager.Nearest(ev.StateMachine.transform.position, nTowerDisatance, SPELL.CampFlag.CampFlag_Self);
            //if (tower_Entities == null )
            //{
            //    Trace.LogWarning("tower_Entities = " + tower_Entities);
            //    continue;
            //}

            //if (tower_Entities.Count == 0)
            //{
            //    Trace.LogWarning("tower_Entities.Count = " + tower_Entities.Count);
            //    continue;
            //}
                

            //bool bIsHaveTower = false;
            //foreach (U3D_Render.EntityView tower_ev in tower_Entities)
            //{
            //    if (tower_ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER)
            //    {
            //        Trace.LogWarning("tower_ev.Flag = " + tower_ev.Flag);
            //        continue;
            //    }
            //    if (CheckDistance(ev, tower_ev, nTowerDisatance) == false)
            //    {
            //        Trace.LogWarning("tower_ev = " + tower_ev + ", nTowerDisatance = " + nTowerDisatance );
            //        continue;
            //    }
                    
            //    bIsHaveTower = true;
            //    break;
            //}

            // 设置怪物的颜色   临时注释之后要找原因
            bool bIsHaveTower = false;
            setShowBloodColor(bIsHaveTower, ev);
        }
    }

    public void recoverBloodColor(IList<U3D_Render.EntityView> list, U3D_Render.EntityView mainEv)
    {
        if (dicMonsterBlood.Count == 0)
            return;


        List<U3D_Render.EntityView> lisBackups = new List<U3D_Render.EntityView>();

        U3D_Render.EntityView ev;
        foreach (KeyValuePair<U3D_Render.EntityView, bool> kvp in dicMonsterBlood)
        {
            ev = kvp.Key;
            if (list.Contains(ev) && CheckDistance(ev, mainEv, GameLogicConfigApi.MonsterDistance))
                continue;

            // 此处在定时处理时，怪物死亡的时候并没有从容器里删除， 所以要做个判断删除掉
            if (ev == null || ev.StateMachine == null)
            {
                lisBackups.Add(ev);
                continue;
            }

            if (kvp.Value == true)
                //GfxBloodManager.SetBloodColor(ev.ID, false);
                USpeedUI.Blood.UBloodManager.Instance.ChangeBloodColor(ev.ID, false);

            lisBackups.Add(ev);
        }

        for (int i = 0; i < lisBackups.Count; ++i)
        {
            ev = lisBackups[i];
            if (dicMonsterBlood.ContainsKey(ev))
                dicMonsterBlood.Remove(ev);
        }
        lisBackups.Clear();
        lisBackups = null;
    }

    protected void setShowBloodColor( bool bIsHaveTower, U3D_Render.EntityView ev )
    {
        Int32 uID = ev.ID;
        if (bIsHaveTower)
        {
            if (dicMonsterBlood.ContainsKey(ev) && !dicMonsterBlood[ev])
                return;

            //GfxBloodManager.SetBloodColor(uID, false);
            USpeedUI.Blood.UBloodManager.Instance.ChangeBloodColor(uID, false);
            dicMonsterBlood[ev] = false;
        }
        else
        {
            if (dicMonsterBlood.ContainsKey(ev) && dicMonsterBlood[ev])
                return;

            //GfxBloodManager.SetBloodColor(uID, true);
            USpeedUI.Blood.UBloodManager.Instance.ChangeBloodColor(uID, true);
            dicMonsterBlood[ev] = true;
        }
        return;
    }

    protected void checkFlyEncounter()
    {
        U3D_Render.EntityView mainHero = EntityFactory.MainHeroView;
        if (mainHero == null)
            return;

        IList<U3D_Render.EntityView> enemyList = MapFinderManager.Nearest(mainHero.StateMachine.transform.position, nFlyEncounterDistance, SPELL.CampFlag.CampFlag_Enemy);

        foreach (U3D_Render.EntityView ev in enemyList)
        {
            if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE && ev.StateMachine.GetState() != GameLogic.EntityState.Dead)
            {
                // 请求降落
                cmd_creature_set_wing_state data;
                data.nType = WING_STATE_TYPE.WING_STATE_FLOOR;
                data.nEncounterFlag = 1;

                IntPtrHelper helper = new IntPtrHelper();
                IntPtr ptr = helper.toPtr<cmd_creature_set_wing_state>(ref data);
                int len = Marshal.SizeOf(data);
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SET_WING_STATE, len, "", ptr, len);
                
                // 关闭定时器
                // onFlyStateChange(false, nFlyEncounterDistance);

                return;
            }
        }
    }

    public void release()
    {
        listEntity.Clear();
        dicMonster.Clear();
    }

}
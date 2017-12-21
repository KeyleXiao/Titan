/*******************************************************************
** 文件名:	WarOBDataManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB全场实体数据管理
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using GameLogic;
using U3D_Render;

namespace DataCenter
{
    public class WarOBDataManager
    {
        /// <summary>
        /// 获取当前观察阵营所有人物实体
        /// </summary>
        /// <returns></returns>
        public ICollection<int> GetCurMainCampPlayerList()
        {
            if(m_CurMainCamp == 0)
            {
                return m_PlayerEntitiesList;
            }
            return m_PlayerEntitiesMap[m_CurMainCamp];
        }
        /// <summary>
        /// 各阵营视野内实体
        /// </summary>
        private Dictionary<int, List<EntityView>> m_EntitiesMap = new Dictionary<int, List<EntityView>>();
        /// <summary>
        /// 全场所有实体
        /// </summary>
        private List<EntityView> m_EntitiesList = new List<EntityView>();
        /// <summary>
        /// 各阵营人物实体
        /// </summary>
        private Dictionary<int, List<int>> m_PlayerEntitiesMap = new Dictionary<int, List<int>>();
        /// <summary>
        /// 全场所有人物实体
        /// </summary>
        private List<int> m_PlayerEntitiesList = new List<int>();
        public int CurMainCamp { get { return m_CurMainCamp; } }

        public delegate void SwitchOBViewCamp(int nNewCamp);
        public event SwitchOBViewCamp SwitchOBViewCampEvent;
        /// <summary>
        /// 当前观察阵营
        /// </summary>
        private int m_CurMainCamp;

        public void Init()
        {
            m_CurMainCamp = 0;

            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void Clear()
        {
            m_CurMainCamp = 0;

            ClearData();
            m_EntitiesMap = null;
            m_EntitiesList = null;
            m_PlayerEntitiesMap = null;
            m_PlayerEntitiesList = null;

            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void onLeaveBattleState()
        {
            ClearData();
        }

        public void onEnterLoginState()
        {
            ClearData();
        }

        /// <summary>
        /// 切换观察阵营
        /// </summary>
        /// <param name="camp"></param>
        /// <returns></returns>
        public bool SwitchMainCamp(int camp)
        {
            if (camp == m_CurMainCamp)
                return false;

            if (camp != 0 && !m_EntitiesMap.ContainsKey(camp))
                return false;

            m_CurMainCamp = camp;

            if(SwitchOBViewCampEvent != null)
            {
                SwitchOBViewCampEvent(m_CurMainCamp);
            }

            foreach (U3D_Render.EntityView ev in m_EntitiesList)
            {
                UpdateEntity(ev);
            }

            return true;
        }

        /// <summary>
        /// 更新阵营玩家
        /// </summary>
        /// <param name="ptrParam"></param>
        /// <param name="nLen"></param>
        public void UpdateCampPlayer(IntPtr ptrParam, int nLen)
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (StageManager.Instance.CurSubState != EStage_SubState.ESSS_WAR)
                return;

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nLen);
            cmd_war_personInfo_count cmdCount = helper.get<cmd_war_personInfo_count>();

            for(int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_war_personinfo data = helper.get<cmd_war_personinfo>();

                if (data.uID < 1)
                    continue;

                if(!m_PlayerEntitiesMap.ContainsKey(data.nCamp))
                {
                    m_PlayerEntitiesMap.Add(data.nCamp, new List<int>());
                }

                if (!m_PlayerEntitiesMap[data.nCamp].Contains(data.uID))
                {
                    m_PlayerEntitiesMap[data.nCamp].Add(data.uID);
                }

                if (!m_PlayerEntitiesList.Contains(data.uID))
                {
                    m_PlayerEntitiesList.Add(data.uID);
                }
            }
        }

        /// <summary>
        /// 获取实体
        /// </summary>
        /// <param name="nCamp">阵营</param>
        /// <param name="nIndex">索引</param>
        /// <returns></returns>
        public EntityView GetEntityIDByCampAndIndex(int nCamp, int nIndex)
        {
            if (nCamp < 0 || nIndex < 0)
                return null;

            if (!m_PlayerEntitiesMap.ContainsKey(nCamp))
                return null;

            if (m_PlayerEntitiesMap[nCamp].Count <= nIndex)
                return null;

            return EntityFactory.getEntityViewByID(m_PlayerEntitiesMap[nCamp][nIndex]);
        }

        int randomIndex;
        /// <summary>
        /// 随机实体
        /// </summary>
        /// <returns></returns>
        public EntityView GetRondomEntityView()
        {
            if (GetCurMainCampPlayerList().Count < 1)
                return null;

            int[] entityList = GetCurMainCampPlayerList().ToArray();
            if (entityList.Length < 1)
                return null;
            else if(entityList.Length == 1)
            {
                return EntityFactory.getEntityViewByID(entityList[0]);
            }
            else
            {
                if (randomIndex < 0 || randomIndex > entityList.Length - 1)
                    randomIndex = 0;

                int index = new Random().Next(0, entityList.Length);
                while(randomIndex == index)
                {
                    index = new Random().Next(0, entityList.Length);
                }
                randomIndex = index;
                return EntityFactory.getEntityViewByID(entityList[randomIndex]);
            }

        }

        /// <summary>
        /// 检测实体当前是否可见
        /// </summary>
        /// <param name="ev"></param>
        /// <returns></returns>
        public bool CheckEntityViewVisible(EntityView ev)
        {
            if (m_CurMainCamp == 0)
                return true;

            if (EntityView.isNullOrEmpty(ev))
                return false;

            return m_EntitiesMap[m_CurMainCamp].Contains(ev);
        }

        public void AddObject(EntityView ev)
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (StageManager.Instance.CurSubState != EStage_SubState.ESSS_WAR)
                return;

            if (ev == null || ev.IsHero)
                return;

            m_EntitiesList.Add(ev);
            UpdateEntity(ev);
        }

        public void RemoveObject(EntityView ev)
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (StageManager.Instance.CurSubState != EStage_SubState.ESSS_WAR)
                return;

            if (ev != null && m_EntitiesList.Contains(ev))
            {
                m_EntitiesList.Remove(ev);
            }

            foreach (int campItem in m_EntitiesMap.Keys)
            {
                if (m_EntitiesMap[campItem].Contains(ev))
                    m_EntitiesMap[campItem].Remove(ev);
            }
        }

        private void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            //if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_VISIBLE_FLAG)
            //{
            //    Trace.Log("Update_Visible_Flag_Line1----entityID:" + e.objev.ID + "-------Visible:" + e.data.nValue);
            //}

            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (StageManager.Instance.CurSubState != EStage_SubState.ESSS_WAR)
                return;

            if (e.objev == null || e.objev.IsHero || e.objev.Property == null)
                return;

            if(e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_VISIBLE_FLAG)
            {
                //Trace.Log("Update_Visible_Flag----entityID:" + e.objev.ID + "----entityName:" + e.objev.gameObject.name + "----Visible:" + e.data.nValue + "----Camp:" + e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP));
                AddEntity(e.objev, e.data.nValue);
                UpdateEntity(e.objev);
            }
        }

        private void AddEntity(EntityView ev, int nVisible)
        {
            if (ev == null || ev.Property == null)
                return;

            int nCamp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
            if (nCamp > 0 && !m_EntitiesMap.ContainsKey(nCamp))
            {
                m_EntitiesMap.Add(nCamp, new List<EntityView>());
            }

            foreach (int campItem in m_EntitiesMap.Keys)
            {
                if (((1 << campItem) & nVisible) != 0)
                {
                    if (!m_EntitiesMap[campItem].Contains(ev))
                        m_EntitiesMap[campItem].Add(ev);
                }
                else
                {
                    if (m_EntitiesMap[campItem].Contains(ev))
                        m_EntitiesMap[campItem].Remove(ev);
                }
            }
        }

        private void UpdateEntity(EntityView ev)
        {
            if (ev == null)
                return;

            if (ev.StateMachine == null)
                return;

            if (m_CurMainCamp == 0 || m_EntitiesMap[m_CurMainCamp].Contains(ev))
            {
                if(!ev.StateMachine.isOnView_Logic())
                    Entity_EnterView(ev);
            }
            else
            {
                if (ev.StateMachine.isOnView_Logic())
                    Entity_LeaveView(ev);
            }
        }

        private void Entity_EnterView(EntityView ev)
        {
            if (ev == null || ev.StateMachine == null)
                return;

            ev.StateMachine.OnEnterView();
            MapFinderManager.InsertObject(ref ev, ev.gameObject.transform.position);
            if (ev.StateMachine.GetState() != EntityState.Dead)
            {
                LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(ev);
            }
        }

        private void Entity_LeaveView(EntityView ev)
        {
            if (ev == null || ev.StateMachine == null)
                return;

            ev.StateMachine.OnLeaveView();
            MapFinderManager.RemoveObject(ref ev);
            LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(ev);
        }

        private void ClearData()
        {
            m_CurMainCamp = 0;

            m_EntitiesMap.Clear();
            m_EntitiesList.Clear();
            m_PlayerEntitiesMap.Clear();
            m_PlayerEntitiesList.Clear();
        }

        /*
        public void EnterView(U3D_Render.EntityView ev, int id)
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (ev == null || ev.Property == null)
                return;

            int flag = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VISIBLE_FLAG);

            int nCamp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
            if (!m_EntitiesMap.ContainsKey(nCamp))
            {
                m_EntitiesMap.Add(nCamp, new List<int>());
            }

            foreach (int campItem in m_EntitiesMap.Keys)
            {
                if ((campItem & flag) != 0)
                {
                    if (!m_EntitiesMap[campItem].Contains(id))
                        m_EntitiesMap[campItem].Add(id);
                }
            }
        }

        public void LeaveView(U3D_Render.EntityView ev, int id)
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            if (ev == null || ev.Property == null)
                return;

            int flag = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VISIBLE_FLAG);

            foreach (int campItem in m_EntitiesMap.Keys)
            {
                if ((campItem & flag) == 0)
                {
                    if (m_EntitiesMap[campItem].Contains(id))
                        m_EntitiesMap[campItem].Remove(id);
                }
            }
        }
        */
    }
}

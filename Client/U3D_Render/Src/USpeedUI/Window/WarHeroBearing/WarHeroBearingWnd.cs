/*******************************************************************
** 文件名:	WarHeroBearingWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-30
** 版  本:	1.0
** 描  述:	战场指示器界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using U3D_Render.Utility;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using SPELL;

namespace USpeedUI.WarHeroBearing
{
    enum EN_HeroBearing_Timer
    {
        EHBT_Pointer = 0,     // 指示器定时器id
    }

    public class UWarHeroBearingWnd : UIPopupWnd<UWarHeroBearingWndView>, IEventHandler
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_HEROBEARING;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarHeroBearingView/WarHeroBearingView";
        }

        //public override int GetLoadMask()
        //{
        //    return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        //}

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {
            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PKINFO_STARTPK, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PKINFO_ENDPK, this);
            EventManager.Subscibe(this, EVENT.ENTITY_SHOW_HERO, 0);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PKINFO_STARTPK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PKINFO_ENDPK, this);

            EventManager.UnSubscibe(this, EVENT.ENTITY_SHOW_HERO, 0);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                case WndMsgID.WND_MSG_PKINFO_STARTPK:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_PKINFO_ENDPK:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnEvent(U3D_Render.Utility.EVENT eventID, int sourceID, System.Object objEvent)
        {
            switch (eventID)
            {
                case EVENT.ENTITY_SHOW_HERO:
                    {
                        if (objEvent == null || m_wndView == null)
                            return;

                        SEntityShowHero event_data = (SEntityShowHero)objEvent;
                        int nId = event_data.ev.ID;
                        if (event_data.bIsHide && !m_wndView.HideHeroList.Contains(nId))
                            m_wndView.HideHeroList.Add(nId);
                        else if (!event_data.bIsHide && m_wndView.HideHeroList.Contains(nId))
                            m_wndView.HideHeroList.Remove(nId);                       
                    }
                    break;
                default:
                    break;
            }       
        }
    }

    public class UWarHeroBearingWndView : UIBaseWndView, ITimerHanlder
    {
        public GameObject[] HeroBearingGo;
        public GameObject[] Frame;
        public Text[] Distance;
        public Image[] Portrait;
        public Image[] Standpoint;

        private float BillboardRadius = 600;

        private int BillboardAngle = 30;

        private List<UIBillboardQueryResult> tmpAroundEntitys = new List<UIBillboardQueryResult>();

        private Dictionary<Int32, EntityView> RangeActivityEntity = new Dictionary<int, EntityView>();

        internal List<int> HideHeroList = new List<int>();
        private CampFlag m_showAround;

        public override bool Init(IUIWnd wnd)
        {
            TimerManager.SetTimer(this, (int)EN_HeroBearing_Timer.EHBT_Pointer, 0.5f);

            m_showAround = GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP ? SPELL.CampFlag.CampFlag_Friend : SPELL.CampFlag.CampFlag_Enemy;

            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();

            TimerManager.KillTimer(this, (int)EN_HeroBearing_Timer.EHBT_Pointer);
            ClearBillboardCache();           
        }

        public override void Update()
        {
            if (tmpAroundEntitys == null || RangeActivityEntity == null)
                return;
            GetPlayerAndUpdate();  
            
            base.Update();
        }

        private void GetPlayerAndUpdate()
        {
            tmpAroundEntitys.Clear();
            if (EntityFactory.MainHeroView == null)
                return;
            if (RangeActivityEntity.Count > 0 && EntityFactory.MainHeroView.StateMachine.GetState() != EntityState.Dead)
            {
                foreach (KeyValuePair<int, EntityView> keypair in RangeActivityEntity)
                {
                    if (keypair.Value.gameObject == null)
                        continue;
      
                    GetQueryTarget(keypair.Key, keypair.Value.gameObject.transform);
                }
            }
            UpdateBillboard(tmpAroundEntitys);
        }

        // 计算英雄与附近玩家目标角度并映射到UI上
        private void GetQueryTarget(Int32 entityID, Transform TargetEntityTrans)
        {
            Transform CameraTransform = SoldierCamera.MainInstance<SoldierCamera>().getCameraTransform();
            if (CameraTransform == null)
                return;

            if (EntityFactory.MainHero == null)
                return;
            float checkDis = Vector3.Distance(TargetEntityTrans.position, EntityFactory.MainHero.transform.position);
            if (checkDis > EntityFactory.MainHeroView.Sight || ScreenRaycast.isInScreen(TargetEntityTrans.position))
                return;

            Vector3 vecHeroPos = EntityFactory.MainHeroView.gameObject.transform.position;
            Vector3 DirHeroToTarget = TargetEntityTrans.position - vecHeroPos;
            Vector3 DirHeroToCamera = CameraTransform.position - vecHeroPos;
            DirHeroToTarget.y = 0;
            DirHeroToCamera.y = 0;

            // 判断目标在相机的左右方， -1左方， +1右方
            Vector3 vecCross = Vector3.Cross(DirHeroToTarget.normalized, DirHeroToCamera.normalized);
            float fLorR = Mathf.Sign(-vecCross.y);
            // 计算之间的夹角
            float angle = Vector3.Angle(DirHeroToTarget.normalized, DirHeroToCamera.normalized);
            float relAngle = (angle * fLorR + 90) % 360;

            UIBillboardQueryResult qr = new UIBillboardQueryResult();
            qr.entityID = entityID;
            qr.x = BillboardRadius * (Mathf.Cos(Mathf.Deg2Rad * (relAngle)));
            qr.y =  - BillboardRadius * (Mathf.Sin(Mathf.Deg2Rad * (relAngle)));
            //在这个角度范围内图标水平移动
            if (relAngle > BillboardAngle && relAngle < BillboardAngle + 120)
                qr.y = - BillboardRadius * (Mathf.Sin(Mathf.Deg2Rad * BillboardAngle));

            qr.distance = Convert.ToUInt32(checkDis);
            qr.angle = Convert.ToInt32(relAngle + 90);

            tmpAroundEntitys.Add(qr);
        }

        private void UpdateBillboard(List<UIBillboardQueryResult> entitys)
        {
            for (int i = 0; i < entitys.Count; i++)
            {
                HeroBearingGo[i].transform.localPosition = new Vector3(entitys[i].x, entitys[i].y, 0);
                Frame[i].transform.rotation = Quaternion.Euler(0, 0, -entitys[i].angle);
                Distance[i].text = entitys[i].distance.ToString() + "米";

                EntityView targetEntityView = EntityFactory.getEntityViewByID(entitys[i].entityID);
                int nEntityHeroID = targetEntityView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                Portrait[i].sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_HEROBEARING, 1, 4, nEntityHeroID);
                CreatureStateMachine playerMachine = (CreatureStateMachine)targetEntityView.StateMachine;
                Image standpoint = Standpoint[i];
                if (playerMachine)
                {
                    int formType = playerMachine.creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_FORMTYPE);
                    if (formType == (int)FORM_TYPE.FORM_HATHPACE)
                    {
                        standpoint.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "Icon_gaotai", GetUIWnd().GetID());
                        standpoint.SetNativeSize();
                        standpoint.gameObject.SetActive(true);
                    }
                    else if (formType == (int)FORM_TYPE.FORM_SKY)
                    {
                        standpoint.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "Icon_chibang", GetUIWnd().GetID());
                        standpoint.SetNativeSize();
                        standpoint.gameObject.SetActive(true);
                    }
                    else
                    {
                        standpoint.gameObject.SetActive(false);
                    }
                }
                else
                {
                    standpoint.gameObject.SetActive(false);
                }
                
                HeroBearingGo[i].SetActive(true);
            }
            for (int i = entitys.Count; i < HeroBearingGo.Length; i++)
            {
                HeroBearingGo[i].SetActive(false);
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_HeroBearing_Timer.EHBT_Pointer:
                    {
                        SaveAroundPlayer();

                        for (int i = 0; i < HideHeroList.Count; i++)
                        {
                            if (RangeActivityEntity.ContainsKey(HideHeroList[i]))
                                RangeActivityEntity.Remove(HideHeroList[i]);                                                      
                        } 
                    }
                    break;
                default:
                    break;
            }
        }

        public void SaveAroundPlayer()
        {
            EntityView ev = EntityFactory.getMainPlayer();
            if (ev == null)
            {
                return;
            }
            List<Int32> TmpEntityIDList = new List<int>(RangeActivityEntity.Keys);
            IList<EntityView> AroundEntitiesList = MapFinderManager.Nearest(ev.StateMachine.transform.position, ev.Sight, m_showAround);
            if (AroundEntitiesList == null)
            {
                return;
            }

            Int32 nTmpEntityID = 0;
            foreach (EntityView Entity_Ev in AroundEntitiesList)
            {
                if (Entity_Ev == null
                    || Entity_Ev.StateMachine == null
                    || EntityFactory.MainHeroView == null
                    || EntityFactory.MainHeroView.ID == Entity_Ev.ID
                    || Entity_Ev.StateMachine.GetState() == GameLogic.EntityState.Dead
                    || Entity_Ev.Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
                    continue;
                nTmpEntityID = Entity_Ev.ID;
                if (!RangeActivityEntity.ContainsKey(nTmpEntityID))               
                    RangeActivityEntity.Add(nTmpEntityID, Entity_Ev);                
                else                
                    TmpEntityIDList.Remove(nTmpEntityID);                                                  
            }
            while (TmpEntityIDList.Count > 0)
            {
                nTmpEntityID = TmpEntityIDList[0];
                RangeActivityEntity.Remove(nTmpEntityID);
                TmpEntityIDList.RemoveAt(0);
            }
        }

        private void ClearBillboardCache()
        {
            if (RangeActivityEntity != null)
            {
                RangeActivityEntity.Clear();
                RangeActivityEntity = null;
            }

            if (tmpAroundEntitys != null)
            {
                tmpAroundEntitys.Clear();
                tmpAroundEntitys = null;
            }

            if (HideHeroList != null)
            {
                HideHeroList.Clear();
                HideHeroList = null;
            }
        }
    }
}

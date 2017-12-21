using System;
using System.Collections.Generic;
using UnityEngine;
using SPELL;
using U3D_Render;
using System.Collections;
using War;
using GameLogic;
using USpeedUI;

namespace DataCenter
{
    public class WarMinimapDataManager
    {
        private Rect m_sceneRect;
        public Rect SceneRect { get { return m_sceneRect; } }

        public void Init()
        {
        }

        // TODO: 生存模式，小地图不显示敌方英雄头像
        private bool NeedFiltering(EntityView ev)
        {
            if(ev.ID == LogicDataCenter.recordTableDataManager.CurDevilKing)
            {
                // 魔王需要全图可见
                return false;
            }
            else
            {
                return (GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP && ev.createinfo.byCampFlag == (byte)CampFlag.CampFlag_Enemy);
            }
        }

        // 小地图添加物体
        public void AddObject(EntityView ev)
        {
            if (ev.createinfo.nMinimapIcon == 0)
                return;

            if (NeedFiltering(ev))
                return;

            MinimapObjData uiData = new MinimapObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_ADDOBJECT;
            uiData.ev = ev;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDOBJECT, uiData);
        }

        // 小地图移除物体
        public void RemoveObject(EntityView ev)
        {
            if (ev == null)
                return;

            if (NeedFiltering(ev))
                return;

            MinimapObjData uiData = new MinimapObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_REMOVEOBJECT;
            uiData.ev = ev;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVEOBJECT, uiData);
        }

        // 小地图物体显示/隐藏
        public void UpdateObjectVisiblity(EntityView ev)
        {
            if (ev == null)
                return;

            if (NeedFiltering(ev))
                return;

            MinimapObjData uiData = new MinimapObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY;
            uiData.ev = ev;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY, uiData);
        }

        // 刷新实体位置(进视野要求实时刷新 要不然显示后过一会才会刷新到正确的位置)
        public void UpdateObjectPos(EntityView ev, EntityPosAngleInfo pos)
        {
            if (ev == null)
                return;

            if (NeedFiltering(ev))
                return;

            MinimapObjData uiData = new MinimapObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS;
            uiData.ev = ev;
            uiData.pos = pos;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS, uiData);
        }

        // 标记小地图
        public void MarkMinimap(float xPos, float zPos, int sigType)
        {
            MinimapAddSignal uiData = new MinimapAddSignal();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL;
            uiData.fPosX = xPos;
            uiData.fPosY = zPos;
            uiData.nType = sigType;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL, uiData);
        }

        // 实体加特效
        // nEffectType: 1--传送 2--进攻建筑物 3--防守建筑物 4--遭遇战斗
        public void MinimapEntityAttachEffect(int nObjID, int nEffectType)
        {
            MinimapObjectAttachEffect uiData = new MinimapObjectAttachEffect();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT;
            uiData.nObjID = nObjID;
            uiData.nEffectType = nEffectType;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT, uiData);
        }

        // 创建静态打点
        public void AddStaticObj(int objIndex)
        {
            MinimapStaticObjData uiData = new MinimapStaticObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT;
            uiData.nObjType = objIndex;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT, uiData);
        }

        // 销毁静态打点
        public void RemoveStaticObj(int objIndex)
        {
            MinimapStaticObjData uiData = new MinimapStaticObjData();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT;
            uiData.nObjType = objIndex;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT, uiData);
        }

        // 小地图移除物体
        public void ClearStaticObj()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT, null);
        }

        // 创建野怪（静态打点）倒计时
        public void CreateCountDown(int objIndex, int countDown)
        {
            MinimapStaticObjCountdown uiData = new MinimapStaticObjCountdown();
            uiData.msgID = (int)WndMsgID.WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN;
            uiData.nObjType = objIndex;
            uiData.nCountdown = countDown;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN, uiData);
        }

        public void BatchUpdateEntityPos()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS, null);
        }

        public void LeaveGodEye()
        {
            SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.MapGodControl, false);
        }

        public void SetSceneRect(Rect rect)
        {
            m_sceneRect = rect;
        }

        public int GetMinimapSignalEffectID(EMinimapClickType nType, bool bAttach)
        {
            if (nType == EMinimapClickType.EMCT_AttackSignal)
            {
                if (bAttach)
                    return 9;
                else
                    return 8;
            }
            else if (nType == EMinimapClickType.EMCT_RetreatSignal)
            {
                if (bAttach)
                    return 11;
                else
                    return 10;
            }
            else if (nType == EMinimapClickType.EMCT_DangerSignal)
            {
                return 12;
            }
            else if (nType == EMinimapClickType.EMCT_OnWaySignal)
            {
                return 15;
            }
            else if (nType == EMinimapClickType.EMCT_HelpMeSignal)
            {
                return 13;
            }
            else if (nType == EMinimapClickType.EMCT_LostEyeSignal)
            {
                return 14;
            }

            return 0;
        }
    }
}

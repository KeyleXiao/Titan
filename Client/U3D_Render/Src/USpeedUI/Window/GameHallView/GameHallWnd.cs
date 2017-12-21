/*******************************************************************
** 文件名:	UMediatorGameHall.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-1
** 版  本:	1.0
** 描  述:	战场右侧队伍
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
namespace USpeedUI.GameHallPart
{
    public class GameHallWnd : UIFillWnd<GameHallWndView>
    {
        
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_HALLROOMINFOVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "GameHallSelectRoom/GameHallWin";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL ;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;


            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHILL_PRESSDOWN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_ADDROOMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_MAPMODEINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOWAFTERLOADSCENE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHILL_PRESSDOWN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_ADDROOMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_MAPMODEINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOWAFTERLOADSCENE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_WARHILL_PRESSDOWN:
                    {
                        this.SetVisible(true);
                        // 请求进入大厅
                        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ENTER_HALL);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_OPEN:
                    {
                        this.LoadView();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        m_wndView.ClearRoomListData();
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_ADDROOMINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                        SetRoomListData(uiData.ptrParam, uiData.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEROOMINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiRemoveItem = (UIMsgCmdData)msgData;
                        RemoveRoomListData(uiRemoveItem.nParam);
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_REFRESHROOMINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }


                        UIMsgCmdData uiRoomInfo = (UIMsgCmdData)msgData;
                        cmd_match_hall_add_room data = IntPtrHelper.toData<cmd_match_hall_add_room>(uiRoomInfo.ptrParam);
                        RefreshRoomListData(data);
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiRemoveItem = (UIMsgCmdData)msgData;
                        m_wndView.ClearRoomListData();
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_SHOWCREATEROOMVIEW:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        m_wndView.SetCreateRoomShow();
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_GAMEMODEINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiGameMode = (UIMsgCmdData)msgData;
                        cmd_match_create_room_mode data = IntPtrHelper.toData<cmd_match_create_room_mode>(uiGameMode.ptrParam);
                        SetCreateRoomGameModeInfo(uiGameMode.nParam, data);
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_MAPMODEINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiMapMode = (UIMsgCmdData)msgData;
                        cmd_match_create_room_map data = IntPtrHelper.toData<cmd_match_create_room_map>(uiMapMode.ptrParam);
                        SetCreateRoomMapModeInfo(uiMapMode.nParam, data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOWAFTERLOADSCENE:
                    {
                        CloseHallView();
                    }
                    break;
                case WndMsgID.WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        UIMsgCmdData uiChangeID = (UIMsgCmdData)msgData;

                        m_wndView.GroupMapBtnClick(uiChangeID.nParam);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE:
                    {
                        if (m_wndView == null || !IsVisible())
                        {
                            return;
                        }
                        m_wndView.OnReturnBtnClick();
                    }
                    break;

                default:
                    break;
            }
        }

        private void SetRoomListData(IntPtr ptrParam, int nPtrLen)
        {
            
            m_wndView.SetRoomListData(ptrParam, nPtrLen);
        }

        private void RefreshRoomListData(cmd_match_hall_add_room data)
        {
            m_wndView.RefreshRoomListData(data);
        }

        private void RemoveRoomListData(int nIndex)
        {
            m_wndView.RemoveRoomListDataItem(nIndex);
        }

        private void SetCreateRoomMapModeInfo(int nIndex, cmd_match_create_room_map data)
        {
            bool bSelect = data.bSelect <= 0 ? false : true;
            m_wndView.SetCreateRoomMapModeInfo(nIndex, data.nMapID, data.szName, bSelect, data.nSchemeConfigWarID);
        }

        private void SetCreateRoomGameModeInfo(int nIndex, cmd_match_create_room_mode data)
        {
            bool bSelect = data.bSelect <= 0 ? false : true;
            m_wndView.SetCreateRoomGameModeInfo(nIndex, data.nModeID, data.szName, bSelect);
        }

        public void SendCreateRoomMessage(int nMatchType, string RoomName,string strPasswrod, Byte byPermitOb)
        {
            cmd_match_create_room data = new cmd_match_create_room();
            data.szName = RoomName;
            data.nMatchType = nMatchType;
            data.bHavePassWord = strPasswrod.Length;
            data.szPassword = strPasswrod;
            data.byPermitOb = byPermitOb;

            ViewEventHelper.Instance.SendCommand<cmd_match_create_room>(GameLogicDef.GVIEWCMD_MATCH_CREATE_ROOM, ref data);
            SendUIOpenCreateRoomMessage();
        }

        private void CloseHallView()
        {
            this.SetVisible(false);
        }

        public void SendAddRoomIDMessage(int nCurSelectRoomID, string strPassword)
        {
            cmd_match_enter_room data = new cmd_match_enter_room();
            data.nRoomID = nCurSelectRoomID;
            data.szPassword = strPassword;
            ViewEventHelper.Instance.SendCommand<cmd_match_enter_room>(GameLogicDef.GVIEWCMD_MATCH_ENTER_ROOM, ref data);

            SendUIOpenCreateRoomMessage();
        }

        public void SendOpenCreateRoomMessage()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM);
        }

        public void SendUIOpenCreateRoomMessage()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_SUREENTERROOM, null);
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }
    }
}

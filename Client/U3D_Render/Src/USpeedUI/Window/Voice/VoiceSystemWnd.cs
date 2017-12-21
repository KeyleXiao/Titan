using System;
using System.Collections.Generic;
using UnityEngine;

namespace USpeedUI.VoiceSystem
{
    public class VoiceSystemWnd : UIPopupWnd<VoiceSystemView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_VOICESYSTEM;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/VoiceSystem/VoiceSystemView";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {
            base.Init();

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_ENTERROOM,this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_QUITVOICE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATE_ROOMBASEINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEMICORDER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEUSERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATECHANNELLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_RECEIVEUSERSEARCHDATA, this);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_ENTERROOM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_QUITVOICE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATE_ROOMBASEINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEMICORDER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEUSERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATECHANNELLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_RECEIVEUSERSEARCHDATA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        if(null != m_wndView)
                        {
                            SetVisible(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if(null == m_wndView)
                            LoadView();
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (null != m_wndView)
                            m_wndView.EnterBattleState(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (null != m_wndView)
                            m_wndView.EnterBattleState(false);
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_ENTERROOM:
                    {
                        if (null != m_wndView)
                            m_wndView.EnterRoom();
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_QUITVOICE:
                    {
                        if (null != m_wndView)
                            m_wndView.QuitVoice();
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_UPDATE_ROOMBASEINFO:
                    {
                        if (null != m_wndView)
                            m_wndView.UpdateRoomBaseInfo(data as VoiceRoomBaseInfoData);
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO:
                    {
                        if (null != m_wndView)
                            m_wndView.UpdateRightInfo(data as VoiceRoomRigthInfo);
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_UPDATEMICORDER:
                    {
                        if (null != m_wndView)
                            m_wndView.UpdateMicOrder();
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_UPDATEUSERINFO:
                    {
                        if (null != m_wndView)
                            m_wndView.UpdateUserInfo();
                    }
                    break;

                case WndMsgID.WND_MSG_VOICESYSTEM_UPDATECHANNELLIST:
                    {
                        if (null != m_wndView)
                            m_wndView.UpdateChannelList();
                    }
                    break;
                case WndMsgID.WND_MSG_VOICESYSTEM_RECEIVEUSERSEARCHDATA:
                    {
                        if (null != m_wndView)
                            m_wndView.ReceiveUserSearchData();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

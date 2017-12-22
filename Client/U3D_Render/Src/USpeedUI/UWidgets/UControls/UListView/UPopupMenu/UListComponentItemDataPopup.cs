/*******************************************************************
** 文件名:	UListComponentItemDataPopup.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/22
** 版  本:	1.0
** 描  述:	弹出菜单列表显示组件数据
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using DataCenter;
using USpeedUI.PlayerFrame;
using GameLogic;
using U3D_Render;
using U3D_Render.Common;
using USpeedUI.TooltipAndDialog;

namespace USpeedUI.UWidgets
{
    public class UListComponentItemDataPopupBase
    {
        /// <summary>
        /// 显示的名字
        /// </summary>
        public string ItemTypeName;
        /// <summary>
        /// 分组ID，默认是0
        /// </summary>
        public int ItemGroupID;
        /// <summary>
        /// 点击回调
        /// </summary>
        public UnityAction ItemSelectedAction;
        /// <summary>
        /// 清空弹出列表
        /// </summary>
        public UnityAction ClearPopupMenuAction;
        /// <summary>
        /// 显示图标类型
        /// </summary>
        public Sprite ItemPopupSprite;
        /// <summary>
        /// 显示的深度，不过目前功能没完成
        /// </summary>
        public int ItemDepth;
        /// <summary>
        /// 次级元素列表
        /// </summary>
        public List<UListComponentItemDataPopupBase> ItemChildItems;
        /// <summary>
        /// 是否可用
        /// </summary>
        public bool ItemInteractable;


        public UListComponentItemDataPopupBase(UnityAction _actionSelected, UnityAction _actionClearPopupMenu,
                                                string _strItemTypeName = "", int _nGroupID = 0, Sprite _sprItemPopup = null,
                                                int _nDepth = 0, List<UListComponentItemDataPopupBase> _listChildItems = null,bool _bInteractable = true)
        {
            ItemTypeName = _strItemTypeName;
            ItemGroupID = _nGroupID;
            ItemSelectedAction = _actionSelected;
            ClearPopupMenuAction = _actionClearPopupMenu;
            ItemDepth = _nDepth;
            ItemChildItems = _listChildItems;
            ItemPopupSprite = _sprItemPopup;
            ItemInteractable = _bInteractable;
        }
    }

    public class UListComponentItemDataPopup : UListComponentItemDataPopupBase
    {
        /// <summary>
        /// 弹出菜单元素数据内容
        /// </summary>
        /// <param name="_actionSelected">选中时回调方法</param>
        /// <param name="_actionClearPopupMenu">删除时回调方法，内部已赋值</param>
        /// <param name="_strItemTypeName">菜单元素显示名字，内部已赋值</param>
        /// <param name="_nGroupID">菜单元素分组ID</param>
        /// <param name="_sprItemPopup">菜单元素显示图标（功能暂不需要，没实现）</param>
        /// <param name="_nDepth">菜单元素深度信息（功能暂不需要，没实现）</param>
        public UListComponentItemDataPopup(UnityAction _actionSelected, UnityAction _actionClearPopupMenu = null,string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0, List<UListComponentItemDataPopupBase> _listChildItems = null, bool _bInteractable = true)
            
            : base(_actionSelected, _actionClearPopupMenu,_strItemTypeName, _nGroupID, _sprItemPopup, _nDepth, _listChildItems, _bInteractable)
        {

        }
    }

    // 邀请组队
    public class UPopupItemDataTeamInvite : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamInvite(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamInvite");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.InviteJoin(m_nActorID);
        }
    }

    // 请求组队
    public class UPopupItemDataTeamRequest : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamRequest(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamRequest");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.RequestJoin(m_nActorID);
        }
    }

    // 踢出队伍
    public class UPopupItemDataTeamKickout : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamKickout(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamKickout");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.KickoutMember(m_nActorID);
        }
    }

    // 提升为队长
    public class UPopupItemDataTeamAppointCaptain : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamAppointCaptain(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamAppointCaptain");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.AppointCaptain(m_nActorID);
        }
    }

    // 离开队伍
    public class UPopupItemDataTeamLeave : UListComponentItemDataPopup
    {
        //private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamLeave(UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            //m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamLeave");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.LeaveTeam();
        }
    }

    // 解散队伍
    public class UPopupItemDataTeamDisband : UListComponentItemDataPopup
    {
        //private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamDisband(UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            //m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamDisband");
        }

        private void OnCallBack()
        {
            LogicDataCenter.teamDataManager.DisbandTeam();
        }
    }

    // 屏蔽
    public class UPopupItemDataTeamShield : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamShield(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamShield");
        }

        private void OnCallBack()
        {
            // 屏蔽功能
            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");
        }
    }

    // 举报
    public class UPopupItemDataTeamReport : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataTeamReport(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "TeamReport");
        }

        private void OnCallBack()
        {
            // 举报功能
            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");
        }
    }

    // 查看玩家
    public class UPopupItemDataGetPlayerDetail : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataGetPlayerDetail(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "GetPlayerDetail");
        }

        private void OnCallBack()
        {
            // 观察角色功能
            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ButtonType.bIsRequestSelf = (nSelfID == m_nActorID);
            ButtonType.nRequestPDBID = m_nActorID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
        }
    }

    // 加好友
    public class UPopupItemDataAddFriend : UListComponentItemDataPopup
    {
        private int m_userID;
        private string m_actorName;

        // 弹出菜单元素数据内容
        public UPopupItemDataAddFriend(int nUserID, string actorName, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_userID = nUserID;
            m_actorName = actorName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "AddFriend");
        }

        private void OnCallBack()
        {
			SNSAddBuddyWndMsgData msgData = new SNSAddBuddyWndMsgData();
			msgData.msgID = (int)WndMsgID.WND_MSG_SNS_ADD_BUDDY;
			msgData.actorName = m_actorName;
			msgData.nUserID = m_userID;

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_ADD_BUDDY, msgData);
		}
	}

    // 复制名字
    public class UPopupItemDataCopyName : UListComponentItemDataPopup
    {
        private String m_strName;

        // 弹出菜单元素数据内容
        public UPopupItemDataCopyName(String strName, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_strName = strName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "CopyName");
        }

        private void OnCallBack()
        {
            if (m_strName != null && m_strName.Length > 0)
                GUIUtility.systemCopyBuffer = m_strName;
        }
    }

    // 邀请加入战队
    public class UPopupItemDataKinInvite : UListComponentItemDataPopup
    {
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataKinInvite(int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinInvite");
        }

        private void OnCallBack()
        {
            // 邀请加入战队功能
            LogicDataCenter.kinDataManager.OnKinInvite(m_nActorID);
        }
    }

    // 邀请加入联盟
    public class UPopupItemDataClanInvite : UListComponentItemDataPopup
    {
        private int m_nEntityID;
        private int m_nActorID;

        // 弹出菜单元素数据内容
        public UPopupItemDataClanInvite(int nEntityID, int nActorID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nEntityID = nEntityID;
            m_nActorID = nActorID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInvite");
        }

        private void OnCallBack()
        {
            // 邀请加入联盟功能

            EntityView ev = EntityFactory.getEntityViewByID(m_nEntityID);
            if (ev == null)
            {
                return;
            }

            if (ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) != 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_INVITE_HAVA_CLAN);
            }
            else if (ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN) != 0)
            {
                UTopName topName = UTopNameManager.Instance.FindTopName(m_nEntityID);
                string content = ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteNoClanContent");
                ShowDialog(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteNoClanTitle"),
                    topName != null ? String.Format(content, topName.RoleName, topName.KinName) : content,
                    new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), InviteJoinClan }, { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } });
            }
            else
            {
                ShowDialog(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteNoKinTitle"),
                    ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanInviteNoKinContent"),
                    new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), InviteJoinKin }, { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } });
            }
        }

        private bool InviteJoinClan()
        {
            LogicDataCenter.clanDataManager.OnClanInvite(m_nActorID);
            return true;
        }

        private bool InviteJoinKin()
        {
            LogicDataCenter.kinDataManager.OnKinInvite(m_nActorID);
            return true;
        }

        private void ShowDialog(string title, string content, DialogActions buttons)
        {
            UBB.toHtml(ref content);
            USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                        title: title,
                        message: content,
                        modal: true,
                        hideByModalClick: false,
                        buttons: buttons
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
    }

    // 请求加入家族
    public class UPopupItemDataKinRequest : UListComponentItemDataPopup
    {
        private int m_nNewKinID;

        // 弹出菜单元素数据内容
        public UPopupItemDataKinRequest(int nNewKinID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nNewKinID = nNewKinID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinRequest");
        }

        private void OnCallBack()
        {
            // 申请加入战队功能
            //UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "此功能尚未支持，敬请期待");
            LogicDataCenter.kinDataManager.OnKinApply(m_nNewKinID);
        }
    }

    public class UPopupItemDataBuddyChat : UListComponentItemDataPopup
    {
        private int m_dwUserID;

        // 弹出菜单元素数据内容
        public UPopupItemDataBuddyChat(int dwUserID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwUserID = dwUserID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "Chat");
        }

        private void OnCallBack()
        {
            SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
            msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
            msgData.SessionID = m_dwUserID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
        }


    }

    // 密聊
    public class UPopupItemDataPrivateChat : UListComponentItemDataPopup
    {
        private string m_ActorName;

        // 弹出菜单元素数据内容
        public UPopupItemDataPrivateChat(string actorName, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_ActorName = actorName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "PrivateChat");
        }

        private void OnCallBack()
        {
            // 通知触发私聊
            ChatBoxPrivateChat msgData = new ChatBoxPrivateChat();
            msgData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT;
            msgData.name = m_ActorName;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_PRIVATECHAT, msgData);
        }
    }

    // 修改备注名
    public class UPopupItemDataUpdateRemark : UListComponentItemDataPopup
    {
        private int m_dwUserID;
        private string m_ActorName;

        // 弹出菜单元素数据内容
        public UPopupItemDataUpdateRemark(int dwUserID, string actorName, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwUserID = dwUserID;
            m_ActorName = actorName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "UpdateRemark");
        }

        private void OnCallBack()
        {
            SNSOpenInputDialogMsgData msgData = new SNSOpenInputDialogMsgData();
            msgData.type = SNSOpenInputDialogMsgData.EMDialogType.EM_CHANGE_REMARK;
            msgData.title = ULocalizationService.Instance.Get("UIView", "SNS", "ChangeRemarkTitle");
            msgData.defaultText = LogicDataCenter.snsDataManager.getBuddyRemark(m_dwUserID);
            msgData.placeHolderText = ULocalizationService.Instance.Get("UIView", "SNS", "InputNewRemark");
            msgData.onClickDelegate = (text)=>{
                string remark = text;
                if (remark.Length >= 10)
                    return;

                if (m_dwUserID <= 0)
                    return;
                LogicDataCenter.snsDataManager.reqChangeBuddyName(m_dwUserID, m_ActorName, remark);
            };

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, msgData);
        }
    }

    // 删除好友
    public class UPopupItemDataDelBuddy : UListComponentItemDataPopup
    {
        private int m_dwUserID;
        private string m_ActorName;

        // 弹出菜单元素数据内容
        public UPopupItemDataDelBuddy(int dwUserID, string actorName, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwUserID = dwUserID;
            m_ActorName = actorName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "DelBuddy");
        }

        private void OnCallBack()
        {
            UIDialogData toolTipData = new UIDialogData(
                title: ULocalizationService.Instance.Get("UIView", "SNS", "DelBuddyWndTitle"),
                message: String.Format(ULocalizationService.Instance.Get("UIView", "SNS", "DelBuddyWndContent"), m_ActorName),
                buttons: new UIWidgets.DialogActions() {
                         {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {LogicDataCenter.snsDataManager.reqDelBuddy(m_dwUserID, m_ActorName); return true; } },
                          {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),() => {return true; } }
            });

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
        }
    }

    // 加入黑名单
    public class UPopupItemDataAddBlackList : UListComponentItemDataPopup
    {
        private int m_dwUserID;
        private string m_ActorName;

        // 弹出菜单元素数据内容
        public UPopupItemDataAddBlackList(int dwUserID, string actorName,  UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwUserID = dwUserID;
            m_ActorName = actorName;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "AddBlackList");
        }

        private void OnCallBack()
        {
            UIDialogData toolTipData = new UIDialogData(
              title: ULocalizationService.Instance.Get("UIView", "SNS", "AddBlackListWndTitle"),
              message: String.Format(ULocalizationService.Instance.Get("UIView", "SNS", "AddBlackListWndContent"), m_ActorName),
              buttons: new UIWidgets.DialogActions() {
                         {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {LogicDataCenter.snsDataManager.reqAddBlackList(m_dwUserID, m_ActorName); return true; } },
                          {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"),() => {return true; } }
              });

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
        }
    }

    // 移到某一分组
    public class UPopupItemDataChangeBuddyGroup : UListComponentItemDataPopup
    {
        private int m_dwUserID;
        private int m_dwBuddyGroupID;

        // 弹出菜单元素数据内容
        public UPopupItemDataChangeBuddyGroup(int dwUserID, int dwBuddyGroupID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0, List<UListComponentItemDataPopupBase> _itemChildItems = null)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth, _itemChildItems)
        {
            m_dwUserID = dwUserID;
            m_dwBuddyGroupID = dwBuddyGroupID;
            ItemSelectedAction = OnCallBack;
        }

        private void OnCallBack()
        {
            LogicDataCenter.snsDataManager.reqChangeBuddyGroup(m_dwUserID, m_dwBuddyGroupID);
        }
    }

    // 添加好友分组
    public class UPopupItemDataAddBuddyGroup : UListComponentItemDataPopup
    {
        // 弹出菜单元素数据内容
        public UPopupItemDataAddBuddyGroup(UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "AddBuddyGroup");
        }

        private void OnCallBack()
        {
            SNSOpenInputDialogMsgData msgData = new SNSOpenInputDialogMsgData();
            msgData.type = SNSOpenInputDialogMsgData.EMDialogType.EM_ADD_BUDDYGROUP;
            msgData.title = ULocalizationService.Instance.Get("UIView", "SNS", "AddNewBuddyGroup");
            msgData.defaultText = "";
            msgData.placeHolderText = ULocalizationService.Instance.Get("UIView", "SNS", "InputNewBuddyGroupName");
            msgData.onClickDelegate = (text) =>
            {
                string newName = text;
                if (newName.Length <= 0 || newName.Length >= 10)
                    return;

                LogicDataCenter.snsDataManager.reqAddBuddyGroup(newName);
            };

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, msgData);
        }
    }

    // 修改好友分组名称
    public class UPopupItemDataUpdateBuddyGroupName : UListComponentItemDataPopup
    {
        private int m_dwBuddyGroupID;

        // 弹出菜单元素数据内容
        public UPopupItemDataUpdateBuddyGroupName(int dwBuddyGroupID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwBuddyGroupID = dwBuddyGroupID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "UpdateBuddyGroupName");
        }

        private void OnCallBack()
        {
            SNSOpenInputDialogMsgData msgData = new SNSOpenInputDialogMsgData();
            msgData.type = SNSOpenInputDialogMsgData.EMDialogType.EM_CHANGE_BUDDYGROUP_NAME;
            msgData.title = ULocalizationService.Instance.Get("UIView", "SNS", "ChangeBuddyGroupTitle");
            msgData.defaultText = LogicDataCenter.snsDataManager.getBuddyGroup(m_dwBuddyGroupID).Info.szBuddyGroupName;
            msgData.placeHolderText = ULocalizationService.Instance.Get("UIView", "SNS", "InputNewBuddyGroupName");
            msgData.onClickDelegate = (text) =>
            {
                string newName = text;
                if (newName.Length <= 0 || newName.Length >= 10)
                    return;

                if (m_dwBuddyGroupID <= 0)
                    return;
                LogicDataCenter.snsDataManager.reqChangeBuddyGroupName(m_dwBuddyGroupID, newName);
            };

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INPUTDIALOG, msgData);
        }
    }

    // 删除好友分组
    public class UPopupItemDataDelBuddyGroup : UListComponentItemDataPopup
    {
        private int m_dwBuddyGroupID;

        // 弹出菜单元素数据内容
        public UPopupItemDataDelBuddyGroup(int dwBuddyGroupID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_dwBuddyGroupID = dwBuddyGroupID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "DelBuddyGroup");
        }

        private void OnCallBack()
        {
            LogicDataCenter.snsDataManager.reqDelBuddyGroup(m_dwBuddyGroupID);
        }
    }
    public class UPopupItemDataTransmitTo : UListComponentItemDataPopup
    {
        private int m_nWorldID = 0;
        private int m_nServerID = 0;
        private int m_nSceneID = 0;

        // 弹出菜单元素数据内容
        public UPopupItemDataTransmitTo(int nWorldID, int nServerID, int nSceneID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nWorldID = nWorldID;
            m_nServerID = nServerID;
            m_nSceneID = nSceneID;

            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "Common", "TransmitTo");
        }

        private void OnCallBack()
        {
            Debug.Log(String.Format("transmitTo, worldID={0}, serverID={1}, sceneID={2}", m_nWorldID, m_nServerID, m_nSceneID));

            if (m_nWorldID == 0 || m_nSceneID == 0)
                return;

            gamelogic_migrate_switch_scene cmd = new gamelogic_migrate_switch_scene();
            cmd.nGameWorldID = m_nWorldID;
            cmd.nServerID = m_nServerID;
            cmd.nSceneID = m_nSceneID;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MIGRATE_SWITCH_SCENE, ref cmd);
        }
    }


    // 解散师徒
    public class UPopupItemDataDismissMentorship : UListComponentItemDataPopup
    {
        private int m_MentorshipID;

        // 弹出菜单元素数据内容
        public UPopupItemDataDismissMentorship(int nMentorshipID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_MentorshipID = nMentorshipID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "DismissMentorship");
        }

        private void OnCallBack()
        {
            if (m_MentorshipID > 0)
                LogicDataCenter.mentorshipDataManager.SendDismissMentorship(m_MentorshipID);
        }
    }

    // 提升学徒
    public class UPopupItemDataPromotePrentice : UListComponentItemDataPopup
    {
        private int m_MentorshipID;
        private int m_DesPDBID;

        // 弹出菜单元素数据内容
        public UPopupItemDataPromotePrentice(int nMentorshipID, int nDesPDBID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_MentorshipID = nMentorshipID;
            m_DesPDBID = nDesPDBID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "PromotePrentice");
        }

        private void OnCallBack()
        {
            if (m_MentorshipID > 0 && m_DesPDBID > 0)
                LogicDataCenter.mentorshipDataManager.SendPromotePrentice(m_MentorshipID, m_DesPDBID);
        }
    }

    // 申请正式学徒
    public class UPopupItemDataApplyFormalPrentice : UListComponentItemDataPopup
    {
        private int m_DesPDBID;

        // 弹出菜单元素数据内容
        public UPopupItemDataApplyFormalPrentice(int nDesPDBID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_DesPDBID = nDesPDBID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "ApplyFormalPrentice");
        }

        private void OnCallBack()
        {
            if (m_DesPDBID > 0)
                LogicDataCenter.mentorshipDataManager.SendApplyFormalPrentice(m_DesPDBID);
        }
    }

    // 申请见习学徒
    public class UPopupItemDataApplyInternPrentice : UListComponentItemDataPopup
    {
        private int m_DesPDBID;

        // 弹出菜单元素数据内容
        public UPopupItemDataApplyInternPrentice(int nDesPDBID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_DesPDBID = nDesPDBID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "ApplyInternPrentice");
        }

        private void OnCallBack()
        {
            if (m_DesPDBID > 0)
                LogicDataCenter.mentorshipDataManager.SendApplyInternPrentice(m_DesPDBID);
        }
    }

    // 申请见习师傅
    public class UPopupItemDataApplyInternMaster : UListComponentItemDataPopup
    {
        private int m_DesPDBID;

        // 弹出菜单元素数据内容
        public UPopupItemDataApplyInternMaster(int nDesPDBID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_DesPDBID = nDesPDBID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "ApplyInternMaster");
        }

        private void OnCallBack()
        {
            if (m_DesPDBID > 0)
                LogicDataCenter.mentorshipDataManager.SendApplyInternMaster(m_DesPDBID);
        }
    }
    
    // 查看战队
    public class UPopupItemDataKinView : UListComponentItemDataPopup
    {
        private int m_nKinID;

        // 弹出菜单元素数据内容
        public UPopupItemDataKinView(int nKinID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nKinID = nKinID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinView");
        }

        private void OnCallBack()
        {
            // 查看战队信息
            LogicDataCenter.kinDataManager.viewCompetitionKinInfo(m_nKinID);
        }
    }

    // 查看战队参赛人员(杯赛专有弹窗选项)
    public class UPopupItemDataCompetitionKinMemberView : UListComponentItemDataPopup
    {
        private int m_nKinID;

        // 弹出菜单元素数据内容
        public UPopupItemDataCompetitionKinMemberView(int nKinID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_nKinID = nKinID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinMemberView");
        }

        private void OnCallBack()
        {
            // 查看战队参赛人员
            LogicDataCenter.kinDataManager.viewCompetitionKinMember(m_nKinID);
        }
    }

    // 查看报名阶段战队参赛人员(杯赛专有弹窗选项)
    public class UPopupItemDataRegistKinMemberView : UListComponentItemDataPopup
    {
        private long m_lLegendCupID;
        private int m_nKinID;

        // 弹出菜单元素数据内容
        public UPopupItemDataRegistKinMemberView(long lLegendCupID, int nKinID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                            Sprite _sprItemPopup = null, int _nDepth = 0)

            : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_lLegendCupID = lLegendCupID;
            m_nKinID = nKinID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinMemberView");
        }

        private void OnCallBack()
        {
            // 查看战队参赛人员
            LegendCupRegistKinData data = new LegendCupRegistKinData();
            data.lLegendCupID = m_lLegendCupID;
            data.nKinID = m_nKinID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_REGIST_SHOW, data);
        }
    }

    public class UPopupItemDataPK : UListComponentItemDataPopup
    {
        private int m_entityID;

        // 弹出菜单元素数据内容
        public UPopupItemDataPK(int nEntityID, UnityAction _actionClearPopupMenu = null, string _strItemTypeName = "", int _nGroupID = 0,
                                                Sprite _sprItemPopup = null, int _nDepth = 0)

        : base(null, _actionClearPopupMenu, _strItemTypeName, _nGroupID, _sprItemPopup, _nDepth)
        {
            m_entityID = nEntityID;
            ItemSelectedAction = OnCallBack;
        }

        static public String GetDescString()
        {
            return ULocalizationService.Instance.Get("UIView", "PK", "RequestPK");
        }

        private void OnCallBack()
        {
            if (DataCenter.LogicDataCenter.playerSystemDataManager.SelfGameState == (int)ACTOR_GAME_STATE.GAME_STATE_MATCH)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_PK_MATCHSTATE);
                return;
            }

            cmd_creature_pk_choose_target data = new cmd_creature_pk_choose_target();
            data.uidTarget = (uint)m_entityID;

            EntityEventHelper.Instance.SendCommand<cmd_creature_pk_choose_target>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_PK_CHOOSE_TARGET, ref data);
        }
    }
}

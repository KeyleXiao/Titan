using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using DataCenter;
using GameLogic;
using UnityEngine.UI;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgets;
using UnityEngine;

namespace USpeedUI.LegendCup
{
    public class LegendCupBlacklistWnd : UIPopupWnd<LegendCupBlacklistWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_BLACKLIST;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupBlacklistView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_BLACKLIST, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_BLACKLIST, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_SHOW:
                    {
                        SetVisible(true);

                        LegendCupIDData cmdData = (LegendCupIDData)data;
                        if (m_wndView != null)
                            m_wndView.OnOpenBlacklistWnd(cmdData.nLegendCupID);
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_BLACKLIST_BLACKLIST:
                    {
                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnRecvBlacklist(cmdData.nLegendCupID);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }
    }

    public class LegendCupBlacklistWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text DeleteBtnDes;
        public Text CancelBtnDes;

        public UListLegendCupBlacklist LegendCupBlacklist;
        private int m_CurSelectIndex;
        private UListItemLegendCupBlacklist m_CurSelectItem;

        private long nLegendCupID;
        private const int SinglePageMaxCount = 8;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupBlacklist", "Title");
            DeleteBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupBlacklist", "DeleteBtn");
            CancelBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupBlacklist", "CancelBtn");

            LegendCupBlacklist.UserItemCount = SinglePageMaxCount;

            return true;
        }

        public void OnOpenBlacklistWnd(long nCupID)
        {
            nLegendCupID = nCupID;
            cmd_legendcup_req_regist_blacklist reqData = new cmd_legendcup_req_regist_blacklist();
            reqData.nLegendCupID = nCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_regist_blacklist>(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST, ref reqData);
        }

        public void OnRecvBlacklist(long nCupID)
        {
            SingleRegistBlacklistInfo blacklist = LogicDataCenter.legendCupDataManager.GetSingleRegistBlacklistInfo(nCupID);
            if (blacklist == null)
                return;

            LegendCupBlacklist.OnSelect.RemoveListener(OnSelectCupItem);
            LegendCupBlacklist.DataSource.Clear();
            ObservableList<UListItemLegendCupBlacklist> dataSource = new ObservableList<UListItemLegendCupBlacklist>();
            int nIndex = 1;
            foreach (cmd_legendcup_recv_regist_balcklistnode node in blacklist.nodeInfo)
            {
                UListItemLegendCupBlacklist item = new UListItemLegendCupBlacklist();
                item.nIndex = nIndex;
                item.nKinID = node.nKinID;
                item.strKinName = node.szKinName;
                dataSource.Add(item);
                nIndex++;
            }
            LegendCupBlacklist.DataSource = dataSource;
            LegendCupBlacklist.OnSelect.AddListener(OnSelectCupItem);
        }

        public void OnSelectCupItem(int index, ListViewItem item)
        {
            m_CurSelectIndex = index;
            m_CurSelectItem = LegendCupBlacklist.DataSource[index];
        }

        public void OnClickDeleteBtn()
        {
            if (m_CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_SELECT_RIGHT_KINID);
                return;
            }
            cmd_legendcup_delete_regist_blacklist deleteblacklist = new cmd_legendcup_delete_regist_blacklist();
            deleteblacklist.nLegendCupID = nLegendCupID;
            deleteblacklist.nKinID = m_CurSelectItem.nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_delete_regist_blacklist>(GameLogicDef.GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST, ref deleteblacklist);
        }

        public void OnClickCancelBtn()
        {
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}

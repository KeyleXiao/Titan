using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using Mail_ManagerDef;

namespace USpeedUI.MailBox
{
    public enum MailTimerID
    {
        TimerID_ReqFastRecv_OverTime,
    }

    public class MailWnd : UIBaseSubWnd<MailWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)MailBoxSubWndType.WND_MAIL;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "MailBox/UViewMail";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            SetVisible(false);
            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
                OnOpenMailWnd();
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_MAIL_REQUESTMAILBASELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.onRecvMailBaseDataList();
                    }
                    break;
                case WndMsgID.WND_MSG_MAIL_DETAILINFO:
                    {
                        if (m_wndView != null)
                            m_wndView.onRecvMailDetailInfo(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_MAIL_REQFASTRECVPLUS:
                    {
                        if (m_wndView != null)
                            m_wndView.onRecvMailFastRecvPlus(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_MAIL_RECVPLUSSUCCESS:
                    {
                        if (m_wndView != null)
                            m_wndView.onRecvMailPlusSuccess(msgID, msgData);
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnOpenMailWnd()
        {
            // 打开邮件列表
            GameLogic.gameview_mail_req_basedatalist cmdData = new GameLogic.gameview_mail_req_basedatalist();
            ViewEventHelper.Instance.SendCommand<GameLogic.gameview_mail_req_basedatalist>(GameLogicDef.GVIEWCMD_MAIL_REQUESTBASEDATALIST, ref cmdData);
        }

        public void ResetViewShow()
        {
            if (m_wndView != null)
                m_wndView.Clear();
        }
    }

    public class MailContentFrame : MonoBehaviour
    {
        public Text mailTitle;
        public Text senderName;
        public Text mailContent;
        public UListMailPlusData ListMailPlusNum;
        public UListMailGoods ListMailGoods;

        public Button recvBtn;
        public Text recvConfirmDesc;

        private MailWndView m_MailWndView;

        private long m_nMailID;


        public bool Init(MailWndView mailWndView)
        {
            m_nMailID = 0;
            m_MailWndView = mailWndView;
            return true;
        }

        public void Clear()
        {
            m_nMailID = 0;
            mailTitle.text = "";
            senderName.text = "";
            mailContent.text = "";
            recvConfirmDesc.text = "";

            ListMailPlusNum.DataSource.BeginUpdate();
            ListMailPlusNum.DataSource.Clear();
            ListMailPlusNum.DataSource.EndUpdate();

            ListMailGoods.DataSource.BeginUpdate();
            ListMailGoods.DataSource.Clear();
            ListMailGoods.DataSource.EndUpdate();
        }

        public void SetMailContentData(String strMailTitle, String strSenderName, GameLogic.gameview_mail_rsp_detaildata detailData)
        {
            m_nMailID = detailData.nMailID;

            recvConfirmDesc.text = ULocalizationService.Instance.Get("UIView", "Mail", "RecvConfirmDes");

            recvBtn.gameObject.SetActive(false);
            recvConfirmDesc.gameObject.SetActive(false);

            mailTitle.text = ULocalizationService.Instance.Get("UIView", "Mail", "MailTitleDesc") + strMailTitle;
            senderName.text = ULocalizationService.Instance.Get("UIView", "Mail", "SenderNameDesc") + strSenderName;
            mailContent.text = "    " + ULocalizationService.Instance.Get("UIView", "Mail", "MailContentDesc") + detailData.szContext;

            ListMailPlusNum.DataSource.BeginUpdate();
            ListMailPlusNum.DataSource.Clear();
            ListMailGoods.DataSource.BeginUpdate();
            ListMailGoods.DataSource.Clear();

            bool bHavePlus = false;

            if (detailData.nPlusMoney > 0 || detailData.nPlusExp > 0)
            {
                bHavePlus = true;
                int nIndex = 1;
                UIWidgets.ObservableList<UListItemMailPlusData> dataSource = new UIWidgets.ObservableList<UListItemMailPlusData>();
                if (detailData.nPlusMoney > 0)
                {
                    UListItemMailPlusData itemMoney = new UListItemMailPlusData();
                    itemMoney.nIndex = nIndex;
                    itemMoney.szName = ULocalizationService.Instance.Get("UIView", "Mail", "MailPlusMoneyDesc");
                    itemMoney.nCount = detailData.nPlusMoney;

                    dataSource.Add(itemMoney);

                    ++nIndex;
                }

                if (detailData.nPlusExp > 0)
                {
                    UListItemMailPlusData itemExp = new UListItemMailPlusData();
                    itemExp.nIndex = nIndex;
                    itemExp.szName = ULocalizationService.Instance.Get("UIView", "Mail", "MailPlusExpDesc");
                    itemExp.nCount = detailData.nPlusExp;

                    dataSource.Add(itemExp);
                }

                ListMailPlusNum.DataSource = dataSource;
            }

            if (detailData.nPlusGoodsNum > 0)
            {
                int nGoodsIndex = 1;
                UIWidgets.ObservableList<UListItemMailGoods> goodsDataSource = new UIWidgets.ObservableList<UListItemMailGoods>();
                for (int i = 0; i < detailData.nPlusGoodsNum; ++i)
                {
                    int nGoodsID = detailData.sGoodsList[i].nGoodsID;
                    if (nGoodsID > 0)
                    {
                        UListItemMailGoods itemGoods = new UListItemMailGoods();
                        itemGoods.nIndex = nGoodsIndex;
                        itemGoods.nGoodsID = nGoodsID;
                        itemGoods.nCount = detailData.sGoodsList[i].nCount;

                        goodsDataSource.Add(itemGoods);

                        bHavePlus = true;

                        ++nGoodsIndex;
                    }
                }

                ListMailGoods.DataSource = goodsDataSource;
            }

            ListMailPlusNum.DataSource.EndUpdate();
            ListMailGoods.DataSource.EndUpdate();

            int nMailType = LogicDataCenter.mailDataManager.GetMailType(m_nMailID);
            if (bHavePlus && (nMailType == (int)EMMailType.emMailType_Player || nMailType == (int)EMMailType.emMailType_System))
            {
                if (LogicDataCenter.mailDataManager.PlusMailList.Contains(m_nMailID))
                {
                    recvBtn.gameObject.SetActive(true);
                }
                else
                {
                    recvConfirmDesc.gameObject.SetActive(true);
                }                
            }
        }

        public void OnRecvPlusBtnClick()
        {
            if (m_nMailID <= 0)
                return;

            recvBtn.gameObject.SetActive(false);

            if (LogicDataCenter.mailDataManager.PlusMailList.Contains(m_nMailID))
                LogicDataCenter.mailDataManager.PlusMailList.Remove(m_nMailID);

            GameLogic.gameview_mail_req_fastrecvplus cmdData = new GameLogic.gameview_mail_req_fastrecvplus();
            cmdData.nMailID = m_nMailID;
            ViewEventHelper.Instance.SendCommand<GameLogic.gameview_mail_req_fastrecvplus>(GameLogicDef.GVIEWCMD_MAIL_REQUESTFASTRECVPLUS, ref cmdData);
        }

        public void SetMailRecvConfirm(long nMailID)
        {
            if (nMailID == m_nMailID)
                recvConfirmDesc.gameObject.SetActive(true);
        }
    }

    public class MailWndView : UIBaseSubWndView, ITimerHanlder
    {
        //public Text topText;
        public Text EmptyDesc;
        public UListMail ListMail;
        public MailContentFrame mailContentFrame;
        public GameObject EmptyShowObj;

        private int nCurSelectIndex;
        private UListItemMail CurSelectItem;


        public override bool Init(IUISubWnd wnd)
        {
            mailContentFrame.Init(this);

            EmptyShowObj.SetActive(true);

            EmptyDesc.text = ULocalizationService.Instance.Get("UIView", "Mail", "EmptyDesc");

            return base.Init(wnd);
        }

        public void Clear()
        {
            mailContentFrame.Clear();
            ListMail.DataSource.Clear();
        }

        public void onRecvMailBaseDataList()
        {
            mailContentFrame.Clear();

            ListMail.OnSelect.RemoveListener(onSelectMail);
            ListMail.DataSource.BeginUpdate();
            ListMail.DataSource.Clear();

            UIWidgets.ObservableList<UListItemMail> dataSource = new UIWidgets.ObservableList<UListItemMail>();

            int nIndex = 1;
            foreach (var baseInfo in LogicDataCenter.mailDataManager.MailBaseInfoList)
            {
                UListItemMail item = new UListItemMail();
                item.nIndex = nIndex;
                item.nMailID = baseInfo.nMailID;
                item.nType = baseInfo.nType;
                item.szTitle = baseInfo.szTitle;
                item.nIsPlusData = baseInfo.nIsPlusData;
                item.nSourceType = baseInfo.nSourceType;
                item.szSenderName = baseInfo.szSenderName;
                item.nSenderPDBID = baseInfo.nSenderPDBID;
                item.nRecvTime = baseInfo.nRecvTime;
                item.nLifeTime = baseInfo.nLifeTime;
                item.nReadFlag = baseInfo.nIsRead;

                dataSource.Add(item);
                nIndex++;
            }

            ListMail.DataSource = dataSource;
            ListMail.OnSelect.AddListener(onSelectMail);
            ListMail.DataSource.EndUpdate();

            if (ListMail.DataSource.Count > 0)
            {
                EmptyShowObj.SetActive(false);
                mailContentFrame.gameObject.SetActive(true);
                ListMail.Select(0);
            }
            else
            {
                EmptyShowObj.SetActive(true);
                mailContentFrame.gameObject.SetActive(false);
            }

            //topText.text = String.Format(ULocalizationService.Instance.Get("UIView", "Mail", "TopTextDes"), nTmpCount, m_MailTotalCount);
        }

        public void onRecvMailDetailInfo(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
            GameLogic.gameview_mail_rsp_detaildata detailData = helper.get<GameLogic.gameview_mail_rsp_detaildata>();

            if (CurSelectItem != null && CurSelectItem.nMailID == detailData.nMailID)
            {
                mailContentFrame.SetMailContentData(CurSelectItem.szTitle, CurSelectItem.szSenderName, detailData);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_NEWMAILCOUNT, null);
        }

        public void onRecvMailFastRecvPlus(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
            GameLogic.gameview_mail_req_fastrecvplus fastRecvData = helper.get<GameLogic.gameview_mail_req_fastrecvplus>();

            StopFastRecvPlus(false);
        }

        public void onRecvMailPlusSuccess(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
            GameLogic.gameview_mail_recvsuccess recvSuccess = helper.get<GameLogic.gameview_mail_recvsuccess>();

            mailContentFrame.SetMailRecvConfirm(recvSuccess.nMailID);

            LogicDataCenter.mailDataManager.ClearFastRecvData();
            LogicDataCenter.mailDataManager.AddFastRecvData(recvSuccess.nMailID);

            if (LogicDataCenter.mailDataManager.IsHaveFastRecvPlusData())
                UISystem.Instance.ShowWnd(WndID.WND_ID_MAIL_MSGBOXFASTRECV, true);
        }

        public virtual void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)MailTimerID.TimerID_ReqFastRecv_OverTime:
                    {
                        StopFastRecvPlus();
                    }
                    break;
                default:
                    break;
            }
        }

        public void StopFastRecvPlus(bool bUpdateFlag = false)
        {
            // 杀掉快速取件定时器
            TimerManager.KillTimer(this, (int)MailTimerID.TimerID_ReqFastRecv_OverTime);

            if (bUpdateFlag)
            {            
                UpdateListMail();

                if (LogicDataCenter.mailDataManager.IsHaveFastRecvPlusData())
                    UISystem.Instance.ShowWnd(WndID.WND_ID_MAIL_MSGBOXFASTRECV, true);
            }
        }

        public void onSelectMail(int index, UIWidgets.ListViewItem item)
        {
            // 选择当前数据
            nCurSelectIndex = index;
            CurSelectItem = ListMail.DataSource[index];
            ListMail.DataSource[index].nReadFlag = 1;

            if (LogicDataCenter.mailDataManager.MailDetailInfoList.ContainsKey(CurSelectItem.nMailID))
            {
                mailContentFrame.SetMailContentData(CurSelectItem.szTitle, CurSelectItem.szSenderName, LogicDataCenter.mailDataManager.MailDetailInfoList[CurSelectItem.nMailID]);
                return;
            }
            
            GameLogic.gameview_mail_req_detaildata cmdData = new GameLogic.gameview_mail_req_detaildata();
            cmdData.nMailID = CurSelectItem.nMailID;
            ViewEventHelper.Instance.SendCommand<GameLogic.gameview_mail_req_detaildata>(GameLogicDef.GVIEWCMD_MAIL_REQUESTDETAILDATA, ref cmdData);
        }

        private void UpdateListMail()
        {
            ListMail.DataSource.BeginUpdate();
            ListMail.DataSource.Clear();

            UIWidgets.ObservableList<UListItemMail> dataSource = new UIWidgets.ObservableList<UListItemMail>();

            int nIndex = 1;
            foreach (var baseInfo in LogicDataCenter.mailDataManager.MailBaseInfoList)
            {
                UListItemMail item = new UListItemMail();
                item.nIndex = nIndex;
                item.nMailID = baseInfo.nMailID;
                item.nType = baseInfo.nType;
                item.szTitle = baseInfo.szTitle;
                item.nIsPlusData = baseInfo.nIsPlusData;
                item.nSourceType = baseInfo.nSourceType;
                item.szSenderName = baseInfo.szSenderName;
                item.nSenderPDBID = baseInfo.nSenderPDBID;
                item.nRecvTime = baseInfo.nRecvTime;
                item.nLifeTime = baseInfo.nLifeTime;
                item.nReadFlag = baseInfo.nReadFlag;

                dataSource.Add(item);
                nIndex++;
            }

            ListMail.DataSource = dataSource;
            ListMail.DataSource.EndUpdate();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.MentorshipConfig;
using Mentorship_ManagerDef;
using ASpeedGame.Data.ShieldWorld;
using DataCenter;

namespace USpeedUI.MsgBoxMailFastRecv
{
    public class MsgBoxMailFastRecvWnd : UIBaseWnd<MsgBoxMailFastRecvWndView>
    {
        private MsgBoxMailFastRecvWndView view { get { return m_wndView as MsgBoxMailFastRecvWndView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_MAIL_MSGBOXFASTRECV;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "MailBox/UViewMsgBoxMailFastRecv";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 预先载入
            //LoadView();
            SetVisible(false);

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible && view != null)
                view.SetData();
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                default:
                    break;
            }
        }

    }

    public class MsgBoxMailFastRecvWndView : UIBaseWndView
    {
        public Text titleText;
        public Text recvPlusText;

        public UListMailPlusData ListMailPlusNum;
        public UListMailGoods ListMailGoods;

        private int m_GoodsMaxNum = 5;

        public override bool Init(IUIWnd wnd)
        {
            titleText.text = ULocalizationService.Instance.Get("UIView", "Mail", "FastRecvPlusBoxTitle");

            SetData();

            return base.Init(wnd);
        }

        public void SetData()
        {
            recvPlusText.text = "";

            ListMailPlusNum.DataSource.BeginUpdate();
            ListMailPlusNum.DataSource.Clear();

            ListMailPlusNum.transform.localPosition = new UnityEngine.Vector3(-1.2f, 90.3f, 0f);

            if (LogicDataCenter.mailDataManager.SumPlusMoney > 0 || LogicDataCenter.mailDataManager.SumPlusExp > 0)
            {
                int nIndex = 1;
                UIWidgets.ObservableList<UListItemMailPlusData> dataSource = new UIWidgets.ObservableList<UListItemMailPlusData>();

                if (LogicDataCenter.mailDataManager.SumPlusMoney > 0)
                {
                    UListItemMailPlusData itemMoney = new UListItemMailPlusData();
                    itemMoney.nIndex = nIndex;
                    itemMoney.szName = ULocalizationService.Instance.Get("UIView", "Mail", "MailPlusMoneyDesc");
                    itemMoney.nCount = LogicDataCenter.mailDataManager.SumPlusMoney;

                    dataSource.Add(itemMoney);

                    ++nIndex;
                }

                if (LogicDataCenter.mailDataManager.SumPlusExp > 0)
                {
                    UListItemMailPlusData itemExp = new UListItemMailPlusData();
                    itemExp.nIndex = nIndex;
                    itemExp.szName = ULocalizationService.Instance.Get("UIView", "Mail", "MailPlusExpDesc");
                    itemExp.nCount = LogicDataCenter.mailDataManager.SumPlusExp;

                    dataSource.Add(itemExp);
                }

                ListMailPlusNum.DataSource = dataSource;
            }

            ListMailPlusNum.DataSource.EndUpdate();

            ListMailGoods.DataSource.BeginUpdate();
            ListMailGoods.DataSource.Clear();

            int nTmpGoodsNum = LogicDataCenter.mailDataManager.FastRecvGoodsList.Count;
            if (nTmpGoodsNum > 0)
            {
                int nGoodsIndex = 1;
                UIWidgets.ObservableList<UListItemMailGoods> goodsDataSource = new UIWidgets.ObservableList<UListItemMailGoods>();
                foreach (var item in LogicDataCenter.mailDataManager.FastRecvGoodsList)
                {
                    if (nGoodsIndex > m_GoodsMaxNum)
                        break;

                    UListItemMailGoods itemGoods = new UListItemMailGoods();
                    itemGoods.nIndex = nGoodsIndex;
                    itemGoods.nGoodsID = item.Key;
                    itemGoods.nCount = item.Value.nCount;

                    goodsDataSource.Add(itemGoods);

                    ++nGoodsIndex;
                }

                ListMailGoods.DataSource = goodsDataSource;
            }

            ListMailGoods.DataSource.EndUpdate();

            if (ListMailGoods.DataSource.Count == 0)
                ListMailPlusNum.transform.localPosition = new UnityEngine.Vector3(-1.2f, 25.1f, 0f);

            if (nTmpGoodsNum > m_GoodsMaxNum)
                recvPlusText.text = ULocalizationService.Instance.Get("UIView", "Mail", "FastRecvPlusBoxDesc");
        }

        public void OnColseBtnClick()
        {
            LogicDataCenter.mailDataManager.ClearFastRecvData();
            // 关闭该窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_MAIL_MSGBOXFASTRECV, false);
        }
    }
}

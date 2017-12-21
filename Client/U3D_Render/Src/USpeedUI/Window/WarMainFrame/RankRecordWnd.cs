using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using GameLogic;
using UIWidgets;

namespace USpeedUI.WarMainFrame
{
    public class RankRecordWnd : UIBaseWnd<RankRecordWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_RANK_RECORD;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "WarWnd/RankRecordView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            //return WndLayerDef.WND_LAYER_POPUP02;
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_RECORD_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_RECORD_RECEIVE, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_RECORD_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_RECORD_RECEIVE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RANK_RECORD_SHOW:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                            return;

                        SetVisible(true);

                        OnRankRecordWndShow();
                    }
                    break;
                case WndMsgID.WND_MSG_RANK_RECORD_RECEIVE:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            m_wndView.OnReceiveRankRecord(cmdData.ptrParam, cmdData.nPtrLen);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // 请求赛季历史数据
        private void OnRankRecordWndShow()
        {
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_SEASON_RECORD);
        }
        
    }

    public class RankRecordWndView : UIBaseWndView
    {
        public Text TitleDesc;

        public Text SeasonDesc;
        public Text RankDesc;
        public Text WinCountDesc;
        public Text WinProDesc;
        public Text MostUseHeroDesc;

        public UListRankRecord RankRecordList;

        private const int SinglePageItemCount = 6;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TitleDesc.text = "历史记录";

            SeasonDesc.text = "赛季";
            RankDesc.text = "段位";
            WinCountDesc.text = "胜场";
            WinProDesc.text = "胜率";
            MostUseHeroDesc.text = "常用英雄";

            RankRecordList.UserItemCount = SinglePageItemCount;

            return true;
        }

        public void OnReceiveRankRecord(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nCount = helper.get<int>();

            RankRecordList.DataSource.Clear();
            ObservableList<UListItemRankRecord> dataSource = new ObservableList<UListItemRankRecord>();
            for (int i = 0; i < nCount; i++)
            {
                cmd_entity_rank_season_record_node sNode = helper.get<cmd_entity_rank_season_record_node>();
                UListItemRankRecord item = new UListItemRankRecord();
                item.node = sNode;
                dataSource.Add(item);
            }
            RankRecordList.DataSource = dataSource;
        }

        public void OnClickCloseBtn()
        {
            SetVisible(false);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using USpeedUI.UWidgets;

namespace USpeedUI.HighLadder
{
    public class UHighLadderWndView : UIBaseWndView
    {
        public UListLadder ListLadder;
        private int nCurSelectIndex;
        private UListItemLadder CurSelectItem;
        public Text ranking;
        public Text roleName;
        public Text rankGrade;
        public Text rankScore;
        public Text rankGradeDes;
        public Text refreshRankDes;
        private UPopupAction PopupAction;

        public override void Start()
        {
            base.Start();
        }

        public override bool Init(IUIWnd wnd)
        {
            ranking.text = ULocalizationService.Instance.Get("UIView", "LadderList", "ranking");
            roleName.text = ULocalizationService.Instance.Get("UIView", "LadderList", "roleName");
            rankGrade.text = ULocalizationService.Instance.Get("UIView", "LadderList", "rankGrade");
            rankScore.text = ULocalizationService.Instance.Get("UIView", "LadderList", "rankScore");
            rankGradeDes.text = ULocalizationService.Instance.Get("UIView", "LadderList", "rankGradeDes");
            refreshRankDes.text = ULocalizationService.Instance.Get("UIView", "LadderList", "refreshRankDes");

            return base.Init(wnd);
        }

        public void ShowHighLadderInfo(HighLadderArray ladderArray)
        {
            ListLadder.OnSelect.RemoveListener(onSelectLadder);
            ListLadder.onRightClick.RemoveListener(onSelectLadder);
            ListLadder.DataSource.Clear();
            UIWidgets.ObservableList<UListItemLadder> dataSource = new UIWidgets.ObservableList<UListItemLadder>();

            for (int i = 0; i < ladderArray.nLadderCount; i++)
            {
                UListItemLadder item = new UListItemLadder();
                item.OrderID = ladderArray.allLadderInfo[i].OrderID;
                item.nPDBID = ladderArray.allLadderInfo[i].nPDBID;
                item.PlayerName = ladderArray.allLadderInfo[i].PlayerName;
                item.RankGrade = ladderArray.allLadderInfo[i].RankGrade;
                item.RankScore = ladderArray.allLadderInfo[i].RankScore;
                item.RankGradeDes = ladderArray.allLadderInfo[i].RankGradeDes;
                PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.nPDBID));
                item.PopupAction = PopupAction;

                dataSource.Add(item);
            }

            ListLadder.DataSource = dataSource;
            ListLadder.OnSelect.AddListener(onSelectLadder);
            ListLadder.onRightClick.AddListener(onSelectLadder);
        }

        public void onSelectLadder(int index, UIWidgets.ListViewItem item)
        {
            // 选择当前数据
            nCurSelectIndex = index;
            CurSelectItem = ListLadder.DataSource[index];
        }

        //刷新排行榜，目前处理方式为再次向逻辑层请求一次数据
        public void OnRefreshDown()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HIGH_LADDER);
        }

        //返回按钮
        public void OnBackDown()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, null);
        }
    }
}

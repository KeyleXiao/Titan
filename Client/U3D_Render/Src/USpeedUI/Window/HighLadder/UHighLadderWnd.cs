using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using Stage_ManagerDef;
using USpeedUI;
using UnityEngine;

namespace USpeedUI.HighLadder
{
    public class UHighLadderWnd : UIBaseWnd<UHighLadderWndView>
    {
        private UHighLadderWndView view 
        { 
            get 
            {
                return m_wndView as UHighLadderWndView; 
            } 
        }

        //获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_HIGHLADDER;
        }

        //获取资源路径
        public override String GetPrefabPath()
        {
            return "StaticState/HighLadder/HighLadder";
        }

        //获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        //初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            //订阅打开排行榜事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_OPEN, this);
            //订阅获得排行榜数据事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_GETINFO, this);
            //订阅关闭排行榜事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, this);
            //订阅进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            return true;
        }

        //销毁
        public override void Destroy()
        {
            base.Destroy();

            //退订打开排行榜事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_OPEN, this);
            //退订获得排行榜数据事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_GETINFO, this);
            //退订关闭排行榜事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HIGHLADDER_CLOSE, this);
            //退订进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        //接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_HIGHLADDER_OPEN:
                    {
                        //隐藏其他同级窗口
                        UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
                        SetVisible(true);
                        //向逻辑层请求打开排行榜
                        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_OPEN_ORDER_HIGH_LADDER);
                    }
                    break;
                case WndMsgID.WND_MSG_HIGHLADDER_CLOSE:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_HIGHLADDER_GETINFO:
                    {
                        //显示逻辑层下发的排行榜数据信息
                        UIMsgCmdData uiHighLadder = (UIMsgCmdData)msgData;
                        OnRecvHighLadder(uiHighLadder.ptrParam, uiHighLadder.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnRecvHighLadder(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_Entity_Rank_Info_head nCountdata = helper.get<cmd_Entity_Rank_Info_head>();
            HighLadderArray ladderArray = new HighLadderArray(nCountdata.nCount);
            ladderArray.nLadderCount = nCountdata.nCount;
            for (int i = 0; i < nCountdata.nCount; i++)
            {
                cmd_Entity_HighLadder_Rank_Info data = helper.get<cmd_Entity_HighLadder_Rank_Info>();
                HighLadderInfo ladderInfo = new HighLadderInfo();
                ladderInfo.OrderID = data.nOrderID;
                ladderInfo.nPDBID = (int)data.idActor;
                ladderInfo.PlayerName = data.szPlayerName;
                ladderInfo.RankGrade = data.nRankGrade;
                ladderInfo.RankScore = data.nRankScore;
                ladderInfo.RankGradeDes = data.szGradeName;
                ladderArray.allLadderInfo[i] = ladderInfo;
            }
            if (view != null)
            {
                view.ShowHighLadderInfo(ladderArray);
            }
        }
    }
}

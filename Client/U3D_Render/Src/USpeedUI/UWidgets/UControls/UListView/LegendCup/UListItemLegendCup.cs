using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemLegendCup
    {
        public int nIndex;
        public cmd_legendcup_recv_cuplist_node data;
    }

    public class UListItemLegendCupRegist
    {
        public int nIndex;
        public cmd_legendcup_recv_regist_kinlistnode data;
    }

    public class UListItemLegendCupBlacklist
    {
        public int nIndex;
        public int nKinID;
        public string strKinName;
    }

    public class UListItemLegendCupGroup
    {
        public long nLegendCupID;
        public string strGroupIndex;
        public List<legendcup_kininfo> kinInfoList;
        public List<cmd_legendcup_recv_competition_node> competitionNodeList;
    }

    public class UListItemLegendCupBigKnockout
    {
        public long nLegendCupID;
        public string strGroupIndex;
        public List<legendcup_kinvsinfo> kinVsInfoList;
        public List<cmd_legendcup_recv_competition_node> competitionNodeList;
    }
}

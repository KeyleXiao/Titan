using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using ASpeedGame.Data.Scheme;
using USpeedUI.Blood;

namespace DataCenter
{
    public class RankDataManager
    {
        private bool m_bRankChanged;
        public int m_nDownOrUp;  // 0：不变 1： 升 -1：降
        public int m_nCurRankGrade;
        public int m_nCurRankScore;
        public int m_nRankIconID;
        public string m_chRankName;

        public void Init()
        {
            m_bRankChanged = false;
            m_nDownOrUp = 0;
            m_nCurRankGrade = 0;
            m_nCurRankScore = 0;
            m_nRankIconID = 0;
            m_chRankName = "";
        }

        public int getRankID()
        {
            return m_nRankIconID;
        }

        public string getRankName()
        {
            return m_chRankName;
        }

        public bool getRankChanged()
        {
            return m_bRankChanged;
        }

        public int getDownOrUp()
        {
            return m_nDownOrUp;
        }

        public void resetRankChanged()
        {
            m_bRankChanged = false;
        }

        public void Clear()
        {
            m_bRankChanged = false;
            m_nDownOrUp = 0;
            m_nCurRankGrade = 0;
            m_nCurRankScore = 0;
            m_nRankIconID = 0;
            m_chRankName = "";
        }

        public void RankUpOrDown(IntPtr ptrParam, int nPtrLen)
        {
            m_bRankChanged = true;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_entity_rank_down_or_upgrade cmd = helper.get<cmd_entity_rank_down_or_upgrade>();
            m_nDownOrUp = cmd.nDownOrUp;
            m_nCurRankGrade = cmd.nGrade;
            m_nCurRankScore = cmd.nRankScore;
            m_nRankIconID = cmd.nRankIconID;
            m_chRankName = cmd.chRankName;
        }
    }
}

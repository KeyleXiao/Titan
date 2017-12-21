using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using GameLogic;

namespace DataCenter
{
    public class TeamRightInfo
    {
        public int nSlotId;
        public bool bFirstLearnSkill = true;
    }

    public class TeamRightWndDataManager
    {
        //实体UID与对应的槽位等信息
        private Hashtable m_MyTeamUIDTable = new Hashtable();
        public  Hashtable MyTeamUIDTable {get {return m_MyTeamUIDTable;}}
        private Hashtable m_OtherTeamUIDTable = new Hashtable();
        public Hashtable OtherTeamUIDTable { get { return m_OtherTeamUIDTable; } }
        //实体槽位与对应的UID
        private Hashtable m_MyTeamEntityIdTable = new Hashtable();
        public Hashtable MyTeamEntityIdTable { get { return m_MyTeamEntityIdTable; } }

        const int nFlagLen = 5;
        private bool[] bIsJoinFollow;
        public bool[] BIsJoinFollow { get { return bIsJoinFollow; }}


        public void Init()
        {
            m_MyTeamUIDTable.Clear();
            m_OtherTeamUIDTable.Clear();
            m_MyTeamEntityIdTable.Clear();

            bIsJoinFollow = new bool[nFlagLen];
            for (int i = 0; i < nFlagLen; i++)
            {
                bIsJoinFollow[i] = false;
            }
        }

        public void Clear()
        {
            m_MyTeamUIDTable.Clear();
            m_OtherTeamUIDTable.Clear();
            m_MyTeamEntityIdTable.Clear();

            for (int i = 0; i < nFlagLen; i++)
            {
                bIsJoinFollow[i] = false;
            }
        }

        public void onLeaveBattleState()
        {
            Clear();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using USpeedUI.TooltipAndDialog;

namespace DataCenter
{
    public class ViewPersonStateDataManager
    {
        // 主角是否在房间里
        private bool m_bDelayJoinMatchRoom;
        public bool bDelayJoinMatchRoom
        {
            get { return m_bDelayJoinMatchRoom; }
            set { m_bDelayJoinMatchRoom = value; }
        }

        private bool m_bIsMatching;

        public bool bIsMatching
        {
            get { return m_bIsMatching; }
            set { m_bIsMatching = value; }
        }

        public void Init()
        {
            m_bDelayJoinMatchRoom = false;
            m_bIsMatching = false;
        }
    }
}

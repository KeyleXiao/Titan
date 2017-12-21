using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DataCenter
{
    public class EmojiDataManager
    {
        private List<int> m_listEmojiInfo;
        public List<int> ListEmojiInfo { get { return m_listEmojiInfo; } }

        public void Init()
        {
            m_listEmojiInfo = new List<int>();
        }

        public void Clear()
        {
            m_listEmojiInfo.Clear();
        }

        public void OnUpdateEmojiInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nCount = helper.get<int>();

            m_listEmojiInfo.Clear();
            for (int i = 0; i < nCount; i++)
            {
                byte byEmojiType = helper.get<byte>();
                m_listEmojiInfo.Add((int)byEmojiType);
            }

            List<int> groupList = new List<int>();
            groupList.AddRange(m_listEmojiInfo);

            USpeedUI.UChatEmoticonGroupState unlockEmoticonGroup = new USpeedUI.UChatEmoticonGroupState(groupList);
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_CHATBOX_UPDATELOCKSTATE, unlockEmoticonGroup);
        }

        public bool CheckEmojiGroupLocked(int groupID)
        {
            if (groupID < 0)
                return true;

            if (m_listEmojiInfo == null)
                return true;

            bool bOwn = m_listEmojiInfo.Contains(groupID);

            return !bOwn;
        }
    }
}

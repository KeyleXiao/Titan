using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;

namespace DataCenter
{

    public class DistrictListDataManager
    {
        public Dictionary<int, List<gamelogic_district_info>> DistrictInfoMap { get { return m_districtInfoMap; } }
        private Dictionary<int, List<gamelogic_district_info>> m_districtInfoMap;

        public List<gamelogic_district_info> LastEnterDistrictInfoList { get { return m_LastEnterDistrictInfoList; } }
        private List<gamelogic_district_info> m_LastEnterDistrictInfoList;
        private List<gamelogic_lastEnter_district_info> m_LastEnterDistrictList;

        public gamelogic_district_info DistrictInfo { get { return m_districtInfo; } }
        private gamelogic_district_info m_districtInfo;
        public void Init()
        {
            m_districtInfoMap = new Dictionary<int, List<gamelogic_district_info>>();
            m_LastEnterDistrictList = new List<gamelogic_lastEnter_district_info>();
            m_districtInfo = new gamelogic_district_info();
            m_LastEnterDistrictInfoList = new List<gamelogic_district_info>();
        }

        public void Clear()
        {
            foreach(int key in m_districtInfoMap.Keys)
            {
                m_districtInfoMap[key].Clear();
            }
            m_districtInfoMap.Clear();
            m_LastEnterDistrictList.Clear();
            m_LastEnterDistrictInfoList.Clear();
        }

        public void RecvDistrictListData(IntPtr  ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            gameview_district_info_list_head cmdCount = helper.get<gameview_district_info_list_head>();

            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                gamelogic_district_info info = helper.get<gamelogic_district_info>();
                List<gamelogic_district_info> list;
                if(!m_districtInfoMap.TryGetValue(info.nGroup, out list))
                {
                    list = new List<gamelogic_district_info>();
                    m_districtInfoMap.Add(info.nGroup, list);
                }
                m_districtInfoMap[info.nGroup].Add(info);
            }

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_CREATEHERO_RECV_DISTRICTLIST, null);
        }

        public void UpdateLastEnterDistrictList(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            gamelogic_lastEnter_district_info_head cmdCount = helper.get<gamelogic_lastEnter_district_info_head>();

            m_LastEnterDistrictList.Clear();
            m_LastEnterDistrictInfoList.Clear();
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                gamelogic_lastEnter_district_info info = helper.get<gamelogic_lastEnter_district_info>();
                m_LastEnterDistrictList.Add(info);
                gamelogic_district_info districtInfo = GetDistrictInfo(info.nGroup, info.nDistrict, info.nServer);
                if (districtInfo.nWorldID > 0)
                    m_LastEnterDistrictInfoList.Add(districtInfo);
            }

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_CREATEHERO_RECV_LASTENTER_DISTRICTLIST, null);
        }

        public void UpdateDistrictInfo(gamelogic_district_info server)
        {
            m_districtInfo = server;

            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_CREATEHERO_UPDATE_SERVERINFO, null);
        }

        public gamelogic_district_info GetDistrictInfo(int nGroup, int nDistrict, int nServer)
        {
            gamelogic_district_info info = new gamelogic_district_info();

            if (m_districtInfoMap.ContainsKey(nGroup))
            {
                int index = m_districtInfoMap[nGroup].FindIndex( x => x.nGroup == nGroup && x.nDistrict == nDistrict && x.nServer == nServer);
                if (index >= 0)
                    info = m_districtInfoMap[nGroup][index];
            }
            return info;
        }

        public List<gamelogic_district_info> SearchContent(string content)
        {
            List<gamelogic_district_info> infoList = new List<gamelogic_district_info>();
            if (m_districtInfoMap.ContainsKey(m_districtInfo.nGroup))
            {
                IEnumerable<gamelogic_district_info> ListQuery;

                ListQuery = from item in m_districtInfoMap[m_districtInfo.nGroup]
                                where item.szName.Contains(content)
                                select item;

                foreach (gamelogic_district_info item in ListQuery)
                {
                    infoList.Add(item);
                }
            }
            return infoList;
        }
    }
}

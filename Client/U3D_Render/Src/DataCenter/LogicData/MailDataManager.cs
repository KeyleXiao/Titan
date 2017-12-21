using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using UnityEngine;
using Mail_ManagerDef;
using USpeedUI.TooltipAndDialog;

namespace DataCenter
{
    public class MailDataManager
    {
        //MailWnd
        private int m_MailCount;
        public int MailCount { get { return m_MailCount;} set { m_MailCount = value; } }
        private List<GameLogic.gameview_mail_baseinfo> m_MailBaseInfoList;
        public List<GameLogic.gameview_mail_baseinfo> MailBaseInfoList { get { return m_MailBaseInfoList; } }


        // 邮件详细数据<mailID, gameview_mail_rsp_detaildata>
        private Dictionary<long, GameLogic.gameview_mail_rsp_detaildata> m_MailDetailInfoList;
        public Dictionary<long, GameLogic.gameview_mail_rsp_detaildata> MailDetailInfoList { get { return m_MailDetailInfoList; } }

        // 收取附件邮件列表
        private List<long> m_PlusMailList;
        public List<long> PlusMailList { get { return m_PlusMailList; } }

        // 快速收件奖励统计
        private int m_SumPlusMoney;
        public int SumPlusMoney { get { return m_SumPlusMoney; } set { m_SumPlusMoney = value; } }
        private int m_SumPlusExp;
        public int SumPlusExp { get { return m_SumPlusExp; } set { m_SumPlusExp = value; } }
        private Dictionary<int, GameLogic.gameview_mail_goodsinfo> m_FastRecvGoodsList;
        public Dictionary<int, GameLogic.gameview_mail_goodsinfo> FastRecvGoodsList { get { return m_FastRecvGoodsList; } }


        public void Init()
        {
            m_MailCount = 0;
            m_MailBaseInfoList = new List<GameLogic.gameview_mail_baseinfo>();
            m_MailDetailInfoList = new Dictionary<long, GameLogic.gameview_mail_rsp_detaildata>();
            m_FastRecvGoodsList = new Dictionary<int, GameLogic.gameview_mail_goodsinfo>();
            m_PlusMailList = new List<long>();

            ClearFastRecvData();
        }

        private void sortMailBaseInfoList()
        {
            m_MailBaseInfoList.Sort(
                (x, y) =>
                {
                    return (x.nRecvTime > y.nRecvTime) ? -1 : 1;
                }
                );
        }

        public void ClearFastRecvData()
        {
            m_SumPlusMoney = 0;
            m_SumPlusExp = 0;
            m_FastRecvGoodsList.Clear();
        }

        public bool IsHaveFastRecvPlusData()
        {
            if (m_SumPlusMoney > 0 || m_SumPlusExp > 0 || m_FastRecvGoodsList.Count > 0)
                return true;

            return false;
        }

        public int GetMailType(long nMailID)
        {
            for (int i = 0; i < m_MailBaseInfoList.Count; ++i)
            {
                if (nMailID == m_MailBaseInfoList[i].nMailID)
                {
                    return m_MailBaseInfoList[i].nType;
                }
            }

            return -1;
        }

        public void AddFastRecvData(long nMailID)
        {
            if (m_MailDetailInfoList.ContainsKey(nMailID))
            {
                GameLogic.gameview_mail_rsp_detaildata detailData = m_MailDetailInfoList[nMailID];
                if (detailData.nPlusMoney > 0)
                    m_SumPlusMoney += detailData.nPlusMoney;

                if (detailData.nPlusExp > 0)
                    m_SumPlusExp += detailData.nPlusExp;

                for (int i = 0; i < detailData.nPlusGoodsNum; ++i)
                {
                    GameLogic.gameview_mail_goodsinfo goodsInfo = detailData.sGoodsList[i];
                    if (goodsInfo.nGoodsID > 0)
                    {
                        if (m_FastRecvGoodsList.ContainsKey(goodsInfo.nGoodsID))
                        {
                            GameLogic.gameview_mail_goodsinfo tmpGoodsInfo = m_FastRecvGoodsList[goodsInfo.nGoodsID];
                            tmpGoodsInfo.nCount += goodsInfo.nCount;
                            m_FastRecvGoodsList[goodsInfo.nGoodsID] = tmpGoodsInfo;
                        }
                        else
                        {
                            GameLogic.gameview_mail_goodsinfo tmpGoodsInfo;
                            tmpGoodsInfo.nGoodsID = goodsInfo.nGoodsID;
                            tmpGoodsInfo.nCount = goodsInfo.nCount;
                            m_FastRecvGoodsList.Add(tmpGoodsInfo.nGoodsID, tmpGoodsInfo);
                        }
                    }
                }
            }
        }

        public void OnRecvMailBaseInfoList(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.gameview_mail_req_basedatalist infoListHead = helper.get<GameLogic.gameview_mail_req_basedatalist>();

            m_MailBaseInfoList.Clear();
            m_MailDetailInfoList.Clear();
            m_PlusMailList.Clear();
            m_MailCount = 0;
            for (int i = 0; i < infoListHead.nCount; ++i)
            {
                GameLogic.gameview_mail_baseinfo cmdNodeData = helper.get<GameLogic.gameview_mail_baseinfo>();
                m_MailBaseInfoList.Add(cmdNodeData);

                if (cmdNodeData.nIsPlusData == 1 && (cmdNodeData.nType == (int)EMMailType.emMailType_Player || cmdNodeData.nType == (int)EMMailType.emMailType_System))
                    m_PlusMailList.Add(cmdNodeData.nMailID);

                if (cmdNodeData.nIsRead == 0)
                    m_MailCount++;
            }

            sortMailBaseInfoList();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_REQUESTMAILBASELIST, null);
        }

        public void OnRecvMailDetailInfo(int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            GameLogic.gameview_mail_rsp_detaildata cmdData = helper.get<GameLogic.gameview_mail_rsp_detaildata>();

            if (m_MailDetailInfoList.ContainsKey(cmdData.nMailID))
            {
                m_MailDetailInfoList[cmdData.nMailID] = cmdData;
            }
            else
            {
                m_MailDetailInfoList.Add(cmdData.nMailID, cmdData);
            }

            for (int i = 0; i < m_MailBaseInfoList.Count; ++i)
            {
                if (cmdData.nMailID == m_MailBaseInfoList[i].nMailID)
                {
                    GameLogic.gameview_mail_baseinfo tmpBaseInfo = m_MailBaseInfoList[i];
                    if (tmpBaseInfo.nIsRead == 0)
                    {
                        m_MailCount--;
                        tmpBaseInfo.nIsRead = 1;
                    }
                    tmpBaseInfo.nReadFlag = 1;
                    m_MailBaseInfoList[i] = tmpBaseInfo;
                    break;
                }
            }

            m_MailCount = m_MailCount < 0 ? 0 : m_MailCount;

            UIMsgCmdData uiMailDetailInfo = new UIMsgCmdData((int)WndMsgID.WND_MSG_MAIL_DETAILINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAIL_DETAILINFO, uiMailDetailInfo);
        }
    }
}

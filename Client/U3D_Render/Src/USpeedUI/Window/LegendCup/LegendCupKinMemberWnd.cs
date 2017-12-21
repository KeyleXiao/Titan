using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using GameLogic;

namespace USpeedUI.LegendCup
{
    public class LegendCupKinMemberWnd : UIPopupWnd<LegendCupKinMemberWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_KINMEMBER;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupKinMemberView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_REGIST_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_REGIST_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnShowKinMemberWnd(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_KINMEMBER_REGIST_SHOW:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            LegendCupRegistKinData cmdData = (LegendCupRegistKinData)data;
                            m_wndView.OnOpenRegistKinMemberWnd(cmdData.lLegendCupID, cmdData.nKinID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_REGIST_MEMBERLIST:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnRecvRegistKinMemberList(cmdData.nParam);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class LegendCupKinMemberWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text KinMemberDes;
        public Text KinMemberLevelDes;
        public Text KinMemberRankDes;
        public RectTransform KinMemberListFrame;
        public GameObject KinMemberTemplate;
        private LegendCupRegistKinMemberItem[] m_kinMemberItems;

        public override bool Init(IUIWnd wnd)
        {
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupKinMember", "Title");
            KinMemberDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMember");
            KinMemberLevelDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMemberLevel");
            KinMemberRankDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupRegist", "KinMemberRank");
            KinMemberTemplate.SetActive(false);

            return base.Init(wnd);
        }

        public void OnShowKinMemberWnd(int nKinID)
        {
            // 清空KinMemberFrame
            int nCount = KinMemberListFrame.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = KinMemberListFrame.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            // 填充数据
            List<cmd_legendcup_competition_kinmembernode> memberList = LogicDataCenter.legendCupDataManager.GetKinMemberInfo(nKinID);
            if (memberList == null)
                return;

            KinMemberTemplate.SetActive(true);
            cmd_legendcup_recv_regist_memberlistnode tempNode = new cmd_legendcup_recv_regist_memberlistnode();
            m_kinMemberItems = new LegendCupRegistKinMemberItem[memberList.Count];
            for (int i = 0; i < memberList.Count; ++i)
            {
                GameObject memberItemObj = ResNode.InstantiateRes(KinMemberTemplate);
                if (!memberItemObj)
                    return;

                m_kinMemberItems[i] = memberItemObj.GetComponent<LegendCupRegistKinMemberItem>();
                if (!m_kinMemberItems[i])
                    return;

                tempNode.uDBID = memberList[i].nUserID;
                tempNode.nPDBID = memberList[i].nPDBID;
                tempNode.nActorSex = memberList[i].nActorSex;
                tempNode.nActorLevel = memberList[i].nActorLevel;
                tempNode.szActorName = memberList[i].szActorName;
                tempNode.nType = memberList[i].nType;
                tempNode.nRankGrade = memberList[i].nRankGrade;
                tempNode.nRankScore = memberList[i].nRankScore;
                tempNode.nRankIconId = memberList[i].nRankIconId;
                tempNode.szGradeName = memberList[i].szGradeName;
                m_kinMemberItems[i].SetData(tempNode);

                memberItemObj.name = "MemberItem" + (i + 1);
                RectTransform rectTrans = memberItemObj.transform as RectTransform;
                rectTrans.SetParent(KinMemberListFrame, false);
            }
            KinMemberTemplate.SetActive(false);
        }

        public void OnOpenRegistKinMemberWnd(long lLegendCupID, int nKinID)
        {
            cmd_legendcup_req_regist_memberlist reqData = new cmd_legendcup_req_regist_memberlist();
            reqData.nLegendCupID = lLegendCupID;
            reqData.nKinID = nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_regist_memberlist>(GameLogicDef.GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST, ref reqData);
        }

        public void OnRecvRegistKinMemberList(int nKinID)
        {
            SingleRegistKinMemberInfo memberlist = LogicDataCenter.legendCupDataManager.GetSingleRegistKinMemberInfo(nKinID);
            if (memberlist == null)
                return;

            // 清空KinMemberFrame
            int nCount = KinMemberListFrame.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = KinMemberListFrame.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            KinMemberTemplate.SetActive(true);
            m_kinMemberItems = new LegendCupRegistKinMemberItem[memberlist.headInfo.nMemberCount];
            for (int i = 0; i < memberlist.headInfo.nMemberCount; ++i)
            {
                GameObject memberItemObj = ResNode.InstantiateRes(KinMemberTemplate);
                if (!memberItemObj)
                    return;

                m_kinMemberItems[i] = memberItemObj.GetComponent<LegendCupRegistKinMemberItem>();
                if (!m_kinMemberItems[i])
                    return;

                m_kinMemberItems[i].SetData(memberlist.nodeInfo[i]);

                memberItemObj.name = "MemberItem" + (i + 1);
                RectTransform rectTrans = memberItemObj.transform as RectTransform;
                rectTrans.SetParent(KinMemberListFrame, false);
            }
            KinMemberTemplate.SetActive(false);

        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}

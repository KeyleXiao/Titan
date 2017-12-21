using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using UnityEngine.EventSystems;
using Clan_ManagerDef;

namespace USpeedUI.Social
{
    public class ClanListWnd : UIBaseSubWnd<ClanListWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)SocialSubWndType.WND_CLANLIST;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanList";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                RequestClanListInfo(1, "");
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_REQUESTCLANLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvClanList();
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_CREATESUCCEED:
                    {
                        if (m_wndView != null)
                            m_wndView.OnCreateClanSucceed();
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_APPLYRET:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClanApplyRet(msgData);
                    }
                    break;
                default:
                    break;
            }
        }

        // 请求指定页数联盟列表内容
        public void RequestClanListInfo(int nPageNum, String szClanName)
        {
            cmd_clan_request_clanlist cmdData = new cmd_clan_request_clanlist();
            cmdData.nPageNum = nPageNum;
            cmdData.szClanName = szClanName;
            ViewEventHelper.Instance.SendCommand<cmd_clan_request_clanlist>(GameLogicDef.GVIEWCMD_CLAN_REQUESTCLANLIST, ref cmdData);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)SocialTimerID.TimerID_RequestClanList:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRequestClanListInfo();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class ClanListItem : MonoBehaviour, IPointerDownHandler
    {
        public Image pictureBadges;
        public Text clanName;
        public Text leader;
        public Text kinCount;
        public Text memberCount;
        public Text manifesto;
        public Text clanTotalFight;
        public Text application;

        private int m_clanID;
        public int ClanID { get { return m_clanID; } }

        private ClanListWndView wndView;
        public void SetWndView(ClanListWndView view)
        {
            wndView = view; 
        }

        public void Clear()
        {
            gameObject.SetActive(false);
        }

        public void SetData(cmd_clan_clanlist_node data)
        {
            m_clanID = data.nClanID;

            pictureBadges.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ClanQiZhi, WndID.WND_ID_SOCIALCONTACT, data.nFlag + 1);
            clanName.text = data.szName;
            leader.text = data.szShaikhName;
            kinCount.text = data.nKinCount.ToString();
            memberCount.text = data.nMemberCount.ToString();
            manifesto.text = data.szManifesto;
            clanTotalFight.text = data.nClanTotalFight.ToString();
            if (data.nState == (int)EMClanListClanStatus.CLAN_LIST_STATUS_REQUESTING)
                application.text = String.Format("<color=#18ff00>{0}</color>", ULocalizationService.Instance.Get("UIView", "ClanList", "ClanRequestingDesc"));
            else if (data.nState == (int)EMClanListClanStatus.CLAN_LIST_STATUS_FULL)
                application.text = String.Format("<color=#f6ff00>{0}</color>", ULocalizationService.Instance.Get("UIView", "ClanList", "ClanFullDesc"));
            else if (data.nState == (int)EMClanListClanStatus.CLAN_LIST_STATUS_DISMISSING)
                application.text = String.Format("<color=#a8232e>{0}</color>", ULocalizationService.Instance.Get("UIView", "ClanList", "ClanDismissingDesc"));
            else
                application.text = "";

            gameObject.SetActive(true);
        }

        public void ChangeClanApplicationState(int nState)
        {
            if (nState == (int)EMClanListClanStatus.CLAN_LIST_STATUS_REQUESTING)
                application.text = String.Format("<color=#18ff00>{0}</color>", ULocalizationService.Instance.Get("UIView", "ClanList", "ClanRequestingDesc"));
            else if (nState == (int)EMClanListClanStatus.CLAN_LIST_STATUS_FULL)
                application.text = String.Format("<color=#f6ff00>{0}</color>", ULocalizationService.Instance.Get("UIView", "ClanList", "ClanFullDesc"));
            else
                application.text = "";
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (wndView != null)
                wndView.OnSelectClan(m_clanID, this);
        }
    }

    public class ClanListWndView : UIBaseSubWndView
    {
        public Text titleText;
        public InputField searchInput;
        public Text searchInputPlaceHolder;
        public ClanListItem[] listClanItem;
        public Image clanItemSelectImage;
        public Text pageText;
        public Button prevPageBtn;
        public Button nextPageBtn;
        public Button createClanBtn;
        public Text createClanText;
        public Button requestClanBtn;
        public Text requestClanText;
        public Button returnClanBtn;
        public Text returnClanText;

        public Text clanIconText;
        public Text clanNameText;
        public Text clanLeaderText;
        public Text clanKinCountText;
        public Text clanMemberNumText;
        public Text clanDescText;
        public Text clanRequestStatusText;

        private int m_curSelectClanID;

        private bool m_bIsCreateClanSuccess;
        private float m_nextUpdateTime;
        private bool m_bAutoJumpClanDetail;
        public Transform SerchTI;
        private string m_LastSerch;
        private bool m_bSerchChange;
        // 当前页数
        private int m_curPageNum;

        // 初始化
        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            searchInputPlaceHolder.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanSearchDesc");
            createClanText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanCreateBtn");
            requestClanText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanRequestBtn");
            returnClanText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanReturnBtn");
            pageText.text = "0/0";

            clanIconText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanIconDesc");
            clanNameText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanNameDesc");
            clanLeaderText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanLeaderDesc");
            clanKinCountText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanKinCountDesc");
            clanMemberNumText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanMemberNumDesc");
            clanDescText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanDescDesc");
            clanRequestStatusText.text = ULocalizationService.Instance.Get("UIView", "ClanList", "ClanRequestDesc");

            m_curSelectClanID = -1;
            for (int i = 0; i < listClanItem.Length; ++i)
                listClanItem[i].SetWndView(this);
            prevPageBtn.onClick.AddListener(OnPrevPageBtn);
            nextPageBtn.onClick.AddListener(OnNextPageBtn);

            searchInput = SerchTI.GetComponent<InputField>();

            m_LastSerch = "";
            m_bSerchChange = false;
            m_curPageNum = 1;

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0)
                {
                    m_bAutoJumpClanDetail = false;

                    createClanBtn.gameObject.SetActive(false);
                    requestClanBtn.gameObject.SetActive(false);
                    returnClanBtn.gameObject.SetActive(true);
                }
                else
                {
                    m_bAutoJumpClanDetail = true;

                    createClanBtn.gameObject.SetActive(true);
                    requestClanBtn.gameObject.SetActive(true);
                    returnClanBtn.gameObject.SetActive(false);
                }
                UpdateData();
            }
        }

        public void Update()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            if (m_bIsCreateClanSuccess)
            {
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, null);
                    m_bIsCreateClanSuccess = false;
                }
            }

            if (m_bAutoJumpClanDetail && Time.unscaledTime > m_nextUpdateTime)
            {
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) > 0)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, null);
                }

                // 每秒更新一次
                m_nextUpdateTime = Time.unscaledTime + 1f;
            }
        }

        public void OnCreateClanSucceed()
        {
            m_bIsCreateClanSuccess = true;
        }

        public void UpdateData()
        {
            OnRecvClanList();
        }

        public void OnRecvClanList()
        {
            OnSelectClan(-1, null);
            cmd_clan_list_head listInfo = LogicDataCenter.clanDataManager.ClanListHeadData;
            List<cmd_clan_clanlist_node> clanList = LogicDataCenter.clanDataManager.ClanListData;
            for (int i = 0; i < listClanItem.Length; ++i)
            {
                if (i < clanList.Count)
                {
                    listClanItem[i].SetData(clanList[i]);
                }
                else
                {
                    listClanItem[i].Clear();
                }
            }

            pageText.text = listInfo.nPageNum + "/" + listInfo.nTotalNum;
            if (listInfo.nPageNum <= 1)
                prevPageBtn.interactable = false;
            else
                prevPageBtn.interactable = true;

            if (listInfo.nPageNum >= listInfo.nTotalNum)
                nextPageBtn.interactable = false;
            else
                nextPageBtn.interactable = true;

            m_curPageNum = listInfo.nPageNum;
        }

        public void OnClanApplyRet(UIMsgData msgData)
        {
            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
            GameLogic.cmd_clan_apply_ret applyRet = helper.get<GameLogic.cmd_clan_apply_ret>();

            for (int i = 0; i < listClanItem.Length; ++i)
            {
                if (applyRet.nClanID > 0 && listClanItem[i].ClanID == applyRet.nClanID)
                {
                    listClanItem[i].ChangeClanApplicationState(applyRet.nApplyState);
                }
                else if (applyRet.nOldClanID > 0 && listClanItem[i].ClanID == applyRet.nOldClanID)
                {
                    listClanItem[i].ChangeClanApplicationState((int)EMClanListClanStatus.CLAN_LIST_STATUS_NORMAL);
                }
            }
        }

        public void OnCreateClanBtnClick()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_CLAN_MSGBOXCREATECLAN, true);
        }

        public void OnClanApplyBtnClick()
        {
            // 申请加入帮会
            if (m_curSelectClanID <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_NOT_SLELECT);
                return;
            }

            GameLogic.cmd_clan_apply cmdData = new GameLogic.cmd_clan_apply();
            cmdData.nClanID = m_curSelectClanID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_apply>(GameLogicDef.GVIEWCMD_CLAN_APPLY, ref cmdData);
        }

        public void OnReturnClanBtnClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_REFRESH, null);
        }

        public void OnSelectClan(int nClanID, ClanListItem selItem)
        {
            m_curSelectClanID = nClanID;

            if (selItem != null)
            {
                clanItemSelectImage.rectTransform.SetPosition(selItem.gameObject.transform.position);
                clanItemSelectImage.enabled = true;
            }
            else
            {
                clanItemSelectImage.enabled = false;
            }
        }

        void OnPrevPageBtn()
        {
            cmd_clan_list_head listInfo = LogicDataCenter.clanDataManager.ClanListHeadData;
            if (m_curPageNum <= 1)
                return;

            ClanListWnd wnd = m_wnd as ClanListWnd;
            wnd.RequestClanListInfo(m_curPageNum - 1, m_LastSerch);
        }

        void OnNextPageBtn()
        {
            cmd_clan_list_head listInfo = LogicDataCenter.clanDataManager.ClanListHeadData;
            if (m_curPageNum >= listInfo.nTotalNum)
                return;

            ClanListWnd wnd = m_wnd as ClanListWnd;
            wnd.RequestClanListInfo(m_curPageNum + 1, m_LastSerch);
        }

        public void OnInputFieldValueChange()
        {
            OnSelectClan(-1, null);

            // 查询数据
            if (searchInput == null)
            {
                return;
            }

            m_LastSerch = searchInput.text;
            m_bSerchChange = true;
        }

        public void OnRequestClanListInfo()
        {
            if (!m_bSerchChange)
            {
                return;
            }

            m_bSerchChange = false;

            ClanListWnd wnd = m_wnd as ClanListWnd;

            // 没搜索,请求第一页信息
            if (m_LastSerch == string.Empty)
            {
                wnd.RequestClanListInfo(1, "");
            }
            else
            {
                wnd.RequestClanListInfo(1, m_LastSerch);
            }
        }
    }
}

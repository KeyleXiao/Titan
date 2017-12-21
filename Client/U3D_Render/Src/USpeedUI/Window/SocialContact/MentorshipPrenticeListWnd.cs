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
using Mentorship_ManagerDef;
using USpeedUI.UWidgets;
using ASpeedGame.Data.MentorshipConfig;

namespace USpeedUI.Social
{
    public class MentorshipPrenticeListWnd : UIBaseSubWnd<MentorshipPrenticeListWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)MentorshipSubWndType.WND_MENTORSHIP_PRENTICEISSUE;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewPrenticeIssueList";
        }

        // 初始化
        public override bool Init()
        {
            SetVisible(false);

            return base.Init();
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                RequestPrenticeIssueListInfo();
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_MENTORSHIP_UPDATEPERSONBASE:
                    {
                        if (m_wndView != null)
                            m_wndView.SetPrentcieOperateBtn();
                    }
                    break;
                case WndMsgID.WND_MSG_MENTORSHIP_PRENTICEISSUELIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvPrenticeIssueList();
                    }
                    break;
                default:
                    break;
            }
        }

        public void RequestPrenticeIssueListInfo()
        {
            cmd_mentorship_issuelist_head cmdData = new cmd_mentorship_issuelist_head();
            cmdData.nIssueType = (int)EMMentorshipIssueType.emMentorshipIssueType_Prentice;
            cmdData.nCount = 0;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_issuelist_head>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTISSUELIST, ref cmdData);
        }
    }

    public class PrenticeIssueListItem : MonoBehaviour, IPointerDownHandler
    {
        public Text name;
        public Text level;
        public Image sexIcon;
        public Text corpsName;
        public Text clanName;
        public Text issueInfo;
        public Image bgIcon;
        public Text stateDesc;
        //public Sprite selfBgBorder;
        public Sprite normalBgBorder;

        private int m_index;
        private int m_PersonPDBID;
        public int PersonPDBID { get { return m_PersonPDBID; } }
        private bool m_bIsSelf;
        private UPopupMenuHandle popupMenuHandle;
        private MentorshipPrenticeListWndView wndView;
        public void SetWndView(MentorshipPrenticeListWndView view, int index)
        {
            wndView = view;
            m_index = index;
        }

        public void Clear()
        {
            m_bIsSelf = false;
            bgIcon.sprite = normalBgBorder;
            if (popupMenuHandle != null)
                popupMenuHandle.HandlePopupAction = null;

            gameObject.SetActive(false);
            SetCheckMark(false);
        }

        public void SetCheckMark(bool bIsOn)
        {
            Toggle tmpToggle = gameObject.GetComponent<Toggle>();
            if (tmpToggle != null)
            {
                tmpToggle.isOn = bIsOn;
            }
        }

        public void SetData(cmd_mentorship_issueinfo data)
        {
            m_PersonPDBID = data.nPDBID;
            name.text = data.szName;
            level.text = data.nLevel.ToString();
            stateDesc.text = "";
            corpsName.text = (data.szCorpsName == "" ? "暂无" : data.szCorpsName);
            clanName.text = (data.szClanName == "" ? "暂无" : data.szClanName);
            issueInfo.text = (data.szIssueInfo == "" ? "暂无留言" : data.szIssueInfo);

            bgIcon.gameObject.SetActive(true);

            SetIssueItemStateDesc(LogicDataCenter.mentorshipDataManager.IsPApplyFreezing(m_PersonPDBID));

            if (data.bySex < (int)PERSON_SEX.SEX_MAX && data.bySex >= (int)PERSON_SEX.SEX_MALE)
            {
                sexIcon.enabled = true;
                sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_SOCIALCONTACT, data.bySex + 1);
            }
            else
            {
                sexIcon.enabled = false;
            }

            if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
            {
                if (m_PersonPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                {
                    m_bIsSelf = true;
                    //bgIcon.sprite = selfBgBorder;
                    bgIcon.gameObject.SetActive(true);
                }
            }

            if (!m_bIsSelf)
            {
                if (popupMenuHandle == null)
                    popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();

                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(m_PersonPDBID));
                popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(data.szName));
                popupMenuHandle.HandlePopupAction = popupAction;
                popupMenuHandle.gameObject.SetActive(true);
            }

            gameObject.SetActive(true);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (wndView != null)
            {
                wndView.OnSelectIssue(m_index, m_PersonPDBID, this);
            }
        }

        public void SetIssueItemStateDesc(bool bState)
        {
            if (bState)
            {
                stateDesc.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "IssueItemStateDesc");
            }
            else
            {
                stateDesc.text = "";
            }
        }
    }

    public class MentorshipPrenticeListWndView : UIBaseSubWndView
    {
        public PrenticeIssueListItem[] listIssueItem;
        public Image issueItemSelectImage;

        public Button issueBtn;
        public Text issueBtnText;
        public Button changeIssueBtn;
        public Text changeIssueBtnText;
        public Button internPrenticeBtn;
        public Text internPrenticeBtnText;

        public Text nameText;
        public Text levelText;
        public Text sexText;
        public Text corpsText;
        public Text clanText;
        public Text issueText;

        private int m_curIndex;
        private int m_curSelPersonPDBID;

        // 初始化
        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            issueBtnText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PIssueBtnDesc");
            changeIssueBtnText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PChangeIssueBtnDesc");
            internPrenticeBtnText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PInternPrenticeBtnDesc");

            nameText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PNameDesc");
            levelText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PLeveDesc");
            sexText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PSexDesc");
            corpsText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PCorpsNameDesc");
            clanText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PClanNameDesc");
            issueText.text = ULocalizationService.Instance.Get("UIView", "PrenticeIssueList", "PIssueDesc");

            m_curIndex = 0;
            m_curSelPersonPDBID = 0;
            for (int i = 0; i < listIssueItem.Length; ++i)
            {
                listIssueItem[i].SetWndView(this, i);
                listIssueItem[i].Clear();
            }

            return true;
        }

        public void OnRecvPrenticeIssueList()
        {
            OnSelectIssue(0, 0, null);
            List<cmd_mentorship_issueinfo> issueInfoList = LogicDataCenter.mentorshipDataManager.PrenticeIssueList;
            for (int i = 0; i < listIssueItem.Length; ++i)
            {
                listIssueItem[i].Clear();
            }
            for (int i = 0; i < listIssueItem.Length; ++i)
            {
                if (i < LogicDataCenter.mentorshipDataManager.PRENTICEISSUECOUNT)
                {
                    listIssueItem[i].SetData(issueInfoList[i]);
                }
            }

            SetPrentcieOperateBtn();
        }        

        public void OnIssueBtnClick()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_PRENTICE_MSGBOXISSUE, true);
        }

        public void OnChangeIssueBtnClick()
        {
            MentorshipPrenticeListWnd wnd = m_wnd as MentorshipPrenticeListWnd;
            wnd.RequestPrenticeIssueListInfo();
        }

        public void OnInternPrenticeBtnClick()
        {
            if (m_curSelPersonPDBID <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MENTORSHIP_NOT_SELECTED);
                return;
            }

            int nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (m_curSelPersonPDBID == nSelfPDBID)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MENTORSHIP_NOT_APPLY_PRENTICE);
                return;
            }

            if (LogicDataCenter.mentorshipDataManager.IsPApplyFreezing(m_curSelPersonPDBID))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MENTORSHIP_APPLY_FREQUENT);
                return;
            }

            cmd_mentorship_request_apply cmdData = new cmd_mentorship_request_apply();
            cmdData.nDesPDBID = m_curSelPersonPDBID;
            cmdData.nRelationType = (int)EMMentorshipType.emMentorshipType_Intern;
            cmdData.nApplyPost = (int)EMMentorshipPost.emMentorshipPost_Master;
            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_apply>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP, ref cmdData);

            listIssueItem[m_curIndex].SetIssueItemStateDesc(true);
            LogicDataCenter.mentorshipDataManager.AddPApplyRecord(m_curSelPersonPDBID, Time.unscaledTime + 60f);
        }

        public void OnSelectIssue(int index, int nPresonPDBID, PrenticeIssueListItem selItem)
        {
            listIssueItem[m_curIndex].SetCheckMark(false);
            m_curIndex = index;

            m_curSelPersonPDBID = nPresonPDBID;

            listIssueItem[m_curIndex].SetCheckMark(true);

            if (selItem != null)
            {
                issueItemSelectImage.rectTransform.SetPosition(selItem.gameObject.transform.position);
                issueItemSelectImage.enabled = true;
            }
            else
            {
                issueItemSelectImage.enabled = false;
            }
        }

        public void SetPrentcieOperateBtn()
        {
            bool bIssueBtnVisible = true;
            bool bPrenticeBtnVisible = true;
            cmd_mentorship_person_baseinfo personData = LogicDataCenter.mentorshipDataManager.MentorshipPersonData;
            if (personData.nPDBID > 0)
            {
                int nLevel = 0;
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                    nLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);

                if (LogicDataCenter.mentorshipDataManager.IsInternMasterFull() || nLevel < MentorshipConfig.Instance.tMentorshipConfig.nPrenticeNeedLevel)
                {
                    bIssueBtnVisible = false;
                }

                if (LogicDataCenter.mentorshipDataManager.IsInternPrenticeFull() || nLevel < MentorshipConfig.Instance.tMentorshipConfig.nMasterNeedLevel)
                {
                    bPrenticeBtnVisible = false;
                }
            }

            Color clr = new Color();
            string htmlString = (bIssueBtnVisible ? "#ffffff" : "#bebebe");
            ColorUtility.TryParseHtmlString(htmlString, out clr);
            issueBtn.interactable = bIssueBtnVisible;
            issueBtnText.color = clr;

            htmlString = (bPrenticeBtnVisible ? "#ffffff" : "#bebebe");
            ColorUtility.TryParseHtmlString(htmlString, out clr);
            internPrenticeBtn.interactable = bPrenticeBtnVisible;
            internPrenticeBtnText.color = clr;
        }
    }
}

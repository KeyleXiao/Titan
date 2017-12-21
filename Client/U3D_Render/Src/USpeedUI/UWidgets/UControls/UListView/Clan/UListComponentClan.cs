using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using GameLogic;
using USpeedUI;
using UnityEngine;
using DataCenter;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using USpeedUI.TooltipAndDialog;
using Clan_ManagerDef;
using U3D_Render.Common;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentClanMember : ListViewItem
    {
        public Image SexIcon;
        public Text actorName;
        public Text job;
        public Text level;
        public Text gaveThisWeek;
        public Text power;
        public Text rank;
        public Image rankImg;
        public Image changeImage;
        public Text sortChange;
        public Text grade;
        public Text status;
        public Text kinName;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        public Sprite DefalutBGSprite;
      //  public Image DefaultBG;
        public Sprite selfSelect;
        public UPopupMenuHandle popupMenuHandle;
        

        private int m_nMemberID = 0;

        private bool m_bSelf = false;

        public virtual void SelectColoring()
        {
            setComponentBG(1);
        }

        public virtual void DefaultColoring()
        {
            setComponentBG(0);
        }

        public virtual void SetData(UListItemClanMember item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;

            if (item.data.nSex < (int)PERSON_SEX.SEX_MAX && item.data.nSex >= (int)PERSON_SEX.SEX_MALE)
            {
                SexIcon.enabled = true;
                // 这里使用战队那边的,重复资源没必要再弄一份
                SexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_SOCIALCONTACT, item.data.nSex + 1);
            }
            else
            {
                SexIcon.enabled = false;
            }
            actorName.text = item.data.szName;
            if ((item.data.nIdentity == (int)EMClanIdentity.EMCLANIDENTITY_SHAIKH)
                && (LogicDataCenter.clanDataManager.ClanBaseData.nShaikhState == (int)EMClanShaikhState.ClanShaikhState_Outgoing))
            {
                actorName.text += "(卸任)";
            }

            job.text = LogicDataCenter.clanDataManager.GetClanIdentityName(item.data.nIdentity);
            level.text = item.data.nLevel.ToString();
            gaveThisWeek.text = item.data.nDonateThisWeek.ToString() + "/" + item.data.nDonateTotal;
            power.text = item.data.dwBattleVal.ToString();
            rank.text = item.data.dwBVOrder.ToString();
            if (Convert.ToInt32(item.data.dwBVOrder) <= (int)EMClanOrderMax.EMCLANORDERMAX)
            {
                rank.enabled = false;
                rankImg.enabled = true;
                // 这里使用战队那边的,重复资源没必要再弄一份
                rankImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinBattleOrder, WndID.WND_ID_SOCIALCONTACT, item.data.dwBVOrder);
            }
            else
            {
                rank.enabled = true;
                rankImg.enabled = false;
            }
            int nChangeOrder = item.data.dwLastOrder - item.data.dwBVOrder;
            if (item.data.dwLastOrder == 0)
            {
                nChangeOrder = 0;
            }
            sortChange.text = nChangeOrder.ToString();
            if (nChangeOrder >= 0)
            {
                changeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ClanRankChange, WndID.WND_ID_SOCIALCONTACT, 1);
                Color clr;
                ColorUtility.TryParseHtmlString("#fb8507", out clr);
                sortChange.color = clr;
            }
            else
            {
                changeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ClanRankChange, WndID.WND_ID_SOCIALCONTACT, 2);
                Color clr;
                ColorUtility.TryParseHtmlString("#71bf81", out clr);
                sortChange.color = clr;
            }
            grade.text = item.data.szGrade;
            kinName.text = item.data.szKinName;

            int nGameState = item.data.nIsOnline == 0 ? (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE : item.data.nGameStatus;
            string strStateColor = UDefines.GetGameStateColor(nGameState);
            string strStateDesc = UDefines.GetGameStateDesc(nGameState);
            status.text = String.Format("<color=#{0}>{1}</color>", strStateColor, ULocalizationService.Instance.Get("UIView", "Common", strStateDesc));

            if (item.data.nIsOnline > 0)
            {
                Color clr;
                ColorUtility.TryParseHtmlString("#FFFFFF", out clr);
                actorName.color = clr;
                job.color = clr;
                level.color = clr;
                gaveThisWeek.color = clr;
                power.color = clr;
                rank.color = clr;
                grade.color = clr;
                kinName.color = clr;
            }
            else
            {
                Color clr;
                ColorUtility.TryParseHtmlString("#adadad", out clr);
                status.color = clr;
                actorName.color = clr;
                job.color = clr;
                level.color = clr;
                gaveThisWeek.color = clr;
                power.color = clr;
                rank.color = clr;
                grade.color = clr;
                kinName.color = clr;
            }

            m_nMemberID = item.data.nPDBID;

            if (popupMenuHandle == null)
                popupMenuHandle = gameObject.AddComponent<UPopupMenuHandle>();

            int nHeroID = 0;
            int nHeroKinID = 0;
            if (EntityFactory.MainHeroView != null)
            {
                nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                nHeroKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);

                m_bSelf = item.data.nPDBID == nHeroID ? true : false;
                // 设置显示图片
                setComponentBG(0);
            }

            if (m_nMemberID != nHeroID)
            {
                UPopupAction popupAction = new UPopupAction();
                popupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(m_nMemberID));
                popupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(m_nMemberID));
                popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(m_nMemberID));
                popupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(m_nMemberID, actorName.text));
                popupAction.Add(UPopupItemDataCopyName.GetDescString(), new UPopupItemDataCopyName(item.data.szName));
                if(item.data.nIsOnline > 0 && GameLogicAPI.isMainMap((int)item.data.dwSceneID))
                    popupAction.Add(UPopupItemDataTransmitTo.GetDescString(), new UPopupItemDataTransmitTo((int)item.data.dwWorldID, (int)item.data.serverID, (int)item.data.dwSceneID));

                if (item.data.nIdentity == (int)EMClanIdentity.EMCLANIDENTITY_ELDER)
                {
                    popupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanSetLeader"), new UListComponentItemDataPopup(OnSetLeader));
                }

                if (item.data.nKinID != nHeroKinID)
                {
                    popupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ClanKick"), new UListComponentItemDataPopup(OnKick));
                }

                popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        void OnSetLeader()
        {
            String strMsg = String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "SetLeaderDesc"), actorName.text);
            UIDialogData data = new UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "Common", "TipText"),
                                    message: strMsg,
                                    buttons: new DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), OnSetLeaderConfirm },
                                                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        bool OnSetLeaderConfirm()
        {
            cmd_clan_setleader msgInfo = new cmd_clan_setleader();
            msgInfo.nActorID = m_nMemberID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_setleader>(GameLogicDef.GVIEWCMD_CLAN_SETLEADER, ref msgInfo);
            return true;
        }

        void OnKick()
        {
            String strMsg = String.Format(ULocalizationService.Instance.Get("UIView", "ClanDetail", "KickMemberDesc"), actorName.text);
            UIDialogData data = new UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "Common", "TipText"),
                                    message: strMsg,
                                    buttons: new DialogActions() { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), OnKickConfirm },
                                                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        bool OnKickConfirm()
        {
            cmd_clan_kickkin msgInfo = new cmd_clan_kickkin();
            msgInfo.nActorID = m_nMemberID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_kickkin>(GameLogicDef.GVIEWCMD_CLAN_KICKKIN, ref msgInfo);
            return true;
        }

        void setComponentBG(int nType)
        {
            if (nType == 1)
            {
                // 选中
                if (ComponentBG != null)
                    ComponentBG.sprite = SelectBGSprite;
            }
            else
            {
                if (m_bSelf)
                {
                    ComponentBG.sprite = selfSelect;
                }
                else
                {
                    ComponentBG.sprite = DefalutBGSprite;
                }
            }
        }
    }

    public class UListComponentClanAcceptMember : ListViewItem
    {
        public Text kinName;
        public Text kinLeader;
        public Text kinMemberCount;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        public Sprite DefaultBGSprite;

        public virtual void SetData(UListItemClanAcceptMember item)
        {
            kinName.text = item.data.szName;
            kinLeader.text = item.data.szShaikhName;
            kinMemberCount.text = item.data.nMemberCount.ToString();
        }

        public virtual void DefaultColoring()
        {
            // 未选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;
        }
    }

    public class UListComponentClanEvent : ListViewItem
    {
        public Text ClanEventDes;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        public Sprite DefalutBGSprite;
        //public Image DefaultBG;

        public virtual void SetData(UListItemClanEvent item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;

            string sEventDesc = item.ClanEventDes;
            UBB.toHtml(ref sEventDesc, UBB_FORMAT_TYPE.UGUI);
            ClanEventDes.text = sEventDesc;
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;
        }

        public virtual void DefaultColoring()
        {
            // 选中
            //SelectBG.gameObject.SetActive(false);
            if (ComponentBG != null)
                ComponentBG.sprite = DefalutBGSprite;
        }
    }
}

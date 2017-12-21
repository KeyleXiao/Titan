using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine;
using Mail_ManagerDef;
using U3D_Render.Common;
using UnityEngine.EventSystems;
using Data.ActorPrizeConfig;
using DataCenter;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentTaskReply : ListViewItem
    {
        public Text ReplyIndex;
        public Image HeadIcon;
        public Text ReplyName;
        public Image ReplySexIcon;
        public Text ReplyContent;
        public Text ReplyTime;
        public UPopupMenuHandle ReplyMenuHandle;

        public virtual void SetData(UListItemTaskReply reply)
        {
            if (reply == null)
                return;

            // 楼层
            ReplyIndex.text = String.Format("{0}楼", reply.nIndex); 

            // 文字
            ReplyName.text = reply.node.szReplyName;
            ReplyContent.text = reply.node.szReplyContent;

            // 性别
            if (reply.node.byReplySex < (byte)PERSON_SEX.SEX_MAX && reply.node.byReplySex >= (byte)PERSON_SEX.SEX_MALE)
            {
                ReplySexIcon.gameObject.SetActive(true);
                ReplySexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_TASK_UNFOLD, reply.node.byReplySex + 1);
                HeadIcon.gameObject.SetActive(true);
                HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_TASK_UNFOLD, 1, reply.node.byReplySex + 1);
            }
            else
            {
                ReplySexIcon.gameObject.SetActive(false);
                HeadIcon.gameObject.SetActive(false);
            }

            // 时间
            DateTime replyTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(reply.node.nReplyTime);
            replyTime = replyTime.ToLocalTime();
            int nGapTime = reply.nServerTime - reply.node.nReplyTime;
            if (nGapTime < 60)//1分钟
            {
                ReplyTime.text = String.Format("{0}秒前", nGapTime);
            }
            else if (nGapTime < 3600)//1小时
            {
                ReplyTime.text = String.Format("{0}分钟前", nGapTime / 60);
            }
            else if (nGapTime < 24 * 3600)//1天
            {
                ReplyTime.text = String.Format("{0}小时前", nGapTime / 3600);
            }
            else if (nGapTime < 2 * 24 * 3600)//2天
            {
                ReplyTime.text = String.Format("昨天 {0:D2}:{1:D2}", replyTime.Hour, replyTime.Minute);
            }
            else if (nGapTime < 3 * 24 * 3600)//3天
            {
                ReplyTime.text = String.Format("前天 {0:D2}:{1:D2}", replyTime.Hour, replyTime.Minute);
            }
            else
            {
                ReplyTime.text = String.Format("{0}-{1}-{2} {3:D2}:{4:D2}", replyTime.Year, replyTime.Month, replyTime.Day, replyTime.Hour, replyTime.Minute);
            }
            // 右键弹窗
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            int nTargetID = reply.node.nReplyPDBID;
            if (nTargetID > 0 && nTargetID != nSelfID)
            {
                UPopupAction PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(reply.node.nReplyUserID, reply.node.szReplyName));
                PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(nTargetID));
                PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(nTargetID));
                PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(nTargetID));
                PopupAction.Add(UPopupItemDataBuddyChat.GetDescString(), new UPopupItemDataBuddyChat(nTargetID));
                PopupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(reply.node.szReplyName));

                ReplyMenuHandle.HandlePopupAction = PopupAction;
            }
        }
    }
}

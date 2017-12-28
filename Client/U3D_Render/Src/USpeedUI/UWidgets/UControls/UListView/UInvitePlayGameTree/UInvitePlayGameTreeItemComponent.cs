/*******************************************************************
** 文件名:	UBuddyInviteTreeItemComponent.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/6/2
** 版  本:	1.0
** 描  述:	邀请好友游戏
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using ASpeedGame.Data.RunePage;
using DataCenter;
using U3D_Render.Common;
using USpeedUI.PlayerFrame;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;

namespace USpeedUI.UWidgets
{
    public class UInvitePlayGameTreeItemComponent : TreeViewComponentBase<UInvitePlayGameTreeItemData>//, IPointerClickHandler
    {
        UInvitePlayGameTreeItemData item;

        // 游戏状态描述
        private string[] GameStateDesc = null;

        protected override void Start()
        {
            base.Start();
        }
		
        protected override void Awake()
        {
            if ((LocalPositionZReset) && (transform.localPosition.z != 0f))
            {
                var pos = transform.localPosition;
                pos.z = 0f;
                transform.localPosition = pos;
            }

			onClick.AddListener(onToggle);
        }
		
        /// <summary>
        /// Gets or sets the item.
        /// </summary>
        /// <value>The item.</value>
        public UInvitePlayGameTreeItemData Item
        {
            get
            {
                return item;
            }
            set
            {
                if (item != null)
                {
                    item.OnChange -= UpdateView;
                }
                item = value;
                if (item != null)
                {
                    item.OnChange += UpdateView;
                }
                UpdateView();
            }
        }
        private int nItemDepth;
        public int ItemDepth
        {
            get { return nItemDepth; }
            protected set
            {
                nItemDepth = value;
            }
        }

        // 好友分组
        public Transform GroupTrans;

        // 好友信息
        public Transform PlayerTrans;
        public Text ActorName;
        public Image HeadImage;
        public Image SexImage;
        public Text StateDesc;
        public Image StateIcon;
        public Image RankImage;
        public Text RankName;
        public GameObject CheckMarkImage;
        public GameObject CheckMarkBg;

        public override void SetData(TreeNode<UInvitePlayGameTreeItemData> newNode, int depth)
        {
            Node = newNode;
            ItemDepth = depth;
            base.SetData(Node, depth);
          
            Item = (Node == null) ? null : Node.Item;
        }

        /// <summary>
        /// Updates the view.
        /// </summary>
        protected virtual void UpdateView()
        {
            if (ItemDepth > 0)  // 显示好友
            {
                GroupTrans.gameObject.SetActive(false);
                PlayerTrans.gameObject.SetActive(true);

                if (item.nGroupID == (int)InvitePlayGame.InvitePlayGameGroup.emBuddyGroup
                    || item.nGroupID == (int)InvitePlayGame.InvitePlayGameGroup.emNearbyGroup)
                {
                    SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(Item.nUserID);
                    ActorName.text = getBuddyName(buddyInfo);
                }
                else
                {
                    ActorName.text = item.Name;
                }

                HeadImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_INVITEPLAYGAME, 1, 1); // 设置头像
                SexImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_SNS, "Sex_" + item.nSex, WndID.WND_ID_INVITEPLAYGAME);
                RankName.text = Item.RankName;

                // 游戏状态图标
                int nStateIconID = UDefines.GetGameStateIconID(item.nGameState);
                StateIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GameState, WndID.WND_ID_INVITEPLAYGAME, nStateIconID);

                // 当前游戏状态描述
                if (item.isInvited)
                {
                    StateDesc.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "AlreadyInivtedDesc");
                }
                else
                {
                    string GameStateDesc = UDefines.GetGameStateDesc(item.nGameState);
                    string GameStateColor = UDefines.GetGameStateColor(item.nGameState);
                    StateDesc.text = String.Format("<color=#{0}>{1}</color>", GameStateColor, ULocalizationService.Instance.Get("UIView", "Common", GameStateDesc));
                }

                CheckMarkImage.SetActive(item.isSelected);

                if (item.isCanInvite)
                    CheckMarkBg.SetActive(true);
                else
                    CheckMarkBg.SetActive(false);
            }
            else   // 显示分组
            {
				Text.text = Item.Name;
                GroupTrans.gameObject.SetActive(true);
                PlayerTrans.gameObject.SetActive(false);
            }
        }

        /// <summary>
        /// Called when item moved to cache, you can use it free used resources.
        /// </summary>
        public override void MovedToCache()
        {
            if (Icon != null)
            {
                Icon.sprite = null;
            }
        }

        /// <summary>
        /// This function is called when the MonoBehaviour will be destroyed.
        /// </summary>
        protected override void OnDestroy()
        {
            if (item != null)
            {
                item.OnChange -= UpdateView;
            }
            base.OnDestroy();
        }

		/// <summary>
		/// 
		/// </summary>
		private void onToggle()
		{
            if (ItemDepth == 0)
            {
                //点击分组时，触发这个函数，而不止是点击Toggle的时候才展开分组
                ToggleNode();
            }
            else
            {
                onClickPlayer();
            }
        }

		private string getBuddyName(SNSDataManager.BuddyInfo buddyInfo)
		{
			string remark = LogicDataCenter.snsDataManager.getBuddyRemark(buddyInfo.Info.nUserID);

			remark = remark == null ? "" : remark;

			if (String.IsNullOrEmpty(remark) || remark == buddyInfo.Info.szName)
			{
				return buddyInfo.Info.szName;
			}
			else
			{
				return string.Format("{0}（{1}）", remark, buddyInfo.Info.szName);
			}
		}

        public void onClickPlayer()
        {
            if (item.isCanInvite == false)
                return;

            if(CheckMarkImage.activeSelf)
            {
                CheckMarkImage.SetActive(false);
                item.isSelected = false;
            }
            else
            {
                CheckMarkImage.SetActive(true);
                item.isSelected = true;
            }
        }
	}
}

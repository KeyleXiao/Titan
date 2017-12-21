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
    public class UBuddyInviteTreeItemComponent : TreeViewComponentBase<UBuddyInviteTreeItemData>//, IPointerClickHandler
    {
        UBuddyInviteTreeItemData item;

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

			onClick.AddListener(MyToggleEvent);
        }
		
        /// <summary>
        /// Gets or sets the item.
        /// </summary>
        /// <value>The item.</value>
        public UBuddyInviteTreeItemData Item
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
        public Transform BuddyGroupTrans;

        // 好友信息
        public Transform BuddyInfoTrans;
        public Text BuddyName;
        public Text BuddyStatus;

        public override void SetData(TreeNode<UBuddyInviteTreeItemData> newNode, int depth)
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
				DataCenter.SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(Item.nUserID);

                BuddyGroupTrans.gameObject.SetActive(false);
                BuddyInfoTrans.gameObject.SetActive(true);

				BuddyName.text = getBuddyName(buddyInfo);
				//BuddyStatus.text = buddyInfo.Info.szStatusStr;
			}
			else   // 显示分组
            {
				Text.text = Item.Name;
                BuddyGroupTrans.gameObject.SetActive(true);
                BuddyInfoTrans.gameObject.SetActive(false);
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
		/// 点击分组时，触发这个函数，而不止是点击Toggle的时候才展开分组
		/// </summary>
		public void MyToggleEvent()
		{
			ToggleNode();
		}

		/// <summary>
		/// 双击好友，打开聊听窗口
		/// </summary>
		public void openChatWnd()
        {
            if (ItemDepth > 0)
            {
                DataCenter.SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(Item.nUserID);

                SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
                msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
                msgData.nSessionID = buddyInfo.Info.nUserID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
            }
        }

		private string getBuddyName(SNSDataManager.BuddyInfo buddyInfo)
		{
			string remark = LogicDataCenter.snsDataManager.getBuddyRemark(buddyInfo.Info.nUserID);

			remark = remark == null ? "" : remark;

			if (remark.Length == 0)
			{
				return buddyInfo.Info.szName;
			}
			else
			{
				return string.Format("{0}（{1}）", remark, buddyInfo.Info.szName);
			}
		}
	}
}

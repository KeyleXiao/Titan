/*******************************************************************
** 文件名:	UBuddySessionListView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/9/22
** 版  本:	1.0
** 描  述:	好友系统聊天框会话列表项
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections.Generic;
using DataCenter;
using UnityEngine.EventSystems;
using USpeedUI.UWidgets;
using USpeedUI;
using USpeedUI.UExtensions;

namespace UIWidgets {
	public class UBuddySessionListItemComponent : ListViewItem, IResizableItem {
		/// <summary>
		/// Gets the objects to resize.
		/// </summary>
		/// <value>The objects to resize.</value>
		public GameObject[] ObjectsToResize {
			get
            {
				return new GameObject[] 
                {
                    HeadIcon.transform.parent.gameObject,
                    GameStateIcon.gameObject,
                    Name.gameObject,
                };
			}
		}

		[SerializeField]
		public Image HeadIcon;
		[SerializeField]
        public Text GameStateDesc;
		[SerializeField]
        public Image GameStateIcon;
		[SerializeField]
		public Text Name;
		[SerializeField]
		public Image SelectedBg;

		UBuddySessionListItem item;

		/// <summary>
		/// Gets the current item.
		/// </summary>
		/// <value>Current item.</value>
		public UBuddySessionListItem Item {
			get {
				return item;
			}
		}
		
		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="newItem">Item.</param>
		public virtual void SetData(UBuddySessionListItem newItem)
		{
			item = newItem;

            if (item == null)
                return;

            SNSDataManager.BuddyInfo buddy = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(newItem.UserID);

            HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_SNS_CHAT, 1, buddy.Info.nSex+1);
			GameStateDesc.text = LogicDataCenter.snsDataManager.getBuddyStatusDesc(buddy);
            Name.text = buddy.Info.szName;

            if (buddy.Info.nStatus == (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
            {
                GameStateIcon.gameObject.SetActive(false);
                HeadIcon.AddNewMaterial(EUIShaderType.EST_Gray);
                Name.color = Color.gray;
                GameStateDesc.color = Color.gray;
            }
            else
            {
                HeadIcon.ResetToDefaultMaterial();
                GameStateIcon.gameObject.SetActive(true);
                GameStateIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GameState, WndID.WND_ID_SNS_MAINPANEL,
                        UDefines.GetGameStateIconID(buddy.Info.nStatus));
                Name.color = Color.white;
                GameStateDesc.color = Color.white;
            }

            SelectedBg.gameObject.SetActive(item.isCurSession);
        }

		/// <summary>
		/// Called when item moved to cache, you can use it free used resources.
		/// </summary>
		public override void MovedToCache()
		{
			if (HeadIcon != null)
			{
                HeadIcon.sprite = null;
			}

			if (GameStateIcon != null)
			{
                GameStateIcon.sprite = null;
			}
		}

		public override void OnPointerClick(PointerEventData eventData)
		{
			base.OnPointerClick(eventData);

            SNSSelectSessionMsgData msgData = new SNSSelectSessionMsgData();
            msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SELECT_SESSION;
            msgData.dwUserID = item.UserID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SELECT_SESSION, msgData);
        }
	}
}
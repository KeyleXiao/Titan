/*******************************************************************
** 文件名:	SNSFindBuddyWndView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/4/11
** 版  本:	1.0
** 描  述:	SNS搜索好友
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.Common;
using DataCenter;
using DG.Tweening;
using USpeedUI.UWidgets;
using UIWidgets;
using GameLogic;
using UnityEngine.EventSystems;

namespace USpeedUI.SNS
{
	public class SearchBuddyInputField : InputField
	{
		public Action onSelectEvent;
		public Action onDeselectEvent;

		public override void OnSelect(BaseEventData eventData)
		{
			if(onSelectEvent != null)
				onSelectEvent();

			base.OnSelect(eventData);
		}

		public override void OnDeselect(BaseEventData eventData)
		{
			if(onDeselectEvent != null)
				onDeselectEvent();

			base.OnDeselect(eventData);
		}
	}

    public class SNSSearchBuddyWndView : MonoBehaviour, IPointerExitHandler, IPointerEnterHandler
    {
		public UBuddySearchListView SearchList;

		private bool m_IsPointerExit;
		public bool IsPointerExit { get { return m_IsPointerExit; } }

        public bool Init()
        {

            return true;
        }

        public void closeFindBuddyWnd()
        {
             gameObject.SetActive(false);
        }

		public void onSearchBuddy(string filterStr)
		{
			filterStr = filterStr.ToLower();

			// 查找符合过滤字的好友名字
			List<int> buddyList = LogicDataCenter.snsDataManager.searchBuddy(filterStr);

			SearchList.DataSource.Clear();
			// 把好友的名字填充到列表中
			foreach(int userID in buddyList)
			{
				UBuddySearchListItem item = new UBuddySearchListItem();
				item.UserID = userID;
				SearchList.DataSource.Add(item);
			}
			SearchList.DataSource.EndUpdate();
		}

		public void OnPointerExit(PointerEventData eventData)
		{
			m_IsPointerExit = true;
		}

		public void OnPointerEnter(PointerEventData eventData)
		{
			m_IsPointerExit = false;
		}
	}
}

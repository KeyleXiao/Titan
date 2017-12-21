/*******************************************************************
** 文件名:	UBuddyGroupTreeItemComponent.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/7
** 版  本:	1.0
** 描  述:	好友系统中好友分组列表
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
    public class UBuddyGroupTreeItemComponent : TreeViewComponentBase<UBuddyGroupTreeItemData>
    {
        UBuddyGroupTreeItemData item;

        protected override void Start()
        {
            base.Start();

            if (Toggle != null)
            {
                Toggle.OnClick.RemoveListener(ToggleNode);
            }

            // 监听点击事件
            onClick.AddListener(MyToggleEvent);
            onDoubleClick.AddListener(openChatWnd);
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

            onClick.RemoveListener(MyToggleEvent);
            onDoubleClick.RemoveListener(openChatWnd);

            base.OnDestroy();
        }

        protected override void Awake()
        {
            if ((LocalPositionZReset) && (transform.localPosition.z != 0f))
            {
                var pos = transform.localPosition;
                pos.z = 0f;
                transform.localPosition = pos;
            }
        }

        /// <summary>
        /// Gets or sets the item.
        /// </summary>
        /// <value>The item.</value>
        public UBuddyGroupTreeItemData Item
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
        public Image SexIcon;
        public Text BuddyName;
        public Text GameStateDesc;
        public Image GameStateIcon;
        public Image RankIcon;
        public Text RankName;

        private UPopupMenuHandle popupMenuHandle;

        public override void SetData(TreeNode<UBuddyGroupTreeItemData> newNode, int depth)
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
                BuddyItemComponentManager.initBuddyItem(this);
            }
            else   // 显示分组
            {
                DataCenter.SNSDataManager.BuddyGroup buddyGroupInfo = LogicDataCenter.snsDataManager.getBuddyGroup(item.nBuddyGroupID);

                BuddyGroupTrans.gameObject.SetActive(true);
                BuddyInfoTrans.gameObject.SetActive(false);

                Text.text = String.Format("{0}  {1}/{2}", buddyGroupInfo.Info.szBuddyGroupName, LogicDataCenter.snsDataManager.getOnlineBuddyNum(buddyGroupInfo.Info.nBuddyGroupID),
                    LogicDataCenter.snsDataManager.getBuddyNum(buddyGroupInfo.Info.nBuddyGroupID));

                addPopupMenuForBuddyGroup(buddyGroupInfo.Info.nBuddyGroupID);
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
        /// 点击好友分组时，触发这个函数，而不止是点击Toggle的时候才展开好友分组
        /// </summary>
        public void MyToggleEvent()
        {
            ToggleNode();

			if(ItemDepth == 0)
			{
				LogicDataCenter.snsDataManager.setBuddyGroupExpand(item.nBuddyGroupID, Node.IsExpanded);
			}
        }

        /// <summary>
        /// 双击好友，打开聊听窗口
        /// </summary>
        public void openChatWnd(ListViewItem item)
        {
            if (ItemDepth > 0)
            {
                DataCenter.SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(Item.nUserID);

                SNSOpenChatWndMsgData msgData = new SNSOpenChatWndMsgData();
                msgData.msgID = (int)WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND;
                msgData.SessionID = buddyInfo.Info.nUserID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_CHAT_WND, msgData);
            }
        }

  

        // 为好友分组添加PopupMenu
        private void addPopupMenuForBuddyGroup(int buddyGroupID)
        {
            // 添加PopupAction
            popupMenuHandle = BuddyGroupTrans.gameObject.GetComponent<UPopupMenuHandle>();
            if (popupMenuHandle != null)
            {
                popupMenuHandle.HandlePopupAction.Clear();

                UPopupAction popupAction = new UPopupAction();

                popupAction.Add(UPopupItemDataAddBuddyGroup.GetDescString(), new UPopupItemDataAddBuddyGroup());
                // 非系统默认的分组才可以编辑名字和删除
                if (!DataCenter.SNSDataManager.isDefaultBuddyGroup(buddyGroupID))
                {
                    popupAction.Add(UPopupItemDataUpdateBuddyGroupName.GetDescString(), new UPopupItemDataUpdateBuddyGroupName(buddyGroupID));
                    popupAction.Add(UPopupItemDataDelBuddyGroup.GetDescString(), new UPopupItemDataDelBuddyGroup(buddyGroupID));
                }

                popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        // 好友内容初始化管理器
        public class BuddyItemComponentManager
        {
            public static void initBuddyItem(UBuddyGroupTreeItemComponent component)
            {
                int nUserID = component.Item.nUserID;
             
                BuddyItemComponentBase buddyItem = new BuddyItemComponentBase(component);

                int nBuddyGroupID = LogicDataCenter.snsDataManager.getBuddyGroupID(nUserID);
                switch (nBuddyGroupID)
                {
                    case (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST:
                        {
                            buddyItem = new BuddyItemComponentBlackList(component);
                        }
                        break;
                    case (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_NEARBY:
                        {
                            buddyItem = new BuddyItemComponentNearby(component);
                        }
                        break;
                    case (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_LastPlaying:
                        {
                            buddyItem = new BuddyItemComponentLastPlay(component);
                        }
                        break;
                }

                buddyItem.initContent();

                component.popupMenuHandle = component.BuddyInfoTrans.gameObject.GetComponent<UPopupMenuHandle>();
                if (component.popupMenuHandle != null)
                {
                    component.popupMenuHandle.HandlePopupAction.Clear();
                    buddyItem.addPopupMenu();
                }
            }
        }

        // 普通好友分组下的好友显示
        public class BuddyItemComponentBase
        {
            protected UBuddyGroupTreeItemComponent m_Component;

			protected string  getBuddyName(SNSDataManager.BuddyInfo buddyInfo)
			{
				string remark = LogicDataCenter.snsDataManager.getBuddyRemark(buddyInfo.Info.nUserID);

				if(String.IsNullOrEmpty(remark))
				{
					return buddyInfo.Info.szName;
				}
				else
				{
					return string.Format("{0}({1})", remark, buddyInfo.Info.szName);
				}
			}

            public BuddyItemComponentBase(UBuddyGroupTreeItemComponent component)
            {
                m_Component = component;
            }

            public virtual void initContent()
            {
                SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);

                m_Component.BuddyInfoTrans.gameObject.SetActive(true);
                m_Component.BuddyGroupTrans.gameObject.SetActive(false);

                m_Component.Icon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_SNS_MAINPANEL, 1, buddyInfo.Info.nSex+1); // 设置头像
                m_Component.SexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_SNS, "Sex_" + buddyInfo.Info.nSex, WndID.WND_ID_SNS_MAINPANEL);
                m_Component.RankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_SNS_MAINPANEL, 2, buddyInfo.Info.nRankIconID);
				m_Component.BuddyName.text = getBuddyName(buddyInfo);
				m_Component.GameStateDesc.text = LogicDataCenter.snsDataManager.getBuddyStatusDesc(buddyInfo);
				m_Component.GameStateIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GameState, WndID.WND_ID_SNS_MAINPANEL, 
                    UDefines.GetGameStateIconID(buddyInfo.Info.nStatus));
                m_Component.RankName.text = buddyInfo.Info.szRankName;

                if (buddyInfo.Info.nStatus == (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
                {
                    Color gray = new Color(118f / 255f, 120f / 255f, 123f / 255f);
					m_Component.Icon.AddNewMaterial(EUIShaderType.EST_Gray);
					m_Component.SexIcon.AddNewMaterial(EUIShaderType.EST_Gray);
					m_Component.RankIcon.AddNewMaterial(EUIShaderType.EST_Gray);
					m_Component.BuddyName.color = gray;
                    m_Component.GameStateDesc.color = gray;
                    m_Component.RankName.color = gray;

                    m_Component.GameStateIcon.gameObject.SetActive(false);
                }
                else
                {
					m_Component.Icon.ResetToDefaultMaterial();
					m_Component.RankIcon.ResetToDefaultMaterial();
					m_Component.SexIcon.ResetToDefaultMaterial();
                    m_Component.BuddyName.color = Color.white;
                    m_Component.RankName.color = Color.white;

                    m_Component.GameStateIcon.gameObject.SetActive(true);
                }

                setBuddyInfoTooltips(buddyInfo);
            }

            private void setBuddyInfoTooltips(DataCenter.SNSDataManager.BuddyInfo buddy)
            {

                UTooltipTrigger buddyInfoTooltipTrigger = m_Component.BuddyInfoTrans.gameObject.GetComponent<UTooltipTrigger>();
                if (buddyInfoTooltipTrigger == null)
                {
                    buddyInfoTooltipTrigger = m_Component.BuddyInfoTrans.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_SNSBuddyInfo);
                    buddyInfoTooltipTrigger.tipPosition = TipPosition.BottomLeftCorner;
                }

                int nWinPro = buddy.Info.nMatchNum > 0 ? (buddy.Info.nWinNum * 100 / buddy.Info.nMatchNum) : 0;
                buddyInfoTooltipTrigger.SetImage("Avatar", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_SNS_MAINPANEL, 1, buddy.Info.nSex + 1));
                buddyInfoTooltipTrigger.SetText("Name", buddy.Info.szName);
                buddyInfoTooltipTrigger.SetImage("RankIcon", USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_SNS_MAINPANEL, 2, buddy.Info.nRankIconID));
                buddyInfoTooltipTrigger.SetText("RankName", buddy.Info.szRankName);
                buddyInfoTooltipTrigger.SetText("Clan", ULocalizationService.Instance.Get("UIView", "SNS", "Clan") + buddy.Info.szClan);
                buddyInfoTooltipTrigger.SetText("WinPro", ULocalizationService.Instance.Get("UIView", "SNS", "WinPro") + nWinPro + "%");
                buddyInfoTooltipTrigger.SetText("MatchNum", ULocalizationService.Instance.Get("UIView", "SNS", "MatchNum") + buddy.Info.nMatchNum);
                buddyInfoTooltipTrigger.SetText("Status", LogicDataCenter.snsDataManager.getBuddyStatusDesc(buddy));
            }

            public virtual void addPopupMenu()
            {
                UPopupAction popupAction = new UPopupAction();
                
                DataCenter.SNSDataManager.BuddyInfo buddy= LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);
				bool isSameWorld = LogicDataCenter.snsDataManager.isSameGameWorld(buddy.Info.nUserID);

				popupAction.Add(UPopupItemDataBuddyChat.GetDescString(), new UPopupItemDataBuddyChat(buddy.Info.nUserID));
				if (isSameWorld)
				{
					if (buddy.Info.nStatus != (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
					{
						popupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest((int)buddy.Info.dwPdbID));
						popupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite((int)buddy.Info.dwPdbID));
					}
					popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataKinRequest.GetDescString(), new UPopupItemDataKinRequest((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataKinInvite.GetDescString(), new UPopupItemDataKinInvite((int)buddy.Info.dwPdbID));
				}
				if (buddy.Info.nStatus != (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE)
				{
					popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(buddy.Info.szName));
                    if(GameLogicAPI.isMainMap((int)buddy.Info.dwSceneID))
				        popupAction.Add(UPopupItemDataTransmitTo.GetDescString(), new UPopupItemDataTransmitTo(buddy.Info.dwWorldID, buddy.Info.dwServerID, buddy.Info.dwSceneID));
                }
                popupAction.Add(UPopupItemDataDelBuddy.GetDescString(), new UPopupItemDataDelBuddy(buddy.Info.nUserID, buddy.Info.szName));
                popupAction.Add(UPopupItemDataUpdateRemark.GetDescString(), new UPopupItemDataUpdateRemark(buddy.Info.nUserID, buddy.Info.szName));

                // 移动好友到分组
                UListComponentItemDataPopup popupItem = new UListComponentItemDataPopup(_actionSelected: null, _listChildItems: new List<UListComponentItemDataPopupBase>());
                Dictionary<int, string> buddyGroupList = new Dictionary<int, string>();
                int nValidBuddyGroupCount = 0;
                foreach (var item in LogicDataCenter.snsDataManager.getBuddyGroupList())
                {
                     if (item.Info.nBuddyGroupID == (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_BLACKLIST
                         || item.Info.nBuddyGroupID == (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_LastPlaying
                         || item.Info.nBuddyGroupID == (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_NEARBY)
                     {
                         continue;
                     }
                     if (item.Info.nBuddyGroupID == LogicDataCenter.snsDataManager.getBuddyGroupID(buddy.Info.nUserID))
                     {
                         continue;
                     }
                     popupItem.ItemChildItems.Add(new UPopupItemDataChangeBuddyGroup(buddy.Info.nUserID, item.Info.nBuddyGroupID, _strItemTypeName: item.Info.szBuddyGroupName));

                    nValidBuddyGroupCount++;
                }
                if(nValidBuddyGroupCount > 0)
                {
                    popupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "ChangeBuddyGroup"), popupItem);
                }

                popupAction.Add(UPopupItemDataAddBlackList.GetDescString(), new UPopupItemDataAddBlackList(buddy.Info.nUserID, buddy.Info.szName));
                

                 m_Component.popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        // 黑名单分组下的好友显示
        public class BuddyItemComponentBlackList : BuddyItemComponentBase
        {
            public BuddyItemComponentBlackList(UBuddyGroupTreeItemComponent component)
                :base(component)
            {
            }

            public override void initContent()
            {
                base.initContent();

                SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);

                m_Component.BuddyName.text = buddyInfo.Info.szName;
            }

            public override void addPopupMenu()
            {
                UPopupAction popupAction = new UPopupAction();
                
                DataCenter.SNSDataManager.BuddyInfo buddy = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);
				bool isSameWorld = LogicDataCenter.snsDataManager.isSameGameWorld(buddy.Info.nUserID);

                popupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(buddy.Info.nUserID, buddy.Info.szName));

				if(isSameWorld)
					popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)buddy.Info.dwPdbID));

                popupAction.Add(UPopupItemDataDelBuddy.GetDescString(), new UPopupItemDataDelBuddy(buddy.Info.nUserID, buddy.Info.szName));

                m_Component.popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        // 附近的人分组下的好友显示
        public class BuddyItemComponentNearby : BuddyItemComponentBase
        {
            public BuddyItemComponentNearby(UBuddyGroupTreeItemComponent component)
                : base(component)
            {
            }

            public override void initContent()
            {
                base.initContent();

                SNSDataManager.BuddyInfo buddyInfo = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);

                m_Component.BuddyName.text = buddyInfo.Info.szName;
            }

            public override void addPopupMenu()
            {
                if (m_Component.popupMenuHandle == null)
                {
                    return;
                }
                UPopupAction popupAction = new UPopupAction();

                DataCenter.SNSDataManager.BuddyInfo buddy = LogicDataCenter.snsDataManager.getBuddyInfoByUserID(m_Component.Item.nUserID);
				bool isSameWorld = LogicDataCenter.snsDataManager.isSameGameWorld(buddy.Info.nUserID);

				popupAction.Add(UPopupItemDataBuddyChat.GetDescString(), new UPopupItemDataBuddyChat(buddy.Info.nUserID));
				if(isSameWorld)
				{
					popupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataKinInvite.GetDescString(), new UPopupItemDataKinInvite((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataKinRequest.GetDescString(), new UPopupItemDataKinRequest((int)buddy.Info.dwPdbID));
					popupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(buddy.Info.szName));
				}
				popupAction.Add(UPopupItemDataAddBlackList.GetDescString(), new UPopupItemDataAddBlackList(buddy.Info.nUserID, buddy.Info.szName));
				popupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(buddy.Info.nUserID, buddy.Info.szName));

				m_Component.popupMenuHandle.HandlePopupAction = popupAction;
            }
        }

        // 最近游戏分组下的好友显示
        public class BuddyItemComponentLastPlay : BuddyItemComponentBase
        {
            public BuddyItemComponentLastPlay(UBuddyGroupTreeItemComponent component)
                : base(component)
            {
            }
        }
  }
}

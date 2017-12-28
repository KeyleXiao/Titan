/*******************************************************************
** 文件名:	SNSMainPanelView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/7
** 版  本:	1.0
** 描  述:	SNS系统主面板
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
using ASpeedGame.Data.ShieldWorld;
using USpeedUI.UExtensions;
using UnityEngine.EventSystems;

namespace USpeedUI.SNS
{
    [Serializable]
    public class UBuddyGroupTreeViewDataSource
    {
        /// <summary>
        /// The depth.
        /// </summary>
        [SerializeField]
        public int Depth;

        /// <summary>
        /// The is visible.
        /// </summary>
        [SerializeField]
        public bool IsVisible = true;

        /// <summary>
        /// The is expanded.
        /// </summary>
        [SerializeField]
        public bool IsExpanded;

        /// <summary>
        /// 好友头像.
        /// </summary>
        [SerializeField]
        public Sprite Icon;

        /// <summary>
        /// 分组名称和好友名称共用该字段
        /// </summary>
        [SerializeField]
        public string Name;

        /// <summary>
        /// The tag.
        /// </summary>
        [SerializeField]
        public object Tag;

        public int nUserID;

        public int nBuddyGroupID;
    }

    public enum EMSNS_CHANGE_NAME_TYPE
    {
        EMSNS_CHANGE_NAME_TYPE_NONE = 0,
        EMSNS_CHANGE_NAME_TYPE_BUDDY,
        EMSNS_CHANGE_NAME_TYPE_BUDDYGROUP,
    }

	public class SNSMainPanelWnd : UIPopupWnd<SNSMainPanelWndView>
	{
		public override WndID GetID()
		{
			return WndID.WND_ID_SNS_MAINPANEL;
		}

		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP02;
		}

		public override string GetPrefabPath()
		{
			return "SNS/SNSMainPanelWndView";
		}

		public override bool Init()
		{
			m_isKeyClose = true;

			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_MAINPANEL, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_SELF_STATUS, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_HIDE_SEARCH_BUDDY_WND, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNS_SWITCH_MAINPANEL, this);

			return base.Init();
		}

		public override void Destroy()
		{
			base.Destroy();
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SHOW_MAINPANEL, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_SELF_STATUS, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_HIDE_SEARCH_BUDDY_WND, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNS_SWITCH_MAINPANEL, this);
		}

		public override void OnMessage(WndMsgID msgID, UIMsgData data)
		{
			switch (msgID)
			{
				case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
					{
						// 进入登录界面,清理上一局数据
						if (m_wndView != null)
						{
							m_wndView.Clear();
						}

						UnloadView();
					}
					break;
				case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
					{
						//SetVisible(true);
					}
					break;
				case WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP:
					{
						if (m_wndView != null)
						{
							m_wndView.updateMainPanel();
						}
					}
					break;
				case WndMsgID.WND_MSG_SNS_SHOW_MAINPANEL:
					{
						SetVisible(true);
						if (m_wndView != null)
						{
							m_wndView.updateMainPanel();
						}
					}
					break;
				case WndMsgID.WND_MSG_SNS_UPDATE_SELF_STATUS:
					{
						if (m_wndView != null)
						{
							m_wndView.updateSelfStatus();
						}
					}
					break;
				case WndMsgID.WND_MSG_SNS_HIDE_SEARCH_BUDDY_WND:
					{
						if (m_wndView != null)
							m_wndView.closeSearchBuddyWnd();
					}
					break;
				case WndMsgID.WND_MSG_SNS_SWITCH_MAINPANEL:
					{
						if (m_wndView != null)
							m_wndView.switchMainPanel();
						else
						{
							// 第一次按快捷键显示
							SetVisible(true);
							m_wndView.updateMainPanel();
						}
					}
					break;
				default:
					break;
			}
		}
	}


	public class SNSMainPanelWndView : UIBaseWndView, IPointerClickHandler
    {
        public UBuddyGroupTreeList BuddyGroupTree;
        public Image MyAvatar;
        public Text MyName;
        public Image MyStateIcon;
        public Text MyStateText;
        public InputField MyMoodInputField;
        public SNSSettingWndView SettingWnd;
		public SearchBuddyInputField SearchBuddy;
		public SNSSearchBuddyWndView SearchBuddyView;

		private List<UBuddyGroupTreeViewDataSource> Data = new List<UBuddyGroupTreeViewDataSource>();

        private Image MoodBg = null;
        private UTooltipTrigger MoodTips = null;

        public override bool Init(IUIWnd wnd)
        {
			base.Init(wnd);

            BuddyGroupTree.DefaultColor = Color.white;
            BuddyGroupTree.SelectedColor = Color.white;
            BuddyGroupTree.HighlightedColor = Color.white;
            BuddyGroupTree.Start();

            SettingWnd.Init();

			SearchBuddy.onValueChanged.AddListener(onSearchBuddy);
			SearchBuddy.onSelectEvent = delegate(){ SearchBuddyView.gameObject.SetActive(true);};
			SearchBuddy.onDeselectEvent = delegate(){
				if(SearchBuddyView.IsPointerExit)
					SearchBuddyView.gameObject.SetActive(false);
			};

            MyMoodInputField.onEndEdit.AddListener(onMoodChange);
            MoodBg = MyMoodInputField.GetComponent<Image>();
            MoodTips = MyMoodInputField.GetComponent<UTooltipTrigger>();
            MoodTips.enabled = false;

            return true;
        }

		public void Clear()
		{

		}

        /// <summary>
        /// Convert flat list to tree.
        /// </summary>
        /// <param name="nodes">Nodes.</param>
        public virtual void List2Tree(ObservableList<TreeNode<UBuddyGroupTreeItemData>> nodes)
        {
            TreeNode<UBuddyGroupTreeItemData> last_node = null;
            for (int i = 0; i < Data.Count; i++)
            {
                var item = Data[i];
                item.IsVisible = true;

                //Debug.Log(item.Depth + " -> " + item.Name + " -> " + item.IsVisible);
                if (item.Depth == 0)
                {
                    last_node = Item2Node(item);
                    nodes.Add(last_node);
                }
                else if (item.Depth == (Data[i - 1].Depth + 1))
                {
                    var current_node = Item2Node(item);
                    last_node.Nodes.Add(current_node);

                    last_node = current_node;
                }
                else if (item.Depth <= Data[i - 1].Depth)
                {
                    int n = item.Depth - Data[i - 1].Depth + 1;
                    for (int j = 0; j < n; j++)
                    {
                        last_node = last_node.Parent;
                    }

                    var current_node = Item2Node(item);
                    last_node.Nodes.Add(current_node);

                    last_node = current_node;
                }
                else
                {
                    //Debug.LogWarning("Unknown case");
                }
            }
        }

        /// <summary>
        /// Convert item to node.
        /// </summary>
        /// <returns>The node.</returns>
        /// <param name="item">Item.</param>
        protected virtual TreeNode<UBuddyGroupTreeItemData> Item2Node(UBuddyGroupTreeViewDataSource item)
        {
            var nodeItem = new UBuddyGroupTreeItemData(item.Name, item.Icon);
            nodeItem.Tag = item.Tag;
            nodeItem.Name = item.Name;
            nodeItem.nUserID = item.nUserID;
            nodeItem.nBuddyGroupID = item.nBuddyGroupID;

            return new TreeNode<UBuddyGroupTreeItemData>(nodeItem, new ObservableList<TreeNode<UBuddyGroupTreeItemData>>(), item.IsExpanded, item.IsVisible);
        }

        /// <summary>
        /// 设置好友分组数据源
        /// </summary>
        public void setBuddyGroupTreeViewDataSource()
        {
            var nodes = new ObservableList<TreeNode<UBuddyGroupTreeItemData>>();
            List2Tree(nodes);
            BuddyGroupTree.Nodes = nodes;
        }

		/// <summary>
		/// 切换显示主面板
		/// </summary>
		public void switchMainPanel()
		{

			bool bActive = gameObject.activeSelf;
			gameObject.SetActive(!bActive);

			if (!bActive)
			{
				updateMainPanel();
			}
		}

		/// <summary>
		/// 显示主面板
		/// </summary>
		public void updateMainPanel()
        {
            // 设置好友列表内容
            Data.Clear();

            List<SNSDataManager.BuddyInfo> buddyList = null;
            bool isWarScene = GameLogicAPI.isInWarScene() > 0;
            foreach (var buddyGroup in LogicDataCenter.snsDataManager.getBuddyGroupList())
            {
                // 战场中不显示附近的人分组信息
                if (isWarScene && buddyGroup.Info.nBuddyGroupID == (int)EMSNS_BUDDYGROUP_ID.EMSNS_BUDDYGROUP_ID_NEARBY)
                {
                    continue;
                }

                // 添加一个好友分组
                UBuddyGroupTreeViewDataSource data = new UBuddyGroupTreeViewDataSource();
                data.nBuddyGroupID = buddyGroup.Info.nBuddyGroupID;
                data.Depth = 0;
                data.IsExpanded = buddyGroup.isExpanded;
                
                Data.Add(data);

                // 添加该好友分组下的好友
                buddyList = LogicDataCenter.snsDataManager.getBuddyListByBuddyGroupID(buddyGroup.Info.nBuddyGroupID);
                foreach (var buddy in buddyList)
                {
                    data = new UBuddyGroupTreeViewDataSource();
                    data.nUserID = buddy.Info.nUserID;
                    data.Depth = 1;

                    Data.Add(data);
                }
            }

            setBuddyGroupTreeViewDataSource();

            // 设置自己的信息
            DataCenter.SNSDataManager.BuddyInfo myInfo = LogicDataCenter.snsDataManager.MyBuddyInfo;
            MyAvatar.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_SNS_MAINPANEL, 1, 1);
            MyName.text = myInfo.Info.szName;
            MyStateText.text = LogicDataCenter.snsDataManager.getSelfStatusDesc();
            MyStateIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GameState, WndID.WND_ID_SNS_MAINPANEL,
                UDefines.GetGameStateIconID(myInfo.Info.nStatus));

            if(String.IsNullOrEmpty(myInfo.Info.szMood) == false)
            {
                MyMoodInputField.text = myInfo.Info.szMood;
                if(MoodTips != null)
                {
                    MoodTips.SetText(UTooltipParamName.BodyText, myInfo.Info.szMood);
                    MoodTips.tipPosition = TipPosition.MouseTopRightCorner;
                    MoodTips.enabled = true;
                }
            }
        }
        
        public void openFindBuddyWnd()
        {
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_FINDBUDDY_WND, null);
        }

        // 打开设置窗口
        public void openSettingWnd()
        {
            SettingWnd.gameObject.SetActive(true);
            SettingWnd.initSetting();
        }

		// 保存设置
		public void saveSetting()
		{
			SettingWnd.gameObject.SetActive(false);

			SettingWnd.savePermission();
		}

		// 关闭设置窗口
		public void closeSettingWnd()
        {
            SettingWnd.gameObject.SetActive(false);
		}

        // 更新自己的状态描述
        public void updateSelfStatus()
        {
            MyStateText.text = LogicDataCenter.snsDataManager.getSelfStatusDesc();
        }

		// 搜索好友内容改变
		public void onSearchBuddy(string str)
		{
			if(string.IsNullOrEmpty(str))
			{
				SearchBuddyView.gameObject.SetActive(false);
				return;
			}
			
			if(!SearchBuddyView.gameObject.activeSelf)
			{
				SearchBuddyView.gameObject.SetActive(true);
			}

			SearchBuddyView.onSearchBuddy(str);
		}

		// 关闭搜索好友结果窗口
		public void closeSearchBuddyWnd()
		{
			SearchBuddyView.gameObject.SetActive(false);
		}

        // 选中心情
        private void onMoodFocus(bool bFocus)
        {
            if(bFocus)
            {
                if(MoodBg != null && MoodBg.enabled == false)
                {
                    MoodBg.enabled = true;
                }
            }
            else
            {
                if (MoodBg != null && MoodBg.enabled == true)
                {
                    MoodBg.enabled = false;
                }
            }
        }
        // 修改心情
        private void onMoodChange(string strText)
        {
            if(LogicDataCenter.snsDataManager.MyBuddyInfo.Info.szMood == strText)
            {
                return;
            }

            // 判断屏蔽词
            if(ShieldWord.Instance.filterShieldWord(ref strText))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, "输入内容含有屏蔽词");
                MyMoodInputField.text = LogicDataCenter.snsDataManager.MyBuddyInfo.Info.szMood;
                return;
            }

            LogicDataCenter.snsDataManager.reqChangeMood(strText);

            onMoodFocus(false);
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            Vector2 mousePos = UISystem.Instance.GetMousePos();
            RectTransform rectTrans = MyMoodInputField.gameObject.GetComponent<RectTransform>();
            if (RectTransformUtility.RectangleContainsScreenPoint(rectTrans, mousePos, UISystem.Instance.GetCamera()))
            {
                onMoodFocus(true);
            }
            else
            {
                onMoodFocus(false);
            }
        }
    }

#region 设置窗口
    public class SNSSettingWndView : MonoBehaviour
    {
        public GameObject DefaultToggle;

        private Dictionary<int, string> m_PermissionList;

        private Dictionary<int, Toggle> m_PermissionGroup;

        private int m_nTempPermission;

        public void Init()
        {
            m_PermissionList = new Dictionary<int, string>{
                {(int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_FILTER_ALL_REQ, ULocalizationService.Instance.Get("UIView", "SNS", "FilterBuddyReq")},
                {(int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_FILTER_ALL_MSG, ULocalizationService.Instance.Get("UIView", "SNS", "FilterBuddyMsg")},
                {(int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_FILTER_TEMP_SESSION, ULocalizationService.Instance.Get("UIView", "SNS", "FilterTempSession")},
                {(int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_DONT_SHOW_OFFLINE_BUDDY, ULocalizationService.Instance.Get("UIView", "SNS", "DontShowOfflineBuddy")},
            };

            m_PermissionGroup = new Dictionary<int, Toggle>();
            foreach (var item in m_PermissionList)
            {
                GameObject obj = ResNode.InstantiateRes(DefaultToggle);
                if (obj == null)
                {
                    return;
                }

                obj.name = item.Key.ToString();
                obj.transform.SetParent(DefaultToggle.transform.parent, false);
                obj.transform.FindChild("Label").GetComponent<Text>().text = item.Value;
                obj.SetActive(true);

                Toggle tg = obj.GetComponent<Toggle>();

                m_PermissionGroup.Add(item.Key, tg);

                // 设置复选框状态变化回调
                tg.onValueChanged.AddListener(delegate(bool check)
                {
                    this.onValueChange(check, obj);
                });
            }
        }

        public void onValueChange(bool check, GameObject obj)
        {
            int nPermissionID = int.Parse(obj.name);
            if (check)
            {
                // 添加权限
                m_nTempPermission |= (1 << nPermissionID);
            }
            else
            {
                // 删除权限
                int tmpMask = (1 << nPermissionID);
                tmpMask ^= ~0;
                m_nTempPermission &= tmpMask;
            }


        }

        public void savePermission()
        {
            LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nPermission = m_nTempPermission;

            LogicDataCenter.snsDataManager.reqChangePermission();

            // 是否不显示离线好友
            LogicDataCenter.snsDataManager.SettingMgr.isDontShowOffline = (m_nTempPermission & (1 << (int)EMSNS_PERMISSION_ID.EMSNS_PERMISSION_ID_DONT_SHOW_OFFLINE_BUDDY)) > 0;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_UPDATE_BUDDYGROUP, null);
        }

        public void initSetting()
        {
            m_nTempPermission = LogicDataCenter.snsDataManager.MyBuddyInfo.Info.nPermission;
            foreach (var item in m_PermissionGroup)
            {
                if ((m_nTempPermission & (1 << item.Key)) > 0)
                {
                    item.Value.isOn = true;
                }
                else
                {
                    item.Value.isOn = false;
                }
            }
        }
    }
#endregion
}

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

    public class SNSMainPanelWndView : MonoBehaviour
    {
        public UBuddyGroupTreeList BuddyGroupTree;
        public GameObject FindBuddyWnd;
        public Image MyAvatar;
        public Text MyName;
        public Text MyStatus;
        public SNSSettingWndView SettingWnd;
		public SearchBuddyInputField SearchBuddy;
		public SNSSearchBuddyWndView SearchBuddyView;

		private List<UBuddyGroupTreeViewDataSource> Data = new List<UBuddyGroupTreeViewDataSource>();

        public bool Init()
        {

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

			return true;
        }

		public void Update()
		{
			if(SearchBuddy.isFocused)
			{

			}
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
        /// 显示主面板
        /// </summary>
        public void updateMainPanel()
        {
            // 设置好友列表内容
            Data.Clear();
            foreach (var buddyGroup in LogicDataCenter.snsDataManager.getBuddyGroupList())
            {
                // 添加一个好友分组
                UBuddyGroupTreeViewDataSource data = new UBuddyGroupTreeViewDataSource();
                data.nBuddyGroupID = buddyGroup.Info.nBuddyGroupID;
                data.Depth = 0;
                data.IsExpanded = buddyGroup.isExpanded;
                
                Data.Add(data);

                // 添加该好友分组下的好友
                foreach (var buddy in LogicDataCenter.snsDataManager.getBuddyListByBuddyGroupID(buddyGroup.Info.nBuddyGroupID))
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
            MyAvatar.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_PLAYERFRAME, 2, 1);
            MyName.text = myInfo.Info.szName;
            MyStatus.text = LogicDataCenter.snsDataManager.getSelfStatusDesc();
        }
        
        public void openFindBuddyWnd()
        {
            if (FindBuddyWnd != null)
            {
                FindBuddyWnd.SetActive(true);
            }
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
            MyStatus.text = LogicDataCenter.snsDataManager.getSelfStatusDesc();
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

	}

#region 设置窗口
    public class SNSSettingWndView : MonoBehaviour
    {
        public GameObject DefaultToggle;

        private Dictionary<int, string> m_PermissionList;

        private Dictionary<int, Toggle> m_PermissionGroup;

        private bool m_bInit = false;

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
            //if (!m_bInit)
            //{
                //return;
            //}

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
            //if (m_bInit)
            //{
                //return;
            //}

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

            m_bInit = true;
        }
    }
#endregion
}

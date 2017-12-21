/*******************************************************************
** 文件名:	GameHallViewWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-12
** 版  本:	1.0
** 描  述:	天赋页界面视图模块
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.EventSystems;
using UnityEngine.Events;
using System;
using GameLogic;
using DataCenter;
using USpeedUI.GameHallPart;
using UIWidgets;
using USpeedUI.UExtensions;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgetsSamples;
using ASpeedGame.Data.CreateMatchRoomCustomer;
using System.Linq;
using ASpeedGame.Data.ShieldWorld;
using USpeedUI.UEffect;

namespace USpeedUI
{
    public class GameCreateRoomMapButtonItem : MonoBehaviour
    {
        private int m_nID;
        public Text MapButtonText;

        public void SetCreateRoomMapModeInfo(int nID, string sMapName, bool bSelect)
        {
            m_nID = nID;
            MapButtonText.text = sMapName;
            this.gameObject.GetComponent<UButtonGroupItem>().enabled = true;
            this.gameObject.GetComponent<UButtonGroupItem>().Selected = bSelect;
            this.gameObject.SetActive(true);
        }

        public void OnCreateRoomMapButtonClick(GameCreateRoomMapButtonItem mapButtonItem)
        {
            if (mapButtonItem.gameObject.GetComponent<UButtonGroupItem>().Selected)
            {
                UIMsgCmdData uiMapChangeInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK, mapButtonItem.m_nID, string.Empty, IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_GROUP_MAPBTNCLICK, uiMapChangeInfo);
            }
        }
    }

    public class GameCreateRoomView : MonoBehaviour
    {
        public Image MapImage;
        public Text MapDesc;
        public UButtonGroup MapButtonGroup;
        public InputField RoomNameText;
        public InputField RoomPassWord;
        private const int m_nMaxMapCount = 20;
        private GameCreateRoomMapButtonItem[] aMapButtonArray = new GameCreateRoomMapButtonItem[m_nMaxMapCount];
        private int m_CurPersonCountIndex = 0;
        private int m_CurSelectMatchType = 0;
        private Hashtable m_CurMatchTypeInfo = new Hashtable();
        public ComboboxSampleListView PersonCountComboboxList;
        private int CreateRoomNameLen = 32;
        private int CreateRoomPasswordLen = 32;

        private ResNode MapItemPrefabRes = null;

        public Dropdown PermitObDd;

        public void onPersonCountPageIndexChange(int nPageIndex, ListViewItem component)
        {
            if (component == null)
            {
                return;
            }
            m_CurPersonCountIndex = nPageIndex;
            ComboboxSampleListViewData comBoxData = PersonCountComboboxList.DataSource[nPageIndex];
            if (!m_CurMatchTypeInfo.ContainsKey(comBoxData.ComboboxText))
            {
                return;
            }

            m_CurSelectMatchType = (int)m_CurMatchTypeInfo[comBoxData.ComboboxText];
        }

        public void SetCreateRoomViewVisible(bool bShow)
        {
            this.gameObject.SetActive(bShow);
            if (bShow && EntityFactory.MainHeroView != null)
            {
                CreatureProperty property = EntityFactory.MainHeroView.Property;
                if (property != null)
                {
                    RoomNameText.text = property.CreatureName;
                }
            }
        }

        public void InitalCustomerRoom()
        {
            RoomPassWord.text = string.Empty;
            
            // 获取自定义房间配置相关
            int nIndex = 0;
            foreach (DictionaryEntry Info in MatchRoomCustomer.Instance.allMatchRoomCustomerTable)
            {
                bool bDefaultSelect = false;
                SSchemeMatchRoomCustomer sInfo = Info.Value as SSchemeMatchRoomCustomer;
                if (sInfo.bIsOpen == false)
                {
                    continue;
                }
                if (IsOutOfMapButtonArray(nIndex))
                {
                    return;
                }

                if (nIndex == 0)
                {
                    bDefaultSelect = true;
                }

                if (aMapButtonArray[nIndex] != null)
                {
                    
                    aMapButtonArray[nIndex].SetCreateRoomMapModeInfo(sInfo.nID, sInfo.MapDes, bDefaultSelect);
                    if (bDefaultSelect)
                    {
                        SetCreateRoomMapDesc(sInfo.MapDetailDes, sInfo.PictrueID);
                        UpdateComboBox(sInfo);
                    }
                    return;
                }

                MapItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/GameHallSelectRoom/MapButton");
                if(null == MapItemPrefabRes)
                {
                    Debug.Log("TeamItemPrefab Is Null");
                    return;
                }

                GameObject MapItemObject = MapItemPrefabRes.InstanceMainRes();
                if (MapItemObject == null)
                {
                    return;
                }

                //添加音效
                MapItemObject.AddUISoundTrigger(EventTriggerType.PointerEnter, "Common", "Button_common_slide");
                MapItemObject.AddUISoundTrigger(EventTriggerType.PointerClick, "Common", "Button_common_click");

                // 地图信息按钮
                GameCreateRoomMapButtonItem item = MapItemObject.GetComponent<GameCreateRoomMapButtonItem>();
                if (item == null)
                {
                    return;
                }

                item.SetCreateRoomMapModeInfo(sInfo.nID, sInfo.MapDes, bDefaultSelect);
                aMapButtonArray[nIndex] = item;

                UButtonGroupItem itemGroup = MapItemObject.GetComponent<UButtonGroupItem>();
                if (itemGroup == null || MapButtonGroup == null)
                {
                    return;
                }

                MapItemObject.transform.SetParent(MapButtonGroup.transform);
                MapItemObject.transform.localScale = new Vector3(1, 1, 1);
                MapItemObject.transform.localPosition = new Vector3(0, 0, 0);
                itemGroup.Group = MapButtonGroup;
                // 地图信息对战人数下拉列表
                if (bDefaultSelect)
                {
                    SetCreateRoomMapDesc(sInfo.MapDetailDes, sInfo.PictrueID);
                    UpdateComboBox(sInfo);
                }

                ++nIndex;
            }
            MapButtonGroup.SetItemSelectedItem(0);

            PermitObDd.options.Clear();
            PermitObDd.options.Add(new Dropdown.OptionData("允许"));
            PermitObDd.options.Add(new Dropdown.OptionData("拒绝"));
            PermitObDd.captionText.text = PermitObDd.options.First().text;
        }

        public void ClearCustomerRoom()
        {
            AssetBundleManager.DeleteAssets(ref MapItemPrefabRes, true);
        }

        public void GroupMapBtnClick(int nID)
        {
            if (!MatchRoomCustomer.Instance.allMatchRoomCustomerTable.ContainsKey(nID))
            {
                return;
            }

            SSchemeMatchRoomCustomer sInfo = MatchRoomCustomer.Instance.allMatchRoomCustomerTable[nID] as SSchemeMatchRoomCustomer;
            SetCreateRoomMapDesc(sInfo.MapDetailDes, sInfo.PictrueID);

            // 更新地图信息对战人数下拉列表
            UpdateComboBox(sInfo);

        }

        
        public string getRoomName()
        {
            return RoomNameText.text;
        }

        public string getPassword()
        {
            return RoomPassWord.text;
        }

        public int getCurSelectMatchType()
        {
            return m_CurSelectMatchType;
        }

        public void UpdateComboBox( SSchemeMatchRoomCustomer sInfo)
        {
            string[] PersonCountArrayStr = sInfo.strPersonCountListDes.Split(';');
            string[] MatchTypeList = sInfo.strMatchTypeList.Split(';');
            if (MatchTypeList.Length != PersonCountArrayStr.Length)
            {
                Trace.LogError("配置出错 请检查 MatchRoomCustomer.csv!!!");
            }
            m_CurMatchTypeInfo.Clear();
            
            PersonCountComboboxList.DataSource.Clear();
            PersonCountComboboxList.OnSelect.RemoveListener(onPersonCountPageIndexChange);
            for (int i = 0; i < MatchTypeList.Length; ++i)
            {
                ComboboxSampleListViewData comBoxData = new ComboboxSampleListViewData();
                comBoxData.ComboboxText = PersonCountArrayStr[i];
                m_CurMatchTypeInfo[PersonCountArrayStr[i]] = Convert.ToInt32(MatchTypeList[i]);
                PersonCountComboboxList.Add(comBoxData);
            }
            PersonCountComboboxList.OnSelect.AddListener(onPersonCountPageIndexChange);
            PersonCountComboboxList.Select(0);
        }

        public void SetCreateRoomMapDesc(string sMapDesc, int nMapIconID)
        {
            MapDesc.text = sMapDesc;
            MapImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_StaticState, WndID.WND_ID_HALLROOMINFOVIEW, nMapIconID);
        }

        public bool IsOutOfMapButtonArray(int nIndex)
        {
            if (nIndex < 0 || nIndex >= aMapButtonArray.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetCreateRoomMapModeInfo(int nIndex, int nMapID, string sMapName, bool bSelect, int nWarID)
        {
            
        }

        public bool CheckCreateRoomName()
        {

            if (RoomNameText.text.Length <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_ROOMNAME_IS_EMPTY);
                return false;
            }

            if (RoomNameText.text.Length > CreateRoomNameLen)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_ROOMNAME_LEN_OVER);
                return false;
            }

            string RoomName = RoomNameText.text;
            if (ShieldWord.Instance.filterShieldWordOfName(ref RoomName))
            {
                RoomNameText.text = RoomName;
                return false;
            }

            return true;
        }

        public bool CheckCreateRoomPassword()
        {
            if (RoomPassWord.text.Length <= 0)
            {
                return true;
            }

            if (RoomNameText.text.Length > CreateRoomPasswordLen)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_PASSWORD_LEN_OVER);
                return false;
            }

            if (ShieldWord.Instance.strHaveCN(RoomPassWord.text))
            {
                RoomPassWord.text = string.Empty;
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_CN);
                return false;
            }
            if (ShieldWord.Instance.IsHaveShieldWord(RoomPassWord.text))
            {
                RoomPassWord.text = string.Empty;
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_SHEILD);
                return false;
            }

            return true;
        }

        public Byte getPermitOb()
        {
            return (Byte)(PermitObDd.value == 0 ? 1 : 0);
        }
    }

    public class GamePasssWordView : MonoBehaviour
    {
        public Text PasswordTitle;
        public InputField PassWord;
        public Button SureButton;
        public Text RoomName;
        public Text PasswordRoomDes;

        public virtual void Awake()
        {
            PasswordTitle.text = ULocalizationService.Instance.Get("UIView", "HallRoom", "PasswordTitle");
            PasswordRoomDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "HallRoom", "PasswordRoomDes"));
        }

        public void OnCancelBtnClick()
        {
            this.SetPasssWordViewVisible(false);
        }

        public void SetPasssWordViewVisible(bool bShow)
        {
            this.gameObject.SetActive(bShow);

            if (bShow)
            {
                InputField inputfield = PassWord.GetComponent<InputField>();
                if (inputfield != null)
                {
                    EventSystem.current.SetSelectedGameObject(PassWord.gameObject, new BaseEventData(EventSystem.current));
                }
            }
        }

        public void SetSetPasssWordViewRoomName(string sNameRoom)
        {
            RoomName.text = sNameRoom;
        }

        public string getViewRoomPasssWord()
        {
            return PassWord.text;
        }

        public void clearPasssWordText()
        {
            PassWord.text = string.Empty;
        }

        public void OnEnterRoomPasswordEndEidt()
        {
            if (ShieldWord.Instance.strHaveCN(PassWord.text))
            {
                PassWord.text = string.Empty;
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_CN);
                return;
            }

            if (ShieldWord.Instance.IsHaveShieldWord(PassWord.text) )
            {
                PassWord.text = string.Empty;
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_SHEILD);
                return;
            }
        }
    }

    public class GameHallWndView : UIBaseWndView
    {
        GameHallWnd GameHallViewWnd;
        public UListHall ListHall;
        public GameCreateRoomView CreateRoomView;
        public GamePasssWordView PassWordView;
        public InputField SerchInfo;
        public Image BackgroundEffectImage;
        public Text RoomTitle;
        public Text RoomDesc;


        private UIWidgets.ObservableList<UListItemHallData> AllDataSource = new UIWidgets.ObservableList<UListItemHallData>();

        private int m_CurSelectRoomID = -1;
        private UListItemHallData m_CurHallItem;
        
        public override void Start()
        {
            if (m_wnd == null)//测试用
            {
            }
            else
            {
                GameHallViewWnd = (GameHallWnd)m_wnd;
            }
            ClearRoomListData();
            ListHall.OnSelect.AddListener(OnSelectRoomClick);
            ListHall.onDoubleClick.AddListener(OnAddRoomDoubleClick);
            PassWordView.SureButton.onClick.AddListener(OnPassWordSureBtnClick);

            UEffectParamBase materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, BackgroundEffectImage, true);
            if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref materialEffectParam))
            {
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.20f));
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_Vibrancy", 0f);
            }

            RoomTitle.text = ULocalizationService.Instance.Get("UIView", "HallRoom", "RoomTitle");
            RoomDesc.text = ULocalizationService.Instance.Get("UIView", "HallRoom", "RoomDesc");

            return;
        }

        public override void Destroy()
        {
            base.Destroy();
            CreateRoomView.ClearCustomerRoom();
        }

        public override void Update()
        {
            base.Update();
        }

        public void SetRoomListData(IntPtr ptrParam, int nPtrLen)
        {
            
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_match_hall_rooms_head head = helper.get<GameLogic.cmd_match_hall_rooms_head>();

            for (int i = 0; i < head.nRoomCount; ++i)
            {
                GameLogic.cmd_match_hall_add_room cmdNodeData = helper.get<GameLogic.cmd_match_hall_add_room>();

                UListItemHallData hallData = new UListItemHallData();
                hallData.nRoomIndex = cmdNodeData.nRoomIndex;
                hallData.nRoomId = cmdNodeData.nRoomIndex;
                hallData.RoomName = cmdNodeData.szRoomName;
                hallData.GameMap = cmdNodeData.szMapDes;
                hallData.nCurPlayerCount = cmdNodeData.nCurPlayerNum;
                hallData.nTotalPlayerCount = cmdNodeData.nMaxPlayerNum;
                hallData.bLock = cmdNodeData.bLock > 0 ? true : false;
                ListHall.DataSource.Add(hallData);
                UListItemHallData hallDataCpy = hallData;

                AllDataSource.Add(hallDataCpy);
            }

            if (m_CurSelectRoomID > 0)
            {
                for (int i = 0; i < ListHall.DataSource.Count; i++)
                {
                    UListItemHallData temp = ListHall.DataSource[i];
                    if (temp.nRoomId == m_CurSelectRoomID)
                    {
                        ListHall.Select(i);
                        break;
                    }
                }

                
            }
            else
            {
                // 默认选中一个无密码、人数不足的房间
                for (int i = 0; i < ListHall.DataSource.Count; i++)
                {
                    UListItemHallData temp = ListHall.DataSource[i];
                    if (temp.nCurPlayerCount < temp.nTotalPlayerCount && !temp.bLock)
                    {
                        m_CurHallItem = temp;
                        m_CurSelectRoomID = temp.nRoomId;
                        ListHall.Select(i);
                        break;
                    }
                }
            }
        }


        public void RefreshRoomListData(cmd_match_hall_add_room data)
        {
            if (ListHall.DataSource.Count == 0)
            {
                return;
            }

            ListHall.DataSource.BeginUpdate();
            foreach (UListItemHallData Item in ListHall.DataSource)
            {
                if (data.nRoomIndex == Item.nRoomIndex)
                {
                    Item.nCurPlayerCount = data.nCurPlayerNum;
                }
            }
            ListHall.DataSource.EndUpdate();
            ListHall.UpdateVisible();
        }

        public void ClearRoomListData()
        {
            ListHall.DataSource.Clear();
            AllDataSource.Clear();
            m_CurHallItem = null;
            m_CurSelectRoomID = -1;
        }

        public void RemoveRoomListDataItem(int nRoomId)
        {
            foreach (UListItemHallData Item in ListHall.DataSource)
            {
                if (nRoomId == Item.nRoomIndex)
                {
                    ListHall.DataSource.Remove(Item);
                    break;
                }
            }

            foreach (UListItemHallData Item in AllDataSource)
            {
                if (nRoomId == Item.nRoomIndex)
                {
                    AllDataSource.Remove(Item);
                    break;
                }
            }
        }

        public void OnInputFieldValueChange()
        {
            // 查询数据
            if (SerchInfo == null || AllDataSource == null)
            {
                return;
            }

            if (SerchInfo.text == string.Empty)
            {
                // 所有数据显示
                ListHall.DataSource = AllDataSource;
            }
            else
            {
                string strValue = SerchInfo.text;
                // 显示过滤的数据
                IEnumerable<UListItemHallData> ListQuery = from item in AllDataSource
                                                           where item.nRoomIndex == Convert.ToInt32(strValue)
                                                           select item;

                UIWidgets.ObservableList<UListItemHallData> dataSource = new UIWidgets.ObservableList<UListItemHallData>();
                foreach (UListItemHallData Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                ListHall.DataSource = dataSource;
            }

        }

        public void OnRefreshButtonClick()
        {
            UIWidgets.ObservableList<UListItemHallData> dataSource = new UIWidgets.ObservableList<UListItemHallData>();
            foreach (UListItemHallData Item in AllDataSource)
            {
                dataSource.Add(Item);
            }

            ListHall.DataSource.Clear();
            ListHall.DataSource.BeginUpdate();
            ListHall.DataSource = dataSource;
            ListHall.DataSource.EndUpdate();

            m_CurHallItem = null;
            m_CurSelectRoomID = -1;
        }

        public void OnCreateSureButtonClickInHall()
        {
            if (PassWordView == null)
            {
                return;
            }

            if (PassWordView.gameObject.activeSelf)
            {
                return;
            }

            GameHallViewWnd.SendOpenCreateRoomMessage();
        }

        public void OnAddButtonClickInHall()
        {
            if (GameHallViewWnd == null || CreateRoomView.gameObject.activeSelf)
            {
                return;
            }

            // 失去选中框时，默认选择一个可进的，如果没有引导创建房间
            if (m_CurHallItem == null)
            {
                // 默认选中一个无密码、人数不足的房间
                for (int i = 0; i < ListHall.DataSource.Count; i++)
                {
                    UListItemHallData temp = ListHall.DataSource[i];
                    if (temp.nCurPlayerCount < temp.nTotalPlayerCount && !temp.bLock)
                    {
                        m_CurHallItem = temp;
                        m_CurSelectRoomID = temp.nRoomId;
                        ListHall.Select(i);
                        break;
                    }
                }

                if (m_CurHallItem == null)
                {
                    TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "Common", "TipText"),
                        message: ULocalizationService.Instance.Get("UIView", "HallRoom", "CreateTipContent"),
                        buttons: new UIWidgets.DialogActions() 
                                    { { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfirmCreateRoom }, 
                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } }
                        );
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

                    return;
                }
            }

            if (m_CurHallItem.bLock)
            {
                PassWordView.SetPasssWordViewVisible(true);
                PassWordView.SetSetPasssWordViewRoomName(m_CurHallItem.RoomName);
                
            }
            else
            {
                OnGameViewEvent_MatchEnterRoom();
            }
        }

        private bool ConfirmCreateRoom()
        {
            OnCreateSureButtonClickInHall();
            return true;
        }

        public void OnSelectRoomClick(int nIndex, ListViewItem item)
        {
            if (PassWordView == null)
            {
                return;
            }

            m_CurHallItem = ListHall.DataSource[nIndex];
            m_CurSelectRoomID = m_CurHallItem.nRoomId;
        }

        // 双击待控件完善
        public void OnAddRoomDoubleClick(int nIndex, ListViewItem item)
        {
            OnSelectRoomClick(nIndex, item);
            OnAddButtonClickInHall();
        }

        // 这个目前不需要,后续有需要再添加
        public void OnFreshRoomClick()
        {

        }

        public void OnReturnBtnClick()
        {
            if (GameHallViewWnd == null)
            {
                return;
            }

            UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, true);
            GameHallViewWnd.SetVisible(false);
        }

        private void OnGameViewEvent_MatchEnterRoom()
        {
            if (m_CurSelectRoomID < 0)
            {
                return;
            }

            if (GameHallViewWnd == null)
            {
                return;
            }

            GameHallViewWnd.SendAddRoomIDMessage(m_CurSelectRoomID, PassWordView.getViewRoomPasssWord());
        }

        public void OnPassWordSureBtnClick()
        {
            if (PassWordView == null)
            {
                return;
            }

            if (GameHallViewWnd == null)
            {
                return;
            }

            // 把密码发送到服务器效验,现在直接进入，后面功能齐了再改
            OnGameViewEvent_MatchEnterRoom();
            PassWordView.SetPasssWordViewVisible(false);
            PassWordView.clearPasssWordText();
        }


        public void OnCreateSureClickInCreateRoom()
        {
            if (GameHallViewWnd == null)
            {
                return;

            }

            if(CreateRoomView == null)
            {
                return;
            }

            if (!CreateRoomView.CheckCreateRoomName())
            {
                return;
            }

            if (!CreateRoomView.CheckCreateRoomPassword())
            {
                return;
            }

            CreateRoomView.SetCreateRoomViewVisible(false);
            GameHallViewWnd.SendCreateRoomMessage(CreateRoomView.getCurSelectMatchType(), CreateRoomView.getRoomName(), CreateRoomView.getPassword(), CreateRoomView.getPermitOb());
		}

        public void onMatchBtnClick()
        {

        }

        public void onCancelMatchBtnClick()
        {

        }

        public void SetCreateRoomShow()
        {
            if (CreateRoomView == null)
            {
                return;
            }

            CreateRoomView.SetCreateRoomViewVisible(true);

            //初始化自定义房间界面数据
            CreateRoomView.InitalCustomerRoom();
        }
        // 这块待开发，新界面无用
        public void SetCreateRoomGameModeInfo(int nIndex, int nModeID, string sModeName, bool bSelect)
        {
 
        }

        public void SetCreateRoomMapModeInfo(int nIndex, int nMapID, string sMapName, bool bSelect, int nWarID)
        {
            CreateRoomView.SetCreateRoomMapModeInfo(nIndex, nMapID, sMapName, bSelect, nWarID);
        }

        public void SetCreateRoomMapSelectInfo(string sMapDesc, int nMapIconID)
        {
            if (CreateRoomView == null)
            {
                return;
            }

            CreateRoomView.SetCreateRoomMapDesc(sMapDesc, nMapIconID);
        }

        public void GroupMapBtnClick(int nID)
        {
            CreateRoomView.GroupMapBtnClick(nID);
        }

    }
}
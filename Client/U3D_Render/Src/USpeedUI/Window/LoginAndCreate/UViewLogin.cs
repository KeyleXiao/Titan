using ASpeedGame.Data.GameSettingConfig;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UWidgets;
using UWidgets.WebBrowser;


namespace USpeedUI.LoginAndCreate
{
    public class UMediatorLogin : UIBaseWnd<UViewLogin>
    {
        #region 重载部分
        public override WndID GetID()
        {
            return WndID.WND_ID_LOGIN;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "Login/LoginView";
        }

        public override bool Init()
        {
            //订阅进入登录状态
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            //订阅离开登录状态
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            //订阅创建角色界面返回
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_FALLBACK, this);
            //订阅显示创建英雄界面
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_OPEN, this); 
            //订阅更新服务器信息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_UPDATE_SERVERINFO, this);
            //订阅接收大区列表信息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RECV_DISTRICTLIST, this); 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RECV_LASTENTER_DISTRICTLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, this);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_REQUEST_DISTRICT_LIST);

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey1, KeyStatus.Up, this);
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey2, KeyStatus.Up, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_FALLBACK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_UPDATE_SERVERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RECV_DISTRICTLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RECV_LASTENTER_DISTRICTLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey1, KeyStatus.Up, this);
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.ChatBoxActiveKey2, KeyStatus.Up, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        SetVisible(true);
                        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST);
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_FALLBACK:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_OPEN:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_UPDATE_SERVERINFO:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.UpdateServerInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_RECV_DISTRICTLIST:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.UpdateDistrictList();
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_RECV_LASTENTER_DISTRICTLIST:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.UpdateLastEnterDistrictList();
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.WebBrowserOpen = (data as UIMsgCmdData).nParam > 0;
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null)
                return false;

            if(status == KeyStatus.Up && (index == FuntionShortCutSlotIndex.ChatBoxActiveKey1 || index == FuntionShortCutSlotIndex.ChatBoxActiveKey2))
            {
                m_wndView.SelectableNavigation(false);
            }

            return base.OnKeyMessage(index, status);
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if(_bVisible)
                SoundManager.CreateUISound((int)SoundIDIndex.MapLogin);
            else
                SoundManager.DeleteSound((int)SoundIDIndex.MapLogin);
        }
        #endregion


        internal void SendLoginData(gameview_login voLogin)
        {
            ViewEventHelper.Instance.SendCommand<gameview_login>(GameLogicDef.GVIEWCMD_USER_LOGIN, ref voLogin);
        }
        private bool ConfigBtnClick()
        {
            Debug.Log("ConfigBtnClick");
            return true;
        }
        internal void SendChangeDistrictData(gameview_select_server info)
        {
            ViewEventHelper.Instance.SendCommand<gameview_select_server>(GameLogicDef.GVIEWCMD_REQUEST_SELECT_SERVER, ref info);
        }
    }

    public class UViewLogin : UIBaseWndView
    {
        protected UMediatorLogin mediator;
        public UInputFieldCombobox UserNameIFCombobox;
        public UInputFieldCombobox IPAddressIFCombobox;
        public GameObject DefaultSelected;
        public Text DistrictTitle;
        public Text CapsLockTip;
        public Transform PasswordTI;
        public Button LoginBtn;

        public GameObject DistrictFrame;
        public Text CurrentServerName;
        public GameObject State_Green;
        public GameObject State_Red;
        public UButtonGroup ServerList;
        public UButtonGroup LastEnterServerList;
        public GameObject DefaultServerItem;
        public GameObject DefaultLastServerItem;
        public InputField DistrictSearchInputField;

        private Dictionary<int, List<gamelogic_district_info>> m_districtMap;
        private List<gamelogic_district_info> m_lastEnterDistrictList;
        private gamelogic_district_info m_serverInfo;
        private int m_CurrentSelectGroup;
        private gameview_select_server tempServer;
        public bool WebBrowserOpen { get;set; }
        private bool bWebBrowserOpen;

        //注册网址，默认是内网
        private string m_strRegisterURL = "http://passport.szg1a.net/zhuce.html";

        private List<String> mUserNameList;

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true, CallingConvention = CallingConvention.Winapi)]
        public static extern short GetKeyState(int keyCode);

        public override bool Init(IUIWnd wnd)
        {
            m_CurrentSelectGroup = 0;
            tempServer = new gameview_select_server();

            mediator = wnd as UMediatorLogin;

            if (UserNameIFCombobox == null)
                UserNameIFCombobox = GetTransform().GetComponentInChildren<UInputFieldCombobox>();

            DistrictFrame.gameObject.SetActive(false);

            GetUserNameFromLocalConfig();
            //GetIPAddressFromLocalConfig();

            ServerList.onButtonGroupSelectedChange.AddListener(OnselectedServerChanged);
            LastEnterServerList.onButtonGroupSelectedChange.AddListener(OnselectedServerChanged);
            DistrictSearchInputField.onValueChanged.AddListener(OnSearchInputFieldChanged);

            UpdateDistrictList();
            UpdateServerInfo();

            if (!Debug.isDebugBuild)
            {
                InitForRelease();
                //发布版走外网注册网址
                m_strRegisterURL = "http://passport.q1.com/zhuce.html";
            }
               
            AdvInfo info = GameUtil.GetAdvInfo();
            m_strRegisterURL += "?" + "radid=" + info.radid + "&" +
                                "rsid=" + info.rsid + "&gameid=7";

            return base.Init(wnd);
        }

        public override void Start()
        {
            base.Start();

            if (UserNameIFCombobox != null)
            {
                UserNameIFCombobox.GetComponent(0).ComboboxInputField.onValueChanged.AddListener(OnValueChanged);
                UserNameIFCombobox.GetComponent(0).ComboboxInputField.onEndEdit.AddListener(OnEndEdit);
            }
            string[] userNameArray = GameSettingConfig.Instance.GetGameSettingStringValue(GameSettingIndex.UserName).Split(';');
            if (userNameArray != null && userNameArray.Length > 0)
            {
                mUserNameList = userNameArray.ToList();
            }
        }

        public override void Update()
        {
            base.Update();

            if (!WebBrowserOpen && Input.GetKeyDown(KeyCode.Tab))
            {
                SelectableNavigation(true);
            }

            if (PasswordTI.GetComponent<InputField>().isFocused && GetKeyState(0x14) != 0) // 0x14 -- CapsLock的虚拟键值
            {
                CapsLockTip.gameObject.SetActive(true);
            }
            else
            {
                CapsLockTip.gameObject.SetActive(false);
            }
        }

        public void SelectableNavigation(bool _bTabKey)
        {
            Selectable nextSelect, currentSelect, lastSelect;

            GameObject currentGO = EventSystem.current.currentSelectedGameObject ?? DefaultSelected;
            GameObject lastGO = EventSystem.current.firstSelectedGameObject ?? DefaultSelected;

            if (currentGO == null)
                return;

            currentSelect = currentGO.GetComponent<Selectable>();
            if (currentSelect == null)
                return;

            lastSelect = lastGO != null ? lastGO.GetComponent<Selectable>() : null;

            if (Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift))
            {
                nextSelect = currentSelect.FindSelectableOnUp();
                if (nextSelect == null)
                    nextSelect = lastSelect;
            }
            else
            {
                nextSelect = currentSelect.FindSelectableOnDown();
                if (nextSelect == null)
                    nextSelect = lastSelect;
            }
            if (nextSelect != null)
            {
                InputField inputfield = nextSelect.GetComponent<InputField>();
                EventSystem.current.SetSelectedGameObject(nextSelect.gameObject, new BaseEventData(EventSystem.current));
            }

            if(!_bTabKey)
            {
                if(EventSystem.current.currentSelectedGameObject == LoginBtn.gameObject)
                {
                    Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
                    if (Data.UISoundConfig.UISoundConfig.Instance.GetData("login", "Button_login_click", ref soundData))
                    {
                        SoundManager.CreateUISound(soundData.USoundID);
                    }
                    onLoginBtnClick(LoginBtn.transform);
                }
            }
        }

        /// <summary>
        /// 远程/本地登录按钮 按下事件
        /// </summary>
        public void onLoginBtnClick(Transform _btnServer)
        {
            gameview_login vo = new GameLogic.gameview_login();

            // 当前登录IP地址缓存起来
            //vo.svrip = GetIPAddress();
            vo.nClientVersion = Initialize.localClientVersion;
            if (_btnServer.name == "LocalBtn")
            {
                vo.isLocalLogin = 1;
            }
            
            //SaveIPAddressToLocalConfig(vo.svrip);

            InputField Password = PasswordTI.GetComponent<InputField>();
            if (Password.textComponent.text == string.Empty)
                vo.password = Password.placeholder.GetComponent<Text>().text;
            else
                vo.password = Password.text;

            vo.worldid = DataCenter.LogicDataCenter.districtListDataManger.DistrictInfo.nWorldID;

            // 当前登录账号名缓存起来
            Config.AccountName = vo.account = GetUserName();
            SaveUserNameToLocalConfig(vo.account);

            mediator.SendLoginData(vo);
        }

        public void onRegisterBtnClick()
        {
            Application.OpenURL(m_strRegisterURL);
        }

        public void onChangeDistrictBtnClick()
        {
            if (tempServer.nGroup != m_serverInfo.nGroup || tempServer.nDistrict != m_serverInfo.nDistrict || tempServer.nServer != m_serverInfo.nServer)
            {
                mediator.SendChangeDistrictData(tempServer);
            }
            DistrictFrame.gameObject.SetActive(false);
        }

        public void onDistrictEntryBtnClick()
        {
            if (!DistrictFrame.gameObject.activeInHierarchy)
            {
                DistrictSearchInputField.text = "";
                DistrictFrame.gameObject.SetActive(true);
            }
        }

        public void onDistrictCloseBtnClick()
        {
            DistrictFrame.gameObject.SetActive(false);
        }

        public void UpdateServerInfo()
        {
            m_serverInfo = DataCenter.LogicDataCenter.districtListDataManger.DistrictInfo;
            DistrictTitle.text = m_serverInfo.szName;
            CurrentServerName.text = m_serverInfo.szName;
            //正常
            if(m_serverInfo.nState < 2)
            {
                if(State_Green)
                    State_Green.gameObject.SetActive(true);
                if (State_Red)
                    State_Red.gameObject.SetActive(false);
            }
            else//爆满
            {
                if (State_Green)
                    State_Green.gameObject.SetActive(false);
                if (State_Red)
                    State_Red.gameObject.SetActive(true);
            }

            SetButtonGroupState(ServerList);
            SetButtonGroupState(LastEnterServerList);
        }

        public void UpdateDistrictList()
        {
            m_districtMap = DataCenter.LogicDataCenter.districtListDataManger.DistrictInfoMap;
            if (m_districtMap == null || m_districtMap.Count == 0)
            {
                return;
            }

            if (m_districtMap != null && m_districtMap.ContainsKey(m_CurrentSelectGroup))
                UpdateServerList(m_districtMap[m_CurrentSelectGroup]);
        }

        public void UpdateLastEnterDistrictList()
        {
            int nCount = LastEnterServerList.transform.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = LastEnterServerList.transform.GetChild(i);
                if (childTrans)
                {
                    GameObject go = childTrans.gameObject;
                    // 销毁obj
                    ResNode.DestroyRes(ref go);
                }
            }

            m_lastEnterDistrictList = DataCenter.LogicDataCenter.districtListDataManger.LastEnterDistrictInfoList;

            if (m_lastEnterDistrictList == null || m_lastEnterDistrictList.Count < 1)
                return;

            for (int i = 0; i < m_lastEnterDistrictList.Count; i++)
            {
                //最多列5个
                if (i > 5)
                    break;

                GameObject serverGo = ResNode.InstantiateRes(DefaultLastServerItem);
                if (serverGo != null)
                {
                    serverGo.transform.SetParent(LastEnterServerList.transform, false);
                    serverGo.GetComponent<UButtonGroupItem>().Group = LastEnterServerList;
                    serverGo.SetActive(true);

                    Text btnLabel = serverGo.GetComponentInChildren<Text>();
                    if (btnLabel)
                        btnLabel.text = m_lastEnterDistrictList[i].szName;

                    DistrictInfo districtInfo = serverGo.GetComponent<DistrictInfo>();
                    if (districtInfo != null)
                    {
                        districtInfo.SetData(i, m_lastEnterDistrictList[i].nGroup, m_lastEnterDistrictList[i].nDistrict, m_lastEnterDistrictList[i].nServer);
                    }
                }
            }
        }

        public void SearchDistrict()
        {
            if (DistrictSearchInputField)
            {
                OnSearchInputFieldChanged(DistrictSearchInputField.text);
            }
        }

        protected void OnEndEdit(string _strText)
        {
            if (UserNameIFCombobox != null && (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.Tab)))
            {
                UserNameIFCombobox.HideList();
            }
        }

        public void OnValueChanged(string filter)
        {
            if (mUserNameList == null || mUserNameList.Count <= 0)
            {
                return;
            }

            List<String> userNameList = null;
            if (!string.IsNullOrEmpty(filter))
            {
                userNameList = new List<String>();
                foreach (String userName in mUserNameList)
                {
                    if (string.IsNullOrEmpty(userName))
                        continue;

                    int index = userName.IndexOf(filter);
                    if (index == 0)
                    {
                        userNameList.Insert(0, userName);
                    }
                    else if (index > 0)
                    {
                        userNameList.Add(userName);
                    }
                }
            }

            UserNameIFCombobox.ListView.DataSource.Clear();
            if (userNameList == null)
            {
                userNameList = mUserNameList;
            }

            UIWidgets.ObservableList<UInputFieldComboboxListViewData> newDataSource = new UIWidgets.ObservableList<UInputFieldComboboxListViewData>();
            foreach (var localUserName in userNameList)
            {
                if (localUserName != string.Empty)
                    newDataSource.Add(new UInputFieldComboboxListViewData(localUserName));
            }
            if (newDataSource.Count <= 0)
            {
                UserNameIFCombobox.HideList();
            }
            else
            {
                UserNameIFCombobox.ListView.DataSource = newDataSource;
                if (!UserNameIFCombobox.ListView.gameObject.activeSelf)
                {
                    UserNameIFCombobox.ShowList();
                }
            }
        }

        protected string GetUserName()
        {
            return UserNameIFCombobox.GetComponent(0).ComboboxInputField.text;
        }

        //protected string GetIPAddress()
        //{
        //    //return IPAddressIFCombobox.GetComponent(0).ComboboxInputField.text;
        //}

        /// <summary>
        /// 判断该登陆用户名是否存在于配置文件中，如果不存在则进行添加
        /// </summary>
        /// <param name="_strUserName"></param>
        protected void SaveUserNameToLocalConfig(string _strUserName)
        {
            System.Text.StringBuilder loginedUserName_str = new System.Text.StringBuilder();
            List<string> userNameArrayStr;
            int userNameCount = 1;

            //把当前记录的所有用户名取出来存到数组里
            userNameArrayStr = GameSettingConfig.Instance.GetGameSettingStringValue(GameSettingIndex.UserName).Split(';').ToList();
            //判断当前输入的用户名是否已经记录了
            //true：从数组移除
            {
                int tmpIndex = -1;
                if ((tmpIndex = userNameArrayStr.IndexOf(_strUserName)) != -1)
                {
                    userNameArrayStr.RemoveAt(tmpIndex);
                }
            }
            //把当前输入的作为字符流首位，之前的依次填入字符流
            loginedUserName_str.Append(_strUserName);
            //填入之前的时候判断记录的长度有没有超出限定值
            //true：不再填入
            foreach(var oldUserName in userNameArrayStr)
            {
                userNameCount++;
                if (userNameCount > 4)
                    break;

                loginedUserName_str.Append(';');
                loginedUserName_str.Append(oldUserName);
            }
            // 保存用户名信息
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.UserName, loginedUserName_str.ToString());  
        }

        protected void GetUserNameFromLocalConfig()
        {
            string[] userNameArrayStr =
                GameSettingConfig.Instance.GetGameSettingStringValue(GameSettingIndex.UserName).Split(';');

            UIWidgets.ObservableList<UInputFieldComboboxListViewData> newDataSource = new UIWidgets.ObservableList<UInputFieldComboboxListViewData>();
           // for(int i=userNameArrayStr.Length-1;i>=0;i--)
            foreach (var localUserName in userNameArrayStr)
            {
                if(localUserName != string.Empty)
                    newDataSource.Add(new UInputFieldComboboxListViewData(localUserName));
            }
            if (newDataSource.Count <= 0)
            {
                newDataSource.Add(new UInputFieldComboboxListViewData(string.Empty));
            }
            UserNameIFCombobox.ListView.DataSource = newDataSource;
            UserNameIFCombobox.ListView.Select(0);
        }

        protected void SaveIPAddressToLocalConfig(string _strIPAddress)
        {
            List<string> IPAddressStrArray;
            int IPAddressCount = 1;

            // 取出当前记录的所有IP地址到数组中
            IPAddressStrArray = GameSettingConfig.Instance.GetGameSettingStringValue(GameSettingIndex.IPAddress).Split(';').ToList();
            // 判断当前输入的IP地址是否已经记录， true则移除
            int tmpIndex = -1;
            if ((tmpIndex = IPAddressStrArray.IndexOf(_strIPAddress)) != -1)
            {
                IPAddressStrArray.RemoveAt(tmpIndex);
            }
            // 把当前输入的作为首位，之前的依次填入
            System.Text.StringBuilder loginedIPAddress = new System.Text.StringBuilder();
            loginedIPAddress.Append(_strIPAddress);
            foreach (var oldUserIP in IPAddressStrArray)
            {
                IPAddressCount++;
                if (IPAddressCount > 4)
                    break;

                loginedIPAddress.Append(';');
                loginedIPAddress.Append(oldUserIP);
            }
            
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.IPAddress, loginedIPAddress.ToString());
        }

        protected void GetIPAddressFromLocalConfig()
        {
            string[] IPAddressArray = GameSettingConfig.Instance.GetGameSettingStringValue(GameSettingIndex.IPAddress).Split(';');

            UIWidgets.ObservableList<UInputFieldComboboxListViewData> newDataSource = new UIWidgets.ObservableList<UInputFieldComboboxListViewData>();
            foreach (var item in IPAddressArray)
            {
                newDataSource.Add(new UInputFieldComboboxListViewData(item));
            }
            IPAddressIFCombobox.ListView.DataSource = newDataSource;
            IPAddressIFCombobox.ListView.Select(0);
        }

        private void InitForRelease()
        {
            Transform LocalBtn = null;
            Transform RegisterBtn = null;
            Transform IPAddress = null;
            Transform IPText = null;
            Transform UserAgreement = null;

            LocalBtn = GetTransform().FindChild("LocalBtn");
            RegisterBtn = GetTransform().FindChild("RegisterBtn");
            IPAddress = GetTransform().FindChild("IPAddress");
            IPText = GetTransform().FindChild("IPText");
            UserAgreement = GetTransform().FindChild("UserAgreement");

            if (LocalBtn != null)
                LocalBtn.gameObject.SetActive(false);
            //if (RegisterBtn != null)
            //    RegisterBtn.gameObject.SetActive(false);
            if (IPAddress != null)
                IPAddress.gameObject.SetActive(false);
            if (IPText != null)
                IPText.gameObject.SetActive(false);
            if (UserAgreement != null)
                UserAgreement.gameObject.SetActive(false);
        }
    
        private void UpdateServerList(List<gamelogic_district_info> groupList)
        {
            int nCount = ServerList.transform.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = ServerList.transform.GetChild(i);
                if (childTrans)
                {
                    GameObject go = childTrans.gameObject;
                    // 销毁obj
                    ResNode.DestroyRes(ref go);
                }
            }

            for (int i = 0; i < groupList.Count; i++)
            {
                GameObject serverGo = ResNode.InstantiateRes(DefaultServerItem);
                if (serverGo != null)
                {
                    serverGo.transform.SetParent(ServerList.transform, false);
                    serverGo.GetComponent<UButtonGroupItem>().Group = ServerList;
                    serverGo.SetActive(true);

                    Text btnLabel = serverGo.GetComponentInChildren<Text>();
                    if (btnLabel)
                        btnLabel.text = groupList[i].szName;

                    //正常
                    if(groupList[i].nState < 2)
                    {
                        Transform icon_Green = serverGo.transform.FindChild("Icon_Green");
                        if (icon_Green)
                            icon_Green.gameObject.SetActive(true);
                    }
                    else//爆满
                    {
                        Transform icon_Red = serverGo.transform.FindChild("Icon_Red");
                        if (icon_Red)
                            icon_Red.gameObject.SetActive(true);
                    }

                    DistrictInfo districtInfo = serverGo.GetComponent<DistrictInfo>();
                    if (districtInfo != null)
                    {
                        districtInfo.SetData(i, groupList[i].nGroup, groupList[i].nDistrict, groupList[i].nServer);
                    }
                }
            }
        }

        private void SetButtonGroupState(UButtonGroup group)
        {
            int nCount = group.transform.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = group.transform.GetChild(i);
                if (childTrans)
                {
                    DistrictInfo districtInfo = childTrans.GetComponent<DistrictInfo>();
                    if (districtInfo != null)
                    {
                        UButtonGroupItem item = childTrans.GetComponent<UButtonGroupItem>();
                        if(item != null)
                        {
                            if (districtInfo.CheckEqual(m_serverInfo.nGroup, m_serverInfo.nDistrict, m_serverInfo.nServer))
                                item.Selected = true;
                            else
                                item.Selected = false;
                        }
                    }
                }
            }
        }

        private void OnselectedServerChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            DistrictInfo districtInfo = newItem.GetComponent<DistrictInfo>();
            if (districtInfo != null)
            {
                tempServer.nGroup = districtInfo.GetGroupID();
                tempServer.nDistrict = districtInfo.GetDistrictID();
                tempServer.nServer = districtInfo.GetServerID();
            }
        }

        private void OnSearchInputFieldChanged(string content)
        {
            if(content == string.Empty)
            {
                if (m_districtMap != null && m_districtMap.ContainsKey(m_CurrentSelectGroup))
                    UpdateServerList(m_districtMap[m_CurrentSelectGroup]);
            }
            else
            {
                List<gamelogic_district_info> infoList = DataCenter.LogicDataCenter.districtListDataManger.SearchContent(content);
                UpdateServerList(infoList);
            }
            UpdateServerInfo();
        }
    }

    [Serializable]
    public class UAdvanceLogin : UIBehaviour,ITimerHanlder
    {
        /// <summary>
        /// 额外登录类型
        /// </summary>
        public enum EAdvanceLoginType : byte
        {
            EALT_QQ,
            EALT_WeChat,
        }
        [Serializable]
        public class JSQueryJson
        {
            public string action;
            public string access_token;
            public string code;
        }

        /// <summary>
        /// 内置网页 显示宽高
        /// </summary>
        [Tooltip("内置网页 窗口宽高")]
        public Vector2 WebBrowserSize = new Vector2(800, 600);

        /// <summary>
        /// 内置网页 窗口位置偏移值
        /// </summary>
        [Tooltip("内置网页 窗口位置偏移值")]
        public Vector2 WebBrowserPosOffset = new Vector2(-800, 0);


        private ResNode m_WebBrowserResNode;
        private UBrowser m_BrowserInstance;

        private EAdvanceLoginType AdvanceLoginType = EAdvanceLoginType.EALT_QQ;

        protected override void Awake()
        {
            base.Awake();

        }

        public void OnAdvanceLoginClick(int nLoginType)
        {
            if (nLoginType < 0 || nLoginType > (int)EAdvanceLoginType.EALT_WeChat)
                return;


            AdvanceLoginType = (EAdvanceLoginType)nLoginType;

            if (null != m_BrowserInstance)
            {
                m_BrowserInstance.OnToolbarUrlFieldEndEdit(GetWebLoginURL(AdvanceLoginType));
                return;
            }

            UIMsgCmdData msg = new UIMsgCmdData((int)WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, 1, "", IntPtr.Zero, 0);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LOGIN, WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, msg);

            m_WebBrowserResNode = UISystem.Instance.GetResNode("UI/Widgets/WebBrowser/WebBrowser");
            if (m_WebBrowserResNode.IsValid())
            {
                m_BrowserInstance = m_WebBrowserResNode.InstanceMainRes<UBrowser>();
                m_BrowserInstance.Width = (int)WebBrowserSize.x;
                m_BrowserInstance.Height = (int)WebBrowserSize.y;
                m_BrowserInstance.InitialURL = GetWebLoginURL(AdvanceLoginType);
                m_BrowserInstance.SetToolbarType(UWebBrowserToolbarWidget.EToolbar.ETB_Close);

                m_BrowserInstance.GetRectTransform.SetParent(GetAttachParent(), false);
                m_BrowserInstance.GetRectTransform.anchoredPosition = WebBrowserPosOffset;

                m_BrowserInstance.OnJsQueryEvent.RemoveListener(OnJSQueryEvent);
                m_BrowserInstance.OnJsQueryEvent.AddListener(OnJSQueryEvent);

                m_BrowserInstance.OnDisConnectedEvent.RemoveListener(OnDisConnectedEvent);
                m_BrowserInstance.OnDisConnectedEvent.AddListener(OnDisConnectedEvent);
            }
        }

        /// <summary>
        /// 获取登录地址
        /// </summary>
        /// <param name="webType"></param>
        /// <returns></returns>
        protected string GetWebLoginURL(EAdvanceLoginType webType)
        {
            string url = "";

            if (Debug.isDebugBuild)        // 测试版
            {
                url = "http://openapi.wxg.dev.q1.com/";
            }
            else                            // 正式版
            {
                url = "http://openapi.game.q1.com/";
            }

            if (webType == EAdvanceLoginType.EALT_QQ)
            {
                //QQ
                url += "account/loginfrom.aspx?account=qq";
            }
            else if (webType == EAdvanceLoginType.EALT_WeChat)
            {
                // 微信
                url += "account/loginfrom.aspx?account=23";
            }

            return url;
        }

        /// <summary>
        /// 默认挂在UViewLogin下，避免浏览器窗口实例没删除
        /// </summary>
        /// <returns></returns>
        protected Transform GetAttachParent()
        {
            Transform tmpTrans = transform.parent;
            if (tmpTrans == null)
            {
                Trace.LogWarning("额外登录按钮没创建在登录界面？");
                return UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_OVERHEAD);
            }
            else
            {
                
                while(tmpTrans != null && tmpTrans.GetComponent<UViewLogin>() == null)
                {
                    tmpTrans = tmpTrans.parent;
                }

                return tmpTrans;
            }
            
        }

        /// <summary>
        /// 侦听浏览器查询的方法回调
        /// </summary>
        /// <param name="_strQuery"></param>
        protected void OnJSQueryEvent(string _strQuery)
        {
            //JObject jsQuery = JObject.Parse(_strQuery);
            JSQueryJson loginJson = JsonUtility.FromJson<JSQueryJson>(_strQuery);



            if(loginJson.action != "login")//if (jsQuery["action"].ToString() != "login")
            {
                Trace.LogError("外部登录网页返回的操作不是 action: login ");
                DelayRelease(0.5f);
                return;
            }

            gameview_login login_data = new gameview_login();
            login_data.nClientVersion = Initialize.localClientVersion;
            login_data.worldid = DataCenter.LogicDataCenter.districtListDataManger.DistrictInfo.nWorldID;
            login_data.access_token = loginJson.access_token;//login_data.access_token = jsQuery["access_token"].ToString();

            ViewEventHelper.Instance.SendCommand<gameview_login>(GameLogicDef.GVIEWCMD_USER_LOGIN, ref login_data);

            DelayRelease(0.5f);
        }

        /// <summary>
        /// 取消连接浏览器事件回调
        /// </summary>
        /// <param name="_strQuery"></param>
        protected void OnDisConnectedEvent(string _strQuery)
        {
            DelayRelease(0.5f);
        }

        /// <summary>
        /// 延迟释放资源，因为是在 UBrowser::OnJsQueryEvent 回调里创建资源的避免冲突
        /// </summary>
        protected void DelayRelease(float _fDelayTime)
        {
            UIMsgCmdData msg = new UIMsgCmdData((int)WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, 0, "", IntPtr.Zero, 0);
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LOGIN, WndMsgID.WND_MSG_CREATEHERO_WEBBROWSER_STATE, msg);

            TimerManager.SetTimer(this, 0, _fDelayTime, 1);
        }

        public void OnTimer(int nTimerID)
        {
            if(nTimerID == 0 && m_WebBrowserResNode != null)
            {
                UISystem.Instance.ReleaseResNode(m_WebBrowserResNode, true);
            }
        }
    }



    public class DistrictInfo:MonoBehaviour
    {
        private int m_btnIndex;
        private int m_group;
        private int m_district;
        private int m_server;

        public void SetData(int nIndex, int nGroup, int nDistrict, int nServer)
        {
            m_btnIndex = nIndex;
            m_group = nGroup;
            m_district = nDistrict;
            m_server = nServer;
        } 

        public bool CheckEqual(int nGroup, int nDistrict, int nServer)
        {
            if (m_group == nGroup && m_district == nDistrict && m_server == nServer)
                return true;
            return false;
        }

        public int GetGroupID()
        {
            return m_group;
        }

        public int GetDistrictID()
        {
            return m_district;
        }

        public int GetServerID()
        {
            return m_server;
        }
    }



    //window.OnJavaScriptCallBack({"request":'{"action":\"login\","access_token":"8FD6265B9BB0C57ACBC607170DF230AD7C8D3AD0","code":"ceunLIH4V8IQWg/Qd14StaGPzKkpWx0AebCrGsr14xc6oKMOKlGGfQ=="}'});

}

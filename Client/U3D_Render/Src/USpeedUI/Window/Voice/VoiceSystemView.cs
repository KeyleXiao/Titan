using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using GameLogic;
using DataCenter;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using USpeedUI.UWidgets;

namespace USpeedUI.VoiceSystem
{
    public enum EMVoiceTimer : int     //计时器
    {
        VoiceTimer_MicOrderInterval = 0,
    }

    public enum EMVoiceRoomSettingType      //房间管理类型
    {
        None,
        AddLock,                           //加密码
        RemoveLock,                     //取消密码
        SetMicOrderTime,              //设置默认麦序时间
        EnterLockRoom,                //进入密码房间
    }

    public class VoiceChannelItem : UCustomUIBehaviour, IPointerClickHandler, IPointerExitHandler, IPointerEnterHandler
    {
        //UI控件
        public Image channelIcon;
        public Text channelTxt;
        public Image channelItemBG;
        public Sprite activeIcon;
        public Sprite forbidIcon;
        //当前频道类型
        public EMVoiceChannelType channelType;

        private VoiceSystemView voiceView;
        private bool bActiveChannel = true;                   //是否激活了当前频道
        private string channelName;                        

        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            channelName = voiceView.GetChannelName(channelType);

            if (channelTxt != null)
            {
                if(channelType == EMVoiceChannelType.VoiceChannel_None)
                    channelTxt.text = channelName;
                else
                    channelTxt.text = channelName + "频道";
            }
        }

        public void Clear()
        {
            if (channelType == EMVoiceChannelType.VoiceChannel_None)
                return;

            bActiveChannel = false;
            if (channelIcon != null && forbidIcon != null)
                channelIcon.sprite = forbidIcon;
        }

        //是否激活此频道
        public void AcitveState(bool bActive)
        {
            if (channelType == EMVoiceChannelType.VoiceChannel_None)
                return;

            bActiveChannel = bActive;
            if (channelIcon != null)
            {
                if (bActiveChannel)
                {
                    if (activeIcon != null)
                        channelIcon.sprite = activeIcon;
                    if (channelTxt != null)
                        channelTxt.color = Color.white;
                }
                else
                {
                    if (forbidIcon != null)
                        channelIcon.sprite = forbidIcon;
                    if (channelTxt != null)
                        channelTxt.color = Color.gray;
                }
            }
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            //点击左键则向上请求进入频道
            if (eventData.button == PointerEventData.InputButton.Left)
            {
                if (bActiveChannel && voiceView != null)
                    voiceView.RequestEnterChannel(channelType);
            }
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (bActiveChannel && channelItemBG != null)
                channelItemBG.gameObject.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            if (bActiveChannel && channelItemBG != null)
                channelItemBG.gameObject.SetActive(false);
        }
    }

    public class VoiceRoomItem : UCustomUIBehaviour, IPointerClickHandler, IPointerExitHandler, IPointerEnterHandler
    {
        //UI控件
        public Text roomTitle;
        public Image roomItemBG;
        //当前房间类型
        public EMVoiceRoomType roomType;

        private VoiceSystemView voiceView;
        private UPopupAction popupAction;  //右键弹出菜单表

        //private bool bActiveRoom;   
        private bool bInMainFrameGroup;           //是否属于主界面房间按钮组
        private int roonNum = 0;      
        private string roomName = "";
        private string channelName = "";

        //描述
        //private string connectDesc;
        private string szAddLock;                             
        private string szRemoveLock;                       
        private string szSetMicOrderTime;

        public void Init(VoiceSystemView view, bool bInMainFrame)
        {
            voiceView = view;
            bInMainFrameGroup = bInMainFrame;

            popupAction = new UPopupAction();
            roomName = voiceView.GetRoomName(roomType);

            if (roomTitle != null)
                roomTitle.text = roomName;

            szAddLock = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szAddLock");
            szRemoveLock = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szRemoveLock");
            szSetMicOrderTime = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szSetMicOrderTime");
        }

        public void Clear()
        {
            roomTitle.text = "";
            channelName = "";
            roonNum = 0;
            popupAction.Clear();
            this.gameObject.SetActive(false);
        }

        /// <summary>
        /// 设置房间数据
        /// </summary>
        /// <param name="num">人数</param>
        /// <param name="nInConnect">是否连接</param>
        /// <param name="nLock">是否有密码</param>
        public void SetData(int num, int nInConnect, int nLock)
        {
            roonNum = num;

            if(voiceView != null)
                channelName = voiceView.CurrentChannelName;

            if(roomName != null)
            {
                if (roomType == EMVoiceRoomType.VoiceRoomType_Main || roomType == EMVoiceRoomType.VoiceRoomType_PK || roomType == EMVoiceRoomType.VoiceRoomType_KTV)
                    roomTitle.text = channelName + roomName;
                else
                    roomTitle.text = roomName;
                if (num > 0)
                    roomTitle.text += "(" + roonNum + ")";
                //if (nInConnect > 0)
                //    roomTitle.text += connectDesc;
            }

            //判断是否有设置密码的权限,是否属于主界面，是否是大厅
            if (bInMainFrameGroup && roomType != EMVoiceRoomType.VoiceRoomType_Main && (LogicDataCenter.voiceSystemDataManager.UserRight & (int)EMVoiceRightFlag.VoiceRight_Lock) != 0)
            {
                if (nLock > 0)
                {
                    if (!popupAction.ContainsKey(szRemoveLock))
                        popupAction.Add(szRemoveLock, new UListComponentItemDataPopup(() => { RemoveRoomLock(); }));
                    if (popupAction.ContainsKey(szAddLock))
                        popupAction.Remove(szAddLock);
                }
                else
                {
                    if (!popupAction.ContainsKey(szAddLock))
                        popupAction.Add(szAddLock, new UListComponentItemDataPopup(() => { AddRoomLock(); }));
                    if (popupAction.ContainsKey(szRemoveLock))
                        popupAction.Remove(szRemoveLock);
                }
            }

            this.gameObject.SetActive(true);
        }

        public void AddPopUpAction(EMVoiceRoomSettingType type)
        {
            if (bInMainFrameGroup)
            {
                if (type == EMVoiceRoomSettingType.SetMicOrderTime)
                {
                    if (!popupAction.ContainsKey(szSetMicOrderTime))
                        popupAction.Add(szSetMicOrderTime, new UListComponentItemDataPopup(() => { AddMicOrderTime(); }));
                }
            }
        }

        public void RemovePopUpAction(EMVoiceRoomSettingType type)
        {
            if (type == EMVoiceRoomSettingType.SetMicOrderTime)
            {
                if (popupAction.ContainsKey(szSetMicOrderTime))
                    popupAction.Remove(szSetMicOrderTime);
            }
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            //左键请求进入房间
            if (eventData.button == PointerEventData.InputButton.Left)
            {
                if (voiceView != null)
                    voiceView.RequestEnterRoom(roomType);
            }
            //右键弹出菜单
            else if (eventData.button == PointerEventData.InputButton.Right)
            {
                //判断是否有菜单列表
                if (popupAction.Count > 0)
                {
                    if (voiceView != null)
                        voiceView.SetReqSettingRoomID((int)roomType);
                    if (popupAction.Count > 0)
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, new UIPopupMenuData(popupAction, GetTransform, UIWidgets.TipPosition.MouseBottomRightCorner));         
                }
            }
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (roomItemBG != null)
                roomItemBG.gameObject.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            if (roomItemBG != null)
                roomItemBG.gameObject.SetActive(false);
        }

        private void AddMicOrderTime()
        {
            if (voiceView != null)
                voiceView.ShowInputFieldFrame(EMVoiceRoomSettingType.SetMicOrderTime);
        }

        private void AddRoomLock()
        {
            if (voiceView != null)
                voiceView.ShowInputFieldFrame(EMVoiceRoomSettingType.AddLock);
        }

        private void RemoveRoomLock()
        {
            cmd_voice_req_lock_room unlockRoom = new cmd_voice_req_lock_room();
            unlockRoom.nRoomID = (int)roomType;
            unlockRoom.isLock = 0;
            unlockRoom.szPassword = "";
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_lock_room>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_LOCK_ROOM, ref unlockRoom);
        }
    }

    public class VoiceMiniPanel : UCustomUIBehaviour, IPointerExitHandler, IPointerEnterHandler
    {
        //UI控件
        public Button voiceSystemEntryBtn;
        public Button channelSelectBtn;
        public Button miniEarphoneBtn;
        public Button miniMicphoneBtn;

        public Text speakModeLabel;
        public Slider volumeSlider;
        public Dropdown speakModeDropdown;

        public RectTransform minipanelFrame;
        public RectTransform channelDropList;
        public RectTransform miniChannelSelectedBG;
        public RectTransform miniRoomSelectedBG;

        public List<VoiceChannelItem> miniChannelBtnGroup;
        public List<VoiceRoomItem> miniRoomBtnGroup;

        //mini面板隐藏时间，多少秒不交互则隐藏
        [Range(5.0f, 20.0f)]
        public float MiniPanelHideTime = 5.0f;

        private int modalKey;                         //模态key
        private float nVolume;                        //音量
        private float miniPaneltimer;               //mini面板计时器
        private bool bOpenMiniPanel;              //是否打开了mini面板
        private bool bMouseInMiniPanel;         //鼠标是否在mini面板上
        private bool bOpenMiniChannelList;     //是否打开了房间列表

        private VoiceSystemView voiceView;

        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            voiceSystemEntryBtn.onClick.AddListener(OnVoiceEntryBtnClick);
            channelSelectBtn.onClick.AddListener(OnChannelSelectBtnClick);
            miniEarphoneBtn.onClick.AddListener(OnEarphoneClick);
            miniMicphoneBtn.onClick.AddListener(OnMicphoneClick);

            speakModeDropdown.options.Add(new Dropdown.OptionData() { text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "PushToTalk") });
            speakModeDropdown.options.Add(new Dropdown.OptionData() { text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "FreeSpeech") });
            speakModeDropdown.value = 0;
            speakModeDropdown.onValueChanged.AddListener(OnSpeakModeChange);

            for (int i = 0; i < miniChannelBtnGroup.Count; i++)
            {
                miniChannelBtnGroup[i].Init(voiceView);
            }

            for (int i = 0; i < miniRoomBtnGroup.Count; i++)
            {
                miniRoomBtnGroup[i].Init(voiceView, false);
            }

            nVolume = 1.0f;
            bOpenMiniPanel = false;
            bMouseInMiniPanel = false;
            bOpenMiniChannelList = false;
        }

        public void ClearMiniPanelData()
        {
            foreach (VoiceChannelItem item in miniChannelBtnGroup)
            {
                item.Clear();
            }

            foreach (VoiceRoomItem item in miniRoomBtnGroup)
            {
                item.Clear();
            }

            SwitchMiniMicphoneState(true, false);
            speakModeLabel.text = speakModeDropdown.captionText.text;
            miniMicphoneBtn.interactable = false;
            miniChannelSelectedBG.gameObject.SetActive(false);
            miniRoomSelectedBG.gameObject.SetActive(false);
        }

        public void Update()
        {
            if (bOpenMiniPanel && !bOpenMiniChannelList && !bMouseInMiniPanel)
            {
                miniPaneltimer += Time.deltaTime;

                if(Mathf.Abs(MiniPanelHideTime - miniPaneltimer) < 1.0f)
                    speakModeDropdown.Hide();

                if (miniPaneltimer > MiniPanelHideTime)
                {
                    minipanelFrame.gameObject.SetActive(false);
                    bOpenMiniPanel = false;
                    miniPaneltimer = 0f;
                }
            }
        }

        //更新频道列表激活状态
        public void UpdateMiniPanelChannelState()
        {
            List<int> channelList = LogicDataCenter.voiceSystemDataManager.ChannelActiveList;

            for (int i = 0; i < channelList.Count; i++)
            {
                bool bActive = channelList[i] != 0 ? true : false;

                VoiceChannelItem item = miniChannelBtnGroup.Find((x) => x.channelType == (EMVoiceChannelType)i);
                if (item != null)
                    item.AcitveState(bActive);
            }
        }

        //更新房间列表信息
        public void UpdateMiniPanelRoomInfo(VoiceRoomBaseInfoData baseInfo)
        {
            if(miniRoomBtnGroup != null)
            {
                VoiceRoomItem item = miniRoomBtnGroup.Find((x) => (int)x.roomType == baseInfo.nRoomID);
                if (item != null)
                {
                    item.SetData(baseInfo.nUserNum, baseInfo.isSpeaking, baseInfo.nLock);
                }
            }
        }

        public void EnterRoom(EMVoiceChannelType channel, EMVoiceRoomType room)
        {
            ClearMiniPanelData();

            if (null != voiceSystemEntryBtn && voiceSystemEntryBtn.gameObject.activeInHierarchy)
                voiceSystemEntryBtn.gameObject.SetActive(false);

            if (null != channelSelectBtn && !channelSelectBtn.gameObject.activeInHierarchy)
            {
                channelSelectBtn.gameObject.SetActive(true);
            }

            VoiceChannelItem channelItem = miniChannelBtnGroup.Find((x) => x.channelType == channel);
            if (channelItem != null && miniChannelSelectedBG != null)
            {
                miniChannelSelectedBG.gameObject.SetActive(true);
                Vector2 pos = channelItem.GetRectTransform.anchoredPosition;
                miniChannelSelectedBG.anchoredPosition = pos;
            }

            VoiceRoomItem roomItem = miniRoomBtnGroup.Find((x) => x.roomType == room);
            if (roomItem != null && miniRoomSelectedBG != null)
            {
                miniRoomSelectedBG.gameObject.SetActive(true);
                Vector2 pos = roomItem.GetRectTransform.anchoredPosition;
                miniRoomSelectedBG.anchoredPosition = pos;
            }
        }

        public void QuitVoice()
        {
            if (null != channelSelectBtn)
                channelSelectBtn.gameObject.SetActive(false);

            if (null != channelDropList)
                channelDropList.gameObject.SetActive(false);

            if (null != voiceSystemEntryBtn)
                voiceSystemEntryBtn.gameObject.SetActive(true);

            bOpenMiniPanel = false;

            if (minipanelFrame != null)
                minipanelFrame.gameObject.SetActive(false);

            if(speakModeLabel != null)
                speakModeLabel.gameObject.SetActive(false);
        }

        //切换耳机状态
        public void SwtichMiniEarphoneState(bool bOpen)
        {
            if(bOpen)
            {
                if (miniEarphoneBtn != null && voiceView.earphoneStateIcons[0] != null)
                    miniEarphoneBtn.GetComponent<Image>().sprite = voiceView.earphoneStateIcons[0];
                if (volumeSlider != null)
                    volumeSlider.value = nVolume;
            }
            else
            {
                if (miniEarphoneBtn != null && voiceView.earphoneStateIcons[1] != null)
                    miniEarphoneBtn.GetComponent<Image>().sprite = voiceView.earphoneStateIcons[1];
                if (volumeSlider != null)
                {
                    nVolume = volumeSlider.value;
                    volumeSlider.value = 0f;
                }
            }
        }

        //切换麦克风状态
        public void SwitchMiniMicphoneState(bool bForbid, bool bOpen)
        {
            if (miniMicphoneBtn != null)
            {
                miniMicphoneBtn.interactable = !bForbid;
                miniMicphoneBtn.transform.FindChild("OpenIcon").gameObject.SetActive(bOpen);
            }
            if (speakModeLabel != null)
                speakModeLabel.gameObject.SetActive(!bForbid);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            bMouseInMiniPanel = false;
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            bMouseInMiniPanel = true;
            miniPaneltimer = 0;
        }

        //主页按钮点击事件
        public void OnHomeBtnClick()
        {
            HideModal();

            if (voiceView != null)
                voiceView.SwitchMainFrameHideState();
        }

        public void OnMiniPanelCloseBtnClick()
        {
            if (minipanelFrame != null)
                minipanelFrame.gameObject.SetActive(false);

            bOpenMiniPanel = false;
        }

        //入口图标点击事件
        private void OnVoiceEntryBtnClick()
        {
            LogicDataCenter.voiceSystemDataManager.RequestEnterRoom((int)EMVoiceChannelType.VoiceChannel_GM, (int)EMVoiceRoomType.VoiceRoomType_Main);
        }

        //频道选择按钮点击事件
        private void OnChannelSelectBtnClick()
        {
            if (minipanelFrame != null)
            {
                minipanelFrame.gameObject.SetActive(true);
                bOpenMiniPanel = true;
                miniPaneltimer = 0;
            }

            if (channelDropList != null)
            {
                if (channelDropList.gameObject.activeInHierarchy)
                {
                    channelDropList.gameObject.SetActive(false);
                    channelDropList.transform.SetParent(this.transform);
                    bOpenMiniChannelList = false;
                }
                else
                {
                    bOpenMiniChannelList = true;
                    miniPaneltimer = 0;
                    channelDropList.gameObject.SetActive(true);
                    modalKey = ModalHelper.Open(this, null, new Color(0.15f, 0.15f, 0.15f, 0.2f), HideModal);
                    channelDropList.transform.SetParent(UISystem.Instance.GetCanvas().GetComponent<RectTransform>());
                }
            }
        }

        //耳机图标点击
        private void OnEarphoneClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchEarphoneState();
        }

        //麦克风图标点击
        private void OnMicphoneClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchMicphoneState();
        }

        //说话模式切换
        private void OnSpeakModeChange(int mode)
        {
            if (voiceView != null)
                voiceView.RequestChangeSpeakMode(mode);

            speakModeLabel.text = speakModeDropdown.captionText.text;
        }

        //隐藏模态窗口
        private void HideModal()
        {
            ModalHelper.Close(modalKey);

            channelDropList.gameObject.SetActive(false);
            channelDropList.transform.SetParent(this.transform);

            bOpenMiniChannelList = false;
            bMouseInMiniPanel = false;
        }
    }

    public class VoiceMainFrameChannelGroup : UCustomUIBehaviour
    {
        //UI控件
        public List<VoiceChannelItem> mainChannelBtnGroup;
        public RectTransform mainChannelSelectBG;

        private VoiceSystemView voiceView;

        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            for (int i = 0; i < mainChannelBtnGroup.Count; i++)
            {
                mainChannelBtnGroup[i].Init(voiceView);
            }
        }

        public void ClearMainChannelGroupState()
        {
            for (int i = 0; i < mainChannelBtnGroup.Count; i++)
            {
                mainChannelBtnGroup[i].Clear();
            }

            if (mainChannelSelectBG != null)
                mainChannelSelectBG.gameObject.SetActive(false);
        }

        public void UpdateMainFrameChannelState()
        {
            List<int> channelList = LogicDataCenter.voiceSystemDataManager.ChannelActiveList;

            for (int i = 0; i < channelList.Count; i++)
            {
                bool bActive = channelList[i] != 0 ? true : false;

                VoiceChannelItem item = mainChannelBtnGroup.Find((x) => x.channelType == (EMVoiceChannelType)i);
                if (item != null)
                    item.AcitveState(bActive);
            }
        }

        public void SelectChannel(EMVoiceChannelType type)
        {
            VoiceChannelItem channelItem = mainChannelBtnGroup.Find((x) => x.channelType == type);
            if (channelItem != null && mainChannelSelectBG != null)
            {
                mainChannelSelectBG.gameObject.SetActive(true);
                Vector2 pos = channelItem.GetRectTransform.anchoredPosition;
                mainChannelSelectBG.anchoredPosition = pos;
            }
        }
    }

    public class VoiceMainFrameRoomGroup : UCustomUIBehaviour
    {
        //UI控件
        public List<VoiceRoomItem> mainRoomBtnGroup;
        public RectTransform mainRoomSelectBG;
        public Text mainRoomTitle;

        private VoiceSystemView voiceView;

        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            for (int i = 0; i < mainRoomBtnGroup.Count; i++)
            {
                mainRoomBtnGroup[i].Init(voiceView, true);
            }
            mainRoomTitle.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "mainRoomTitle");
        }

        public void ClearMainRoomGroupState()
        {
            for (int i = 0; i < mainRoomBtnGroup.Count; i++)
            {
                mainRoomBtnGroup[i].Clear();
            }

            if (mainRoomSelectBG != null)
                mainRoomSelectBG.gameObject.SetActive(false);
        }

        public void UpdateMainRoomInfo(VoiceRoomBaseInfoData baseInfo)
        {
            if (mainRoomBtnGroup != null)
            {
                VoiceRoomItem item = mainRoomBtnGroup.Find((x) => (int)x.roomType == baseInfo.nRoomID);
                if (item != null)
                {
                    item.SetData(baseInfo.nUserNum, baseInfo.isSpeaking, baseInfo.nLock);
                }
            }
        }

        public void SelectRoom(EMVoiceRoomType type)
        {
            VoiceRoomItem roomItem = mainRoomBtnGroup.Find((x) => x.roomType == (EMVoiceRoomType)type);
            if (roomItem != null && mainRoomSelectBG != null)
            {
                mainRoomSelectBG.gameObject.SetActive(true);
                Vector2 pos = roomItem.GetRectTransform.anchoredPosition;
                mainRoomSelectBG.anchoredPosition = pos;
            }
        }

        public void AddRoomPopUpAction(EMVoiceRoomSettingType settingType)
        {
            foreach (VoiceRoomItem item in mainRoomBtnGroup)
            {
                item.AddPopUpAction(settingType);
            }
        }

        public void RemoveRoomPopUpAction(EMVoiceRoomSettingType settingType)
        {
            foreach (VoiceRoomItem item in mainRoomBtnGroup)
            {
                item.RemovePopUpAction(settingType);
            }
        }
    }

    public class VoiceMicOrderPanel : UCustomUIBehaviour,ITimerHanlder
    {
        //UI控件
        public UListMicOrder micOrderList;
        public Dropdown micModeDropdown;
        public Image dropdownIcon;
        public Button micphoneBtn;
        public Button graspMicBtn;
        public Text micOrderIntervalTxt;

        private VoiceSystemView voiceView;
        private int nTimer;
        private string graspMicDesc;      //抢麦描述
        private string quitMicDesc;         //下麦描述
        private bool bActiveMicOrderIntervalTimer;                 
        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            micModeDropdown.options.Add(new Dropdown.OptionData() { text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "MicMaster") });
            micModeDropdown.options.Add(new Dropdown.OptionData() { text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "MicFree") });
            micModeDropdown.options.Add(new Dropdown.OptionData() { text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "MicOrder") });
            micModeDropdown.onValueChanged.AddListener(RequestChangeMicMode);
            micModeDropdown.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "MicMaster");
            
            micphoneBtn.onClick.AddListener(OnMicphoneBtnClick);
            graspMicBtn.onClick.AddListener(OnGraspMicBtnClick);

            graspMicDesc = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "graspMicDesc");
            quitMicDesc = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "quitMicDesc");

            micOrderIntervalTxt.text = "";
            bActiveMicOrderIntervalTimer = false;
        }

        public void ClearMicOrder()
        {
            TimerManager.KillTimer(this, (int)EMVoiceTimer.VoiceTimer_MicOrderInterval);

            nTimer = 0;
            bActiveMicOrderIntervalTimer = false;

            if (micOrderIntervalTxt != null)
                micOrderIntervalTxt.text = "";
            micOrderList.DataSource.Clear();

            if (micModeDropdown != null)
            {
                micModeDropdown.onValueChanged.RemoveAllListeners();
                micModeDropdown.interactable = false;
                micModeDropdown.value = 0;
                micModeDropdown.onValueChanged.AddListener(RequestChangeMicMode);
            }

            if (dropdownIcon != null)
                dropdownIcon.gameObject.SetActive(false);

            if (micphoneBtn != null)
            {
                micphoneBtn.interactable = false;
                micphoneBtn.transform.FindChild("OpenIcon").gameObject.SetActive(false);
            }

            if (graspMicBtn != null)
            {
                graspMicBtn.interactable = false;
                graspMicBtn.GetComponentInChildren<Text>().text = graspMicDesc;
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EMVoiceTimer.VoiceTimer_MicOrderInterval:
                    {
                        nTimer--;
                        if (nTimer > 0 && micOrderIntervalTxt != null)
                        {
                            micOrderIntervalTxt.text = nTimer + " S";
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        //更新麦序表
        public void UpdateMicOrderList()
        {
            if (micOrderList == null)
                return;

            micOrderList.DataSource.Clear();
            List<cmd_voice_update_user_info> orderList = LogicDataCenter.voiceSystemDataManager.MicOrderList;
            for (int i = 0; i < orderList.Count; i++)
            {
                cmd_voice_update_user_info order = orderList[i];
                bool bSpeak = LogicDataCenter.voiceSystemDataManager.GetSpeakingState(order.nClientID);
                UListItemMicOrder item = new UListItemMicOrder(bSpeak, (i + 1).ToString(), order.nUserType, order.szUserName);
                micOrderList.Add(item);
            }
        }

        //改变麦序模式
        public void ChangeMicOrderMode(int nValue)
        {
            if (micModeDropdown != null)
                micModeDropdown.value = Mathf.Clamp(nValue,0,2);
        }

        //改变麦序模式管理状态
        public void ChangeMicOrderManageState(bool bDrop)
        {
            if (dropdownIcon != null)
                dropdownIcon.gameObject.SetActive(bDrop);

            if (micModeDropdown != null)
                micModeDropdown.interactable = bDrop;
        }

        //切换麦克风装态
        public void SwitchMicphoneState(bool bForbid, bool bOpen)
        {
            if (micphoneBtn != null)
            {
                micphoneBtn.interactable = !bForbid;
                micphoneBtn.transform.FindChild("OpenIcon").gameObject.SetActive(bOpen);
            }
        }

        //切换抢麦装态
        public void SwitchGraspMicState(bool bEnableClick, bool bOnMic)
        {
            if (graspMicBtn != null)
            {
                graspMicBtn.interactable = bEnableClick;
                if(bOnMic)
                    graspMicBtn.GetComponentInChildren<Text>().text = quitMicDesc;
                else
                    graspMicBtn.GetComponentInChildren<Text>().text = graspMicDesc;
            }
        }

        //激活麦序定时器
        public void ActiveMicOrderTimer(bool active)
        {
            if (micOrderIntervalTxt == null)
                return;

            if(active && !bActiveMicOrderIntervalTimer)
            {
                nTimer = LogicDataCenter.voiceSystemDataManager.CurrentMicOrderInterval;
                if (nTimer > 0)
                {
                    micOrderIntervalTxt.text = nTimer + " S";
                    TimerManager.KillTimer(this, (int)EMVoiceTimer.VoiceTimer_MicOrderInterval);
                    TimerManager.SetTimer(this, (int)EMVoiceTimer.VoiceTimer_MicOrderInterval, 1.0f);
                    bActiveMicOrderIntervalTimer = true;
                }
                else
                    nTimer = 0;
            }
            else if (!active && bActiveMicOrderIntervalTimer)
            {
                TimerManager.KillTimer(this, (int)EMVoiceTimer.VoiceTimer_MicOrderInterval);
                nTimer = 0;
                bActiveMicOrderIntervalTimer = false;
                micOrderIntervalTxt.text = "";
            }
        }

        //请求改变麦序
        private void RequestChangeMicMode(int mode)
        {
            LogicDataCenter.voiceSystemDataManager.ReqChangeMicMode(mode);
        }

        //麦克风按钮点击
        private void OnMicphoneBtnClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchMicphoneState();
        }

        //抢麦按钮点击
        private void OnGraspMicBtnClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchGraspMicState();
        }
    }

    public class VoiceRoomMemerPanel : UCustomUIBehaviour
    {
        //UI控件
        public UListVoiceUser roomMemberList;
        public InputField searchField;
        public Button earphoneBtn;
        public Button micphoneBtn;
        public Button prevPageBtn;
        public Button nextPageBtn;
        public Button firstPageBtn;
        public Button lastPageBtn;
        public Text vestLabel;
        public Text playerNameLabel;
        public Text rankLabel;
        public Text teamLabel;

        private VoiceSystemView voiceView;
        private int PerPageCount = 13;
        private int nMaxCount = 0;
        private int nMaxPage = 1;
        private int nCurrentPage = 1;
        private bool bIsSearching = false;
        private List<cmd_voice_update_user_info> userInfoList = new List<cmd_voice_update_user_info>();

        public void Init(VoiceSystemView view)
        {
            voiceView = view;

            searchField.onValueChanged.AddListener(OnSearchValueChanged);
            earphoneBtn.onClick.AddListener(OnEarphoneClick);
            micphoneBtn.onClick.AddListener(OnMicphoneClick);
            prevPageBtn.onClick.AddListener(ReqUserMemberListPrevPage);
            nextPageBtn.onClick.AddListener(ReqUserMemberListNextPage);
            firstPageBtn.onClick.AddListener(ReqUserMemberListFirstPage);
            lastPageBtn.onClick.AddListener(ReqUserMemberListLastPage);

            vestLabel.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "vestLabel");
            playerNameLabel.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "playerNameLabel");
            rankLabel.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "rankLabel");
            teamLabel.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "teamLabel");
        }

        public void ClearRoomMember()
        {
            nCurrentPage = 1;
            nMaxPage = 1;
            nMaxCount = 0;
            bIsSearching = false;
            userInfoList.Clear();

            roomMemberList.DataSource.Clear();
            searchField.text = "";

            if(micphoneBtn != null)
            {
                micphoneBtn.interactable = false;
                micphoneBtn.transform.FindChild("OpenIcon").gameObject.SetActive(false);
            }
            if (prevPageBtn != null)
                prevPageBtn.interactable = false;
            if (nextPageBtn != null)
                nextPageBtn.interactable = false;
            if (firstPageBtn != null)
                firstPageBtn.interactable = false;
            if (lastPageBtn != null)
                lastPageBtn.interactable = false;
        }

        public void UpdateRoomMemberList()
        {
            if (bIsSearching)
                return;

            userInfoList = LogicDataCenter.voiceSystemDataManager.UserInfoList;

            nMaxCount = userInfoList.Count;

            if (nMaxCount % PerPageCount == 0)
                nMaxPage = nMaxCount / PerPageCount;
            else
                nMaxPage = nMaxCount / PerPageCount + 1;

            nCurrentPage = Mathf.Clamp(nCurrentPage, 1, nMaxPage);

            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        public void ReceiveSearchData()
        {
            nCurrentPage = 1;
            nMaxPage = 1;
            nMaxCount = 0;

            userInfoList = LogicDataCenter.voiceSystemDataManager.TempUserInfoList;
            nMaxCount = userInfoList.Count;
            if (nMaxCount % PerPageCount == 0)
                nMaxPage = nMaxCount / PerPageCount;
            else
                nMaxPage = nMaxCount / PerPageCount + 1;

            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        public void SwitchEarphoneState(bool bOpen)
        {
            if(bOpen)
            {
                if (earphoneBtn != null && voiceView.earphoneStateIcons[0] != null)
                    earphoneBtn.GetComponent<Image>().sprite = voiceView.earphoneStateIcons[0];
            }
            else
            {
                if (earphoneBtn != null && voiceView.earphoneStateIcons[1] != null)
                    earphoneBtn.GetComponent<Image>().sprite = voiceView.earphoneStateIcons[1];
            }
        }

        public void SwitchMicphoneState(bool bForbid, bool bOpen)
        {
            if (micphoneBtn != null)
            {
                micphoneBtn.interactable = !bForbid;
                micphoneBtn.transform.FindChild("OpenIcon").gameObject.SetActive(bOpen);
            }
        }

        private void FillUserData(int page)
        {
            if (null == roomMemberList)
                return;

            roomMemberList.DataSource.Clear();
            int count = PerPageCount;
            if (nCurrentPage == nMaxPage)
                count = nMaxCount % PerPageCount;
            int startIndex = (nCurrentPage - 1) * PerPageCount;

            for (int i = 0; i < count; i++)
            {
                UListItemVoiceUser user = new UListItemVoiceUser();
                if (userInfoList.Count >= startIndex + 1)
                {
                    user.userID = userInfoList[startIndex].nUDBID;
                    user.isSpeaking = LogicDataCenter.voiceSystemDataManager.GetSpeakingState(userInfoList[startIndex].nClientID); 
                    user.vest = userInfoList[startIndex].nUserType;
                    user.sex = userInfoList[startIndex].nSex;
                    user.playerName = userInfoList[startIndex].szUserName;
                    user.rank = userInfoList[startIndex].nRankLevel;
                    user.kinName = userInfoList[startIndex].szKinName;
                    roomMemberList.DataSource.Add(user);
                    startIndex++;
                }
            }
        }

        private void RefreshPageBtnState()
        {
            if (prevPageBtn == null || nextPageBtn == null || firstPageBtn == null || lastPageBtn == null)
                return;

            if (nCurrentPage == 1)
            {
                prevPageBtn.interactable = false;
                firstPageBtn.interactable = false;
            }
            else
            {
                prevPageBtn.interactable = true;
                firstPageBtn.interactable = true;
            }
            if (nCurrentPage == nMaxPage)
            {
                nextPageBtn.interactable = false;
                lastPageBtn.interactable = false;
            }
            else
            {
                nextPageBtn.interactable = true;
                lastPageBtn.interactable = true;
            }
        }

        private void ReqUserMemberListFirstPage()
        {
            if (nCurrentPage == 1)
                return;
            nCurrentPage = 1;
            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        private void ReqUserMemberListLastPage()
        {
            if (nCurrentPage == nMaxPage)
                return;
            nCurrentPage = nMaxPage;
            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        private void ReqUserMemberListPrevPage()
        {
            nCurrentPage = Mathf.Clamp(nCurrentPage - 1, 1, nMaxPage);
            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        private void ReqUserMemberListNextPage()
        {
            nCurrentPage = Mathf.Clamp(nCurrentPage + 1, 1, nMaxPage);
            FillUserData(nCurrentPage);
            RefreshPageBtnState();
        }

        private void OnSearchValueChanged(string value)
        {
            if (value.Equals(string.Empty))
            {
                ClearRoomMember();
                UpdateRoomMemberList();
            }
            else
            {
                bIsSearching = true;
                LogicDataCenter.voiceSystemDataManager.ReqSearchUserInfo(value);
            }
        }

        private void OnEarphoneClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchEarphoneState();
        }

        private void OnMicphoneClick()
        {
            if (voiceView != null)
                voiceView.RequestSwitchMicphoneState();
        }
    }

    public class VoiceSystemView : UIBaseWndView
    {
        //UI控件
        public RectTransform mainFrame;
        public Text mainTitle;
        public Button closeBtn;
        public Sprite[] earphoneStateIcons;

        public RectTransform inputFieldFrame;
        public InputField inputField;
        public Button inputFieldConfirmBtn;
        public Button inputFieldCancelBtn;
        public Text inputFieldTitle;
        public Text inputFieldDesc;
        public Text confirmLabel;
        public Text cancelLabel;

        //挂接子物体
        public VoiceMiniPanel miniPanel;
        public VoiceMainFrameChannelGroup mainFrameChannelGroup;
        public VoiceMainFrameRoomGroup mainFrameRoomGroup;
        public VoiceMicOrderPanel micOrderPanel;
        public VoiceRoomMemerPanel roomMemberPanel;

        //频道冷却时间
        [Range(0f,10f)]
        public float SwitchRoomCD = 3.0f;
        public Vector3[] MiniPanelPos = new Vector3[2];

        public int CurrentChannel { get { return nCurrentChannel; } }
        private int nCurrentChannel;

        public int CurrentRoom { get { return nCurrentRoom; } }
        private int nCurrentRoom;

        public string CurrentChannelName
        {
            get { return GetChannelName((EMVoiceChannelType)nCurrentChannel); }
        }

        public string CurrentRoomName
        {
            get { return GetRoomName((EMVoiceRoomType)nCurrentRoom); }
        }

        private int nSpeackMode;
        private int nCurrentMicOrderMode;
        //private int nCurrentMicOrderInterval;  //当前麦序时间
        private int nTempRoomID;                   
        private float switchChannelCDTimer;   //切频道冷却计时器
        private EMVoiceRoomSettingType roomSettingType;

        private bool bEnableManageMicOrderMode;    //是否有权限管理麦序
        private bool bEnableSwitchChannel;                //切频道是否冷却完毕
        private bool bForbidMicphone;                        //是否禁麦
        private bool bOpenMicphone;                          //是否开麦克风
        private bool bOpenEarphone;                          //是否开耳麦
        private bool bInMicOrder;                                //是否在麦序上

        private string szSetpassword;
        private string szSetpasswordDesc;
        private string szEnterLockRoom;
        private string szEnterLockRoomDesc;
        private string szSetMicOrderTime;
        private string szSetMicOrderTimeDesc;
        private string szConfirm;
        private string szCancel;

        private Dictionary<EMVoiceChannelType, string> dicChannelName = new Dictionary<EMVoiceChannelType, string>();
        private Dictionary<EMVoiceRoomType, string> dicRoomName = new Dictionary<EMVoiceRoomType, string>();

        public override bool Init(IUIWnd wnd)
        {
            //载入配置名称
            LoadChannelAndRoomName();

            mainTitle.text = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "mainTitle");

            szSetpassword = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szSetpassword");
            szSetpasswordDesc = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szSetpasswordDesc");
            szEnterLockRoom = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szEnterLockRoom");
            szEnterLockRoomDesc = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szEnterLockRoomDesc");
            szSetMicOrderTime = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szSetMicOrderTime");
            szSetMicOrderTimeDesc = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szSetMicOrderTimeDesc");
            szConfirm = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szConfirm");
            szCancel = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szCancel");

            miniPanel.Init(this);
            miniPanel.ClearMiniPanelData();

            mainFrameChannelGroup.Init(this);
            mainFrameChannelGroup.ClearMainChannelGroupState();

            mainFrameRoomGroup.Init(this);
            mainFrameRoomGroup.ClearMainRoomGroupState();

            micOrderPanel.Init(this);
            micOrderPanel.ClearMicOrder();

            roomMemberPanel.Init(this);
            roomMemberPanel.ClearRoomMember();

            if (null != mainFrame)
                mainFrame.gameObject.SetActive(false);

            nSpeackMode = 0;
            nCurrentChannel = 0;
            nCurrentRoom = 0;
            nCurrentMicOrderMode = 0;
            switchChannelCDTimer = 0f;
            //nCurrentMicOrderInterval = 0;
            bOpenEarphone = true;
            bInMicOrder = false;
            nTempRoomID = 0;
            roomSettingType = EMVoiceRoomSettingType.None;

            closeBtn.onClick.AddListener(OnMainFrameCloseBtnClick);
            inputFieldConfirmBtn.onClick.AddListener(OnInputFieldConfirmBtnClick);
            inputFieldCancelBtn.onClick.AddListener(OnInputFieldCancelBtnClick);

            confirmLabel.text = szConfirm;
            cancelLabel.text = szCancel;

            return base.Init(wnd);
        }

        public override void Update() 
        {
            base.Update();

            //监控按键
            if (!bForbidMicphone && nSpeackMode == 0)
            {
                if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.VoiceOpenMic))
                {
                    cmd_voice_req_open_mic mic = new cmd_voice_req_open_mic();
                    mic.isOpen = 1;
                    ViewEventHelper.Instance.SendCommand<cmd_voice_req_open_mic>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_OPEN_MIC, ref mic);
                }
                else if (FuntionShortCutCtrl.CheckFunctionShortcutKeyUp(FuntionShortCutSlotIndex.VoiceOpenMic))
                {
                    cmd_voice_req_open_mic mic = new cmd_voice_req_open_mic();
                    mic.isOpen = 0;
                    ViewEventHelper.Instance.SendCommand<cmd_voice_req_open_mic>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_OPEN_MIC, ref mic);
                }
            }

            //频道切换CD计时
            if (!bEnableSwitchChannel)
            {
                switchChannelCDTimer += Time.deltaTime;
                if (switchChannelCDTimer > SwitchRoomCD)
                {
                    bEnableSwitchChannel = true;
                    switchChannelCDTimer = 0f;
                }
            }
        }

        //更新频道列表
        public void UpdateChannelList()
        {
            miniPanel.UpdateMiniPanelChannelState();
            mainFrameChannelGroup.UpdateMainFrameChannelState();
        }

        public void EnterRoom()
        {
            nCurrentMicOrderMode = 0;
            //nCurrentMicOrderInterval = 0;
            bInMicOrder = false;
            bEnableManageMicOrderMode = false;
            bEnableSwitchChannel = false;
            bForbidMicphone = true;
            bOpenMicphone = false;
            nTempRoomID = 0;
            roomSettingType = EMVoiceRoomSettingType.None;

            if (inputFieldFrame != null)
                inputFieldFrame.gameObject.SetActive(false);

            mainFrameChannelGroup.ClearMainChannelGroupState();
            mainFrameRoomGroup.ClearMainRoomGroupState();
            micOrderPanel.ClearMicOrder();
            roomMemberPanel.ClearRoomMember();

            int tempChannelID = nCurrentChannel;
            nCurrentChannel = LogicDataCenter.voiceSystemDataManager.CurrentChannelType;
            nCurrentRoom = LogicDataCenter.voiceSystemDataManager.CurrentRoomType;

            if(miniPanel != null)
                miniPanel.EnterRoom((EMVoiceChannelType)nCurrentChannel, (EMVoiceRoomType)nCurrentRoom);

            if (mainFrameChannelGroup != null)
                mainFrameChannelGroup.SelectChannel((EMVoiceChannelType)nCurrentChannel);

            if (mainFrameRoomGroup != null)
                mainFrameRoomGroup.SelectRoom((EMVoiceRoomType)nCurrentRoom);

            SetRoomTitle();

            if(tempChannelID != nCurrentChannel)
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_VOICE_ENTER_ROOM_SUCCESS, CurrentChannelName);
        }

        public void QuitVoice()
        {
            if (null != mainFrame)
                mainFrame.gameObject.SetActive(false);

            miniPanel.QuitVoice();
            mainFrameChannelGroup.ClearMainChannelGroupState();
            mainFrameRoomGroup.ClearMainRoomGroupState();
            micOrderPanel.ClearMicOrder();
            roomMemberPanel.ClearRoomMember();

            nCurrentChannel = 0;
            nCurrentRoom = 0;
            bInMicOrder = false;
        }

        public void EnterBattleState(bool _bEnter)
        {
            if (miniPanel == null)
                return;

            ((miniPanel.transform) as RectTransform).anchoredPosition = _bEnter ? MiniPanelPos[0] : MiniPanelPos[1];
        }

        public void UpdateRoomBaseInfo(VoiceRoomBaseInfoData roomBaseInfo)
        {
            miniPanel.UpdateMiniPanelRoomInfo(roomBaseInfo);
            mainFrameRoomGroup.UpdateMainRoomInfo(roomBaseInfo);
        }

        //更新权限信息
        public void UpdateRightInfo(VoiceRoomRigthInfo rightInfo)
        {
            //更新耳麦状态
            if (bOpenEarphone != rightInfo.bOpenEarphone)
            {
                bOpenEarphone = rightInfo.bOpenEarphone;
                if(miniPanel != null)
                    miniPanel.SwtichMiniEarphoneState(bOpenEarphone);
                if (roomMemberPanel != null)
                    roomMemberPanel.SwitchEarphoneState(bOpenEarphone);
            }

            //更新麦序管理状态
            if(bEnableManageMicOrderMode != rightInfo.bManageMicMode)
            {
                bEnableManageMicOrderMode = rightInfo.bManageMicMode;
                if (micOrderPanel != null)
                    micOrderPanel.ChangeMicOrderManageState(bEnableManageMicOrderMode);
                if (bEnableManageMicOrderMode)
                    mainFrameRoomGroup.AddRoomPopUpAction(EMVoiceRoomSettingType.SetMicOrderTime);
                else
                    mainFrameRoomGroup.RemoveRoomPopUpAction(EMVoiceRoomSettingType.SetMicOrderTime);
            }

            //更新麦序模式
            if (nCurrentMicOrderMode != rightInfo.nMicMode)
            {
                nCurrentMicOrderMode = rightInfo.nMicMode;
                if (micOrderPanel != null)
                    micOrderPanel.ChangeMicOrderMode(nCurrentMicOrderMode);
            }

            //更新麦克风状态
            if (bForbidMicphone != rightInfo.bForbidMic || bOpenMicphone != rightInfo.bOpenMicphone)
            {
                bForbidMicphone = rightInfo.bForbidMic;
                bOpenMicphone = rightInfo.bOpenMicphone;
                if (miniPanel != null)
                    miniPanel.SwitchMiniMicphoneState(bForbidMicphone, bOpenMicphone);
                if(micOrderPanel != null)
                    micOrderPanel.SwitchMicphoneState(bForbidMicphone, bOpenMicphone);
                if (roomMemberPanel != null)
                    roomMemberPanel.SwitchMicphoneState(bForbidMicphone, bOpenMicphone);
            }

            bInMicOrder = rightInfo.bInMicOrder;
            if (micOrderPanel != null)
            {
                micOrderPanel.ActiveMicOrderTimer(bInMicOrder);
                micOrderPanel.SwitchGraspMicState(rightInfo.bEnableGraspMic, rightInfo.bInMicOrder);
            }

            if (!bForbidMicphone && nSpeackMode == (int)EMVoiceMicState.VoiceMicState_MicFree)
            {
                cmd_voice_req_open_mic mic = new cmd_voice_req_open_mic();
                mic.isOpen = 1;
                ViewEventHelper.Instance.SendCommand<cmd_voice_req_open_mic>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_OPEN_MIC, ref mic);
            }
        }

        //更新麦序列表
        public void UpdateMicOrder()
        {
            micOrderPanel.UpdateMicOrderList();
        }

        //更新房间人员列表
        public void UpdateUserInfo()
        {
            roomMemberPanel.UpdateRoomMemberList();
        }

        //接受搜索返回数据
        public void ReceiveUserSearchData()
        {
            roomMemberPanel.ReceiveSearchData();
        }

        public void RequestEnterChannel(EMVoiceChannelType type)
        {
            //如果冷却完成，则切频道
            if (bEnableSwitchChannel)
            {
                LogicDataCenter.voiceSystemDataManager.RequestEnterRoom((int)type, (int)EMVoiceRoomType.VoiceRoomType_Main);
            }
            else
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_VOICE_FREQUENT_SWITCH, SwitchRoomCD.ToString());
            }
        }

        public void RequestEnterRoom(EMVoiceRoomType type)
        {
            //判断房间是否有密码
            if (LogicDataCenter.voiceSystemDataManager.GetRoomLockState((int)type))
                ShowInputFieldFrame(EMVoiceRoomSettingType.EnterLockRoom);
            else
                LogicDataCenter.voiceSystemDataManager.RequestEnterRoom(nCurrentChannel, (int)type);
        }

        public void RequestSwitchEarphoneState()
        {
            cmd_voice_req_change_mute_state state = new cmd_voice_req_change_mute_state();
            state.isMute = bOpenEarphone ? 1 : 0;
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_change_mute_state>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE, ref state);
        }

        public void RequestSwitchMicphoneState()
        {
            cmd_voice_req_open_mic mic = new cmd_voice_req_open_mic();
            mic.isOpen = bOpenMicphone ? 0 : 1;
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_open_mic>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_OPEN_MIC, ref mic);
        }

        public void RequestSwitchGraspMicState()
        {
            cmd_voice_req_mic_order mic = new cmd_voice_req_mic_order();
            mic.isAcquire = bInMicOrder ? 0 : 1;
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_mic_order>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_MIC_ORDER, ref mic);
        }

        public void RequestChangeSpeakMode(int mode)
        {
            nSpeackMode = mode;
            cmd_voice_req_open_mic mic = new cmd_voice_req_open_mic();
            mic.isOpen = mode == 0 ? 0 : 1;
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_open_mic>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_OPEN_MIC, ref mic);
        }

        public string GetChannelName(EMVoiceChannelType type)
        {
            if (dicChannelName.ContainsKey(type))
                return dicChannelName[type];
            return "";
        }

        public string GetRoomName(EMVoiceRoomType type)
        {
            if (dicRoomName.ContainsKey(type))
                return dicRoomName[type];
            return "";
        }

        public void SwitchMainFrameHideState()
        {
            if (mainFrame != null)
                mainFrame.gameObject.SetActive(!mainFrame.gameObject.activeInHierarchy);
        }

        public void ShowInputFieldFrame(EMVoiceRoomSettingType settingType)
        {
            roomSettingType = settingType;

            if (inputField != null && inputFieldTitle != null && inputFieldDesc != null)
            {
                inputField.text = "";
                if (roomSettingType == EMVoiceRoomSettingType.SetMicOrderTime)
                {
                    inputField.contentType = InputField.ContentType.IntegerNumber;
                    inputFieldTitle.text = szSetMicOrderTime;
                    inputFieldDesc.text = szSetMicOrderTimeDesc;
                }
                else if (roomSettingType == EMVoiceRoomSettingType.AddLock)
                {
                    inputField.contentType = InputField.ContentType.Password;
                    inputFieldTitle.text = szSetpassword;
                    inputFieldDesc.text = szSetpasswordDesc;
                }
                else if (roomSettingType == EMVoiceRoomSettingType.EnterLockRoom)
                {
                    inputField.contentType = InputField.ContentType.Password;
                    inputFieldTitle.text = szEnterLockRoom;
                    inputFieldDesc.text = szEnterLockRoomDesc;
                }
            }

            if (inputFieldFrame != null)
                inputFieldFrame.gameObject.SetActive(true);
        }

        public void SetReqSettingRoomID(int roomID)
        {
            nTempRoomID = roomID;
        }

        private void LoadChannelAndRoomName()
        {
            string[] channelName = {"Channel_None","Channel_GM", "Channel_World", "Channel_Country", "Channel_Clan", "Channel_Kin", "Channel_Group", "Channel_Team", "Channel_War", "Channel_Personal" };
            for(int i = (int)EMVoiceChannelType.VoiceChannel_None; i < (int)EMVoiceChannelType.VoiceChannel_Max; i++)
            {
                dicChannelName.Add((EMVoiceChannelType)i, ULocalizationService.Instance.Get("UIView", "VoiceSystemView", channelName[i]));
            }

            string[] roomName = { "Room_Main", "Room_PK", "Room_KTV", "Room_Chat", "Room_Hang", "Room_Meeting", "Room_Office", "Room_Private" };
            for (int i = (int)EMVoiceRoomType.VoiceRoomType_Main; i < (int)EMVoiceRoomType.VoiceRoomType_Max; i++)
            {
                dicRoomName.Add((EMVoiceRoomType)i, ULocalizationService.Instance.Get("UIView", "VoiceSystemView", roomName[i]));
            }
        }

        private void SetRoomTitle()
        {
            if (mainTitle != null)
                mainTitle.text = CurrentChannelName + "频道" + "——" + CurrentRoomName + " [语音中]";
        }

        private void OnMainFrameCloseBtnClick()
        {
            if (mainFrame != null)
                mainFrame.gameObject.SetActive(false);
        }

        private void OnInputFieldConfirmBtnClick()
        {
            if (inputField == null)
                return;

            if (roomSettingType == EMVoiceRoomSettingType.EnterLockRoom)
                LogicDataCenter.voiceSystemDataManager.RequestEnterRoom(nCurrentChannel, nTempRoomID, inputField.text);
            else if (roomSettingType == EMVoiceRoomSettingType.AddLock)
            {
                cmd_voice_req_lock_room lockRoom = new cmd_voice_req_lock_room();
                lockRoom.nRoomID = nTempRoomID;
                lockRoom.isLock = 1;
                lockRoom.szPassword = inputField.text;
                ViewEventHelper.Instance.SendCommand<cmd_voice_req_lock_room>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_LOCK_ROOM, ref lockRoom);
            }
            else if (roomSettingType == EMVoiceRoomSettingType.SetMicOrderTime)
            {
                cmd_voice_req_set_mic_order_time time = new cmd_voice_req_set_mic_order_time();
                time.nRoomID = nTempRoomID;
                if(inputField.text != string.Empty)
                    time.nSec = Convert.ToInt32(inputField.text);
                ViewEventHelper.Instance.SendCommand<cmd_voice_req_set_mic_order_time>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME, ref time);
            }

            if (inputFieldFrame != null)
                inputFieldFrame.gameObject.SetActive(false);
        }

        private void OnInputFieldCancelBtnClick()
        {
            if (inputFieldFrame != null)
                inputFieldFrame.gameObject.SetActive(false);
        }
    }
}
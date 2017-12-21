using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.ShieldWorld;
using GameLogic;
using USpeedUI.UWidgets;
using UIWidgets;
using ASpeedGame.Data.PersonModelTransFormInfo;
using USpeedUI.UExtensions;
using UnityEngine.EventSystems;
using USpeedUI.UEffect;
using ASpeedGame.Data.HeroTypeConfig;
using Common.FSMSample;
using System;

namespace USpeedUI.LoginAndCreate
{
    enum ECreateHero : uint
    {
        ECH_Name,
        ECH_Hero
    }

    public class UMediatorCreateHero : UIFillWnd<UViewCreateHero>
    {
        public Transform SceneRoot
        {
            get
            {
                if (m_wndView != null)
                {
                    return m_wndView.SceneRoot;
                }
                return null;
            }
        }


        #region 重载父类方法
        public override WndID GetID()
        {
            return WndID.WND_ID_CREATEHERO;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override string GetPrefabPath()
        {
            return UPath.ULoginState + "/CreateHero/CreateHeroView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RANDOMNAMERESULT, this);

            return base.Init();
        }
        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_RANDOMNAMERESULT, this);
        }

        private void SceneLoadFinish(SceneResInfoManager manager)
        {
            SetVisible(true);

            if (m_wndView)
            {
                m_wndView.SceneRoot = manager.SceneRoot.transform;
            }
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_OPEN:
                    {
                        SceneManager.Instance.LoadSelectHeroScene(SceneLoadFinish);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE:
                    {
                        UnloadView();
                        SceneManager.Instance.UnLoadSelectHeroScene();
                    }
                    break;

                case WndMsgID.WND_MSG_CREATEHERO_RANDOMNAMERESULT:
                    {
                        UIMsgCmdData uiData = (UIMsgCmdData)data;
                        game_player_request_name_result cmdData = IntPtrHelper.toData<game_player_request_name_result>(uiData.ptrParam);
                        if (m_wndView != null)
                        {
                            DataCenter.CreateHeroDataManager.Instance.PlayerName = cmdData.szName;
                            DataCenter.CreateHeroDataManager.Instance.PlayerSex = (PERSON_SEX)cmdData.nSex;
                            m_wndView.SetRandomName();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_CREATE_PLAYER_RESULT:
                    {
                        if(m_wndView != null)
                        {
                            UIMsgCmdData uiData = (UIMsgCmdData)data;

                            gamelogic_create_player_result cmdData = IntPtrHelper.toData<gamelogic_create_player_result>(uiData.ptrParam);
                            if(cmdData.nResult == 1)
                            {
                                m_wndView.SwitchState(ECreateHero.ECH_Hero);
                            }
                        }
                    }
                    break;


                default:
                    break;
            }
        }
        #endregion

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (_bVisible)
            {
                SoundManager.CreateUISound((int)SoundIDIndex.MapLogin);

            }
            else
            {
                SoundManager.DeleteSound((int)SoundIDIndex.MapLogin);
            }
        }

        // 重載关闭操作处理
        public override bool OnCloseKey()
        {
            return base.OnCloseKey();
        }

        internal void onFallBackToLoginView()
        {
            UnloadView();
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_RETURN_LOGIN);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CREATEHERO_FALLBACK, null);

            DataCenter.CreateHeroDataManager.Instance.PlayerSex = PERSON_SEX.SEX_MAX;
        }
        internal void onClickRandomNameButton()
        {
            game_player_request_name data = new game_player_request_name();
            // 新旧性别临时全部定为1
            data.nSex = (byte)DataCenter.CreateHeroDataManager.Instance.PlayerSex;
            data.nOldSex = data.nSex;//旧临时性别是啥？           

            data.szOldName = DataCenter.CreateHeroDataManager.Instance.PlayerName;
            data.nFlag = 0;
            ViewEventHelper.Instance.SendCommand<game_player_request_name>(GameLogicDef.GVIEWCMD_USER_RANDOM_PLAYER_NAME, ref data);
        }

        internal void onClickSubFrame01ConfirmBtn()
        {
            gameview_create_player data = new gameview_create_player();
            data.heroid = 0;
            data.name = DataCenter.CreateHeroDataManager.Instance.PlayerName;
            // 皮肤id暂时按照英雄id来定 之后修改
            data.skinid = 0;
            data.sex = (int)DataCenter.CreateHeroDataManager.Instance.PlayerSex;

            ViewEventHelper.Instance.SendCommand<gameview_create_player>(GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER, ref data);

        }
        internal void onClickSubFrame02ConfirmBtn()
        {
            if (DataCenter.CreateHeroDataManager.Instance.PlayerCreateHeroID == -1)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_HEROCREATE_BADID);
                return;
            }

            gameview_create_player_extend data = new gameview_create_player_extend();
            data.heroid = DataCenter.CreateHeroDataManager.Instance.PlayerCreateHeroID;
            data.skinid = 0;

            ViewEventHelper.Instance.SendCommand<gameview_create_player_extend>(GameLogicDef.GVIEWCMD_USER_CREATE_PLAYER_EXTEND, ref data);
        }

    }

    public class UViewCreateHero : UIBaseWndView
    {
        protected UMediatorCreateHero mediator;

        /// <summary>
        /// 三维加载场景的根节点
        /// </summary>
        public Transform SceneRoot;
        /// <summary>
        /// 状态列表
        /// </summary>
        public UCustomUIBehaviour[] StateList;
        /// <summary>
        /// 当前状态
        /// </summary>
        internal IFSM_Base CurrentState = null;
        /// <summary>
        /// 状态数据
        /// </summary>
        internal SCreateHeroParam StateData;
        /// <summary>
        /// 返回登录界面按钮
        /// </summary>
        public Button BtnReturn;
        
        /// <summary>
        /// 切换界面状态
        /// </summary>
        /// <param name="_newState"></param>
        internal void SwitchState(ECreateHero _newState)
        {
            if (CurrentState != null)
                CurrentState.Exit(StateData);

            LeaveState();
            CurrentState = StateList[(uint)_newState] as IFSM_Base;
            EnterState();

            CurrentState.Enter(StateData);
        }
        public override bool Init(IUIWnd wnd)
        {
            mediator = wnd as UMediatorCreateHero;

            if (StateList == null || StateList.Length < 2)
            {
                Trace.LogWarning("StateList 未初始化。");
            }

            StateData.Mediator = mediator;

            (StateList[(uint)ECreateHero.ECH_Name] as IFSM_Base).Exit(StateData);
            (StateList[(uint)ECreateHero.ECH_Hero] as IFSM_Base).Exit(StateData);

            SwitchState(ECreateHero.ECH_Name);

            return base.Init(wnd);
        }
        public override void Update()
        {
            base.Update();

            if (CurrentState != null)
                CurrentState.Execute(StateData);

        }

        /// <summary>
        /// 进入状态主界面操作
        /// </summary>
        protected void EnterState()
        {
            if (CurrentState.GetStateID() == (uint)ECreateHero.ECH_Hero)
                BtnReturn.gameObject.SetActive(false);
            else
                BtnReturn.gameObject.SetActive(true);
        }
        /// <summary>
        /// 离开状态主界面操作
        /// </summary>
        protected void LeaveState()
        {
        }


        /// <summary>
        /// 点击返回按钮
        /// </summary>
        public void onClickFallBackBtn()
        {
               mediator.onFallBackToLoginView();
        }
        /// <summary>
        /// 设置随机名字
        /// </summary>
        public void SetRandomName()
        {
            (StateList[(uint)ECreateHero.ECH_Name] as UViewCreateHeroState_Name).SetRandomName();
        }
    }
    /// <summary>
    /// 状态传递参数
    /// </summary>
    struct SCreateHeroParam : IFSMParam
    {
        public UMediatorCreateHero Mediator;
    }
    /// <summary>
    /// 创建角色名字状态
    /// </summary>
    public class UViewCreateHeroState_Name : UCustomUIBehaviour, IFSM_Base
    {
        /// <summary>
        /// 第一个子界面确定按钮
        /// </summary>
        public Selectable BtnConfirm01;
        /// <summary>
        /// 性别男按钮
        /// </summary>
        public UButtonGroupItem BtnSexBoy;
        /// <summary>
        /// 性别女按钮
        /// </summary>
        public UButtonGroupItem BtnSexGirl;
        /// <summary>
        /// 随机名字按钮
        /// </summary>
        public Button BtnRandonName;
        /// <summary>
        /// 用户名输入控件
        /// </summary>
        public InputField UserNameField;
        /// <summary>
        /// 用户名错误原因提示
        /// </summary>
        public Text ErrorHint;


        /// <summary>
        /// 界面中介
        /// </summary>
        protected UMediatorCreateHero m_viewMediator;
        /// <summary>
        /// 确定按钮特效
        /// </summary>
     //   private UCreateHeroConfirmSelectableHelper m_btnConfirm01Helper;

        public void onClickSubFrame01ConfirmBtn()
        {
            DataCenter.CreateHeroDataManager.Instance.PlayerName = UserNameField.textComponent.text;
            m_viewMediator.onClickSubFrame01ConfirmBtn();
        }
        /// <summary>
        /// 角色名输入事件
        /// </summary>
        /// <param name="sText"></param>
        public void OnInputPlayerTextChange(string sText)
        {
            if (ShieldWord.Instance.IsStringMaxLenRight(sText))
            {
                DataCenter.CreateHeroDataManager.Instance.PlayerName = sText;
            }
            else
            {
                if (!string.IsNullOrEmpty(DataCenter.CreateHeroDataManager.Instance.PlayerName))
                    UserNameField.text = DataCenter.CreateHeroDataManager.Instance.PlayerName;
            }

            List<string> shieldWordList;
            string text = sText;
            if (string.IsNullOrEmpty(text))
            {
                ErrorHint.text = "";
            }
            else if (sText.Contains(" "))
            {
                ErrorHint.text = ULocalizationService.Instance.Get("UIView", "CreateHeroView", "ErrorHint4");
            }
            else if (!ShieldWord.Instance.IsStringMinLenRight(text))
            {
                ErrorHint.text = ULocalizationService.Instance.Get("UIView", "CreateHeroView", "ErrorHint1");
            }
            else if (text[0] >= 0x30 && text[0] <= 0x40)
            {
                ErrorHint.text = ULocalizationService.Instance.Get("UIView", "CreateHeroView", "ErrorHint3");
            }
            else if (ShieldWord.Instance.IsHaveShieldWordEx(text, out shieldWordList))
            {
                ErrorHint.text = string.Format(ULocalizationService.Instance.Get("UIView", "CreateHeroView", "ErrorHint2"), string.Join("、", shieldWordList.Distinct().ToArray()));
            }
            else
            {
                ErrorHint.text = "";
            }
        }

        /// <summary>
        /// 设置随机名字显示
        /// </summary>
        internal void SetRandomName()
        {
            UserNameField.text = DataCenter.CreateHeroDataManager.Instance.PlayerName;
        }

        /// <summary>
        /// 性别按钮点击事件
        /// </summary>
        /// <param name="_nSexType">0是男性，1是女性</param>
        public void onClickSexTypeButton(int _nSexType)
        {
            DataCenter.CreateHeroDataManager.Instance.PlayerSex = (PERSON_SEX)_nSexType;
        }
        /// <summary>
        /// 随机名字按钮点击事件
        /// </summary>
        public void onClickRandomNameButton()
        {
            if (string.IsNullOrEmpty(UserNameField.textComponent.text))
                DataCenter.CreateHeroDataManager.Instance.PlayerName = UserNameField.placeholder.GetComponent<Text>().text;
            else
                DataCenter.CreateHeroDataManager.Instance.PlayerName = UserNameField.textComponent.text;
            if (DataCenter.CreateHeroDataManager.Instance.PlayerSex == PERSON_SEX.SEX_MAX)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_HEROCREATE_BADSEX);
                return;
            }
            m_viewMediator.onClickRandomNameButton();
        }

        protected void Init(IFSMParam _param)
        {
            m_viewMediator = ((SCreateHeroParam)_param).Mediator;

            //m_btnConfirm01Helper = BtnConfirm01.gameObject.AddComponent<UCreateHeroConfirmSelectableHelper>();
            //m_btnConfirm01Helper.SetupHelper(UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Up, UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Down,
            //                   UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Over, UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_CanInteractable);

            (BtnConfirm01 as Button).onClick.AddListener(onClickSubFrame01ConfirmBtn);
            BtnSexBoy.onSelectedChanged.AddListener((bSelected) => { if (bSelected) onClickSexTypeButton(0); });
            BtnSexGirl.onSelectedChanged.AddListener((bSelected) => { if (bSelected) onClickSexTypeButton(1); });
            UserNameField.onValueChanged.AddListener(OnInputPlayerTextChange);
            BtnRandonName.onClick.AddListener(onClickRandomNameButton);

            BtnSexBoy.Selected = true;
            ErrorHint.text = "";
            
        }

        public int Enter(IFSMParam _data)
        {
            //初始化
            if(m_viewMediator == null)
            {
                Init(_data);
            }


            gameObject.SetActive(true);

            return RunningStatus.FINISHED;
        }

        public int Execute(IFSMParam _data)
        {
            if (!string.IsNullOrEmpty(UserNameField.textComponent.text) && ShieldWord.Instance.IsConformString(UserNameField.textComponent.text) &&
    DataCenter.CreateHeroDataManager.Instance.PlayerSex != PERSON_SEX.SEX_MAX && !ShieldWord.Instance.IsStringHeadCharIsNum(UserNameField.textComponent.text))
            {
                //m_btnConfirm01Helper.enabled = true;
                BtnConfirm01.enabled = true;
                BtnConfirm01.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
            }
            else
            {
                //m_btnConfirm01Helper.ChangeState(UCreateHeroConfirmSelectableHelper.SelectionState.Disabled);
                //m_btnConfirm01Helper.enabled = false;
                BtnConfirm01.enabled = false;
                BtnConfirm01.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
            }

            if (string.IsNullOrEmpty(UserNameField.textComponent.text) && UserNameField.isFocused)
            {
                UserNameField.placeholder.GetComponent<Text>().gameObject.SetActive(false);
            }
            else
            {
                UserNameField.placeholder.GetComponent<Text>().gameObject.SetActive(true);
            }


            return RunningStatus.FINISHED;
        }

        public int Exit(IFSMParam _data)
        {
            gameObject.SetActive(false);
            return RunningStatus.FINISHED;

        }

        public uint GetStateID()
        {
            return (uint)ECreateHero.ECH_Name;
        }
    }
    /// <summary>
    /// 选择英雄状态
    /// </summary>
    public class UViewCreateHeroState_Hero : UCustomUIBehaviour, IFSM_Base
    {
        /// <summary>
        /// 第二个子界面确定按钮
        /// </summary>
        public Selectable BtnConfirm02;
        /// <summary>
        /// 英雄名字，描述父节点
        /// </summary>
        public GameObject TopLeftGameObj;
        /// <summary>
        /// 英雄名字文本
        /// </summary>
        public Text TxtHeroName;
        /// <summary>
        /// 英雄描述文本
        /// </summary>
        public Text TxtHeroDes;
        /// <summary>
        /// 挂接英雄头像的点
        /// </summary>
        public RectTransform HeroPortraitAttach;
        /// <summary>
        /// 英雄头像资源名
        /// </summary>
        public string StrHeroPortraitPrefab = "BtnHeroPortrait";
        /// <summary>
        /// 英雄头像资源节点
        /// </summary>
        private ResNode m_goHeroPortraitResNode = null;
        /// <summary>
        /// 上一次选的英雄皮肤ID
        /// </summary>
        private int m_nLastSkinId;
        /// <summary>
        /// 上一次选的英雄音效ID
        /// </summary>
        private int m_nLastSkinSoundID;
        /// <summary>
        /// 上一次选的英雄出场效果
        /// </summary>
        private SceneEffectParamBase m_LastSkinEffect = null;
        /// <summary>
        /// 英雄头像按钮数组列表
        /// </summary>
        private List<UCreateHeroPortraitBtn> m_listCreateHeroPortrait = new List<UCreateHeroPortraitBtn>();
        /// <summary>
        /// 界面中介
        /// </summary>
        protected UMediatorCreateHero m_viewMediator;

        /// <summary>
        /// 清除选择英雄特效
        /// </summary>
        private void ClearSelectSkinModel()
        {
            if (m_LastSkinEffect == null)
            {
                return;
            }

            SceneEffectManager.Instance.DestroySceneEffect(ref m_LastSkinEffect);
            m_LastSkinEffect = null;
            m_nLastSkinId = 0;
        }
        /// <summary>
        /// 移除所有英雄头像
        /// </summary>
        public void RemoveAllHeroHeadPortrait()
        {
            while (m_listCreateHeroPortrait.Count > 0)
            {
                var portrait = m_listCreateHeroPortrait[0];
                m_listCreateHeroPortrait.Remove(portrait);
                DestroyObject(portrait);
            }
            m_listCreateHeroPortrait.Clear();
        }

        /// <summary>
        /// 点击英雄头像
        /// </summary>
        /// <param name="nHeroID"></param>
        public void onClickHeroHeadPortrait(int nHeroID)
        {
            SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(nHeroID);
            PersonSkinModelItem ModelItem = new PersonSkinModelItem();
            if (config == null || !GameLogicAPI.GetPersonModel((ushort)nHeroID, ref ModelItem) || TxtHeroName == null || TxtHeroDes == null)
                return;

            TopLeftGameObj.SetActive(true);

            TxtHeroName.text = ModelItem.szHeroName;
            TxtHeroDes.text = config.strHeroDesc;
            DataCenter.CreateHeroDataManager.Instance.PlayerCreateHeroID = nHeroID;

            SetChangeHeroSkin(ModelItem.nDefaultSkinID, nHeroID);
        }
        /// <summary>
        /// 切换英雄
        /// </summary>
        /// <param name="nSkinId"></param>
        /// <param name="nHeroId"></param>
        private void SetChangeHeroSkin(int nSkinId, int nHeroId)
        {
            if (nSkinId == m_nLastSkinId)
            {
                return;
            }

            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (GameLogicAPI.GetHeroSkinInfo(nSkinId, ref skinConfig))
            {
                SoundManager.DeleteSound(m_nLastSkinSoundID);
                m_nLastSkinSoundID = skinConfig.nSoundID;
            }


            SkinManager.SetChangeHeroSkinEffect(nSkinId, ref m_LastSkinEffect, ref m_nLastSkinId, m_viewMediator.SceneRoot, ASpeedGame.Data.PersonModelTransFormInfo.PERSONMODELVIEWID.PERSONMODELVIEWID_GAMEWAITINGROOM, OnSkinLoadFinish, this);
        }

        private void OnSkinLoadFinish(object obj)
        {
            SoundManager.CreateUISound(m_nLastSkinSoundID);
        }

        /// <summary>
        /// 添加可选的英雄头像
        /// </summary>
        /// <param name="_nIndex">头像在列表的序号</param>
        /// <param name="_nHeroID">英雄ID</param>
        private void AddHeroHeadPortrait(int _nIndex, int _nHeroID)
        {
            GameObject newHeadPortrait = m_goHeroPortraitResNode.InstanceMainRes();
            if (newHeadPortrait != null)
            {
                UCreateHeroPortraitBtn portrait = newHeadPortrait.GetComponent<UCreateHeroPortraitBtn>();
                portrait.HeroPortrait.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, m_viewMediator.GetID(), 1, 7, _nHeroID);
                portrait.HeroID = _nHeroID;
                var btnPortrait = portrait.gameObject.GetComponent<Button>();

                if (btnPortrait != null)
                {
                    btnPortrait.onClick.AddListener(() => { onClickHeroHeadPortrait(portrait.HeroID); });
                }

                Vector2 newPosition = Vector2.zero;
                portrait.GetRectTransform.SetParent(HeroPortraitAttach, false);

                newPosition.x = ((_nIndex % 2) == 0 ? -1 : 1) * portrait.GetRectTransform.sizeDelta.x / 4;
                newPosition.y = _nIndex == 0 ? 0 : m_listCreateHeroPortrait[_nIndex - 1].GetRectTransform.anchoredPosition.y - portrait.GetRectTransform.sizeDelta.y * 0.8f;
                portrait.GetRectTransform.anchoredPosition = newPosition;


                m_listCreateHeroPortrait.Add(portrait);

            }
        }


        /// <summary>
        /// 点解SubFrame02确定按钮
        /// </summary>
        public void onClickSubFrame02ConfirmBtn()
        {
            StartCoroutine(DelayClickSubFrame02ConfirmBtn());
        }
        protected IEnumerator DelayClickSubFrame02ConfirmBtn()
        {
            yield return new WaitForSeconds(0.5f);
            SoundManager.DeleteSound(m_nLastSkinSoundID);
            m_viewMediator.onClickSubFrame02ConfirmBtn();
        }

        protected void Init(IFSMParam _param)
        {
            m_viewMediator = ((SCreateHeroParam)_param).Mediator;

            BtnConfirm02.interactable = false;


            //var helper = BtnConfirm02.gameObject.AddComponent<UCreateHeroConfirmSelectableHelper>();
            //helper.SetupHelper(UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Up, UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Down,
            //                   UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_Over, UEffectPrefabType.UEPT_CreateHeroView_BtnConfirm_CanInteractable);

            (BtnConfirm02 as Button).onClick.AddListener(onClickSubFrame02ConfirmBtn);

        }
        public int Enter(IFSMParam _data)
        {
            if(m_viewMediator == null)
            {
                Init(_data);
            }

            m_goHeroPortraitResNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + UPath.ULoginState + "/CreateHero/" + StrHeroPortraitPrefab);
            //从配置获取玩家性别可用的英雄列表
            List<int> canCreateHeroList = Data.CreateHeroConfig.CreateHeroConfig.Instance.GetSexCreateHeroList(DataCenter.CreateHeroDataManager.Instance.PlayerSex);

            TopLeftGameObj.SetActive(false);
            canCreateHeroList.ForEach((id, index) => { AddHeroHeadPortrait(index, id); });

            if (DataCenter.CreateHeroDataManager.Instance.PlayerCreateHeroID != -1)
            {
                BtnConfirm02.interactable = true;
            }

            gameObject.SetActive(true);

            return RunningStatus.FINISHED;
        }

        public int Execute(IFSMParam _data)
        {
            if (DataCenter.CreateHeroDataManager.Instance.PlayerCreateHeroID != -1)
            {
                BtnConfirm02.interactable = true;
            }

            return RunningStatus.FINISHED;
        }

        public int Exit(IFSMParam _data)
        {
            gameObject.SetActive(false);
            RemoveAllHeroHeadPortrait();
            ClearSelectSkinModel();
            if (m_goHeroPortraitResNode != null)
            {
                AssetBundleManager.DeleteAssets(ref m_goHeroPortraitResNode, true);
                m_goHeroPortraitResNode = null;
            }

            return RunningStatus.FINISHED;
        }

        public uint GetStateID()
        {
            return (uint)ECreateHero.ECH_Hero;
        }
    }
    
    public class UCreateHeroPortraitBtn : UCustomUIBehaviour
    {
        public Image HeroPortrait;
        public int HeroID;
    }

    #region 先注释掉,新的ui暂时没有光效
    /*public class UCreateHeroConfirmSelectableHelper : UCustomUIBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerDownHandler
    {
        public Selectable ControllerSelectable;
        protected bool m_bOldSelectableInteractable = false;

        protected UEffectParamBase m_PointerDownEffect = null;
        protected UEffectParamBase m_PointerUpEffect = null;
        protected UEffectParamBase m_PointerOverEffect = null;
        protected UEffectParamBase m_InteractiveEffect = null;

        public enum SelectionState
        {
            Normal,
            Highlighted,
            Pressed,
            Disabled
        }

        protected override void Start()
        {
            base.Start();
            if (ControllerSelectable == null)
            {
                ControllerSelectable = gameObject.GetComponent<Selectable>();
            }
        }

        protected override void OnDestroy()
        {
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_PointerDownEffect);
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_PointerUpEffect);
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_PointerOverEffect);
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_InteractiveEffect);
            base.OnDestroy();
        }

        protected override void OnDisable()
        {
            m_bOldSelectableInteractable = false;
            base.OnDisable();
        }

        public void Update()
        {
            if (ControllerSelectable.IsInteractable())
            {
                if (!m_bOldSelectableInteractable)
                    ChangeState(SelectionState.Normal);
                if (m_InteractiveEffect != null)
                    (m_InteractiveEffect as UEffectPrefabParam).EffectGameObject.SetActive(true);
            }
            else
            {
                if (m_InteractiveEffect != null)
                    (m_InteractiveEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
            }
            m_bOldSelectableInteractable = ControllerSelectable.interactable;
        }

        public void SetupHelper(UEffectPrefabType _ePointerUpState, UEffectPrefabType _ePointerDownState, UEffectPrefabType _ePointerOverState, UEffectPrefabType _eCanInteractable)
        {
            if (_ePointerDownState != UEffectPrefabType.UEPT_None)
            {
                m_PointerDownEffect = new UEffectPrefabParam(_ePointerDownState, GetRectTransform);
                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_PointerDownEffect))
                {
                    (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                }
            }

            if (_ePointerUpState != UEffectPrefabType.UEPT_None)
            {
                m_PointerUpEffect = new UEffectPrefabParam(_ePointerUpState, GetRectTransform);
                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_PointerUpEffect))
                {
                    (m_PointerUpEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                }
            }

            if (_ePointerOverState != UEffectPrefabType.UEPT_None)
            {
                m_PointerOverEffect = new UEffectPrefabParam(_ePointerOverState, GetRectTransform);
                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_PointerOverEffect))
                {
                    (m_PointerOverEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                }
            }

            if (_eCanInteractable != UEffectPrefabType.UEPT_None)
            {
                m_InteractiveEffect = new UEffectPrefabParam(_eCanInteractable, GetRectTransform);
                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_InteractiveEffect))
                {
                    (m_InteractiveEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                }
            }
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            ChangeState(SelectionState.Highlighted);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            ChangeState(SelectionState.Normal);
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            ChangeState(SelectionState.Pressed);
        }

        public void ChangeState(SelectionState _eState)
        {
            if (ControllerSelectable != null && !ControllerSelectable.IsInteractable())
                return;

            switch (_eState)
            {
                case SelectionState.Normal:
                    {
                        if (m_PointerUpEffect != null)
                            (m_PointerUpEffect as UEffectPrefabParam).EffectGameObject.SetActive(true);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerOverEffect != null)
                            (m_PointerOverEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                    }
                    break;
                case SelectionState.Highlighted:
                    {
                        if (m_PointerUpEffect != null)
                            (m_PointerUpEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerOverEffect != null)
                            (m_PointerOverEffect as UEffectPrefabParam).EffectGameObject.SetActive(true);
                    }
                    break;
                case SelectionState.Pressed:
                    {
                        if (m_PointerUpEffect != null)
                            (m_PointerUpEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(true);
                        if (m_PointerOverEffect != null)
                            (m_PointerOverEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                    }
                    break;
                case SelectionState.Disabled:
                    {
                        if (m_PointerUpEffect != null)
                            (m_PointerUpEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerDownEffect != null)
                            (m_PointerDownEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                        if (m_PointerOverEffect != null)
                            (m_PointerOverEffect as UEffectPrefabParam).EffectGameObject.SetActive(false);
                    }
                    break;
                default:
                    break;
            }
        }
    }*/
    #endregion
}

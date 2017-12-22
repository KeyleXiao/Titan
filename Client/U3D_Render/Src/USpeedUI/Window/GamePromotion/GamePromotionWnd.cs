using DataCenter;
using System;
using System.Collections.Generic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UnityEngine;
using UnityEngine.UI;
using UWidgets.WebBrowser;
using ASpeedGame.Data.Scheme;
using GameLogic;
using DG.Tweening;
using USpeedUI.UWidgets;
using Data.ActorPrizeConfig;
using ASpeedGame.Data.RecommendPrize;

namespace USpeedUI.GamePromotion
{
    public enum GamePromotionButtonType : int
    {
        BTN_TAITANSAYSAY,
        BTN_WEBLINK,
        BTN_MAX,
    }

    public enum GamePromotionBuiltInMethodType : int
    {
        EMT_TTSAYSAY = 1,
        EMT_PROMOTIONPRIZE,
        EMT_PROMOTIONWAY,
    }

    public abstract class UISubFrameView : MonoBehaviour
    {
        public abstract GamePromotionButtonType GetButtonType();

        public abstract bool Init(GamePromotionView masterView);

        public virtual void ShowFrameView(bool bShow)
        {
            this.gameObject.SetActive(bShow);
        }
    }

    public class GamePromotionWnd : UIPopupWnd<GamePromotionView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_GAME_PROMOTION;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "GamePromotion/GamePromotionPanel";
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_NAV, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_REDIRECT_WEB, this);

            return true;
        }

        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_NAV, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_REDIRECT_WEB, this);

            base.Destroy();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                        {
                            m_wndView.UpdateMyUserInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_NAV:
                    {
                        if (m_wndView != null)
                        {
                            if (data != null)
                                m_wndView.UpdateNavItem(data as UPromotionNavItem);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAME_PROMOTION_REDIRECT_WEB:
                    {
                        if(m_wndView != null)
                        {
                            if (data != null)
                                m_wndView.RedirectWeb(data as UPromotionWebUrl);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public struct PromotionWebUrl
    {
        public int webUrlID;
    }


    public class GamePromotionView : UIBaseWndView
    {
        public Text Title;
        public UButtonGroup NavGroup;
        public GameObject DefaultNavItem;
        public List<UISubFrameView> m_subFrameViewList;

        private List<UButtonGroupItem> m_navItemList;
        private List<int> m_navIDList;
        private Dictionary<int, int> m_builtInNavMethodMap;
        private int m_curNavID;

        public override bool Init(IUIWnd wnd)
        {
            bool ret = base.Init(wnd);

            m_navItemList = new List<UButtonGroupItem>();
            m_navIDList = new List<int>();
            m_builtInNavMethodMap = new Dictionary<int, int>();

            Title.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "PromotionTitle");

            if(NavGroup == null || DefaultNavItem == null)
            {
                Debug.LogError("GamePromotionView: NavGroup == null || DefaultNavItem == null!");
                return false;
            }

            if (!InitSubFrameView())
                return false;

            m_curNavID = 0;
            InitDefaultNavList();
            //ShowSubFrameView(GamePromotionButtonType.BTN_TAITANSAYSAY);
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQUEST_NAVGROUP);

            RequestWebUrl((int)GameWebUrl.TAITAN_MAINHOME);
            RequestWebUrl((int)GameWebUrl.PROMOTION_LINK);

            return ret;
        }

        private bool InitSubFrameView()
        {
            if (m_subFrameViewList == null || m_subFrameViewList.Count != (int)GamePromotionButtonType.BTN_MAX)
            {
                Debug.LogError("游戏界面子视图个数不够");
                return false;
            }

            foreach (UISubFrameView frame in m_subFrameViewList)
            {
                frame.Init(this);
            }

            return true;
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            base.SetRendererVisible(_bVisible);
            foreach (UISubFrameView item in m_subFrameViewList)
            {
                if (item.GetButtonType() == GamePromotionButtonType.BTN_WEBLINK)
                    (item as PromotionWebFrame).RefreshBrowser(_bVisible);
            }
        }

        public void UpdateMyUserInfo()
        {
            foreach (UISubFrameView item in m_subFrameViewList)
            {
                if (item.GetButtonType() == GamePromotionButtonType.BTN_TAITANSAYSAY)
                    (item as TaiTanSaySayFrame).SetMyUserListData();
            }
        }

        public void UpdateNavItem(UPromotionNavItem navItem)
        {
            if (navItem == null)
                return;

            if (navItem.nNavID < 1)
                return;

            int navIndex = m_navIDList.IndexOf(navItem.nNavID);
            if(navIndex > -1)
            {
                UButtonGroupItem btnItem = m_navItemList[navIndex];
                if(btnItem != null)
                {
                    Text navLabel = btnItem.GetComponentInChildren<Text>();
                    if (navLabel != null)
                        navLabel.text = navItem.szNavName;
                }
            }
            else
            {
                GameObject go = ResNode.InstantiateRes(DefaultNavItem);
                if (go == null)
                    return;

                UButtonGroupItem btnItem = go.GetComponent<UButtonGroupItem>();
                if (btnItem == null)
                    return;

                Text navLabel = go.GetComponentInChildren<Text>();
                if (navLabel == null)
                    return;

                navLabel.text = navItem.szNavName;

                go.transform.SetParent(NavGroup.transform, false);
                go.gameObject.SetActive(true);

                btnItem.Group = NavGroup;
                btnItem.GroupIndex = navItem.nNavID;
                btnItem.onSelectedChangedEx.RemoveAllListeners();
                btnItem.onSelectedChangedEx.AddListener(OnNavButtonClicked);

                m_navIDList.Add(navItem.nNavID);
                m_navItemList.Add(btnItem);
            }
        }

        public void RedirectWeb(UPromotionWebUrl webUrlData)
        {
            LogicDataCenter.gamePromotionDataManager.WebUrl[(GameWebUrl)webUrlData.nNavID] = webUrlData.szWebUrl;
            if (webUrlData.nNavID == (int)GameWebUrl.TAITAN_MAINHOME)
            {
                (m_subFrameViewList[(int)GamePromotionButtonType.BTN_TAITANSAYSAY] as TaiTanSaySayFrame).privateLinkText.text = webUrlData.szWebUrl;
            }

            if (webUrlData.nNavID < 0 || webUrlData.nNavID != m_curNavID)
                return;

            foreach (UISubFrameView item in m_subFrameViewList)
            {
                if (item.GetButtonType() == GamePromotionButtonType.BTN_WEBLINK)
                    (item as PromotionWebFrame).Redirection(webUrlData.szWebUrl);
            }
        }

        public void OnCloseBtnClicked()
        {
            SetVisible(false);
        }

        public void InitDefaultNavList()
        {
            m_navItemList.Clear();
            m_navIDList.Clear();
            m_builtInNavMethodMap.Clear();

            List<SchemeWebUrlNode> urlNodeList = SchemeWebUrl.Instance.SchemeWebUrlNodeList;
            for (int i = 0; i < urlNodeList.Count; i++)
            {
                SchemeWebUrlNode node = urlNodeList[i];
                if (node == null)
                    continue;

                GameObject go = ResNode.InstantiateRes(DefaultNavItem);
                if (go == null)
                    continue;

                UButtonGroupItem btnItem = go.GetComponent<UButtonGroupItem>();
                if (btnItem == null)
                    continue;

                Text navLabel = go.GetComponentInChildren<Text>();
                if (navLabel == null)
                    continue;

                if(node.nClientFuncType > 0 && !m_builtInNavMethodMap.ContainsKey(node.nId))
                {
                    m_builtInNavMethodMap.Add(node.nId, node.nClientFuncType);
                }

                navLabel.text = node.szName;

                go.transform.SetParent(NavGroup.transform);
                go.gameObject.SetActive(true);

                btnItem.Group = NavGroup;
                btnItem.GroupIndex = i;
                btnItem.onSelectedChangedEx.RemoveAllListeners();
                btnItem.onSelectedChangedEx.AddListener(OnNavButtonClicked);

                m_navIDList.Add(node.nId);
                m_navItemList.Add(btnItem);

                if (i == 0)
                {
                    m_curNavID = node.nId;
                    btnItem.Selected = true;
                }
            }
        }

        public void OnNavButtonClicked(UButtonGroupItem item, bool bActive)
        {
            if (!bActive || item == null)
                return;

            int navIndex = m_navItemList.IndexOf(item);
            int navId = m_navIDList[navIndex];

            if (m_builtInNavMethodMap.ContainsKey(navId))
            {
                OnBuiltInNavMethod(m_builtInNavMethodMap[navId]);
            }
            else
            {
                OnWebLinkNavMethod(navId);
            }
            m_curNavID = navId;
        }

        private void OnBuiltInNavMethod(int _methodType)
        {
            switch (_methodType)
            {
                case (int)GamePromotionBuiltInMethodType.EMT_TTSAYSAY:
                    {
                        OnTaiTanSaySay();
                    }
                    break;
                default:
                    break;
            }
        }

        private void OnWebLinkNavMethod(int _navId)
        {
            ShowSubFrameView(GamePromotionButtonType.BTN_WEBLINK);
            RequestWebUrl(_navId);
        }

        private void RequestWebUrl(int nUrlID)
        {
            PromotionWebUrl url = new PromotionWebUrl();
            url.webUrlID = nUrlID;
            EntityEventHelper.Instance.SendCommand<PromotionWebUrl>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQUEST_WEBURL, ref url);
        }

        #region 内置方法
        public void OnTaiTanSaySay()
        {
            ShowSubFrameView(GamePromotionButtonType.BTN_TAITANSAYSAY);
            StartCoroutine(LogicDataCenter.gamePromotionDataManager.GetInfoAndUserList());
        }
        #endregion

        private void ShowSubFrameView(GamePromotionButtonType buttonType)
        {
            if (m_subFrameViewList != null)
            {
                foreach (UISubFrameView item in m_subFrameViewList)
                {
                    item.ShowFrameView(item.GetButtonType() == buttonType);
                }
            }
        }
    }
    
    public class DefaultPromotionPrizeItem : MonoBehaviour
    {
        public Text targetText;
        public Button obtainBtn;
        public Text obtainBtnDesc;
        [HideInInspector]
        public List<DefaultPrizeItem> m_prizeIDList = new List<DefaultPrizeItem>();
        private int m_targetID; // 奖励目标ID 

        public bool Init(TaiTanSaySayFrame saysayFrame)
        {
            targetText.text = "";
            obtainBtn.onClick.AddListener(OnObtainBtnClick);

            for (int i = 0; i < saysayFrame.PRIZE_ID_COUNT; ++i)
            {
                GameObject go = Instantiate(saysayFrame.defaultSubPrizeItem.gameObject);
                if (go == null)
                {
                    Trace.LogError("DefaultPrizeItem Instantiate failed");
                    return false;
                }

                go.transform.SetParent(saysayFrame.defaultSubPrizeItem.parent, false);
                go.SetActive(false);
                DefaultPrizeItem item = go.GetComponent<DefaultPrizeItem>();
                if (!item.Init())
                    return false;
                
                m_prizeIDList.Add(item);
            }

            return true;
        }

        public void OnObtainBtnClick()
        {

        }

        public void SetData(int targetID, int totalNumber, List<int> prizeIDList)
        {
            m_targetID = targetID;
            targetText.text = totalNumber.ToString();
            int count = 0;
            if (prizeIDList != null)
            {
                count = prizeIDList.Count < m_prizeIDList.Count ? prizeIDList.Count : m_prizeIDList.Count;
            }

            for (int i = 0; i < count; ++i)
            {
                m_prizeIDList[i].SetData(prizeIDList[i]);
            }
        }
    }

    public class DefaultPrizeItem : MonoBehaviour
    {
        public Text prizeNameText;
        public Image prizeImage;

        public bool Init()
        {
            prizeNameText.text = "";
            prizeImage.gameObject.SetActive(false);

            return true;
        }

        public void SetData(int prizeID)
        {
            SSchemeActorPrizeConfig actorPrizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(prizeID);
            if (actorPrizeConfig != null)
            {
                prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_GAME_PROMOTION, actorPrizeConfig.nPrizeType, actorPrizeConfig.nPrizeIcon);
                prizeNameText.text = actorPrizeConfig.strPrizeName;
             // this.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, actorPrizeConfig.strPrizeDesc);
            }
        }
    }

    public class TaiTanSaySayFrame : UISubFrameView
    {
        public Text privateLinkText;
        public Button copyBtn;
        public Image screenshortImage;
        public Text screenShortText;
        public Image QRCodeImage;
        public Text QRCodeText;

        public Text withdrawText;
        public InputField withdrawInputField;
        public Button withDrawBtn;
        public Text myUserPayInfoText;
        public Text joinedRecentlyText;
        public Button TaiTanLinkBtn;

        public Text accontText;
        public Text totalPayText;
        public Text totalExchangeText;
        public Text createDatetimeText;

        public UListMyUser myUserList;

        #region 总局数
        public Text totalNumGamesLabel;
        public InputField totalNumGamesIF;
        public Button giftBtn;
        public Image redPointImage;
        public Text targetHeaderLabel;
        public Text prizeHeaderLabel;
        public GameObject totalNumberGamesObj;
        public Transform defaultPromotionPrizeItem;
        public Transform defaultSubPrizeItem;
        private GameObject m_prizeFrameObj;

        public readonly int PRIZE_LEVEL_COUNT = 5; // 推广奖励列表最大数量
        public readonly int PRIZE_ID_COUNT = 3;    // 每一个等级下的最大奖励ID数
        private List<DefaultPromotionPrizeItem> m_prizeList = new List<DefaultPromotionPrizeItem>();

        #endregion

        #region recordShare
        public UShareBar ShareBar;
        private readonly string shareImgPath = "/GameData/GamePromotion/GamePromotion_share.texture";
        private readonly string shareImgName = "GamePromotion_share";
        #endregion

        private GamePromotionView m_masterVeiw;

        public override GamePromotionButtonType GetButtonType()
        {
            return GamePromotionButtonType.BTN_TAITANSAYSAY;
        }

        public override bool Init(GamePromotionView masterView)
        {
            m_masterVeiw = masterView;

            withdrawText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "WithdrawDesc");
            withDrawBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "WithdrawBtnDesc");
            withDrawBtn.onClick.AddListener(OnWithdrawBtnClick);
            myUserPayInfoText.text = String.Format(ULocalizationService.Instance.Get("UIView", "GamePromotion", "MyUserPayInfo"), 0);
            joinedRecentlyText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "JoinedRecently");
            accontText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "Account");
            totalPayText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "TotalPay");
            totalExchangeText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "TotalExchange");
            createDatetimeText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "CreateDatetime");
            privateLinkText.text = "";
            copyBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "CopyBtnText");
            copyBtn.onClick.AddListener(OnCopyBtnClick);
            QRCodeText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "QRCodeDesc");
            screenShortText.text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "ScreenShortDesc");
            screenshortImage.GetComponent<Button>().onClick.AddListener(OnScreenshortBtnClick);
            TaiTanLinkBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GamePromotion", "TaiTanLinkBtnDesc");
            TaiTanLinkBtn.onClick.AddListener(OnTaiTanLinkBtnClick);

            if (!InitTotalNumGamesFrame())
                return false;

            InitShareRecordData();

            SetMyUserListData();

            return true;
        }

        private bool InitTotalNumGamesFrame()
        {
            totalNumberGamesObj.SetActive(false);
            m_prizeFrameObj = totalNumberGamesObj.transform.FindChild("PrizeFrame").gameObject;
            totalNumGamesIF.text = LogicDataCenter.gamePromotionDataManager.TotalNumGames.ToString();
            giftBtn.onClick.AddListener(OnGiftBtnClick);

            if (!InitPrizeFrame())
                return false;

            return true;
        }

        private bool InitPrizeFrame()
        {
            for (int i = 0; i < PRIZE_LEVEL_COUNT; ++i)
            {
                GameObject go = Instantiate(defaultPromotionPrizeItem.gameObject);
                if (go == null)
                {
                    Trace.LogError("DefaultPromotionPrizeItem Instantiate failed");
                    return false;
                }

                go.transform.SetParent(defaultPromotionPrizeItem.parent, false);
                DefaultPromotionPrizeItem item = go.GetComponent<DefaultPromotionPrizeItem>();
                if (!item.Init(this))
                    return false;

                m_prizeList.Add(item);
            }

            defaultPromotionPrizeItem.gameObject.SetActive(false);
            defaultSubPrizeItem.gameObject.SetActive(false);

            return true;
        }

        private void SetPrizeFrameData()
        {
            List<SSchemeRecommendPrize> list = RecommendPrizeConfig.Instance.ConfigList;
            int count = list.Count < m_prizeList.Count ? list.Count : m_prizeList.Count;
            for (int i = 0; i < count; ++i)
            {
                m_prizeList[i].SetData(list[i].targetID, list[i].totalNumGames, list[i].prizeList);
                m_prizeList[i].gameObject.SetActive(true);
            }

            for (int i = count; i < m_prizeList.Count; ++i)
            {
                m_prizeList[i].gameObject.SetActive(false);
            }
        }

        public void OnGiftBtnClick()
        {
            if (m_prizeFrameObj != null)
                m_prizeFrameObj.SetActive(!m_prizeFrameObj.activeSelf);

            SetPrizeFrameData();
        }

        public void InitShareRecordData()
        {
            Debug.Log("xxxpath:" + Application.dataPath);

            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GamePromotion, shareImgName, m_masterVeiw.GetUIWnd().GetID());
            if (sprite == null)
            {
                Trace.LogError(string.Format("get promotion sprite failed"));
                return;
            }

            ShareBar.SetData(SocialSharingType.GamePromotion, sprite.texture.EncodeToPNG());
        }

        public void OnWithdrawBtnClick()
        {
            Debug.LogFormat("OnWithdrawBtnClick");

            OnTaiTanLinkBtnClick();
        }

        public void OnScreenshortBtnClick()
        {
            Debug.LogFormat("OnScreenshortBtnClick");

            UIButtonType buttonType = new UIButtonType((int)PlayerFrame.PlayerSubWndType.WND_MOMENT);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, buttonType);
            m_masterVeiw.SetVisible(false);
        }

        public void OnCopyBtnClick()
        {
            Trace.LogFormat("OnCopyBtnClick");

            SocialSharingNode node = SchemeSocialSharing.Instance.RandomSharingNode(SocialSharingType.GamePromotion);
            string prefix = "";
            if (node != null && !string.IsNullOrEmpty(node.szTitle))
            {
                prefix = node.szTitle + ":";
            }

            TextEditor textEditor = new TextEditor();
            textEditor.text = string.Format("{0}{1}", prefix, privateLinkText.text);
            textEditor.OnFocus();
            textEditor.Copy();

            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_GAMEPROMOTION_COPY_SUCCESS);
        }

        public void OnTaiTanLinkBtnClick()
        {
            string mainhomeLink;
            if (LogicDataCenter.gamePromotionDataManager.WebUrl.TryGetValue(GameWebUrl.TAITAN_MAINHOME, out mainhomeLink))
            {
                Trace.Log(string.Format("OnTaiTanLinkBtnClick{0}", mainhomeLink));
                try
                {
                    System.Diagnostics.Process.Start(mainhomeLink);
                }
                catch (Exception e)
                {
                    Trace.LogError(string.Format("{0},{1}", e.Message, mainhomeLink));
                    return;
                }
            }
        }

        public void SetMyUserListData()
        {
            GamePromotionDataManager.PromotionPrizeData data = LogicDataCenter.gamePromotionDataManager.PromotionPrize;
            if (data == null)
                return;

            withdrawInputField.text = data.UsableBalance;
            myUserPayInfoText.text = String.Format(ULocalizationService.Instance.Get("UIView", "GamePromotion", "MyUserPayInfo"), data.MyUserCount);

            if (data.UserList != null)
            {
                myUserList.DataSource.BeginUpdate();
                myUserList.DataSource.Clear();
                foreach (GamePromotionDataManager.MyUser user in data.UserList)
                {
                    UListItemMyUser item = new UListItemMyUser();
                    item.data = user;
                    myUserList.DataSource.Add(item);
                }

                myUserList.DataSource.EndUpdate();
            }
        }
    }

    public class PromotionWebFrame : UISubFrameView
    {
        public RectTransform WebBrowserContainer;
        public GameObject WebBrowserProgressBar;
        public Vector2 WebBrowserSize;
        public string WebBrowserUrl = "q1.com";

        private ResNode m_webBrowserResNode;
        private UBrowser m_browserInstance;
        private CanvasGroup m_canvGroup;
        private GamePromotionView m_masterView;
        private string m_curUrl = "";
        private LinkedList<string> m_reqUrlList;

        public override bool Init(GamePromotionView masterView)
        {
            m_reqUrlList = new LinkedList<string>();

            m_masterView = masterView;
            LoadWebBrowser();

            if (m_canvGroup == null)
                m_canvGroup = this.GetComponent<CanvasGroup>();

            return true;
        }

        public override GamePromotionButtonType GetButtonType()
        {
            return GamePromotionButtonType.BTN_WEBLINK;
        }

        public override void ShowFrameView(bool bShow)
        {
            if (m_canvGroup != null)
                m_canvGroup.alpha = bShow ? 1 : 0;

            base.ShowFrameView(true);
        }

        public void RefreshBrowser(bool bVisible)
        {
            if (bVisible)
                LoadWebBrowser();
            else
                UnloadWebBrowser();
        }

        public void OnDestroy()
        {
            UnloadWebBrowser();
        }

        public void Redirection(string _url)
        {
            if (_url == "" || m_curUrl == _url)
                return;

            m_curUrl = _url;
            m_reqUrlList.AddLast(m_curUrl);

            if (WebBrowserProgressBar != null)
            {
                WebBrowserProgressBar.gameObject.SetActive(true);
            }

            if(m_browserInstance != null)
            {
                m_browserInstance.OnToolbarUrlFieldEndEdit(m_curUrl);
            }
        }

        private void OnPageLoadedEvent(string _strMessage)
        {
            if (m_reqUrlList.Count > 0)
                m_reqUrlList.RemoveFirst();

            if(m_reqUrlList.Count > 0)
            {
                string url = m_reqUrlList.First.Value;
                if (m_browserInstance != null)
                {
                    m_browserInstance.OnToolbarUrlFieldEndEdit(url);
                    return;
                }
            }

            if (WebBrowserProgressBar != null)
            {
                WebBrowserProgressBar.gameObject.SetActive(false);
            }
        }

        private void UnloadWebBrowser()
        {
            if (m_webBrowserResNode != null)
            {
                UISystem.Instance.ReleaseResNode(m_webBrowserResNode, true);
                m_webBrowserResNode = null;
            }
        }

        private void LoadWebBrowser()
        {
            if (m_browserInstance != null)
                return;

            if (WebBrowserProgressBar != null)
            {
                WebBrowserProgressBar.gameObject.SetActive(true);
            }

            m_webBrowserResNode = UISystem.Instance.GetResNode("UI/Widgets/WebBrowser/WebBrowser");

            if (m_webBrowserResNode == null || !m_webBrowserResNode.IsValid())
                return;

            m_browserInstance = m_webBrowserResNode.InstanceMainRes<UBrowser>();
            if (m_browserInstance == null)
                return;

            if (m_browserInstance.WebBrowserToolbar != null)
            {
                m_browserInstance.WebBrowserToolbar.gameObject.SetActive(false);
            }

            m_browserInstance.Width = (int)WebBrowserSize.x;
            m_browserInstance.Height = (int)WebBrowserSize.y;
            m_browserInstance.InitialURL = m_curUrl == "" ? WebBrowserUrl : m_curUrl;
            m_browserInstance.GetRectTransform.SetParent(WebBrowserContainer, false);

            m_browserInstance.OnPageLoadedEvent.RemoveListener(OnPageLoadedEvent);
            m_browserInstance.OnPageLoadedEvent.AddListener(OnPageLoadedEvent);
        }
    }

    public class UWebBrowserProgressBar : MonoBehaviour
    {
        public RectTransform ProgressBarCircle;

        public void OnEnable()
        {
            if (ProgressBarCircle != null)
            {
                ProgressBarCircle.localEulerAngles = Vector3.zero;
                ProgressBarCircle.DOBlendableLocalRotateBy(new Vector3(0, 0, -180), 1.0f).SetLoops(-1, LoopType.Incremental);
            }
        }

        public void OnDisable()
        {
            if (ProgressBarCircle != null)
            {
                ProgressBarCircle.DOPause();
            }
        }
    }
}

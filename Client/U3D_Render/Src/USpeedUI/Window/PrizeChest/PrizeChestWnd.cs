using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using DataCenter;
using GameLogic;
using Data.ActorPrizeConfig;
using UnityEngine.UI;
using UIWidgets;
using USpeedUI.PointShop;
using U3D_Render.Common;
using USpeedUI.UEffect;
using System.Reflection;
using USpeedUI.UExtensions;

namespace USpeedUI.PrizeChest
{
    public class PrizeChestWnd : UIFillWnd<PrizeChestWndView>
    {
        public WndMutexLevelDef m_mutexLevelDef = WndMutexLevelDef.WND_Mutex_High;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_PRIZECHEST;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PrizeChest/PrizeChest";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return m_mutexLevelDef;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PRIZECHEST_UPDATECHESTINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PRIZECHEST_OPENCHESTRESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PRIZECHEST_UPDATECHESTINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PRIZECHEST_OPENCHESTRESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {

                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.StopPrizeChest();
                            UnloadView();
                        }
                    }
                    break;

                case WndMsgID.WND_MSG_PRIZECHEST_UPDATECHESTINFO:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                            m_wndView.OnUpdateChestInfo();
                    }
                    break;

                case WndMsgID.WND_MSG_PRIZECHEST_OPENCHESTRESULT:
                    {
                        if (m_wndView != null)
                            m_wndView.OnOpenChestResult();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                            m_wndView.StopPrizeChest();
                    }
                    break;
                default:
                    break;
            }
        }

        protected override void PreSetVisible(bool _bVisible)
        {
            base.PreSetVisible(_bVisible);

            m_mutexLevelDef = WndMutexLevelDef.WND_Mutex_High;
            if (_bVisible)
            {
                UISystem.Instance.SetMutexWndVisbleAlone(this.m_wndView, WndLayerDef.WND_LAYER_POPUP01, WndLayerDef.WND_LAYER_POPUP02);
            }
        }
    }

    public enum EMPrizeResultTimerType : int 
    {
        EMTimer_ResultCreatEffect = 1,              //奖励模型创建计时器(圆盘底座)
        EMTiemr_AwardOpenEffect = 2,              //奖励结果打开计时器(奖励结果图标)
    }

    public enum EMPrizeChestView
    {
        EMView_Main,
        EMView_Award,
    }
    public class PrizeChestInfo:MonoBehaviour
    {
        public Image Icon;
        public Text Count;
        public Text Name;

        public SSchemePrizeChestConfig PrizeChestConfig { get { return prizeChestConfig; } }
        private SSchemePrizeChestConfig prizeChestConfig;
        public int PrizeChestCount { get { return prizeChestCount; } }
        private int prizeChestCount;

        public void SetData(SSchemePrizeChestConfig config, int num)
        {
            prizeChestConfig = config;
            prizeChestCount = num;
            Count.text = num.ToString();
            Name.text = config.strChestName;
            Icon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PrizeChest, "ChestIcon_" + config.nChestIcon, WndID.WND_ID_PRIZECHEST);
        }

        public void OnDestroy()
        {
            prizeChestConfig = null;
        }
    }

    public class PrizeChestItem : MonoBehaviour
    {
        public Text openChestBtnText;
        public Text chestName;
        public Button openChestBtn;

        private PrizeChestWndView m_wndView;
        private int m_nChestType;

        public void SetOpenChestBtnText(int num)
        {
            if (num > 0)
            {
                openChestBtnText.text = String.Format("{0}({1})", ULocalizationService.Instance.Get("UIView", "PrizeChest", "OpenChestBtn"), num);
            }
            else
            {
                openChestBtnText.text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "OpenChestBtn");
            }
        }

        public void SetData(PrizeChestWndView wndView, int id, string chestNameDesc, int nHaveNum)
        {
            m_wndView = wndView;
            m_nChestType = id;

            chestName.text = chestNameDesc;

            gameObject.SetActive(true);
        }

        public void OnOpenChest()
        {
            LogicDataCenter.prizeChestDataManager.RequestOpenChest(m_nChestType);
        }

        public void Clear()
        {
            m_nChestType = -1;
            gameObject.SetActive(false);
        }
    }

    public class PrizeResultItem : MonoBehaviour, ITimerHanlder
    {
        public Text prizeName;
        public Image prizeIconImage;
        public Image prizeTypeImage;
        public RectTransform prizeIconContainer;
        public GameObject DetailFrame;
        public Button prizeChestClickBtn;
        public string SoundFilter;
        public string SoundKey_Normal;
        public string soundKey_Value;

        private int m_Index;
        private PrizeIconItem prizeIcon;
        private SceneEffectParamBase pedestalEffect;
        private SceneEffectParamBase glowEffectParam;
        private SceneEffectParamBase clickEffectParam;
        private PrizeChestWndView m_wndView;
        private SSchemeActorPrizeConfig actorPrizeConfig;

        public void SetData(PrizeChestWndView wndView, int index, int nPrizeID, SSchemeActorPrizeConfig config)
        {
            if (config == null ||　wndView == null)
                return;

            m_wndView = wndView;
            m_Index = index;
            actorPrizeConfig = config;

            prizeName.text = config.strPrizeName;
            //prizeGradeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PrizeChest, "PrizeBase" + config.nPrizeGrade, WndID.WND_ID_PRIZECHEST);
            prizeIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PrizeChest, "PrizeIconFrame" + config.nPrizeGrade, WndID.WND_ID_PRIZECHEST);
            prizeTypeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PrizeChest, "PrizeType" + config.nPrizeType, WndID.WND_ID_PRIZECHEST);
            //prizeTypeImage.SetNativeSize();

            prizeIcon = LogicDataCenter.pointShopDataManager.CreatePrizeIconItem(prizeIconContainer);
            prizeIcon.SetPrizeID(nPrizeID);

            TimerManager.SetTimer(this, (int)EMPrizeResultTimerType.EMTimer_ResultCreatEffect, 4.3f);
            prizeChestClickBtn.onClick.RemoveAllListeners();
            prizeChestClickBtn.onClick.AddListener(ShowDetailInfo);
            prizeChestClickBtn.interactable = true;
            DetailFrame.SetActive(false);
        }

        public void Clear()
        {
            int nCount = prizeIconContainer.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = prizeIconContainer.GetChild(i);
                if (childTrans)
                {
                    var temp1 = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp1);
                }
            }

            prizeChestClickBtn.interactable = false;
            DetailFrame.SetActive(false);

            if(pedestalEffect != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref pedestalEffect);
            if (glowEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref glowEffectParam);
            if (clickEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref clickEffectParam);
        }

        private void ShowDetailInfo()
        {
            prizeChestClickBtn.interactable = false;

            TimerManager.SetTimer(this, (int)EMPrizeResultTimerType.EMTiemr_AwardOpenEffect, 1);

            if (glowEffectParam != null)
            {
                clickEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_PrizeChest_Click, glowEffectParam.AttachTransform);
                SceneEffectManager.Instance.CreateSceneEffect(ref clickEffectParam);
                SceneEffectManager.Instance.DestroySceneEffect(ref glowEffectParam, false);
            }

            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            string key = actorPrizeConfig.nPrizeGrade > 3 ? soundKey_Value : SoundKey_Normal;
            if (Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter, key, ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }

            UTooltipTrigger trigger = this.transform.GetComponent<UTooltipTrigger>();
            if (trigger != null)
            {
                trigger.enabled = true;
                trigger.SetText(UTooltipParamName.BodyText, actorPrizeConfig.strPrizeDesc);
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EMPrizeResultTimerType.EMTimer_ResultCreatEffect:
                    {
                        CreatPrizeResultModelAndEffect();
                        TimerManager.KillTimer(this, nTimerID);
                    }
                    break;
                case (int)EMPrizeResultTimerType.EMTiemr_AwardOpenEffect:
                    {
                        DetailFrame.SetActive(true);
                        if (m_wndView != null)
                            m_wndView.CountPrizeOpenNum();
                        TimerManager.KillTimer(this, nTimerID);
                    }
                    break;
                default:
                    break;
            }
        }

        private void CreatPrizeResultModelAndEffect()
        {
            if (m_wndView.PrizeResultObjContainer == null)
                return;

            GameObject go = new GameObject();
            go.name = "ResultItem" + m_Index;
            go.transform.SetParent(m_wndView.PrizeResultObjContainer, false);
            go.transform.localPosition = m_wndView.prizeResultObjPos[m_Index];

            pedestalEffect = new SceneEffectParamBase(actorPrizeConfig.nPedestalEffectID, go.transform);
            SceneEffectManager.Instance.CreateSceneEffect(ref pedestalEffect);

            glowEffectParam = new SceneEffectParamBase(actorPrizeConfig.nPedestalGlowEffectID, go.transform);
            SceneEffectManager.Instance.CreateSceneEffect(ref glowEffectParam);
        }
    }

    public class PrizeChestWndView : UIBaseWndView, ITimerHanlder
    {
        public GameObject chestPageObj;
        public UButtonGroup chestGroup;
        public PrizeChestItem prizeChestItem; 
        public Text ReturnBtnText;

        public GameObject resultPageObj;
        public List<PrizeResultItem> prizeResultList;
        public Text taskTip;
        public Text taskDesc;

        public Button closeBtn;
        public Button continueOpenBtn;
        public Button linkBtn;
        public Button nextChestBtn;
        public Text nextChestHint;
        public Text continueOpenText;
        public Text continueLeftDesc;
        public Text awardclickLabel;

        //主角面模型根节点位置
        public Vector3 mainViewChestPos = new Vector3(0, -4.5f, 18);  
        //摄像机目标点
        public Vector3 camTargetPos = new Vector3(0, 0, 0);  
        public List<Vector3> prizeResultObjPos;        //奖励界面模型位置表
        public GameObject prizeChestInfoGO;
        public string SoundFilter_AnimStart;
        public string SoundKey_AnimStart;

        private ResNode[] prizeChestResNodeList;        //宝箱模型资源节点表
        private Transform chestObjContainer;                //宝箱模型根节点
        public Transform PrizeResultObjContainer { get { return prizeResultObjContainer; } }
        private Transform prizeResultObjContainer;         //奖励模型根节点

        private List<PrizeChestInfo> prizeChestInfoList;
        private GameObject tempPriezeChestModel;                                 //奖励界面创建的临时宝箱模型(也就是播放开箱子动画那个模型)
        private SSchemePrizeChestConfig prizeChestConfig;                    //当前开启的宝箱配置信息
        private int prizeCount = 0;              //当前宝箱的奖品数量
        private int openPrizeCount = 0;      //当前宝箱已经开启的奖品数量 (当开启数量=总数量时才可允许返回操作)
        private int m_nResultPageChestType;      //当前开启的宝箱结果类型
        private int tempModelID;                                     //临时宝箱类型ID
        private string[] bonesName;                               //骨骼名字数组
        const int RAND_CARD_FIRST_INDEX = 901;
        private bool bEnableClose;     //是否允许关闭界面，在播放开箱子的动画过程中，是不允许关闭整个UI界面的
        private string openTips;
        private int chestAnimTimer = 1;
        private EMPrizeChestView currentView;
        private SceneEffectParamBase defaultBGEffect;
        private bool m_isFromNextChestBtn = false;
        private int m_nextChestId = -1;
        private int m_reaminingChestCount = 0;
        private int m_camEffectID = -1;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            currentView = EMPrizeChestView.EMView_Main;

            openTips = ULocalizationService.Instance.Get("UIView", "PrizeChest", "OpenToolTip");
            ReturnBtnText.text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "ReturnBtn");
            continueOpenText.text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "ReturnBtn");
            taskTip.text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "taskTip");
            string tip = ULocalizationService.Instance.Get("UIView", "PrizeChest", "taskDesc");
            UBB.toHtml(ref tip);
            taskDesc.text = tip;

            //宝箱模型资源节点
            prizeChestResNodeList = new ResNode[3];
            prizeChestResNodeList[0] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Skin/Other/TreasureBoxRoomView/BaoXiang_01");
            prizeChestResNodeList[1] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Skin/Other/TreasureBoxRoomView/BaoXiang_02");
            prizeChestResNodeList[2] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Skin/Other/TreasureBoxRoomView/BaoXiang_03");

            bonesName = new string[8];
            string[] names = { "Back_M", "Back_M1", "Back_M2", "Back_M3", "Back_M4", "Back_M5", "Back_M6", "Back_M7"};
            bonesName = names;

            prizeChestInfoList = new List<PrizeChestInfo>();
            chestGroup.onButtonGroupSelectedChange.RemoveAllListeners();
            chestGroup.onButtonGroupSelectedChange.AddListener(OnPrizeChestSelectChanged);

            chestPageObj.SetActive(false);
            bEnableClose = false;

            linkBtn.gameObject.SetActive(false);
            linkBtn.gameObject.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "LinkBtn");
            linkBtn.onClick.RemoveAllListeners();
            linkBtn.onClick.AddListener(OnLinkBtnClicked);
            nextChestBtn.gameObject.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "NextChestBtn");
            nextChestBtn.onClick.RemoveAllListeners();
            nextChestBtn.onClick.AddListener(OnNextChestBtnClicked);
            nextChestBtn.gameObject.SetActive(false);
            nextChestHint.gameObject.SetActive(false);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            Clear();

            //释放资源节点
            for(int i = 0; i <  3; i++)
            {
                AssetBundleManager.DeleteAssets(ref prizeChestResNodeList[i], true);
            }
        }

        internal override void SetRendererVisible(bool visible)
        {
            if (visible)
            {
                Clear();
                //加载宝箱主界面
                SceneManager.Instance.LoadTreasureBoxMainViewScene(SetPrizeChestObjContainer);
            }
            else
            {
                //如果不允许关闭，则返回
                if(!bEnableClose)
                    return;

                Clear();
                chestObjContainer = null;
                SceneManager.Instance.UnLoadTreasureBoxMainViewScene();
            }
            base.SetRendererVisible(visible);
            SoundManager.Forbid3DSound(visible);
        }

        //强行终止宝箱动画，关闭整个UI
        //此方法供游戏匹配进入时调用
        public void StopPrizeChest()
        {
            KillAllTimer();
            if (currentView == EMPrizeChestView.EMView_Award)
            {
                LightingEffectManager lem = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
                if (lem != null && m_camEffectID > 0)
                {
                    lem.RemoveLightingImmediate(m_camEffectID);
                }

                Clear();
                resultPageObj.SetActive(false);
                //卸载动画场景
                SceneManager.Instance.UnLoadTreasureBoxAwardViewScene();
                currentView = EMPrizeChestView.EMView_Main;
                bEnableClose = true;
            }
            OnCloseBtn();
        }
        
        public void OnTimer(int nTimerID)
        {
            if (nTimerID == chestAnimTimer)
            {
                SceneEffectParamBase tmpParam = null;
                for (int i = 0; i < CakeTrailEffectParamList.Count; ++i)
                {
                    tmpParam = CakeTrailEffectParamList[i];
                    SceneEffectManager.Instance.DestroySceneEffect(ref tmpParam);
                }
                CakeTrailEffectParamList.Clear();

                ResNode.DestroyRes(ref tempPriezeChestModel);  //删除开宝箱创建的临时模型
                tempPriezeChestModel = null;

                resultPageObj.SetActive(true);        //显示结果页面UI
                Invoke("OpenAwardClickTip", 1f);
                TimerManager.KillTimer(this, 1);

                SSchemePrizeChestConfig prizeChestConfig = FindNextChestPrize();
                if (prizeChestConfig != null)
                {
                    m_nextChestId = prizeChestConfig.nChestId;
                    nextChestBtn.gameObject.SetActive(true);
                    nextChestBtn.interactable = false;
                    if (m_nResultPageChestType == prizeChestConfig.nChestId)
                    {
                        nextChestHint.gameObject.SetActive(false);
                        int nNextChestCount = 0;
                        if (LogicDataCenter.prizeChestDataManager.ChestInfo.ContainsKey(m_nextChestId))
	                    {
		                    nNextChestCount = LogicDataCenter.prizeChestDataManager.ChestInfo[m_nextChestId];
	                    }
                        nextChestBtn.gameObject.GetComponentInChildren<Text>().text = String.Format("{0}({1})", ULocalizationService.Instance.Get("UIView", "PrizeChest", "NextChestBtn"), nNextChestCount);
                    }
                    else
                    {
                        nextChestHint.gameObject.SetActive(true);
                        nextChestHint.text = String.Format(ULocalizationService.Instance.Get("UIView", "PrizeChest", "WillOpenDifferentChest"), prizeChestConfig.strChestName);
                        nextChestBtn.gameObject.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "PrizeChest", "NextChestBtn");
                    }
                }
                else
                {
                    m_nextChestId = -1;
                    nextChestHint.gameObject.SetActive(false);
                    nextChestBtn.gameObject.SetActive(false);
                }
            }
        }

        private SSchemePrizeChestConfig FindNextChestPrize()
        {
            Dictionary<int, int> selfChestInfo = LogicDataCenter.prizeChestDataManager.ChestInfo;
            Dictionary<int, SSchemePrizeChestConfig> chestConfig = ActorPrizeConfig.Instance.PrizeChestConfig;
            if (selfChestInfo.ContainsKey(m_nResultPageChestType) && selfChestInfo[m_nResultPageChestType] > 0)
            {
                return chestConfig[m_nResultPageChestType];
            }

            foreach (var config in chestConfig)
            {
                int nHaveNum = 0;
                if (selfChestInfo.ContainsKey(config.Key))
                {
                    nHaveNum = selfChestInfo[config.Key];
                }
                if (config.Value.nAlwaysShow == 0 && nHaveNum <= 0)
                    continue;

                //只显示宝箱，不显示矿石
                if (config.Value.nChestFlag > 1)
                    continue;

                return config.Value;
            }

            return null;
        }

        public void OpenAwardClickTip()
        {
            Debug.LogWarning("OpenAwardClickTip,xxxxxxxxxxxxxxxxxxx");// TODO(临时代码) 问号上面的字unity里有，在客户端消失的问题
            awardclickLabel.gameObject.SetActive(true);
        }

        //更新主界面宝箱信息
        public void OnUpdateChestInfo()
        {
            RefreshChest();
        }

        //开启宝箱
        public void OnOpenChestResult()
        {
            //宝箱开启动画播放过程中是不允许关闭宝箱整个UI的
            bEnableClose = false;

            foreach (PrizeResultItem item in prizeResultList)
            {
                item.Clear();
            }

            cmd_entity_open_chest_result resultInfo = LogicDataCenter.prizeChestDataManager.OpenChestResult;
            m_nResultPageChestType = resultInfo.nChestType;

            //隐藏主界面UI
            chestPageObj.SetActive(false);
            closeBtn.gameObject.SetActive(false);
            ClearContainer(prizeResultObjContainer);
            
            if (!m_isFromNextChestBtn)
            {
                //加载宝箱动画场景
                SceneManager.Instance.LoadTreasureBoxAwardViewScene(SetPrizeResultObjContainer);
            }
            else
            {
                m_isFromNextChestBtn = false;
                LoadPrizeResultEffectAndSetUIData();
            }


            //更新宝箱入口红点数量
            Dictionary<int, int> selfChestInfo = LogicDataCenter.prizeChestDataManager.ChestInfo;
            int nLeftNum = 0;
            if (selfChestInfo.ContainsKey(m_nResultPageChestType))
            {
                nLeftNum = selfChestInfo[m_nResultPageChestType];
            }

            //更新继续开启按钮的交互属性，如果剩余宝箱数为0，则置灰
        //    continueOpenBtn.interactable = (nLeftNum > 0);
        //    continueLeftDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "PrizeChest", "ChestHaveNumDesc"), nLeftNum);
        }

        public void OnJumpToTaskView()
        {
            OnCloseBtn();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, null);
        }

        public void OnContinueOpenBtn()
        {
            LightingEffectManager lem = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
            if (lem != null && m_camEffectID > 0)
            {
                lem.RemoveLightingImmediate(m_camEffectID);
            }

            resultPageObj.SetActive(false);

            Clear();

            //卸载动画场景
            SceneManager.Instance.UnLoadTreasureBoxAwardViewScene();

            //开启主界面UI
            chestPageObj.SetActive(true);
            closeBtn.gameObject.SetActive(true);

            RefreshChest();

            currentView = EMPrizeChestView.EMView_Main;
            bEnableClose = true;

            // 如果没有宝箱了就直接返回游戏主城界面
            if (m_reaminingChestCount <= 0)
            {
                OnCloseBtn();
            }
        }

        //计算宝箱开启数量，如果全开了，则通知逻辑层广播开箱结果
        public void CountPrizeOpenNum()
        {
            Debug.LogWarning("CountPrizeOpenNum,wwwwwwwwwww");// TODO (调试用临时代码)问号上面的字unity里有，在客户端消失的问题
            awardclickLabel.gameObject.SetActive(false);
            openPrizeCount++;
            if (openPrizeCount >= prizeCount)
            {
                if (continueOpenBtn != null)
                {
                    continueOpenBtn.interactable = true;
                    if (m_nextChestId > 0)
                        nextChestBtn.interactable = true;
                    UTooltipTrigger tooltip = continueOpenBtn.gameObject.GetComponent<UTooltipTrigger>();
                    if (tooltip != null)
                        ResNode.DestroyRes(ref tooltip);
                }

                openPrizeCount = 0;
                EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_BROADCAST_CHEST_RESULT);
            }
        }

        public void OnCloseBtn()
        {
            m_wnd.SetVisible(false);
        }

        private void OnLinkBtnClicked()
        {
            (m_wnd as PrizeChestWnd).m_mutexLevelDef = WndMutexLevelDef.WND_Mutex_Normal;
            UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_PrizeChest);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
        }

        private void OnNextChestBtnClicked()
        {
            LightingEffectManager lem = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
            if (lem != null && m_camEffectID > 0)
            {
                lem.RemoveLightingImmediate(m_camEffectID);
            }

            foreach (PrizeResultItem item in prizeResultList)
            {
                item.Clear();
            }
            m_isFromNextChestBtn = true;
            resultPageObj.SetActive(false);
            nextChestBtn.interactable = false;
            nextChestBtn.gameObject.SetActive(false);
            nextChestHint.gameObject.SetActive(false);
            continueOpenBtn.interactable = false;
            LogicDataCenter.prizeChestDataManager.RequestOpenChest(m_nextChestId);
        }

        //刷新主界面宝箱模型及UI数据
        private void RefreshChest()
        {
            prizeChestItem.Clear();
            if (defaultBGEffect != null)
            {
                SceneEffectManager.Instance.DestroySceneEffect(ref defaultBGEffect);
                defaultBGEffect = null;
            }
            ClearContainer(chestObjContainer);
            ClearContainer(chestGroup.transform);
            Dictionary<int, int> selfChestInfo = LogicDataCenter.prizeChestDataManager.ChestInfo;
            Dictionary<int, SSchemePrizeChestConfig> chestConfig = ActorPrizeConfig.Instance.PrizeChestConfig;

            int nIndex = 0;
            m_reaminingChestCount = 0;
            foreach (var config in chestConfig)
            {
                if (!selfChestInfo.ContainsKey(config.Key))
                    continue;

                int nHaveNum = selfChestInfo[config.Key];
                if (config.Value.nAlwaysShow == 0 && nHaveNum <= 0)
                    continue;

                //只显示宝箱，不显示矿石
                if (config.Value.nChestFlag > 1)
                    continue;

                ++nIndex;
                if (nIndex > 6)
                    break;

                GameObject go = GameObject.Instantiate(prizeChestInfoGO);
                if (go)
                {
                    go.name = "PrizeChestInfoItem";
                    go.transform.SetParent(chestGroup.transform);
                    go.transform.localScale = Vector3.one;
                    UButtonGroupItem item = go.GetComponent<UButtonGroupItem>();
                    if (item)
                        item.Group = chestGroup;
                    PrizeChestInfo info = go.GetComponent<PrizeChestInfo>();
                    if (info)
                    {
                        info.SetData(config.Value, nHaveNum);
                        go.SetActive(true);
                        m_reaminingChestCount++;
                    }
                }
            }

            if (m_reaminingChestCount > 0)
            {
                linkBtn.gameObject.SetActive(false);
                Transform chestItemTr = chestGroup.transform.GetChild(0);
                OnPrizeChestSelectChanged(null, chestItemTr.gameObject.GetComponent<UButtonGroupItem>());
                if (m_reaminingChestCount == 1)
                {
                    prizeChestItem.SetOpenChestBtnText(chestItemTr.GetComponent<PrizeChestInfo>().PrizeChestCount);
                    ClearContainer(chestGroup.transform);
                }
                else
                {
                    prizeChestItem.SetOpenChestBtnText(-1);
                }
            }
        }

        private void Clear()
        {
            foreach (PrizeResultItem item in prizeResultList)
            {
                item.Clear();
            }
            ClearContainer(prizeResultObjContainer);
            ClearContainer(chestObjContainer);

            prizeResultObjContainer = null;

            m_nResultPageChestType = -1;
            m_nextChestId = -1;
            prizeChestConfig = null;
            m_camEffectID = -1;
        }

        private void KillAllTimer()
        {
            TimerManager.KillTimer(this, chestAnimTimer);
            TimerManager.KillTimer(this, (int)EMPrizeResultTimerType.EMTimer_ResultCreatEffect);
            TimerManager.KillTimer(this, (int)EMPrizeResultTimerType.EMTiemr_AwardOpenEffect);
        }

        private void ClearContainer(Transform container)
        {
            if (container == null)
                return;

            int nCount = container.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = container.GetChild(i);
                if (childTrans)
                {
                    var temp1 = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp1);
                }
            }
        }

        //主界面场景加载完毕回调
        private void SetPrizeChestObjContainer(SceneResInfoManager info)
        {
            currentView = EMPrizeChestView.EMView_Main;
            //挂载主界面场景模型根节点，创建场景光效
            if (chestObjContainer == null)
            {
                GameObject go = new GameObject();
                go.name = "PrizeChestContainer";
                chestObjContainer = go.transform;
            }
            //挂载宝箱模型根节点到场景上
            chestObjContainer.SetParent(info.SceneRoot.transform);
            chestObjContainer.localPosition = mainViewChestPos;

            RefreshChest();

            if (m_reaminingChestCount <= 0)
            {
                linkBtn.gameObject.SetActive(true);
                if (defaultBGEffect == null && chestObjContainer != null)
                {
                    defaultBGEffect = new SceneEffectParamBase((int)ESceneEffectID.SE_PrizeChest_DefaultBG, chestObjContainer);
                    SceneEffectManager.Instance.CreateSceneEffect(ref defaultBGEffect);
                }
            }
            chestPageObj.SetActive(true);
            closeBtn.gameObject.SetActive(true);
            bEnableClose = true;
        }

        //奖励界面场景加载完毕回调
        private void SetPrizeResultObjContainer(SceneResInfoManager info)
        {
            if (info == null || info.SceneRoot == null)
            {
                Debug.LogError("load scene 'TreasureBoxRoomView02' failed.");
                return;
            }

            currentView = EMPrizeChestView.EMView_Award;
            if (prizeResultObjContainer == null)
            {
                prizeResultObjContainer = info.SceneRoot.transform.FindChild("Scenes_Container");
                if(prizeResultObjContainer == null)
                {
                    GameObject go = new GameObject();
                    go.name = "prizeResultContainer";
                    prizeResultObjContainer = go.transform;

                    //挂载奖励界面场景模型根节点
                    prizeResultObjContainer.SetParent(info.SceneRoot.transform);
                    prizeResultObjContainer.localPosition = new Vector3(0, 0, 0);
                }
            }

            LoadPrizeResultEffectAndSetUIData();
        }

        private void LoadPrizeResultEffectAndSetUIData()
        {
            //获取当前开启的宝箱配置
            prizeChestConfig = ActorPrizeConfig.Instance.GetPrizeChestConfig(m_nResultPageChestType);
            if (prizeChestConfig == null)
                return;

            //创建临时模型
            tempModelID = Mathf.Clamp(prizeChestConfig.nChestModelID - 1, 0, 2);
            if (prizeChestResNodeList[tempModelID] != null)
            {
                GameObject prizeChestObj = prizeChestResNodeList[tempModelID].InstanceMainRes();
                if (prizeChestObj != null)
                {
                    prizeChestObj.transform.SetParent(prizeResultObjContainer);
                    prizeChestObj.transform.localPosition = Vector3.zero;

                    tempPriezeChestModel = prizeChestObj;
                }
            }
            else
            {
                Debug.LogError(String.Format("not find model ID in prizeChestResNodeList[{0}]", tempModelID));
                return;
            }

            //播放模型打开动画的光效
            SceneEffectParamBase openEffectParam = new SceneEffectParamBase(prizeChestConfig.nOpenEffect, tempPriezeChestModel.transform);
            SceneEffectManager.Instance.CreateSceneEffect(ref openEffectParam);

            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            if (Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter_AnimStart, SoundKey_AnimStart, ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }

            Transform[] tempChestModelChilds = tempPriezeChestModel.transform.GetComponentsInChildren<Transform>();

            //挂载摄像机动画
            Transform cmdTr = prizeResultObjContainer.parent.FindChild("CamTarget");
            if (cmdTr != null)
            {
                DestroyObject(cmdTr.gameObject);
            }
            GameObject camTarget = new GameObject();
            camTarget.name = "CamTarget";
            camTarget.transform.SetParent(prizeResultObjContainer.parent);
            camTarget.transform.localPosition = prizeResultObjContainer.localPosition;
            camTarget.transform.localEulerAngles = Vector3.zero;

            LightingEffectManager lem = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
            if (lem != null)
            {
                LightEffectParam param = new LightEffectParam();
                param.srcObject = camTarget;
                param.sourcePos = camTarget.transform.position;
                param.targetPos = camTarget.transform.position;

                m_camEffectID = prizeChestConfig.nCamEffectID;

                lem.AddLighting(prizeChestConfig.nCamEffectID, param);
            }


            List<int> prizeIdList = LogicDataCenter.prizeChestDataManager.PrizeIdList;
            openPrizeCount = 0;
            prizeCount = Mathf.Clamp(prizeIdList.Count, 0, 7);

            //增加Tooltip
            if (continueOpenBtn != null)
            {
                continueOpenBtn.interactable = prizeCount > 0 ? false : true;
                if (!continueOpenBtn.interactable)
                {
                    UTooltipTrigger tooltip = continueOpenBtn.gameObject.GetComponent<UTooltipTrigger>();
                    if (tooltip == null)
                        tooltip = continueOpenBtn.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                    tooltip.SetText(UTooltipParamName.BodyText, openTips);
                }
            }

            foreach (PrizeResultItem item in prizeResultList)
            {
                UTooltipTrigger trigger = item.transform.GetComponent<UTooltipTrigger>();
                if (trigger != null)
                    trigger.enabled = false;
            }

            //奖励宝箱模型上挂载光效
            for (int i = 0; i < prizeIdList.Count; ++i)
            {
                //PrizeID必须大于0，界面大小，图标最多放12个
                if (prizeIdList[i] <= 0 || i >= 7)
                    break;

                int nPrizeID = prizeIdList[i];
                SSchemeActorPrizeConfig config = null;

                if (nPrizeID >= RAND_CARD_FIRST_INDEX && nPrizeID < LogicDataCenter.prizeChestDataManager.RandCardIndex)
                {
                    config = LogicDataCenter.prizeChestDataManager.RandCardResult[nPrizeID];
                }
                else
                {
                    config = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);
                }

                if (config == null)
                    return;

                //骨骼上挂载奖励光效
                foreach (Transform tr in tempChestModelChilds)
                {
                    if (tr != null && tr.name.Equals(bonesName[i]))
                    {
                        SceneEffectParamBase effectParam = new SceneEffectParamBase(config.nCakeTrailEffectID, tr, null, true, 2700);
                        SceneEffectManager.Instance.CreateSceneEffect(ref effectParam);

                        CakeTrailEffectParamList.Add(effectParam);
                    }
                }
                //奖励UI数据填充
                prizeResultList[i].SetData(this, i, prizeIdList[i], config);
            }
            //开启动画定时器
            TimerManager.SetTimer(this, chestAnimTimer, 4.2f);
        }

        List<SceneEffectParamBase> CakeTrailEffectParamList = new List<SceneEffectParamBase>();

        private void OnPrizeChestSelectChanged(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            ClearContainer(chestObjContainer);
            if (newItem)
            {
                PrizeChestInfo info = newItem.GetComponent<PrizeChestInfo>();
                if (info)
                {
                    if (chestObjContainer == null)
                    {
                        GameObject go = new GameObject();
                        go.name = "PrizeChestContainer";
                        chestObjContainer = go.transform;
                    }

                    GameObject parent = new GameObject();
                    parent.name = "PrizeItem";
                    parent.transform.SetParent(chestObjContainer);
                    parent.transform.localPosition = Vector3.zero;

                    SceneEffectParamBase effectParam = new SceneEffectParamBase(info.PrizeChestConfig.nNormalEffect, parent.transform);
                    SceneEffectManager.Instance.CreateSceneEffect(ref effectParam);

                    SceneEffectParamBase bgEffectParam = new SceneEffectParamBase(info.PrizeChestConfig.nBgEffectID, parent.transform);
                    SceneEffectManager.Instance.CreateSceneEffect(ref bgEffectParam);

                    prizeChestItem.Clear();
                    prizeChestItem.SetData(this, info.PrizeChestConfig.nChestId, info.PrizeChestConfig.strChestName, info.PrizeChestCount);
                }
            }
        }
    }
}

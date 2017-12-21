/*******************************************************************
** 文件名:	UViewWarRecordTableEnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	战后战绩表显示界面
		
********************************************************************/
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using War;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using USpeedUI.UExtensions;
using U3D_Render.Common;
using USpeedUI.PlayerFrame;
using DG.Tweening;
using UIWidgets;
using U3D_Render;
using USpeedUI.UEffect;
using ASpeedGame.Data.PrizeUnlockConfig;
using Data.ActorPrizeConfig;
using ASpeedGame.Data.WarSupportPlayerConfig;
using Data.UISoundConfig;
using ASpeedGame.Data.Scheme;
using SocialSharing;
using USpeedUI.UWidgets;

namespace USpeedUI
{
    public class PopFrameInRecordInfo : MonoBehaviour
    {
        private int m_nTimeID;
        public int TimeID
        {
            set { m_nTimeID = value; }
            get { return m_nTimeID; }
        }
    }

    public class PopFrameActorUnlockPrize : MonoBehaviour
    {
        public Image PrizeImage;
        public Text PrizeText;

        public void SetActorUnlockPrizeView(int nPrizeIconId, int nPrizeType, string sPrizeDesc)
        {
            if (PrizeText == null || PrizeImage == null)
            {
                return;
            }

            PrizeText.text = sPrizeDesc;
            PrizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_PLAYERFRAME, nPrizeType, nPrizeIconId);
        }
    }

    public class PopFrameHeroUnlockPrize : MonoBehaviour
    {
        public Image PrizeImage;
        public Text PrizeText;
        public void SetHeroUnlockPrizeView(int nPrizeIconId, int nPrizeType, string sPrizeDesc)
        {
            if (PrizeText == null || PrizeImage == null)
            {
                return;
            }

            PrizeText.text = sPrizeDesc;
            PrizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_PLAYERFRAME, nPrizeType, nPrizeIconId);
        }
    }

    public class UPromptWnd : MonoBehaviour
    {
        void Start()
        {

        }

        public void Awake()
        {
            Text promptDesc = this.transform.FindChild("PromptText").GetComponent<Text>();
            promptDesc.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "PromptNotHaveAwakeReason");
        }

    }

    public  class UHeroCardInfo : MonoBehaviour
    {
        Image heroHead;
        //Image cardFrame;
        Text cardName;
        Text cardCount;
        void Start()
        {
        }

        public void Awake()
        {
            heroHead = transform.FindChild("HeroHead").GetComponent<Image>();
            //cardFrame = transform.FindChild("CardFrame").GetComponent<Image>();
            cardName = transform.FindChild("CardName").GetComponent<Text>();
            cardCount = transform.FindChild("CardCount").GetComponent<Text>();
        }

        public void setCardInfo(cmd_entity_hero_card cmdData)
        {
            heroHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 7, cmdData.nHeroID);
            //cardFrame.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_CardFrameUI, WndID.WND_ID_WAR_RECORDTABLE_END, cmdData.nCardFrameID);

            string stCardName = cmdData.chHeroCardName;
            if (stCardName != null)
            {
                UBB.toHtml(ref stCardName, UBB_FORMAT_TYPE.UGUI);
            }

            cardName.text = stCardName;
            cardCount.text = cmdData.nCount.ToString();
        }
    }

    public class UViewHeroUpLvFrame : MonoBehaviour
    {
        public Text[] Hp;
        public Text[] PhysicAttack;
        public Text[] PhysicDef;
        public Text[] Mp;
        public Text[] MagicAttack;
        public Text[] MagicDef;
        public Image heroIcon;
        public Text heroName;
        public Text heroLv;

        void Start()
        {
        }

        public void Awake()
        {

        }


        public void setHeroUpLvInfo(cmd_entity_statichero_upgrade cmdInfo)
        {
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 3, cmdInfo.nHeroID);
            heroName.text = cmdInfo.chHeroName;
            heroLv.text = cmdInfo.nCurrntLv.ToString();
            if (heroIcon.sprite == null)
            {
                heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 3, 10);
            }
            int nHeroStar = cmdInfo.nStarLv;
            int nGrade = (nHeroStar - 1) / UDefines.UPGRADE_VALUE + 1;
            int nGradeLv = nHeroStar % UDefines.UPGRADE_VALUE;
            if (nGradeLv == 0)
            {
                nGradeLv = UDefines.UPGRADE_VALUE;
            }

            for (int i = 0; i < UDefines.PROPERTY_COUNT; ++i)
            {
                if (i >= 2)
                {
                    Hp[i].text = (cmdInfo.fHP[i] == 0 ? "" : ("+" + cmdInfo.fHP[i].ToString("0.0")));
                    PhysicAttack[i].text = (cmdInfo.fPA[i] == 0 ? "" : ("+" + cmdInfo.fPA[i].ToString("0.0")));
                    PhysicDef[i].text = (cmdInfo.fPD[i] == 0 ? "" : ("+" + cmdInfo.fPD[i].ToString("0.0")));
                    Mp[i].text = (cmdInfo.fMagic[i] == 0 ? "" : ("+" + cmdInfo.fMagic[i].ToString("0.0")));
                    MagicAttack[i].text = (cmdInfo.fMA[i] == 0 ? "" : ("+" + cmdInfo.fMA[i].ToString("0.0")));
                    MagicDef[i].text = (cmdInfo.fMD[i] == 0 ? "" : ("+" + cmdInfo.fMD[i].ToString("0.0")));
                }
                else
                {
                    Hp[i].text = cmdInfo.fHP[i].ToString("0.0");
                    PhysicAttack[i].text = cmdInfo.fPA[i].ToString("0.0");
                    PhysicDef[i].text = cmdInfo.fPD[i].ToString("0.0");
                    Mp[i].text = cmdInfo.fMagic[i].ToString("0.0");
                    MagicAttack[i].text = cmdInfo.fMA[i].ToString("0.0");
                    MagicDef[i].text = cmdInfo.fMD[i].ToString("0.0");
                }
            }
        }


        public void onClickCloseButton()
        {
            if (gameObject.activeSelf)
            {
                gameObject.SetActive(false);
            }
        }
    }

    public class UMedalScore : MonoBehaviour
    {
        public Sprite[] medalSprite;
        public float medalValueAnomationTime = 3.0f;
        public float medalImageAnomationTime = 0.6f;

        Image medalImage;
        Text medalScore;
        Text property;
        Text meanScore;
        UEffectParamBase param;
        UEffectParamBase paramContinue;
        UEffectParamBase scoreEffect;

        bool bScoreEffectIsRight = false;

        public enum EN_PlayerGrade
        {
            ENPLAYERGRADE_S_1 = 1,
            ENPLAYERGRADE_A_ADD_2,
            ENPLAYERGRADE_A_3,
            ENPLAYERGRADE_B_ADD_4,
            ENPLAYERGRADE_B_5,
            ENPLAYERGRADE_B_6,
            ENPLAYERGRADE_C_ADD_7,
            ENPLAYERGRADE_C_ADD_8,
            ENPLAYERGRADE_C_9,
            ENPLAYERGRADE_C_10,

            ENPLAYERGRADE_MAX,
        }

        public void Start()
        {

        }

        public void Awake()
        {
            medalImage = this.transform.FindChild("MedalImage").GetComponent<Image>();
            medalScore = this.transform.FindChild("Score").GetComponent<Text>();
            property = this.transform.FindChild("Property").GetComponent<Text>();
            meanScore = this.transform.FindChild("MeanScore").FindChild("Value").GetComponent<Text>();
        }

        public void Destroy()
        {
            if (param != null)
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            if (paramContinue != null)
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref paramContinue);
            if (scoreEffect != null)
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref scoreEffect);
        }
        public void setMedalScore(int nLevel, int nScore, int nMeanScore)
        {
            int nOldScore = 0;

            meanScore.text = nMeanScore.ToString();
            DOTween.To(() => nOldScore, x => nOldScore = x, nScore, medalValueAnomationTime).
                       OnUpdate(() => updateHeroScore(nLevel, nOldScore, nScore, nMeanScore)).
                       OnComplete(() => onCompleteScore(nLevel));
        }
        void updateHeroScore(int nLevel, int nOldScore, int nEndScore, int nMeanScore)
        {
            if (nLevel <= 3 && nLevel > 0)
            {
                medalScore.text = "<size=50>" + nOldScore.ToString() + "</size>";
                medalScore.GetComponent<Outline>().enabled = true;
            }
            else
            {
                medalScore.text = "<size=30>" + nOldScore.ToString() + "</size>";
                medalScore.GetComponent<Outline>().enabled = false;
            }

            if (nEndScore == nOldScore && nEndScore >= nMeanScore && !bScoreEffectIsRight)
            {
                scoreEffect = new UEffectPrefabParam(UEffectPrefabType.UEPT_UWarRecordTableEnd_ScoreEffect, _tfAttachParent: medalScore.transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref scoreEffect);
                bScoreEffectIsRight = true;
            }
        }
        void onCompleteScore(int nLevel)
        {
            if (nLevel >= (int)EN_PlayerGrade.ENPLAYERGRADE_MAX || nLevel < (int)EN_PlayerGrade.ENPLAYERGRADE_S_1)
            {
                return;
            }

            medalImage.gameObject.SetActive(true);
            UEffectPrefabType enEffectPreabTypeContinue = new UEffectPrefabType();
            UEffectPrefabType enEffectPreabType = new UEffectPrefabType();
            EN_PlayerGrade playerGradeType = (EN_PlayerGrade)nLevel;
            int nSpriteIndex = 0;
            switch (playerGradeType)
            {
                case EN_PlayerGrade.ENPLAYERGRADE_S_1:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_S_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_S;
                        nSpriteIndex = 0;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_A_ADD_2:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A_ADD;
                        nSpriteIndex = 1;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_A_3:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_A;
                        nSpriteIndex = 2;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_B_ADD_4:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B_ADD;
                        nSpriteIndex = 3;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_B_5:
                case EN_PlayerGrade.ENPLAYERGRADE_B_6:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_B;
                        nSpriteIndex = 4;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_C_ADD_7:
                case EN_PlayerGrade.ENPLAYERGRADE_C_ADD_8:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C_ADD;
                        nSpriteIndex = 5;
                    }
                    break;
                case EN_PlayerGrade.ENPLAYERGRADE_C_9:
                case EN_PlayerGrade.ENPLAYERGRADE_C_10:
                    {
                        enEffectPreabTypeContinue = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C_Continue;
                        enEffectPreabType = UEffectPrefabType.UEPT_UWarRecordTableEnd_Medal_C;
                        nSpriteIndex = 6;
                    }
                    break;
                default:
                    {
                        return;
                    }
            }

            medalImage.sprite = medalSprite[nSpriteIndex];
            UEffectParamBase param = new UEffectPrefabParam(enEffectPreabType, _tfAttachParent: this.medalImage.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            // TODO 上一个弹出牌子动画播放完之后执行下面的
            UEffectParamBase paramContinue = new UEffectPrefabParam(enEffectPreabTypeContinue, _tfAttachParent: this.medalImage.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref paramContinue);

            playMedalSound();
        }

        public void setMedalName(string stName)
        {
            property.text = stName;
        }

        // 勋章动画结束后播放音效,所有勋章只播放一次
        private void playMedalSound()
        {

            SSchemeUSoundData soundData = new SSchemeUSoundData();
            if (UISoundConfig.Instance.GetData("statistics", "Button_statisticsmedal", ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }

            SSchemeUSoundData soundData2 = new SSchemeUSoundData();
            if (UISoundConfig.Instance.GetData("statistics", "Button_statisticsscore", ref soundData2))
            {
                SoundManager.CreateUISound(soundData2.USoundID);
            }
        }
    }

    // 基础逻辑
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        private enum EFrameType
        {
            FRAME_VSINFO= 0,
            FRAME_DATAGRID,
            FRAME_DATACHART,

            FRAME_MAX,
        }

        private enum EDataType
        {
            EDATATYPE_OUTPUT = 3,       // 输出伤害
            EDATATYPE_BEARDAMAGE,       // 承受的伤害
            EDATATYPE_DESTORYTOWER,     // 摧毁塔的数量
        }


        public GameObject[] Frames;

        public UButtonGroupItem[] FrameBtnGroup;

        public Text showBtnLabel;
        public Text exitBtnLabel;
        public GameObject MenuBtnGroup;

        // 屏幕中心动画组合
        ResNode upHeroLvFrameRes;
        UPromptWnd promptWnd;
        //Image resultImag;

        GameObject showBtn;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            VSInfoTrans = Frames[(int)EFrameType.FRAME_VSINFO].transform;
            DataGridTrans = Frames[(int)EFrameType.FRAME_DATAGRID].transform;
            DataChartTrans = Frames[(int)EFrameType.FRAME_DATACHART].transform;

            // 子窗口按钮组
            for (int i = 0; i < FrameBtnGroup.Length; ++i)
            {
                FrameBtnGroup[i].onSelectedChangedEx.AddListener(onClickGroupButton);
            }
            showBtn = MenuBtnGroup.transform.FindChild("ShowButton").gameObject;

            //shareBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "ShareBtnText");
            showBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "ShowBtnText");
            exitBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "ExitBtnText");

            // 对战信息
            initVSInfoPlayerFrame();

            // 数据网格 
            initDataGridFrame();

            // 数据表格
            initDataChartFrame();

            // 奖励信息
            initAwardInfo();

            // recordShare
            initRecordShare();

            // UpHeroLvUI
            initUpHeroLvUI();

            initUIWhenOB();

            return true;
        }

        private void initUIWhenOB()
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            showBtn.SetActive(false);
            ShareBar.gameObject.SetActive(false);
        }


        internal override void SetRendererVisible(bool _bVisible)
        {
            base.SetRendererVisible(_bVisible);
            if (_bVisible)
            {
                #region recordShare
                CaptureScreen.GetInstance().CaptureAreaImage(new CaptureAreaImgData(shareImgPath, GetRecordRect()));
                #endregion
            }
        }

        public override void Awake()
        {
            base.Awake();

        }

        public override void Destroy()
        {
            // 销毁对战信息表
            destroyVSInfoPlayerFrame();

            // 销毁数据网格
            destroyDataGridFrame();

            // 销毁数据表格
            destroyDataChartFrame();

            // 销毁奖励信息
            destroyAwardInfo();

            // 销毁RecordShare
            destroyRecordShare();

            // 销毁UpHeroLv
            destroyUpHeroLvUI();

            base.Destroy();
        }

        // 当点击取消按钮时退出战场进入静态场景
        public void onChickReturnButton()
        {
            SoundPlayHandler.StopSceneSound();
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_BACK_GAME);

            // 隐藏聊天框
            ChatForceConditionHide ChatData = new ChatForceConditionHide();
            ChatData.bForceHide = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, ChatData);
            LogicDataCenter.chatMessageManager.clearAllChatMsgData();
        }

        // 当点击炫耀时将自己的对战信息发送到聊天框中
        public void onClickFlauntButton()
        {
            EntityView objMaster = EntityFactory.getMainPlayer();
            if (objMaster == null)
            {
                Debug.LogError(" objMaster == NULL ");
                return;
            }
            ulong pDBID = (ulong)objMaster.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> dicCmdData = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();
            int nGlory = 0;
            int nMultKillMax = 0;
            int nContinueKillMax = 0;
            Int64 lWarID = 0;
            if (dicCmdData.ContainsKey(pDBID))
            {
                cmd_entity_end_player_data cmdData = dicCmdData[pDBID].data;
                nGlory = cmdData.nTitle;
                nMultKillMax = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX];
                nContinueKillMax = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_ALIVECONTKILLMAX];
                lWarID = dicCmdData[pDBID].dbWarID;
            }

            LogicDataCenter.warStatisticDataManager.ShowWarResult((int)pDBID, lWarID, nGlory, nMultKillMax, nContinueKillMax);
        }

        public void setRecordTable3DefualtToggle()
        {
            tmpToggle = dicToggle[(int)ERecord_Data_Type.ERDT_OUTPUT];
            tmpToggle.isOn = true;

            tmpToggle = dicToggle[(int)ERecord_Data_Type.ERDT_BEARDAMAGE];
            tmpToggle.isOn = true;
            tmpToggle = dicToggle[(int)ERecord_Data_Type.ERDT_MONEY];
            tmpToggle.isOn = true;

            tmpToggle = null;
        }

        public void onClickGroupButton(UButtonGroupItem item, bool bActive)
        {
            int nGroupIndex = item.GroupIndex;
            for(int i = 0; i < (int)EFrameType.FRAME_MAX; ++i)
            {
                if(nGroupIndex == i)
                {
                    Frames[i].SetActive(true);
                }
                else
                {
                    Frames[i].SetActive(false);
                }
            }

            // 除了对战信息界面，其他界面不显示
            if(nGroupIndex == (int)EFrameType.FRAME_VSINFO)
            {
                AwardInfoFrame.gameObject.SetActive(true);
                showBtn.SetActive(true);
                ShareBar.gameObject.SetActive(true);
            }
            else
            {
                AwardInfoFrame.gameObject.SetActive(false);
                showBtn.SetActive(false);
                ShareBar.gameObject.SetActive(false);
            }

        }

        #region 新手引导功能
        /// <summary>
        /// 返回按钮
        /// </summary>
        public Button ExitBtn = null;
        internal void AddExitBtnGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (ExitBtn != null)
            {
                Guide.UGuideWidget guideWidget = ExitBtn.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = ExitBtn.gameObject.AddComponent<Guide.UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)_msgData.nGuideID;
                guideWidget.GuideStepID = _msgData.nGuideStepID;
                guideWidget.GuideEffectID = _msgData.nGuideEffectID;

            }

        }

        internal void RemoveExitBtnGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (ExitBtn != null)
            {
                Guide.UGuideWidget guideWidget = ExitBtn.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget != null)
                {
                    ExitBtn.gameObject.RemoveComponent<Guide.UGuideWidget>();
                }
            }
        }
        #endregion
    }

    // 奖励
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        private enum TimerID
        {
            TimerID_PopFrameInterval,
        }

        public float treasureChestsAnomationTime = 1.0f;
        public float heroExpAnomationTime = 4.0f;
        public float popFrameDoTweenTime = 0.5f;
        public Text UnlockText;
        public GameObject UnlockFrame;
        public Text benefitCardText;

        public Transform AwardInfoFrame;

        ResNode medalFrameRes;
        ResNode cardFrameRes;
        GameObject cardGroup;
        GameObject cardAwardDesc;
        Text GetExp;
        Text GetGold;
        Text UpLvTip;
        Transform GetExpTrans;
        Transform GetGoldTrans;

        Text antiReduceRate;
        Image firstWinArark;
        UEffectParamBase firstWinBoxEffect;
        UEffectParamBase heroUpgrade;
        Sequence mySequence;
        private const int m_nMedalScoreTypeNum = 6;
        private List<GameObject> PopFrameList = new List<GameObject>();
        private int m_nPopFrameIndex = -1;

        ResNode promptWndReqRes;

        private void initAwardInfo()
        {
            GetGold = AwardInfoFrame.FindChild("GetGold").FindChild("Value").GetComponent<Text>();
            GetExp = AwardInfoFrame.FindChild("GetExp").FindChild("Value").GetComponent<Text>();
            UpLvTip = AwardInfoFrame.FindChild("GetExp").FindChild("UpLvTip").GetComponent<Text>();

            antiReduceRate = AwardInfoFrame.FindChild("antiReduceRate").GetComponent<Text>();
            firstWinArark = AwardInfoFrame.FindChild("FirstWinAwark").GetComponent<Image>();
            cardFrameRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/HeroCard");
            cardGroup = AwardInfoFrame.transform.FindChild("GetCardGroup").gameObject;
            cardAwardDesc = cardGroup.transform.FindChild("Text").gameObject;

            GetExpTrans = AwardInfoFrame.FindChild("GetExp");
            GetGoldTrans = AwardInfoFrame.FindChild("GetGold");

            initPromptWnd();
        }

        private void destroyAwardInfo()
        {
            if (firstWinBoxEffect != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref firstWinBoxEffect);
            }
            if (heroUpgrade != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref heroUpgrade);
            }

            if (mySequence != null)
            {
                mySequence.Kill();
            }
            AssetBundleManager.DeleteAssets(ref promptWndReqRes, true);
            AssetBundleManager.DeleteAssets(ref medalFrameRes, true);
            AssetBundleManager.DeleteAssets(ref cardFrameRes, true);

            TimerManager.KillTimer(this, (int)TimerID.TimerID_PopFrameInterval);
        }

        // 提示玩家没有获得奖励的原因
        public void notHaveAwakeReason()
        {
            if (promptWnd != null)
            {
                promptWnd.gameObject.SetActive(true);
                promptWnd.gameObject.transform.DOScale(1.0f, 1.0f).SetDelay(1f);
            }
        }

        // 设置奖励信息
        public void setAwardInfoData()
        {
            // 检测数据是否有改变
            if (!LogicDataCenter.recordTableDataManager.checkIsShowAwarkUI())
            {
                // 如果没有改变的数据则显示没有获得奖励的原因
                notHaveAwakeReason();
                return;
            }

            cmd_entity_return_player_awark cmdData = LogicDataCenter.recordTableDataManager.getPlayerAwake();
            if (cmdData.nPlayerOldLv > cmdData.nPlayerNewLv)
            {
                Debug.LogError("cmdData.nPlayerOldLv > cmdData.nPlayerNewLv setAwarkInfo fail!");
                return;
            }

            if(cmdData.nActorGetExp > 0)
            {
                GetExpTrans.gameObject.SetActive(true);
                GetExp.text = "+" + cmdData.nActorGetExp.ToString();
                int nActorLevel = cmdData.nPlayerNewLv;
                if(cmdData.nPlayerOldLv != cmdData.nPlayerNewLv)
                {
                    UpLvTip.text = String.Format("等级提升 {0}级",cmdData.nPlayerNewLv);
                    UpLvTip.gameObject.SetActive(true);
                }
            }
            else
            {
                GetExpTrans.gameObject.SetActive(false);
            }

            //getplayerExp.text = cmdData.nActorCurExp + "/" + cmdData.nActorNeedExp;
            //playerLv.text = nActorLevel.ToString();
            //getHonor.text = cmdData.nSuccessScore.ToString();
            // 设置获得卡牌
            List<cmd_entity_hero_card> listCardInfo = LogicDataCenter.recordTableDataManager.getListCardInfo();
            int nCount = listCardInfo.Count;

            //RectTransform rectInfo = cardGroup.GetComponent<RectTransform>();
            //Vector2 rectSize = rectInfo.sizeDelta;
            //rectSize.x = rectSize.x * nCount;
            //rectInfo.sizeDelta = rectSize;

            if(nCount > 0)
            {
                cardAwardDesc.SetActive(true);
            }
            else
            {
                cardAwardDesc.SetActive(false);
            }

            for (int i = 0; i < nCount; ++i)
            {
                GameObject instance = cardFrameRes.InstanceMainRes();
                if (!instance)
                {
                    Debug.LogError("setAwarkInfo instance = NULL");
                    return;
                }
                instance.name = cardFrameRes.AssetName;
                instance.transform.SetParent(cardGroup.transform, false);
                if (instance)
                {
                    UHeroCardInfo uCardInfo = instance.GetComponent<UHeroCardInfo>();
                    uCardInfo.setCardInfo(listCardInfo[i]);
                }
            }

            // 判断玩家是否获得首胜
            //bool bIsFirstWinAwark = Convert.ToBoolean(cmdData.nIsFirstWinAwark);
            //int nOldHeroExp = cmdData.nActorOldExp;
            //if (bIsFirstWinAwark)
            //{
            //    // 播放宝箱动画
            //    firstWinBoxEffect = new UEffectPrefabParam(UEffectPrefabType.UEPT_UWarRecordTableEnd_FirstWinBox, _tfAttachParent: this.transform);
            //    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref firstWinBoxEffect);
            //}

            // 防沉迷衰减
            if (cmdData.nAntiReduceRate < 100)
            {
                antiReduceRate.text = string.Format(ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "AntiReduceRate"), cmdData.nAntiReduceRate);
                cmdData.nGold = cmdData.nGold * cmdData.nAntiReduceRate / 100;
            }

            if(cmdData.nGold > 0)
            {
                GetGoldTrans.gameObject.SetActive(true);
                GetGold.text = String.Format("+{0}", cmdData.nGold);
            }
            else
            {
                GetGoldTrans.gameObject.SetActive(false);
            }

            // TODO 宝箱动画播放完后执行下面的函数
            //firstWinAwarkAnomationComplete(cmdData);
        }

        // 初始化提示界面
        void initPromptWnd()
        {
            promptWndReqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/PromptWnd");
            if (promptWndReqRes == null)
            {
                Debug.LogError("initPromptWnd promptWnd = null");
                return;
            }
            GameObject instance = promptWndReqRes.InstanceMainRes();
            if (!instance)
            {
                Debug.LogError("initPromptWnd instance = NULL");
                return;
            }
            instance.SetActive(false);

            instance.name = promptWndReqRes.AssetName;
            if (instance != null)
            {
                instance.transform.SetParent(this.transform, false);
                promptWnd = instance.GetComponent<UPromptWnd>();
            }
        }

        // 首胜动画播放完后进行的处理
        void firstWinAwarkAnomationComplete(cmd_entity_return_player_awark data)
        {
            //if (firstWinArark.gameObject.activeSelf)
            //{
            //    firstWinArark.gameObject.SetActive(false);
            //}
            //// 设置自己的得分奖牌
            ////setMedal();

            //// 设置角色经验
            //int nInitValue = 0;
            //DOTween.To(() => nInitValue, x => nInitValue = x, data.nActorGetExp, heroExpAnomationTime).OnUpdate(() => updateHeroExp(nInitValue));
            //int nLevelOffset = data.nPlayerNewLv - data.nPlayerOldLv;
            //if (nLevelOffset > 0)
            //{
            //    Sequence tweenExpSlider = DOTween.Sequence();
            //    for (int i = 0; i < nLevelOffset; ++i)
            //    {
            //        if (i == 0)
            //        {
            //            tweenExpSlider.PrependInterval(1);
            //        }

            //        //if (i == nLevelOffset - 1)
            //            //tweenExpSlider.Append(heroExpSlider.DOValue(data.nActorOldNeedExp, (heroExpAnomationTime / 4.0f)).OnComplete(() => UpdateLevelExpInfo(data.nPlayerNewLv, data.nActorNeedExp)));
            //        //else
            //            //tweenExpSlider.Append(heroExpSlider.DOValue(data.nActorOldNeedExp, (heroExpAnomationTime / 4.0f)).OnComplete(() => ClearExpBarValue()));
            //    }

            //    //tweenExpSlider.Append(heroExpSlider.DOValue(data.nActorCurExp, (heroExpAnomationTime / 4.0f)).OnComplete(() => BeginPopFrame()));
            //    tweenExpSlider.OnKill(() => tweenExpSlider = null);
            //}
            //else if (nLevelOffset == 0)
            //{
            //    //heroExpSlider.DOValue(data.nActorCurExp, heroExpAnomationTime).OnComplete(() => BeginPopFrame());
            //}
            // 角色经验条音效
            //SSchemeUSoundData soundData = new SSchemeUSoundData();
            //if (UISoundConfig.Instance.GetData("statistics", "Button_statisticsxp", ref soundData))
            //{
            //    SoundManager.CreateUISound(soundData.USoundID);
            //}
        }

        private void ClearExpBarValue()
        {
            //heroExpSlider.value = 0;
        }

        void UpdateLevelExpInfo(int nNewLv, int nNewNeedExp)
        {
            //heroExpSlider.maxValue = nNewNeedExp;
            //heroExpSlider.value = 0;
            // 升级后设置新老等级
            //heroOldLv.text = nNewLv.ToString();
            //heroNewLv.text = (nNewLv + 1).ToString();
        }

        // 处理弹框
        void BeginPopFrame()
        {
            TimerManager.SetTimer(this, (int)TimerID.TimerID_PopFrameInterval, 10.0f);

            //setUpHeroLv();
            // CheckUnlockActorPrize();
            //CheckUnlockActorPrizeEx();
            //CheckUnlockHeroPrize();
            BeginPopSquenTween();
        }

        private void BeginPopSquenTween()
        {
            if (m_nPopFrameIndex >= 0 && m_nPopFrameIndex < PopFrameList.Count && PopFrameList[m_nPopFrameIndex] != null)
            {
                PopFrameList[m_nPopFrameIndex].SetActive(false);
            }

            m_nPopFrameIndex++;

            if (m_nPopFrameIndex < 0 || m_nPopFrameIndex >= PopFrameList.Count || PopFrameList[m_nPopFrameIndex] == null)
            {
                return;
            }

            GameObject popObj = PopFrameList[m_nPopFrameIndex];
            popObj.transform.SetParent(this.transform, false);
            popObj.transform.localPosition = Vector3.zero;

            bool bIsHeroObj = popObj.GetComponent<UViewHeroUpLvFrame>();
            if (bIsHeroObj)
                popObj.transform.DOScale(1.0f, popFrameDoTweenTime).OnComplete(() => playHeroLvUpEffect());
            else
                popObj.transform.DOScale(1.0f, popFrameDoTweenTime);
        }

        void updateHeroExp(int nExp)
        {
            //getExp.text = nExp.ToString();
        }

        // 显示增益卡相关信息
        public void displayBenefitCardInfo()
        {
            List<BenefitCardInfo> usedCardInfoList = DataCenter.LogicDataCenter.playerSystemDataManager.GetBenefitCardManager().UsedCardList;
            foreach (var item in usedCardInfoList)
            {
                Text text = ResNode.InstantiateRes<Text>(benefitCardText.gameObject);
                text.text = item.getRemainTimeDesc();
                text.gameObject.SetActive(true);

                text.transform.SetParent(benefitCardText.transform.parent, false);
            }
        }
    }

    // UpHeroLvUI
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        private void initUpHeroLvUI()
        {
            upHeroLvFrameRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarRecordTable_End/HeroUpLvFrame");
        }

        private void destroyUpHeroLvUI()
        {
            AssetBundleManager.DeleteAssets(ref upHeroLvFrameRes, true);
        }

        public void setUpHeroLv()
        {
            if (!LogicDataCenter.recordTableDataManager.m_HeroIsUpgrade)
            {
                return;
            }

            cmd_entity_statichero_upgrade cmdData = LogicDataCenter.recordTableDataManager.HeroUpgradeInfo;
            GameObject heroLevelUpObj = DOPopFrameLoadAndMakeDoTween("HeroUpLvFrame");
            if (heroLevelUpObj == null)
            {
                return;
            }

            UViewHeroUpLvFrame uUpLvFrame = heroLevelUpObj.GetComponent<UViewHeroUpLvFrame>();
            if (uUpLvFrame == null)
            {
                Debug.LogError("uUpLvFrame == NULL");
            }

            uUpLvFrame.setHeroUpLvInfo(cmdData);
            // uUpLvFrame.transform.DOScale(1.0f, popFrameDoTweenTime).OnComplete(() => playHeroLvUpEffect());
        }

        public void playHeroLvUpEffect()
        {
            //播放升级光效
            heroUpgrade = new UEffectPrefabParam(UEffectPrefabType.UEPT_UWarRecordTableEnd_HeroUpgrade, _tfAttachParent: this.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref heroUpgrade);
        }

        public void OnPopFrameClick()
        {
            TimerManager.KillTimer(this, (int)TimerID.TimerID_PopFrameInterval);
            TimerManager.SetTimer(this, (int)TimerID.TimerID_PopFrameInterval, 10.0f);
            BeginPopSquenTween();
        }

        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            // Draggable dragInfo = Obj.GetComponent<Draggable>().
            eventData.pointerDrag.GetComponent<Button>().enabled = false;
        }

        public virtual void OnEndDrag(PointerEventData eventData)
        {
            // Draggable dragInfo = Obj.GetComponent<Draggable>().
            eventData.pointerDrag.GetComponent<Button>().enabled = true;
        }

        public void CheckUnlockHeroPrize()
        {
            if (!LogicDataCenter.recordTableDataManager.m_HeroIsUpgrade)
            {
                UnlockFrame.SetActive(false);
                if (mySequence != null)
                {
                    mySequence.Kill();
                }
            }
            else
            {
                cmd_entity_return_player_awark playerInfo = LogicDataCenter.recordTableDataManager.getPlayerAwake();
                if (playerInfo.nCurrentLv <= playerInfo.nHeroOldLv)
                {
                    return;
                }

                cmd_entity_statichero_upgrade heroInfo = LogicDataCenter.recordTableDataManager.HeroUpgradeInfo;
                int nHeroId = heroInfo.nHeroID;
                int nPrizeId = 0;
                List<HeroLevelPrizeUnlockItem> listHero = null;
                PrizeUnlockConfig.Instance.heroLevelPrizeUnlockDic.TryGetValue(nHeroId, out listHero);
                if (listHero == null || listHero.Count <= 0)
                {
                    return;
                }

                SSchemeActorPrizeConfig config = new SSchemeActorPrizeConfig();
                foreach (HeroLevelPrizeUnlockItem configItem in listHero)
                {
                    if (configItem.nUnlockLevel > playerInfo.nCurrentLv || configItem.nUnlockLevel <= playerInfo.nHeroOldLv)
                    {
                        continue;
                    }

                    nPrizeId = configItem.nPrizeIdList[0];
                    GameObject PopFrameObj = DOPopFrameLoadAndMakeDoTween("HeroUnlockPrizeFrame");
                    if (PopFrameObj == null)
                    {
                        return;
                    }

                    PopFrameHeroUnlockPrize heroItem = PopFrameObj.GetComponent<PopFrameHeroUnlockPrize>();
                    if (heroItem == null)
                    {
                        return;
                    }

                    config = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeId);
                    if (config == null)
                    {
                        continue;
                    }

                    heroItem.SetHeroUnlockPrizeView(config.nPrizeIcon, config.nPrizeType, config.strPrizeName);
                }

                if (config.nPrizeID <= 0)
                {
                    return;
                }

                UnlockFrame.SetActive(true);
                UnlockText.text = "恭喜 \"英雄解锁\" 获得奖励 " + config.strPrizeName + " : " + config.strPrizeDesc;
                int nMoveDistance = (UnlockText.text.Length - 1) * 20 + 410;
                mySequence = DOTween.Sequence();
                mySequence.Append(UnlockText.gameObject.transform.DOLocalMoveX(UnlockText.gameObject.transform.localPosition.x - nMoveDistance, 10));
                mySequence.SetLoops(-1, LoopType.Restart);
            }
        }

        private void CheckUnlockActorPrize()
        {
            cmd_entity_return_player_awark playerInfo = LogicDataCenter.recordTableDataManager.getPlayerAwake();
            if (playerInfo.nPlayerNewLv <= playerInfo.nPlayerOldLv)
            {
                return;
            }

            List<ActorPrizeUnlockItem> listActorConfig = PrizeUnlockConfig.Instance.actorPrizeUnlockList;
            if (listActorConfig == null || listActorConfig.Count <= 0)
            {
                return;
            }

            SSchemeActorPrizeConfig config = new SSchemeActorPrizeConfig();
            foreach (ActorPrizeUnlockItem item in listActorConfig)
            {
                if (item.nUnlockLevel > playerInfo.nPlayerNewLv || item.nUnlockLevel <= playerInfo.nPlayerOldLv)
                {
                    continue;
                }

                GameObject PopFrameObj = DOPopFrameLoadAndMakeDoTween("ActorUnlockView");
                if (PopFrameObj == null)
                {
                    return;
                }

                PopFrameActorUnlockPrize actorItem = PopFrameObj.GetComponent<PopFrameActorUnlockPrize>();
                if (actorItem == null)
                {
                    return;
                }

                config = ActorPrizeConfig.Instance.GetActorPrizeConfig(item.nPrizeIdList[0]);
                if (config == null)
                {
                    continue;
                }

                actorItem.SetActorUnlockPrizeView(config.nPrizeIcon, config.nPrizeType, config.strPrizeName);
            }
        }

        public GameObject DOPopFrameLoadAndMakeDoTween(string sPrefName)
        {
            ResNode PopFrameNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "DynamicState_Moba/WarRecordTable_End/" + sPrefName);
            if (PopFrameNode == null)
            {
                return null;
            }

            GameObject PopFrameObj = PopFrameNode.InstanceMainRes();
            if (PopFrameObj == null)
            {
                return null;
            }


            PopFrameAddDelegateEvent(PopFrameObj);
            //PopFrameObj.transform.SetParent(this.transform,false);
            //PopFrameObj.transform.localPosition = Vector3.zero;

            PopFrameList.Add(PopFrameObj);
            PopFrameObj.SetActive(true);
            //PopFrameObj.transform.DOScale(1.0f, 1.0f);

            return PopFrameObj;
        }

        public void PopFrameAddDelegateEvent(GameObject PopFrameObj)
        {
            DraggableHandle draggableHandle = PopFrameObj.GetComponent<DraggableHandle>();

            if (draggableHandle == null)
            {
                draggableHandle = PopFrameObj.AddComponent<DraggableHandle>();
                draggableHandle.Drag(PopFrameObj.GetComponent<RectTransform>());
            }

            draggableHandle.OnBeginDragEvent += OnBeginDrag;
            draggableHandle.OnEndDragEvent += OnEndDrag;

            PopFrameObj.GetComponent<Button>().onClick.AddListener(OnPopFrameClick);
            //PopFrameObj.GetComponent<DraggableHandle>().OnEndDragEvent += OnEndDrag;
        }

        public virtual void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)TimerID.TimerID_PopFrameInterval:
                    {
                        BeginPopSquenTween();
                    }
                    break;
                default:
                    break;
            }
        }

        //private void CheckUnlockActorPrizeEx()
        //{
        //    cmd_entity_return_player_awark playerInfo = LogicDataCenter.recordTableDataManager.getPlayerAwake();
        //    if (playerInfo.nPlayerNewLv <= playerInfo.nPlayerOldLv)
        //    {
        //        return;
        //    }

        //    List<ActorPrizeUnlockItem> listActorConfig = PrizeUnlockConfig.Instance.actorPrizeUnlockList;
        //    if (listActorConfig == null || listActorConfig.Count <= 0)
        //    {
        //        return;
        //    }

        //    bool bHasActorUnlock = false;
        //    SSchemeActorPrizeConfig config = new SSchemeActorPrizeConfig();
        //    foreach (ActorPrizeUnlockItem item in listActorConfig)
        //    {
        //        if (item.nUnlockLevel > playerInfo.nPlayerNewLv || item.nUnlockLevel <= playerInfo.nPlayerOldLv)
        //        {
        //            continue;
        //        }

        //        bHasActorUnlock = true;
        //        break;
        //    }

        //    if (!bHasActorUnlock)
        //    {
        //        return;
        //    }


        //    GameObject PopFrameObj = DOPopFrameLoadAndMakeDoTween("ActorUnlockView");
        //    if (PopFrameObj == null)
        //    {
        //        return;
        //    }

        //    ActorUnlockView actorView = PopFrameObj.GetComponent<ActorUnlockView>();
        //    if (actorView == null)
        //        return;

        //    LogicDataCenter.playerSystemDataManager.bRequestSelf = true;
        //    LogicDataCenter.playerSystemDataManager.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
        //    actorView.InitActorUnlockView();
        //    actorView.UpDataActorUnlockView();
        //    actorView.checkActorLevelChangeToUnlock();
        //}

    }

    // RecordShare
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        public UShareBar ShareBar;
        public RectTransform ShareContentRt;

        private readonly string shareImgName = "WarRecordEndTemp.jpg";
        private string shareImgPath;

        private void initRecordShare()
        {
            shareImgPath = "";

            int nActorID = 0;
            if (null != EntityFactory.MainHeroView && null != EntityFactory.MainHeroView.Property)
            {
                nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }

            shareImgPath = nActorID + shareImgName;

            ShareBar.SetData(SocialSharingType.WarRecord, CaptureScreen.GetInstance().CaptureTempDir + "/" + shareImgPath);
        }

        private void destroyRecordShare()
        {
            shareImgPath = "";
        }

        private Rect GetRecordRect()
        {
            Vector3[] worldCorners = new Vector3[4];

            ShareContentRt.GetWorldCorners(worldCorners);

            Vector3 corner0 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[0]);
            Vector3 corner1 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[1]);
            Vector3 corner3 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[3]);

            return new Rect(corner0.x, corner0.y, corner3.x - corner0.x, corner1.y - corner0.y);
        }
    }
}

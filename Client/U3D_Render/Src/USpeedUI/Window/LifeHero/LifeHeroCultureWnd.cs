using ASpeedGame.Data.LifeHero;
using ASpeedGame.Data.Scheme;
using Data.ActorPrizeConfig;
using Data.PersonModel;
using DataCenter;
using GameLogic;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;

namespace USpeedUI.LifeHero
{
    public class LifeHeroCultureWnd : UIFillWnd<LifeHeroCultureView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LIFE_HERO_CULTURE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override string GetPrefabPath()
        {
            return "LifeHero/LifeHeroCulture";
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bHaveEscCallBack = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_INJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_CLOSE, this);

            return true;
        }

        public override bool OnEscCallBack()
        {
            if (m_wndView != null)
            {
                return m_wndView.ProcessEscKey();
            }

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_INJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_CLOSE, this);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                // 加载本命英雄场景
                SceneManager.Instance.LoadDestinedHeroViewScene(SetRootNodeAfterLoadScene);
            }
            else
            {
                if (m_wndView != null)
                    m_wndView.Clear();
                
                // 卸载场景
                SceneManager.Instance.UnLoadDestinedHeroViewScene();

                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_ENTRANCE, WndMsgID.WND_MSG_LIFEHERO_ENTRANCE_REFRESH_DATA, null);
            }
        }

        private void SetRootNodeAfterLoadScene(SceneResInfoManager info)
        {
            if (m_wndView != null)
                m_wndView.SetRootSceneNode(info);
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
                case WndMsgID.WND_MSG_LIFEHERO_INJECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnInjectLifeHero(data as ULifeHeroInjectHeroData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnRecvHeroDetailInfo(data as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_HERODETAIL_CLOSE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RestoreVeiwState();
                        }
                    }
                    break;
                #region 引导相关的
                case WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONBEGIN:
                    {
                        UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)data;

                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GuideActionBegin(cmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LIFEHERO_CULTURE_GUIDE_ACTIONEND:
                    {
                        UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)data;

                        if (m_wndView != null && cmdData != null)
                        {
                            m_wndView.GuideActionEnd(cmdData);
                        }
                    }
                    break;
                #endregion
                default:
                    break;
            }
        }
    }

    [Serializable]
    public class CultureItem
    {
        public UTooltipTrigger heroTip;
        public Button cultureBtn;
        public Button geneBtn;
        public Text starLevelDes;
        public Button replaceBtn;

        public void InitCultureItem()
        {
            cultureBtn.gameObject.GetComponent<Image>().enabled = true;
            heroTip.gameObject.GetComponent<Image>().enabled = false;
            geneBtn.gameObject.SetActive(false);
            starLevelDes.gameObject.SetActive(false);
        }
    }

    public enum LifeHeroSoundID
    {
        ADD_LIFEHERO_SOUND = 436,
    }

    public class LifeHeroCultureView : UIBaseWndView
    {
        public GameObject cultures;
        public Button backBtn;
        public CultureItem[] cultureArray;
        public InjectFrame injectFrame;
        public PrizeFrame prizeFrame;

        // 当前选中的cultureID,就是培养仓的索引，从左到右依次是(0 - ((int)ELifeHeroType.LIFEHERO_MAX_COUNT - 1)）
        private int mCurrentSelectCultureID = -1;
        //有英雄的培养仓(key:cultureID,value:heroID)
        private Dictionary<int, int> m_hasHeroCulture = new Dictionary<int, int>();
        private Dictionary<int, Color32> m_starTipDic = new Dictionary<int, Color32>();
        #region recordShare
        public UShareBar ShareBar;
        public RectTransform ShareContentRt;
        public GameObject RecordShareFrame;
        public RawImage ShareRawImg;
        public Text ShareDesc;
        public Text shareConfirmBtnText;

        private readonly string shareImgName = "LifeHeroCultureTemp.jpg";

        private Texture2D shareTex2D;
        #endregion

        private ULifeHeroInjectHeroData m_injectMsgData = new ULifeHeroInjectHeroData();
        private Transform SelectTransForm;
        private Transform[] heroMountPoints = new Transform[(int)ELifeHeroType.LIFEHERO_MAX_COUNT];
        private bool m_isPlayingEffect = false;
        private UEffectParamBase m_fillupEffectParam;
        private SceneEffectParamBase[] m_heroModleArray = new SceneEffectParamBase[(int)ELifeHeroType.LIFEHERO_MAX_COUNT];
        private bool m_openShareFrame = false;
        // 满足基因强化的和觉醒条件的时光效
        private UEffectParamBase[] m_canAddOrUpSatrArray = new UEffectParamBase[(int)ELifeHeroType.LIFEHERO_MAX_COUNT];
        private bool m_isJumpToHeroDetail = false;

        public void RestoreVeiwState()
        {
            if (m_isJumpToHeroDetail)
            {
                m_isJumpToHeroDetail = !m_isJumpToHeroDetail;
                SetVisible(true);
            }
        }

        public void SetRootSceneNode(SceneResInfoManager info)
        {
            if (info != null)
            {
                for (int i = 0; i < m_heroModleArray.Length; ++i)
                {
                    m_heroModleArray[i] = null;
                }
                SelectTransForm = info.SceneRoot.transform;
                SetHeroMountPoint();
                LoadStandbyEffect();
                LoadLifeHero();
                prizeFrame.SetData();
                SetReplaceBtnVisible();
                LoadAllAddHeroOrUpSatrEffect();
            }
            else
            {
                Debug.LogError("(" + SceneManager.DestinedHeroViewSceneName + ")场景加载失败!");
                return;
            }
        }

        private void LoadAllAddHeroOrUpSatrEffect()
        {
            for (int i = 0; i < cultureArray.Length; ++i)
            {
                LoadAddHeroOrUpSatrEffectByCultuerID(i);
            }
        }

        private void LoadAddHeroOrUpSatrEffectByCultuerID(int i)
        {
            if (m_canAddOrUpSatrArray[i] != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_canAddOrUpSatrArray[i]);
                m_canAddOrUpSatrArray[i] = null;
            }
            if (m_hasHeroCulture.ContainsKey(i))
            {
                int starLv = LogicDataCenter.playerSystemDataManager.GetHeroStarLv(m_hasHeroCulture[i]);
                if (starLv < 1)
                {
                    Debug.LogWarningFormat("sartLv({0}) < 1", starLv);
                    return;
                }
                if (GameLogicAPI.isHeroCanUpStar(m_hasHeroCulture[i], starLv, true))
                {
                    m_canAddOrUpSatrArray[i] = new UEffectPrefabParam(
                        _eType: UEffectPrefabType.UEPT_LifeHero_Can_AddHero_Upstar,
                        _tfAttachParent: cultureArray[i].geneBtn.transform, _bAutoDestroy: false);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_canAddOrUpSatrArray[i]);
                    (m_canAddOrUpSatrArray[i] as UEffectPrefabParam).EffectGameObject.transform.SetLocalPosition(new Vector3(0, 24, 0));
                }
            }
            else
            {
                if (LogicDataCenter.lifeHeroDataManager.CanAddLifeHero())
                {
                    m_canAddOrUpSatrArray[i] = new UEffectPrefabParam(
                        _eType: UEffectPrefabType.UEPT_LifeHero_Can_AddHero_Upstar,
                        _tfAttachParent: cultureArray[i].cultureBtn.transform, _bAutoDestroy: false);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_canAddOrUpSatrArray[i]);
                    (m_canAddOrUpSatrArray[i] as UEffectPrefabParam).EffectGameObject.transform.SetLocalPosition(new Vector3(0, -150, 0));
                }
            }
        }

        private void LoadLifeHero()
        {
            m_hasHeroCulture.Clear();
            m_isPlayingEffect = false;
            if (cultureArray != null && cultureArray.Length == (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
            {
                foreach (CultureItem item in cultureArray)
                {
                    item.InitCultureItem();
                }
            }
            else
            {
                Debug.LogError("预制体培养仓个数不对.请打开LifeHeroCulture.prefab,检查cultureArray变量.");
                return;
            }

            Dictionary<int, cmd_entity_lifehero_info> allLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero;
            if (allLifeHero != null)
            {
                foreach (cmd_entity_lifehero_info lifeHero in allLifeHero.Values)
                {
                    FillingCultureWithHero(lifeHero.nCultureID, lifeHero.nHeroID, true);
                }

                m_openShareFrame = allLifeHero.Count < (int)ELifeHeroType.LIFEHERO_MAX_COUNT ? true : false;
            }
        }

        public bool ProcessEscKey()
        {
            if (injectFrame.gameObject.activeSelf)
            {
                injectFrame.CloseFrame();
                return true;
            }

            if (RecordShareFrame.activeSelf)
            {
                CloseRecordShareFrame();
                return true;
            }

            m_wnd.SetVisible(false);

            return true;
        }

        public void Clear()
        {
            if (m_fillupEffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_fillupEffectParam);
                m_fillupEffectParam = null;
            }

            injectFrame.CloseFrame();
            CloseRecordShareFrame();
            prizeFrame.CloseFrame();

            UISystem.Instance.ShowWnd(WndID.WND_ID_LIFE_HERO_SELECT, false);
            UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZEPOPUP, false);

            SoundManager.DeleteSound((int)LifeHeroSoundID.ADD_LIFEHERO_SOUND);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, new UIDialogHide(EDialogType.LifeHeroCulture));
        }

        private void SetReplaceBtnVisible()
        {
            bool isHideReplaceBtn = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count == (int)ELifeHeroType.LIFEHERO_MAX_COUNT;
            foreach (CultureItem item in cultureArray)
            {
                item.replaceBtn.gameObject.SetActive(isHideReplaceBtn);
            }
        }

        // 设置与培养仓对应的英雄挂点
        private void SetHeroMountPoint()
        {
            Transform node = SelectTransForm.FindChild("Scenes_AlwaysNormal");
            if (node == null)
            {
                Debug.LogError("not find 'Scenes_AlwaysNormal' in 'DestinedHero View'");
                return;
            }
            node = node.FindChild("hero_guadian");
            if (node == null)
            {
                Debug.LogError("not find 'hero_guadian' in 'DestinedHero View'");
                return;
            }
            Dictionary<int, string> tmpDic = new Dictionary<int, string>(); // key：cultureID(培养仓ID)
            tmpDic.Add(2, "guadian01");
            tmpDic.Add(1, "guadian02");
            tmpDic.Add(3, "guadian03");
            tmpDic.Add(0, "guadian04");
            tmpDic.Add(4, "guadian05");
            foreach (var item in tmpDic)
            {
                Transform tmp = node.FindChild(item.Value);
                if (tmp == null)
                {
                    Debug.LogError("not find '" + item.Value + "' in 'DestinedHero View'");
                    return;
                }
                heroMountPoints[item.Key] = tmp;
            }
        }

        private void LoadStandbyEffect()
        {
            for (int i = 0; i < heroMountPoints.Length; ++i)
            {
                SceneEffectParamBase sceneEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_LifeHero_StandbyEffect, heroMountPoints[i], _bAutoPlay: true);
                SceneEffectManager.Instance.CreateSceneEffect(ref sceneEffectParam);
            }
        }

        public void OnBackBtnClicked()
        {
            m_wnd.SetVisible(false);
        }

        public void OncultureClicked()
        {
            if (m_isPlayingEffect)
                return;

            mCurrentSelectCultureID = IndexOfCultureArrayByCultureBtn(EventSystem.current.CurrentPointerEventData().selectedObject.gameObject);
            if (m_hasHeroCulture.ContainsKey(mCurrentSelectCultureID))
            {
                // 培养仓不为空。显示英雄全息投影。
                Debug.LogWarning("有英雄,显示英雄的界面index=" + mCurrentSelectCultureID);
            }
            else
            {
                OpenSelectView(ESelectViewState.INJECT);
            }
        }

        public void OnReplaceBtnClicked()
        {
            if (m_isPlayingEffect)
                return;

            if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count < (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LIFEHERO_NO_FULL_NO_REPLACE);
                return;
            }

            int index = -1;
            if (cultureArray != null)
            {
                GameObject obj = EventSystem.current.CurrentPointerEventData().selectedObject.gameObject;
                for (int i = 0; i < cultureArray.Length; ++i)
                {
                    if (cultureArray[i].replaceBtn.GetComponent<Button>().gameObject == obj)
                    {
                        index = i;
                        break;
                    }
                }
            }
            if (index < 0)
            {
                Debug.LogWarning("not get valid index in cultureArray by replaceBtn");
                return;
            }

            mCurrentSelectCultureID = index;
            OpenSelectView(ESelectViewState.REPLACE);
        }

        public void OnGeneBtnClicked()
        {
            if (m_isPlayingEffect)
                return;

            int index = IndexOfCultureArrayByGeneBtn(EventSystem.current.CurrentPointerEventData().selectedObject.gameObject);
            if (index < 0)
            {
                Debug.LogWarning("not get valid index in cultureArray by geneBtn");
                return;
            }
            mCurrentSelectCultureID = index;
            JumpHeroDetailView(m_hasHeroCulture[index]);
        }

        private void OpenSelectView(ESelectViewState state)
        { 
            // 培养仓为空。先判断是否有满足当前条件的本命英雄。若有，则跳转本命英雄选择界面，否则弹窗提示
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info == null || info.dicEntityHeroInfo == null)
                return;

            List<int> heroIDList = LogicDataCenter.lifeHeroDataManager.FindCandidatesFromHistoryRecord();
            if (heroIDList != null && heroIDList.Count > 0)
            {
                List<cmd_entity_hero_info> candidates = new List<cmd_entity_hero_info>();
                foreach (int heroID in heroIDList)
                {
                    cmd_entity_hero_info hero;
                    if (info.dicEntityHeroInfo.TryGetValue(heroID, out hero))
                    {
                        candidates.Add(hero);
                    }
                    else
                    {
                        Debug.LogWarning("在玩家拥有的英雄中未找到该英雄ID=" + heroID);
                    }
                }
                ULifeHeroCandidateData msgData = new ULifeHeroCandidateData();
                msgData.msgID = (int)WndID.WND_ID_LIFE_HERO_SELECT;
                msgData.candidates = candidates;
                msgData.cultureID = mCurrentSelectCultureID;
                msgData.state = state;
                if (state == ESelectViewState.REPLACE)
                {
                    msgData.heroID = m_hasHeroCulture[mCurrentSelectCultureID];
                }
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_SELECT, WndMsgID.WND_MSG_OPEN_LIFEHERO_SELECT, msgData);
            }
            else
            {
                SSchemeLifeHero config;
                int heroCount = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count;
                int key = state == ESelectViewState.INJECT ? heroCount : heroCount - 1;
                key = key < 0 ? 0 : key;
                if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out config))
                {
                    string content = ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelContent_NoLifeHero");
                    ShowDialog(ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelTitle"),
                        String.Format(content, config.rule),
                        new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Confirm"), GoToBattle }, { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Cancel"), null } });
                }
                else
                {
                    Debug.LogError("获取本命配置失败.key = " + key);
                }
            }
        }

        public void JumpHeroDetailView(int heroID)
        {
            if (heroID <= 0)
                return;

            UISystem.Instance.ShowWnd(WndID.WND_ID_LIFE_HERO_SELECT, false);
            UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZEPOPUP, false);
            cmd_Return_Hero_Detail_Info returnHeroInfo;
            returnHeroInfo.nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            returnHeroInfo.nHeroID = heroID;
            UIMsgCmdData uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, ref returnHeroInfo);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, uiReturnHeroInfo);
            m_isJumpToHeroDetail = true;
            SetVisible(false);
        }

        public void OnInjectLifeHero(ULifeHeroInjectHeroData msg)
        {
            if (msg == null || m_isPlayingEffect)
                return;

            m_injectMsgData = msg;
            if (this.gameObject.activeSelf)
                StartCoroutine(PlayEffect());
            else
                Debug.Log("culturewndview is inactive so not play effect.");
        }

        private IEnumerator PlayEffect()
        {
            m_isPlayingEffect = true;
            int index = m_injectMsgData.lifeHero.nCultureID;
            if (cultureArray == null || index >= cultureArray.Length)
            {
                Debug.LogError("预制体培养仓个数不对，无法播放光效.");
                m_isPlayingEffect = false;
                yield break;
            }

            if (m_heroModleArray[index] != null)
            {
                SceneEffectManager.Instance.DestroySceneEffect(ref m_heroModleArray[index]);
                m_heroModleArray[index] = null;
            }

            SoundManager.CreateUISound((int)LifeHeroSoundID.ADD_LIFEHERO_SOUND);

            m_fillupEffectParam = new UEffectPrefabParam(
                _eType: UEffectPrefabType.UEPT_LifeHero_FillUp,
                _tfAttachParent: cultureArray[index].cultureBtn.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_fillupEffectParam);
            yield return new WaitForSeconds(1.1f);

            cultureArray[index].cultureBtn.GetComponent<Image>().enabled = false;
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_fillupEffectParam);
            m_fillupEffectParam = null;

            SceneEffectParamBase sceneEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_LifeHero_VerticalDownBlink, heroMountPoints[index], _bAutoPlay: true);
            SceneEffectManager.Instance.CreateSceneEffect(ref sceneEffectParam);
            yield return new WaitForSeconds(0.7f);
            SceneEffectManager.Instance.DestroySceneEffect(ref sceneEffectParam);

            sceneEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_LifeHero_DNASpiralUp, heroMountPoints[index], _bAutoPlay: true);
            SceneEffectManager.Instance.CreateSceneEffect(ref sceneEffectParam);
            yield return new WaitForSeconds(0.6f);
            SceneEffectManager.Instance.DestroySceneEffect(ref sceneEffectParam);

            bool isSuccess = FillingCultureWithHero(m_injectMsgData.lifeHero.nCultureID, m_injectMsgData.lifeHero.nHeroID, false);
            if (isSuccess)
            {
                if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count == (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
                {
                    if (m_openShareFrame)
                    {
                        m_openShareFrame = !m_openShareFrame;
                        #region recordShare
                        shareTex2D = new Texture2D((int)ShareRawImg.rectTransform.sizeDelta.x, (int)ShareRawImg.rectTransform.sizeDelta.y);

                        yield return new WaitForSeconds(1.2f);

                        CaptureScreen.GetInstance().CaptureAreaImage(new CaptureAreaImgData(shareImgName, GetRecordRect()));
                        yield return new WaitForSeconds(0.5f);
                        //Invoke("OpenRecordShareFrame", 0.5f);
                        OpenRecordShareFrame();
                        #endregion
                    }
                }
                else
                {
                    yield return new WaitForSeconds(1.2f);
                    UEffectParamBase param = new UEffectPrefabParam(
                        _eType: UEffectPrefabType.UEPT_LifeHero_InjectFrame_Title,
                        _tfAttachParent: injectFrame.transform, _bAutoDestroy: true);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                    injectFrame.OpenInjectFrame(this, m_injectMsgData.lifeHero.nHeroID, param);
                }
            }

            m_isPlayingEffect = false;

            prizeFrame.SetObtainBtn();

            SetReplaceBtnVisible();

            yield break;
        }

        private void ShowDialog(string title, string content, DialogActions buttons)
        {
            UBB.toHtml(ref content);
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: title,
                        message: content,
                        modal: false,
                        hideByModalClick: false,
                        modalColor: Color.clear,
                        buttons: buttons,
                        mutex: true,
                        eDialogType: EDialogType.LifeHeroCulture
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public bool GoToBattle()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAIN_FRAME, true);
            m_wnd.SetVisible(false);
            return true;
        }

        private int IndexOfCultureArrayByCultureBtn(GameObject obj)
        {
            if (cultureArray != null)
            {
                for (int i = 0; i < cultureArray.Length; ++i)
                {
                    if (cultureArray[i].cultureBtn.GetComponent<Button>().gameObject == obj)
                        return i;
                }
            }

            return -1;
        }

        private int IndexOfCultureArrayByGeneBtn(GameObject obj)
        {
            if (cultureArray != null)
            {
                for (int i = 0; i < cultureArray.Length; ++i)
                {
                    if (cultureArray[i].geneBtn.GetComponent<Button>().gameObject == obj)
                        return i;
                }
            }

            return -1;
        }

        private bool FillingCultureWithHero(int cultureID, int heroID, bool isRemoveRongjie)
        {
            if (cultureArray == null
                || cultureID < 0
                || cultureID >= cultureArray.Length)
                return false;

            SSchemeLifeHeroLight lightConfig;
            if (LifeHeroConfig.Instance.LifeHeroLightDic.TryGetValue(heroID, out lightConfig))
            {
                int effectID;
                if (isRemoveRongjie)
                {
                    effectID = lightConfig.staticEffectID;
                }
                else
                {
                    effectID = lightConfig.effectID;
                }

                m_heroModleArray[cultureID] = new SceneEffectParamBase(effectID, heroMountPoints[cultureID], _bAutoPlay: true);
                SceneEffectManager.Instance.CreateSceneEffect(ref m_heroModleArray[cultureID]);

                cultureArray[cultureID].cultureBtn.gameObject.GetComponent<Image>().enabled = false;
            }
            else
            {
                Debug.LogError("not find hero model.heroID=" + heroID + ",cultureID=" + cultureID);
                return false;
            }

            m_hasHeroCulture[cultureID] = heroID;

            SetHeroToolTip(heroID, cultureID);

            LoadAddHeroOrUpSatrEffectByCultuerID(cultureID);

            return true;
        }

        private void SetHeroToolTip(int heroID, int cultureID)
        {
            if (cultureArray == null || cultureArray.Length <= cultureID)
            {
                return;
            }

            UTooltipTrigger tipsTrigger = cultureArray[cultureID].heroTip;
            tipsTrigger.gameObject.GetComponent<Image>().enabled = true;
            StringBuilder desc = new StringBuilder();
            SSchemePersonModelData sPersonModel = null;
            if (PersonModelConfig.Instance.GetData_Out(heroID, out sPersonModel))
            {
                desc.Append(String.Format("[size=20]{0}[/size]\n", sPersonModel.szHeroName));
                desc.Append(String.Format("[size=16][color=#a5dbef]{0}[/color][/size]\n", sPersonModel.szHeroTitle));
            }

            ActorHeroDetailInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(heroID, eType: HeroDetailReqType.HDRT_LIFEHERO);
            if (heroInfo != null && heroInfo.heroDetailInfo.nHeroID > 0)
            {
                cmd_entity_hero_detail_info info = heroInfo.heroDetailInfo;
                desc.Append(String.Format("[size=16][color=#a5dbef]场次：{0}[/color][/size]\n", info.nGameCount));
                desc.Append(String.Format("[size=16][color=#a5dbef]胜率：{0}%[/color][/size]\n", info.usWinPro));
                desc.Append(String.Format("[size=16][color=#a5dbef]英雄评级：{0}[/color][/size]", info.nStar));

                cultureArray[cultureID].geneBtn.gameObject.SetActive(true);
                Text starLvDes = cultureArray[cultureID].starLevelDes;
                starLvDes.gameObject.SetActive(true);
                starLvDes.text = String.Format("{0}({1})", heroInfo.heroDetailInfo.chStarDesc, heroInfo.heroDetailInfo.nStar);
                starLvDes.GetComponent<UGradient>().enabled = false;
                starLvDes.GetComponent<Outline>().enabled = true;
                if (heroInfo.heroDetailInfo.nStar > 0 && heroInfo.heroDetailInfo.nStar < 5)
                {
                    starLvDes.color = Color.white;
                }
                else if (heroInfo.heroDetailInfo.nStar < 10)
                {
                    starLvDes.color = new Color32(0x10, 0x9b, 0xfe, 0xff);
                }
                else if (heroInfo.heroDetailInfo.nStar < 20)
                {
                    starLvDes.color = new Color32(0xcc, 0x09, 0xce, 0xff);
                }
                else if (heroInfo.heroDetailInfo.nStar < 25)
                {
                    starLvDes.color = new Color32(0xd1, 0x8d, 0x5f, 0xff);
                }
                else if (heroInfo.heroDetailInfo.nStar < 30)
                {
                    starLvDes.color = new Color32(0xff, 0xf6, 0x4a, 0xff);
                }
                else
                {
                    starLvDes.color = new Color32(0xbc, 0x30, 0x00, 0xff);
                }
            }

            tipsTrigger.SetText(UTooltipParamName.BodyText, desc.ToString());
        }

        #region recordShare
        public void OpenRecordShareFrame()
        {
            string desc = String.Format(ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "ShareDesc"), EntityFactory.MainHeroView.Property.CreatureName);
            UBB.toHtml(ref desc);
            ShareDesc.text = desc;
            shareConfirmBtnText.text = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "NewChallenge");

            string imgFullPath = CaptureScreen.GetInstance().CaptureTempDir + "/" + shareImgName;
            if (File.Exists(imgFullPath))
            {
                byte[] imgData = ImageExtensions.GetAllByteData(imgFullPath);

                if (null != ShareRawImg)
                {
                    if (null == ShareRawImg.texture)
                    {
                        if (shareTex2D != null)
                            shareTex2D.LoadImage(imgData);
                        ShareRawImg.texture = shareTex2D;
                    }
                    ShareRawImg.gameObject.SetActive(true);
                }

                ShareBar.SetData(SocialSharingType.LifeHero, imgData);
            }
            RecordShareFrame.gameObject.SetActive(true);
        }

        public void CloseRecordShareFrame()
        {
            RecordShareFrame.gameObject.SetActive(false);
        }

        public void OnShareFrameSureBtnClicked()
        {
            GoToBattle();
            CloseRecordShareFrame();
        }

        public void OnRecvHeroDetailInfo(UIMsgCmdData data)
        {
            prizeFrame.SetObtainBtn();
            cmd_Return_Hero_Detail_Info info = IntPtrHelper.toData<cmd_Return_Hero_Detail_Info>(data.ptrParam);
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            if (info.nPDBID == nActorID)
            {
                foreach (var item in m_hasHeroCulture)
                {
                    if (item.Value == info.nHeroID)
                    {
                        SetHeroToolTip(item.Value, item.Key);
                        break;
                    }
                }
            }
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
        #endregion
        #region 新手引导相关
        public void GuideActionBegin(UGuideWidgetMsgData _msgData)
        {
            if (cultureArray.Length == 0)
                Trace.LogWarning("本命英雄培养仓数量为0？");
            if (_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_2003 && _msgData.nGuideStepID == 0)
            {
                Guide.UGuideWidget widget = cultureArray[0].cultureBtn.gameObject.AddComponent<Guide.UGuideWidget>();
                widget.SetFromMsgData(_msgData);
            }
            else if (_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_2003 && _msgData.nGuideStepID == 3)
            {
                this.injectFrame.GuideActionBegin(_msgData);
            }
            if(_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_2004 && _msgData.nGuideStepID == 0)
            {
                if(RecordShareFrame != null)
                {
                    Transform SureBtnTrans = RecordShareFrame.transform.FindChild("SureBtn");
                    if(SureBtnTrans == null)
                    {
                        Trace.LogError("LifeHeroCultureWnd.RecordShareFrame.SureBtn 对象移除了？目前这里需要引导，处理下。");
                        return;
                    }
                    Guide.UGuideWidget widget = SureBtnTrans.gameObject.AddComponent<Guide.UGuideWidget>();
                    if(widget != null)
                    {
                        widget.SetFromMsgData(_msgData);
                    }
                }
            }
        }
        public void GuideActionEnd(UGuideWidgetMsgData _msgData)
        {
            if (cultureArray.Length == 0)
                Trace.LogWarning("本命英雄培养仓数量为0？");

            if (_msgData.nGuideStepID == 0)
                cultureArray[0].cultureBtn.gameObject.RemoveComponent<Guide.UGuideWidget>();
            else if (_msgData.nGuideStepID == 3)
            {
                this.injectFrame.GuideActionEnd(_msgData);
            }

            if (_msgData.nGuideID == (int)GUIDE.EGuideNodeID.GuideNodeID_2004)
            {
                if (RecordShareFrame != null)
                {
                    Transform SureBtnTrans = RecordShareFrame.transform.FindChild("SureBtn");
                    if (SureBtnTrans == null)
                    {
                        Trace.LogError("LifeHeroCultureWnd.RecordShareFrame.SureBtn 对象移除了？目前这里需要引导，处理下。");
                        return;
                    }
                    SureBtnTrans.gameObject.RemoveComponent<Guide.UGuideWidget>();
                }
            }

            #endregion
        }
    }
    public class InjectFrame : MonoBehaviour
    {
        public Text content;
        public Text confirmBtnText;
        public Text backBtnText;
        public Text title;

        private LifeHeroCultureView m_parentView;
        private int m_currentHeroID;
        private UEffectParamBase m_titleEffectParam;
        public void Start()
        {
            title.text = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "InjectFrameTitle");
            backBtnText.text = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "InjectFrameBackBtn");
            confirmBtnText.text = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "InjectFrameConfirmBtn");
        }

        public void OpenInjectFrame(LifeHeroCultureView parentView, int heroID, UEffectParamBase param)
        {
            m_currentHeroID = heroID;
            m_parentView = parentView;
            m_titleEffectParam = param;
            SSchemeLifeHero config;
            int key = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count;
            if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out config))
            {
                string content = String.Format(ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelContent_FindOne"), config.rule);
                UBB.toHtml(ref content);
                this.content.text = content;
            }
            else
            {
                Debug.LogError("获取本命配置失败.key = " + key);
                content.text = "";
            }

            this.gameObject.SetActive(true);
        }

        public void OnConfirmBtnClicked()
        {
            m_parentView.JumpHeroDetailView(m_currentHeroID);
            CloseFrame();
        }

        public void OnBackBtnClicked()
        {
            CloseFrame();
        }

        public void CloseFrame()
        {
            if (m_titleEffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_titleEffectParam);
                m_titleEffectParam = null;
            }

            this.gameObject.SetActive(false);
        }

        #region 新手引导部分
        public void GuideActionBegin(UGuideWidgetMsgData _msgData)
        {
            if (confirmBtnText == null || confirmBtnText.transform.parent == null || confirmBtnText.transform.parent.GetComponent<Button>() == null)
            {
                Trace.LogError("是不是 InjectPrizeFrame.confirmBtnText 改了？这里需要新手引导响应，处理下。");
                return;
            }
            Button confirmBtn = confirmBtnText.transform.parent.GetComponent<Button>();
            Guide.UGuideWidget widget = confirmBtn.gameObject.AddComponent<Guide.UGuideWidget>();
            widget.SetFromMsgData(_msgData);
        }

        public void GuideActionEnd(UGuideWidgetMsgData _msgData)
        {
            if (confirmBtnText == null || confirmBtnText.transform.parent == null)
            {
                Trace.LogError("是不是 InjectPrizeFrame.confirmBtnText 改了？这里需要新手引导响应，处理下。");
                return;
            }
            confirmBtnText.transform.parent.gameObject.RemoveComponent<Guide.UGuideWidget>();
        }

        #endregion
    }

    public class PrizeFrame : MonoBehaviour
    {
        public PrizeItem prizeItem;
        public Button obtainBtn;
        public Text prizeDesc;
        private List<int> m_prizeIDList = new List<int>();
        private int m_taskID;

        public void Start()
        {
            LogicDataCenter.lifeHeroDataManager.AddPrizeTaskDataHandler(SetTaskData);
            obtainBtn.onClick.AddListener(OnObtainBtnClick);
            obtainBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "PrizeFrameObtainBtn");

            UEffectParamBase param = new UEffectPrefabParam(
                _eType: UEffectPrefabType.UEPT_LifeHero_PrizeFrame_ObtainBtn,
                _tfAttachParent: obtainBtn.transform, _bAutoDestroy: true);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
        }

        public void OnDestroy()
        {
            LogicDataCenter.lifeHeroDataManager.RemovePrizeTaskDataHandler(SetTaskData);
        }

        private void SetTaskData(int taskID)
        {
            m_taskID = taskID;
            SetData();
        }

        public void CloseFrame()
        {
            this.gameObject.SetActive(false);
        }

        public void SetData()
        {
            m_taskID = LogicDataCenter.lifeHeroDataManager.PrizeTaskData;
            if ( m_taskID >= 0 && m_taskID < LifeHeroConfig.Instance.LifeHeroPrizeList.Count)
            {
                this.gameObject.SetActive(true);
                Transform container = prizeItem.transform.parent;
                SSchemeLifeHeroPrize prizeConfig = LifeHeroConfig.Instance.LifeHeroPrizeList[m_taskID];
                m_prizeIDList = prizeConfig.prizeIDList;
                if (m_prizeIDList != null && m_prizeIDList.Count > 0)
                {
                    prizeItem.SetData(m_prizeIDList[0]);
                    int count = m_prizeIDList.Count - container.childCount;
                    int i;
                    if (count > 0)
                    {
                        for (i = 0; i < count; ++i)
                        {
                            PrizeItem item = Instantiate(prizeItem);
                            if (item != null)
                            {
                                item.transform.SetParent(container);
                                item.transform.localScale = Vector3.one;
                                item.transform.localRotation = item.transform.localRotation;
                                item.transform.SetLocalPosition(Vector3.zero);
                            }
                        }
                    }

                    for (i = 0; i < m_prizeIDList.Count; ++i)
                    {
                        GameObject child = container.GetChild(i).gameObject;
                        child.SetActive(true);
                        child.GetComponent<PrizeItem>().SetData(m_prizeIDList[i]);
                    }
                    for (; i < container.childCount; ++i)
                    {
                        container.GetChild(i).gameObject.SetActive(false);
                    }
                }
                else
                {
                    Debug.LogWarningFormat("该级别无奖励ID。taskID={0}", m_taskID);
                }

                string desc = prizeConfig.prizeDesc;
                UBB.toHtml(ref desc);
                prizeDesc.text = desc;

                SetObtainBtn();
            }
            else
            {
                this.gameObject.SetActive(false);
            }
        }

        public void SetObtainBtn()
        {
            if (m_taskID >= 0 && m_taskID < LifeHeroConfig.Instance.LifeHeroPrizeList.Count)
            {
                int meetHeroCount = 0;
                foreach (int heroID in LogicDataCenter.lifeHeroDataManager.AllLifeHero.Keys)
                {
                    ActorHeroDetailInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(heroID, eType: HeroDetailReqType.HDRT_LIFEHERO);
                    if (heroInfo != null && heroInfo.heroDetailInfo.nHeroID > 0)
                    {
                        if (heroInfo.heroDetailInfo.nStar >= LifeHeroConfig.Instance.LifeHeroPrizeList[m_taskID].starLv)
                        {
                            ++meetHeroCount;
                        }
                    }
                    else
                    {
                        Debug.Log("英雄详情信息还未收到？");
                    }
                }

                obtainBtn.gameObject.SetActive(meetHeroCount >= LifeHeroConfig.Instance.LifeHeroPrizeList[m_taskID].heroCount);
            }
            else
            {
                obtainBtn.gameObject.SetActive(false);
            }
        }

        public void OnObtainBtnClick()
        {
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_OBTAIN_PRIZE, 0);
            UPrizePopupData uiData = new UPrizePopupData();
            uiData.strPrizeTitle = ULocalizationService.Instance.Get("UIView", "LifeHeroCulture", "PrizeFrameTitle");
            uiData.strPrizeDesc = null;
            uiData.nPrizeIdList = m_prizeIDList;
            uiData.nHeroID = 0;
            uiData.nTalentIDList = new List<int>();
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_PRIZEPOPUP, WndMsgID.WND_MSG_PRIZEPOPUP_OPEN, uiData);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, new UIDialogHide(EDialogType.LifeHeroCulture));
        }
    }

    public class PrizeItem : MonoBehaviour
    {
        public Image PrizeIcon;
        public Text prizeName;

        public void SetData(int prizeID)
        {
            SSchemeActorPrizeConfig actorPrizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(prizeID);
            if (actorPrizeConfig != null)
            {
                this.PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_LIFE_HERO_CULTURE, actorPrizeConfig.nPrizeType, actorPrizeConfig.nPrizeIcon); ;
                this.prizeName.text = actorPrizeConfig.strPrizeName;
                this.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, actorPrizeConfig.strPrizeDesc);
            }
        }
    }
}

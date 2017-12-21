/*******************************************************************
** 文件名:	HeroTalentView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2016-11-25
** 版  本:	1.0
** 描  述:	战场英雄天赋选择
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using UnityEngine.EventSystems;
using System;
using GameLogic;
using DataCenter;
using USpeedUI.WarHeroTalent;
using UIWidgets;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using USpeedUI.UEffect;
using USpeedUI.WarBottonCenterPart;

namespace USpeedUI
{
    public class WarHeroTalentView : UIBaseWndView
    {
        public GameObject TalentButtonObject;     // 打开天赋按钮
        public GameObject LearnTalentHintEffect;  // 学习天赋提示特效
        public GameObject MainTalentShowObject;   // 当前天赋列表
        public Image MainBgImage;                 // 背景

        public UListHeroTalentList HeroTalentList;

        protected ObservableList<UListHeroTalentData> HeroTalentData = new ObservableList<UListHeroTalentData>();

        private UEffectPrefabParam m_OldEffectParam;
        private List<int> m_listTalentReconnect = new List<int>();

        // 自动激活天赋逻辑
        private CAutoActiveTalent m_AutoActiveTalent = null;

        public override void Start()
        {
            m_AutoActiveTalent = new CAutoActiveTalent(this);
        }

        public override void Update()
        {
            CheckFastLearnShortcut();
        }

        public void AddHeroTalentListData(int nTalentId, bool bCanUse, int nShowType, int nIconId, int nTalentLevel, string sTalentName, string sTalentDescInStatic, string sTalenDescInWar, int nOnlyActive, int nTalentTermId, bool isRecommandShow)
        {
            if (bCanUse == false)
            {
                return;
            }

            string sTalentDesc = sTalenDescInWar;
            UListHeroTalentData data = new UListHeroTalentData();
            data.nTalentId = nTalentId;
            data.nTalentIconId = nIconId;
            data.nTalentLevel = nTalentLevel;
            data.sTalentName = sTalentName;
            data.sTalentDesc = sTalentDesc;
            data.bOnlyActive = nOnlyActive > 0;
            data.bLock = !bCanUse;
            data.nTalentTermId = nTalentTermId;
            data.bRecommand = isRecommandShow;


            AddHeroTalentListData(data);
        }

        public void SetHeroTalentViewShowState()
        {
            transform.SetAsLastSibling();
            gameObject.SetActive(true);

            ShowMainTalent(true);
        }

        // 用于战场
        //public void SetLevelActiveHeroTalentInWar(int nSlotId)
        //{
        //    if (TalentViewPart == null)
        //    {
        //        return;
        //    }

        //    TalentViewPart.SetLevelActiveHeroTalentInWar(nSlotId);
        //}

        public void UpdateTalentItemShow()
        {
            HeroTalentList.UpdateItems();
        }

        // 检查学习天赋快捷键是否按下
        private void CheckFastLearnShortcut()
        {
            for (int slotID = 0; slotID < 6; slotID++)
            {
                if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.Ctrl_1 + slotID))
                {
                    int nIndex = slotID;
                    OnHeroTalentItemClick(nIndex, null);
                }
            }
        }

        public void AddHeroTalentListData(UListHeroTalentData data)
        {
            HeroTalentData.Add(data);
        }

        public virtual void SetHeroTalentListData()
        {
            HeroTalentList.DataSource = HeroTalentData;

            this.HeroTalentList.OnSelect.AddListener(OnHeroTalentItemClick);
            ShowMainTalent(false);
        }

        public void ClearHeroTalentData()
        {
            HeroTalentData.Clear();
            HeroTalentList.DataSource = HeroTalentData;
            SetTalentOpenButtonColorChange(false);
        }

        //public void SetTalentOpenButtonInWarVisible(bool bSHow)
        //{
        //    TalentButtonObject.SetActive(bSHow);
        //}

        public void ShowMainTalent(bool bSHow, bool bSendMessage = true)
        {
            // 天赋列表
            MainTalentShowObject.SetActive(bSHow);

            if (bSHow)
            {
                UpdateTalentView();
            }
            else
            {
                if (bSendMessage)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE, null);
                }
            }

            // 提示光效
            if (bSHow)
            {
                SetTalentOpenButtonColorChange(false);
            }
            else
            {
                UpdateTalentBtnEffect();
            }
        }

        // 更新天赋按钮特效状态
        public void UpdateTalentBtnEffect()
        {
            if (isHasTalentCanActive())
                SetTalentOpenButtonColorChange(true);
            else
                SetTalentOpenButtonColorChange(false);
        }

        private void UpdateTalentView()
        {
            int nLv = getCurShowTalentLv();

            ShowTalentView(nLv);
        }

        // 获取当前显示天赋的等级
        private int getCurShowTalentLv()
        {
            int nHeroCurLv = EntityUtil.getMainHeroLv();

            int nCount = LogicDataCenter.heroTalentDataManager.LevelArray.Length;

            for (int i = 0; i < nCount; ++i)
            {
                int nLv = LogicDataCenter.heroTalentDataManager.LevelArray[i];
                if (nLv <= nHeroCurLv)
                {
                    // 未激活
                    if (!LogicDataCenter.heroTalentDataManager.isTalentActivedThisLv(nLv))
                    {
                        return nLv;
                    }
                }
            }

            return getNextActiveTalentLv(nHeroCurLv);
        }

        public void ShowTalentView(int nLevel)
        {
            if (nLevel <= 0)
            {
                Debug.LogWarning(String.Format("ShowTalentView,nLevel=", nLevel));
                return;
            }

            bool isActived = LogicDataCenter.heroTalentDataManager.isTalentActivedThisLv(nLevel);
            ObservableList<UListHeroTalentData> NewTalentData = new ObservableList<UListHeroTalentData>();
            foreach (UListHeroTalentData data in HeroTalentData)
            {
                if (data.nTalentLevel == nLevel)
                {
                    NewTalentData.Add(data);
                }
            }

            int nSortIndex = 0;
            foreach (UListHeroTalentData data in NewTalentData)
            {
                data.nSortIndex = ++nSortIndex;
            }

            HeroTalentList.DataSource = NewTalentData;

            // 设置大小
            int itemCount = HeroTalentList.DataSource.Count;
            var size = MainTalentShowObject.GetComponent<RectTransform>().sizeDelta;
            MainTalentShowObject.GetComponent<RectTransform>().sizeDelta = new Vector2(size.x, itemCount * 95f + 5f);
        }

        // 天赋项点击事件
        public void OnHeroTalentItemClick(int nIndex, ListViewItem item)
        {
            if (nIndex < 0 || nIndex >= HeroTalentList.DataSource.Count)
            {
                return;
            }

            UListHeroTalentData itemData = HeroTalentList.DataSource[nIndex];

            // 激活天赋
            if (ActiveTalent(itemData))
            {
                ShowMainTalent(false);
            }
        }

        // 天赋不可激活
        private void onTalentCantActive()
        {
            int nCurLv = EntityUtil.getMainHeroLv();
            string tips = "";
            if (nCurLv < 18)
            {
                tips = ULocalizationService.Instance.Get("UIView", "TalentView", "NextLvActiveTalentTips");
                int nNextLv = DataCenter.LogicDataCenter.heroTalentDataManager.getNextCanActiveLv(nCurLv);
                tips = String.Format(tips, nNextLv);
            }
            else
            {
                tips = ULocalizationService.Instance.Get("UIView", "TalentView", "AllTalentActivedTips");
            }

            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, tips);

            // 关闭天赋窗口
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE, null);
        }

        // 激活天赋
        private bool ActiveTalent(UListHeroTalentData itemData)
        {
            if (itemData == null)
                return false;

            if (!LogicDataCenter.heroTalentDataManager.isCanActiveTalent(itemData.nTalentId, itemData.nTalentLevel))
            {
                onTalentCantActive();
                return false;
            }

            if (itemData.nTalentTermId > 0)
            {
                // 判断前置天赋是否已经激活
                bool bTermActive = LogicDataCenter.heroTalentDataManager.isActivedThisTalent(itemData.nTalentTermId);

                if (!bTermActive)
                    return false;
            }

            int nTalentId = itemData.nTalentId;

            // 判断该等级是否激活次数用完了
            if (LogicDataCenter.heroTalentDataManager.isTalentActivedThisLv(itemData.nTalentLevel))
            {
                onTalentCantActive();
                return false;
            }

            // 激活天赋
            cmd_Entity_Hero_Talent data = new cmd_Entity_Hero_Talent();
            data.nHeroTalentId = nTalentId;
            EntityEventHelper.Instance.SendCommand<cmd_Entity_Hero_Talent>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_ACTIVE_TALENT, ref data);

            return true;
        }

        public void SetTalentOpenButtonColorChange(bool bChange)
        {
            if (bChange)
            {
                SetTalentUpdateCanUseEffect();
            }
            else
            {
                HideTalentUpdateCanUseEffect();
            }
        }

        public void SetTalentUpdateCanUseEffect()
        {
            if (m_OldEffectParam != null)
            {
                m_OldEffectParam.EffectGameObject.SetActive(true);
                return;
            }

            UEffectPrefabType effectType = UEffectPrefabType.UEPT_UHeroTalent_TalentOpenButtonEffect;
            // 看玩家是否是新手（角色等级小于3级）
            if (LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo().playerInfo.nLv < 3)
            {
                effectType = UEffectPrefabType.UEPT_UHeroTalent_TalentOpenButtonEffectForNewbie;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: effectType, _tfAttachParent: this.LearnTalentHintEffect.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParam = (UEffectPrefabParam)param;

            m_OldEffectParam.EffectGameObject.SetActive(true);
        }

        public void HideTalentUpdateCanUseEffect()
        {
            if (m_OldEffectParam == null)
            {
                return;
            }

            m_OldEffectParam.EffectGameObject.SetActive(false);
        }

        // 计算当前等级的下一个可激活天赋等级
        private int getNextActiveTalentLv(int nCurLv)
        {
            if (LogicDataCenter.heroTalentDataManager.LevelArray != null)
            {
                foreach (var lv in LogicDataCenter.heroTalentDataManager.LevelArray)
                {
                    if (nCurLv < lv)
                    {
                        return lv;
                    }
                }

                // 返回最后一个18级
                int nCount = LogicDataCenter.heroTalentDataManager.LevelArray.Length;
                if (nCount > 0)
                {
                    int nLv = LogicDataCenter.heroTalentDataManager.LevelArray[nCount - 1];
                    Debug.LogWarning(String.Format("getNextActiveTalentLv return {0} nCurLv={1}", nLv, nCurLv));
                    return nLv;
                }


            }

            Debug.LogWarning("getNextActiveTalentLv return 0,nCurLv=" + nCurLv);

            return 0;
        }

        public void onUpdateLevel(int nUpdatelv)
        {
            // 自动激活该等级以下的的天赋
            if (m_AutoActiveTalent != null && GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_Moba)
            {
                m_AutoActiveTalent.AutoActiveTalent(nUpdatelv);
            }

            // 有新的天赋可激活,则显示提示光效
            UpdateTalentBtnEffect();
        }

        public void onActivedTalent(int nTalentID)
        {
            if (CheckActiveTalentIdIsCorrect(nTalentID) == false)
            {
                return;
            }

            UListHeroTalentData itemData = getTalentData(nTalentID);
            if (itemData != null)
            {
                UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_ID_HEROTALENTVIEW_LEARNHEROTALENT, itemData.nTalentId, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_LEARNHEROTALENT, uiResult);
            }
        }

        // 获取天赋数据
        private UListHeroTalentData getTalentData(int nTalentID)
        {
            foreach (UListHeroTalentData itemData in HeroTalentData)
            {
                if (itemData.nTalentId == nTalentID)
                {
                    return itemData;
                }
            }

            return null;
        }

        // 检查天赋ID有效性
        private bool CheckActiveTalentIdIsCorrect(int nTalentId)
        {
            int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            if (nHeroId <= 0)
            {
                return false;
            }

            if (LogicDataCenter.heroTalentDataManager.dicTalentInfoData == null || !LogicDataCenter.heroTalentDataManager.dicTalentInfoData.ContainsKey(nHeroId))
            {
                return false;
            }

            List<HeroTalentDataManager.HeroTalentInfo> listInfo = LogicDataCenter.heroTalentDataManager.dicTalentInfoData[nHeroId];
            foreach (HeroTalentDataManager.HeroTalentInfo item in listInfo)
            {
                if (item.nTalentId == nTalentId)
                {
                    return true;
                }
            }

            return false;
        }

        public override void Destroy()
        {
            DestoryTalentUpdateCanUseEffect();
        }

        public void DestoryTalentUpdateCanUseEffect()
        {
            if (m_OldEffectParam == null)
            {
                return;
            }

            UEffectParamBase param = (UEffectParamBase)m_OldEffectParam;
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            m_OldEffectParam = null;
        }

        // 是否还有天赋可以激活
        private bool isHasTalentCanActive()
        {
            if (LogicDataCenter.heroTalentDataManager.LevelArray == null)
                return false;

            int nCurLv = EntityUtil.getMainHeroLv();
            if (nCurLv == 0)
            {
                Debug.LogWarning("isHasTalentCanActive,EntityUtil.getMainHeroLv() == 0");
                return false;
            }

            for (int j = 0; j < LogicDataCenter.heroTalentDataManager.LevelArray.Length; ++j)
            {
                int nLv = LogicDataCenter.heroTalentDataManager.LevelArray[j];
                if (nLv <= nCurLv)
                {
                    if (!LogicDataCenter.heroTalentDataManager.isTalentActivedThisLv(nLv))
                    {
                        return true;
                    }

                }
            }

            return false;
        }

        #region 新手引导部分
        public void AddGuideWidget(UGuideWidgetMsgData _msgData)
        {

            for (int i = 0; i < HeroTalentData.Count; ++i)
            {
                if (HeroTalentData[i].nTalentLevel == (int)_msgData.oUserData)
                {
                    HeroTalentData[i].bUseGuideWidget = true;
                    HeroTalentData[i].nGuideID = _msgData.nGuideID;
                    HeroTalentData[i].nGuideStepID = _msgData.nGuideStepID;
                    HeroTalentData[i].nGuideEffectID = _msgData.nGuideEffectID;
                    break;
                }
            }
        }

        public void RemoveGuideWidget(UGuideWidgetMsgData _msgData)
        {
            for (int i = 0; i < HeroTalentData.Count; ++i)
            {
                if (HeroTalentData[i].nTalentLevel == (int)_msgData.oUserData)
                {
                    HeroTalentData[i].bUseGuideWidget = false;
                    HeroTalentData[i].nGuideID = _msgData.nGuideID;
                    HeroTalentData[i].nGuideStepID = _msgData.nGuideStepID;
                    break;
                }
            }
        }
        #endregion

        public class CAutoActiveTalent
        {
            WarHeroTalentView m_WarHeroTalentView;

            public CAutoActiveTalent(WarHeroTalentView view)
            {
                m_WarHeroTalentView = view;
            }

            // 自动激活该等级的天赋
            public void AutoActiveTalent(int nCurLv)
            {
                int nNewLv = getCurCanActiveTalentLevel(nCurLv);

                for (int j = 0; j < LogicDataCenter.heroTalentDataManager.LevelArray.Length; ++j)
                {
                    int nLv = LogicDataCenter.heroTalentDataManager.LevelArray[j];
                    if (nLv < nNewLv)
                    {
                        // 该等级已激活，则跳过
                        if (LogicDataCenter.heroTalentDataManager.isTalentActivedThisLv(nLv))
                            continue;

                        // 统计推荐激活天赋和未激活天赋
                        List<UListHeroTalentData> unRecommandTalents = new List<UListHeroTalentData>();
                        List<UListHeroTalentData> recommandTalents = new List<UListHeroTalentData>();
                        foreach (UListHeroTalentData data in m_WarHeroTalentView.HeroTalentData)
                        {
                            if (data.nTalentLevel == nLv)
                            {
                                if (data.bRecommand)
                                    recommandTalents.Add(data);
                                else
                                    unRecommandTalents.Add(data);
                            }
                        }

                        // 激活nLv等级的天赋(根据推荐天赋)
                        bool isAutoActiveOk = false;
                        foreach (var data in recommandTalents)
                        {
                            isAutoActiveOk = m_WarHeroTalentView.ActiveTalent(data);
                            if (isAutoActiveOk)
                                break;
                        }

                        // 推荐天赋未激活成功,则选择一个非推荐天赋来激活
                        if (!isAutoActiveOk)
                        {
                            foreach (var data in unRecommandTalents)
                            {
                                isAutoActiveOk = m_WarHeroTalentView.ActiveTalent(data);
                                if (isAutoActiveOk)
                                    break;
                            }
                        }

                        if (!isAutoActiveOk)
                        {
                            Debug.LogWarning(String.Format("自动激活天赋失败,nCurLv={0}, nLv={1}, nHeroID={2}", nCurLv, nLv, EntityUtil.getMainHeroHeroID()));
                        }
                    }
                }
            }

            // 计算当前可激活天赋的等级,比如现在7级,则结果为6
            private int getCurCanActiveTalentLevel(int nCurLv)
            {
                if (LogicDataCenter.heroTalentDataManager.LevelArray != null)
                {
                    int nLvLen = LogicDataCenter.heroTalentDataManager.LevelArray.Length;
                    for (int i = nLvLen - 1; i >= 0; --i)
                    {
                        int nLv = LogicDataCenter.heroTalentDataManager.LevelArray[i];
                        if (nLv <= nCurLv)
                        {
                            return nLv;
                        }
                    }
                }

                Debug.LogWarning("getCurActiveTalentLevel,return 0");

                return 0;
            }
        }
    }
}
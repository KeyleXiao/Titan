using ASpeedGame.Data.GameGoodsLabelDesConfig;
using ASpeedGame.Data.GameMobaSchemes;
using Data.PersonModel;
using GameLogic;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI;
using USpeedUI.GoodsDisposition;

namespace DataCenter
{
    public class GoodsDispositionDataManager : Singleton<GoodsDispositionDataManager>, IModelData, ITimerHanlder
    {
        private enum ETimerHandlerID
        {
            HeroPortraitList,
            GoodsDispositionList,
        }

        protected Hashtable m_SchemeGoodsLabelDescirptionTable = null;
        /// <summary>
        /// 物品类型标签列表
        /// </summary>
        public Hashtable SchemeGoodsLabelDescirptionTable
        {
            get { return m_SchemeGoodsLabelDescirptionTable; }
        }

        protected ObservableList<SSchemeMobaGoods> m_SchemeMobaGoodsDataSource = null;
        /// <summary>
        /// 观察类型的物品总列表
        /// </summary>
        public ObservableList<SSchemeMobaGoods> SchemeMobaGoodsDataSource
        {
            get { return m_SchemeMobaGoodsDataSource; }
        }
        
        protected ActorHeroInfo m_nPlayerHeroInfo = null;

        protected ObservableList<UGD_HeroHeadPortraitItem> m_currentHeroPortraitItemList = null;
        /// <summary>
        /// 当前显示的英雄头像列表数据
        /// </summary>
        public ObservableList<UGD_HeroHeadPortraitItem> CurrentHeroPortraitItemList
        {
            get { return m_currentHeroPortraitItemList; }
        }

        protected List<string> m_allHeroTypeList = null;
        /// <summary>
        /// 所有英雄类型
        /// </summary>
        public List<string> AllHeroTypeList
        {
            get { return m_allHeroTypeList; }
        }

        protected ObservableList<UGD_GoodsDispositionItem> m_GoodsDispositionList = null;
        /// <summary>
        /// 配置物品方案列表
        /// </summary>
        public ObservableList<UGD_GoodsDispositionItem> GoodsDispositionList
        {
            get { return m_GoodsDispositionList; }
        }

        /// <summary>
        /// 当前选择配置物品方案
        /// </summary>
        public UGD_GoodsDispositionItem CurrentGoodsDisposition
        {
            get
            {
                int index = GetGoodsDispositionIndex(CurrentGoodsDispositionID);

                if (index == -1)
                    return null;

                return GoodsDispositionList[index];
            }
        }
        protected int m_nCurrentGoodsDispositionID = 0;
        /// <summary>
        /// 当前选择配置物品方案ID 
        /// </summary>
        public int CurrentGoodsDispositionID
        {
            get { return m_nCurrentGoodsDispositionID; }

            set
            {
                if (!GoodsDispositionDataManager.Instance.IsCurrentNeedToSave(true,
                    () =>
                    {//玩家保存
                        UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION, null);
                        SetCurrentGoodsDispositionID(value);
                        return true;
                    },
                    () =>
                    {
                        SetCurrentGoodsDispositionID(value);

                        return true;
                    }))
                {
                    //不需要保存
                    SetCurrentGoodsDispositionID(value);
                }
            }
        }

        protected bool m_bIsOnlyShowBuyHero = true;
        /// <summary>
        /// 是否显示已购买英雄
        /// </summary>
        public bool IsOnlyShowBuyHero
        {
            get { return m_bIsOnlyShowBuyHero; }

            set
            {
                m_bIsOnlyShowBuyHero = value;
                SetPlayerCurrentHeroPortraitItemList();
            }
        }

        protected int m_nCurrentSelectHeroType = -1;
        /// <summary>
        /// 当前选择的英雄类型ID
        /// </summary>
        public int CurrentSelectHeroType
        {
            get { return m_nCurrentSelectHeroType; }
            set
            {
                m_nCurrentSelectHeroType = value;
                SetPlayerCurrentHeroPortraitItemList();
            }
        }

        protected int m_nCurrentSelectHero = 0;
        public int CurrentSelectHero
        {
            get { return m_nCurrentSelectHero; }
            set
            {
                if (!GoodsDispositionDataManager.Instance.IsCurrentNeedToSave(true,
                    () =>
                    {//玩家保存
                        UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION, null);

                        SetPlayerCurrentHero(value);
                        return true;
                    },
                    () =>
                    {
                        SetPlayerCurrentHero(value);
                        return true;
                    }))
                {
                    //不需要保存
                    SetPlayerCurrentHero(value);
                }
            }
        }

        protected int m_nCurrentSelectGoodsID = 0;
        /// <summary>
        /// 当前选择的物品ID
        /// </summary>
        public int CurrentSelectGoodsID
        {
            get { return m_nCurrentSelectGoodsID; }
        }


        public void Init()
        {

            InitMobaGoodsFromSchemes();

            InitMobaGoodsLabelDescirption();

            InitAllHeroPortraitItem();

            InitAllHeroType();

            InitGoodsDispositionList();
        }

        public void Clear()
        {
            if (m_SchemeMobaGoodsDataSource != null)
            {
                while (m_SchemeMobaGoodsDataSource.Count > 0)
                {
                    if (m_SchemeMobaGoodsDataSource[0] != null && m_SchemeMobaGoodsDataSource.Contains(m_SchemeMobaGoodsDataSource[0]))
                    {
                        m_SchemeMobaGoodsDataSource.RemoveAt(0);
                    }
                }
                m_SchemeMobaGoodsDataSource = null;
            }

            if (m_SchemeGoodsLabelDescirptionTable != null)
            {
                m_SchemeGoodsLabelDescirptionTable.Clear();
                m_SchemeGoodsLabelDescirptionTable = null;
            }

            if (m_currentHeroPortraitItemList != null)
            {
                m_currentHeroPortraitItemList.Clear();
                m_currentHeroPortraitItemList = null;
            }
        }

        private void InitMobaGoodsFromSchemes()
        {
            Dictionary<int, SSchemeMobaGoods> m_SchemeGoodsTable = new Dictionary<int, SSchemeMobaGoods>();

            m_SchemeMobaGoodsDataSource = new ObservableList<SSchemeMobaGoods>();

            GoodsSchemesDataManager.Instance.GetGoodsSschemdTable(8, ref m_SchemeGoodsTable);

            foreach (var keyValuepair in m_SchemeGoodsTable)
            {
                m_SchemeMobaGoodsDataSource.Add(keyValuepair.Value);
            }
        }

        private void InitMobaGoodsLabelDescirption()
        {
            m_SchemeGoodsLabelDescirptionTable = new Hashtable();
            GameGoodsLabelDesConfig.Instance.GetLabelDescirptionTable(ref m_SchemeGoodsLabelDescirptionTable);
        }

        private void InitAllHeroPortraitItem()
        {
            m_currentHeroPortraitItemList = new ObservableList<UGD_HeroHeadPortraitItem>();
            //{
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 1 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 2 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 3 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 4 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 5 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 6 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 7 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 8 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 9 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 10 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 11 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 12 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 13 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 14 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 15 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 16 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 17 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 18 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 19 } },
            //    { new UGD_HeroHeadPortraitItem() { HeroHeadPortraitID = 20 } },
            //};
        }

        private void InitAllHeroType()
        {
            //-1,0,1,2,3,4
            //以后想办法搞成可配的
            m_allHeroTypeList = new List<string>() { "全部", "坦克", "战士", "法师", "射手", "辅助" };

        }

        private void InitGoodsDispositionList()
        {
            m_GoodsDispositionList = new ObservableList<UGD_GoodsDispositionItem>();

            //m_GoodsDispositionList = new ObservableList<UGD_GoodsDispositionItem>()
            //{
            //    { new UGD_GoodsDispositionItem() { Name="方案1",Author="作者1",IsCurrent=true,
            //        EerlyGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[1], m_SchemeMobaGoodsDataSource[5], m_SchemeMobaGoodsDataSource [10]},
            //        CoreGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[2], m_SchemeMobaGoodsDataSource[3], m_SchemeMobaGoodsDataSource[11] }
            //    } },

            //    { new UGD_GoodsDispositionItem() { Name="方案2",Author="作者2",
            //        EerlyGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[3], m_SchemeMobaGoodsDataSource[6], m_SchemeMobaGoodsDataSource [12]},
            //        CoreGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[4], m_SchemeMobaGoodsDataSource[4], m_SchemeMobaGoodsDataSource[13] }
            //    } },

            //   { new UGD_GoodsDispositionItem() { Name="方案3",Author="作者3",
            //        EerlyGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[5], m_SchemeMobaGoodsDataSource[7], m_SchemeMobaGoodsDataSource [14]},
            //        CoreGoodsList = new List<SSchemeMobaGoods>() { m_SchemeMobaGoodsDataSource[6], m_SchemeMobaGoodsDataSource[5], m_SchemeMobaGoodsDataSource[15] }
            //    } },
            //};

        }

        #region 英雄相关
        /// <summary>
        /// 每次打开界面都重新获取一次玩家的所有英雄信息
        /// </summary>
        public void SetPlayerCurrentHeroPortraitItemList()
        {
            CurrentHeroPortraitItemList.Clear();

            foreach (var heroInfo in m_nPlayerHeroInfo.dicEntityHeroInfo)
            {
                if (heroInfo.Value.bIsBuy <= 0)
                {
                    continue;
                }
                if (!IsUsedHeroType(heroInfo.Value.nHeroID))
                    continue;

                UGD_HeroHeadPortraitItem item = new UGD_HeroHeadPortraitItem();
                item.From(heroInfo.Value);
                CurrentHeroPortraitItemList.Add(item);
            }

            if(!IsOnlyShowBuyHero)
            {
                foreach (var heroInfo in m_nPlayerHeroInfo.dicEntityNoHaveHeroInfo)
                {
                    if (!IsUsedHeroType(heroInfo.Value.nHeroID))
                        continue;

                    UGD_HeroHeadPortraitItem item = new UGD_HeroHeadPortraitItem();
                    item.From(heroInfo.Value);
                    CurrentHeroPortraitItemList.Add(item);
                }
            }


            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROPORTRAITITEMLIST, null);
        }

        public void StartGetPlayerHeroPortraitItemTimer()
        {
            TimerManager.SetTimer(this, (int)ETimerHandlerID.HeroPortraitList, 0.5f, 20);
        }
        /// <summary>
        /// 设置当前英雄列表数据
        /// </summary>
        public void SetPlayerHeroPortraitItemList()
        {
            ActorHeroInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();

            //数据没下来，等待数据
            if (heroInfo.dicEntityHeroInfo.Count <= 0 || heroInfo.dicEntityNoHaveHeroInfo.Count <= 0 )
            {

                return;
            }

            m_nPlayerHeroInfo = heroInfo;

            SetPlayerCurrentHeroPortraitItemList();
            if (CurrentSelectHero <= 0)
            {
                CurrentSelectHero = CurrentHeroPortraitItemList[0].HeroHeadPortraitID;
            }

            TimerManager.KillTimer(this, (int)ETimerHandlerID.HeroPortraitList);
        }

        /// <summary>
        /// 设置当前英雄
        /// </summary>
        protected void SetPlayerCurrentHero(int _nNewValue)
        {
            ClearCurrentInfo();
            m_nCurrentSelectHero = _nNewValue;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO, null);
            //请求当前英雄的配置表数据
            TimerManager.SetTimer(this, (int)ETimerHandlerID.GoodsDispositionList, 0.5f, 20);
            //SetHeroGoodsDispositionList();
        }

        /// <summary>
        /// 判断英雄类型标签
        /// </summary>
        /// <param name="_nHeroID"></param>
        /// <returns></returns>
        private bool IsUsedHeroType(int _nHeroID)
        {
            bool result = false;
            if (CurrentSelectHeroType != -1)
            {
                SSchemePersonModelData data = null;
                if (PersonModelConfig.instance.GetData_Out(_nHeroID, out data))
                {
                    result = data.nHeroType == CurrentSelectHeroType;
                }
            }
            else
            {
                result = true;
            }


            return result;
        }
        #endregion

        #region 物品配置相关
        /// <summary>
        /// 每次切换英雄时，获取当前英雄的所有物品配置列表
        /// </summary>
        protected void SetHeroGoodsDispositionList()
        {
            if (CurrentSelectHero <= 0)
                return;

            List<cmd_war_equipscheme> schemeList;
            if (!EquipSchemeDataManager.Instance.getDiyEquipSchemeList(CurrentSelectHero, out schemeList))
            {

                return;
            }

            GoodsDispositionList.Clear();

            int defaultDispositionID = 0;

            {
                for (int i = 0; i < schemeList.Count; ++i)
                {
                    cmd_war_equipscheme scheme = schemeList[i];
                    UGD_GoodsDispositionItem item = new UGD_GoodsDispositionItem();
                    item.From(scheme);

                    if (i == 0)
                    {
                        defaultDispositionID = item.DispositionID;
                        item.IsCurrent = true;
                    }


                    GoodsDispositionList.Add(item);
                }
            }

            CurrentGoodsDispositionID = defaultDispositionID;
            TimerManager.KillTimer(this, (int)ETimerHandlerID.GoodsDispositionList);
            //UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONLIST, null);
        }
        /// <summary>
        /// 重置当前英雄的当前物品配置
        /// </summary>
        public void RestoreCurrent()
        {
            cmd_war_equipscheme scheme;
            if (EquipSchemeDataManager.Instance.getEquipScheme(CurrentSelectHero, (byte)CurrentGoodsDispositionID, out scheme))
            {
                CurrentGoodsDisposition.From(scheme, true);
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTDISPOSITION, null);
            }
            else
            {
                Trace.LogError("当前物品配置重置失败。");
            }
        }
        /// <summary>
        /// 判断当前英雄的当前物品配置是否需要保存
        /// </summary>
        /// <returns></returns>
        public bool IsCurrentNeedToSave()
        {
            if (CurrentSelectHero <= 0 || CurrentGoodsDispositionID <= 0)
                return false;

            cmd_war_equipscheme scheme;
            //与原始数据比较
            if (EquipSchemeDataManager.Instance.getEquipScheme(CurrentSelectHero, (byte)CurrentGoodsDispositionID, out scheme))
            {
                if (!string.Equals(CurrentGoodsDisposition.Name, scheme.szSchemeName))
                {
                    return true;
                }
                if (!string.Equals(CurrentGoodsDisposition.Author, scheme.szAuthorName))
                    return true;
                if(scheme.nEquipIDList != null)
                {
                    for (int i = 0; i < CurrentGoodsDisposition.GoodsList.Length; ++i)
                    {
                        if (CurrentGoodsDisposition.GoodsList[i] != scheme.nEquipIDList[i])
                            return true;
                    }
                }
            }
            else
            {                    //与空数据比较
                if (CurrentGoodsDisposition != null)
                {
                    if (!string.IsNullOrEmpty(CurrentGoodsDisposition.Name))
                        return true;
                    if (!string.IsNullOrEmpty(CurrentGoodsDisposition.Author))
                        return true;
                    for (int i = 0; i < CurrentGoodsDisposition.GoodsList.Length; ++i)
                    {
                        if (CurrentGoodsDisposition.GoodsList[i] != 0)
                            return true;
                    }
                }
            }

            return false;
        }
        public bool IsCurrentNeedToSave(bool _bShowDialog, Func<bool> _ConfirnAction, Func<bool> _CancelAction)
        {
            bool result = IsCurrentNeedToSave();

            if (result && _bShowDialog)
            {
                USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                    title: ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "TitleAsk"),
                    message: ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "DispositionSave"),
                    buttons: new UIWidgets.DialogActions() {
                    {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), _ConfirnAction},
                    {ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), _CancelAction}}
                    );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }

            return result;
        }
        /// <summary>
        /// 获取物品配置在列表中的序号
        /// </summary>
        /// <param name="_nDispositionID">物品配置ID</param>
        /// <returns>列表序号</returns>
        protected int GetGoodsDispositionIndex(int _nDispositionID)
        {
            if (GoodsDispositionList != null && GoodsDispositionList.Count > 0)
            {
                for (int i = 0; i < GoodsDispositionList.Count; ++i)
                {
                    if (GoodsDispositionList[i].DispositionID == _nDispositionID)
                        return i;
                }
            }
            return -1;
        }
        /// <summary>
        /// 给当前英雄添加新的物品配置
        /// </summary>
        public void AddHeroGoodsDisposition()
        {
            cmd_war_equipscheme newScheme = new cmd_war_equipscheme();
            //向上申请新配置
            if (EquipSchemeDataManager.Instance.addEquipScheme(CurrentSelectHero, ref newScheme))
            {
                UGD_GoodsDispositionItem item = new UGD_GoodsDispositionItem();
                item.From(newScheme, true);
                GoodsDispositionList.Add(item);
                CurrentGoodsDispositionID = newScheme.bySchemeID;
            }
            else
            {
                //USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                //    title: ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "TitleAsk"),
                //    message: ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "DispositionFilled"),
                //    buttons: new UIWidgets.DialogActions() {
                //      {ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), null},}
                //    );
                //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
                UGD_DispositionFilledMsgData msgData = new UGD_DispositionFilledMsgData();
                msgData.HeroID = CurrentSelectHero;
                msgData.onConfirmCallback = AddHeroGoodsDisposition;
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_OPEN_DISPOSITIONFILLED, msgData);
            }
        }

        public bool AddHeroGoodsDispositionFromShare(int _nHeroID, cmd_war_equipscheme _nNewScheme)
        {
            if (EquipSchemeDataManager.Instance.addEquipScheme(_nHeroID, ref _nNewScheme))
            {
                if (_nHeroID == CurrentSelectHero)
                {
                    UGD_GoodsDispositionItem item = new UGD_GoodsDispositionItem();
                    item.From(_nNewScheme, true);
                    GoodsDispositionList.Add(item);
                }                
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 拷贝操作只用于当前英雄的当前物品配置
        /// </summary>
        /// <param name="_nHeroID"></param>
        /// <param name="_remainingHeroID"></param>
        /// <returns></returns>
        public bool AddHeroGoodsDispositionFromCopy(int _nHeroID,Action _fillConfirmCallback)
        {
            //当前英雄配置设为目标英雄的
            cmd_war_equipscheme HeroScheme = CurrentGoodsDisposition.To();
            HeroScheme.nHeroID = _nHeroID;

            if (AddHeroGoodsDispositionFromShare(_nHeroID, HeroScheme))
                return true;
            else
            {
                UGD_DispositionFilledMsgData msgData = new UGD_DispositionFilledMsgData();
                msgData.HeroID = _nHeroID;
                msgData.onConfirmCallback = _fillConfirmCallback;
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_OPEN_DISPOSITIONFILLED, msgData);
                return false;
            }
        }
        /// <summary>
        /// 移除当前英雄的指定物品配置
        /// </summary>
        /// <param name="_nDispositionID"></param>
        public void RemoveHeroGoodsDisposition(int _nHeroID,int _nDispositionID)
        {
            _nHeroID = _nHeroID <= 0 ? CurrentSelectHero : _nHeroID;

            _removeHeroGoodsDisposition(_nHeroID, _nDispositionID);

            //通知显示更新
            if(_nHeroID == CurrentSelectHero)
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONLIST, null);
            else//非当前英雄的话目前也就只有配置列表已满界面(可以显示其他英雄的物品配置)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONFILLEDLIST, null);
            }
        }

        protected void _removeHeroGoodsDisposition(int _nHeroID,int _nDispositionID)
        {
            //通知逻辑去移除
            EquipSchemeDataManager.Instance.delEquipScheme(_nHeroID, (byte)_nDispositionID);
            //移除当前英雄的
            if (_nHeroID == CurrentSelectHero)
            {
                //移除当前英雄的
                int index = GetGoodsDispositionIndex(_nDispositionID);

                if (index != -1)
                    GoodsDispositionList.RemoveAt(index);
                //如果移除的是当前的物品配置的话
                if (_nDispositionID == CurrentGoodsDispositionID)
                {
                    if (GoodsDispositionList.Count > 0)
                    {
                        CurrentGoodsDispositionID = GoodsDispositionList[0].DispositionID;
                    }
                    else
                    {
                        //走申请
                        AddHeroGoodsDisposition();
                    }
                }
            }
        }

        protected void SetCurrentGoodsDispositionID(int _nNewValue)
        {
            if (CurrentGoodsDisposition != null)
                CurrentGoodsDisposition.IsCurrent = false;
            m_nCurrentGoodsDispositionID = _nNewValue;
            if (CurrentGoodsDisposition != null)
                CurrentGoodsDisposition.IsCurrent = true;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTDISPOSITION, null);

        }

        /// <summary>
        /// 分享装备配置ID到聊天框
        /// </summary>
        /// <param name="_nShareEquipID">装备配置ID，-1为当前配置ID</param>
        public void ShareEquipScheme(int _nShareEquipID = -1)
        {
            int index = GetGoodsDispositionIndex(_nShareEquipID == -1 ? CurrentGoodsDispositionID : _nShareEquipID);

            UGD_GoodsDispositionItem tmpShareItem = GoodsDispositionList[index];

            EquipSchemeDataManager.Instance.shareEquipScheme(CurrentSelectHero, tmpShareItem.To());
        }
        #endregion

        /// <summary>
        /// 设置当前选择的物品ID
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <param name="_bSendCallback">是否要分发事件</param>
        public void SetCurrentSelectGoodsID(int _nGoodsID, bool _bSendCallback = true)
        {
            if (m_nCurrentSelectGoodsID != _nGoodsID)
            {
                m_nCurrentSelectGoodsID = _nGoodsID;
                if (_bSendCallback)
                    UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTSELECTEDGOODS, null);
            }
        }

        public void ClearCurrentInfo()
        {
            m_nCurrentSelectHero = 0;
            m_nCurrentGoodsDispositionID = 0;
        }


        public void OnTimer(int nTimerID)
        {
            ETimerHandlerID handlerID = (ETimerHandlerID)nTimerID;
            switch (handlerID)
            {
                case ETimerHandlerID.HeroPortraitList:
                    {
                        SetPlayerHeroPortraitItemList();
                    }
                    break;
                case ETimerHandlerID.GoodsDispositionList:
                    {
                        SetHeroGoodsDispositionList();
                    }
                    break;
            }

        }
    }
}
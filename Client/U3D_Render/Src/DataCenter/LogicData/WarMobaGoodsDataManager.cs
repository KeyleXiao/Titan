using ASpeedGame.Data.GameMobaSchemes;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using USpeedUI;
using USpeedUI.WarGoodsStore;
using War;

namespace DataCenter
{
    public class WarMobaGoodsDataManager : Singleton<WarMobaGoodsDataManager>, IModelData, ITimerHanlder
    {
        public delegate void InitHeroDispositionListHanlder();
        public static event InitHeroDispositionListHanlder InitHeroDispositionListEvent;


        private enum ETimerHandlerID
        {
            GoodsDispositionList,
        }
        //当前模式可使用的所有配置物品
        private Dictionary<int, SSchemeMobaGoods> m_dicWarMobaGoodsTable;
        // 战场内所有玩家与其购买物品数据表
        private Dictionary<ulong, List<cmd_war_persongoodsinfo>> m_dicPlayersGoodsTable;
        // 英雄当前物品配置
        protected int[] m_nHeroCurrentGoodsDisposition = null;
        public int[] HeroCurrentGoodsDisposition
        {
            get
            {
                return m_nHeroCurrentGoodsDisposition;
            }

            set
            {
                m_nHeroCurrentGoodsDisposition = value;
            }
        }

        protected int[] m_nHeroDefaultGoodsDisposition = null;
        protected Dictionary<int, List<int>> m_dicHeroDefaultGoodsDisposition = null;

        protected List<UWarGoodsStoreDispositionItem> m_HeroDispositionList = null;
        public List<UWarGoodsStoreDispositionItem> HeroDispositionList
        {
            get
            {
                return m_HeroDispositionList;
            }
        }

        public int[] HeroDefaultGoodsDisposition
        {
            get
            {
                List<int> tmpList = new List<int>();
                if (m_dicHeroDefaultGoodsDisposition == null)
                    return new int[12];
                foreach (var value in m_dicHeroDefaultGoodsDisposition.Values)
                {
                    tmpList.AddRange(value);
                }
                return tmpList.ToArray();
            }
        }

        /// <summary>
        /// 获取物品配置替换的推荐类型，
        /// </summary>
        public int[] GoodsDispositionRecmomendType
        {
            get
            {
                return m_dicHeroDefaultGoodsDisposition.Keys.ToArray();
            }
        }

        private uint m_nMainHeroPDBID = 0;
        private uint MainHeroPDBID
        {
            get
            {
                if (m_nMainHeroPDBID == 0)
                {
                    Trace.LogError("WarMobaGoodsDataManager 取MainHeroPDBID失败.");
                }
                return (uint)m_nMainHeroPDBID;
            }
        }

        /// <summary>
        /// 判断当前是否以获取本地玩家的PDBID
        /// </summary>
        public bool IsMainHeroPDBIDValid
        {
            get { return m_nMainHeroPDBID != 0; }
        }

        /// <summary>
        ///判断当前是否开启物品过滤
        /// </summary>
        private bool m_enableFilterGoods;
        public bool EnableGoodsFilter
        {
            get{ return m_enableFilterGoods; }
        }


        public void Init()
        {

        }
        public void Load(int _nWarID, int _nMatchTypeId)
        {
            m_dicWarMobaGoodsTable = new Dictionary<int, SSchemeMobaGoods>();
            m_SchemeMobaGoodsDataSource = new ObservableList<UWarGoodsStoreItem>();
            m_dicPlayersGoodsTable = new Dictionary<ulong, List<cmd_war_persongoodsinfo>>();
            m_dicHeroDefaultGoodsDisposition = new Dictionary<int, List<int>>();
            m_HeroDispositionList = new List<UWarGoodsStoreDispositionItem>();

            m_enableFilterGoods = GameLogicAPI.getGameModeByMatchType(_nMatchTypeId) == Game_ManagedDef.EGameMode.EGM_MCAMP;

            LogicDataCenter.goodsSchemesDataManager.GetGoodsSschemdTable(_nWarID, ref m_dicWarMobaGoodsTable);

            EntityFactory.CreateMainHeroEntityEvent -= EntityFactory_CreateMainHeroEntityEvent;
            EntityFactory.CreateMainHeroEntityEvent += EntityFactory_CreateMainHeroEntityEvent;


            InitWarGoodsStoreMobaGoodsDataSource();
        }
        private void EntityFactory_CreateMainHeroEntityEvent(CreateMainHeroEntityEventArgs e)
        {
            TimerManager.SetTimer(this, (int)ETimerHandlerID.GoodsDispositionList, 0.5f, 10);
            //SetDispositionListViewData();
        }
        public void UnLoad()
        {
            m_nMainHeroPDBID = 0;
            m_enableFilterGoods = false;

            EntityFactory.CreateMainHeroEntityEvent -= EntityFactory_CreateMainHeroEntityEvent;
            TimerManager.KillTimer(this, (int)ETimerHandlerID.GoodsDispositionList);

            if (m_dicWarMobaGoodsTable != null)
            {
                m_dicWarMobaGoodsTable.Clear();
                m_dicWarMobaGoodsTable = null;
            }
            if (m_SchemeMobaGoodsDataSource != null)
            {
                while (m_SchemeMobaGoodsDataSource.Count > 0)
                {
                    RemoveGoodsItemDataSource(m_SchemeMobaGoodsDataSource[0]);
                }
                m_SchemeMobaGoodsDataSource = null;
            }
            if (m_dicPlayersGoodsTable != null)
            {
                foreach (var player in m_dicPlayersGoodsTable)
                {
                    if (player.Value != null)
                        player.Value.Clear();
                }
                m_dicPlayersGoodsTable.Clear();
                m_dicPlayersGoodsTable = null;
            }

            if (m_dicHeroDefaultGoodsDisposition != null)
            {
                m_dicHeroDefaultGoodsDisposition.Clear();
                m_dicHeroDefaultGoodsDisposition = null;
            }

            if (m_HeroDispositionList != null)
            {
                m_HeroDispositionList.Clear();
                m_HeroDispositionList = null;
            }
        }

        protected void SetDispositionListViewData()
        {
            List<cmd_war_equipscheme> schemeList;

            int m_nMainHeroID = EntityFactory.MainHeroView == null ? -1 : EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            if (EnableGoodsFilter || !EquipSchemeDataManager.Instance.getAllEquipSchemeList(m_nMainHeroID, out schemeList))
            {

                return;
            }

            UWarGoodsStoreDispositionItem newItem = new UWarGoodsStoreDispositionItem();
            cmd_war_equipscheme oftenEquipScheme;
            //策划需要的新需求，之前提前传里。自动记录出过的装备（前期装.核心装），战场结束时会给出过的装备加权重分，然后把权重最高的记录为常用装备，下一场比赛把记录的常用装替换到系统推荐装的前期装和核心装上
            if (EquipSchemeDataManager.Instance.getEquipScheme(m_nMainHeroID, (byte)EWEquipSchemeType.EWEST_OftenUse, out oftenEquipScheme))
            {
                newItem.From(oftenEquipScheme);
                HeroCurrentGoodsDisposition = oftenEquipScheme.nEquipIDList;
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_CHANGEGOODSDISPOSITION, null);
            }
            else
            {
                newItem.DispositionName = "系统配置";
                newItem.DispositionGoods = WarMobaGoodsDataManager.Instance.HeroDefaultGoodsDisposition;
                newItem.DispositionID = 0;
            }

            m_HeroDispositionList.Add(newItem);
            //丢个默认的过去显示
            UWGS_SelectedGoodsDispositionMsgData msgData = new UWGS_SelectedGoodsDispositionMsgData();
            msgData.SelectedDisposition = newItem;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SELECTEDRECOMMENDDISPOSITION, msgData);



            for (int i = 0; i < schemeList.Count; ++i)
            {
                //已经取了常用配置，这里不再取
                if (schemeList[i].bySchemeID == oftenEquipScheme.bySchemeID)
                    continue;

                newItem = new UWarGoodsStoreDispositionItem();
                newItem.From(schemeList[i]);
                m_HeroDispositionList.Add(newItem);
            }
            //这里改成回调
            if (InitHeroDispositionListEvent != null)
            {
                InitHeroDispositionListEvent.Invoke();
            }
            //DispositionListView.DataSource = tmpItemList;
            //DispositionListView.OnSelect.AddListener(OnDispositionSelect);

            TimerManager.KillTimer(this, (int)ETimerHandlerID.GoodsDispositionList);
        }


        private bool IsHasGoods(int _goodsID)
        {
            bool res = false;
            if (m_dicWarMobaGoodsTable != null && m_dicWarMobaGoodsTable.ContainsKey(_goodsID))
            {
                res = true;
            }
            return res;
        }
        /// <summary>
        /// 拷贝一份新的，如果 “会改变里面值” 的话尽量用这方法
        /// </summary>
        /// <param name="_goodsID"></param>
        /// <param name="_goods"></param>
        public bool GetMobaGoods_Ref(int _goodsID, ref SSchemeMobaGoods _goods)
        {
            if (!IsHasGoods(_goodsID))
            {
                //Trace.LogWarning("没找到ID为:" + _goodsID + "的物品");
                _goods = null;
                return false;
            }
            m_dicWarMobaGoodsTable[_goodsID].CopyTo(ref _goods);
            return true;
        }
        /// <summary>
        /// 取引用，如果 “不会改变里面的值” 的话尽量用这个方法
        /// </summary>
        /// <param name="_goodsID"></param>
        /// <param name="_goods"></param>
        public bool GetMobaGoods_Out(int _goodsID, out SSchemeMobaGoods _goods)
        {
            if (!IsHasGoods(_goodsID))
            {
                //Trace.LogWarning("没找到ID为:" + _goodsID + "的物品");
                _goods = null;
                return false;
            }
            _goods = m_dicWarMobaGoodsTable[_goodsID];
            return true;
        }
        /// <summary>
        /// 获取某个玩家的物品信息
        /// </summary>
        /// <param name="_nPDBID"></param>
        /// <returns></returns>
        public List<cmd_war_persongoodsinfo> GetPlayerGoodsInfo(ulong _nPDBID)
        {
            if (m_dicPlayersGoodsTable.ContainsKey(_nPDBID))
                return m_dicPlayersGoodsTable[_nPDBID];
            return null;
        }
        /// <summary>
        /// 获取本地玩家的物品信息
        /// </summary>
        /// <returns></returns>
        public List<cmd_war_persongoodsinfo> GetMainPlayerGoodsInfo()
        {
            return GetPlayerGoodsInfo(MainHeroPDBID);
        }

        /// <summary>
        /// 某个玩家（不包括自己）有物品栏有更新的时候存储他的物品栏数据
        /// </summary>
        /// <param name="_nPtrParam"></param>
        /// <param name="_nPtrLen"></param>
        public void UpdatePlayersGoods(IntPtr _nPtrParam, int _nPtrLen)
        {

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(_nPtrParam, _nPtrLen);
            cmd_war_persongoodsinfo_count dataCount = helper.get<cmd_war_persongoodsinfo_count>();
            uint pdbid = dataCount.idActor;

            if (dataCount.nCount == 0 || pdbid == 0)
            {
                return;
            }

            if (!m_dicPlayersGoodsTable.ContainsKey(pdbid))
                m_dicPlayersGoodsTable.Add(pdbid, new List<cmd_war_persongoodsinfo>());

            m_dicPlayersGoodsTable[pdbid].Clear();

            for (int i = 0; i < dataCount.nCount; ++i)
            {
                cmd_war_persongoodsinfo data = helper.get<cmd_war_persongoodsinfo>();
                m_dicPlayersGoodsTable[pdbid].Add(data);
            }

            if (LogicDataCenter.recordTableDataManager.isOneRequest == true)
            {
                LogicDataCenter.recordTableDataManager.OnTabClick();
            }

            //通知战绩表更新数据
            if (LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible)
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, null);
        }

        /// <summary>
        /// 更新本地玩家物品信息
        /// </summary>
        /// <param name="_nPtrParam"></param>
        /// <param name="_nPtrLen"></param>
        public void UpdateMainPlayerGoods(IntPtr _nPtrParam, int _nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(_nPtrParam, _nPtrLen);
            cmd_war_persongoodsinfo_count dataCount = helper.get<cmd_war_persongoodsinfo_count>();

            uint pdbid = dataCount.idActor;
            int nCount = dataCount.nCount;

            if (nCount == 0 || pdbid == 0)
            {
                Debug.LogWarning("UpdateMainPlayerGoods:dataCount.nCount == 0 || pdbid == 0");
                return;
            }

            // 缓存本地玩家的PDBID,取EntityView中的话,有可能还没同步下来
            m_nMainHeroPDBID = pdbid;

            if (!m_dicPlayersGoodsTable.ContainsKey(MainHeroPDBID))
                m_dicPlayersGoodsTable.Add(MainHeroPDBID, new List<cmd_war_persongoodsinfo>());

            //小于最大物品槽数量即是更新的是使用物品
            if (nCount < (int)SLOT_DEFINE.D_MAX_GOODS_SLOT_COUNT)
            {
                UpdateMainPlayerUseGoods(helper, dataCount.nCount);
            }
            else
            {
                UpdataMainPlayerAllGoods(helper, dataCount.nCount);
            }

            FindWhichGoodsCanEffect();


            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, null);
        }
        /// <summary>
        /// 更新本地玩家使用物品数据
        /// </summary>
        /// <param name="_nHelper"></param>
        /// <param name="_nDataCount"></param>
        private void UpdateMainPlayerUseGoods(IntPtrVaArgParseHelper _nHelper, int _nDataCount)
        {
            for (int i = 0; i < _nDataCount; i++)
            {
                cmd_war_persongoodsinfo data = _nHelper.get<cmd_war_persongoodsinfo>();
                if (data.nSlotType != (int)SLOT_TYPE.SLOT_TYPE_GOODS)
                {
                    Trace.LogWarning("UpdateMainPlayerUseGoods 这里有个非物品槽位的数据。检查下");
                    continue;
                }
                int nIndex = m_dicPlayersGoodsTable[MainHeroPDBID].FindIndex(x => x.nSlotIndex == data.nSlotIndex);
                if (nIndex == -1)
                {
                    Trace.LogWarning("UpdateMainPlayerUseGoods 这里有个使用的物品槽位" + data.nSlotIndex + "与当前数据不符，请检查。");
                    continue;
                }

                //使用次数为0，移除
                m_dicPlayersGoodsTable[MainHeroPDBID][nIndex] = data;
            }

        }
        /// <summary>
        /// 更新本地玩家所有物品购买物品数据
        /// </summary>
        /// <param name="_nHelper"></param>
        /// <param name="_nDataCount"></param>
        private void UpdataMainPlayerAllGoods(IntPtrVaArgParseHelper _nHelper, int _nDataCount)
        {
            ResetAssociatedWithPurchasedGoodsSlot();
            for (int i = 0; i < _nDataCount; i++)
            {
                cmd_war_persongoodsinfo data = _nHelper.get<cmd_war_persongoodsinfo>();
                m_dicPlayersGoodsTable[MainHeroPDBID].Add(data);
            }
        }

        /// <summary>
        /// 判断当前物品是否在已购买列表
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <returns></returns>
        public bool IsGoodsInPurchasedList(int _nGoodsID)
        {
            List<cmd_war_persongoodsinfo> tmpPurchasedList = GetMainPlayerGoodsInfo();
            if (tmpPurchasedList == null || tmpPurchasedList.Count <= 0)
                return false;
            for (int i = 0; i < tmpPurchasedList.Count; ++i)
            {
                if (tmpPurchasedList[i].nGoodID != 0 && tmpPurchasedList[i].nGoodID == _nGoodsID)
                    return true;
            }
            return false;
        }
        #region 商店部分
        private ObservableList<UWarGoodsStoreItem> m_SchemeMobaGoodsDataSource;
        /// <summary>这里缓存一份所有物品的引用，界面其他地方需要获取物品时从这里取引用 </summary>
        internal ObservableList<UWarGoodsStoreItem> WarGoodsStoreMobaGoodsDataSource
        {
            get
            {
                if (m_SchemeMobaGoodsDataSource != null)
                    return m_SchemeMobaGoodsDataSource;
                Trace.LogWarning("WarMobaGoodsDataManager.Load()没执行");
                return null;
            }
        }
        private Dictionary<int, UWarGoodsStoreItem> m_dicGoodsItemProtertyChange = new Dictionary<int, UWarGoodsStoreItem>();
        /// <summary>记录当前有属性修改的GoodsItem数据</summary>
        internal Dictionary<int, UWarGoodsStoreItem> GoodsItemPropertyChange
        {
            get
            {
                return m_dicGoodsItemProtertyChange;
            }
        }

        protected bool m_bCanBuyOrSell;
        /// <summary>
        /// 当前逻辑上是否可以购买或者出售
        /// </summary>
        public bool CanBuyOrSell
        {
            get { return m_bCanBuyOrSell; }
            set { m_bCanBuyOrSell = value; }
        }

        /// <summary>
        /// 从配置信息初始化所有物品数据
        /// 这里要在获取战场物品配置后执行
        /// </summary>
        /// <param name="_allGoodsTable"></param>
        private void InitWarGoodsStoreMobaGoodsDataSource()
        {
            foreach (var goods in m_dicWarMobaGoodsTable)
            {
                AddGoodsItemDataSource(goods.Value);
            }
        }
        /// <summary>
        /// 添加物品数据
        /// </summary>
        /// <param name="_SchemeGoods"></param>
        protected void AddGoodsItemDataSource(SSchemeMobaGoods _SchemeGoods)
        {
            UWarGoodsStoreItem goods = new UWarGoodsStoreItem(_SchemeGoods);
            goods.OnPropertyChange += OnDataPropertyChange;
            m_SchemeMobaGoodsDataSource.Add(goods);
        }
        /// <summary>
        /// 移除物品数据
        /// </summary>
        /// <param name="_nItem"></param>
        protected void RemoveGoodsItemDataSource(UWarGoodsStoreItem _nItem)
        {
            if (m_SchemeMobaGoodsDataSource.Contains(_nItem))
            {
                _nItem.OnPropertyChange -= OnDataPropertyChange;
                m_SchemeMobaGoodsDataSource.Remove(_nItem);
            }
        }
        /// <summary>
        /// 物品数据某个属性发生改变时触发,前提是这个属性被标记为可触发类型
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnDataPropertyChange(object sender, PropertyChangeEventArgs e)
        {
            if (sender is UWarGoodsStoreItem)
            {
                UWarGoodsStoreItem item = sender as UWarGoodsStoreItem;
                if (!m_dicGoodsItemProtertyChange.ContainsKey(item.GoodsID))
                {
                    m_dicGoodsItemProtertyChange.Add(item.GoodsID, item);
                }
            }
        }
        /// <summary>
        /// 根据配置信息获取物品数据
        /// </summary>
        /// <param name="_goodsScheme"></param>
        /// <returns></returns>
        internal UWarGoodsStoreItem GetWarGoodsStoreItem(SSchemeMobaGoods _goodsScheme, bool _bFilterGood = true)
        {
            if (_goodsScheme == null || _goodsScheme.GoodsID <= 0)
            {
                Trace.LogWarning("GetWarGoodsStoreItem 的参数是空或者GoodsID为0");
                return null;
            }

            return GetWarGoodsStoreItem(_goodsScheme.GoodsID, _bFilterGood);
        }
        /// <summary>
        /// 根据物品ID获取物品数据
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <returns></returns>
        internal UWarGoodsStoreItem GetWarGoodsStoreItem(int _nGoodsID, bool _bFilterGood = true)
        {
            UWarGoodsStoreItem tmpGoods = null;

            foreach (var goods in WarGoodsStoreMobaGoodsDataSource)
            {
                if (goods.GoodsID == _nGoodsID)
                {
                    tmpGoods = goods;
                    break;
                }
            }

            //如果当前战场开启了物品过滤,则过滤一次
            if (_bFilterGood && EnableGoodsFilter)
            {
                if (tmpGoods != null && tmpGoods.IsFiltered)
                    tmpGoods = null;
            }

            //if (tmpGoods == null)
            //{
            //    Trace.LogWarning("没找到ID：" + _nGoodsID + "的物品");
            //}
            return tmpGoods;
        }
        /// <summary>
        /// 根据物品类型获取物品数据列表
        /// </summary>
        /// <param name="_nGoodsLabel">物品类型</param>
        /// <returns></returns>
        internal ObservableList<UWarGoodsStoreItem> GetWarGoodsStoreItem(string _strGoodsLabelID, bool _bFilterGood = true)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };

            //选的是类型子类
            if (Convert.ToInt32(_strGoodsLabelID) > 1000)
            {
                return WarGoodsStoreMobaGoodsDataSource.Where((item) =>
                {
                    if (_bFilterGood && EnableGoodsFilter) //如果需要过滤
                    {
                        if(item.IsFiltered)
                            return false;
                    }

                    foreach (string label in item.SchemeGoodsInfo.GoodsLabel.Split(charSeparator))
                    {
                        if (string.IsNullOrEmpty(label))
                        {
                            continue;
                        }
                        if (label == _strGoodsLabelID)
                            return true;
                    }
                    return false;
                }).ToObservableList();
            }
            else
            {
                int nGoodsLabelID = Convert.ToInt32(_strGoodsLabelID) * 1000;
                int nItemLabelID = -1;
                return WarGoodsStoreMobaGoodsDataSource.Where((item) =>
                {
                    if (_bFilterGood && EnableGoodsFilter) //如果需要过滤
                    {
                        if (item.IsFiltered)
                            return false;
                    }

                    foreach (string label in item.SchemeGoodsInfo.GoodsLabel.Split(charSeparator))
                    {
                        if (string.IsNullOrEmpty(label))
                        {
                            continue;
                        }
                        nItemLabelID = Convert.ToInt32(label);

                        if (nItemLabelID > nGoodsLabelID && nItemLabelID < nGoodsLabelID + 1000)
                            return true;
                    }
                    return false;
                }).ToObservableList();
            }
        }
        /// <summary>
        /// 根据物品类型获取物品数据列表
        /// </summary>
        /// <param name="_nGoodsLabel">物品类型</param>
        /// <returns></returns>
        internal ObservableList<UWarGoodsStoreItem> GetWarGoodsStoreItem(List<int> _listGoodsLabelID, bool _bFilterGood = true)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };
            //遍历所有物品数据
            return WarGoodsStoreMobaGoodsDataSource.Where((item) =>
            {
                string[] itemLabels = item.SchemeGoodsInfo.GoodsLabel.Split(charSeparator);
                if(itemLabels.Length == 0)
                {
                    Trace.LogWarning("策划检查下物品ID：" + item.GoodsID + ",的标签子类型.");
                    return false;
                }

                if (_bFilterGood && EnableGoodsFilter) //如果需要过滤
                {
                    if (item.IsFiltered)
                        return false;
                }

                int confirmCount = 0;
                int tmpLabelID = -1;

                //遍历物品所有类型标签数据
                for (int i = 0; i < itemLabels.Length; ++i)
                {
                    if (string.IsNullOrEmpty(itemLabels[i]))
                        continue;

                    if (!int.TryParse(itemLabels[i], out tmpLabelID))
                        continue;
                    //判断类型是否在勾选列表里
                    if (_listGoodsLabelID.Contains(tmpLabelID))
                    {                       
                        confirmCount++;                       
                    }
                }
                
                return confirmCount == _listGoodsLabelID.Count;
            }).ToObservableList();

        }
        /// <summary>
        /// 玩家金钱显示改变的时候执行，判断物品能否购买
        /// </summary>
        /// <param name="_nMoney"></param>
        internal void onMoneyChange(int _nMoney)
        {
            WarGoodsStoreMobaGoodsDataSource.ForEach((item) =>
            {
                item.GoodsCanBuy = item.GoodsCost <= _nMoney ? true : false;
            });

        }
        /// <summary>
        /// 有物品购买时更新数据
        /// </summary>
        internal void onPurchasedChange()
        {
            List<int> purchasedGoodsID = GetPurchasedGoodsIDList();
            if(purchasedGoodsID != null)
            {
                WarGoodsStoreMobaGoodsDataSource.ForEach((item) =>
                {
                    item.IsPurchased = purchasedGoodsID.Contains(item.GoodsID);
                });
            }

        }

        #region 购买物品
        //检测函数迭代次数，避免死循环
        private int m_nRunFindNum = 0;
        //检测函数迭代次数，避免死循环
        private int m_nFirstGoodsID = 0;

        /// <summary>
        /// 存储与当前已购买物品相关的所有物品
        /// </summary>
        protected Dictionary<int, List<int>> allCanComposeGoodsMap = new Dictionary<int, List<int>>();


        /// <summary>
        /// 遍历所有已购买物品插槽，根据已购买物品ID更改与之相关的物品价格
        /// </summary>
        private void FindWhichGoodsCanEffect()
        {
            for (int i = 0; i < m_dicPlayersGoodsTable[MainHeroPDBID].Count; i++)
            {
                if (m_dicPlayersGoodsTable[MainHeroPDBID][i].nGoodID != 0)
                {
                    IteratorFindCanComposeGoods(m_dicPlayersGoodsTable[MainHeroPDBID][i].nGoodID, ref allCanComposeGoodsMap);
                }
            }
        }

        /// <summary>
        /// 重置与所有物品的价格
        /// </summary>
        private void ResetAssociatedWithPurchasedGoodsSlot()
        {
            SetDataSourceReset();
            m_dicPlayersGoodsTable[MainHeroPDBID].Clear();
        }

        /// <summary>
        /// 迭代当前已购买物品可合成物品，并存入重置列表
        /// </summary>
        /// <param name="_nCurrentGoodsID">当前已购买物品</param>
        /// <param name="_AllCanComposeGoodsMap">物品重置列表</param>
        private void IteratorFindCanComposeGoods(int _nCurrentGoodsID, ref Dictionary<int, List<int>> _AllCanComposeGoodsMap)
        {
            List<SSchemeMobaGoods> CurPurchasedGoodsCanComposeList;

            int deltaCost = 0;
            int newCost = 0;
            CurPurchasedGoodsCanComposeList = GameMobaSchemesManager.Instance.GetCanComposeSchemeGoodsList(_nCurrentGoodsID);
            List<cmd_war_persongoodsinfo> curPurchasedGoodsCopyList;

            m_nRunFindNum++;
            if (m_nRunFindNum == 1)
                m_nFirstGoodsID = _nCurrentGoodsID;
            if (m_nRunFindNum > 10)
            {
                Trace.LogError("配置物品的可合成物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nCurrentGoodsID.ToString());

                m_nRunFindNum--;
                m_nFirstGoodsID = 0;
                return;
            }

            //根据当前物品是否有可合成物品列表，来决定是否要更改价格
            if (CurPurchasedGoodsCanComposeList == null)
            {
                m_nRunFindNum--;
                return;
            }

            for (int j = 0; j < CurPurchasedGoodsCanComposeList.Count; j++)
            {

                curPurchasedGoodsCopyList = new List<cmd_war_persongoodsinfo>(m_dicPlayersGoodsTable[MainHeroPDBID].ToArray());

                deltaCost = 0;
                //迭代合成所需物品列表
                IteratorFindNeedComposeGoods(CurPurchasedGoodsCanComposeList[j].GoodsID, curPurchasedGoodsCopyList, ref deltaCost);

                newCost = CurPurchasedGoodsCanComposeList[j].GoodsCost - deltaCost;

                SetAssociatedWithPurchasedGoodsSlot(CurPurchasedGoodsCanComposeList[j].GoodsID, newCost);

                if (!_AllCanComposeGoodsMap.ContainsKey(CurPurchasedGoodsCanComposeList[j].GoodsID))
                {
                    _AllCanComposeGoodsMap.Add(CurPurchasedGoodsCanComposeList[j].GoodsID, new List<int>(0));
                }
                //迭代当前物品的可合成物品列表
                IteratorFindCanComposeGoods(CurPurchasedGoodsCanComposeList[j].GoodsID, ref _AllCanComposeGoodsMap);
            }

            curPurchasedGoodsCopyList = null;
            m_nRunFindNum--;
        }

        /// <summary>
        /// 迭代当前物品的合成所需物品列表，判断该列表里面是否有已购买物品，有则，扣除已购买物品价格
        /// </summary>
        /// <param name="_nCurGoodsID">当前物品</param>
        /// <param name="_curPurchasedGoodsCoypList">当前已购买物品列表</param>
        /// <param name="_nDeltaCost">当前需要扣除的价格</param>
        private void IteratorFindNeedComposeGoods(int _nCurGoodsID, List<cmd_war_persongoodsinfo> _curPurchasedGoodsCoypList, ref int _nDeltaCost)
        {
            List<int> CurGoodsNeedList;
            CurGoodsNeedList = GameMobaSchemesManager.Instance.GetSchemeGoodsNeedComposeList(_nCurGoodsID);
            SSchemeMobaGoods CurSchemeMobaGoods;

            m_nRunFindNum++;
            if (m_nRunFindNum == 1)
                m_nFirstGoodsID = _nCurGoodsID;
            if (m_nRunFindNum > 10)
            {
                m_nRunFindNum--;
                m_nFirstGoodsID = 0;
                Trace.LogError("配置物品的可合成所需物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nCurGoodsID.ToString());
                return;
            }

            if (CurGoodsNeedList == null)
            {
                m_nRunFindNum--;
                return;
            }

            for (int i = 0; i < CurGoodsNeedList.Count; i++)
            {
                //当前物品需要的合成物品遍历
                int needGoodID = CurGoodsNeedList[i];
                bool bFind = false;
                for (int k = 0; k < _curPurchasedGoodsCoypList.Count; k++)
                {
                    int curHaveGoodID = _curPurchasedGoodsCoypList[k].nGoodID;
                    if (curHaveGoodID == 0)
                        continue;
                    if (needGoodID == curHaveGoodID)
                    {
                        //找到了
                        bFind = true;

                        LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(needGoodID, out CurSchemeMobaGoods);

                        // 减去价格
                        _nDeltaCost += CurSchemeMobaGoods.GoodsCost;
                        _curPurchasedGoodsCoypList.RemoveAt(k);
                        break;
                    }
                }

                if (!bFind)
                {
                    IteratorFindNeedComposeGoods(needGoodID, _curPurchasedGoodsCoypList, ref _nDeltaCost);
                }
            }
            CurSchemeMobaGoods = null;
            m_nRunFindNum--;
        }

        /// <summary>
        /// 修改与当前已购买物品相关的物品价格，如当前已购买物品可合成物品的价格
        /// </summary>
        /// <param name="_nCurGoodsID">当前与已购买物品相关的物品</param>
        /// <param name="_nNewCost">该物品新的价格</param>
        private void SetAssociatedWithPurchasedGoodsSlot(int _nCurGoodsID, int _nNewCost)
        {
            //m_wndView.SetGoodsCost(_nCurGoodsID, _nNewCost);
            if (GetWarGoodsStoreItem(_nCurGoodsID) != null)
            {
                GetWarGoodsStoreItem(_nCurGoodsID).GoodsCost = _nNewCost;
            }
        }

        internal List<int> GetPurchasedGoodsIDList()
        {
            if (IsMainHeroPDBIDValid && m_dicPlayersGoodsTable.ContainsKey(MainHeroPDBID))
                return m_dicPlayersGoodsTable[MainHeroPDBID].Select(keyValuePair => keyValuePair.nGoodID).ToList();
            else
                return null;
        }

        /// <summary>
        /// 重置物品显示价格数据
        /// </summary>
        internal void SetDataSourceReset()
        {
            WarGoodsStoreMobaGoodsDataSource.ForEach((item) =>
            {
                item.ResetItemValue();
            });
        }

        #endregion

        public void SetHeroDefaultGoods(int _nIndex, int _nGoodsID, int _eMobaRecommendGoods)
        {
            if (!m_dicHeroDefaultGoodsDisposition.ContainsKey(_eMobaRecommendGoods))
                m_dicHeroDefaultGoodsDisposition.Add(_eMobaRecommendGoods, new List<int>());
            //这里不限定6位了，以免策划以后想增加不好处理
            m_dicHeroDefaultGoodsDisposition[_eMobaRecommendGoods].Add(_nGoodsID);


        }
        #endregion

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)ETimerHandlerID.GoodsDispositionList:
                    SetDispositionListViewData();
                    break;
            }
        }

    }
}

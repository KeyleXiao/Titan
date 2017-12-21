using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using GameLogic;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
    public enum EGoodsStoreSubView
    {
        EGSSV_RecommendGoods,
        EGSSV_AllGoods,
        EGSSV_DetailGoods,
        EGSSV_CanSyntheszedGoods,
        EGSSV_TreeSynthesisGoods,
        EGSSV_PurchasedGoods,
        EGSSV_SearchGoods,
        EGSSV_DispositionGoods,
        EGSSV_TopBarGoods,
    }
    public enum EWarGoodsStoreTimerHandler
    {
        ETH_ProtertyChange,
    }

    public enum EWarGoodsStoreSearchFilter
    {
        ESF_Startswith,
        ESF_Contains,
    }

    [Serializable]
    public class UWarGoodsStoreGoodsClickEvent : UnityEvent<UWarGoodsStoreItem>
    { }
    [Serializable]
    public class UWarGoodsStoreDataSourceChangeEvent : UnityEvent<UWarGoodsStoreDataSourceChangeParam>
    { }
    public class UWarGoodsStoreDataSourceChangeParam
    {
        public Dictionary<int, UWarGoodsStoreItem> DicGoodsItemProtertyChange;

        public UWarGoodsStoreDataSourceChangeParam(Dictionary<int, UWarGoodsStoreItem> _dicProtetyrChange)
        {
            DicGoodsItemProtertyChange = _dicProtetyrChange;
        }
    }


    public class UWarGoodsStoreSubViewBase : UCustomUIBehaviour
    {
        protected UWarGoodsStoreView m_MyParent;

        public virtual EGoodsStoreSubView GetSubViewType
        {
            get
            {
                throw new NotImplementedException();
            }

        }

        public virtual bool Init(UWarGoodsStoreView _viewParent)
        {
            m_MyParent = _viewParent;
            m_MyParent.onMobaGoodsDataSourceChange.AddListener(OnMobaGoodsDataSourceChange);
            m_MyParent.onMobaGoodsClick.AddListener(OnMobaGoodsClick);

            return true;
        }
        public virtual void SetVisible(bool visible)
        {
            gameObject.SetActive(visible);
        }


        #region 界面内购买和选择事件回调统一方法，因为用了两种list和没用list的（合成树部分） 按需重载
        public virtual void OnMobaGoodsBuy(UButtonGroupItem _nItem, bool _bSelected) { }
        public virtual void OnMobaGoodsBuy(int _nIndex, ListViewItem _itemComponent) { }
        public virtual void OnMobaGoodsBuy(ListViewItem _itemComponent) { }
        public virtual void OnMobaGoodsSelected(UButtonGroupItem _nItem, bool _bSelected) { }
        public virtual void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent) { }
        public virtual void OnMobaGoodsSelected(ListViewItem _itemComponent) { }

        #endregion
        public virtual void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        { }
        /// <summary>
        /// 当前选择的物品改变时响应的方法
        /// </summary>
        /// <param name="_nParam"></param>
        public virtual void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {

        }

        protected override void OnDestroy()
        {
            USpriteManager.Instance.DeleteSprite(WndID.WND_ID_WAR_GOODSSTORE);
            m_MyParent.onMobaGoodsDataSourceChange.RemoveListener(OnMobaGoodsDataSourceChange);
            m_MyParent.onMobaGoodsClick.RemoveListener(OnMobaGoodsClick);
            base.OnDestroy();
        }

    }

    public class UWarGoodsStoreTabItem : UButtonGroupItem
    {
        private RectTransform m_nRectTransform;
        public RectTransform rectTransform
        {
            get
            {
                if (m_nRectTransform == null)
                    m_nRectTransform = transform as RectTransform;
                return m_nRectTransform;
            }
        }

        private Text m_ItemTxt;
        public Text ItemTxt
        {
            get
            {
                if (m_ItemTxt == null)
                    m_ItemTxt = this.GetComponentInChildren<Text>();

                return m_ItemTxt;
            }
        }

        private ULocalizedText m_nLocalizedText;
        public ULocalizedText LocalizedText
        {
            get
            {
                if (m_nLocalizedText == null)
                    m_nLocalizedText = GetComponentInChildren<ULocalizedText>();
                return m_nLocalizedText;
            }
        }

        private int m_nItemIndex;
        public int ItemIndex { get { return m_nItemIndex; } }

        private int m_nDispositionID;
        public int DispositionID { get { return m_nDispositionID; } }

        protected override void OnEnable()
        {
            if (m_nItemIndex == -1)
                m_nItemIndex = Group != null ? Group.Length() : -1;

            base.OnEnable();
        }

        public void Init(string _strLocalFilter,string _strLocalKey,int _nDispositionID,UnityAction<UButtonGroupItem,bool> _actionSelected,UButtonGroup _buttonGroup)
        {
            if (!string.IsNullOrEmpty(_strLocalFilter))
            {
                LocalizedText.enabled = true;
                ULocalizationService.Instance.Get(ItemTxt, "UIView", _strLocalFilter, _strLocalKey);
            }
                
            else
            {
                LocalizedText.enabled = false;
                ItemTxt.text = _strLocalKey;
            }
                

            rectTransform.SetParent(_buttonGroup.transform, false);
            this.onSelectedChangedEx.AddListener(_actionSelected);
            m_nItemIndex = _buttonGroup != null ? _buttonGroup.Length() : -1;
            m_nDispositionID = _nDispositionID;
            Group = _buttonGroup;
        }

        public override void SetButtonGroup(UButtonGroup newGroup, bool setMemberValue)
        {
            UButtonGroup oldGroup = m_Group;

            if (m_Group != null)
                m_Group.UnregisterButton(this);

            if (setMemberValue)
                m_Group = newGroup;

            if (m_Group != null)
                m_Group.RegisterButton(this);

            if (newGroup != null && newGroup != oldGroup && Selected)// && IsActive())
                m_Group.NotifyItemOn(this);
        }

    }

    public class UWarGoodsStoreView : UIBaseWndView, ITimerHanlder
    {

        public const int MobaGoodsIconSize = 1;

        public UButtonGroup GoodsStoreTab;
        public UWarGoodsStoreTabItem DefaultTabItem;
        public Transform MobaGoodsFrame;
        public Transform GoodsDetailFrame;
        public Transform CanSynthesizedFrame;
        public Transform TreeSynthesisFrame;
        public Transform PurchasedFrame;
        public Transform SearchResultFrame;
        public Transform DispositionFrame;
        public Transform TopFrame;
		
		public Button BtnClose;	


        public UWarGoodsStoreGoodsClickEvent onMobaGoodsClick = new UWarGoodsStoreGoodsClickEvent();

        public UWarGoodsStoreDataSourceChangeEvent onMobaGoodsDataSourceChange = new UWarGoodsStoreDataSourceChangeEvent();

        protected UWarGoodsStoreItem m_CurrentSelectedGoods;
        public UWarGoodsStoreItem CurrentSelectedGoods
        {
            get { return m_CurrentSelectedGoods; }
            set
            {
                if (m_CurrentSelectedGoods != value && value.GoodsID != 0 && value.SchemeGoodsInfo != null)
                {
                    m_CurrentSelectedGoods = value;
                    //这里要根据来的GoodsID再获取下物品当前的信息
                    //Debug.LogWarning("CurrentSelectedGoods：" + CurrentSelectedGoods.GoodsID + " ,名字：" + CurrentSelectedGoods.SchemeGoodsInfo.GoodsName);
                    onMobaGoodsClick.Invoke(m_CurrentSelectedGoods);
                }
            }
        }


        private UWarGoodsStoreWnd m_GoodStoreWnd { get { return m_wnd as UWarGoodsStoreWnd; } }

        //推荐物品按钮选择回调
        public void OnBtnRecommendSelected(UButtonGroupItem _item,bool _bSelect)
        {
            if (_bSelect)
            {
                SwitchSubWndPage(EGoodsStoreSubView.EGSSV_RecommendGoods, _bSelect);

                if (DispositionGoodsView != null)
                    DispositionGoodsView.OnDispositionSelect(0);
            }
                
        }
        //所有物品按钮选择回调
        public void OnBtnAllSelected(UButtonGroupItem _item,bool _bSelect)
        {
            if (_bSelect)
                SwitchSubWndPage(EGoodsStoreSubView.EGSSV_AllGoods, _bSelect);
        }
        // 推荐物品按钮选择
        public void OnOtherTabItemSelected(UButtonGroupItem _item,bool _bSelect)
        {
            if (_bSelect)
            {
                SwitchSubWndPage(EGoodsStoreSubView.EGSSV_RecommendGoods, _bSelect);

                UWarGoodsStoreTabItem tmpItem = _item as UWarGoodsStoreTabItem;
                if (tmpItem != null && DispositionGoodsView != null)
                {
                    //因为前两个TabItem序号是默认推荐物品(0)和所有物品(1)，所以，序号需要往前移1位
                    DispositionGoodsView.OnDispositionSelect(tmpItem.ItemIndex - 1);
                }
            }

        }
        //购买按钮
        public void OnBtnBuyClick()
        {
            if (CurrentSelectedGoods != null && CurrentSelectedGoods.GoodsID != 0)
                SetWndBuyGoods(CurrentSelectedGoods.GoodsID);
        }
        //出售按钮
        public void OnBtnSellClick()
        {
            if(PurchasedGoodsView != null)
            m_GoodStoreWnd.onBtnSellClick(PurchasedGoodsView.SelectedPurchasedGoodsID, PurchasedGoodsView.SelectedPurchasedGoodsSlotID);
        }
        //撤销按钮
        public void OnBtnRevokedClick()
        {
            m_GoodStoreWnd.onBtnRevokedClick();
        }
        //关闭按钮
        public void OnBtnCloseClick()
        {
            m_GoodStoreWnd.SetVisible(false);
        }
        public void onMoneyChange(int _nMoney)
        {
            SetMoneyText(_nMoney);
        }
        /// <summary>
        /// 查找文本改变事件
        /// </summary>
        /// <param name="_strText"></param>
        public void SetSearchValueChanged(int _nSearchType,string _strText)
        {
            if (SearchResultGoodsView != null)
            {
                SearchResultGoodsView.OnSearchValueChange((EWarGoodsStoreSearchFilter)_nSearchType, _strText);
            }
        }

        public void SetSearchValueEndEdit(string _strText)
        {
            if (SearchResultGoodsView != null)
            {
                SearchResultGoodsView.OnSearchValueEndEdit(_strText);
            }
        }

        public void SetSelectedGoodsDispositionInfo(UWGS_SelectedGoodsDispositionMsgData _msgData)
        {
            if(TopBarGoodsView != null)
            {
                TopBarGoodsView.SetSelectedGoodsDispositionInfo(_msgData);
            }
        }

        public void GetToggleDisposition(bool _bToggleIsOn)
        {
            if (DispositionGoodsView != null)
            {
                DispositionGoodsView.OnToggleListView(_bToggleIsOn);
            }
        }
        public void SetToggleState(bool _bToggleIsOn)
        {
            if (TopBarGoodsView != null)
            {
                TopBarGoodsView.SetToggleState(_bToggleIsOn);
            }
        }

        public override bool Init(IUIWnd wnd)
        {
            bool res = base.Init(wnd);
            canDrag = true;
            InitWidget();
            InitSubView();
            m_GoodStoreWnd.InitMobaGoodsScheme();

            OnBtnRecommendSelected(null,true);
            TimerManager.SetTimer(this, (int)EWarGoodsStoreTimerHandler.ETH_ProtertyChange, 0.5f);
            //TimerManager.SetTimer(this, 1, 5.0f);
            return res;
        }
        protected void InitWidget()
        {
            {
                if (BtnClose == null)
                {
                    Transform CloseTrans = GetTransform().FindChild("Frame/BtnClose");
                    if (CloseTrans != null)
                        BtnClose = CloseTrans.GetComponent<Button>();
                    else
                        Trace.LogError("没找到对象名为:BtnClose 的按钮");
                }
                if (BtnClose == null)
                    Trace.LogError("对象名为:BuyBtn 没找到Button组件");
                else
                    BtnClose.onClick.AddListener(OnBtnCloseClick);
            }
            //初始化按钮组
            {
                
                UWarGoodsStoreTabItem RecommendItem  = SetGoodsStoreTabItem("WarGoodsStore", "RecommendGoods",0, OnBtnRecommendSelected);
                if(RecommendItem != null)
                    GoodsStoreTab.SetSelectedItem(RecommendItem, true);

                SetGoodsStoreTabItem("WarGoodsStore", "AllGoods",-1, OnBtnAllSelected);

            }

        }



        public override void Destroy()
        {
            base.Destroy();
            TimerManager.KillTimer(this, (int)EWarGoodsStoreTimerHandler.ETH_ProtertyChange);
        }

        /// <summary>
        /// 创建顶部标签元素
        /// </summary>
        /// <returns></returns>
        public UWarGoodsStoreTabItem SetGoodsStoreTabItem(string _strLocalFilter,string _strLocalKey,int _nDispositionID,UnityAction<UButtonGroupItem,bool> _actionSelected)
        {

            UWarGoodsStoreTabItem tmpItem = ResNode.InstantiateRes<UWarGoodsStoreTabItem>(DefaultTabItem.gameObject, true);
            if(tmpItem != null)
            {
                tmpItem.gameObject.SetActive(true);
                tmpItem.Init(_strLocalFilter, _strLocalKey, _nDispositionID, _actionSelected, GoodsStoreTab);

            }

            return tmpItem;
        }
        /// <summary>
        /// 创建推荐物品配置标签元素
        /// </summary>
        /// <param name="_strLocalKey"></param>
        public void SetGoodsStoreTabItem(int _nDispositionID,string _strLocalKey)
        {
            SetGoodsStoreTabItem(string.Empty, _strLocalKey, _nDispositionID, OnOtherTabItemSelected);
        }
        /// <summary>
        /// 根据物品配置ID设置对应的推荐物品配置标签元素选择
        /// </summary>
        /// <param name="_nDispositionID"></param>
        public void SetGoodsStoreTabItemSelected(int _nDispositionID)
        {
            for (int i = 0; i < GoodsStoreTab.Length(); ++i)
            {
                UWarGoodsStoreTabItem item = GoodsStoreTab.GetItemAt(i) as UWarGoodsStoreTabItem;
                if(item != null)
                {
                    if(item.DispositionID == _nDispositionID)
                    {
                        item.Selected = true;
                        return;
                    }
                }
            }
        }

        /// <summary>
        /// 设置当前显示玩家的金钱数量
        /// </summary>
        /// <param name="_nMoney"></param>
        public void SetMoneyText(int _nMoney)
        {
            if(PurchasedGoodsView !=null)
            PurchasedGoodsView.SetMoneyText(_nMoney);
        }
        /// <summary>
        /// 设置已购买物品列表
        /// </summary>
        /// <param name="_purchasedGoods"></param>
        public void SetPurchasedGoods(List<cmd_war_persongoodsinfo> _purchasedGoods)
        {
            //UWarGoodsStoreItem item = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_purchasedGoods.nGoodID);
            if (PurchasedGoodsView != null)
                PurchasedGoodsView.SetPurchasedGoods(_purchasedGoods);
            if (RecommendGoodsView != null)
                RecommendGoodsView.SetPurchasedGoods(_purchasedGoods);
        }

        /// <summary>
        /// 判断当前能否执行撤销操作
        /// </summary>
        /// <param name="_bCanRevoked"></param>
        public void SetCanRevoked(bool _bCanRevoked)
        {
            if(PurchasedGoodsView != null)
            PurchasedGoodsView.SetCanRevoked(_bCanRevoked);
        }
        public void SetBuyBtnEnable(bool _bCanBuy)
        {
            if(DetailGoodsView != null)
            {
                _bCanBuy = CurrentSelectedGoods == null ? false : CurrentSelectedGoods.GoodsCanBuy;

                DetailGoodsView.SetBuyBtnEnable(_bCanBuy);
            }
            if (PurchasedGoodsView != null)
            {
                _bCanBuy = CurrentSelectedGoods == null ? false : WarMobaGoodsDataManager.Instance.IsGoodsInPurchasedList(CurrentSelectedGoods.GoodsID);
                PurchasedGoodsView.SetSellRevokedBtnEnable(_bCanBuy);
            }            
        }
        /// <summary>
        /// 设置合成树已购买物品图标
        /// </summary>
        /// <param name="_nPurchasedList"></param>
        public void SetTreeSynthesisPurchasedIcon(List<int> _nPurchasedList)
        {
            if(TreeSynthesisGoodsView != null)
            TreeSynthesisGoodsView.DrawPurchasedIcon(_nPurchasedList);
        }
        /// <summary>
        /// 通知中介者去进行购买操作
        /// </summary>
        /// <param name="_nGoodsID"></param>
        public void SetWndBuyGoods(int _nGoodsID)
        {
            m_GoodStoreWnd.onBtnBuyClick(_nGoodsID);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_nGoodsID">当前推荐配置物品ID列表（目前默认替换2、3种类）</param>
        public void SetCurrentGoodsDisposition(int[] _nGoodsID)
        {
            if(RecommendGoodsView != null && _nGoodsID.Length > 0)
            {
                //推荐页推荐物品序号，每一种类有6个物品
                int recommendIndex = -1;

                if ((int)WarMobaGoodsDataManager.Instance.GoodsDispositionRecmomendType.Length != 2)
                    Trace.LogWarning("策划看看 MobaRecommendGoods.csv该英雄的 替换的物品种类列表 是不是配置有问题。只需要两个可替换的种类列表序号。");

                //取默认配置中种类4所有物品ID
                List<int> UpWindgoodsIDList = GameMobaSchemesManager.Instance.CurrentHeroRecommendGoods.RecommendGoods_UpWindList.Select(d => d.GoodsID).ToList();
                //默认配置中类1出门装物品ID
                List<int> EerlygoodsIDList = GameMobaSchemesManager.Instance.CurrentHeroRecommendGoods.RecommendGoods_EerlyList.Select(d => d.GoodsID).ToList();
                //去除记录的常用装备中与推荐出门装中重复的
                if (EerlygoodsIDList != null && EerlygoodsIDList.Count > 0)
                {
                    for (int i = 0; i < EerlygoodsIDList.Count; i++)
                    {
                        if (EerlygoodsIDList[i] == 0) continue;
                        for (int j = 0; j < _nGoodsID.Length; j++)
                        {
                            if (_nGoodsID[j] == EerlygoodsIDList[i])    
                            {
                                _nGoodsID[j] = 0;
                                break;
                            }
                        }
                    }
                }

                for (int i=0;i< _nGoodsID.Length;++i)
                {
                    //目前固定都是6个，只替换两个种类
                    recommendIndex = i < 6 ? (int)WarMobaGoodsDataManager.Instance.GoodsDispositionRecmomendType[0] : (int)WarMobaGoodsDataManager.Instance.GoodsDispositionRecmomendType[1];
                    //推荐类型不是从0起的，偏移一下
                    recommendIndex = (recommendIndex) * 6 + (i < 6 ? i : i - 6);

                    RecommendGoodsView.SetRecommendedGoods(_nGoodsID[i], recommendIndex);

                    //推荐四与替换部分 物品重复则移除
                    if(UpWindgoodsIDList != null && UpWindgoodsIDList.Count > 0)
                    {
                        int UpWindIndex = UpWindgoodsIDList.IndexOf(_nGoodsID[i]);
                        if (UpWindIndex != -1)
                        {
                            //goodsIDList[UpWindIndex] = 0;
                            UpWindgoodsIDList.RemoveAt(UpWindIndex);
                        }
                    }
             
                }

                List<int> usedStageList = GameMobaSchemesManager.Instance.CurrentHeroRecommendGoods.RecommendGoods_UsedStageList;
                int recommendIndexBase = usedStageList.IndexOf((int)EMobaRecommendGoods.EMRG_UpWindStage);

                //重设推荐四物品列表
                if(recommendIndexBase >= 0)
                {
                    int count = UpWindgoodsIDList.Count >= 6 ? 6 : UpWindgoodsIDList.Count;
                    int goodsID = 0;
                    for (int i = 0; i < 6; ++i)
                    {
                        recommendIndex = (recommendIndexBase) * 6 + i;
                        goodsID = i < count ? UpWindgoodsIDList[i] : 0;//少于6个时，隐藏多余的。
                        RecommendGoodsView.SetRecommendedGoods(goodsID, recommendIndex);
                    }
                }

                RecommendGoodsView.UpdateVisible();
            }
        }

        #region 切换两个大的子部件（推荐页和所有物品页）
        private EGoodsStoreSubView m_CurGoodsStoreSubView;

        private void SwitchSubWndPage(EGoodsStoreSubView _eWndType, bool _bVisible)
        {
            if(m_dicSubView.ContainsKey(_eWndType))
            {
                m_dicSubView[m_CurGoodsStoreSubView].SetVisible(!_bVisible);
                m_dicSubView[_eWndType].SetVisible(_bVisible);
                m_CurGoodsStoreSubView = _eWndType;
                RefreshSubView(_eWndType);
            }
            if (TopBarGoodsView != null)
                TopBarGoodsView.TopBarTypeChange(_eWndType);
        }
        #endregion

        #region 初始化子部件
        protected Dictionary<EGoodsStoreSubView, UWarGoodsStoreSubViewBase> m_dicSubView = new Dictionary<EGoodsStoreSubView, UWarGoodsStoreSubViewBase>();

        public UWarRecommendGoodsView RecommendGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_RecommendGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_RecommendGoods] as UWarRecommendGoodsView);
                Trace.LogWarning("未初始化 UWarRecommendGoodsView");
                return null;
            }
        }
        public UWarAllGoodsView AllGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_AllGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_AllGoods] as UWarAllGoodsView);
                Trace.LogWarning("未初始化 UWarAllGoodsView");
                return null;
            }
        }
        public UWarDetailGoodsView DetailGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_DetailGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_DetailGoods] as UWarDetailGoodsView);
                Trace.LogWarning("未初始化 UWarDetailGoodsView");
                return null;
            }
        }
        public UWarCanSynthesizedGoodsView CanSyntheszedGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_CanSyntheszedGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_CanSyntheszedGoods] as UWarCanSynthesizedGoodsView);
                Trace.LogWarning("未初始化 UWarCanSynthesizedGoodsView");
                return null;
            }
        }
        public UWarTreeSynthesisGoodsView TreeSynthesisGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_TreeSynthesisGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_TreeSynthesisGoods] as UWarTreeSynthesisGoodsView);
                Trace.LogWarning("未初始化 UWarTreeSynthesisGoodsView");
                return null;
            }
        }
        public UWarPurchasedGoodsView PurchasedGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_PurchasedGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_PurchasedGoods] as UWarPurchasedGoodsView);
                Trace.LogWarning("未初始化 UWarPurchasedGoodsView");
                return null;
            }
        }
        internal UWarSearchResultGoodsView SearchResultGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_SearchGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_SearchGoods] as UWarSearchResultGoodsView);
                Trace.LogWarning("未初始化 UWarSearchResultGoodsView");
                return null;
            }
        }
        public UWarDispositionGoodsView DispositionGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_DispositionGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_DispositionGoods] as UWarDispositionGoodsView);
                Trace.LogWarning("未初始化 UWarDispositionGoodsView");
                return null;
            }
        }
        public UWarTopBarGoodsView TopBarGoodsView
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(EGoodsStoreSubView.EGSSV_TopBarGoods))
                    return (m_dicSubView[EGoodsStoreSubView.EGSSV_TopBarGoods] as UWarTopBarGoodsView);
                Trace.LogWarning("未初始化 UWarTopBarGoodsView");
                return null;
            }
        }


        public void InitMobaGoodsScheme(ASpeedGame.Data.GameMobaSchemes.GameMobaSchemesManager.MobaGoodsSchemesEventArgs arg)
        {
            if(AllGoodsView != null)
            {
                AllGoodsView.BoxAllGoods(LogicDataCenter.warMobaGoodsDataManager.WarGoodsStoreMobaGoodsDataSource, arg.RecommendGoods.RecommendGoods_DefaultGoodsTypeList);
                AllGoodsView.BoxGoodsLabelDescriptrion(arg.LabelDescirptionTable);
            }

            int PlayerSummerID = GameLogicAPI.getWarSummerID();

            if (RecommendGoodsView != null)
            {
                for(int i=0;i< arg.RecommendGoods.RecommendGoods_UsedStageList.Count;++i)
                {
                    //2017.9.18新增需求，带惩戒自动切换到打野推荐装
                    if (arg.RecommendGoods.RecommendGoods_UsedStageList[i] == (int)EMobaRecommendGoods.EMRG_EerlyStage)
                    {
                        List<SSchemeMobaGoods> _typeList = PlayerSummerID == arg.RecommendGoods.RecommendGoods_SummerID ? 
                                                                             arg.RecommendGoods.RecommendGoods_SummerEerlyList : 
                                                                             arg.RecommendGoods.RecommendGoods_EerlyList;
                        RecommendGoodsView.BoxRecommendedGoodsByGoodsType(_typeList, arg.RecommendGoods.RecommendGoods_DispositionStageList, EMobaRecommendGoods.EMRG_EerlyStage);
                    }                        
                    if (arg.RecommendGoods.RecommendGoods_UsedStageList[i] == (int)EMobaRecommendGoods.EMRG_MiddleStage)
                    {
                        List<SSchemeMobaGoods> _typeList = PlayerSummerID == arg.RecommendGoods.RecommendGoods_SummerID ?
                                                     arg.RecommendGoods.RecommendGoods_SummerMiddleList :
                                                     arg.RecommendGoods.RecommendGoods_MiddleList;
                        RecommendGoodsView.BoxRecommendedGoodsByGoodsType(_typeList, arg.RecommendGoods.RecommendGoods_DispositionStageList, EMobaRecommendGoods.EMRG_MiddleStage);
                    }
                    if (arg.RecommendGoods.RecommendGoods_UsedStageList[i] == (int)EMobaRecommendGoods.EMRG_DownWindStage)
                    {
                        List<SSchemeMobaGoods> _typeList = PlayerSummerID == arg.RecommendGoods.RecommendGoods_SummerID ?
                             arg.RecommendGoods.RecommendGoods_SummerDownWindList :
                             arg.RecommendGoods.RecommendGoods_DownWindList;
                        RecommendGoodsView.BoxRecommendedGoodsByGoodsType(_typeList, arg.RecommendGoods.RecommendGoods_DispositionStageList, EMobaRecommendGoods.EMRG_DownWindStage);
                    }

                    if (arg.RecommendGoods.RecommendGoods_UsedStageList[i] == (int)EMobaRecommendGoods.EMRG_UpWindStage)
                        RecommendGoodsView.BoxRecommendedGoodsByGoodsType(arg.RecommendGoods.RecommendGoods_UpWindList, arg.RecommendGoods.RecommendGoods_DispositionStageList, EMobaRecommendGoods.EMRG_UpWindStage);

                    if (arg.RecommendGoods.RecommendGoods_UsedStageList[i] == (int)EMobaRecommendGoods.EMRG_ConsumeGoods)
                        RecommendGoodsView.BoxRecommendedGoodsByGoodsType(arg.RecommendGoods.RecommendGoods_ConsumeGoodsList, arg.RecommendGoods.RecommendGoods_DispositionStageList, EMobaRecommendGoods.EMRG_ConsumeGoods);
                }
                //读取配置完后删除重复种类4与2、3重复部分(我也不想，策划为了不让自己配错)                
                SetCurrentGoodsDisposition(WarMobaGoodsDataManager.Instance.HeroDefaultGoodsDisposition);

            }

        }

        private void InitSubView()
        {
            LoadView<UWarRecommendGoodsView>(MobaGoodsFrame, false);
            LoadView<UWarAllGoodsView>(MobaGoodsFrame, false);
            LoadView<UWarDetailGoodsView>(GoodsDetailFrame, true);
            LoadView<UWarCanSynthesizedGoodsView>(CanSynthesizedFrame, true);
            LoadView<UWarTreeSynthesisGoodsView>(TreeSynthesisFrame, true);
            LoadView<UWarPurchasedGoodsView>(PurchasedFrame, true);
            LoadView<UWarSearchResultGoodsView>(SearchResultFrame, false);
            LoadView<UWarDispositionGoodsView>(DispositionFrame, true);
            LoadView<UWarTopBarGoodsView>(TopFrame, true);
        }

        private bool LoadView<T>(Transform _tContainer, bool _bVisible) where T : UWarGoodsStoreSubViewBase
        {

            ResNode viewPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/" + GetSubPrefabPath<T>());
            if (null == viewPrefab)
                return false;

            GameObject instance = viewPrefab.InstanceMainRes();
            if (!instance)
                return false;

            instance.name = viewPrefab.AssetName;
            UWarGoodsStoreSubViewBase subView = instance.GetComponent<T>() as UWarGoodsStoreSubViewBase;
            if (subView == null)
            {
                Debug.LogError("UISystem SubWnd LoadView m_wndView == null  GetPrefabPath=" + GetSubPrefabPath<T>());
                viewPrefab.DestroyInstanceRes(ref instance);
                return false;
            }

            if (subView.Init(this) == false)
            {
                Debug.LogError("UISystem SubWnd LoadView m_wndView.Init == false  GetPrefabPath=" + GetSubPrefabPath<T>());
                viewPrefab.DestroyInstanceRes(ref instance);
                return false;
            }

            subView.GetTransform.SetParent(_tContainer, false);
            subView.SetVisible(_bVisible);

            m_dicSubView.Add(subView.GetSubViewType, subView);
            return true;
        }

        private string GetSubPrefabPath<T>() where T : UWarGoodsStoreSubViewBase
        {
            if (typeof(T) == typeof(UWarRecommendGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarRecommendGoodsView";
            else if (typeof(T) == typeof(UWarAllGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarAllGoodsView";
            else if (typeof(T) == typeof(UWarDetailGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarDetailGoodsView";
            else if (typeof(T) == typeof(UWarCanSynthesizedGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarCanSynthesizedGoodsView";
            else if (typeof(T) == typeof(UWarTreeSynthesisGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarTreeSynthesisGoodsView";
            else if (typeof(T) == typeof(UWarPurchasedGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarPurchasedGoodsView";
            else if (typeof(T) == typeof(UWarSearchResultGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarSearchResultGoodsView";
            else if (typeof(T) == typeof(UWarDispositionGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarDispositionGoodsView";
            else if(typeof(T) == typeof(UWarTopBarGoodsView))
                return UPath.UDynamicMobaState + "/WarGoodsStore/WarTopBarGoodsView";

            Trace.LogError("类型:" + typeof(T) + "没有对应的预制体类型");
            return "";
        }
        #endregion

        #region 物品界面所有物品数据部分


        /// <summary>
        /// 定时器触发，一定时间将有属性改变的物品通知所有子窗体处理相应操作
        /// </summary>
        protected void OnTimerSendSubViewProtertyChangeItem()
        {
            if (LogicDataCenter.warMobaGoodsDataManager.GoodsItemPropertyChange.Count > 0)
            {
                if (m_wnd.IsVisible())
                {
                    RefreshSubView();

                    LogicDataCenter.warMobaGoodsDataManager.GoodsItemPropertyChange.Clear();
                }
            }

        }

        protected void RefreshSubView(EGoodsStoreSubView? _subView = null)
        {
            if (_subView == null)
            {//刷新所有

                onMobaGoodsDataSourceChange.Invoke(new UWarGoodsStoreDataSourceChangeParam(LogicDataCenter.warMobaGoodsDataManager.GoodsItemPropertyChange));
            }
            else
            {//刷新指定的
                EGoodsStoreSubView type = (EGoodsStoreSubView)_subView;
                m_dicSubView[type].OnMobaGoodsDataSourceChange(new UWarGoodsStoreDataSourceChangeParam(LogicDataCenter.warMobaGoodsDataManager.GoodsItemPropertyChange));
            }
        }
        #endregion


        private static Color NoneIconColor = new Color(1, 1, 1, 0.6f);
        private static Color IconColor = new Color(1, 1, 1, 1);
        public static void GetWarGoodsIcon(int _nGoodsIconID, Image _nGoodsIcon = null)
        {
            if (_nGoodsIconID == 0 || _nGoodsIcon == null)
            {
                if (_nGoodsIcon != null)
                {
                    _nGoodsIcon.sprite = USpriteManager.Instance.DefaultSprite;
                }
                return;
            }

            _nGoodsIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_GOODSSTORE, UWarGoodsStoreView.MobaGoodsIconSize, _nGoodsIconID);
        }
        public static Sprite GetWarGoodsIcon(int _nGoodsIconID)
        {
            if (_nGoodsIconID == 0)
            {
                return null;
            }

            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_GOODSSTORE, UWarGoodsStoreView.MobaGoodsIconSize, _nGoodsIconID);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EWarGoodsStoreTimerHandler.ETH_ProtertyChange:
                    OnTimerSendSubViewProtertyChangeItem();
                    break;
                default:
                    break;
            }
        }
    }
}

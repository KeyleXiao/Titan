using DataCenter;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UIWidgets;
using System.Collections;
using ASpeedGame.Data.GameMobaSchemes;

namespace USpeedUI.GoodsDisposition
{
    #region 子界面类型
    /// <summary>
    /// 一级子界面
    /// </summary>
    public enum ELevel1_SubView
    {
        CustomGoodsFrame,
    }

    /// <summary>
    /// 二级子界面
    /// </summary>
    public enum ELevel2_SubView
    {
        GoodsLabel,
        GoodsList,
        GoodsCanSynthesis,//目前显示部分已合并到GoodsSynthesis子界面内
        GoodsSynthesisTree,//目前显示部分已合并到GoodsSynthesis子界面内
        GoodsDetail,
        GoodsDisposition,
        GoodsDispositionList,
        HeroHeadPortrait,
        HeroHeadPortraitList,
        EditGoodsDispositionInfo,
        GoodsDragDetail,

        GoodsSynthesis,
        GoodsDispositionControl,
        DispositionFilled,
    }

    /// <summary>
    /// 一级子界面基类
    /// </summary>
    public class ULevel1_SubViewBase : UCustomUIBehaviour
    {
        protected UGoodsDispositionView m_MainView = null;
        protected Dictionary<ELevel2_SubView, ULevel2_SubViewBase> m_dicSubView = new Dictionary<ELevel2_SubView, ULevel2_SubViewBase>();

        internal virtual bool Init(UGoodsDispositionView _view)
        {
            m_MainView = _view;

            InitSubView();

            return true;
        }

        public virtual ELevel1_SubView GetSubViewType
        {
            get
            {
                throw new NotImplementedException();
            }

        }

        internal virtual void SetVisible(bool visible)
        {
            gameObject.SetActive(visible);
        }

        protected virtual void InitSubView()
        { }

        protected virtual bool LoadView<T>(Transform _tContainer, bool _bVisible) where T : ULevel2_SubViewBase
        {

            ResNode viewPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/" + GetSubPrefabPath<T>());
            if (null == viewPrefab)
                return false;

            GameObject instance = viewPrefab.InstanceMainRes();
            if (!instance)
                return false;

            instance.name = viewPrefab.AssetName;
            ULevel2_SubViewBase subView = instance.GetComponent<T>() as ULevel2_SubViewBase;
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

        protected virtual string GetSubPrefabPath<T>() where T : ULevel2_SubViewBase
        {
            return string.Empty;
        }

        public virtual bool OnCloseKey()
        {
            return true;
        }
        public virtual bool HaveEscCallBack()
        {
            return false;
        }
    }

    /// <summary>
    /// 二级子界面基类
    /// </summary>
    public class ULevel2_SubViewBase : UCustomUIBehaviour
    {
        protected ULevel1_SubViewBase m_parentView = null;
        protected bool m_bPopupView = false;

        public virtual ELevel2_SubView GetSubViewType
        {
            get
            {
                throw new NotImplementedException();
            }

        }

        internal virtual bool Init(ULevel1_SubViewBase _view)
        {
            m_parentView = _view;

            return true;
        }

        internal virtual void SetVisible(bool visible)
        {
            gameObject.SetActive(visible);
            if(visible && m_bPopupView)
            {
                GetTransform.SetAsLastSibling();
            }
        }

        internal virtual bool IsVisible()
        {
            return gameObject.activeSelf;
        }

        protected T ParentTo<T>() where T : ULevel1_SubViewBase
        {
            if (m_parentView is T)
                return m_parentView as T;
            return null;
        }
    }
    #endregion



    public class UGoodsDispositionView : UIBaseWndView
    {
        public Transform SubView_Container = null;
        public Button CloseBtn = null;

        private UGoodsDispositionWnd m_GoodDispositionWnd { get { return m_wnd as UGoodsDispositionWnd; } }


        public override bool Init(IUIWnd wnd)
        {
            bool result = base.Init(wnd);

            m_UseImageEffect = true;

            InitSubView();

            m_GoodDispositionWnd.InitGoodsScheme();
            m_GoodDispositionWnd.InitHeroHeadPortraitListHeroTypeItem();


            if(CloseBtn != null)
            {
                CloseBtn.onClick.AddListener(OnCloseBtnClick);
            }
            return result;
        }

        protected void OnCloseBtnClick()
        {
            m_GoodDispositionWnd.SetVisible(false);
        }


        public bool OnCloseKey()
        {
            //有其他一级子界面要处理时这里做判断
            if (SubView_CustomGoodsFrame == null)
                return true;

            return (SubView_CustomGoodsFrame).OnCloseKey();
        }

        public bool HaveEscCallBack()
        {
            //有其他一级子界面要处理时这里做判断
            if (SubView_CustomGoodsFrame == null)
                return false;

            return (SubView_CustomGoodsFrame).HaveEscCallBack();

        }

        #region 处理1级子界面

        internal UGD_CustomGoodsFrame SubView_CustomGoodsFrame
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel1_SubView.CustomGoodsFrame))
                    return (m_dicSubView[ELevel1_SubView.CustomGoodsFrame] as UGD_CustomGoodsFrame);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame");
                return null;
            }
        }
        /// <summary>
        /// 初始化物品列表
        /// </summary>
        /// <param name="_labelTable">物品类型</param>
        /// <param name="_SchemeMobaGoodsDataSource">物品</param>
        public void InitGoodsScheme(Hashtable _labelTable, ObservableList<SSchemeMobaGoods> _SchemeMobaGoodsDataSource)
        {
            if (SubView_CustomGoodsFrame != null)
            {
                SubView_CustomGoodsFrame.InitGoodsScheme(_labelTable, _SchemeMobaGoodsDataSource);
            }
        }
        /// <summary>
        /// 初始化英雄头像类型列表
        /// </summary>
        /// <param name="_heroType"></param>
        public void InitHeroHeadPortraitListHeroTypeItem(List<string> _heroType)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.InitHeroHeadPortraitListHeroTypeItem(_heroType);
        }
        /// <summary>
        /// 当前显示的英雄头像列表
        /// </summary>
        /// <param name="_heroDataSource"></param>
        public void SetPlayerHeroPortraitItem(ObservableList<UGD_HeroHeadPortraitItem> _heroDataSource)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.SetPlayerHeroPortraitItem(_heroDataSource);
        }
        /// <summary>
        /// 当前配置中的英雄
        /// </summary>
        /// <param name="_nHeroID"></param>
        public void SetCurrentHero(int _nHeroID)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.SetCurrentHero(_nHeroID);
        }
        /// <summary>
        /// 当前英雄物品配置列表
        /// </summary>
        /// <param name="_goodsDispositionDataSource"></param>
        public void SetGoodsDispositionListData(ObservableList<UGD_GoodsDispositionItem> _goodsDispositionDataSource)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.SetGoodsDispositionListData(_goodsDispositionDataSource);
        }
        /// <summary>
        /// 当前物品配置
        /// </summary>
        /// <param name="_nCurrentGoodsDisposition"></param>
        public void SetCurrentGoodsDisposition(UGD_GoodsDispositionItem _nCurrentGoodsDisposition)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.SetCurrentGoodsDisposition(_nCurrentGoodsDisposition);
        }
        /// <summary>
        /// 关闭物品配置列表
        /// </summary>
        public void CloseGoodsDispositionList()
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.CloseGoodsDispositionList();

        }
        /// <summary>
        /// 当前选择的物品
        /// </summary>
        /// <param name="_nCurrentSelectedGoods"></param>
        public void SetCurrentSelectedGoods(int _nCurrentSelectedGoods)
        {
            if (SubView_CustomGoodsFrame != null)
            {
                SubView_CustomGoodsFrame.SetCurrentSelectedGoods(_nCurrentSelectedGoods);
            }
        }
        /// <summary>
        /// 打开物品合成界面
        /// </summary>
        public void OpenGoodsSynthesisView(int _nCurrentSelectedGoods)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.OpenGoodsSynthesisView(_nCurrentSelectedGoods);
        }
        /// <summary>
        /// 打开物品合成界面
        /// </summary>
        public void OpenDispositionFilled(UGD_DispositionFilledMsgData _msgData)
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.OpenDispositionFilled(_msgData);
        }
        public void UpdateDispositionFilled()
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.UpdateDispositionFilled();
        }
        /// <summary>
        /// 打开物品复制操作
        /// </summary>
        public void OpenDispositionCopy()
        {
            if (SubView_CustomGoodsFrame != null)
                SubView_CustomGoodsFrame.OpenDispositionCopy();
        }
        #region 物品拖拽处理
        public void GoodsBeginDrag(int _nDragGoodsID)
        {
            if (SubView_CustomGoodsFrame == null)
                return;
            SubView_CustomGoodsFrame.GoodsBeginDrag(_nDragGoodsID);
        }
        public void GoodsDragging(int _nDragGoodsID)
        {
            if (SubView_CustomGoodsFrame == null)
                return;
            SubView_CustomGoodsFrame.GoodsDragging(_nDragGoodsID);
        }
        public void GoodsEndDrag(int _nDragGoodsID)
        {
            if (SubView_CustomGoodsFrame == null)
                return;
            SubView_CustomGoodsFrame.GoodsEndDrag(_nDragGoodsID);
        }
        #endregion
        #endregion

        internal Dictionary<ELevel1_SubView, ULevel1_SubViewBase> m_dicSubView = new Dictionary<ELevel1_SubView, ULevel1_SubViewBase>();

        protected void InitSubView()
        {
            LoadView<UGD_CustomGoodsFrame>(SubView_Container, true);
        }

        internal bool LoadView<T>(Transform _tContainer, bool _bVisible) where T : ULevel1_SubViewBase
        {

            ResNode viewPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/" + GetSubPrefabPath<T>());
            if (null == viewPrefab)
                return false;

            GameObject instance = viewPrefab.InstanceMainRes();
            if (!instance)
                return false;

            instance.name = viewPrefab.AssetName;
            ULevel1_SubViewBase subView = instance.GetComponent<T>() as ULevel1_SubViewBase;
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

        internal string GetSubPrefabPath<T>() where T : ULevel1_SubViewBase
        {
            if (typeof(T) == typeof(UGD_CustomGoodsFrame))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_1_CustomGoodsFrame";
            //else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsList))
            //    return UPath.UDynamicMobaState + "/GoodsDisposition/SubView_GoodsList";

            Trace.LogError("类型:" + typeof(T) + "没有对应的预制体类型");
            return "";
        }


        #region 当前界面内部子界面会用的方法
        //获取物品图标形状类型
        public const int GoodsIconSize = 1;


        public static void GetGoodsIcon(int _nGoodsIconID, Image _nGoodsIcon = null)
        {
            if (_nGoodsIconID == 0 || _nGoodsIcon == null)
            {
                if (_nGoodsIcon != null)
                {
                    _nGoodsIcon.sprite = USpriteManager.Instance.DefaultSprite;
                }
                return;
            }

            _nGoodsIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_GOODSDISPOSITION, UGoodsDispositionView.GoodsIconSize, _nGoodsIconID);
        }

        public static Sprite GetGoodsIcon(int _nGoodsIconID)
        {
            if (_nGoodsIconID == 0)
            {
                return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_GOODSDISPOSITION, UGoodsDispositionView.GoodsIconSize, 0);
            }

            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_GOODSDISPOSITION, UGoodsDispositionView.GoodsIconSize, _nGoodsIconID);
        }
        public static Sprite GetGoodsIconByGoodsID(int _nGoodsID)
        {
            if (_nGoodsID == 0)
            {
                return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_GOODSDISPOSITION, UGoodsDispositionView.GoodsIconSize, 0);
            }
            int _nGoodsIconID = GoodsSchemesDataManager.Instance.GetGoodsIconID(_nGoodsID);
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_GOODSDISPOSITION, UGoodsDispositionView.GoodsIconSize, _nGoodsIconID);
        }

        public static Sprite GetHeroHeadPortrait(int _nHeadID)
        {
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GOODSDISPOSITION, 1, 3, _nHeadID);
        }
        #endregion
    }


    class GoodsDispositionViewTest : MonoBehaviour
    {
        UGoodsDispositionWnd wnd = null;
        public UGoodsDispositionView view = null;
        public int heroID = 2;
        public bool test = false;

        void Start()
        {
            //AssetBundleManager.Init();

            //ViewConfigManager.Init();

            //GoodsDispositionDataManager.Instance.Init();


            //wnd = new UGoodsDispositionWnd();
            //wnd.Init();

            //wnd.SetView(view);
            //view.Init(wnd);

        }

        void Update()
        {
            if(test)
            {
                test = false;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, heroID, string.Empty, IntPtr.Zero, 0));

            }
        }


    }
}

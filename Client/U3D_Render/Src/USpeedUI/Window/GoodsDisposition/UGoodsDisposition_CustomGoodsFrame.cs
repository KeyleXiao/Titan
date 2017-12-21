using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.WarGoodsStore;
using UIWidgets;
using USpeedUI.UExtensions;
using System.Collections;
using UnityEngine.EventSystems;
using U3D_Render.Common;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine.Events;
using DataCenter;

namespace USpeedUI.GoodsDisposition
{
    public class UGD_CustomGoodsFrame : ULevel1_SubViewBase
    {
        public class UGoodsTypeClickEvent : UnityEvent<int> { }
        public class UGoodsItemClickEvent : UnityEvent<SSchemeMobaGoods> { }
        public class UChangeHeroClickEvent : UnityEvent { }
        public class UHeroHeadHeadPortraitClickEvent : UnityEvent<int> { }
        public class UGoodsDispositionClickEvent : UnityEvent<int> { }
        public class UChangeGoodsDispositionClickEvent : UnityEvent { }
        public class UEditGoodsDispositionInfoClickEvent : UnityEvent<string, UnityAction<string>, UnityAction<string>> { }

        /// <summary> 物品类型标签点击事件 </summary>
        public UGoodsTypeClickEvent onGoodsTypeClickEvent = null;
        /// <summary> 物品点击事件 </summary>
        public UGoodsItemClickEvent onGoodsItemClickEvent = null;
        /// <summary> 切换英雄点击事件 </summary>
        public UChangeHeroClickEvent onChangeHeroClickEvent = null;
        /// <summary> 装备配置页点击事件 </summary>
        public UGoodsDispositionClickEvent onGoodsDispositionClickEvent = null;
        /// <summary> 切换装备配置页点击事件 </summary>
        public UChangeGoodsDispositionClickEvent onChangeGoodsDispositionClickEvent = null;
        /// <summary> 
        /// 文本输入弹框
        /// <para>参数1：标题</para>
        /// <para>参数2：确定按钮回调，参数是输入的文本</para>
        /// <para>参数3：确定按钮回调，参数是输入的文本</para>
        /// </summary>
        public UEditGoodsDispositionInfoClickEvent onEditGoodsDispositionInfoClickEvent = null;


        internal override bool Init(UGoodsDispositionView _view)
        {
            AddEvent();

            bool result = base.Init(_view);
            return result;
        }

        #region 初始化子界面
        public Transform GoodsLabelContainer = null;
        public Transform GoodsListContainer = null;
        public Transform GoodsCanSynthesisContainer = null;
        public Transform GoodsSynthesisTreeContainer = null;
        public Transform GoodsDetailContainer = null;
        public Transform HeroHeadPortraitContainer = null;
        public Transform PopupContainer = null;
        public Transform GoodsDispositionContainer = null;
        public Transform DispositionControlContainer = null;

        internal UGD_CustomGoodsFrame_GoodsLabel SubView_GoodsLabel
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsLabel))
                    return (m_dicSubView[ELevel2_SubView.GoodsLabel] as UGD_CustomGoodsFrame_GoodsLabel);
                Trace.LogWarning("未初始化 UGD_SubView_GoodsLabel");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsList SubView_GoodsList
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsList))
                    return (m_dicSubView[ELevel2_SubView.GoodsList] as UGD_CustomGoodsFrame_GoodsList);
                Trace.LogWarning("未初始化 UGD_SubView_GoodsList");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GooodsCanSynthesized SubView_GoodsCanSynthesized
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsCanSynthesis))
                    return (m_dicSubView[ELevel2_SubView.GoodsCanSynthesis] as UGD_CustomGoodsFrame_GooodsCanSynthesized);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_GooodsCanSynthesized");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsSynthesisTree SubView_GoodsSynthesisTree
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsSynthesisTree))
                    return (m_dicSubView[ELevel2_SubView.GoodsSynthesisTree] as UGD_CustomGoodsFrame_GoodsSynthesisTree);
                Trace.LogWarning("未初始化 UGD_SubView_GoodsSynthesisTree");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsDetail SubView_GoodsDetail
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsDetail))
                    return (m_dicSubView[ELevel2_SubView.GoodsDetail] as UGD_CustomGoodsFrame_GoodsDetail);
                Trace.LogWarning("未初始化 UGD_SubView_GoodsDetail");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_HeroHeadPortrait SubView_HeroHeadPortrait
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.HeroHeadPortrait))
                    return (m_dicSubView[ELevel2_SubView.HeroHeadPortrait] as UGD_CustomGoodsFrame_HeroHeadPortrait);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_HeroHeadPortrait");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsDisposition SubView_GoodsDisposition
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsDisposition))
                    return (m_dicSubView[ELevel2_SubView.GoodsDisposition] as UGD_CustomGoodsFrame_GoodsDisposition);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_GoodsDisposition");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_HeroHeadPortraitList SubView_HeroHeadPortraitList
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.HeroHeadPortraitList))
                    return (m_dicSubView[ELevel2_SubView.HeroHeadPortraitList] as UGD_CustomGoodsFrame_HeroHeadPortraitList);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_HeroHeadPortraitList");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsDispositionList SubView_GoodsDispositionList
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsDispositionList))
                    return (m_dicSubView[ELevel2_SubView.GoodsDispositionList] as UGD_CustomGoodsFrame_GoodsDispositionList);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_GoodsDispositionList");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_EditGoodsDispositionInfo SubView_EditGoodsDispositionInfo
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.EditGoodsDispositionInfo))
                    return (m_dicSubView[ELevel2_SubView.EditGoodsDispositionInfo] as UGD_CustomGoodsFrame_EditGoodsDispositionInfo);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_EditGoodsDispositionInfo");
                return null;
            }

        }
        internal UGD_CustomGoodsFrame_GoodsDragDetail SubView_GoodsDragDetail
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsDragDetail))
                    return (m_dicSubView[ELevel2_SubView.GoodsDragDetail] as UGD_CustomGoodsFrame_GoodsDragDetail);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_GoodsDragDetail");
                return null;
            }

        }
        internal UGD_CustomGoodsFrame_GoodsSynthesis SubView_GoodsSynthesis
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsSynthesis))
                    return (m_dicSubView[ELevel2_SubView.GoodsSynthesis] as UGD_CustomGoodsFrame_GoodsSynthesis);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_GoodsSynthesis");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_GoodsDispositionControl SubView_DispositionControl
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.GoodsDispositionControl))
                    return (m_dicSubView[ELevel2_SubView.GoodsDispositionControl] as UGD_CustomGoodsFrame_GoodsDispositionControl);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_DispositionControl");
                return null;
            }
        }
        internal UGD_CustomGoodsFrame_DispositionFilled SubView_DispositionFilled
        {
            get
            {
                if (m_dicSubView != null && m_dicSubView.ContainsKey(ELevel2_SubView.DispositionFilled))
                    return (m_dicSubView[ELevel2_SubView.DispositionFilled] as UGD_CustomGoodsFrame_DispositionFilled);
                Trace.LogWarning("未初始化 UGD_CustomGoodsFrame_DispositionFilled");
                return null;
            }

        }
        public override ELevel1_SubView GetSubViewType
        {
            get
            {
                return ELevel1_SubView.CustomGoodsFrame;
            }
        }


        protected void AddEvent()
        {
            onGoodsTypeClickEvent = new UGoodsTypeClickEvent();
            //onGoodsItemClickEvent = new UGoodsItemClickEvent();
            onChangeHeroClickEvent = new UChangeHeroClickEvent();
            onGoodsDispositionClickEvent = new UGoodsDispositionClickEvent();
            onChangeGoodsDispositionClickEvent = new UChangeGoodsDispositionClickEvent();
            onEditGoodsDispositionInfoClickEvent = new UEditGoodsDispositionInfoClickEvent();
        }

        protected void RemoveEvent()
        {
            _removeEvent(onGoodsTypeClickEvent);
            //_removeEvent(onGoodsItemClickEvent);
            _removeEvent(onChangeHeroClickEvent);
            _removeEvent(onGoodsDispositionClickEvent);
            _removeEvent(onChangeGoodsDispositionClickEvent);
            _removeEvent(onEditGoodsDispositionInfoClickEvent);
        }

        private void _removeEvent(UnityEventBase _event)
        {
            if (_event != null)
            {
                _event.RemoveAllListeners();
                _event = null;
            }
        }

        protected override void OnDestroy()
        {
            RemoveEvent();
            base.OnDestroy();
        }

        protected override void InitSubView()
        {
            LoadView<UGD_CustomGoodsFrame_GoodsLabel>(GoodsLabelContainer, true);
            LoadView<UGD_CustomGoodsFrame_GoodsList>(GoodsListContainer, true);
            //LoadView<UGD_CustomGoodsFrame_GooodsCanSynthesized>(GoodsCanSynthesisContainer, true);
            //LoadView<UGD_CustomGoodsFrame_GoodsSynthesisTree>(GoodsSynthesisTreeContainer, true);
            LoadView<UGD_CustomGoodsFrame_GoodsDetail>(GoodsCanSynthesisContainer, true);
            LoadView<UGD_CustomGoodsFrame_HeroHeadPortrait>(HeroHeadPortraitContainer, true);
            LoadView<UGD_CustomGoodsFrame_GoodsDisposition>(GoodsDispositionContainer, true);
            LoadView<UGD_CustomGoodsFrame_GoodsDispositionControl>(DispositionControlContainer, true);

            LoadView<UGD_CustomGoodsFrame_GoodsDragDetail>(GetRectTransform, false);

            //弹框类型，加载顺序跟显示顺序有关
            LoadView<UGD_CustomGoodsFrame_HeroHeadPortraitList>(PopupContainer, false);
            LoadView<UGD_CustomGoodsFrame_GoodsDispositionList>(PopupContainer, false);
            LoadView<UGD_CustomGoodsFrame_EditGoodsDispositionInfo>(PopupContainer, false);
            LoadView<UGD_CustomGoodsFrame_GoodsSynthesis>(PopupContainer, false);
            LoadView<UGD_CustomGoodsFrame_DispositionFilled>(PopupContainer, false);
        }

        protected override string GetSubPrefabPath<T>()
        {
            if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsLabel))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsLabel";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsList))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsList";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GooodsCanSynthesized))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GooodsCanSynthesized";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsSynthesisTree))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsSynthesisTree";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsDetail))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDetail";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_HeroHeadPortrait))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_HeroHeadPortrait";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_HeroHeadPortraitList))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_HeroHeadPortraitList";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsDispositionList))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDispositionList";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsDisposition))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDisposition";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_EditGoodsDispositionInfo))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_EditGoodsDispositionInfo";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsDragDetail))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDragDetail";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsSynthesis))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsSynthesis";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_GoodsDispositionControl))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDispositionControl";
            else if (typeof(T) == typeof(UGD_CustomGoodsFrame_DispositionFilled))
                return UPath.UStaticState + "/GoodsDisposition/GD_SL_2_DispositionFilled";

            Trace.LogError("类型:" + typeof(T) + "没有对应的预制体类型");
            return string.Empty;
        }
        #endregion

        #region 物品拖拽处理
        public void GoodsBeginDrag(int _nDragGoodsID)
        {
            if (SubView_GoodsDragDetail != null)
            {
                SubView_GoodsDragDetail.OnBeginDrag(_nDragGoodsID);
            }
        }
        public void GoodsDragging(int _nDragGoodsID)
        {
            if (SubView_GoodsDragDetail != null)
            {
                SubView_GoodsDragDetail.OnDragging(_nDragGoodsID);
            }
        }
        public void GoodsEndDrag(int _nDragGoodsID)
        {
            if (SubView_GoodsDragDetail != null)
            {
                SubView_GoodsDragDetail.OnEndDrag(_nDragGoodsID);
            }
        }
        #endregion

        /// <summary>
        /// 接收当前选择的物品消息
        /// </summary>
        /// <param name="_nCurrentSelectedGoods"></param>
        public void SetCurrentSelectedGoods(int _nCurrentSelectedGoods)
        {
            SSchemeMobaGoods goodsSScheme = null;
            if (GoodsSchemesDataManager.Instance.GetMobaGoods_Out(_nCurrentSelectedGoods, out goodsSScheme))
            {
                if (SubView_GoodsList != null)
                    SubView_GoodsList.OnGoodsItemClickEvent(goodsSScheme);
                if (SubView_GoodsDetail != null && !SubView_GoodsSynthesis.IsVisible())
                    SubView_GoodsDetail.OnGoodsItemClickEvent(goodsSScheme);
                //if (SubView_GoodsSynthesisTree != null)
                //    SubView_GoodsSynthesisTree.OnGoodsItemClickEvent(goodsSScheme);
                //if (SubView_GoodsCanSynthesized != null)
                //    SubView_GoodsCanSynthesized.OnGoodsItemClickEvent(goodsSScheme);
                if (SubView_GoodsSynthesis != null)
                    SubView_GoodsSynthesis.OnGoodsItemClickEvent(goodsSScheme);
            }
        }
        public void SetCurrentHero(int _nHeroID)
        {
            if (SubView_HeroHeadPortrait != null)
                SubView_HeroHeadPortrait.OnHeroHeadPortraitClickEvent(_nHeroID);
        }
        public void SetPlayerHeroPortraitItem(ObservableList<UGD_HeroHeadPortraitItem> _heroDataSource)
        {
            if (SubView_HeroHeadPortraitList != null)
                SubView_HeroHeadPortraitList.SetPlayerHeroDataSource(_heroDataSource);
        }
        public void InitGoodsScheme(Hashtable _labelTable, ObservableList<SSchemeMobaGoods> _SchemeMobaGoodsDataSource)
        {
            if (SubView_GoodsLabel != null)
            {
                SubView_GoodsLabel.BoxGoodsLabelDescriptrion(_labelTable);
            }

            if (SubView_GoodsList != null)
            {
                SubView_GoodsList.BoxAllGoods(_SchemeMobaGoodsDataSource, new List<int>() { 2 });
            }
        }
        public void InitHeroHeadPortraitListHeroTypeItem(List<string> _heroType)
        {
            if (SubView_HeroHeadPortraitList != null)
                SubView_HeroHeadPortraitList.SetHeadLineItemData(_heroType);
        }
        public void SetGoodsDispositionListData(ObservableList<UGD_GoodsDispositionItem> _goodsDispositionDataSource)
        {
            if (SubView_GoodsDispositionList != null)
                SubView_GoodsDispositionList.SetGoodsDispositionListData(_goodsDispositionDataSource);
        }
        public void SetCurrentGoodsDisposition(UGD_GoodsDispositionItem _nCurrentGoodsDisposition)
        {
            if (SubView_GoodsDisposition != null)
                SubView_GoodsDisposition.SetCurrentGoodsDisposition(_nCurrentGoodsDisposition);

        }
        public void CloseGoodsDispositionList()
        {
            if (SubView_GoodsDispositionList != null)
                SubView_GoodsDispositionList.SetVisible(false);

        }
        public void OpenGoodsSynthesisView(int _nGoodsSelectedGoods)
        {
            if (SubView_GoodsSynthesis != null)
            {
                SSchemeMobaGoods goodsSScheme = null;

                if (GoodsSchemesDataManager.Instance.GetMobaGoods_Out(_nGoodsSelectedGoods, out goodsSScheme))
                {
                    if (SubView_GoodsSynthesis != null)
                        SubView_GoodsSynthesis.OpenGoodsSynthesis(goodsSScheme);
                }
            }
        }
        public void OpenDispositionFilled(UGD_DispositionFilledMsgData _msgData)
        {
            if (SubView_DispositionFilled != null)
            {
                SubView_DispositionFilled.OpenDispositionFilled(_msgData);
            }
        }
        public void UpdateDispositionFilled()
        {
            if (SubView_DispositionFilled != null)
            {
                SubView_DispositionFilled.UpdateDispositionFilled();
            }
        }
        public void OpenDispositionCopy()
        {
            if (SubView_HeroHeadPortraitList != null)
            {
                SubView_HeroHeadPortraitList.DispositionCopy();
            }
        }

        public override bool HaveEscCallBack()
        {
            if (SubView_HeroHeadPortraitList == null || SubView_GoodsDispositionList == null || SubView_EditGoodsDispositionInfo == null || SubView_GoodsSynthesis == null)
                return base.HaveEscCallBack();

            for (int i = PopupContainer.childCount - 1; i >= 0; i--)
            {
                Transform child = PopupContainer.GetChild(i);
                ULevel2_SubViewBase subView = child.GetComponent<ULevel2_SubViewBase>();
                if (child.gameObject.activeSelf && subView != null && subView.IsVisible())
                {
                    subView.SetVisible(false);
                    return true;
                }

            }


            //if (SubView_HeroHeadPortraitList.IsVisible())
            //{
            //    SubView_HeroHeadPortraitList.SetVisible(false);
            //    return true;
            //}
            //if (SubView_GoodsDispositionList.IsVisible())
            //{
            //    SubView_GoodsDispositionList.SetVisible(false);
            //    return true;
            //}
            //if (SubView_EditGoodsDispositionInfo.IsVisible())
            //{
            //    SubView_EditGoodsDispositionInfo.SetVisible(false);
            //    return true;
            //}
            //if (SubView_GoodsSynthesis.IsVisible())
            //{
            //    SubView_GoodsSynthesis.SetVisible(false);
            //    return true;
            //}

            return base.HaveEscCallBack();
        }

        public override bool OnCloseKey()
        {
            if (SubView_HeroHeadPortraitList == null || SubView_GoodsDispositionList == null || SubView_EditGoodsDispositionInfo == null || SubView_GoodsSynthesis == null)
                return base.OnCloseKey();

            if (SubView_HeroHeadPortraitList.IsVisible())
            {
                SubView_HeroHeadPortraitList.SetVisible(false);
                return false;
            }
            if (SubView_GoodsDispositionList.IsVisible())
            {
                SubView_GoodsDispositionList.SetVisible(false);
                return false;
            }
            if (SubView_EditGoodsDispositionInfo.IsVisible())
            {
                SubView_EditGoodsDispositionInfo.SetVisible(false);
                return false;
            }
            if (SubView_GoodsSynthesis.IsVisible())
            {
                SubView_GoodsSynthesis.SetVisible(false);
                return false;
            }
            return base.OnCloseKey();
        }
    }

    public class UGD_CustomGoodsFrame_GoodsLabel : ULevel2_SubViewBase
    {
        public UWarGoodsStoreTreeView GoodsLabelDesTree = null;

        private int m_OldSelectedTypeIndex = -1;
        private int m_SelectedTypeClickCount = 0;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsLabel;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            GoodsLabelDesTree.Multiple = false;
            GoodsLabelDesTree.KeepSelection = true;

            return base.Init(_view);
        }


        private TreeNode<UWarGoodsStoreTreeComponentData> Data2TreeNode(string name, int depth, int id)
        {
            var newItem = new UWarGoodsStoreTreeComponentData(name, null);
            newItem.Depth = depth;
            newItem.GoodsLabelID = id;
            var newNode = new TreeNode<UWarGoodsStoreTreeComponentData>(newItem, new ObservableList<TreeNode<UWarGoodsStoreTreeComponentData>>());
            return newNode;
        }

        public void BoxGoodsLabelDescriptrion(Hashtable _labelDescriptionTable)
        {
            //if (m_LabelDesListDataSource != null)
            //{
            //    Trace.LogWarning("商店物品标签已经初始化了，还要初始化？");
            //    return;
            //}
            //m_LabelDesListDataSource = new ObservableList<UWarGoodsLabelDesItem>();


            var treeNodes = new ObservableList<TreeNode<UWarGoodsStoreTreeComponentData>>();

            Dictionary<int, List<int>> LabelAndSubLabelDic = new Dictionary<int, List<int>>();

            //根据配置信息建立物品标签二维列表
            foreach (int labelKey in _labelDescriptionTable.Keys)
            {
                if (labelKey == 0)
                    continue;

                int Dickey = labelKey / 1000;

                //大于0深度为1，小于0深度为2
                if (Dickey > 0)
                {
                    if (!LabelAndSubLabelDic.ContainsKey(Dickey))
                    {
                        LabelAndSubLabelDic[Dickey] = new List<int>();
                    }
                    LabelAndSubLabelDic[Dickey].Add(labelKey);
                }
                else
                {
                    if (!LabelAndSubLabelDic.ContainsKey(labelKey))
                        LabelAndSubLabelDic[labelKey] = new List<int>();
                }
            }
            //排下序
            foreach (int key in LabelAndSubLabelDic.Keys)
            {
                LabelAndSubLabelDic[key].Sort();
            }
            var LabelAndSubLabelDicOrder = from keyValue in LabelAndSubLabelDic orderby keyValue.Key select keyValue;


            //遍历物品标签二维列表
            foreach (var keyValuePair in LabelAndSubLabelDicOrder)
            {

                string Label = Convert.ToString(_labelDescriptionTable[keyValuePair.Key]);
                UBB.toHtml(ref Label);


                var node0 = Data2TreeNode(Label, 0, keyValuePair.Key);
                treeNodes.Add(node0);

                foreach (int subKey in keyValuePair.Value)
                {
                    string SubLabel = Convert.ToString(_labelDescriptionTable[subKey]);

                    var node1 = Data2TreeNode(SubLabel, 1, subKey);
                    node0.Nodes.Add(node1);
                }
            }

            //GoodsLabelDesList.DataSource = m_LabelDesListDataSource;
            //GoodsLabelDesList.OnSelect.AddListener(OnGoodsLabelSelected);
            if (GoodsLabelDesTree != null)
            {
                GoodsLabelDesTree.Nodes = treeNodes;
                GoodsLabelDesTree.OnSelect.AddListener(OnGoodsLabelSelected);
                GoodsLabelDesTree.NodeToggle.AddListener(LabelDesNodeToggleEvent);
            }
        }

        private void LabelDesNodeToggleEvent(TreeNode<UWarGoodsStoreTreeComponentData> arg0)
        {
            //阻止事件冒泡，就不会执行OnGoodsLabelSelected这个方法
            UnityEngine.EventSystems.EventSystem.current.CurrentPointerEventData().CanBubbleEvent = false;
        }

        private void OnGoodsLabelSelected(int _nIndex, ListViewItem _itemComponent)
        {//因为列表没开多选，所以Selected和DeSelected都会响应OnSelect事件
            //所以根据响应次数，判断执行方式
            m_SelectedTypeClickCount++;

            if (GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID == -1)
            {
                ParentTo<UGD_CustomGoodsFrame>().onGoodsTypeClickEvent.Invoke(-1);
                //(m_parentView as UGD_CustomGoodsFrame).onGoodsTypeClickEvent.Invoke(-1);
                //SetAllGoodsTileDataSource(m_filterDefaultTypeList);
                return;
            }

            if (_nIndex != m_OldSelectedTypeIndex)
            {
                m_OldSelectedTypeIndex = _nIndex;
                m_SelectedTypeClickCount = 0;
                ParentTo<UGD_CustomGoodsFrame>().onGoodsTypeClickEvent.Invoke(GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID);
                //SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem
                //    (GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID.ToString()));
                return;
            }
        }


    }

    public class UGD_CustomGoodsFrame_GoodsList : ULevel2_SubViewBase
    {
        //大件物品列表
        public UGD_GoodsTile AllGoodsTile = null;
        //小贱物品列表
        public UGD_GoodsTile AllGoodsTile2 = null;

        private ObservableList<SSchemeMobaGoods> m_filterDefaultTypeList;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsList;
            }
        }

        public void BoxAllGoods(ObservableList<SSchemeMobaGoods> _allGoodsTable, List<int> _defaultGoodsTypeList)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };
            int Dickey = -1;

            m_filterDefaultTypeList = _allGoodsTable.Where((item) =>
            {
                foreach (string label in item.GoodsLabel.Split(charSeparator))
                {
                    if (label == string.Empty)
                        Dickey = 0;
                    else
                        Dickey = Convert.ToInt32(label) / 1000;
                    foreach (int defaultType in _defaultGoodsTypeList)
                    {
                        if (Dickey == defaultType)
                            return true;
                    }
                }
                return false;
            }).ToObservableList();

            SetAllGoodsTileDataSource(m_filterDefaultTypeList);
        }

        private void SetAllGoodsTileDataSource(ObservableList<SSchemeMobaGoods> _datSource)
        {
            AllGoodsTile.SelectedIndex = -1;
            AllGoodsTile2.SelectedIndex = -1;

            AllGoodsTile.OnDeselect.RemoveListener(OnAllGoodsOnDeselect);
            AllGoodsTile2.OnDeselect.RemoveListener(OnAllGoodsOnDeselect);

            AllGoodsTile.OnSelect.RemoveListener(OnMobaGoodsSelected);
            AllGoodsTile2.OnSelect.RemoveListener(OnMobaGoodsSelected2);

            //AllGoodsTile.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
            //AllGoodsTile2.onDoubleClick.RemoveListener(OnMobaGoodsBuy2);

            //AllGoodsTile.onRightClick.RemoveListener(OnMobaGoodsBuy);
            //AllGoodsTile2.onRightClick.RemoveListener(OnMobaGoodsBuy2);

            if (EntityFactory.MainHeroView != null)
            {
                int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                PersonSkinModelItem ModelItem = new PersonSkinModelItem();
                GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref ModelItem);

                //移除英雄限制的装备
                var list = _datSource.Select(d => d).Where(s => (s.GoodsVocationLimit != 0 && s.GoodsVocationLimit == ModelItem.nTraits)).ToList();
                foreach (var i in list)
                {
                    _datSource.Remove(i);
                }
            }

            //根据物品的贵重度分放到不同的Title

            var dataSource0 = _datSource.Select(d => d).Where(s => (s.GoodsPreciousDegree == 0)).ToObservableList();
            var dataSource1 = _datSource.Select(d => d).Where(s => (s.GoodsPreciousDegree == 1)).ToObservableList();


            AllGoodsTile.DataSource = dataSource0;
            AllGoodsTile2.DataSource = dataSource1;


            AllGoodsTile.OnDeselect.AddListener(OnAllGoodsOnDeselect);
            AllGoodsTile2.OnDeselect.AddListener(OnAllGoodsOnDeselect);


            AllGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);
            AllGoodsTile2.OnSelect.AddListener(OnMobaGoodsSelected2);

            //AllGoodsTile.onDoubleClick.AddListener(OnMobaGoodsBuy);
            //AllGoodsTile2.onDoubleClick.AddListener(OnMobaGoodsBuy2);

            //AllGoodsTile.onRightClick.AddListener(OnMobaGoodsBuy);
            //AllGoodsTile2.onRightClick.AddListener(OnMobaGoodsBuy2);

        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            ParentTo<UGD_CustomGoodsFrame>().onGoodsTypeClickEvent.AddListener(OnGoodsTypeClickEvent);
            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.AddListener(OnGoodsItemClickEvent);
            return result;
        }

        protected void OnGoodsTypeClickEvent(int _LabelID)
        {
            if (_LabelID == -1)
                SetAllGoodsTileDataSource(m_filterDefaultTypeList);
            else
                SetAllGoodsTileDataSource(GoodsSchemesDataManager.Instance.GetGoodsSchemeList(_LabelID.ToString()));
        }

        private void OnAllGoodsOnDeselect(int _nIndex, ListViewItem _itemComponent)
        {
            if (_itemComponent != null)
                (_itemComponent as UGD_GoodsItemComponent).GoodsSelectedFrame.gameObject.SetActive(false);
        }
        private void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(AllGoodsTile.DataSource[_nIndex].GoodsID);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.Invoke(AllGoodsTile.DataSource[_nIndex]);
            if (_itemComponent != null)
                (_itemComponent as UGD_GoodsItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }
        private void OnMobaGoodsSelected2(int _nIndex, ListViewItem _itemComponent)
        {
            GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(AllGoodsTile2.DataSource[_nIndex].GoodsID);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.Invoke(AllGoodsTile2.DataSource[_nIndex]);
            if (_itemComponent != null)
                (_itemComponent as UGD_GoodsItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }

        public void OnGoodsItemClickEvent(SSchemeMobaGoods _goods)
        {
            if (AllGoodsTile.SelectedItem != null && _goods.GoodsID != AllGoodsTile.SelectedItem.GoodsID)
                AllGoodsTile.Deselect(AllGoodsTile.SelectedIndex);

            if (AllGoodsTile2.SelectedItem != null && _goods.GoodsID != AllGoodsTile2.SelectedItem.GoodsID)
                AllGoodsTile2.Deselect(AllGoodsTile2.SelectedIndex);

        }
    }

    public class UGD_CustomGoodsFrame_GooodsCanSynthesized : ULevel2_SubViewBase
    {
        public UGD_GoodsTile CanSynthesizedGoodsTile;
        public Button BtnPrevious;
        public Button BtnNexts;

        #region 控制转页部分
        public int PerPage = 6;
        private int Pages = 0;
        protected int currentPage;
        public int CurrentPage
        {
            get
            {
                return currentPage;
            }
            set
            {
                GoToPage(value);
            }
        }
        protected void RecalculatePages()
        {
            var per_block = CanSynthesizedGoodsTile.GetItemsPerBlock() * PerPage;
            Pages = (per_block == 0) ? 0 : Mathf.CeilToInt(((float)CanSynthesizedGoodsTile.GetItemsCount()) / per_block);
        }
        protected float GetPagePosition(int page)
        {
            return CanSynthesizedGoodsTile.GetItemPosition(page * CanSynthesizedGoodsTile.GetItemsPerBlock() * PerPage);
        }
        protected void GoToPage(int page)
        {
            var endPosition = GetPagePosition(page);
            if (IsHorizontal())
            {
                endPosition *= -1;
            }
            if (IsHorizontal())
            {
                CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition = new Vector2(endPosition, CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition.y);
            }
            else
            {
                CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition = new Vector2(CanSynthesizedGoodsTile.ScrollRect.content.anchoredPosition.x, endPosition);
            }
            currentPage = page;

            if (BtnPrevious != null)
            {
                BtnPrevious.interactable = (page != 0);
            }
            if (BtnNexts != null)
            {
                BtnNexts.interactable = (page != (Pages - 1));
            }
        }

        private bool IsHorizontal()
        {
            var rect = CanSynthesizedGoodsTile.ScrollRect.content.rect;
            return rect.width >= rect.height;
        }

        public virtual void Next()
        {
            if (CurrentPage == (Pages - 1))
            {
                return;
            }
            CurrentPage += 1;
        }

        public virtual void Prev()
        {
            if (CurrentPage == 0)
            {
                return;
            }
            CurrentPage -= 1;
        }
        #endregion

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsCanSynthesis;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            InitButtons(_view);

            return result;
        }

        public void OnGoodsItemClickEvent(SSchemeMobaGoods _nParam)
        {
            List<string> CanSynthesizedGoodsIDList = _nParam.GoodsCanComposeList.Split(MobaSchemesConfig.separator).ToList();
            if (CanSynthesizedGoodsIDList != null && CanSynthesizedGoodsIDList.Count > 0)
            {
                int nGoodsID = -1;
                //移除旧的侦听
                CanSynthesizedGoodsTile.OnSelect.RemoveListener(OnMobaGoodsSelected);

                CanSynthesizedGoodsTile.DataSource.Clear();
                CanSynthesizedGoodsTile.DataSource.BeginUpdate();
                foreach (var strGoodsID in CanSynthesizedGoodsIDList)
                {
                    if (string.IsNullOrEmpty(strGoodsID))
                        continue;
                    nGoodsID = Convert.ToInt32(strGoodsID);
                    SSchemeMobaGoods item = null;
                    GoodsSchemesDataManager.Instance.GetMobaGoods_Out(nGoodsID, out item);
                    if (item != null)
                        CanSynthesizedGoodsTile.DataSource.Add(item);
                }

                CanSynthesizedGoodsTile.DataSource.EndUpdate();
                //添加新的侦听
                CanSynthesizedGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);

                RecalculatePages();
                CurrentPage = 0;
            }
        }

        private void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(CanSynthesizedGoodsTile.DataSource[_nIndex].GoodsID);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.Invoke(AllGoodsTile.DataSource[_nIndex]);
            //if (_itemComponent != null)
            //    (_itemComponent as UGD_GoodsItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }

        protected void InitButtons(ULevel1_SubViewBase _viewParent)
        {
            if (BtnPrevious == null)
            {
                Transform BtnPreviousTrans = this.GetTransform.FindChild("BtnPrevious");
                if (BtnPreviousTrans == null)
                {
                    Trace.LogError("没找到对象名为:BtnPrevious 的按钮");
                }
                BtnPrevious = BtnPreviousTrans.GetComponent<Button>();
            }


            if (BtnPrevious == null)
                Trace.LogError("对象名为:BuyBtn 没找到Button组件");
            else
                BtnPrevious.onClick.AddListener(Prev);


            if (BtnNexts == null)
            {
                Transform BtnNextsTrans = this.GetTransform.FindChild("BtnNexts");
                if (BtnNextsTrans == null)
                {
                    Trace.LogError("没找到对象名为:BtnNexts 的按钮");
                }
                BtnNexts = BtnNextsTrans.GetComponent<Button>();
            }


            if (BtnNexts == null)
                Trace.LogError("对象名为:BuyBtn 没找到Button组件");
            else
                BtnNexts.onClick.AddListener(Next);
        }


    }

    public class UGD_CustomGoodsFrame_GoodsSynthesisTree : ULevel2_SubViewBase
    {
        public Transform GoodsSynthesisTree = null;
        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsSynthesisTree;
            }
        }

        private ResNode TreeNodeComponentPrefabRes;
        private List<UGD_GoodsTreeNodeComponent> UseTreeNodeList = new List<UGD_GoodsTreeNodeComponent>();
        private List<UGD_GoodsTreeNodeComponent> UnuseTreeNodeList = new List<UGD_GoodsTreeNodeComponent>();

        private List<UGD_GoodsTreeNodeData> UnuseTreeNodeDataList = new List<UGD_GoodsTreeNodeData>();
        private List<UGD_GoodsTreeNodeData> UseTreeNodeDataList = new List<UGD_GoodsTreeNodeData>();

        private ResNode TreeNodeHorizonalLinePrefabRes;
        private List<UWarTreeNodeHorizonalLineComponnet> UseTreeNodeHorizonalLineList = new List<UWarTreeNodeHorizonalLineComponnet>();
        private List<UWarTreeNodeHorizonalLineComponnet> UnuseTreeNodeHorizonalLineList = new List<UWarTreeNodeHorizonalLineComponnet>();

        private Transform m_UnuseTreeNodeContainner;
        private Transform m_UnuseTreeNodeHorizonalLineContainner;
        private Rect m_ViewRect;
        public Rect ViewRect
        {
            get
            {
                if (m_ViewRect.size == Vector2.zero)
                    m_ViewRect = (GoodsSynthesisTree as RectTransform).rect;
                return m_ViewRect;
            }
        }
        private int m_NodeDefaultHeight = 0;
        private int m_CurNodeDataIndex = -1;
        /// <summary>
        /// 如果点击的是合成树上的物品，则不更新合成树
        /// </summary>
        private bool m_bSelectGoodsIsTreeNode = false;

        //检测函数迭代次数，避免死循环
        private int m_nRunFindNum = 0;
        //检测函数迭代次数，避免死循环
        private int m_nFirstGoodsID = 0;

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.AddListener(OnGoodsItemClickEvent);


            TreeNodeComponentPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + UPath.UStaticState + "/GoodsDisposition/GD_SL_2_GoodsDispositionTreeNodeComponent");
            TreeNodeHorizonalLinePrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "DynamicState_Moba/WarGoodsStore/WarTreeNodeHorizonalLine");

            m_UnuseTreeNodeContainner = new GameObject("UnuseTreeNode").transform;
            m_UnuseTreeNodeHorizonalLineContainner = new GameObject("UnuseTreeNodeHorizonalLine").transform;

            m_UnuseTreeNodeContainner.SetParent(GoodsSynthesisTree, false);
            m_UnuseTreeNodeHorizonalLineContainner.SetParent(GoodsSynthesisTree, false);

            m_NodeDefaultHeight = (int)TreeNodeComponentPrefabRes.GetRectTransformRect().height;

            //预先创建一些
            for (int i = 0; i < 10; i++)
            {
                InstantiateTreeNode();
                InstantiateTreeNodeData();
                InstantiateTreeNodeHorizonalLine();
            }


            return result;
        }
        protected override void OnDestroy()
        {
            AssetBundleManager.DeleteAssets(ref TreeNodeComponentPrefabRes, true);
            AssetBundleManager.DeleteAssets(ref TreeNodeHorizonalLinePrefabRes, true);
            base.OnDestroy();
        }

        #region 树节点显示对象、数据对象和横向连接线缓存
        protected void BeginTreeNode()
        {
            if (UseTreeNodeList.Count <= 0)
                return;

            UGD_GoodsTreeNodeComponent node;
            while (UseTreeNodeList.Count > 0)
            {
                node = UseTreeNodeList[0];
                UseTreeNodeList.Remove(node);
                UnuseTreeNodeList.Add(node);
                node.SetPurchasedIconVisible(false);
                node.transform.SetParent(m_UnuseTreeNodeContainner, false);
                node.onSelect.RemoveListener(OnMobaGoodsSelected);
                //node.onRightClick.RemoveListener(OnMobaGoodsBuy);
                //node.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
                node.SetVisible(false);
            }
        }
        protected bool GetUnuseTreeNode(out UGD_GoodsTreeNodeComponent _nUnuseNode)
        {
            UGD_GoodsTreeNodeComponent node;
            //先从缓冲中取
            if (UnuseTreeNodeList.Count > 0)
            {
                node = UnuseTreeNodeList[UnuseTreeNodeList.Count - 1];
                UnuseTreeNodeList.Remove(node);
                UseTreeNodeList.Add(node);
                _nUnuseNode = node;

                return true;
            }
            //没有的话实例化一个出来
            if (TreeNodeComponentPrefabRes == null)
            {
                Trace.LogError("没有节点预制体，不能实例化物品合成树节点");
                _nUnuseNode = null;
                return false;
            }

            InstantiateTreeNode();

            node = UnuseTreeNodeList[UnuseTreeNodeList.Count - 1];
            UnuseTreeNodeList.Remove(node);
            UseTreeNodeList.Add(node);
            _nUnuseNode = node;

            return true;
        }
        protected bool GetUnuseTreeNodeEx(out UGD_GoodsTreeNodeComponent _nUnuseNode)
        {
            bool res = GetUnuseTreeNode(out _nUnuseNode);
            _nUnuseNode.transform.SetParent(GoodsSynthesisTree, false);
            _nUnuseNode.onSelect.AddListener(OnMobaGoodsSelected);
            //_nUnuseNode.onRightClick.AddListener(OnMobaGoodsBuy);
            //_nUnuseNode.onDoubleClick.AddListener(OnMobaGoodsBuy);
            _nUnuseNode.SetVisible(true);
            return res;
        }

        protected void BeginTreeNodeData()
        {
            if (UseTreeNodeDataList.Count <= 0)
                return;

            UGD_GoodsTreeNodeData node;
            while (UseTreeNodeDataList.Count > 0)
            {
                node = UseTreeNodeDataList[0];
                UseTreeNodeDataList.Remove(node);
                UnuseTreeNodeDataList.Add(node);
                node.Clear();
            }

        }
        protected bool GetUnuseTreeNodeData(out UGD_GoodsTreeNodeData _nUnuseNodeData)
        {
            UGD_GoodsTreeNodeData node;
            //先从缓冲中取
            if (UnuseTreeNodeDataList.Count > 0)
            {
                node = UnuseTreeNodeDataList[UnuseTreeNodeDataList.Count - 1];
                UnuseTreeNodeDataList.Remove(node);
                UseTreeNodeDataList.Add(node);
                _nUnuseNodeData = node;

                return true;
            }

            InstantiateTreeNodeData();

            node = UnuseTreeNodeDataList[UnuseTreeNodeDataList.Count - 1];
            UnuseTreeNodeDataList.Remove(node);
            UseTreeNodeDataList.Add(node);
            _nUnuseNodeData = node;

            return true;

        }

        protected void BeginTreeNodeHorizonalLine()
        {
            if (UseTreeNodeHorizonalLineList.Count <= 0)
                return;

            UWarTreeNodeHorizonalLineComponnet node;
            while (UseTreeNodeHorizonalLineList.Count > 0)
            {
                node = UseTreeNodeHorizonalLineList[0];
                UseTreeNodeHorizonalLineList.Remove(node);
                UnuseTreeNodeHorizonalLineList.Add(node);
                node.transform.SetParent(m_UnuseTreeNodeHorizonalLineContainner, false);

                node.SetVisible(false);
            }
        }
        protected bool GetUnuseTreeNodeHorizonalLine(out UWarTreeNodeHorizonalLineComponnet _nUnuseNode)
        {
            UWarTreeNodeHorizonalLineComponnet node;
            //先从缓冲中取
            if (UnuseTreeNodeHorizonalLineList.Count > 0)
            {
                node = UnuseTreeNodeHorizonalLineList[UnuseTreeNodeHorizonalLineList.Count - 1];
                UnuseTreeNodeHorizonalLineList.Remove(node);
                UseTreeNodeHorizonalLineList.Add(node);
                _nUnuseNode = node;

                return true;
            }
            //没有的话实例化一个出来
            if (TreeNodeHorizonalLinePrefabRes == null)
            {
                Trace.LogError("没有节点横线预制体，不能实例化物品合成树节点横线");
                _nUnuseNode = null;
                return false;
            }

            InstantiateTreeNodeHorizonalLine();

            node = UnuseTreeNodeHorizonalLineList[UnuseTreeNodeHorizonalLineList.Count - 1];
            UnuseTreeNodeHorizonalLineList.Remove(node);
            UseTreeNodeHorizonalLineList.Add(node);
            _nUnuseNode = node;

            return true;
        }

        protected void InstantiateTreeNode()
        {
            UGD_GoodsTreeNodeComponent newNode = TreeNodeComponentPrefabRes.InstanceMainRes<UGD_GoodsTreeNodeComponent>();
            newNode.transform.SetParent(m_UnuseTreeNodeContainner, false);
            UnuseTreeNodeList.Add(newNode);

            newNode.SetVisible(false);
        }
        protected void InstantiateTreeNodeData()
        {
            UGD_GoodsTreeNodeData newNode = new UGD_GoodsTreeNodeData();
            UnuseTreeNodeDataList.Add(newNode);
        }
        protected void InstantiateTreeNodeHorizonalLine()
        {
            UWarTreeNodeHorizonalLineComponnet newNode = TreeNodeHorizonalLinePrefabRes.InstanceMainRes<UWarTreeNodeHorizonalLineComponnet>();
            newNode.transform.SetParent(m_UnuseTreeNodeHorizonalLineContainner, false);
            UnuseTreeNodeHorizonalLineList.Add(newNode);

            newNode.SetVisible(false);
        }
        #endregion

        #region 绘制树所有部分
        protected void BuildTree(int _nMaxDepth)
        {

            int nHeight = _nMaxDepth == 0 ? m_NodeDefaultHeight : (int)(GetTransform as RectTransform).rect.height / _nMaxDepth;
            nHeight = nHeight > m_NodeDefaultHeight ? m_NodeDefaultHeight : nHeight;

            SetCTRootNode();

            SetCTNodeData();

            //DrawVerticleLine();

            DrawHorizonalLine();

            //DrawPurchasedIcon(LogicDataCenter.warMobaGoodsDataManager.GetPurchasedGoodsIDList());
        }

        protected void SetCTRootNode()
        {
            UGD_GoodsTreeNodeData _nNode = GetCurrentNode();
            _nNode.ParentNodeWidth = (int)ViewRect.width;

            GetUnuseTreeNodeEx(out _nNode.GoodsNodeComponent);

            _nNode.GoodsNodeComponent.x = (_nNode.ParentNodeWidth / 2) - (_nNode.GoodsNodeComponent.width / 2);
            _nNode.GoodsNodeComponent.y = 0;
            _nNode.GoodsNodeComponent.SetParentVerticalLineVisible(false);
            _nNode.GoodsNodeComponent.SetChildVerticalLineVisible(_nNode.GoodsChildCount == 0 ? false : true);

            if (_nNode.GoodsChildCount == 0)
            {
                return;
            }

            if (_nNode.ChildsNodeList == null)
                _nNode.ChildsNodeList = new List<UGD_GoodsTreeNodeData>();
            else
                _nNode.ChildsNodeList.Clear();

            for (int i = 0; i < _nNode.GoodsChildCount; i++)
            {
                UGD_GoodsTreeNodeData nextNode = GetCurrentNode();
                nextNode.ParentNode = _nNode;
                GetUnuseTreeNodeEx(out nextNode.GoodsNodeComponent);
                _nNode.ChildsNodeList.Add(nextNode);

                SetCTChildNode(nextNode, i);
            }

        }

        protected void SetCTChildNode(UGD_GoodsTreeNodeData _nNode, int _index)
        {

            int minusCount = 0;
            //同级所有节点所占的宽度
            int ChildsWidth = (int)_nNode.GoodsNodeComponent.width * _nNode.ParentNode.GoodsChildCount;
            int ParentWidth = _nNode.ParentNode.ParentNodeWidth;

            //计算当前子级所有节点所占的宽度是否超出父级宽度
            while (ParentWidth < ChildsWidth)
            {
                minusCount++;
                ChildsWidth = (int)_nNode.GoodsNodeComponent.width * (_nNode.ParentNode.GoodsChildCount - minusCount);
            }

            _nNode.ParentNodeWidth = (_nNode.ParentNode.ParentNodeWidth) / (_nNode.ParentNode.GoodsChildCount - minusCount);

            //从父节点中点到左边距离开始算起
            int px = (int)(_nNode.ParentNode.GoodsNodeComponent.x + _nNode.ParentNode.GoodsNodeComponent.width / 2) - (_nNode.ParentNode.ParentNodeWidth / 2);
            float XOffset = 0.5f;

            //同级节点为偶数则偏移半个节点位置，否则不偏移
            if (minusCount > 0)
            {
                XOffset = _nNode.ParentNode.GoodsChildCount % 2 == 0 ? 0.5f : 0;
            }

            _nNode.GoodsNodeComponent.x = px + (_nNode.ParentNodeWidth * (_index + XOffset)) - (_nNode.GoodsNodeComponent.width / 2);
            _nNode.GoodsNodeComponent.y = _nNode.ParentNode.GoodsNodeComponent.y - _nNode.ParentNode.GoodsNodeComponent.height;
            _nNode.GoodsNodeComponent.SetParentVerticalLineVisible(true);
            _nNode.GoodsNodeComponent.SetChildVerticalLineVisible(_nNode.GoodsChildCount == 0 ? false : true);

            if (_nNode.GoodsChildCount == 0)
            {
                return;
            }


            if (_nNode.ChildsNodeList == null)
                _nNode.ChildsNodeList = new List<UGD_GoodsTreeNodeData>();
            else
                _nNode.ChildsNodeList.Clear();



            for (int i = 0; i < _nNode.GoodsChildCount; i++)
            {
                UGD_GoodsTreeNodeData nextNode = GetCurrentNode();
                nextNode.ParentNode = _nNode;
                _nNode.ChildsNodeList.Add(nextNode);
                GetUnuseTreeNodeEx(out nextNode.GoodsNodeComponent);

                SetCTChildNode(nextNode, i);
            }
        }

        protected void SetCTNodeData()
        {
            UseTreeNodeList.ForEach((node, index) =>
            {
                node.SetData(UseTreeNodeDataList[index].GoodsItem);
            });
        }

        protected void DrawVerticleLine()
        {
            UseTreeNodeDataList.ForEach((NodeData) =>
            {

                bool bParentVertileLine = NodeData.ParentNode == null ? false : true;
                bool bChildVertileLine = NodeData.GoodsChildCount <= 0 ? false : true;
                NodeData.GoodsNodeComponent.SetVerticalLineVisible(bChildVertileLine, bParentVertileLine);
            });
        }

        protected void DrawHorizonalLine()
        {
            UGD_GoodsTreeNodeData BeginNode;
            UGD_GoodsTreeNodeData EndNode;
            UWarTreeNodeHorizonalLineComponnet line;

            UseTreeNodeDataList.ForEach((NodeData) =>
            {
                BeginNode = null;
                EndNode = null;
                line = null;
                if (NodeData.GoodsChildCount > 1)
                {
                    BeginNode = NodeData.ChildsNodeList[0];
                    EndNode = NodeData.ChildsNodeList[NodeData.GoodsChildCount - 1];
                    GetUnuseTreeNodeHorizonalLine(out line);


                    Vector2 BeginNodePos = (BeginNode.GoodsNodeComponent.ParentVerticalLine.transform as RectTransform).switchToRectTransform((GetTransform as RectTransform), UISystem.Instance.GetCamera());
                    Vector2 EndNodePos = (EndNode.GoodsNodeComponent.ParentVerticalLine.transform as RectTransform).switchToRectTransform((GetTransform as RectTransform), UISystem.Instance.GetCamera());

                    line.width = EndNodePos.x - BeginNodePos.x;//EndNode.GoodsNodeComponent.ChildVerticalLineRight - line.x;

                    line.x = 0;// BeginNode.GoodsNodeComponent.ChildVerticalLineLeft;
                    line.y = 0;//BeginNode.GoodsNodeComponent.ChildVerticalLineTop;

                    line.transform.SetParent(BeginNode.GoodsNodeComponent.ParentVerticalLine.transform, false);
                    line.SetVisible(true);

                }

            });
        }

        public void DrawPurchasedIcon(List<int> _nPurchasedList)
        {
            //List<int> tmpPurchasedList = m_MyParent.GetCurrentPurchasedGoodsID();
            if (_nPurchasedList == null)
                return;
            int tmpIndex = -1;

            for (int i = 0; i < UseTreeNodeDataList.Count;)
            {
                tmpIndex = _nPurchasedList.IndexOf(UseTreeNodeDataList[i].GoodsItem.GoodsID);
                UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(false);
                if (tmpIndex != -1)
                {
                    UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(true);
                    _nPurchasedList.RemoveAt(tmpIndex);

                    if (i == 0)
                    {
                        UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(false);
                    }
                    else
                    {
                        if (UseTreeNodeDataList[i].GoodsChildCount > 0)
                        {
                            for (int j = 1; j <= UseTreeNodeDataList[i].GoodsChildCount; j++)
                                UseTreeNodeDataList[i + j].GoodsNodeComponent.SetPurchasedIconVisible(true);
                            i += UseTreeNodeDataList[i].GoodsChildCount;
                        }
                    }
                }
                i++;
            }
        }
        #endregion

        protected UGD_GoodsTreeNodeData GetCurrentNode()
        {
            m_CurNodeDataIndex++;
            if (m_CurNodeDataIndex >= UseTreeNodeDataList.Count)
            {
                Debug.LogWarning("超出范围了");
                return null;
            }
            UGD_GoodsTreeNodeData _nNode = UseTreeNodeDataList[m_CurNodeDataIndex];

            return _nNode;
        }

        public void OnGoodsItemClickEvent(SSchemeMobaGoods _nParam)
        {
            if (m_bSelectGoodsIsTreeNode)
            {
                m_bSelectGoodsIsTreeNode = false;
                return;
            }

            //清除合成树显示部分
            BeginTreeNode();
            BeginTreeNodeData();
            BeginTreeNodeHorizonalLine();

            //计算合成树数据部分
            int nDepth = 0;
            m_CurNodeDataIndex = -1;
            IteratorFindNeedGoods(_nParam.GoodsID, ref nDepth);//, ref UnuseTreeNodeDataList);

            BuildTree(nDepth);

        }

        private void IteratorFindNeedGoods(int _nGoodsID, ref int _nDepth)//, ref List<UWarGoodsStoreTreeNodeData> _list)
        {
            UGD_GoodsTreeNodeData node;
            GetUnuseTreeNodeData(out node);//            new UWarGoodsStoreTreeNodeData();

            SSchemeMobaGoods item = null;

            if (GoodsSchemesDataManager.Instance.GetMobaGoods_Out(_nGoodsID, out item) == false)
                return;
            node.GoodsItem = item;
            node.GoodsDepth = _nDepth;


            //这里会挺频繁new的，有需要的时候再优化吧
            List<int> NeedSynthesizedGoodsIDList = GoodsSchemesDataManager.Instance.GetSchemeGoodsNeedComposeList(_nGoodsID);//item.SchemeGoodsInfo.GoodsCanComposeList.Split(MobaSchemesConfig.separator).ToList();

            m_nRunFindNum++;
            if (m_nRunFindNum == 1)
                m_nFirstGoodsID = _nGoodsID;
            if (m_nRunFindNum > 10)
            {
                Trace.LogError("配置物品的合成所需物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nGoodsID.ToString());
                m_nRunFindNum--;
                m_nFirstGoodsID = 0;
                return;
            }


            if (NeedSynthesizedGoodsIDList == null)
            {
                node.GoodsChildCount = 0;
                //_list.Add(node);
            }
            else
            {
                node.GoodsChildCount = NeedSynthesizedGoodsIDList.Count;
                //_list.Add(node);

                _nDepth++;
                for (int i = 0; i < NeedSynthesizedGoodsIDList.Count; i++)
                {
                    int nID = Convert.ToInt32(NeedSynthesizedGoodsIDList[i]);

                    IteratorFindNeedGoods(nID, ref _nDepth);//, ref _list);
                }
            }

            m_nRunFindNum--;

        }

        public void OnMobaGoodsSelected(ListViewItem _itemComponent)
        {
            int nIndex = UseTreeNodeList.FindIndex((node) => { return node == _itemComponent; });
            m_bSelectGoodsIsTreeNode = true;

            GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(UseTreeNodeDataList[nIndex].GoodsItem.GoodsID);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.Invoke(UseTreeNodeDataList[nIndex].GoodsItem);
        }
    }

    public class UGD_CustomGoodsFrame_GoodsDetail : ULevel2_SubViewBase
    {
        public Text GoodsName = null;
        public Text GoodsDetailInfo = null;
        public Button GoodsSynthesisBtn = null;
        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsDetail;
            }
        }

        protected void UpdateView(UWarGoodsStoreItem _nParam)
        {
            if (_nParam == null || _nParam.SchemeGoodsInfo == null)
                return;

            GoodsName.text = _nParam.SchemeGoodsInfo.GoodsName;

            string des = _nParam.SchemeGoodsInfo.GoodsDes;
            UBBProperty.GetInstance().getRatio(des);
            U3D_Render.Common.UBB.toHtml(ref des, UBB_FORMAT_TYPE.UGUI);
            GoodsDetailInfo.text = des;
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            //ParentTo<UGD_CustomGoodsFrame>().onGoodsItemClickEvent.AddListener(OnGoodsItemClickEvent);
            if (GoodsSynthesisBtn != null)
            {
                GoodsSynthesisBtn.onClick.AddListener(OnGoodsSynthesisBtnClick);
                GoodsSynthesisBtn.gameObject.SetActive(false);
            }

            return result;
        }

        public void OnGoodsItemClickEvent(SSchemeMobaGoods _goods)
        {
            if (GoodsName != null)
            {
                GoodsName.text = _goods.GoodsName;
            }
            if (GoodsDetailInfo != null)
            {
                string des = _goods.GoodsDes;
                UBBProperty.GetInstance().getRatio(des);
                U3D_Render.Common.UBB.toHtml(ref des, UBB_FORMAT_TYPE.UGUI);
                GoodsDetailInfo.text = des;
                if (GoodsSynthesisBtn != null)
                    GoodsSynthesisBtn.gameObject.SetActive(true);
            }
        }

        protected void OnGoodsSynthesisBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_OPEN_GOODSSYNTHESISVIEW, null);
        }
    }

    public class UGD_CustomGoodsFrame_HeroHeadPortrait : ULevel2_SubViewBase
    {
        public Button ChangeHeroBtn = null;
        public Image HeadPortraitImage = null;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.HeroHeadPortrait;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            if (ChangeHeroBtn != null)
                ChangeHeroBtn.onClick.AddListener(OnChangeHeroBtnClick);

            return result;
        }

        protected void OnChangeHeroBtnClick()
        {
            ParentTo<UGD_CustomGoodsFrame>().onChangeHeroClickEvent.Invoke();
        }

        public void OnHeroHeadPortraitClickEvent(int _nHeroID)
        {
            HeadPortraitImage.sprite = UGoodsDispositionView.GetHeroHeadPortrait(_nHeroID);
        }
    }

    public class UGD_CustomGoodsFrame_HeroHeadPortraitList : ULevel2_SubViewBase
    {
        public UGD_HeroHeadPortraitTile HeadPortraitTile = null;
        public UButtonGroup HeadLineItemGroup = null;
        public UButtonGroupItem HeadLineItemTemplet = null;
        public Button CloseBtn = null;
        public Button CopyConfirmBtn = null;
        public Button CopyCancelBtn = null;
        public Toggle IsBuyToggle = null;
        protected EViewState m_eCurrentViewState = EViewState.EVS_Default;
        protected EViewState CurrentViewState
        {
            get { return m_eCurrentViewState; }
            set
            {
                OnCurrentViewStateChange(value);
                m_eCurrentViewState = value;
            }
        }
        protected enum EViewState
        {
            EVS_Default,
            EVS_Copy,
        }

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.HeroHeadPortraitList;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            m_bPopupView = true;

            HeadLineItemGroup.SkipInactive = false;
            ParentTo<UGD_CustomGoodsFrame>().onChangeHeroClickEvent.AddListener(OnChangeHeroClickEvent);
            if (CloseBtn != null)
                CloseBtn.onClick.AddListener(OnCloseBtnClick);

            if (HeadLineItemTemplet != null)
                HeadLineItemTemplet.gameObject.SetActive(false);

            if (IsBuyToggle != null)
            {
                IsBuyToggle.isOn = GoodsDispositionDataManager.Instance.IsOnlyShowBuyHero;
                IsBuyToggle.onValueChanged.AddListener(IsBuyToggleValueChanged);
            }

            if (CopyConfirmBtn != null)
            {
                CopyConfirmBtn.onClick.AddListener(OnCopyConfirmBtnClick);
            }
            if (CopyCancelBtn != null)
            {
                CopyCancelBtn.onClick.AddListener(OnCopyCancelBtnClick);
            }


            return result;
        }

        protected override void OnDestroy()
        {
            ParentTo<UGD_CustomGoodsFrame>().onChangeHeroClickEvent.RemoveListener(OnChangeHeroClickEvent);
            if (CloseBtn != null)
                CloseBtn.onClick.RemoveListener(OnCloseBtnClick);

            if (IsBuyToggle != null)
            {
                IsBuyToggle.onValueChanged.RemoveListener(IsBuyToggleValueChanged);
            }

            base.OnDestroy();
        }

        protected void OnChangeHeroClickEvent()
        {
            CurrentViewState = EViewState.EVS_Default;
            this.SetVisible(true);
        }

        protected void OnCloseBtnClick()
        {
            this.SetVisible(false);
        }

        protected void OnHeadPortraitSelect(int index, ListViewItem item)
        {
            if (m_eCurrentViewState == EViewState.EVS_Default)
            {
                UGD_HeroHeadPortraitItem itemData = HeadPortraitTile.DataSource[index];
                if (itemData != null && itemData.HeroHeadPortraitID > 0)
                {
                    GoodsDispositionDataManager.Instance.CurrentSelectHero = itemData.HeroHeadPortraitID;
                    OnCloseBtnClick();
                }
            }
            else if (m_eCurrentViewState == EViewState.EVS_Copy)
            {

            }

        }

        public void SetPlayerHeroDataSource(ObservableList<UGD_HeroHeadPortraitItem> _newDataSource)
        {
            HeadPortraitTile.DataSource = _newDataSource;
            HeadPortraitTile.OnSelect.RemoveListener(OnHeadPortraitSelect);
            HeadPortraitTile.OnSelect.AddListener(OnHeadPortraitSelect);
        }

        public void SetHeadLineItemData(List<string> _listHeroType)
        {
            HeadLineItemTemplet.gameObject.SetActive(true);
            for (int i = 0; i < _listHeroType.Count; ++i)
            {
                UButtonGroupItem newItem = ResNode.InstantiateRes<UButtonGroupItem>(HeadLineItemTemplet.gameObject);
                Text txt = newItem.GetComponentInChildren<Text>();
                if (txt != null)
                {
                    txt.text = _listHeroType[i];
                }
                newItem.Group = HeadLineItemGroup;
                if (i == 0)
                    newItem.Selected = true;
                newItem.transform.SetParent(HeadLineItemGroup.transform, false);
            }
            HeadLineItemTemplet.gameObject.SetActive(false);

            // HeadLineItemGroup.SetItemSelectedItem(0);
            HeadLineItemGroup.onButtonGroupSelectedChange.AddListener((oldItem, newItem) =>
            {
                int selectIndex = HeadLineItemGroup.GetItemIndex(newItem);
                GoodsDispositionDataManager.Instance.CurrentSelectHeroType = (selectIndex - 1);
            });
        }

        protected void IsBuyToggleValueChanged(bool _newValue)
        {
            GoodsDispositionDataManager.Instance.IsOnlyShowBuyHero = _newValue;
        }

        public void DispositionCopy()
        {
            CurrentViewState = EViewState.EVS_Copy;
            this.SetVisible(true);
        }

        protected void OnCurrentViewStateChange(EViewState _eNewState)
        {
            if (_eNewState == EViewState.EVS_Copy)
            {
                _onNewViewState_Copy();
            }
            else if (_eNewState == EViewState.EVS_Default)
            {
                _onNewViewState_Default();
            }
        }

        protected void _onNewViewState_Copy()
        {
            HeadPortraitTile.Multiple = true;
            if (CopyCancelBtn != null)
                CopyCancelBtn.gameObject.SetActive(true);
            if (CopyConfirmBtn != null)
                CopyConfirmBtn.gameObject.SetActive(true);
        }

        protected void _onNewViewState_Default()
        {
            HeadPortraitTile.Multiple = false;

            if (CopyCancelBtn != null)
                CopyCancelBtn.gameObject.SetActive(false);
            if (CopyConfirmBtn != null)
                CopyConfirmBtn.gameObject.SetActive(false);
        }

        protected void OnCopyConfirmBtnClick()
        {
            if (HeadPortraitTile.SelectedIndicies.Count <= 0)
                return;
            List<int> tmpHeroIDs = HeadPortraitTile.SelectedIndicies;
            for (int i = tmpHeroIDs.Count - 1; i >= 0; i--)
            {
                int index = HeadPortraitTile.SelectedIndicies[i];
                if (GoodsDispositionDataManager.Instance.AddHeroGoodsDispositionFromCopy(HeadPortraitTile.DataSource[index].HeroHeadPortraitID, OnCopyConfirmBtnClick))
                {
                    tmpHeroIDs.RemoveAt(i);
                    HeadPortraitTile.Deselect(index);
                }
                else
                    break;
                    
            }
            if(tmpHeroIDs.Count <= 0)
                this.SetVisible(false);

        }

        protected void OnCopyCancelBtnClick()
        {
            this.SetVisible(false);
        }

        internal override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if(!visible)
            {
                for (int i = HeadPortraitTile.SelectedIndicies.Count - 1; i >= 0; i--)
                {
                    HeadPortraitTile.Deselect(HeadPortraitTile.SelectedIndicies[i]);
                }
            }
        }
    }

    public class UGD_CustomGoodsFrame_GoodsDisposition : ULevel2_SubViewBase
    {
        public Button ChangeDispositionBtn = null;
        public Button EditDispositionNameBtn = null;

        public Button EditDispositionCreatorNameBtn = null;

        public Text DispositionNameTF = null;
        public Text DispositionAuthorNameTF = null;

        public List<UGD_GoodsDispositionRendererCurrent> GoodsDispositionRenderer = null;
        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsDisposition;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            if (ChangeDispositionBtn != null)
                ChangeDispositionBtn.onClick.AddListener(OnChangeDispositionBtnClick);

            if (EditDispositionNameBtn != null)
                EditDispositionNameBtn.onClick.AddListener(OnEditDispositionNameClick);



            for (int i = 0; i < GoodsDispositionRenderer.Count; ++i)
            {
                GoodsDispositionRenderer[i].SlotID = i;
            }
            UGD_GoodsDispositionItem defaultItem = new UGD_GoodsDispositionItem();
            SetCurrentGoodsDisposition(defaultItem);

            return result;
        }

        protected void OnChangeDispositionBtnClick()
        {
            ParentTo<UGD_CustomGoodsFrame>().onChangeGoodsDispositionClickEvent.Invoke();
        }
        protected void OnEditDispositionNameClick()
        {
            ParentTo<UGD_CustomGoodsFrame>().onEditGoodsDispositionInfoClickEvent.Invoke(ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "ChangeDispositionName"),
            (newName) =>
            {
                DispositionNameTF.text = newName;
                GoodsDispositionDataManager.Instance.CurrentGoodsDisposition.Name = newName;

            }, (newName) => { });
        }


        public void SetCurrentGoodsDisposition(UGD_GoodsDispositionItem _nCurrentGoodsDisposition)
        {
            if (_nCurrentGoodsDisposition == null)
                return;

            DispositionNameTF.text = _nCurrentGoodsDisposition.Name;
            DispositionAuthorNameTF.text = _nCurrentGoodsDisposition.Author;

            for (int i = 0; i < _nCurrentGoodsDisposition.GoodsList.Length; ++i)
            {
                GoodsDispositionRenderer[i].SetData(_nCurrentGoodsDisposition.GoodsList[i]);
            }
        }
    }

    public class UGD_CustomGoodsFrame_GoodsDispositionList : ULevel2_SubViewBase
    {
        public UGD_GoodsDispositionList GoodsDispositionList = null;
        public Button CloseBtn = null;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsDispositionList;
            }
        }
        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);
            m_bPopupView = true;
            ParentTo<UGD_CustomGoodsFrame>().onChangeGoodsDispositionClickEvent.AddListener(OnChangeGoodsDispositionClick);
            if (CloseBtn != null)
            {
                CloseBtn.onClick.AddListener(OnCloseBtnClick);
            }
            return result;
        }

        protected void OnChangeGoodsDispositionClick()
        {
            this.SetVisible(true);
        }

        protected void OnCloseBtnClick()
        {
            this.SetVisible(false);
        }

        public void SetGoodsDispositionListData(ObservableList<UGD_GoodsDispositionItem> _newDataSource)
        {
            GoodsDispositionList.DataSource = _newDataSource;
        }

        internal override void SetVisible(bool visible)
        {
            if (visible)
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_HEROGOODSDISPOSITIONLIST, null);
            }
            base.SetVisible(visible);
        }
    }

    public class UGD_CustomGoodsFrame_EditGoodsDispositionInfo : ULevel2_SubViewBase
    {
        public Text TitleTxt = null;
        public InputField NewInfoIF = null;
        public Button CancelBtn = null;
        public Button ConfirmBtn = null;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.EditGoodsDispositionInfo;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);
            m_bPopupView = true;
            ParentTo<UGD_CustomGoodsFrame>().onEditGoodsDispositionInfoClickEvent.AddListener(OnEditGoodsDispositionInfoClick);

            return result;
        }

        protected void OnEditGoodsDispositionInfoClick(string _title, UnityAction<string> _confirmBtnAction, UnityAction<string> _cancelAction)
        {
            TitleTxt.text = _title;
            ConfirmBtn.onClick.RemoveAllListeners();
            CancelBtn.onClick.RemoveAllListeners();
            NewInfoIF.text = string.Empty;
            this.SetVisible(true);

            ConfirmBtn.onClick.AddListener(() =>
            {
                this.SetVisible(false);
                _confirmBtnAction.Invoke(NewInfoIF.text);
            });

            CancelBtn.onClick.AddListener(() =>
            {
                this.SetVisible(false);
                _cancelAction.Invoke(NewInfoIF.text);
            });
        }

    }

    public class UGD_CustomGoodsFrame_GoodsDragDetail : ULevel2_SubViewBase
    {        /// <summary>
             /// 是否在拖拽中
             /// </summary>
        protected bool m_draging;
        public UGD_GoodsDispositionRendererDraggingItem DraggingItem;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsDragDetail;
            }
        }

        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);

            return result;

        }

        protected void UpdatePosition()
        {
            Vector3 globalMousePos;

            var eventData = EventSystem.current.CurrentPointerEventData();

            if (RectTransformUtility.ScreenPointToWorldPointInRectangle(m_parentView.GetRectTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
            {
                GetRectTransform.SetPosition(globalMousePos);
            }
        }
        public void OnBeginDrag(int _nDragGoodsID)
        {
            if (m_draging == false)
            {
                m_draging = true;

                UpdatePosition();

                if (DraggingItem != null)
                {
                    DraggingItem.SetData(UGoodsDispositionView.GetGoodsIconByGoodsID(_nDragGoodsID));
                    this.SetVisible(true);
                }
            }
        }
        public void OnDragging(int _nDragGoodsID)
        {
            UpdatePosition();
        }
        public void OnEndDrag(int _nDragGoodsID)
        {
            if (m_draging)
            {
                m_draging = false;
                this.SetVisible(false);
            }
        }

    }

    public class UGD_CustomGoodsFrame_GoodsSynthesis : ULevel2_SubViewBase
    {
        public UGD_CustomGoodsFrame_GoodsSynthesisTree GoodsSynthesisTree = null;
        public UGD_CustomGoodsFrame_GooodsCanSynthesized GooodsCanSynthesized = null;
        public UGD_CustomGoodsFrame_GoodsDetail GoodsDetail = null;
        public Button CloseBtn = null;

        protected int m_nOldSelectedGoodsID;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsSynthesis;
            }
        }
        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);
            m_bPopupView = true;
            if (GoodsSynthesisTree != null)
                GoodsSynthesisTree.Init(_view);
            if (GooodsCanSynthesized != null)
                GooodsCanSynthesized.Init(_view);
            if (GoodsDetail != null)
                GoodsDetail.Init(_view);

            if (CloseBtn != null)
            {
                CloseBtn.onClick.AddListener(OnCloseBtnClick);
            }

            return result;

        }

        public void OpenGoodsSynthesis(SSchemeMobaGoods _nParam)
        {
            this.SetVisible(true);
            OnGoodsItemClickEvent(_nParam);
        }
        public void OnGoodsItemClickEvent(SSchemeMobaGoods _nParam)
        {
            if (!IsVisible())
                return;

            if (GoodsSynthesisTree != null)
                GoodsSynthesisTree.OnGoodsItemClickEvent(_nParam);
            if (GooodsCanSynthesized != null)
                GooodsCanSynthesized.OnGoodsItemClickEvent(_nParam);
            if (GoodsDetail != null)
                GoodsDetail.OnGoodsItemClickEvent(_nParam);
        }
        protected void OnCloseBtnClick()
        {
            this.SetVisible(false);
        }

        internal override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
            {
                m_nOldSelectedGoodsID = GoodsDispositionDataManager.Instance.CurrentSelectGoodsID;
            }
            else
            {
                GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(m_nOldSelectedGoodsID, false);
            }
        }

    }

    public class UGD_CustomGoodsFrame_GoodsDispositionControl : ULevel2_SubViewBase
    {
        public Button RestoreBtn = null;
        public Button SaveBtn = null;
        public Button AddBtn = null;
        public Button MasterBtn = null;
        public Button ShareBtn = null;
        public Button CopyBtn = null;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.GoodsDispositionControl;
            }
        }
        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);
            if (SaveBtn != null)
            {
                SaveBtn.onClick.AddListener(OnSaveBtnClick);
            }

            if (RestoreBtn != null)
            {
                RestoreBtn.onClick.AddListener(OnRestoreBtnClick);
            }

            if (AddBtn != null)
            {
                AddBtn.onClick.AddListener(OnAddBtnClick);
            }

            if (ShareBtn != null)
            {
                ShareBtn.onClick.AddListener(OnShareBtnClick);
            }

            if (CopyBtn != null)
            {
                CopyBtn.onClick.AddListener(OnCopyBtnClick);
            }
            return result;

        }

        protected void OnSaveBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SAVE_CURRENTDISPOSITION, null);
        }

        protected void OnRestoreBtnClick()
        {
            GoodsDispositionDataManager.Instance.RestoreCurrent();
            //UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTDISPOSITION, null);
        }

        protected void OnAddBtnClick()
        {
            GoodsDispositionDataManager.Instance.AddHeroGoodsDisposition();
        }

        protected void OnShareBtnClick()
        {
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SET_DISPOSITIONINFO);
            GoodsDispositionDataManager.Instance.ShareEquipScheme();
        }

        protected void OnCopyBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_COPY_CURRENTDISPOSITION, null);
        }
    }

    /// <summary>
    /// 配置已满时额外显示的英雄配置列表窗口
    /// </summary>
    public class UGD_CustomGoodsFrame_DispositionFilled : ULevel2_SubViewBase, ITimerHanlder
    {
        public UGD_DispositionFilledList DispositionList = null;
        public Button CancelBtn = null;
        public Button ConfirmBtn = null;

        protected UGD_DispositionFilledMsgData m_nCurrentMsgData;
        protected const int HeroGoodsDispositionTimerID = 0;

        public override ELevel2_SubView GetSubViewType
        {
            get
            {
                return ELevel2_SubView.DispositionFilled;
            }
        }
        internal override bool Init(ULevel1_SubViewBase _view)
        {
            bool result = base.Init(_view);
            m_bPopupView = true;
            if (CancelBtn != null)
            {
                CancelBtn.onClick.AddListener(OnCancelBtnClick);
            }

            if (ConfirmBtn != null)
            {
                ConfirmBtn.interactable = false;
                ConfirmBtn.onClick.AddListener(OnConfirmBtnClick);
            }

            return result;

        }

        public void OpenDispositionFilled(UGD_DispositionFilledMsgData _msgData)
        {
            m_nCurrentMsgData = _msgData;

            //SetHeroGoodsDispositionList();
            TimerManager.SetTimer(this, HeroGoodsDispositionTimerID, 0.5f, 20);
        }

        public void UpdateDispositionFilled()
        {
            //SetHeroGoodsDispositionList();
            TimerManager.SetTimer(this, HeroGoodsDispositionTimerID, 0.5f, 20);
        }

        protected void OnCancelBtnClick()
        {
            SetVisible(false);
        }

        protected void OnConfirmBtnClick()
        {
            SetVisible(false);

            if (m_nCurrentMsgData.onConfirmCallback != null)
            {
                m_nCurrentMsgData.onConfirmCallback.Invoke();
            }
        }

        protected void SetHeroGoodsDispositionList()
        {
            if (m_nCurrentMsgData.HeroID <= 0)
                return;

            List<GameLogic.cmd_war_equipscheme> schemeList;
            if (!EquipSchemeDataManager.Instance.getDiyEquipSchemeList(m_nCurrentMsgData.HeroID, out schemeList))
            {

                return;
            }

            TimerManager.KillTimer(this, HeroGoodsDispositionTimerID);


            int defaultDispositionID = 0;

            ObservableList<UGD_GoodsDispositionItem> tmpList = new ObservableList<UGD_GoodsDispositionItem>();

            for (int i = 0; i < schemeList.Count; ++i)
            {
                GameLogic.cmd_war_equipscheme scheme = schemeList[i];
                UGD_GoodsDispositionItem item = new UGD_GoodsDispositionItem();
                item.From(scheme);

                if (i == 0)
                {
                    defaultDispositionID = item.DispositionID;
                    item.IsCurrent = true;
                }


                tmpList.Add(item);
            }

            DispositionList.DataSource = tmpList;
            ConfirmBtn.interactable = true;


            SetVisible(true);
        }


        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case HeroGoodsDispositionTimerID:
                    SetHeroGoodsDispositionList();
                    break;
            }
        }
    }
}

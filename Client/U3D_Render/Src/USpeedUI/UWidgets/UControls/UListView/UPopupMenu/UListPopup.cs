/*******************************************************************
** 文件名:	UListPopup.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/22
** 版  本:	1.0
** 描  述:	弹出菜单列表
** 应  用:  管理菜单元素与组别分隔条的创建、删除

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2017.1.19
** 描  述: 弹窗菜单列表增加多级子菜单
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace USpeedUI.UWidgets
{
    /// <summary>
    /// 弹出框更新时需要的数据
    /// </summary>
    public class UPopupMenuUpdateParam
    {
        public TipPosition PopupTipPostion;
        public Transform PopupTarget;
        public Transform PopupView;
        public Camera UICamera;
        public Canvas RootCanvas;
        public bool PopupOverflowCheck;
        public Sprite PopupModalSprite ;
        public Color? PopupModalColor ;
        public UnityAction ModalClickAction;
        public bool IsModalUsed;

        public UPopupMenuUpdateParam(Transform _tfTarget = null, Camera _camUI = null, Canvas _canvasRoot = null,
                                    TipPosition _eTipPostion = TipPosition.MouseTopRightCorner, bool _bOverflowCheck = true,
                                    Sprite _sprModalSprite = null,Color? _colorModalColor = null,Transform _tfPopupView = null,bool _bUseModal = false,UnityAction _actionModalCilck = null)
        {
            PopupTarget = _tfTarget;
            UICamera = _camUI;
            RootCanvas = _canvasRoot;
            PopupTipPostion = _eTipPostion;
            PopupOverflowCheck = _bOverflowCheck;
            PopupModalSprite = _sprModalSprite;
            PopupModalColor = _colorModalColor;
            PopupView = _tfPopupView;
            IsModalUsed = _bUseModal;
            ModalClickAction = _actionModalCilck;
        }
    }


    public class UListPopupBase<TComponent, TItem> : UCustomUIBehaviour where TComponent : UListComponentPopupBase
                                                                   where TItem : UListComponentItemDataPopupBase
    {
        protected EasyLayout.EasyLayout m_ListLayout;
        public EasyLayout.EasyLayout ListLayout
        {
            get
            {
                if(m_ListLayout == null)
                {
                    m_ListLayout = GetComponent<EasyLayout.EasyLayout>();
                    if (m_ListLayout == null)
                    {
                        m_ListLayout = gameObject.AddComponent<EasyLayout.EasyLayout>();
                        m_ListLayout.Stacking = EasyLayout.Stackings.Vertical;
                        m_ListLayout.Spacing = new Vector2(0, 2);
                        m_ListLayout.Margin = Vector2.zero;
                        m_ListLayout.ChildrenWidth = EasyLayout.ChildrenSize.FitContainer;
                    }
                }
                return m_ListLayout;
            }
        }
        protected RectTransform m_Container;
        public RectTransform Container
        {
            get
            {
                if (m_Container == null)
                    m_Container = this.transform as RectTransform;
                return m_Container;
            }
        }

        protected List<TComponent> m_ItemComponent = new List<TComponent>();
        protected List<TComponent> m_ItemComponentsCache = new List<TComponent>();

        protected List<GameObject> m_LineComponentsCache = new List<GameObject>();
        protected Dictionary<int, GameObject> m_LineGroupIDDic = new Dictionary<int, GameObject>();

        protected ResNode DefaultItemComponentRes;

        protected ResNode SeparationLineComponentRes;
        protected int? m_ModalKey;
        protected Transform m_LastView;

        public UnityAction<PointerEventData,UListComponentPopupBase> OnEnterAction = null;
        public UnityAction<PointerEventData,UListComponentPopupBase> OnExitAction = null;
        public UnityAction OnClickAction = null;
        public TComponent ParentConponent = null;
        protected override void Awake()
        {
            base.Awake();
            SeparationLineComponentRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "CommomState/PopupMenu/SeparationLine");
            DefaultItemComponentRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "CommomState/PopupMenu/DefaultComponent");
        }

        protected override void OnDestroy()
        {
            AssetBundleManager.DeleteAssets(ref SeparationLineComponentRes, true);
            AssetBundleManager.DeleteAssets(ref DefaultItemComponentRes, true);

            base.OnDestroy();
        }

        private void DeactivateComponent(TComponent _typeComponent)
        {
            //RemoveCallback(component, component.Index);
            if (_typeComponent != null)
            {
                _typeComponent.ItemData = null;
                _typeComponent.ChildPopupList = null;
                if(_typeComponent.onEnter != null)
                    _typeComponent.onEnter.RemoveListener(OnEnterAction);
                if(_typeComponent.onExit != null)
                    _typeComponent.onExit.RemoveListener(OnExitAction);
                _typeComponent.gameObject.SetActive(false);
                _typeComponent.onClick.RemoveAllListeners();
            }
        }

        /// <summary>
        /// 添加弹出框内容
        /// </summary>
        /// <param name="newItem"></param>
        public void Add(TItem newItem)
        {
            if (newItem == null)
                return;

            AddItemComponent(newItem);
            AddSeparationLineComponent(newItem);
        }

        /// <summary>
        /// 清空弹出框
        /// </summary>
        public void Clear()
        {
            RemoveItemComponent();
            RemoveSeparationLineComponent();
            RemoveModalHelper();
            
            OnEnterAction = null;
            OnExitAction = null;
            OnClickAction = null;
            if(ParentConponent != null)
                ParentConponent.ChildPopupList = null;
            ParentConponent = null;
            Container.SetParent(m_LastView, false);
            Container.gameObject.SetActive(false);
        }
        /// <summary>
        /// 更新弹出框显示数据
        /// </summary>
        /// <param name="_nParam"></param>
        public void UpdatePopup(UPopupMenuUpdateParam _nParam)
        {
            RemoveLastSeparationLine();
            CaleSizeDelta(_nParam);
            CalePosition(_nParam);
            if(_nParam.IsModalUsed)
                AddModalHelper(_nParam);
            else
            {
                Container.SetParent(_nParam.RootCanvas.transform, false);
                Container.SetAsLastSibling();
            }
            m_LastView = _nParam.PopupView;

            Container.gameObject.SetActive(true);
        }

        public bool Contains(TComponent _component)
        {
            if (m_ItemComponent == null || m_ItemComponent.Count <= 0)
                return false;
            return (m_ItemComponent.Contains(_component));
        }

        /// <summary>
        /// 添加显示对象
        /// </summary>
        protected TComponent AddItemComponent(TItem newItem)
        {
            if (DefaultItemComponentRes == null)
                return null;

            TComponent component;
            
            if (m_ItemComponentsCache.Count > 0)
            {
                component = m_ItemComponentsCache[m_ItemComponentsCache.Count - 1];
                m_ItemComponentsCache.RemoveAt(m_ItemComponentsCache.Count - 1);
            }
            else
            {
                component = DefaultItemComponentRes.InstanceMainRes<TComponent>();
                component.transform.SetParent(Container, false);
                Utilites.FixInstantiated(component.gameObject,DefaultItemComponentRes.GetResPosition(), DefaultItemComponentRes.GetResRotation(), DefaultItemComponentRes.GetResScale(), DefaultItemComponentRes.GetResAnchoredPosition(), DefaultItemComponentRes.GetResSizeDelta());
            }

            component.transform.SetAsLastSibling();
            component.gameObject.SetActive(true);
            m_ItemComponent.Add(component);

            newItem.ClearPopupMenuAction = OnClickAction;

            component.onEnter.AddListener(OnEnterAction);
            component.onExit.AddListener(OnExitAction);

            SetData(component, newItem);
            return component;
        }
        /// <summary>
        /// 移除所有显示对象
        /// </summary>
        protected void RemoveItemComponent()
        {
            m_ItemComponent.ForEach(DeactivateComponent);
            m_ItemComponentsCache.AddRange(m_ItemComponent);
            m_ItemComponent.Clear();
        }

        /// <summary>
        /// 添加分割线
        /// </summary>
        /// <param name="newItem"></param>
        protected void AddSeparationLineComponent(TItem newItem)
        {
            if (SeparationLineComponentRes == null)
                return;
            GameObject component;

            if (!m_LineGroupIDDic.ContainsKey(newItem.ItemGroupID))
            {
                if (m_LineComponentsCache.Count > 0)
                {
                    component = m_LineComponentsCache[m_LineComponentsCache.Count - 1];
                    m_LineComponentsCache.RemoveAt(m_LineComponentsCache.Count - 1);
                }
                else
                {
                    component = SeparationLineComponentRes.InstanceMainRes();
                    component.transform.SetParent(Container, false);
                    GameUtil.FixInstantiated(SeparationLineComponentRes, component);
                }
                m_LineGroupIDDic.Add(newItem.ItemGroupID, component);
            }
            else
            {
                component = m_LineGroupIDDic[newItem.ItemGroupID];
            }

            component.transform.SetAsLastSibling();
            component.gameObject.SetActive(true);

        }
        /// <summary>
        /// 移除所有分割线
        /// </summary>
        protected void RemoveSeparationLineComponent()
        {
            m_LineGroupIDDic.ForEach((x) => { if (x.Value != null) x.Value.SetActive(false); });
            m_LineComponentsCache.AddRange(m_LineGroupIDDic.Select(x => x.Value).ToList());
            m_LineGroupIDDic.Clear();
        }
        /// <summary>
        /// 移除指定的分割线
        /// </summary>
        /// <param name="_nIndex">分割线所在的组ID</param>
        protected void RemoveSeparationLineComponent(int _nIndex)
        {
            m_LineGroupIDDic[_nIndex].SetActive(false);
            m_LineComponentsCache.Add(m_LineGroupIDDic[_nIndex]);
            m_LineGroupIDDic.Remove(_nIndex);
        }

        /// <summary>
        /// 移除最后不必要的分割线
        /// </summary>
        protected void RemoveLastSeparationLine()
        {
            if (m_ItemComponent.Count <= 0)
                return;
            int lastItemSiblingIndex = m_ItemComponent[m_ItemComponent.Count - 1].GetTransform.GetSiblingIndex();
            int lastLineSiblingIndex = -1;
            int lineIndex = -1;
            m_LineGroupIDDic.ForEach(x =>
            {
                if (x.Value.transform.GetSiblingIndex() > lastLineSiblingIndex)
                {
                    lastLineSiblingIndex = x.Value.transform.GetSiblingIndex();

                    lineIndex = x.Key;
                }
            });
            if (lastLineSiblingIndex > lastItemSiblingIndex)
            {
                RemoveSeparationLineComponent(lineIndex);
            }
        }

        /// <summary>
        /// 添加模态管理
        /// </summary>
        /// <param name="_nParam"></param>
        protected void AddModalHelper(UPopupMenuUpdateParam _nParam)
        {
            if (_nParam.PopupModalColor == null)
                _nParam.PopupModalColor = new Color(0f, 0f, 0f, 0f);

            if (_nParam.PopupView == null)
            {
                Trace.LogWarning("这里没有赋值Popup所在的View，所以不会进行模态处理。");
                return;
            }

            m_ModalKey = UIWidgets.ModalHelper.Open(this, _nParam.PopupModalSprite, _nParam.PopupModalColor, _nParam.ModalClickAction, Container);

            Container.SetParent(_nParam.RootCanvas.transform, false);
            Container.SetAsLastSibling();

        }
        /// <summary>
        /// 移除模态管理
        /// </summary>
        protected void RemoveModalHelper()
        {
            if (m_ModalKey != null)
            {
                ModalHelper.Close((int)m_ModalKey);
                m_ModalKey = null;
            }
        }

        /// <summary>
        /// 获取Item的高度
        /// </summary>
        protected float GetItemHeight(TComponent item)
        {
            return item.GetHeight;
        }
        /// <summary>
        /// 获取分割线的高度
        /// </summary>
        protected float GetItemHeight(GameObject item)
        {
            Transform tf = item.transform;
            if (tf is RectTransform)
                return (tf as RectTransform).sizeDelta.y;
            return 3.0f;
        }

        /// <summary>
        /// 重新计算显示的长宽
        /// </summary>
        protected void CaleSizeDelta(UPopupMenuUpdateParam _nParam)
        {
            float sumHeight;
            float sumItemComponentHeight = m_ItemComponent.GetRange(0, m_ItemComponent.Count).SumFloat(GetItemHeight);
            float sumSeparationLineComponentHeight = m_LineGroupIDDic.Select(x => x.Value).ToList().GetRange(0, m_LineGroupIDDic.Count).SumFloat(GetItemHeight);

            sumHeight = sumItemComponentHeight + sumSeparationLineComponentHeight + (ListLayout.Spacing.y * (m_ItemComponent.Count + m_LineGroupIDDic.Count));

            Vector2 sizeDelta = Container.sizeDelta;
            sizeDelta.y = sumHeight+4;
            Container.sizeDelta = sizeDelta;
        }
        /// <summary>
        /// 重新计算显示的位置
        /// </summary>
        protected void CalePosition(UPopupMenuUpdateParam _nParam)
        {
            Vector3[] Corners = new Vector3[4];
            Vector3 newPos = Vector3.zero;
            Vector2 newPivot = Vector2.zero;
            UIWidgets.UTooltipExtensionMethods.GetWorldCorners(_nParam.PopupTarget.gameObject,_nParam.RootCanvas, _nParam.UICamera, ref Corners);

            UIWidgets.UTooltipExtensionMethods.SetPosition(_nParam.PopupTipPostion, Corners, Container, _nParam.RootCanvas, _nParam.UICamera, ref newPos);

            //只有子列表不使用模态
            if (!_nParam.IsModalUsed)
            {//次级列表以上级列表组件边框的右上角为锚点计算偏移值
                newPivot = Container.pivot;
                newPivot.y = 1;
                Container.pivot = newPivot;
            }
                

            Container.SetPosition( newPos);
            //如果在窗口边缘则重新计算显示位置
            if (_nParam.PopupOverflowCheck)
            {
                UTooltipOverflow overflow;
                UIWidgets.UTooltipExtensionMethods.GetOverflow(Container, _nParam.RootCanvas, _nParam.UICamera, out overflow);

                if (overflow.IsAny)
                {
                    
                    UIWidgets.UTooltipExtensionMethods.SetPosition(overflow.SuggestNewPosition(_nParam.PopupTipPostion), Corners, Container
                                                                    , _nParam.RootCanvas, _nParam.UICamera, ref newPos);

                    //只有子列表不使用模态
                    if (!_nParam.IsModalUsed)
                    {
                        newPivot = Container.pivot;
                        newPivot.y = 1;
                        Container.pivot = newPivot;
                    }

                    Container.SetPosition(newPos);
                }                 
            }
            //重置一下Z轴，有可能是取边框角顶点时改点Z轴有
            newPos = Container.localPosition;
            newPos.x += 0.5f;
            newPos.y -= 0.5f;
            newPos.z = 0;
            Container.localPosition = newPos;
        }

        protected virtual void SetData(TComponent component, TItem item)
        {

        }
    }

    public class UListPopup : UListPopupBase<UListComponentPopup, UListComponentItemDataPopup>
    {
        protected override void SetData(UListComponentPopup component, UListComponentItemDataPopup item)
        {
            component.SetData(item);
        }
    }
}

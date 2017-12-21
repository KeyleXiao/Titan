using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using USpeedUI.UExtensions;

namespace USpeedUI.GoodsDisposition
{
    public interface IUGD_DragGoods
    {
        int DragGoodsID();
    }
    /// <summary>
    /// 配置界面物品列表显示对象
    /// </summary>
    public class UGD_GoodsItemComponent : ListViewItem, IBeginDragHandler,IEndDragHandler, IUGD_DragGoods,IDragHandler
    {
        /// <summary> 物品图片 </summary>
        public Image GoodsIcon;
        /// <summary> 物品价格 </summary>
        public Text GoodsCost;
        /// <summary> 物品选择边框 </summary>
        public Image GoodsSelectedFrame;


        /// <summary>
        /// 物品tooltip
        /// </summary>
        public UTooltipTrigger GoodsTooltips;
        public String GoodsTooltip_IconParamName = "GoodsIcon";
        public String GoodsTooltip_CostParamName = "GoodsCost";
        public String GoodsTooltip_NameParamName = "GoodsName";
        public String GoodsTooltip_InfoParamName = "GoodsInfo";

        /// <summary>
        /// 当前组件对应的物品ID，控制一些不需要经常更新的数据
        /// </summary>
        [NonSerialized]
        public int CurrentGoodsID = -1;

        public void SetData(SSchemeMobaGoods item)
        {
            //if (item.SchemeGoodsInfo != null && gameObject != null)
            //    Debug.Log(item.SchemeGoodsInfo.GoodsName + ",Enable：" + this.enabled + ",go.active:" + this.gameObject.activeInHierarchy);
            //else
            //    Debug.Log("null 不能设置颜色");
            if (CurrentGoodsID != item.GoodsID)
            {
                CurrentGoodsID = item.GoodsID;
                NonAlwaysChangeData(item);
            }

            AlwaysChangeData(item);

            GoodsTooltips.enabled = CurrentGoodsID > 0;

        }

        protected virtual void AlwaysChangeData(SSchemeMobaGoods item)
        {
            if (GoodsCost != null)
            {
                GoodsCost.text = item.GoodsName +"\n" + item.GoodsCost.ToString();
            }
                

            if (IsTooltipInit())
                GoodsTooltips.SetText(GoodsTooltip_CostParamName, item.GoodsCost.ToString());
        }

        protected virtual void NonAlwaysChangeData(SSchemeMobaGoods item)
        {

            UGoodsDispositionView.GetGoodsIcon(item != null ? item.GoodsIconID : 0, GoodsIcon);



            if (IsTooltipInit() && item != null)
            {
                GoodsTooltips.SetImage(GoodsTooltip_IconParamName, UGoodsDispositionView.GetGoodsIcon(item.GoodsIconID));
                GoodsTooltips.SetText(GoodsTooltip_NameParamName, item.GoodsName);
                string GoodsDes = item.GoodsDes;
                UBBProperty.GetInstance().getRatio(GoodsDes);
                U3D_Render.Common.UBB.toHtml(ref GoodsDes, UBB_FORMAT_TYPE.UGUI);
                GoodsTooltips.SetText(GoodsTooltip_InfoParamName, GoodsDes);
            }
            else
            {
                GoodsTooltips.SetImage(GoodsTooltip_IconParamName, null);
                GoodsTooltips.SetText(GoodsTooltip_NameParamName, "");
                GoodsTooltips.SetText(GoodsTooltip_InfoParamName, "");
            }

        }

        public void SetVisible(bool _bVisible)
        {
            this.gameObject.SetActive(_bVisible);
        }

        protected bool IsTooltipInit()
        {
            if (GoodsTooltips == null || GoodsTooltips.tooltipStyle == null)
            {
                Trace.LogWarning("物品的Tooltip未初始化或TooltipStyle为null。");
                return false;
            }
            return true;
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_BEGINDRAG, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_BEGINDRAG, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }

        public int DragGoodsID()
        {
            return CurrentGoodsID;
        }

        public void OnEndDrag(PointerEventData eventData)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_ENDDRAG, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_ENDDRAG, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }

        public void OnDrag(PointerEventData eventData)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_DRAGGING, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_DRAGGING, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }
    }
    /// <summary>
    /// 配置界面物品列表
    /// </summary>
    public class UGD_GoodsTile : TileView<UGD_GoodsItemComponent, SSchemeMobaGoods>
    {

        protected override void SetData(UGD_GoodsItemComponent component, SSchemeMobaGoods item)
        {
            component.SetData(item);
        }

        public void UpdateVisible()
        {
            var indicies = Enumerable.Range(topHiddenItems, visibleItems).ToArray();
            components.ForEach((x, i) =>
            {
                x.Index = indicies[i];
                SetData(x, DataSource[indicies[i]]);
                Coloring(x as ListViewItem);
            });
        }
    }

    /// <summary>
    /// 配置界面物品合成树显示对象
    /// </summary>
    public class UGD_GoodsTreeNodeComponent : UGD_GoodsItemComponent
    {
        /// <summary>
        /// 连接子节点的垂直线
        /// </summary>
        public Image ChildVerticalLine;
        /// <summary>
        /// 连接父节点的垂直线
        /// </summary>
        public Image ParentVerticalLine;
        /// <summary>已购买标识</summary>
        public Image PurchasedIcon;

        /// <summary>
        /// 节点子节点数
        /// </summary>
        public int ChildCount;

        public float x
        {
            get
            {
                return RectTransform.anchoredPosition.x;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.x = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float y
        {
            get
            {
                return RectTransform.anchoredPosition.y;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.y = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float width
        {
            get
            {
                return RectTransform.rect.width;
            }
            set
            {
                Vector2 vPos = RectTransform.sizeDelta;
                vPos.x = value;
                RectTransform.sizeDelta = vPos;
            }

        }
        public float height
        {
            get
            {
                return RectTransform.rect.height + ChildVerticalLineHeight;//+ ParentVerticalLineHeight;
            }
            set
            {
                Vector2 vPos = RectTransform.sizeDelta;
                vPos.y = value;
                RectTransform.sizeDelta = vPos;
            }

        }

        public float ChildVerticalLineHeight
        {
            get
            {
                if (!ChildVerticalLine.gameObject.activeSelf)
                    return 0;
                return CaleHeight(ChildVerticalLine.rectTransform.rect);
            }
        }

        public float ChildVerticalLineLeft
        {
            get
            {
                return ChildVerticalLine.rectTransform.anchoredPosition.x;
            }
        }
        public float ChildVerticalLineTop
        {
            get
            {
                return ChildVerticalLine.rectTransform.anchoredPosition.y;
            }
        }

        public float ChildVerticalLineRight
        {
            get
            {
                return ChildVerticalLine.rectTransform.rect.xMax + ChildVerticalLine.rectTransform.anchoredPosition.x;
            }
        }

        public float ParentVerticalLineHeight
        {
            get
            {
                if (!ParentVerticalLine.gameObject.activeSelf)
                    return 0;
                return CaleHeight(ParentVerticalLine.rectTransform.rect);
            }
        }

        private float CaleHeight(Rect rect)
        {
            return Math.Abs(rect.y) + rect.height;
        }

        public void SetChildVerticalLineVisible(bool _bChildLine)
        {
            ChildVerticalLine.gameObject.SetActive(_bChildLine);
        }

        public void SetParentVerticalLineVisible(bool _bParentLine)
        {
            ParentVerticalLine.gameObject.SetActive(_bParentLine);
        }

        public void SetPurchasedIconVisible(bool _bPurchased)
        {
            PurchasedIcon.enabled = _bPurchased;
        }

        public void SetVerticalLineVisible(bool _bChildLine, bool _bParentLine)
        {

        }

        protected override void AlwaysChangeData(SSchemeMobaGoods item)
        {
            base.AlwaysChangeData(item);
            if (GoodsCost != null)
            {
                GoodsCost.text = item.GoodsCost.ToString();
            }
        }
    }
    /// <summary>
    /// 配置界面物品合成树节点数据
    /// </summary>
    public class UGD_GoodsTreeNodeData
    {
        public int GoodsDepth;
        public int GoodsChildCount;
        //当前节点的父节点
        public UGD_GoodsTreeNodeData ParentNode;
        //当前节点父节点所占区域宽度
        public int ParentNodeWidth;
        public SSchemeMobaGoods GoodsItem;
        public UGD_GoodsTreeNodeComponent GoodsNodeComponent;
        //当前节点的子节点链表
        public List<UGD_GoodsTreeNodeData> ChildsNodeList;

        public void Clear()
        {
            GoodsDepth = 0;
            GoodsChildCount = 0;
            ParentNode = null;
            GoodsItem = null;
            GoodsNodeComponent = null;
            if (ChildsNodeList != null)
                ChildsNodeList.Clear();
            ParentNodeWidth = 0;
        }
    }

    /// <summary>
    /// 配置界面英雄头像列表显示对象
    /// </summary>
    public class UGD_HeroHeadPortraitItemComponent : ListViewItem
    {
        public Image HeadPortraitImage = null;
        public Text HeroNameText = null;

        public void SetData(UGD_HeroHeadPortraitItem item)
        {
            HeadPortraitImage.sprite = UGoodsDispositionView.GetHeroHeadPortrait(item.HeroHeadPortraitID);
            HeroNameText.text = item.HeroName;
        }

    }
    /// <summary>
    /// 配置界面英雄头像数据
    /// </summary>
    public class UGD_HeroHeadPortraitItem
    {
        public int HeroHeadPortraitID;
        public string HeroName;
        /// <summary>
        /// 是否已购买
        /// </summary>
        public bool IsBuy;

        public void From(GameLogic.cmd_entity_hero_info cmdInfo)
        {
            HeroHeadPortraitID = cmdInfo.nHeroID;
            HeroName = cmdInfo.chHeroName;
            IsBuy = cmdInfo.bIsBuy > 0;
        }

        public void From(GameLogic.cmd_entity_nohanve_hero cmdInfo)
        {
            HeroHeadPortraitID = cmdInfo.nHeroID;
            HeroName = cmdInfo.chHeroName;
            IsBuy = false;
        }
    }
    /// <summary>
    /// 配置界面英雄头像列表
    /// </summary>
    public class UGD_HeroHeadPortraitTile : TileView<UGD_HeroHeadPortraitItemComponent,UGD_HeroHeadPortraitItem>
    {
        protected override void SetData(UGD_HeroHeadPortraitItemComponent component, UGD_HeroHeadPortraitItem item)
        {
            component.SetData(item);
        }

        public void UpdateVisible()
        {
            var indicies = Enumerable.Range(topHiddenItems, visibleItems).ToArray();
            components.ForEach((x, i) =>
            {
                x.Index = indicies[i];
                SetData(x, DataSource[indicies[i]]);
                Coloring(x as ListViewItem);
            });
        }
    }

    /// <summary>
    /// 一个配置物品显示对象
    /// </summary>
    public class UGD_GoodsDispositionRendererBase : UCustomUIBehaviour, IEndDragHandler, IDropHandler,IPointerClickHandler
    {
        protected Image m_nGoodsImage;
        protected Image GoodsImage
        {
            get
            {
                //懒得一个个去拖
                if(m_nGoodsImage == null)
                {
                    GetComponentInChildren<Image>("GoodsImage", out m_nGoodsImage);
                    if (GoodsImage == null)
                        Trace.LogWarning("UGD_GoodsDispositionRendererBase 没找到 GoodsImage对象。 ");
                }
                return m_nGoodsImage;
            }
        }

        /// <summary>
        /// 是否在拖拽中
        /// </summary>
        protected bool m_draging;

        protected int m_nMobaGoodsID = -1;
        public int GoodsID
        {
            get { return m_nMobaGoodsID; }
        }

        public bool ShowTooltip = false;
        public UTooltipTrigger GoodsTooltips = null;
        public String GoodsTooltip_IconParamName = "GoodsIcon";
        public String GoodsTooltip_CostParamName = "GoodsCost";
        public String GoodsTooltip_NameParamName = "GoodsName";
        public String GoodsTooltip_InfoParamName = "GoodsInfo";

        protected override void Start()
        {
            base.Start();

            if(ShowTooltip)
            {
                if (GoodsTooltips == null)
                    GoodsTooltips = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_WarGoods);
            }
            else
            {
                if (GoodsTooltips != null)
                    GoodsTooltips.enabled = false;
            }
        }

        public bool IsValid()
        {
            return GoodsImage.sprite != null;
        }
        
        //public virtual void OnBeginDrag(PointerEventData eventData)
        //{
        //    if (!IsValid())
        //        return;

        //    if(m_draging == false)
        //    {
        //        m_draging = true;
        //        if(IsUseDrag())
        //        {
        //            var rt = DraggingItem.gameObject.GetComponent<RectTransform>();
        //            Vector3 globalMousePos;
        //            if (RectTransformUtility.ScreenPointToWorldPointInRectangle(DraggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
        //            {
        //                rt.SetPosition(globalMousePos);
        //                rt.rotation = DraggingTransform.rotation;
        //            }
        //        }
        //    }
        //}

        //public virtual void OnDrag(PointerEventData eventData)
        //{
        //    if(m_draging && IsUseDrag())
        //    {
        //        var rt = DraggingItem.gameObject.GetComponent<RectTransform>();
        //        Vector3 globalMousePos;
        //        if (RectTransformUtility.ScreenPointToWorldPointInRectangle(DraggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
        //        {
        //            rt.SetPosition(globalMousePos);
        //            rt.rotation = DraggingTransform.rotation;
        //        }
        //    }
        //}

        public virtual void OnDrop(PointerEventData eventData)
        {

        }

        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;
            }
        }

        public virtual void SetData(int _newMobaGoodsID)
        {
            if (_newMobaGoodsID == m_nMobaGoodsID)
                return;

            m_nMobaGoodsID = _newMobaGoodsID;
            GoodsImage.sprite = UGoodsDispositionView.GetGoodsIconByGoodsID(m_nMobaGoodsID);

            if(ShowTooltip)
            {
                if (GoodsTooltips == null)
                    GoodsTooltips = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_WarGoods);

                SSchemeMobaGoods goodsSScheme;
                GoodsSchemesDataManager.Instance.GetMobaGoods_Out(_newMobaGoodsID, out goodsSScheme);
                if (goodsSScheme != null && _newMobaGoodsID != 0 && GoodsTooltips != null)
                {
                    GoodsTooltips.enabled = true;
                    GoodsTooltips.SetImage(GoodsTooltip_IconParamName, UGoodsDispositionView.GetGoodsIcon(goodsSScheme.GoodsIconID));
                    GoodsTooltips.SetText(GoodsTooltip_NameParamName, goodsSScheme.GoodsName);
                    string GoodsDes = goodsSScheme.GoodsDes;
                    UBBProperty.GetInstance().getRatio(GoodsDes);
                    U3D_Render.Common.UBB.toHtml(ref GoodsDes, UBB_FORMAT_TYPE.UGUI);
                    GoodsTooltips.SetText(GoodsTooltip_InfoParamName, GoodsDes);
                    GoodsTooltips.SetText(GoodsTooltip_CostParamName, goodsSScheme.GoodsCost.ToString());
                }
                else
                    GoodsTooltips.enabled = false;
            }
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
        }
    }
    /// <summary>
    /// 当前配置物品组显示对象的单个配置物品显示对象
    /// </summary>
    public class UGD_GoodsDispositionRendererCurrent : UGD_GoodsDispositionRendererBase,IBeginDragHandler, IUGD_DragGoods, IDragHandler
    {
        /// <summary>
        /// 槽位序号
        /// </summary>
        public int SlotID;

        public int DragGoodsID()
        {
            return GoodsID;
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            if (DragGoodsID() <= 0)
                return;

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_BEGINDRAG, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_BEGINDRAG, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }

        public override void OnEndDrag(PointerEventData eventData)
        {
            base.OnEndDrag(eventData);
            ClearData();

            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_ENDDRAG, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_ENDDRAG, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }

        public void OnDrag(PointerEventData eventData)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_DRAGGING, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_DRAGGING, DragGoodsID(), string.Empty, IntPtr.Zero, 0));
        }

        public override void OnDrop(PointerEventData eventData)
        {
            if (eventData.pointerDrag == null)
                return;

            IUGD_DragGoods drag = eventData.pointerDrag.gameObject.GetComponent<IUGD_DragGoods>();

            if (drag == null)
                return;

            SetData(drag.DragGoodsID());

            DataCenter.GoodsDispositionDataManager.Instance.CurrentGoodsDisposition.GoodsList[SlotID] = GoodsID;

            base.OnDrop(eventData);
            
        }

        public override void OnPointerClick(PointerEventData eventData)
        {

            //右键取消
            if(eventData.button == PointerEventData.InputButton.Right)
            {
                ClearData();
            }
            else if(eventData.button == PointerEventData.InputButton.Left)
            {
                if(GoodsID != 0)
                {
                    GoodsDispositionDataManager.Instance.SetCurrentSelectGoodsID(GoodsID);
                }
            }
        }

        protected void ClearData()
        {
            SetData(0);

            DataCenter.GoodsDispositionDataManager.Instance.CurrentGoodsDisposition.GoodsList[SlotID] = GoodsID;
        }
    }



    /// <summary>
    /// 拖拽时随鼠标移动的对象
    /// </summary>
    public class UGD_GoodsDispositionRendererDraggingItem : UIBehaviour
    {
        public Image DraggingImage;

        public void SetData(Sprite _sprDragging)
        {
            DraggingImage.sprite = _sprDragging;
        }

        public void ResetData()
        {
            DraggingImage.sprite = null;
        }
    }

    /// <summary>
    /// 一组配置物品数据
    /// </summary>
    public class UGD_GoodsDispositionItem
    {
        public int HeroID;
        public int DispositionID;
        public string Name;
        public string Author;
        public string Rank;
        public bool IsCurrent;
        public int[] GoodsList = new int[12];

        public GameLogic.cmd_war_equipscheme To()
        {
            GameLogic.cmd_war_equipscheme scheme = new GameLogic.cmd_war_equipscheme();

            scheme.bySchemeID = (byte)DispositionID;
            scheme.szAuthorName = Author;
            scheme.szSchemeName = Name;
            scheme.nEquipIDList = new int[12];
            scheme.nHeroID = HeroID;
            GoodsList.CopyTo(scheme.nEquipIDList, 0);

            return scheme;
        }

        public void From(GameLogic.cmd_war_equipscheme _scheme,bool _isCurrent = false)
        {
            this.DispositionID = _scheme.bySchemeID;
            this.Name = _scheme.szSchemeName;
            this.Author = _scheme.szAuthorName;
            if(_scheme.nEquipIDList != null)
                _scheme.nEquipIDList.CopyTo(this.GoodsList, 0);
            this.IsCurrent = _isCurrent;
            this.HeroID = _scheme.nHeroID;
        }

        public void Clone(ref UGD_GoodsDispositionItem _target)
        {
            _target.DispositionID = this.DispositionID;
            _target.Name = this.Name;
            _target.Author = this.Author;
            _target.IsCurrent = this.IsCurrent;
            _target.Rank = this.Rank;
            _target.HeroID = this.HeroID;
            if (_target.GoodsList == null)
                _target.GoodsList = new int[12];
            this.GoodsList.CopyTo(_target.GoodsList, 0);
        }

        public void Clear()
        {
            Name = string.Empty;
            Author = string.Empty;
            GoodsList = new int[12];
        }
    }
    /// <summary>
    /// 一组配置物品显示对象
    /// </summary>
    public class UGD_GoodsDispositionItemComponent : ListViewItem
    {
        public int DispositionID;
        public Text NameTxt;
        public Text CreatorTxt;
        public Button StateBtn;
        public Button RemoveBtn;
        public Text StateBtnTxt;
        public Button EditorNameBtn;
        public List<Image> EerlyGoodsImgList = new List<Image>();
        public List<Image> CoreGoodsImgList = new List<Image>();

        protected override void Start()
        {
            base.Start();
            if(StateBtn != null)
            {
                StateBtn.onClick.AddListener(OnStateBtnClick);
            }
            if(RemoveBtn != null)
            {
                RemoveBtn.onClick.AddListener(OnRemoveBtnClick);
            }
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            if (StateBtn != null)
            {
                StateBtn.onClick.RemoveListener(OnStateBtnClick);
            }
        }

        public void SetData(UGD_GoodsDispositionItem _itemData)
        {
            NameTxt.text = _itemData.Name;
            CreatorTxt.text = _itemData.Author;

            SetGoods(_itemData);

            StateBtnTxt.text = _itemData.IsCurrent ? ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "DispositionSelect") : ULocalizationService.Instance.Get("UIView", "GoodsDispositionView", "DispositionUnSelect");
            StateBtn.interactable = _itemData.IsCurrent ? false : true;
            DispositionID = _itemData.DispositionID;
        }

        protected void OnStateBtnClick()
        {
            GoodsDispositionDataManager.Instance.CurrentGoodsDispositionID = DispositionID;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION, WndMsgID.WND_MSG_GOODSDISPOSITION_CLOSE_GOODSDISPOSITIONLIST, null);
        }

        protected void OnRemoveBtnClick()
        {
            GoodsDispositionDataManager.Instance.RemoveHeroGoodsDisposition(0,DispositionID);
        }

        protected void SetGoods(UGD_GoodsDispositionItem _itemData)
        {
            for (int i = 0; i < _itemData.GoodsList.Length; ++i)
            {
                if(i<6)
                {
                    EerlyGoodsImgList[i].sprite = UGoodsDispositionView.GetGoodsIconByGoodsID(_itemData.GoodsList[i]);
                }
                else
                {
                    CoreGoodsImgList[i - 6].sprite = UGoodsDispositionView.GetGoodsIconByGoodsID(_itemData.GoodsList[i]);
                }
            }
        }
    }

    /// <summary>
    /// 多组配置物品列表
    /// </summary>
    public class UGD_GoodsDispositionList : ListViewCustom<UGD_GoodsDispositionItemComponent,UGD_GoodsDispositionItem>
    {
        protected override void SetData(UGD_GoodsDispositionItemComponent component, UGD_GoodsDispositionItem item)
        {
            component.SetData(item);
        }
    }


    #region 物品配置已满时显示的删除物品配置列表
    public class UGD_DispositionFilledItemComponent : ListViewItem
    {
        public Text NameTxt = null;
        public Text AuthorTxt = null;
        public Text RankTxt = null;
        public Button DeleteBtn = null;
        public int DispositionID;
        public int HeroID;

        protected override void Start()
        {
            base.Start();
            if (DeleteBtn != null)
            {
                DeleteBtn.onClick.AddListener(OnDeleteBtnClick);
            }
        }


        public void SetData(UGD_GoodsDispositionItem _itemData)
        {
            SetText(NameTxt, _itemData.Name);
            SetText(AuthorTxt, _itemData.Author);
            SetText(RankTxt, _itemData.Rank);
            DispositionID = _itemData.DispositionID;
            HeroID = _itemData.HeroID;
        }
        protected void SetText(Text _txt,string _strTxt)
        {
            if (_txt != null)
                _txt.text = _strTxt;
        }
        protected void OnDeleteBtnClick()
        {
            GoodsDispositionDataManager.Instance.RemoveHeroGoodsDisposition(HeroID,DispositionID);
        }
    }
    public class UGD_DispositionFilledList : ListViewCustom<UGD_DispositionFilledItemComponent, UGD_GoodsDispositionItem>
    {
        protected override void SetData(UGD_DispositionFilledItemComponent component, UGD_GoodsDispositionItem item)
        {
            component.SetData(item);
        }

    }
    #endregion
}

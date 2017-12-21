using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using ASpeedGame.Data.RunePage;
using DataCenter;
using U3D_Render.Common;
using USpeedUI.PlayerFrame;
namespace USpeedUI.UWidgets
{
    public class URuneTypeTreeItemComponent : TreeViewComponentBase<URuneTypeTreeItemData>//, IPointerClickHandler
    {
        URuneTypeTreeItemData item;
        UTooltipTrigger triggerTip;
        public float fLastClkCount = 0;

        protected override void Start()
        {
            base.Start();
        }

        protected override void Awake()
        {
            if ((LocalPositionZReset) && (transform.localPosition.z != 0f))
            {
                var pos = transform.localPosition;
                pos.z = 0f;
                transform.localPosition = pos;
            }
        }
		
        /// <summary>
        /// Gets or sets the item.
        /// </summary>
        /// <value>The item.</value>
        public URuneTypeTreeItemData Item
        {
            get
            {
                return item;
            }
            set
            {
                if (item != null)
                {
                    item.OnChange -= UpdateView;
                }
                item = value;
                if (item != null)
                {
                    item.OnChange += UpdateView;
                }
                UpdateView();
            }
        }
        private int nItemDepth;
        public int ItemDepth
        {
            get { return nItemDepth; }
            protected set
            {
                nItemDepth = value;

                if(triggerTip == null)
                    triggerTip = this.gameObject.GetComponent<UTooltipTrigger>();

                if (nItemDepth == 0)
                {
                    triggerTip.enabled = false;
                }
                else
                    triggerTip.enabled = true;
            }

        }

        public Text ValueText;

        public override void SetData(TreeNode<URuneTypeTreeItemData> newNode, int depth)
        {
            Node = newNode;
            ItemDepth = depth;
            base.SetData(Node, depth);
           // SetRuneDesc();
          
            Item = (Node == null) ? null : Node.Item;
        }

        /// <summary>
        /// Updates the view.
        /// </summary>
        protected virtual void UpdateView()
        {
            if ((Icon == null) || (Text ==  null) || (ValueText == null))
            {
                return;
            }

            if (ItemDepth > 0)
            {
                ValueText.text = "x " + Item.Value;
                triggerTip.SetText(UTooltipParamName.BodyText, Item.sRuneDesc);
                this.RectTransform.sizeDelta = new Vector2(260, 66);
                this.transform.FindChild("Toggle").GetComponent<RectTransform>().sizeDelta = this.RectTransform.sizeDelta;
            }
            else
            {
                ValueText.text = "";
                this.RectTransform.sizeDelta = new Vector2(260, 30);
                this.transform.FindChild("Toggle").GetComponent<RectTransform>().sizeDelta = this.RectTransform.sizeDelta;
            }

            if (Item == null)
            {
                Icon.sprite = null;
                Text.text = string.Empty;
            }
            else
            {
                Icon.sprite = Item.Icon;
                Text.text = Item.LocalizedName ?? Item.Name;
            }

            //if (SetNativeSize)
            //{
            //   // Icon.SetNativeSize();
               
            //}

            //set transparent color if no icon
            Icon.color = (Icon.sprite == null) ? Color.clear : Color.white;
        }

        /// <summary>
        /// Called when item moved to cache, you can use it free used resources.
        /// </summary>
        public override void MovedToCache()
        {
            if (Icon != null)
            {
                Icon.sprite = null;
            }
        }

        /// <summary>
        /// This function is called when the MonoBehaviour will be destroyed.
        /// </summary>
        protected override void OnDestroy()
        {
            if (item != null)
            {
                item.OnChange -= UpdateView;
            }
            base.OnDestroy();
        }

        public void SetRuneDesc()
        {
            if(ItemDepth<=0)
            {
                return;
            }

            SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[item.nRuneId] as SSchemeRuneConfig;
            if (runeConfig == null)
            {
                triggerTip.SetText(UTooltipParamName.BodyText, "EmptyDesc");
                return;
            }

            if (runeConfig.szParametersValueArray == "")
            {
                triggerTip.SetText(UTooltipParamName.BodyText, "EmptyDesc");
                return;
            }

            UBB.toHtml(ref runeConfig.szRuneDESC, UBB_FORMAT_TYPE.UGUI);
            triggerTip.SetText(UTooltipParamName.BodyText, runeConfig.szRuneDESC);
        }

        public  override void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button == PointerEventData.InputButton.Right)
            {
                //RunePageWnd runePageWnd = ((RunePageWnd)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYER_RUNETPAGE));
                UPlayerFrame playerFrameWnd = (UPlayerFrame)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYERFRAME);
                RunePageWnd runePageWnd = playerFrameWnd.GetSubWnd(PlayerSubWndType.WND_RUNE) as RunePageWnd;
                runePageWnd.SetPutRuneInfo(eventData);
            }

            if (eventData.button == PointerEventData.InputButton.Left)
            {
                float nCurClkCount = eventData.clickTime;
                if (nCurClkCount - this.fLastClkCount < 1)
                {
                    //RunePageWnd runePageWnd = ((RunePageWnd)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYER_RUNETPAGE));
                    UPlayerFrame playerFrameWnd = (UPlayerFrame)UISystem.Instance.GetWnd(WndID.WND_ID_PLAYERFRAME);
                    RunePageWnd runePageWnd = playerFrameWnd.GetSubWnd(PlayerSubWndType.WND_RUNE) as RunePageWnd;
                    runePageWnd.SetPutRuneInfo(eventData);
                    fLastClkCount = 0;
                }
                else
                {
                    this.fLastClkCount = nCurClkCount;
                }
            }
        }
  }
}

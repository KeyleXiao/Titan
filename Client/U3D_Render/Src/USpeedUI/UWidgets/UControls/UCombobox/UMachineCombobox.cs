using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;
using UnityEngine.EventSystems;

namespace UIWidgetsMachines
{
    public class ComboboxMachineListViewComponent:ListViewItem
    {
        ComboboxMachineListViewData item;

        /// <summary>
        /// Gets the current item.
        /// </summary>
        /// <value>Current item.</value>
        public ComboboxMachineListViewData Item
        {
            get
            {
                return item;
            }
        }
        /// <summary>
        /// The text.
        /// </summary>
        [SerializeField]
        public Text ComboboxText;
        [SerializeField]
        public Image MachineImage;
        [SerializeField]
        public GameObject Mask;
        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="newItem">Item.</param>
        public virtual void SetData(ComboboxMachineListViewData newItem)
        {
            item = newItem;
            if(item == null)
            {
                ComboboxText.text = string.Empty;
            }
            else
            {
                ComboboxText.text = item.sMachineDesc;
                MachineImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 3, newItem.nHeroId);
                Mask.gameObject.SetActive(newItem.bLock);
                if (newItem.bLock)
                {
                    ComboboxText.color = Color.gray;
                }
                else
                {
                    ComboboxText.color = Color.white;
                }
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (Item.bLock)
            {
                return;
            }

            base.OnPointerClick(eventData);
        }
    }
    [System.Serializable]
    public class ComboboxMachineListViewData
    {
        [SerializeField]
        public string sMachineDesc;
        [SerializeField]
        public int  nHeroId;
        [SerializeField]
        public bool bLock;
    }

    public class ComboboxMachineListView : ListViewCustom<ComboboxMachineListViewComponent,ComboboxMachineListViewData>
    {
        protected override void SetData(ComboboxMachineListViewComponent component, ComboboxMachineListViewData item)
        {
            component.SetData(item);
        }
    }

    public class ComboboxMachine : ComboboxCustom<ComboboxMachineListView,ComboboxMachineListViewComponent,ComboboxMachineListViewData>
    {
        /// <summary>
        /// Updates the current component with selected item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>
        protected override void SetData(ComboboxMachineListViewComponent component, ComboboxMachineListViewData item)
        {
            component.SetData(item);
        }
    }
}

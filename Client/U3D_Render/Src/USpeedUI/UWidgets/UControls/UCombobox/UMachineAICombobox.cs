using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;
using UnityEngine.EventSystems;

namespace UIWidgetsMachineAIs
{
    public class ComboboxMachineAIListViewComponent:ListViewItem
    {
        private ComboboxMachineAIListViewData item;
        public ComboboxMachineAIListViewData Item {get {return item;}}

        [SerializeField]
        public Text ComboboxText;
        [SerializeField]
        public Image MachineAIImage;
        [SerializeField]
        public Image ComboboxIcon;
        [SerializeField]
        public Button ComboboxBtn;

        public virtual void SetData(ComboboxMachineAIListViewData newItem)
        {
            item = newItem;
            if(item == null)
            {
                ComboboxText.text = string.Empty;
                return;
            }

            ComboboxText.text = item.sMachineAIDesc;
            if (ComboboxIcon != null)
                ComboboxIcon.gameObject.SetActive(item.nShowCombo > 0);
            if (ComboboxBtn != null)
                ComboboxBtn.interactable = item.nShowCombo > 0;
        }
    }


    [System.Serializable]
    public class ComboboxMachineAIListViewData
    {
        [SerializeField]
        public string sMachineAIDesc;
        [SerializeField]
        public int nHeroID;
        [SerializeField]
        public int nAIID;
        [SerializeField]
        public int nShowCombo;
    }

    public class ComboboxMachineAIListView : ListViewCustom<ComboboxMachineAIListViewComponent,ComboboxMachineAIListViewData>
    {
        protected override void SetData(ComboboxMachineAIListViewComponent component, ComboboxMachineAIListViewData item)
        {
            component.SetData(item);
        }
    }

    public class ComboboxMachineAI : ComboboxCustom<ComboboxMachineAIListView,ComboboxMachineAIListViewComponent,ComboboxMachineAIListViewData>
    {
        protected override void SetData(ComboboxMachineAIListViewComponent component, ComboboxMachineAIListViewData item)
        {
            component.SetData(item);
        }
    }
}

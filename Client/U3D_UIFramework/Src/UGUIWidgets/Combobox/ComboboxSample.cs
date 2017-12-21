using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgetsSamples
{
    public class ComboboxSampleListViewComponent:ListViewItem
    {
        protected ComboboxSampleListViewData item;

        /// <summary>
        /// Gets the current item.
        /// </summary>
        /// <value>Current item.</value>
        public ComboboxSampleListViewData Item
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

        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="newItem">Item.</param>
        public virtual void SetData(ComboboxSampleListViewData newItem)
        {
            item = newItem;
            if(item == null)
            {
                ComboboxText.text = string.Empty;
            }
            else
            {
                ComboboxText.text = item.ComboboxText;
            }
        }
    }
    [System.Serializable]
    public class ComboboxSampleListViewData
    {
        [SerializeField]
        public string ComboboxText;
    }

    public class ComboboxSampleListView : ListViewCustom<ComboboxSampleListViewComponent,ComboboxSampleListViewData>
    {
        protected override void SetData(ComboboxSampleListViewComponent component, ComboboxSampleListViewData item)
        {
            component.SetData(item);
        }
    }

    public class ComboboxSample : ComboboxCustom<ComboboxSampleListView,ComboboxSampleListViewComponent,ComboboxSampleListViewData>
    {
        /// <summary>
        /// Updates the current component with selected item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>
        protected override void SetData(ComboboxSampleListViewComponent component, ComboboxSampleListViewData item)
        {
            component.SetData(item);
        }
    }
}

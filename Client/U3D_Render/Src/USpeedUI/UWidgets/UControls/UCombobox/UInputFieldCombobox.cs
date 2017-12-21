using ASpeedGame.Data.GameSettingConfig;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    public class UInputFieldComboboxComponent : ListViewItem
    {
        UInputFieldComboboxListViewData item;

        /// <summary>
        /// 当前显示对象使用的数据
        /// </summary>
        /// <value>Current item.</value>
        public UInputFieldComboboxListViewData Item
        {
            get
            {
                return item;
            }
        }

        [SerializeField]
        public Text ComboboxText;
        [SerializeField]
        public InputField ComboboxInputField;

        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="newItem">Item.</param>
        public virtual void SetData(UInputFieldComboboxListViewData newItem)
        {
            item = newItem;
            if (item == null)
            {
                SetText(string.Empty);
            }
            else
            {
                SetText(item.ComboboxText);
            }
        }

        protected virtual void SetText(string _strData)
        {
            if (ComboboxText != null)
                ComboboxText.text = _strData;
            if (ComboboxInputField != null)
                ComboboxInputField.text = _strData;
        }
    }

    [System.Serializable]
    public class UInputFieldComboboxListViewData
    {
        [SerializeField]
        public string ComboboxText;

        public UInputFieldComboboxListViewData() { }

        public UInputFieldComboboxListViewData(string _strText)
        {
            ComboboxText = _strText;
        }
    }

    public class UInputFieldComboboxListView : ListViewCustom<UInputFieldComboboxComponent,UInputFieldComboboxListViewData>
    {
        protected override void SetData(UInputFieldComboboxComponent component, UInputFieldComboboxListViewData item)
        {
            component.SetData(item);
        }

    }

    public class UInputFieldCombobox : ComboboxCustom<UInputFieldComboboxListView, UInputFieldComboboxComponent, UInputFieldComboboxListViewData>
    {
        protected override void SetData(UInputFieldComboboxComponent component, UInputFieldComboboxListViewData item)
        {
            component.SetData(item);
        }

        public UInputFieldComboboxComponent GetComponent(int nIndex)
        {
            return components[nIndex];
        }

        public override void ShowList()
        {
            if (listView == null)
            {
                return;
            }

            listView.gameObject.SetActive(true);

            ModalKey = ModalHelper.Open(this, null, new Color(0, 0, 0, 0f), HideList, listView.transform as RectTransform);

            PopupListView();

            if (listView.Layout != null)
            {
                listView.Layout.UpdateLayout();
            }
        }
    }
}

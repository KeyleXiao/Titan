using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    public class UShowAllItemComponent : ListViewItem
    {
        [SerializeField]
        public Text DisplayText1;

        public void SetData(UShowAllItemData _data)
        {
            DisplayText1.text = _data.DisplayText1;
        }
    }

    [Serializable]

    public class UShowAllItemData : IItemHeight
    {
        public float Height
        {
            get;

            set;
        }

        [SerializeField]
        public string DisplayText1;
    }

    public class UShowAllItemList : ListViewCustomHeight<UShowAllItemComponent,UShowAllItemData>
    {
        private RectTransform m_CurrentTrans;
        protected RectTransform CurrentTrans
        {
            get
            {
                if(m_CurrentTrans == null)
                {
                    m_CurrentTrans = this.transform as RectTransform;
                }
                return m_CurrentTrans;
            }
        }

        public override int Add(UShowAllItemData item)
        {
            int resIndex = base.Add(item);

            //重新计算背景高度
            float newHeight = GetItemsHeight(0, GetItemsCount());
            Debug.Log("ItemsHeight::" + newHeight);
            Vector2 sizeDelta = CurrentTrans.sizeDelta;
            sizeDelta.y = newHeight;
            CurrentTrans.sizeDelta = sizeDelta;
            return resIndex;
        }

        protected override void SetData(UShowAllItemComponent component, UShowAllItemData item)
        {
            component.SetData(item);
        }

        public override void UpdateItems()
        {
            base.UpdateItems();
            //重新计算背景高度
            float newHeight = GetItemsHeight(0, GetItemsCount());
            Debug.Log("ItemsHeight::" + newHeight);
            Vector2 sizeDelta = CurrentTrans.sizeDelta;
            sizeDelta.y = newHeight;
            CurrentTrans.sizeDelta = sizeDelta;
        }
    }
}

/*******************************************************************
** 文件名:	UCheckboxTreeViewComponent.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-11
** 版  本:	1.0
** 描  述:	带勾选功能的树状控件，显示对象
            1、增加类似数据驱动功能，数据改变时会刷新当前显示对象显示内容
********************************************************************/
using System;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    [Serializable]
    public class UCheckboxTreeViewComponent<TItem> : TreeViewComponentBase<TItem>
        where TItem : UCheckBoxTreeViewItem
    {
        [SerializeField]
        public Toggle CheckBox;
        [Serializable]
        public class NodeCheckBoxEvent : UnityEvent<int, bool>
        {
        }
        public NodeCheckBoxEvent onNodeCBValueChange = new NodeCheckBoxEvent();

        private TItem m_item;
        public TItem Item
        {
            get
            {
                return m_item;
            }
            set
            {
                if (m_item != null)
                {
                    m_item.OnChange -= UpdateView;
                }
                m_item = value;
                if (m_item != null)
                {
                    m_item.OnChange += UpdateView;
                }
                UpdateView();
            }
        }



        protected override void Start()
        {
            base.Start();
            if (CheckBox != null)
            {
                CheckBox.onValueChanged.AddListener(OnCheckBoxValueChange);
            }
        }

        protected override void OnDestroy()
        {
            if (CheckBox != null)
            {
                CheckBox.onValueChanged.RemoveListener(OnCheckBoxValueChange);
            }

            if (m_item != null)
            {
                m_item.OnChange -= UpdateView;
            }
            base.OnDestroy();
        }

        protected void OnCheckBoxValueChange(bool _bValue)
        {
            onNodeCBValueChange.Invoke(Index, _bValue);
        }

        public override void SetData(TreeNode<TItem> node, int depth)
        {
            base.SetData(node, depth);

            Item = (Node == null) ? null : Node.Item as TItem;
        }
        protected virtual void UpdateView()
        {
            if (Icon != null)
            {
                Icon.sprite = Item == null ? null : Item.Icon;
                Icon.color = (Icon.sprite == null) ? Color.clear : Color.white;
            }

            if (Text != null)
            {
                Text.text = Item == null ? string.Empty : Item.LocalizedName ?? Item.Name;
            }

            if (CheckBox != null)
            {
                {
                    //恶心的不提供设置isOn不分发事件的方法，还不能反射
                    //这里不希望设置勾选时分发事件
                    CheckBox.onValueChanged.RemoveListener(OnCheckBoxValueChange);
                    CheckBox.isOn = (Item.IsCheckBoxOn);
                    CheckBox.onValueChanged.AddListener(OnCheckBoxValueChange);
                }
                //父节点不显示勾选框
                CheckBox.gameObject.SetActive((Node.Parent != null && Node.Parent.Item != null) ? true : false);

            }
        }

        public override void MovedToCache()
        {
            if (Icon != null)
            {
                Icon.sprite = null;
            }
        }

    }
}

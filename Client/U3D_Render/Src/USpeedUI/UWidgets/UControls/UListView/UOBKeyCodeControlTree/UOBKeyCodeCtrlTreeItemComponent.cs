/*******************************************************************
** 文件名:	UOBKeyCodeCtrlTreeItemComponent.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB键位设置分组列表组件
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;

namespace USpeedUI.UWidgets.UOBKeyCodeCtrl
{
    public class UOBKeyCodeCtrlTreeItemComponent : TreeViewComponentBase<UOBKeyCodeCtrlTreeItemData>
    {
        public UOBKeyCodeCtrlTreeItemData Item
        {
            get { return item; }
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
        private UOBKeyCodeCtrlTreeItemData item;
        public int ItemDepth
        {
            get { return nItemDepth; }
            protected set { nItemDepth = value; }
        }
        private int nItemDepth;

        public Image ToggleIcon;
        public Image ParentBG;
        public Image ChildBG;
        public Text ItemLabel;
        public Text ItemKeyCode;

        public override void SetData(TreeNode<UOBKeyCodeCtrlTreeItemData> node, int depth)
        {
            Node = node;
            ItemDepth = depth;
            base.SetData(Node, depth);

            Item = (Node == null) ? null : Node.Item;
        }

        protected virtual void UpdateView()
        {
            ToggleIcon.gameObject.SetActive(ItemDepth == 0);
            ParentBG.gameObject.SetActive(ItemDepth == 0);
            ChildBG.gameObject.SetActive(ItemDepth > 0);
            ItemKeyCode.gameObject.SetActive(ItemDepth > 0);

            if (ItemDepth > 0)
            {
                ItemLabel.text = Item.szLabel;
                string szCode = Item.szKeyCode;
                szCode = szCode.Replace(";", "+");
                ItemKeyCode.text = szCode;
            }
            else
            {
                ItemLabel.text = Item.szGroupName;
            }
        }

        protected override void Start()
        {
            base.Start();

            if(Toggle != null)
            {
                Toggle.OnClick.RemoveListener(ToggleNode);
            }

            onClick.AddListener(ToggleNodeEX);
        }

        protected override void OnDestroy()
        {
            onClick.RemoveListener(ToggleNodeEX);
            base.OnDestroy();
        }

        private void ToggleNodeEX()
        {
            if (ItemDepth == 0)
                ToggleNode();
        }
    }
}

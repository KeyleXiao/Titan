/*******************************************************************
** 文件名:	UCheckBoxTreeView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-11
** 版  本:	1.0
** 描  述:	扩展树状UI，增加勾选功能
            1、支持多勾选
            2、一次性获取当前所有已勾选节点数据
            3、清除所有已勾选节点数据
********************************************************************/
using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;

namespace USpeedUI.UWidgets
{
    public class UCheckBoxTreeView<TComponent, TItem>
    : TreeViewCustom<TComponent, TItem>
        where TItem : UCheckBoxTreeViewItem
        where TComponent : UCheckboxTreeViewComponent<TItem>
               // where TItem : UCheckBoxTreeViewItem
    {
        [Serializable]
        public class NodeCheckEvent : UnityEvent<TreeNode<TItem>,bool>
        {

        }


        /// <summary>
        /// 外部侦听勾选事件
        /// </summary>
        public NodeCheckEvent NodeCheck = new NodeCheckEvent();

        /// <summary>
        /// 显示组件勾选时回调方法
        /// </summary>
        /// <param name="_nIndex"></param>
        /// <param name="_bCheck"></param>
        protected virtual void OnComponentCheckBoxValueChange(int _nIndex ,bool _bCheck)
        {      
            NodeCBValueChange(_nIndex, _bCheck);
            NodeCheck.Invoke(NodesList[_nIndex].Node, _bCheck);
        }
        /// <summary>
        /// 处理节点逻辑上的勾选
        /// </summary>
        /// <param name="_nIndex"></param>
        /// <param name="_bCheck"></param>
        protected virtual void NodeCBValueChange(int _nIndex,bool _bCheck)
        {
            //当前开关按钮的节点
            var node = NodesList[_nIndex];

            NodesList.BeginUpdate();
            //数据上改变
            (node.Node.Item as UCheckBoxTreeViewItem).IsCheckBoxOn = _bCheck;

            //选择父节点时把子节点都选上
            //if(node.Node.Nodes != null && node.Node.Nodes.Count > 0)
            //{
            //    for (int i = 0; i < node.Node.Nodes.Count; ++i) 
            //    {
            //        (node.Node.Nodes[i].Item as UCheckBoxTreeViewItem).IsCheckBoxOn = _bCheck;
            //    }
            //}

            //修改父节点的勾选状态
            if (node.Node.Parent != null && node.Node.Parent.Item != null)
            {
                bool _bParentCheck = _bCheck;
                {
                    if (_bCheck == false)
                    {
                        for (int i = 0; i < node.Node.Parent.Nodes.Count; ++i)
                        {
                            if (node.Node.Parent.Nodes[i].Item.IsCheckBoxOn == true)
                            {
                                _bParentCheck = true;
                                break;
                            }
                        }
                    }
                    node.Node.Parent.Item.IsCheckBoxOn = _bParentCheck;
                }
            }
            NodesList.EndUpdate();
        }

        protected override void AddCallback(ListViewItem item, int index)
        {
            base.AddCallback(item, index);

            (item as UCheckboxTreeViewComponent<TItem>).onNodeCBValueChange.AddListener(OnComponentCheckBoxValueChange);

        }
        protected override void RemoveCallback(ListViewItem item, int index)
        {
            base.RemoveCallback(item, index);

            (item as UCheckboxTreeViewComponent<TItem>).onNodeCBValueChange.RemoveListener(OnComponentCheckBoxValueChange);
        }

        protected override void SetData(TComponent component, ListNode<TItem> item)
        {
            base.SetData(component, item);
        }

        /// <summary>
        /// 获得当前所有勾选的数据
        /// </summary>
        /// <returns></returns>
        public virtual List<TItem> GetAllCheckBoxSelectedNode()
        {
            List<TItem> result = new List<TItem>();

            foreach(var node in NodesList)
            {
                if(node.Node != null)
                {
                    if (node.Node.Item != null && (node.Node.Item as UCheckBoxTreeViewItem).IsCheckBoxOn && !result.Contains(node.Node.Item))
                    {
                        result.Add(node.Node.Item);
                    }

                    if (node.Node.Nodes != null)
                    {
                        for (int i = 0; i < node.Node.Nodes.Count; ++i)
                        {
                            if((node.Node.Nodes[i].Item as UCheckBoxTreeViewItem).IsCheckBoxOn && !result.Contains(node.Node.Nodes[i].Item))
                            {
                                result.Add(node.Node.Nodes[i].Item);
                            }
                        }
                    }
                }
            }

            return result;
        }
        /// <summary>
        /// 清除所有勾选节点
        /// </summary>
        public virtual void ClearAllCheckBoxSelectedNode()
        {
            NodesList.BeginUpdate();

            foreach (var node in NodesList)
            {
                if(node.Node != null && node.Node.Item != null)
                {
                    (node.Node.Item as UCheckBoxTreeViewItem).IsCheckBoxOn = false;
                }

                if (node.Node.Nodes != null && node.Node.Nodes.Count > 0)
                {
                    for (int i = 0; i < node.Node.Nodes.Count; ++i)
                    {
                        (node.Node.Nodes[i].Item as UCheckBoxTreeViewItem).IsCheckBoxOn = false;
                    }
                }

            }
            NodesList.EndUpdate();

        }
    }
}

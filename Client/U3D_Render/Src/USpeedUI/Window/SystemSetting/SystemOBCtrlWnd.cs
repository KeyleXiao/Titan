using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets.UOBKeyCodeCtrl;
using ASpeedGame.Data.Scheme;

namespace USpeedUI.SystemSetting
{
    public class SystemOBCtrlWnd : UIBaseSubWnd<SystemOBCtrlWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_CONTROL_OB;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/OBControlView";
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                default:
                    break;
            }
        }
    }

    public class SystemOBCtrlWndView : UIBaseSubWndView
    {
        public UOBKeyCodeCtrlTreeList KeyCodeTree;

        private string[] keycodeGroupLabel;

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            KeyCodeTree.Start();

            keycodeGroupLabel = ULocalizationService.Instance.Get("UIView", "SystemOBControlView", "GroupLabel").Split(';');

            Dictionary<int, List<SchemeOBControl.SchemeOBCtrlKeyCodeNode>> nodeList = SchemeOBControl.Instance.SchemeOBCtrlKeyCodeNodeMap;

            var treeRoot = new ObservableList<TreeNode<UOBKeyCodeCtrlTreeItemData>>();

            foreach (KeyValuePair<int, List<SchemeOBControl.SchemeOBCtrlKeyCodeNode>> kvp in nodeList)
            {
                if (kvp.Value == null || kvp.Value.Count == 0)
                    continue;

                string szGroupName = keycodeGroupLabel.Length >= kvp.Key ? keycodeGroupLabel[kvp.Key - 1] : "";

                var node = new UOBKeyCodeCtrlTreeItemData(kvp.Key.ToString(), null);
                node.nGroupID = kvp.Key;
                node.szGroupName = szGroupName;
                TreeNode<UOBKeyCodeCtrlTreeItemData> tree_node = new TreeNode<UOBKeyCodeCtrlTreeItemData>(node, new ObservableList<TreeNode<UOBKeyCodeCtrlTreeItemData>>(), true);

                foreach (SchemeOBControl.SchemeOBCtrlKeyCodeNode schemeNode in kvp.Value)
                {
                    var leaf = new UOBKeyCodeCtrlTreeItemData(schemeNode.nGroup.ToString(), null);
                    leaf.nGroupID = schemeNode.nGroup;
                    leaf.szGroupName = szGroupName;
                    leaf.szLabel = schemeNode.szLabel;
                    leaf.szKeyCode = schemeNode.szKeyCode;
                    TreeNode<UOBKeyCodeCtrlTreeItemData> tree_leaf = new TreeNode<UOBKeyCodeCtrlTreeItemData>(leaf, new ObservableList<TreeNode<UOBKeyCodeCtrlTreeItemData>>(), false);
                    tree_node.Nodes.Add(tree_leaf);
                }
                treeRoot.Add(tree_node);
            }
            KeyCodeTree.Nodes = treeRoot;

            return true;
        }
    }
}

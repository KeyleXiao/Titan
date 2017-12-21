using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine.EventSystems;

namespace USpeedUI.WarGoodsStore
{
    public class UWarAllGoodsView : UWarGoodsStoreSubViewBase//: UIBaseSubWndView
    {
        //public UWarGoodsLabelDesList GoodsLabelDesList;
        public UWarGoodsStoreTreeView GoodsLabelDesTree;
        //private ObservableList<UWarGoodsLabelDesItem> m_LabelDesListDataSource;
        private ObservableList<UWarGoodsStoreItem> m_filterDefaultTypeList;
        //大件物品列表
        public UWarGoodsStoreAllGoodsTile AllGoodsTile;
        //小贱物品列表
        public UWarGoodsStoreAllGoodsTile AllGoodsTile2;
        private int m_OldSelectedTypeIndex = -1;
        private int m_SelectedTypeClickCount = 0;


        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            //if (GoodsLabelDesList == null)
            //    GoodsLabelDesList = GetTransform.FindChild("GoodsLabelFrame").GetComponent<UWarGoodsLabelDesList>();
            if (AllGoodsTile == null)
                AllGoodsTile = GetTransform.FindChild("AllGoodsFrame").GetComponent<UWarGoodsStoreAllGoodsTile>();
            if (AllGoodsTile2 == null)
                AllGoodsTile2 = GetTransform.FindChild("AllGoodsTile2").GetComponent<UWarGoodsStoreAllGoodsTile>();

            if (GoodsLabelDesTree == null)
            {
                GoodsLabelDesTree = GetTransform.FindChild("GoodsLabelFrame").GetComponent<UWarGoodsStoreTreeView>();
                if (GoodsLabelDesTree != null)
                {
                    GoodsLabelDesTree.Start();
                }
            }
            GoodsLabelDesTree.Multiple = false;
            GoodsLabelDesTree.KeepSelection = false;
            //GoodsLabelDesList.Multiple = false;

            return base.Init(_viewParent);
        }

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_AllGoods;
            }
        }

        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            AllGoodsTile.UpdateVisible();
            AllGoodsTile2.UpdateVisible();
        }

        public override void OnMobaGoodsBuy(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.SetWndBuyGoods(AllGoodsTile.DataSource[_nIndex].GoodsID);
        }
        private void OnMobaGoodsBuy2(int _nIndex,ListViewItem _itemComponent)
        {
            m_MyParent.SetWndBuyGoods(AllGoodsTile2.DataSource[_nIndex].GoodsID);
        }
        public override void OnMobaGoodsSelected(int _nIndex, ListViewItem _itemComponent)
        {
            m_MyParent.CurrentSelectedGoods = AllGoodsTile.DataSource[_nIndex];

            (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }
        private void OnMobaGoodsSelected2(int _nIndex , ListViewItem _itemComponent)
        {
            m_MyParent.CurrentSelectedGoods = AllGoodsTile2.DataSource[_nIndex];

            (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(true);
        }
        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            if (AllGoodsTile.SelectedItem != null && m_MyParent.CurrentSelectedGoods.GoodsID != AllGoodsTile.SelectedItem.GoodsID)
                AllGoodsTile.Deselect(AllGoodsTile.SelectedIndex);

            if (AllGoodsTile2.SelectedItem != null && m_MyParent.CurrentSelectedGoods.GoodsID != AllGoodsTile2.SelectedItem.GoodsID)
                AllGoodsTile2.Deselect(AllGoodsTile2.SelectedIndex);
        }

        private TreeNode<UWarGoodsStoreTreeComponentData> Data2TreeNode(string name,int depth,int id)
        {
            var newItem = new UWarGoodsStoreTreeComponentData(name, null);
            newItem.Depth = depth;
            newItem.GoodsLabelID = id;
            var newNode = new TreeNode<UWarGoodsStoreTreeComponentData>(newItem, new ObservableList<TreeNode<UWarGoodsStoreTreeComponentData>>());            
            return newNode;
        }

        public void BoxGoodsLabelDescriptrion(Hashtable _labelDescriptionTable)
        {
            var treeNodes = new ObservableList<TreeNode<UWarGoodsStoreTreeComponentData>>();

            Dictionary<int, List<int>> LabelAndSubLabelDic = new Dictionary<int, List<int>>();

            //根据配置信息建立物品标签二维列表
            foreach (int labelKey in _labelDescriptionTable.Keys)
            {
                if (labelKey == 0)
                    continue;

                int Dickey = labelKey / 1000;

                //大于0深度为1，小于0深度为2
                if (Dickey > 0)
                {
                    if (!LabelAndSubLabelDic.ContainsKey(Dickey))
                    {
                        LabelAndSubLabelDic[Dickey] = new List<int>();
                    }
                    LabelAndSubLabelDic[Dickey].Add(labelKey);
                }
                else
                {
                    if (!LabelAndSubLabelDic.ContainsKey(labelKey))
                        LabelAndSubLabelDic[labelKey] = new List<int>();
                }
            }
            //排下序
            foreach(int key in LabelAndSubLabelDic.Keys)
            {
                LabelAndSubLabelDic[key].Sort();
            }
            var LabelAndSubLabelDicOrder = from keyValue in LabelAndSubLabelDic orderby keyValue.Key select keyValue;

            //根据当前英雄特征类型默认展开远程/近战栏，所有类型都要默认展开防御栏
            int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            PersonSkinModelItem ModelItem = new PersonSkinModelItem();
            GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref ModelItem);
            int index = (ModelItem.nTraits & 4) == 4 ? 2 : (ModelItem.nTraits & 8) == 8 ? 3 : 1;

            //遍历物品标签二维列表
            foreach (var keyValuePair in LabelAndSubLabelDicOrder)
            {
                
                string Label = Convert.ToString(_labelDescriptionTable[keyValuePair.Key]);
                UBB.toHtml(ref Label);

                //UWarGoodsLabelDesItem item0 = new UWarGoodsLabelDesItem();
                //item0.Depth = 0;
                //item0.GoodsLabel = Label;
                //item0.GoodsLabelID = key;
                //m_LabelDesListDataSource.Add(item0);

                var node0 = Data2TreeNode(Label,0, keyValuePair.Key);

                // 根据英雄类型默认展开
                // 现在说不要显示防御类型
                //if (keyValuePair.Key == 4 || keyValuePair.Key == index)
                if (keyValuePair.Key == index)
                    node0.IsExpanded = true;

                treeNodes.Add(node0);

                foreach (int subKey in keyValuePair.Value)
                {
                    string SubLabel = Convert.ToString(_labelDescriptionTable[subKey]);
                    //UWarGoodsLabelDesItem item1 = new UWarGoodsLabelDesItem();
                    //UBB.toHtml(ref SubLabel);
                    ////Trace.LogWarning("SubLabel:" + SubLabel);
                    //item1.Depth = 1;
                    //item1.GoodsLabel = SubLabel;
                    //item1.GoodsLabelID = subKey;
                    //m_LabelDesListDataSource.Add(item1);

                    var node1=Data2TreeNode(SubLabel,1,subKey);
                    node0.Nodes.Add(node1);
                }
            }

            //GoodsLabelDesList.DataSource = m_LabelDesListDataSource;
            //GoodsLabelDesList.OnSelect.AddListener(OnGoodsLabelSelected);
            if (GoodsLabelDesTree != null)
            {
                GoodsLabelDesTree.Nodes = treeNodes;

                GoodsLabelDesTree.OnSelect.AddListener(OnGoodsLabelSelected);
                GoodsLabelDesTree.OnDeselect.AddListener(OnGoodsLabelDeselected);
                GoodsLabelDesTree.NodeToggle.AddListener(LabelDesNodeToggleEvent);
                GoodsLabelDesTree.NodeCheck.AddListener(OnNodeCheckEvent);
            }
        }
        private void OnNodeCheckEvent(TreeNode<UWarGoodsStoreTreeComponentData> arg0,bool _bCheck)
        {
            //阻止事件冒泡，就不会执行OnGoodsLabelSelected这个方法
            UnityEngine.EventSystems.EventSystem.current.CurrentPointerEventData().CanBubbleEvent = false;

            List<UWarGoodsStoreTreeComponentData> nodeItem = GoodsLabelDesTree.GetAllCheckBoxSelectedNode();
            List<int> nodeLabel = new List<int>();
            int tmpLabelID;

            UIWidgets.ObservableList<UWarGoodsStoreItem> dataSourceList = null;
			
            if(nodeItem.Count == 0)
            {
                dataSourceList = m_filterDefaultTypeList;                
            }
			// 分开处理选择的父节点
            else if(nodeItem.Count == 1 && nodeItem[0].GoodsLabelID < 1000)
            {
                dataSourceList = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(nodeItem[0].GoodsLabelID.ToString());
            }
            else
            {
                for (int i = 0; i < nodeItem.Count; ++i)
                {
                    tmpLabelID = nodeItem[i].GoodsLabelID;
                    {//只算子节点ID
                        if (nodeItem[i].GoodsLabelID > 1000 && nodeLabel.Contains(tmpLabelID) == false)
                            nodeLabel.Add(tmpLabelID);
                    }
                }
                dataSourceList = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(nodeLabel);
            }

            SetAllGoodsTileDataSource(dataSourceList);
        }

        private void LabelDesNodeToggleEvent(TreeNode<UWarGoodsStoreTreeComponentData> arg0)
        {
            //阻止事件冒泡，就不会执行OnGoodsLabelSelected这个方法
            UnityEngine.EventSystems.EventSystem.current.CurrentPointerEventData().CanBubbleEvent = false;
        }

        public void BoxAllGoods(ObservableList<UWarGoodsStoreItem> _allGoodsTable, List<int> _defaultGoodsTypeList)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };
            int Dickey = -1;

            m_filterDefaultTypeList = _allGoodsTable.Where((item) =>
            {
                if (LogicDataCenter.warMobaGoodsDataManager.EnableGoodsFilter)
                {
                    if (item.IsFiltered)
                        return false;
                }
                foreach (string label in item.SchemeGoodsInfo.GoodsLabel.Split(charSeparator))
                {
                    if (label == string.Empty)
                        Dickey = 0;
                    else
                        Dickey = Convert.ToInt32(label) / 1000;
                    foreach (int defaultType in _defaultGoodsTypeList)
                    {
                        if (Dickey == defaultType)
                            return true;
                    }
                }
                return false;
            }).ToObservableList();

            SetAllGoodsTileDataSource(m_filterDefaultTypeList);
        }

        private void OnGoodsLabelSelected(int _nIndex, ListViewItem _itemComponent)
        {
            //if (GoodsLabelDesTree.SelectedIndex != _nIndex)
            {
                GoodsLabelDesTree.ClearAllCheckBoxSelectedNode();
                (_itemComponent as UWarGoodsStoreTreeComponent).CheckBox.isOn = true; //!(_itemComponent as UWarGoodsStoreTreeComponent).CheckBox.isOn;
            }
            return;
			/*
			//下面是旧的处理部分，暂时先不删除
            //因为列表没开多选，所以Selected和DeSelected都会响应OnSelect事件
            //所以根据响应次数，判断执行方式
            m_SelectedTypeClickCount++;

            if(GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID == -1)
            {
                SetAllGoodsTileDataSource(m_filterDefaultTypeList);
                return;
            }

            if (_nIndex != m_OldSelectedTypeIndex)
            {
                m_OldSelectedTypeIndex = _nIndex;
                m_SelectedTypeClickCount = 0;

                SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem
                    (GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID.ToString()));
                return;
            }


            if (GoodsLabelDesTree.SelectedNode.Item.Depth == 0)
            {
                //展开的话显示该类下的所有物品
                if (GoodsLabelDesTree.SelectedNode.IsExpanded)
                {
                    m_OldSelectedTypeIndex = _nIndex;
                    m_SelectedTypeClickCount = 0;
                    SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID.ToString()));
                    return;
                }
                //折叠的话显示默认物品
                else
                {
                    //SetAllGoodsTileDataSource(m_filterDefaultTypeList);
                    //GoodsLabelDesTree.Deselect(m_OldSelectedTypeIndex);
                    m_SelectedTypeClickCount = 0;
                    //m_OldSelectedTypeIndex = -1;

                    m_OldSelectedTypeIndex = _nIndex;
                    SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID.ToString()));
                }
            }
            else
            {
                if (_nIndex != m_OldSelectedTypeIndex)
                {
                    m_OldSelectedTypeIndex = _nIndex;
                    m_SelectedTypeClickCount = 0;
                    SetAllGoodsTileDataSource(LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(GoodsLabelDesTree.SelectedNode.Item.GoodsLabelID.ToString()));
                    return;
                }
                ////选择了同一项的话，意味着要取消选择
                //if (m_SelectedTypeClickCount >= 1)
                //{
                //    SetAllGoodsTileDataSource(m_filterDefaultTypeList);
                //    GoodsLabelDesTree.Deselect(m_OldSelectedTypeIndex);
                //    m_SelectedTypeClickCount = 0;
                //    m_OldSelectedTypeIndex = -1;
                //}
            }
            */
        }

        private void OnGoodsLabelDeselected(int _nIndex,ListViewItem _itemComponent)
        {
            (_itemComponent as UWarGoodsStoreTreeComponent).CheckBox.isOn = false; //!(_itemComponent as UWarGoodsStoreTreeComponent).CheckBox.isOn;
        }

        private void OnAllGoodsOnDeselect(int _nIndex, ListViewItem _itemComponent)
        {
            (_itemComponent as UWarGoodsStoreItemComponent).GoodsSelectedFrame.gameObject.SetActive(false);
        }



        private void SetAllGoodsTileDataSource(ObservableList<UWarGoodsStoreItem> _datSource)
        {
            AllGoodsTile.SelectedIndex = -1;
            AllGoodsTile2.SelectedIndex = -1;

            AllGoodsTile.OnDeselect.RemoveListener(OnAllGoodsOnDeselect);
            AllGoodsTile2.OnDeselect.RemoveListener(OnAllGoodsOnDeselect);

            AllGoodsTile.OnSelect.RemoveListener(OnMobaGoodsSelected);
            AllGoodsTile.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
            AllGoodsTile.onRightClick.RemoveListener(OnMobaGoodsBuy);

            AllGoodsTile2.OnSelect.RemoveListener(OnMobaGoodsSelected2);
            AllGoodsTile2.onDoubleClick.RemoveListener(OnMobaGoodsBuy2);
            AllGoodsTile2.onRightClick.RemoveListener(OnMobaGoodsBuy2);

            if(!U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView) && EntityFactory.MainHeroView.Property != null)
            {
                int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                PersonSkinModelItem ModelItem = new PersonSkinModelItem();
                GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref ModelItem);

                //移除英雄限制的装备
                var list = _datSource.Select(d => d).Where(s => (s.SchemeGoodsInfo.GoodsVocationLimit != 0 && s.SchemeGoodsInfo.GoodsVocationLimit == ModelItem.nTraits)).ToList();
                foreach (var i in list)
                {
                    _datSource.Remove(i);
                }
            }

            //根据物品的贵重度分放到不同的Title

            var dataSource0 = _datSource.Select(d => d).Where(s => (s.SchemeGoodsInfo.GoodsPreciousDegree == 0)).ToObservableList();
            var dataSource1 = _datSource.Select(d => d).Where(s => (s.SchemeGoodsInfo.GoodsPreciousDegree == 1)).ToObservableList();


            AllGoodsTile.DataSource = dataSource0;
            AllGoodsTile2.DataSource = dataSource1;


            AllGoodsTile.OnDeselect.AddListener(OnAllGoodsOnDeselect);
            AllGoodsTile2.OnDeselect.AddListener(OnAllGoodsOnDeselect);


            AllGoodsTile.OnSelect.AddListener(OnMobaGoodsSelected);
            AllGoodsTile.onDoubleClick.AddListener(OnMobaGoodsBuy);
            AllGoodsTile.onRightClick.AddListener(OnMobaGoodsBuy);

            AllGoodsTile2.OnSelect.AddListener(OnMobaGoodsSelected2);
            AllGoodsTile2.onDoubleClick.AddListener(OnMobaGoodsBuy2);
            AllGoodsTile2.onRightClick.AddListener(OnMobaGoodsBuy2);

        }
    }
}

using ASpeedGame.Data.GameMobaSchemes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using USpeedUI.UExtensions;
using UIWidgets;
using DataCenter;

namespace USpeedUI.WarGoodsStore
{
    public class UWarTreeSynthesisGoodsView : UWarGoodsStoreSubViewBase
    {
        private ResNode TreeNodeComponentPrefabRes;
        private List<UWarGoodsStoreTreeNodeComponent> UseTreeNodeList = new List<UWarGoodsStoreTreeNodeComponent>();
        private List<UWarGoodsStoreTreeNodeComponent> UnuseTreeNodeList = new List<UWarGoodsStoreTreeNodeComponent>();

        private List<UWarGoodsStoreTreeNodeData> UnuseTreeNodeDataList = new List<UWarGoodsStoreTreeNodeData>();
        private List<UWarGoodsStoreTreeNodeData> UseTreeNodeDataList = new List<UWarGoodsStoreTreeNodeData>();

        private ResNode TreeNodeHorizonalLinePrefabRes;
        private List<UWarTreeNodeHorizonalLineComponnet> UseTreeNodeHorizonalLineList = new List<UWarTreeNodeHorizonalLineComponnet>();
        private List<UWarTreeNodeHorizonalLineComponnet> UnuseTreeNodeHorizonalLineList = new List<UWarTreeNodeHorizonalLineComponnet>();

        private Transform m_UnuseTreeNodeContainner;
        private Transform m_UnuseTreeNodeHorizonalLineContainner;
        private Rect m_ViewRect;
        public Rect ViewRect
        {
            get
            {
                if (m_ViewRect.size == Vector2.zero)
                    m_ViewRect = (GetTransform as RectTransform).rect;
                return m_ViewRect;
            }
        }
        private int m_NodeDefaultHeight = 0;
        private int m_CurNodeDataIndex = -1;
        /// <summary>
        /// 如果点击的是合成树上的物品，则不更新合成树
        /// </summary>
        private bool m_bSelectGoodsIsTreeNode = false;


        public override bool Init(UWarGoodsStoreView _viewParent)
        {

            TreeNodeComponentPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "DynamicState_Moba/WarGoodsStore/WarGoodsStoreTreeNodeComponent");
            TreeNodeHorizonalLinePrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "DynamicState_Moba/WarGoodsStore/WarTreeNodeHorizonalLine");

            m_UnuseTreeNodeContainner = new GameObject("UnuseTreeNode").transform;
            m_UnuseTreeNodeHorizonalLineContainner = new GameObject("UnuseTreeNodeHorizonalLine").transform;

            m_UnuseTreeNodeContainner.SetParent(this.GetTransform, false);
            m_UnuseTreeNodeHorizonalLineContainner.SetParent(this.GetTransform, false);

            m_NodeDefaultHeight = (int)TreeNodeComponentPrefabRes.GetRectTransformRect().height;

            //预先创建一些
            for (int i = 0; i < 10; i++)
            {
                InstantiateTreeNode();
                InstantiateTreeNodeData();
                InstantiateTreeNodeHorizonalLine();
            }

            return base.Init(_viewParent);
        }

        protected override void OnDestroy()
        {
            AssetBundleManager.DeleteAssets(ref TreeNodeComponentPrefabRes, true);
            AssetBundleManager.DeleteAssets(ref TreeNodeHorizonalLinePrefabRes, true);
            base.OnDestroy();
        }

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_TreeSynthesisGoods;
            }
        }
        //检测函数迭代次数，避免死循环
        private int m_nRunFindNum = 0;
        //检测函数迭代次数，避免死循环
        private int m_nFirstGoodsID = 0;

        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            if (m_bSelectGoodsIsTreeNode)
            {
                m_bSelectGoodsIsTreeNode = false;
                return;
            }
                
            //清除合成树显示部分
            BeginTreeNode();
            BeginTreeNodeData();
            BeginTreeNodeHorizonalLine();

            if (LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_nParam.GoodsID) == null)
                return;

            //计算合成树数据部分
            int nDepth = 0;
            m_CurNodeDataIndex = -1;
            IteratorFindNeedGoods(_nParam.GoodsID, ref nDepth);//, ref UnuseTreeNodeDataList);

            BuildTree(nDepth);
        }

        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            UseTreeNodeList.ForEach((node, index) =>
            {
                node.SetData(UseTreeNodeDataList[index].GoodsItem);
            });
        }
        public override void OnMobaGoodsSelected(ListViewItem _itemComponent)
        {
            int nIndex = UseTreeNodeList.FindIndex((node) => { return node == _itemComponent; });
            if(UseTreeNodeDataList[nIndex].GoodsItem.GoodsID != m_MyParent.CurrentSelectedGoods.GoodsID)
            {
                m_bSelectGoodsIsTreeNode = true;
                m_MyParent.CurrentSelectedGoods = UseTreeNodeDataList[nIndex].GoodsItem;
                GoodsComponentSelectedFrame(_itemComponent, true);

            }

        }

        public override void OnMobaGoodsBuy(ListViewItem _itemComponent)
        {
            int nIndex = UseTreeNodeList.FindIndex((node) => { return node == _itemComponent; });
            m_MyParent.SetWndBuyGoods(UseTreeNodeDataList[nIndex].GoodsItem.GoodsID);
        }

        private void IteratorFindNeedGoods(int _nGoodsID, ref int _nDepth)//, ref List<UWarGoodsStoreTreeNodeData> _list)
        {
            UWarGoodsStoreTreeNodeData node;
            GetUnuseTreeNodeData(out node);//            new UWarGoodsStoreTreeNodeData();

            UWarGoodsStoreItem item = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_nGoodsID);
            node.GoodsItem = item;
            node.GoodsDepth = _nDepth;


            //这里会挺频繁new的，有需要的时候再优化吧
            List<int> NeedSynthesizedGoodsIDList = GameMobaSchemesManager.Instance.GetSchemeGoodsNeedComposeList(_nGoodsID);//item.SchemeGoodsInfo.GoodsCanComposeList.Split(MobaSchemesConfig.separator).ToList();

            m_nRunFindNum++;
            if (m_nRunFindNum == 1)
                m_nFirstGoodsID = _nGoodsID;
            if (m_nRunFindNum > 10)
            {
                Trace.LogError("配置物品的合成所需物品ID重复，导致运行死循环,请核查！！FirstGoodsID=" + m_nFirstGoodsID.ToString() + ",CurGoodsID=" + _nGoodsID.ToString());
                m_nRunFindNum--;
                m_nFirstGoodsID = 0;
                return;
            }


            if (NeedSynthesizedGoodsIDList == null)
            {
                node.GoodsChildCount = 0;
                //_list.Add(node);
            }
            else
            {
                node.GoodsChildCount = NeedSynthesizedGoodsIDList.Count;
                //_list.Add(node);

                _nDepth++;
                for (int i = 0; i < NeedSynthesizedGoodsIDList.Count; i++)
                {
                    int nID = Convert.ToInt32(NeedSynthesizedGoodsIDList[i]);

                    IteratorFindNeedGoods(nID, ref _nDepth);//, ref _list);
                }
            }

            m_nRunFindNum--;

        }

        #region 树节点显示对象、数据对象和横向连接线缓存
        protected void BeginTreeNode()
        {
            if (UseTreeNodeList.Count <= 0)
                return;

            UWarGoodsStoreTreeNodeComponent node;
            while (UseTreeNodeList.Count > 0)
            {
                node = UseTreeNodeList[0];
                UseTreeNodeList.Remove(node);
                UnuseTreeNodeList.Add(node);
                node.SetPurchasedIconVisible(false);
                node.transform.SetParent(m_UnuseTreeNodeContainner, false);
                node.onSelect.RemoveListener(OnMobaGoodsSelected);
                node.onDeselect.RemoveListener(OnAllGoodsOnDeselect);
                node.onRightClick.RemoveListener(OnMobaGoodsBuy);
                node.onDoubleClick.RemoveListener(OnMobaGoodsBuy);
                node.SetVisible(false);
            }
        }
        protected bool GetUnuseTreeNode(out UWarGoodsStoreTreeNodeComponent _nUnuseNode)
        {
            UWarGoodsStoreTreeNodeComponent node;
            //先从缓冲中取
            if (UnuseTreeNodeList.Count > 0)
            {
                node = UnuseTreeNodeList[UnuseTreeNodeList.Count - 1];
                UnuseTreeNodeList.Remove(node);
                UseTreeNodeList.Add(node);
                _nUnuseNode = node;

                return true;
            }
            //没有的话实例化一个出来
            if (TreeNodeComponentPrefabRes == null)
            {
                Trace.LogError("没有节点预制体，不能实例化物品合成树节点");
                _nUnuseNode = null;
                return false;
            }

            InstantiateTreeNode();

            node = UnuseTreeNodeList[UnuseTreeNodeList.Count - 1];
            UnuseTreeNodeList.Remove(node);
            UseTreeNodeList.Add(node);
            _nUnuseNode = node;

            return true;
        }
        protected bool GetUnuseTreeNodeEx(out UWarGoodsStoreTreeNodeComponent _nUnuseNode)
        {
            bool res = GetUnuseTreeNode(out _nUnuseNode);
            _nUnuseNode.transform.SetParent(GetTransform, false);
            _nUnuseNode.onSelect.AddListener(OnMobaGoodsSelected);
            _nUnuseNode.onDeselect.AddListener(OnAllGoodsOnDeselect);
            _nUnuseNode.onRightClick.AddListener(OnMobaGoodsBuy);
            _nUnuseNode.onDoubleClick.AddListener(OnMobaGoodsBuy);
            _nUnuseNode.SetVisible(true);
            return res;
        }

        protected void BeginTreeNodeData()
        {
            if (UseTreeNodeDataList.Count <= 0)
                return;

            UWarGoodsStoreTreeNodeData node;
            while (UseTreeNodeDataList.Count > 0)
            {
                node = UseTreeNodeDataList[0];
                UseTreeNodeDataList.Remove(node);
                UnuseTreeNodeDataList.Add(node);
                node.Clear();
            }

        }
        protected bool GetUnuseTreeNodeData(out UWarGoodsStoreTreeNodeData _nUnuseNodeData)
        {
            UWarGoodsStoreTreeNodeData node;
            //先从缓冲中取
            if (UnuseTreeNodeDataList.Count > 0)
            {
                node = UnuseTreeNodeDataList[UnuseTreeNodeDataList.Count - 1];
                UnuseTreeNodeDataList.Remove(node);
                UseTreeNodeDataList.Add(node);
                _nUnuseNodeData = node;

                return true;
            }

            InstantiateTreeNodeData();

            node = UnuseTreeNodeDataList[UnuseTreeNodeDataList.Count - 1];
            UnuseTreeNodeDataList.Remove(node);
            UseTreeNodeDataList.Add(node);
            _nUnuseNodeData = node;

            return true;

        }

        protected void BeginTreeNodeHorizonalLine()
        {
            if (UseTreeNodeHorizonalLineList.Count <= 0)
                return;

            UWarTreeNodeHorizonalLineComponnet node;
            while (UseTreeNodeHorizonalLineList.Count > 0)
            {
                node = UseTreeNodeHorizonalLineList[0];
                UseTreeNodeHorizonalLineList.Remove(node);
                UnuseTreeNodeHorizonalLineList.Add(node);
                node.transform.SetParent(m_UnuseTreeNodeHorizonalLineContainner, false);

                node.SetVisible(false);
            }
        }
        protected bool GetUnuseTreeNodeHorizonalLine(out UWarTreeNodeHorizonalLineComponnet _nUnuseNode)
        {
            UWarTreeNodeHorizonalLineComponnet node;
            //先从缓冲中取
            if (UnuseTreeNodeHorizonalLineList.Count > 0)
            {
                node = UnuseTreeNodeHorizonalLineList[UnuseTreeNodeHorizonalLineList.Count - 1];
                UnuseTreeNodeHorizonalLineList.Remove(node);
                UseTreeNodeHorizonalLineList.Add(node);
                _nUnuseNode = node;

                return true;
            }
            //没有的话实例化一个出来
            if (TreeNodeHorizonalLinePrefabRes == null)
            {
                Trace.LogError("没有节点横线预制体，不能实例化物品合成树节点横线");
                _nUnuseNode = null;
                return false;
            }

            InstantiateTreeNodeHorizonalLine();

            node = UnuseTreeNodeHorizonalLineList[UnuseTreeNodeHorizonalLineList.Count - 1];
            UnuseTreeNodeHorizonalLineList.Remove(node);
            UseTreeNodeHorizonalLineList.Add(node);
            _nUnuseNode = node;

            return true;
        }

        protected void InstantiateTreeNode()
        {
            UWarGoodsStoreTreeNodeComponent newNode = TreeNodeComponentPrefabRes.InstanceMainRes<UWarGoodsStoreTreeNodeComponent>();
            newNode.transform.SetParent(m_UnuseTreeNodeContainner, false);
            UnuseTreeNodeList.Add(newNode);

            newNode.SetVisible(false);
        }
        protected void InstantiateTreeNodeData()
        {
            UWarGoodsStoreTreeNodeData newNode = new UWarGoodsStoreTreeNodeData();
            UnuseTreeNodeDataList.Add(newNode);
        }
        protected void InstantiateTreeNodeHorizonalLine()
        {
            UWarTreeNodeHorizonalLineComponnet newNode = TreeNodeHorizonalLinePrefabRes.InstanceMainRes<UWarTreeNodeHorizonalLineComponnet>();
            newNode.transform.SetParent(m_UnuseTreeNodeHorizonalLineContainner, false);
            UnuseTreeNodeHorizonalLineList.Add(newNode);

            newNode.SetVisible(false);
        }
        #endregion

        #region 绘制树所有部分
        protected void BuildTree(int _nMaxDepth)
        {

            int nHeight = _nMaxDepth == 0 ? m_NodeDefaultHeight : (int)(GetTransform as RectTransform).rect.height / _nMaxDepth;
            nHeight = nHeight > m_NodeDefaultHeight ? m_NodeDefaultHeight : nHeight;

            SetCTRootNode();

            SetCTNodeData();

            //DrawVerticleLine();

            DrawHorizonalLine();

            DrawPurchasedIcon(LogicDataCenter.warMobaGoodsDataManager.GetPurchasedGoodsIDList());
        }

        protected void SetCTRootNode()
        {
            UWarGoodsStoreTreeNodeData _nNode = GetCurrentNode();
            _nNode.ParentNodeWidth = (int)ViewRect.width;

            GetUnuseTreeNodeEx(out _nNode.GoodsNodeComponent);

            _nNode.GoodsNodeComponent.x = (_nNode.ParentNodeWidth / 2) - (_nNode.GoodsNodeComponent.width / 2);
            _nNode.GoodsNodeComponent.y = 0;
            _nNode.GoodsNodeComponent.SetParentVerticalLineVisible(false);
            _nNode.GoodsNodeComponent.SetChildVerticalLineVisible(_nNode.GoodsChildCount == 0 ? false : true);

            if (_nNode.GoodsChildCount == 0)
            {
                return;
            }

            if (_nNode.ChildsNodeList == null)
                _nNode.ChildsNodeList = new List<UWarGoodsStoreTreeNodeData>();
            else
                _nNode.ChildsNodeList.Clear();

            for (int i = 0; i < _nNode.GoodsChildCount; i++)
            {
                UWarGoodsStoreTreeNodeData nextNode = GetCurrentNode();
                nextNode.ParentNode = _nNode;
                GetUnuseTreeNodeEx(out nextNode.GoodsNodeComponent);
                _nNode.ChildsNodeList.Add(nextNode);

                SetCTChildNode(nextNode, i);
            }

        }

        protected void SetCTChildNode(UWarGoodsStoreTreeNodeData _nNode, int _index)
        {

            int minusCount = 0;
            //同级所有节点所占的宽度
            int ChildsWidth = (int)_nNode.GoodsNodeComponent.width * _nNode.ParentNode.GoodsChildCount;
            int ParentWidth = _nNode.ParentNode.ParentNodeWidth;

            //计算当前子级所有节点所占的宽度是否超出父级宽度
            while (ParentWidth < ChildsWidth)
            {
                minusCount++;
                ChildsWidth = (int)_nNode.GoodsNodeComponent.width * (_nNode.ParentNode.GoodsChildCount - minusCount);
            }

            _nNode.ParentNodeWidth = (_nNode.ParentNode.ParentNodeWidth) / (_nNode.ParentNode.GoodsChildCount - minusCount);

            //从父节点中点到左边距离开始算起
            int px = (int)(_nNode.ParentNode.GoodsNodeComponent.x + _nNode.ParentNode.GoodsNodeComponent.width / 2) - (_nNode.ParentNode.ParentNodeWidth / 2);
            float XOffset = 0.5f;

            //同级节点为偶数则偏移半个节点位置，否则不偏移
            if (minusCount > 0)
            {
                XOffset = _nNode.ParentNode.GoodsChildCount % 2 == 0 ? 0.5f : 0;
            }

            _nNode.GoodsNodeComponent.x = px + (_nNode.ParentNodeWidth * (_index + XOffset)) - (_nNode.GoodsNodeComponent.width / 2);
            _nNode.GoodsNodeComponent.y = _nNode.ParentNode.GoodsNodeComponent.y - _nNode.ParentNode.GoodsNodeComponent.height;
            _nNode.GoodsNodeComponent.SetParentVerticalLineVisible(true);
            _nNode.GoodsNodeComponent.SetChildVerticalLineVisible(_nNode.GoodsChildCount == 0 ? false : true);

            if (_nNode.GoodsChildCount == 0)
            {
                return;
            }


            if (_nNode.ChildsNodeList == null)
                _nNode.ChildsNodeList = new List<UWarGoodsStoreTreeNodeData>();
            else
                _nNode.ChildsNodeList.Clear();



            for (int i = 0; i < _nNode.GoodsChildCount; i++)
            {
                UWarGoodsStoreTreeNodeData nextNode = GetCurrentNode();
                nextNode.ParentNode = _nNode;
                _nNode.ChildsNodeList.Add(nextNode);
                GetUnuseTreeNodeEx(out nextNode.GoodsNodeComponent);

                SetCTChildNode(nextNode, i);
            }
        }

        protected void SetCTNodeData()
        {
            UseTreeNodeList.ForEach((node, index) =>
            {
                node.SetData(UseTreeNodeDataList[index].GoodsItem);
            });
        }

        protected void DrawVerticleLine()
        {
            UseTreeNodeDataList.ForEach((NodeData) =>
            {

                bool bParentVertileLine = NodeData.ParentNode == null ? false : true;
                bool bChildVertileLine = NodeData.GoodsChildCount <= 0 ? false : true;
                NodeData.GoodsNodeComponent.SetVerticalLineVisible(bChildVertileLine, bParentVertileLine);
            });
        }

        protected void DrawHorizonalLine()
        {
            UWarGoodsStoreTreeNodeData BeginNode;
            UWarGoodsStoreTreeNodeData EndNode;
            UWarTreeNodeHorizonalLineComponnet line;

            UseTreeNodeDataList.ForEach((NodeData) =>
            {
                BeginNode = null;
                EndNode = null;
                line = null;
                if (NodeData.GoodsChildCount > 1)
                {
                    BeginNode = NodeData.ChildsNodeList[0];
                    EndNode = NodeData.ChildsNodeList[NodeData.GoodsChildCount - 1];
                    GetUnuseTreeNodeHorizonalLine(out line);


                    Vector2 BeginNodePos = (BeginNode.GoodsNodeComponent.ParentVerticalLine.transform as RectTransform).switchToRectTransform((GetTransform as RectTransform), UISystem.Instance.GetCamera());
                    Vector2 EndNodePos = (EndNode.GoodsNodeComponent.ParentVerticalLine.transform as RectTransform).switchToRectTransform((GetTransform as RectTransform), UISystem.Instance.GetCamera());

                    line.width = EndNodePos.x - BeginNodePos.x;//EndNode.GoodsNodeComponent.ChildVerticalLineRight - line.x;

                    line.x = 0;// BeginNode.GoodsNodeComponent.ChildVerticalLineLeft;
                    line.y = 0;//BeginNode.GoodsNodeComponent.ChildVerticalLineTop;

                    line.transform.SetParent(BeginNode.GoodsNodeComponent.ParentVerticalLine.transform, false);
                    line.SetVisible(true);

                }

            });
        }

        public void DrawPurchasedIcon(List<int> _nPurchasedList)
        {
            //List<int> tmpPurchasedList = m_MyParent.GetCurrentPurchasedGoodsID();
            if (_nPurchasedList == null)
                return;
            int tmpIndex = -1;
            
            for(int i=0;i< UseTreeNodeDataList.Count;)
            {
                tmpIndex = _nPurchasedList.IndexOf(UseTreeNodeDataList[i].GoodsItem.GoodsID);
                UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(false);
                if (tmpIndex != -1)
                {
                    UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(true);
                    _nPurchasedList.RemoveAt(tmpIndex);

                    if (i == 0)
                    {
                        UseTreeNodeDataList[i].GoodsNodeComponent.SetPurchasedIconVisible(false);
                    }
                    else
                    {
                        if (UseTreeNodeDataList[i].GoodsChildCount > 0)
                        {
                            for (int j = 1; j <= UseTreeNodeDataList[i].GoodsChildCount; j++)
                                UseTreeNodeDataList[i + j].GoodsNodeComponent.SetPurchasedIconVisible(true);
                            i += UseTreeNodeDataList[i].GoodsChildCount;
                        }
                    }
                }
                i++;
            }
        }
        #endregion

        protected UWarGoodsStoreTreeNodeData GetCurrentNode()
        {
            m_CurNodeDataIndex++;
            if (m_CurNodeDataIndex >= UseTreeNodeDataList.Count)
            {
                Debug.LogWarning("超出范围了");
                return null;
            }
            UWarGoodsStoreTreeNodeData _nNode = UseTreeNodeDataList[m_CurNodeDataIndex];

            return _nNode;
        }

        private void OnAllGoodsOnDeselect(ListViewItem _itemComponent)
        {
            GoodsComponentSelectedFrame(_itemComponent, false);
        }

        protected void GoodsComponentSelectedFrame(ListViewItem _itemComponent,bool _bVisible)
        {
            if (_itemComponent == null)
                return;

            var tmpSelectedComponent = (_itemComponent as UWarGoodsStoreItemComponent);
            if (tmpSelectedComponent && tmpSelectedComponent.GoodsSelectedFrame != null)
            {
                tmpSelectedComponent.GoodsSelectedFrame.gameObject.SetActive(_bVisible);
                //m_SelectedPurchasedGoodsComponent = null;
            }
        }
    }

    public class UWarGoodsStoreTreeNodeData
    {
        public int GoodsDepth;
        public int GoodsChildCount;
        //当前节点的父节点
        public UWarGoodsStoreTreeNodeData ParentNode;
        //当前节点父节点所占区域宽度
        public int ParentNodeWidth;
        public UWarGoodsStoreItem GoodsItem;
        public UWarGoodsStoreTreeNodeComponent GoodsNodeComponent;
        //当前节点的子节点链表
        public List<UWarGoodsStoreTreeNodeData> ChildsNodeList;

        public void Clear()
        {
            GoodsDepth = 0;
            GoodsChildCount = 0;
            ParentNode = null;
            GoodsItem = null;
            GoodsNodeComponent = null;
            if (ChildsNodeList != null)
                ChildsNodeList.Clear();
            ParentNodeWidth = 0;
        }
    }
}

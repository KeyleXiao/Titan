using ASpeedGame.Data.GameMobaSchemes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UIWidgets;
using DataCenter;
using UnityEngine.UI;

namespace USpeedUI.WarGoodsStore
{
    public class UWarRecommendGoodsView : UWarGoodsStoreSubViewBase//: UIBaseSubWndView
    {
        ///// <summary> 出门装物品挂接对象 </summary>
        //public Transform EerlyGoodsFrameTrans;
        ///// <summary> 中期物品挂接对象 </summary>
        //public Transform MiddleGoodsFrameTrans;
        ///// <summary> 顺风物品挂接对象 </summary>
        //public Transform DownWindGoodsFrameTrans;
        ///// <summary> 逆风物品挂接对象 </summary>
        //public Transform UpWindGoodsFrameTrans;
        ///// <summary> 消耗物品挂接对象 </summary>
        //public Transform ConsumeGoodsFrameTrans;
        /// <summary>
        /// 推荐物品标题文本
        /// </summary>
        public Text[] RecommendTitleTxt;
        public Transform[] GoodsFrameTrans;

        private UButtonGroup m_RecommendGoodsGroup;
        private List<UWarGoodsStoreItem> m_ReCommendGoodsItemList = new List<UWarGoodsStoreItem>();
        /// <summary>
        /// 当前已用到的推荐物品类型序号
        /// </summary>
        private int m_nUsedRecommendType = -1;
        

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            m_RecommendGoodsGroup = gameObject.GetComponent<UButtonGroup>();
            if(m_RecommendGoodsGroup == null)
                m_RecommendGoodsGroup = gameObject.AddComponent<UButtonGroup>();

            m_RecommendGoodsGroup.SkipInactive = false;
            return base.Init(_viewParent);
        }

        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_RecommendGoods;
            }            
        }
        public override void OnMobaGoodsDataSourceChange(UWarGoodsStoreDataSourceChangeParam _nParam)
        {
            UpdateVisible();
        }

        public override void OnMobaGoodsBuy(UButtonGroupItem _nItem, bool _bSelected)
        {
            int nIndex = m_RecommendGoodsGroup.GetItemIndex(_nItem);
            if (nIndex != -1)
                m_MyParent.SetWndBuyGoods(m_ReCommendGoodsItemList[nIndex].GoodsID);
        }
        public override void OnMobaGoodsSelected(UButtonGroupItem _nItem, bool _bSelected)
        {
            if (_bSelected)
                m_MyParent.CurrentSelectedGoods = m_ReCommendGoodsItemList[m_RecommendGoodsGroup.SelectedIndex];

            (_nItem as UWarGoodsStoreRecommendComponent).GoodsSelectedFrame.gameObject.SetActive(_bSelected);
        }
        public override void OnMobaGoodsClick(UWarGoodsStoreItem _nParam)
        {
            if (m_RecommendGoodsGroup.SelectedIndex != -1 && m_MyParent.CurrentSelectedGoods.GoodsID != m_ReCommendGoodsItemList[m_RecommendGoodsGroup.SelectedIndex].GoodsID)
                m_RecommendGoodsGroup.SetAllItemDeSelected();
        }


        public void BoxRecommendedGoodsByGoodsType(System.Collections.Generic.List<SSchemeMobaGoods> _typeList,List<int> _dispositionTypeList, EMobaRecommendGoods _eRecommendType)
        {
            m_nUsedRecommendType++;
			//取显示对象挂载容器
            Transform containner = GetCurrentContainer();
            ResNode go = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/" + UPath.UDynamicMobaState + "/WarGoodsStore/WarGoodsStoreRecommendComponent");
            UWarGoodsStoreRecommendComponent component;

            //取该类型标题本地化文本
            if(GetCurrentContainerTitleText()!= null)
            {
                ULocalizationService.Instance.Get(GetCurrentContainerTitleText(), "UIView", "WarGoodsStore", _eRecommendType.ToString());
            }
            //移除英雄限制的装备
            int nHeroId = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            PersonSkinModelItem ModelItem = new PersonSkinModelItem();
            GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref ModelItem);

            var list = _typeList.Select(d => d).Where(s => (s.GoodsVocationLimit != 0 && s.GoodsVocationLimit == ModelItem.nTraits)).ToList();
            foreach (var i in list)
            {
                _typeList.Remove(i);
            }

            int count = _typeList.Count >= 6 ? 6 : _typeList.Count;

            for (int i = 0; i < count; ++i) //each (var schemeGoods in _typeList)
            {                
                SSchemeMobaGoods schemeGoods = _typeList[i];
                UWarGoodsStoreItem newItem = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(schemeGoods);//m_MyParent.GetWarGoodsStoreItem(schemeGoods);
                if(newItem != null)
                {
                    m_ReCommendGoodsItemList.Add(newItem);

                    component = go.InstanceMainRes<UWarGoodsStoreRecommendComponent>();
                    component.gameObject.name = newItem.SchemeGoodsInfo.GoodsName;
                    component.SetData(newItem);

                    component.Group = m_RecommendGoodsGroup;
                    component.transform.SetParent(containner, false);

                    component.onSelectedChangedEx.AddListener(OnMobaGoodsSelected);
                    component.onDoubleClick.AddListener(OnMobaGoodsBuy);
                    component.onRightClick.AddListener(OnMobaGoodsBuy);
                }
                else
                {
                    newItem = new UWarGoodsStoreItem(schemeGoods);

                    if (LogicDataCenter.warMobaGoodsDataManager.EnableGoodsFilter && newItem.IsFiltered)
                        continue;

                    newItem.GoodsComponentVisible = false;
                    m_ReCommendGoodsItemList.Add(newItem);
                    component = go.InstanceMainRes<UWarGoodsStoreRecommendComponent>();
                    component.Group = m_RecommendGoodsGroup;
                    component.transform.SetParent(containner, false);

                    component.onSelectedChangedEx.AddListener(OnMobaGoodsSelected);
                    component.onDoubleClick.AddListener(OnMobaGoodsBuy);
                    component.onRightClick.AddListener(OnMobaGoodsBuy);

                    component.SetData(newItem);
                }
                //存下更改物品配置时需要替换的推荐类型
                if (_dispositionTypeList != null && _dispositionTypeList.Contains((int)_eRecommendType))
                    LogicDataCenter.warMobaGoodsDataManager.SetHeroDefaultGoods(i, schemeGoods.GoodsID, m_nUsedRecommendType);
            }

        }

        private Transform GetCurrentContainer()
        {
            if(GoodsFrameTrans == null || GoodsFrameTrans.Length <=0 )
            {
                Trace.LogWarning("战场推荐物品挂点没数据.");
                return null;
            }
            if(m_nUsedRecommendType < 0 || m_nUsedRecommendType >= GoodsFrameTrans.Length)
            {
                Trace.LogWarning("当前已使用的推荐物品类型序号异常。");
                return null;
            }
            return GoodsFrameTrans[m_nUsedRecommendType];
        }

        private Text GetCurrentContainerTitleText()
        {
            if (RecommendTitleTxt == null || RecommendTitleTxt.Length <= 0)
            {
                Trace.LogWarning("战场推荐物品挂点没数据.");
                return null;
            }
            if (m_nUsedRecommendType < 0 || m_nUsedRecommendType >= GoodsFrameTrans.Length)
            {
                Trace.LogWarning("当前已使用的推荐物品类型序号异常。");
                return null;
            }
            return RecommendTitleTxt[m_nUsedRecommendType];

        }

        public void SetPurchasedGoods(List<GameLogic.cmd_war_persongoodsinfo> _goodsList)
        {
            int length = m_ReCommendGoodsItemList.Count;
            for (int i = 0; i < length; ++i) 
            {
                m_ReCommendGoodsItemList[i].GoodsComponentVisible = true;
            }

            length = _goodsList.Count;
            List<int> tmpNeedComposeList = null;
            for (int i = 0; i < length; ++i) 
            {
                if (_goodsList[i].nGoodID == 0)
                    continue;

                for (int j = 0; j < m_ReCommendGoodsItemList.Count; ++j)
                {
					//策划强烈要求
                    tmpNeedComposeList = GoodsSchemesDataManager.Instance.GetSchemeGoodsNeedComposeList(_goodsList[i].nGoodID);
                    if (m_ReCommendGoodsItemList[j].GoodsID == _goodsList[i].nGoodID || m_ReCommendGoodsItemList[j].GoodsID == 0 ||
                        (tmpNeedComposeList != null && tmpNeedComposeList.Contains(m_ReCommendGoodsItemList[j].GoodsID))  
                        )
                    {
                        m_ReCommendGoodsItemList[j].GoodsComponentVisible = false;
                        //break;
                    }
                }
            }

            for (int i = 0; i < m_ReCommendGoodsItemList.Count; ++i)
            {
                if(m_ReCommendGoodsItemList[i].GoodsID == 0)
                    m_ReCommendGoodsItemList[i].GoodsComponentVisible = false;
            }
        }

        private void ForeachRecommendGoodesItem(UWarGoodsStoreItem _nItem)
        {

        }

        public void SetRecommendedGoods(int[] _newGoodsIDs,EMobaRecommendGoods _eRecommendType)
        {            
            for (int i = 0; i < _newGoodsIDs.Length; ++i)
            {
                if (_newGoodsIDs[i] == 0)
                    continue;
                //判断下是否已购买，已购买则不显示
                foreach(var PurchasedInfo in LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo())
                {
                    if (PurchasedInfo.nGoodID == _newGoodsIDs[i])
                        _newGoodsIDs[i] = 0;
                }               
            }

            int recommendIndex = -1;
            for (int i = 0; i < _newGoodsIDs.Length; ++i)
            {
                if (_newGoodsIDs[i] == 0)
                    continue;

                UWarGoodsStoreItem newItem = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_newGoodsIDs[i]);//m_MyParent.GetWarGoodsStoreItem(schemeGoods);

                recommendIndex = _eRecommendType == EMobaRecommendGoods.EMRG_MiddleStage ? i + 6 : i;
                m_ReCommendGoodsItemList[recommendIndex] = newItem;
            }
        }

        public void SetRecommendedGoods(int _newGoodsID,int _nRecommendIndex)
        {
            UWarGoodsStoreItem newItem = null;
            List<int> tmpNeedComposeList = null;

            if (_newGoodsID != 0)
            {
                newItem = LogicDataCenter.warMobaGoodsDataManager.GetWarGoodsStoreItem(_newGoodsID);//m_MyParent.GetWarGoodsStoreItem(schemeGoods);
                if(newItem == null)
                {
                    Trace.LogWarning("没找到ID："+_newGoodsID+"的物品，策划检查下是不是该物品不能用于当前战场。");
                    return;
                }
                newItem.GoodsComponentVisible = true;
                if(LogicDataCenter.warMobaGoodsDataManager.IsMainHeroPDBIDValid && LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo() != null)
                {
                    foreach (var PurchasedInfo in LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo())
                    {
						//策划强烈要求，如果已购买大件的话，推荐页不显示小件物品。
                        tmpNeedComposeList = GoodsSchemesDataManager.Instance.GetSchemeGoodsNeedComposeList(PurchasedInfo.nGoodID);
                        if (PurchasedInfo.nGoodID == newItem.GoodsID || newItem.GoodsID == 0 ||
                            (tmpNeedComposeList!= null && tmpNeedComposeList.Contains(newItem.GoodsID)))
                            newItem.GoodsComponentVisible = false;
                    }
                }
            }
            else
            {
                newItem = new UWarGoodsStoreItem(_newGoodsID);
                newItem.GoodsComponentVisible = false;
            }
            if (_nRecommendIndex >= 0 && _nRecommendIndex < m_ReCommendGoodsItemList.Count)
                m_ReCommendGoodsItemList[_nRecommendIndex] = newItem;
            else
                Trace.LogError("设置推荐物品出错，物品ID：" + _newGoodsID + ",序号：" + _nRecommendIndex);

        }

        public void UpdateVisible()
        {
            int length = m_RecommendGoodsGroup.Length();
            for (int i = 0; i < length; i++)
            {
                UWarGoodsStoreRecommendComponent component = m_RecommendGoodsGroup.GetItemAt(i) as UWarGoodsStoreRecommendComponent;
                if (component != null)
                {
                    component.SetData(m_ReCommendGoodsItemList[i]);
                }

            }
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
    [Serializable]
    public class ListViewItemSelect : UnityEvent<UWarGoodsStoreDispositionItemComponent>
    {

    }
    [Serializable]
    public class ListViewBaseEvent : UnityEvent<int, UWarGoodsStoreDispositionItemComponent>
    {
    }

    public class UWarGoodsStoreDispositionItem
    {
        public String DispositionName;
        public String DispositionTitle;
        public int DispositionID;
        public int[] DispositionGoods;

        public void From(GameLogic.cmd_war_equipscheme _scheme)
        {
            DispositionName = _scheme.szSchemeName;
            DispositionTitle = _scheme.szTitle;
            DispositionID = _scheme.bySchemeID;
            if(_scheme.nEquipIDList != null)
                DispositionGoods = _scheme.nEquipIDList;
        }
    }
    public class UWarGoodsStoreDispositionItemComponent : UCustomUIBehaviour, IPointerClickHandler
    {
        public int Index;
        public Text DispositionNameTxt;
        public ListViewItemSelect onClick = new ListViewItemSelect();

        public void OnPointerClick(PointerEventData eventData)
        {
            onClick.Invoke(this);
        }

        public void SetData(UWarGoodsStoreDispositionItem _item)
        {
            DispositionNameTxt.text = _item.DispositionName;
        }
    }
    public class UWarGoodsStoreDispositionList : UCustomUIBehaviour
    {
        public Image CurrentArrow = null;
        public UWarGoodsStoreDispositionItemComponent DefaultComponent = null;
        protected List<UWarGoodsStoreDispositionItemComponent> m_nComponentList = null;
        protected List<UWarGoodsStoreDispositionItem> m_nDataSource = null;
        public List<UWarGoodsStoreDispositionItem> DataSource
        {
            get { return m_nDataSource; }
            set
            {
                SetNewData(value);
            }
        }

        public ListViewBaseEvent OnSelect = new ListViewBaseEvent();

        protected override void Start()
        {
            base.Start();
            DefaultComponent.gameObject.SetActive(false);

        }

        protected void SetNewData(List<UWarGoodsStoreDispositionItem> _newData)
        {
            m_nDataSource = _newData;

            if(m_nComponentList != null && m_nComponentList.Count > 0)
            {
                for (int i = m_nComponentList.Count - 1; i >= 0; i--) 
                {
                    UWarGoodsStoreDispositionItemComponent component = m_nComponentList[i];
                    component.onClick.RemoveAllListeners();
                    ResNode.DestroyRes(ref component);
                    m_nComponentList.RemoveAt(i);
                }
                m_nComponentList.Clear();
            }
            else if(m_nComponentList == null)
            {
                m_nComponentList = new List<UWarGoodsStoreDispositionItemComponent>();
            }


            //实例化显示对象并初始化数据
            DefaultComponent.gameObject.SetActive(true);

            UWarGoodsStoreDispositionItemComponent newComponent = null;
            for (int i = 0; i < m_nDataSource.Count; ++i) 
            {
                newComponent = ResNode.InstantiateRes<UWarGoodsStoreDispositionItemComponent>(DefaultComponent.gameObject,true);
                newComponent.GetTransform.SetParent(DefaultComponent.GetTransform.parent,false);
                newComponent.Index = i;
                newComponent.SetData(m_nDataSource[i]);
                newComponent.onClick.AddListener(OnComponentClick);
                m_nComponentList.Add(newComponent);
            }

            DefaultComponent.gameObject.SetActive(false);            
        }

        protected void OnComponentClick(UWarGoodsStoreDispositionItemComponent _component)
        {
            Vector3 newVec3 = CurrentArrow.rectTransform.anchoredPosition;
            newVec3.y = _component.GetRectTransform.anchoredPosition.y - _component.GetRectTransform.sizeDelta.y / 2;
            CurrentArrow.rectTransform.anchoredPosition = newVec3;

            OnSelect.Invoke(_component.Index, _component);
        }

        public void SetVisible(bool _bNewVisible)
        {
            
        }
    }
}

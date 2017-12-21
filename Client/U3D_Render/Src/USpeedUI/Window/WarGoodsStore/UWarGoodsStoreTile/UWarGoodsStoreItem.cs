using ASpeedGame.Data.GameMobaSchemes;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.WarGoodsStore
{
    public interface IWarGoodsStoreItem
    {
        int GoodsCost { get; set; }
        int GoodsIconID { get; }
        int GoodsID { get; }
        SSchemeMobaGoods SchemeGoods { set; }
    }

    /// <summary>
    /// OnChange.
    /// </summary>
    public delegate void OnPropertyChange(object _sender, PropertyChangeEventArgs _args);
    public class PropertyChangeEventArgs : System.EventArgs
    {
    }
    /// <summary>
    /// IObservable.
    /// </summary>
    public interface IPropertyChanged
    {

        /// <summary>
        /// Occurs when data changed.
        /// </summary>
        event OnPropertyChange OnPropertyChange;
    }


    /// <summary>
    /// 商店物品显示数据
    /// </summary>
    [System.Serializable]
    public class UWarGoodsStoreItem : IPropertyChanged //IObservable, INotifyPropertyChanged
    {
        public UWarGoodsStoreItem() { }
        public UWarGoodsStoreItem(SSchemeMobaGoods _schemes)
        {
            SchemeGoodsInfo = _schemes;
            m_nGoodsCost = SchemeGoodsInfo.GoodsCost;
            m_nGoodsID = SchemeGoodsInfo.GoodsID;
            m_nFlag = SchemeGoodsInfo.GoodsFlag;
        }

        public UWarGoodsStoreItem(int _nGoodsID)
        {
            m_nGoodsID = _nGoodsID;
        }

        private SSchemeMobaGoods m_SchemeGoodsInfo;
        /// <summary>物品配置属性</summary>
        public SSchemeMobaGoods SchemeGoodsInfo
        {
            get
            {
                return m_SchemeGoodsInfo;
            }
            set
            {
                m_SchemeGoodsInfo = value;
            }
        }

        private int m_nGoodsCost;
        /// <summary>物品显示的价格 </summary>
        public int GoodsCost
        {
            get
            {
                return m_nGoodsCost;
            }
            set
            {
                if(m_nGoodsCost != value)
                {
                    //if(UWarGoodsStoreView.TraceGoodsCost)
                    //{
                    //    Trace.Log("Goods:" + GoodsID + "(" + SchemeGoodsInfo.GoodsName + "):Old Cost:" + m_nGoodsCost + ",new Cost:" + value+",scheme Cost:"+SchemeGoodsInfo.GoodsCost);
                    //    //Trace.Log(new System.Diagnostics.StackTrace().ToString());
                    //}
                    m_nGoodsCost = value;
                    Changed("GoodsCost");
                }
            }
        }

        public int GoodsSoldOutCost
        {
            get
            {
                if (SchemeGoodsInfo == null)
                    return 0;
                else
                    return SchemeGoodsInfo.GoodsSoldOutCost;
            }
        }

        private int m_nGoodsID;
        /// <summary>物品ID</summary>
        public int GoodsID
        {
            get
            {
                return m_nGoodsID;
            }
            set
            {
                m_nGoodsID = value;
            }
        }

        private bool m_bGoodsCanBuy;
        /// <summary>物品是否可以购买</summary>
        public bool GoodsCanBuy
        {
            get
            {
                return m_bGoodsCanBuy;
            }
            set
            {
                if(m_bGoodsCanBuy != value)
                {
                    m_bGoodsCanBuy = value;
                    Changed("GoodsCanBuy");
                }
            }

        }

        private int m_nGoodsCanUseTimes;
        /// <summary>物品可使用次数</summary>
        public int GoodsCanUseTimes
        {
            get
            {
                return m_nGoodsCanUseTimes;
            }
            set
            {
                m_nGoodsCanUseTimes = value;
            }
        }

        /// <summary>物品渲染对象是否可显示</summary>
        private bool m_GoodsComponentVisible = true;
        public bool GoodsComponentVisible
        { get { return m_GoodsComponentVisible; } set { m_GoodsComponentVisible = value; Changed("GoodsComponentVisible"); } }

        /// <summary>是否是已购买物品</summary>
        private bool m_bIsPurchased = false;
        public bool IsPurchased
        {
            get { return m_bIsPurchased; }
            set
            {
                m_bIsPurchased = value;
            }
        }

        /// <summary>物品标识</summary>
        private int m_nFlag;
        /// <summary>是否过滤物品</summary>
        public bool IsFiltered
        {
            get
            {
                int nNpcPurchaseFlag = 0;
                nNpcPurchaseFlag = (int)War.MOBA_GOODS_FLAG.GOODS_FLAG_NPC_PURCHASE;
                if ((m_nFlag & nNpcPurchaseFlag) == nNpcPurchaseFlag)
                {
                    return true;
                }
                return false;
            }
        }


        public event OnPropertyChange OnPropertyChange;

        private void Changed(string propertyName = "")
        {
            //if (OnChange != null)
            //{
            //    OnChange();
            //}
            //if (PropertyChanged != null)
            //{
            //    PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            //}
            if(OnPropertyChange != null)
            {
                OnPropertyChange(this,new PropertyChangeEventArgs());
            }
        }

        /// <summary>
        /// 重置下物品需要重置的值
        /// </summary>
        public void ResetItemValue()
        {
            GoodsCost = SchemeGoodsInfo.GoodsCost;
            GoodsCanUseTimes = 0;
            IsPurchased = false;
        }

        public override bool Equals(object obj)
        {
            if (obj is UWarGoodsStoreItem)
                return GoodsID == (obj as UWarGoodsStoreItem).GoodsID;
            else
                return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }


    }
}

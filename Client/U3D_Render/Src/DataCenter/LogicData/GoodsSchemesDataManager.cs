using ASpeedGame.Data.GameMobaSchemes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace DataCenter
{
    /// <summary>
    /// 游戏所有物品总表
    /// </summary>
    public class GoodsSchemesDataManager : Singleton<GoodsSchemesDataManager> , IModelData 
    {
        private Dictionary<int, SSchemeMobaGoods> m_dicGoodsSchemeTable;

        public void Load(string _strPath)
        {
            m_dicGoodsSchemeTable = new Dictionary<int, SSchemeMobaGoods>();

            ScpReader MobaGoodsReader = new ScpReader(_strPath, true, 2);//new ScpReader(SchemeAllText, "MobaGoods", 2);
            for (int i = 0; i < MobaGoodsReader.GetRecordCount(); i++)
            {
                SSchemeMobaGoods goods = new SSchemeMobaGoods();
                goods.GoodsID = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_ID, 0);
                goods.GoodsName = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_Name, string.Empty);
                goods.GoodsIconID = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_IconID, 0);
                goods.GoodsCost = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_Cost, 0);
                goods.GoodsSoldOutCost = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_SoldOutCost, 0);
                goods.GoodsDes = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_Description, string.Empty);
                goods.GoodsCanComposeList = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_CanComposeList, string.Empty);
                goods.GoodsNeedComposeList = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_NeedComposeList, string.Empty);
                goods.GoodsLabel = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_Label, string.Empty);
                goods.GoodsWarLimit = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_WarLimit, string.Empty);
                goods.GoodsSlotDes = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_SlotDescription, string.Empty);
                goods.GoodsVocationLimit = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_VocationTypeLimit, 0);
                goods.GoodsPreciousDegree = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_PreciousDegree, 0);
                goods.GoodsFlag = MobaGoodsReader.GetInt(i, (int)EMobaGoods.EMG_Flag, 0);
                goods.GoodsLocationInstructions = MobaGoodsReader.GetString(i, (int)EMobaGoods.EMG_LocationInstructions, string.Empty);

                m_dicGoodsSchemeTable.Add(goods.GoodsID, goods);
            }
            // 需要加一列空值（服务端传过来时会传过来一个0  在这个容器里查找 如果不加会报错）
            if (m_dicGoodsSchemeTable.ContainsKey(0) == false)
            {
                m_dicGoodsSchemeTable.Add(0, new SSchemeMobaGoods());
            }

            MobaGoodsReader.Dispose();
        }
        public void UnLoad()
        {
            if (m_dicGoodsSchemeTable != null)
            {
                m_dicGoodsSchemeTable.Clear();
                m_dicGoodsSchemeTable = null;
            }
        }

        private bool IsHasGoods(int _goodsID)
        {
            bool res = false;
            if (m_dicGoodsSchemeTable.ContainsKey(_goodsID))
            {
                res = true;
            }
            return res;
        }

        /// <summary>
        /// 拷贝一份新的，如果 “会改变里面值” 的话尽量用这方法
        /// </summary>
        /// <param name="_goodsID"></param>
        /// <param name="_goods"></param>
        public bool GetMobaGoods_Ref(int _goodsID, ref SSchemeMobaGoods _goods)
        {
            if (!IsHasGoods(_goodsID))
            {
                //Trace.LogWarning("没找到ID为:" + _goodsID + "的物品");
                _goods = null;
                return false;
            }
            m_dicGoodsSchemeTable[_goodsID].CopyTo(ref _goods);
            return true;
        }
        /// <summary>
        /// 取引用，如果 “不会改变里面的值” 的话尽量用这个方法
        /// </summary>
        /// <param name="_goodsID"></param>
        /// <param name="_goods"></param>
        public bool GetMobaGoods_Out(int _goodsID, out SSchemeMobaGoods _goods)
        {
            if (!IsHasGoods(_goodsID))
            {
                //Trace.LogWarning("没找到ID为:" + _goodsID + "的物品");
                _goods = null;
                return false;
            }
            _goods = m_dicGoodsSchemeTable[_goodsID];
            return true;
        }
        /// <summary>
        /// 根据某个战场类型ID取相应的物品
        /// </summary>
        /// <param name="_nWarID"></param>
        /// <param name="_dicNewTable"></param>
        public void GetGoodsSschemdTable(int _nWarID, ref Dictionary<int, SSchemeMobaGoods> _dicNewTable)
        {
            List<int> WarIdList = null;
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };

            foreach (var goods in m_dicGoodsSchemeTable)
            {
                if(WarIdList != null)
                    WarIdList.Clear();
                if (!string.IsNullOrEmpty(goods.Value.GoodsWarLimit))
                {
                    WarIdList = ScpReader.ConvertStringToIntArray(goods.Value.GoodsWarLimit, charSeparator);
                    if (WarIdList.IndexOf(_nWarID) != -1)
                        _dicNewTable.Add(goods.Key, goods.Value);
                }
                else if(goods.Key == 0)
                    _dicNewTable.Add(goods.Key, goods.Value);
            }
        }

        /// <summary>
        /// 根据物品类型获取物品数据列表
        /// </summary>
        /// <param name="_nGoodsLabel">物品类型</param>
        /// <returns></returns>
        public ObservableList<SSchemeMobaGoods> GetGoodsSchemeList(string _strGoodsLabelID)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };

            //选的是类型子类
            if (Convert.ToInt32(_strGoodsLabelID) > 1000)
            {
                return m_dicGoodsSchemeTable.Values.Where((item) =>
                {
                    foreach (string label in item.GoodsLabel.Split(charSeparator))
                    {
                        if (string.IsNullOrEmpty(label))
                        {
                            continue;
                        }
                        if (label == _strGoodsLabelID)
                            return true;
                    }
                    return false;
                }).ToObservableList();
            }
            else
            {
                int nGoodsLabelID = Convert.ToInt32(_strGoodsLabelID) * 1000;
                int nItemLabelID = -1;
                return m_dicGoodsSchemeTable.Values.Where((item) =>
                {
                    foreach (string label in item.GoodsLabel.Split(charSeparator))
                    {
                        if (string.IsNullOrEmpty(label))
                        {
                            continue;
                        }
                        nItemLabelID = Convert.ToInt32(label);

                        if (nItemLabelID > nGoodsLabelID && nItemLabelID < nGoodsLabelID + 1000)
                            return true;
                    }
                    return false;
                }).ToObservableList();
            }
        }

        /// <summary>
        /// 获取指定物品合成所需物品ID列表
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <returns></returns>
        public List<int> GetSchemeGoodsNeedComposeList(int _nGoodsID)
        {

            SSchemeMobaGoods goods;

            GetMobaGoods_Out(_nGoodsID, out goods);


            if (goods.GoodsNeedComposeList != null && goods.GoodsNeedComposeList.Length != 0)
            {
                char[] charSeparator = new char[] { MobaSchemesConfig.separator };

                return ScpReader.ConvertStringToIntArray(goods.GoodsNeedComposeList, charSeparator);
            }

            return null;
        }

        /// <summary>
        /// 通过物品ID查找物品图标ID
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <returns></returns>
        public int GetGoodsIconID(int _nGoodsID)
        {
            SSchemeMobaGoods goods = null;
            if (GetMobaGoods_Out(_nGoodsID, out goods))
                return goods.GoodsIconID;
            return 0;
        }

        public void Init()
        {
            
        }
    }
}

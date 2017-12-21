using DataCenter;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.GameGoodsLabelDesConfig
{
    public class GameGoodsLabelDesConfig : Singleton<GameGoodsLabelDesConfig>, IModelConfig
    {
        private Hashtable labelDescirptionTable;

        public void Load(string strPath)
        {
            labelDescirptionTable = new Hashtable();
            ScpReader LabelDescriptionReader = new ScpReader(strPath, true, 2);
            for (int i = 0; i < LabelDescriptionReader.GetRecordCount(); i++)
            {
                int LabelKey = LabelDescriptionReader.GetInt(i, 1, 0);
                string LabelValue = LabelDescriptionReader.GetString(i, 0, "");

                labelDescirptionTable.Add(LabelKey, LabelValue);
            }

            LabelDescriptionReader.Dispose();

        }

        public void UnLoad()
        {
            if (labelDescirptionTable != null)
            {
                labelDescirptionTable.Clear();
                labelDescirptionTable = null;
            }
        }

        public void GetLabelDescirptionTable(ref Hashtable _table)
        {
            if (labelDescirptionTable == null)
            {
                return;
            }

            foreach (DictionaryEntry goods in labelDescirptionTable)
            {
                _table.Add(goods.Key, goods.Value);
            }
        }
    }
}

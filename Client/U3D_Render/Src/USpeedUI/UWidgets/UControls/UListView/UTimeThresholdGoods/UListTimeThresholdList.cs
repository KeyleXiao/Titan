using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.TimeThresholdGoodsPart;
namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListTimeThresholdList : ListViewCustom<UListComponentUTimeThresholdItem, UListTimeThresholdData>
    {
        private Dictionary<TimeThresholdGoodsType, ObservableList<UListTimeThresholdData>> dicListTimeThresholdData = new Dictionary<TimeThresholdGoodsType, ObservableList<UListTimeThresholdData>>();
        protected override void SetData(UListComponentUTimeThresholdItem component, UListTimeThresholdData item)
        {
            component.SetData(item);
        }

        protected override void HighlightColoring(UListComponentUTimeThresholdItem component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentUTimeThresholdItem component)
        {
            base.SelectColoring(component);
        }

        public Dictionary<TimeThresholdGoodsType, ObservableList<UListTimeThresholdData>> ListTimeThresholdData
        {
            set
            {
                dicListTimeThresholdData = value;
            }
            get
            {
                return dicListTimeThresholdData;
            }
        }

        public ObservableList<UListTimeThresholdData> GetTypeListTimeThresholdData(TimeThresholdGoodsType type)
        {
            if (!dicListTimeThresholdData.ContainsKey(type))
            {
                ObservableList<UListTimeThresholdData> ListData = new ObservableList<UListTimeThresholdData>();
                dicListTimeThresholdData[type] = ListData;
            }

            return dicListTimeThresholdData[type];
        }

        public void SetTypeListTimeThresholdData(TimeThresholdGoodsType type, ObservableList<UListTimeThresholdData> listData)
        {
            dicListTimeThresholdData[type] = listData;
        }
    }
}

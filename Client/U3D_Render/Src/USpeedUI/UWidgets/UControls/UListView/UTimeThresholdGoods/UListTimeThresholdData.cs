using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListTimeThresholdData
    {
        public int nGoodId;
        public int nGoodIconId;
        public int nGoodType;
        public string []sGoodName = new string[2];
        public long nGoodThresholdTime;
        public string szParam;
        public int nParam;
    }

    public class UListTimeThresholdDataComparison
    {
        public int Compare(UListTimeThresholdData x, UListTimeThresholdData y)
        {
            if (x == null)
            {
                if (y == null)
                    return 0;
                else
                    return -1;
            }
            else
            {
                if (y == null)
                {
                    return 1;
                }
                else
                {
                    int value = x.nGoodThresholdTime.CompareTo(y.nGoodThresholdTime);
                    if (value == 0)
                        value = x.nGoodType.CompareTo(y.nGoodType);
                    return value;
                }
            }
        }
    }
}

    

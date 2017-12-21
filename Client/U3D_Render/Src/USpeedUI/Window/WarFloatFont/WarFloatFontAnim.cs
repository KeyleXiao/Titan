using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarFloatFont
{
    [Serializable]
    public class FloatFontAnimData
    {
        public WarFloatFontType fontType;
        //public Color fontColor;                     //颜色
        public int fontSize;                            //起始大小
        public float expandTime = 0;                    //膨胀时间
        public float stagnationTime = 0;             //停滞时间
        public float disappearTime = 0;              //消失时间
        public Color topColor;                        //顶部颜色
        public Color bottomColor;                  //底部颜色
        public float startXOffset;                  //起始X值
        public float startYOffset;                  //起始Y值
        public float xOffset;                         //X偏移值
        public float yOffset;                         //Y偏移值
        public float expandMinPercent;           //膨胀最小比
        public float expandMaxPercent;          //膨胀最大比
        public float jumpPower;
        public bool bRandomStartPos;        //是否随机初始位置
        public float animTime { get { return expandTime + stagnationTime + disappearTime; } }
    }

    public class WarFloatFontAnim : MonoBehaviour
    {
        public FloatFontAnimData[] m_FontAnimList = new FloatFontAnimData[(int)WarFloatFontType.Max - 1];

        public FloatFontAnimData GetAnimData(WarFloatFontType type)
        {
            foreach (FloatFontAnimData item in m_FontAnimList)
            {
                if (item.fontType == type)
                    return item;
            }
            return null;
        }
    }
}

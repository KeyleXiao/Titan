/*******************************************************************
** 文件名:	GFxCampManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/2/12
** 版  本:	1.0
** 描  述:	GFx界面阵营管理器，负责将逻辑层发来的阵营类型转换为GFx界面显示效果
** 应  用:  目前支持根据阵营类型输出对应的颜色类型
 *
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/2/5
** 描  述:  增加敌方血量值达到可以被秒杀的类型
 *
********************************************************************/

using UnityEngine;
using System.Collections;
using Scaleform;
public class GFxCampManager
{
    //阵营类型
    public enum GFxCampTpye:int
    {
        CT_Unknow           =0x00    , 
        CT_Self             =0x01    , //本地玩家自己(颜值：green)
        CT_Friend           =0x02    , //己方(颜值：blue)
        CT_Enemy            =0x03    , //敌方（颜值：red）
        CT_Neutral          =0x04    , //中立（颜值：yellow）
        CT_EnemyInstantKill =0x05    , //敌方,并且达到秒杀值(颜值：gold)
    };
    //与阵营相关的类型
    public enum GFxCampRelated : int
    {
        CR_Name     =0x10   ,
        CR_Blood    =0x20   ,
    };

    public static GFxCampManager Instance
    {
        get
        {
            if (instance == null)
               instance = new GFxCampManager();
            return instance;
        }
    }

    protected static GFxCampManager instance = null;

    private SFCxForm SFCxRed, SFCxGreen, SFCxBlue, SFCxYellow, SFCxGray, SFCxWhite, SFCxGold;

    public GFxCampManager()
    {
        SFCxWhite   = new SFCxForm(Color.white);
        SFCxRed     = new SFCxForm(new Color(211, 119, 124, 255));
        SFCxGreen   = new SFCxForm(new Color(49, 169, 45, 255));
        SFCxBlue    = new SFCxForm(new Color(65, 198, 238, 255));
        SFCxYellow  = new SFCxForm(new Color(231 , 231 , 168 , 255));
        SFCxGray    = new SFCxForm(new Color(230 , 234 , 230 , 255));
        SFCxGold    = new SFCxForm(new Color(255 , 255 , 0,  255));
    }

    /// <summary>
    /// 根据阵营类型返回相应的颜色
    /// </summary>
    /// <returns>返回SF类型的Color</returns>
	public SFCxForm GetCampSFColorFromType(int _campType)
    {
        SFCxForm color = null;

        switch (_campType)
        {
            case ((int)GFxCampTpye.CT_Self | (int)GFxCampRelated.CR_Name)://17
                color = SFCxYellow;
                break;
            case ((int)GFxCampTpye.CT_Friend | (int)GFxCampRelated.CR_Name)://18
            case ((int)GFxCampTpye.CT_Enemy | (int)GFxCampRelated.CR_Name)://19
            case ((int)GFxCampTpye.CT_Neutral | (int)GFxCampRelated.CR_Name)://20
            case ((int)GFxCampTpye.CT_EnemyInstantKill | (int)GFxCampRelated.CR_Name)://21
                color = SFCxGray;
                break;

            case ((int)GFxCampTpye.CT_Self | (int)GFxCampRelated.CR_Blood)://33
                color = SFCxGreen;
                break;
            case ((int)GFxCampTpye.CT_Friend | (int)GFxCampRelated.CR_Blood)://34
                color = SFCxBlue;
                break;
            case ((int)GFxCampTpye.CT_Enemy | (int)GFxCampRelated.CR_Blood)://35
            case ((int)GFxCampTpye.CT_Neutral | (int)GFxCampRelated.CR_Blood)://36
                color = SFCxRed;
                break;
            case ((int)GFxCampTpye.CT_EnemyInstantKill | (int)GFxCampRelated.CR_Blood)://37
                color = SFCxGold;
                break;
            default:
                color = SFCxWhite;
                break;
        }
        //Trace.LogError("_campType=" + _campType);
        //Trace.LogError(("CxForm = " + color.GetElem(0) + " " + color.GetElem(1) + " " + color.GetElem(2) + " " + color.GetElem(3)
        //                            + " " + color.GetElem(4) + " " + color.GetElem(5) + " " + color.GetElem(6) + " " + color.GetElem(7)));
        return color;
    }

    public Color GetCampUColorFromType(int _campType)
    {
        SFCxForm SFColor = GetCampSFColorFromType(_campType);
        Color color = new Color((float)SFColor.GetElem(4), (float)SFColor.GetElem(5), (float)SFColor.GetElem(6), (float)SFColor.GetElem(7));
        return color;
    }
}

/*******************************************************************
** 文件名:	ASMobaGoods.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/8/10
** 版  本:	1.0
** 描  述:	这是AS脚本在CS上的表现与Com.Scaleform.Common.MobaGoods脚本要对应起来才能生成相应的值来传递
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/

using System;


public class ASMobaGoods : System.Object
{
    //物品ID
    private int _GoodsID;
    //物品图标ID
    private int _GoodsIconID;
    //物品花费
    private int _GoodsCost;
    //物品名字
    private string _GoodsName;
    //物品描述
    private string _GoodsDes;
    //物品标签
    private string _GoodsLabel;
    //物品效果信息
    private string _GoodsEffectInfo;

    //物品ID
    public int GoodsID
    {
    get
    { return _GoodsID; }
    set
    { _GoodsID = value; }
    }
    //物品图标ID
    public int GoodsIconID
    {
    get
    {
        return _GoodsIconID;
    }

    set
    {
        _GoodsIconID = value;
    }
    }
    //物品花费
    public int GoodsCost
    {
        get
        { return _GoodsCost; }
        set
        { _GoodsCost = value; }
    }
    //物品名字
    public string GoodsName
    {
        get
        { return _GoodsName; }
        set
        { _GoodsName = value; }
    }
    //物品描述
    public string GoodsDes
    {
        get
        { return _GoodsDes; }
        set
        { _GoodsDes = value; }
    }
    //物品标签
    public string GoodsLabel
    {
        get
        { return _GoodsLabel; }
        set
        { _GoodsLabel = value; }
    }
    //物品效果信息
    public string GoodsEffectInfo
    {
        get
        { return _GoodsEffectInfo; }
        set
        { _GoodsEffectInfo = value; }
    }


    public ASMobaGoods()
        { }
}
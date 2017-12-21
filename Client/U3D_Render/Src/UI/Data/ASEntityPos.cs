/*******************************************************************
** 文件名:	ASEntityPos.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李建伟
** 日  期:	2015/9/16
** 版  本:	1.0
** 描  述:	这是AS脚本在CS上的表现与Com.Scaleform.Common.EntityPos脚本要对应起来才能生成相应的值来传递
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/

using System;


public class ASEntityPos : System.Object
{
    // 实体ID
    private int _entityID;

    // x坐标
    private float _fPosX;

    // z坐标
    private float _fPosZ;

    // 朝向
    private float _fAngle;

    // 是否是建筑物
    private int _isTower;

    //实体ID
    public int entityID
    {
        get
        { return _entityID; }
        set
        { _entityID = value; }
    }

    //x坐标
    public float fPosX
    {
        get
        {
            return _fPosX;
        }

        set
        {
            _fPosX = value;
        }
    }

    // z坐标
    public float fPosZ
    {
        get
        { return _fPosZ; }
        set
        { _fPosZ = value; }
    }

    // 朝向
    public float fAngle
    {
        get
        { return _fAngle; }
        set
        { _fAngle = value; }
    }

    // 是否是建筑物
    public int isTower
    {
        get
        { return _isTower; }
        set
        { _isTower = value; }
    }

    public ASEntityPos()
    {
    }
}
/*******************************************************************
** �ļ���:	ASEntityPos.cs
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2014 - Speed
** ������:	�ΰ
** ��  ��:	2015/9/16
** ��  ��:	1.0
** ��  ��:	����AS�ű���CS�ϵı�����Com.Scaleform.Common.EntityPos�ű�Ҫ��Ӧ��������������Ӧ��ֵ������
** Ӧ  ��:  
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��:
********************************************************************/

using System;


public class ASEntityPos : System.Object
{
    // ʵ��ID
    private int _entityID;

    // x����
    private float _fPosX;

    // z����
    private float _fPosZ;

    // ����
    private float _fAngle;

    // �Ƿ��ǽ�����
    private int _isTower;

    //ʵ��ID
    public int entityID
    {
        get
        { return _entityID; }
        set
        { _entityID = value; }
    }

    //x����
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

    // z����
    public float fPosZ
    {
        get
        { return _fPosZ; }
        set
        { _fPosZ = value; }
    }

    // ����
    public float fAngle
    {
        get
        { return _fAngle; }
        set
        { _fAngle = value; }
    }

    // �Ƿ��ǽ�����
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
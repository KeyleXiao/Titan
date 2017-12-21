/*******************************************************************
** 文件名:	Effect_UVScroll.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	UV流动
** 应  用:  	UV流动会和UV动画冲突！

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

/// <summary>
/// UV流动
/// </summary>

public sealed class RoateSceneObj : MonoBehaviour
{
    public Vector3 RotateSpeed = Vector3.zero;

    /// <summary>
    /// X旋转时间
    /// </summary>
    public float RotateXTime = 9999999;

    /// <summary>
    /// Y旋转时间
    /// </summary>
    public float RotateYTime = 9999999;

    /// <summary>
    /// Z旋转时间
    /// </summary>
    public float RotateZTime = 9999999;
    public bool RoateInWorldSpace = false;

    private float currRotateXTime = 0.0f;
    private float currRotateYTime = 0.0f;
    private float currRotateZTime = 0.0f;

    public void Init()
    {
        currRotateXTime = 0.0f;
        currRotateYTime = 0.0f;
        currRotateZTime = 0.0f;
    }

    public void Start()
    {
        Init();
    }



    public void Update()
    {
        UVRotate();

    }


    private void UVRotate()
    {
        if (RotateSpeed != Vector3.zero)
        {
            if (RoateInWorldSpace)
            {
                Vector3 realyRotateSpeed = RotateSpeed;

                if (RotateXTime > 0 && currRotateXTime < RotateXTime)
                {
                    currRotateXTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.x = 0.0f;
                }

                if (RotateYTime > 0 && currRotateYTime < RotateYTime)
                {
                    currRotateYTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.y = 0.0f;
                }

                if (RotateZTime > 0 && currRotateZTime < RotateZTime)
                {
                    currRotateZTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.z = 0.0f;
                }

                if (realyRotateSpeed != Vector3.zero)
                {
                    transform.Rotate(realyRotateSpeed * Time.deltaTime, Space.World);
                }

            }
            else
            {
                Vector3 realyRotateSpeed = RotateSpeed;


                if (RotateXTime > 0 && currRotateXTime < RotateXTime)
                {
                    currRotateXTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.x = 0.0f;
                }

                if (RotateYTime > 0 && currRotateYTime < RotateYTime)
                {
                    currRotateYTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.y = 0.0f;
                }

                if (RotateZTime > 0 && currRotateZTime < RotateZTime)
                {
                    currRotateZTime += Time.deltaTime;
                }
                else
                {
                    realyRotateSpeed.z = 0.0f;
                }

                if (realyRotateSpeed != Vector3.zero)
                {
                    transform.Rotate(realyRotateSpeed * Time.deltaTime, Space.Self);
                }

            }

        }
    }
}



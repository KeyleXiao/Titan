/*******************************************************************
** 文件名:	SoldierCamera.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/18
** 版  本:	1.0
** 描  述:	相机控制，这个脚本只能控制主相机，而且与场景无关，只要场景中有一个主相机，这个脚本就能找到主相机，并进行控制
** 应  用:   只能控制住相机，即 Camera.main相机
 *          
**************************** 修改记录 ******************************
** 修改人:    
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using GameLogic;
using UnityEngine.Events;

public class SoldierCameraModeChangeEvent : UnityEvent<CameraMode>
{
}
public abstract partial class BaseSoldierCamera : MonoBehaviour
{

    protected static BaseSoldierCamera _instance;
    //public static BaseSoldierCamera MainInstance
    //{
    //    get { return _instance; }
    //}
    public static T MainInstance<T>() where T : BaseSoldierCamera
    {
        return _instance as T;
    }
    public static BaseSoldierCamera MainInstance()
    {
        return _instance;
    }
    protected SoldierCameraModeChangeEvent modeChangeEvent = new SoldierCameraModeChangeEvent();

    protected CameraMode m_cameraMode;
    public CameraMode cameraMode
    {
        get
        {
            return m_cameraMode;
        }
        set
        {
            m_cameraMode = value;
            InvokeCameraModeChangeEvent();
            
        }
    }

    protected Camera m_Camera;
    public Camera CurrentCamera
    {
        get { return m_Camera; }
    }

    protected Transform camTransform;
    public Transform getCameraTransform()
    {
        return camTransform;
    }
    public Transform target;
    public void RegsiterCameraModeChangeEvent(UnityAction<CameraMode> callback)
    {
        if (null != modeChangeEvent)
        {
            if (null != callback)
            {
                modeChangeEvent.AddListener(callback);
            }
        }
    }

    public void UnRegsiterCameraModeChangeEvent(UnityAction<CameraMode> callback)
    {
        if (null != modeChangeEvent)
        {
            if (null != callback)
            {
                modeChangeEvent.RemoveListener(callback);
            }
        }
    }

    public void ClearCameraModeChangeEvent()
    {
        if (null != modeChangeEvent)
        {
            modeChangeEvent.RemoveAllListeners();
        }
    }

    private void InvokeCameraModeChangeEvent()
    {
        if (null != modeChangeEvent)
        {
            modeChangeEvent.Invoke(m_cameraMode);
        }
    }

    public abstract GameObject GetSoliderGo();

    public abstract float GetSoliderHeightInMeters();


}

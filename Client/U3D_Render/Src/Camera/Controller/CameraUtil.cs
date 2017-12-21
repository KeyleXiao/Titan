/*******************************************************************
** 文件名:	CameraUtil.cs
** 描  述:	摄像机常用工具类
** 应  用:  	
            InitCameraMode：初始化CameraMode：
                这样新建一个类，就会自动初始化进camCtrlList
            ProjectToFloor：投影到地面（这里应该是整个场景的工具，因为raycast也经常用）
            摄像机系统的log函数
            各种插值工具（用途不明）？
********************************************************************/
using UnityEngine;
using U3D_Render.CameraControl;
public class CameraUtil
{
    /// <summary>
    /// 是否在摄像机视野内
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    public static bool isOnCameraView(Vector3 pos)
    {
        if (!Initialize.mainCam)
        {
            return false;
        }

        Vector3 ViewPos = Initialize.mainCam.WorldToViewportPoint(pos);
        if (ViewPos.x < 0 || ViewPos.x > 1 ||
            ViewPos.y < 0 || ViewPos.y > 1 ||
                ViewPos.z < 0)
            return false;
        return true;
    }

    private static BaseCameraController[] camCtrlList = null;

    public static void InitCameraMode<T>(CameraMode mode, T camCtrl ) where T: BaseCameraController
    {
        if (camCtrlList == null)
            camCtrlList = new BaseCameraController[(int)CameraMode.MaxCount];
        camCtrlList[(int)mode] = camCtrl;
    }

    public static BaseCameraController GetCamCtrl(CameraMode mode)
    {
        return camCtrlList[(int)mode];
    }

    public static Vector3 ProjectToFloor(Vector3 posBeforeProject)
    {
        Vector3 posAfterProject = posBeforeProject;
        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        LayerMaskOnGround = LayerMaskOnGround | (1 << LayerMask.NameToLayer(Config.LayerBuilding));

        Ray upRay = new Ray(posAfterProject + new Vector3(0, 0.5f, 0), new Vector3(0, 1, 0));
        RaycastHit hitInfo;
        //先向上取10米，并且确保不会撞到顶，如果是在石头内部的话，需要确保石头法向向外，这样就不会判定为hit到。
        bool hitUp = Physics.Raycast(upRay, out hitInfo, 10.0f, LayerMaskOnGround);
        if (hitUp)
        {
            posAfterProject = hitInfo.point;
        }
        else
        {
            posAfterProject = posAfterProject + new Vector3(0, 10.0f, 0);
        }
        Ray downRay = new Ray(posAfterProject - new Vector3(0, 0.1f, 0), new Vector3(0, -1, 0));
        bool hitGround = Physics.Raycast(downRay, out hitInfo, 100.0f, LayerMaskOnGround);
        if (hitGround)
        {
            posAfterProject = hitInfo.point;
        }
        else
        {
            posAfterProject = posBeforeProject;
        }

        return posAfterProject;
    }

    public static Vector3 GetGodEyePos()
    {
        return ((MapGodCameraModeController)GetCamCtrl(CameraMode.MapGodControl)).GetGodEyePos();
    }

    public static void Log(string msg)
    {
        if (SoldierCamera.MainInstance<SoldierCamera>().isDebug)
            Trace.Log(msg);
    }
    public static void LogError(string msg)
    {
        if (SoldierCamera.MainInstance<SoldierCamera>().isDebug)
            Trace.LogError(msg);
    }

    public static void LogWarning(string msg)
    {
        if (SoldierCamera.MainInstance<SoldierCamera>().isDebug)
            Trace.LogWarning(msg);
    }


    #region 各种插值函数

    public static float approximateFloat(float cur, float dest, float factor)
    {
        float delta = dest - cur;
        if (Mathf.Abs(delta) < 0.01f)
            return dest;
        float ret = Mathf.Lerp(cur, dest, factor);
        return ret;
    }



    ////////////参数有待优化
    public static float approximateAngleFloat(SoldierCamera soldierCam,float cur, float dest, float factor, ref bool reached, float angleThroad = 0.01f)
    {
        reached = false;
        float delta = Mathf.DeltaAngle(cur, dest);
        if (Mathf.Abs(delta) < angleThroad)
        {
            reached = true;
            return dest;
        }

        float ret;
        if (factor < 0.01f)
        {
            if (delta * soldierCam.m_approximageRotateSpeed < 0.0f)
            {
                soldierCam.m_approximageRotateSpeed = -soldierCam.m_approximageRotateSpeed;
            }
            float step = soldierCam.m_approximageRotateSpeed * Time.deltaTime;
            if (delta * soldierCam.rotateAcceleratedSpeed < 0.0f)
            {
                soldierCam.rotateAcceleratedSpeed = -soldierCam.rotateAcceleratedSpeed;
            }
            soldierCam.m_approximageRotateSpeed = soldierCam.m_approximageRotateSpeed + soldierCam.rotateAcceleratedSpeed * Time.deltaTime;

            if (Mathf.Abs(Mathf.DeltaAngle(cur, dest)) < Mathf.Abs(step))
            {
                ret = dest;
                reached = true;
            }
            else
            {
                ret = cur + step;
            }

        }
        else
        {
            ret = Mathf.LerpAngle(cur, dest, factor);
        }
        while (ret > 360) ret -= 360;
        while (ret < 0) ret += 360;
        return ret;
    }

    public static Vector3 approximateVec(Vector3 cur, Vector3 dest, float factor)
    {
        Vector3 delta = dest - cur;
        if (delta.sqrMagnitude < 0.001f)
            return dest;
        Vector3 ret = Vector3.Lerp(cur, dest, factor);
        return ret;
    }

    public static Vector3 approximateAngleVec(Vector3 cur, Vector3 dest, float factor)
    {
        Vector3 delta = dest - cur;
        if (delta.sqrMagnitude < 0.001f)
            return dest;
        Vector3 ret;
        ret.x = Mathf.LerpAngle(cur.x, dest.x, factor);
        ret.y = Mathf.LerpAngle(cur.y, dest.y, factor);
        ret.z = Mathf.LerpAngle(cur.z, dest.z, factor);
        return ret;
    }

    public static void advanceValue(ref float cur, float dest, float advance)
    {
        float delta = dest - cur;
        if (advance * advance > delta * delta)
        {
            advance = delta;
        }
        cur += advance;
    }

    public static void advanceVec(ref Vector3 cur, Vector3 dest, Vector3 advance)
    {
        Vector3 delta = dest - cur;
        if (advance.sqrMagnitude > delta.sqrMagnitude)
        {
            advance = delta;
        }
        cur += advance;
    }
    #endregion
}

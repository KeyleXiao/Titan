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
using U3D_Render.CameraControl;
using Effect;


public enum CameraNavMode //各种不同的转镜头，优先级由低到高，低的无法打断高的，进入高的自动覆盖低的
{
    None = 0,
    Follow = 1,
    Target = 2,
    FloatingAngle = 3,
    Collect = 4,
    Nav = 5,
}

public partial class SoldierCamera : BaseSoldierCamera
{
    #region Nav+Collect


    private int m_navModeFollowCount = 0; //Follow模式计数器，应对多个地方进入此状态冲突
    private Transform m_navTragetTransform; //跟随对象转镜头的对象
    private Vector3 m_navTargetAngle = Vector3.zero; //转向指定角度
    public void enterNavMode(CameraNavMode enterNavMode, Transform refTransform, Vector3 refAngle)
    {
        if (camCtrl == null)
            return;
        if (enterNavMode == CameraNavMode.Follow)
        {
            m_navModeFollowCount++;
        }
        if (NavMode > enterNavMode) return; //正在进行优先级更高的操作，直接挡掉了        
        if (enterNavMode == CameraNavMode.Target)
        {
            if (refTransform)
            {
                if (m_navTragetTransform == refTransform)
                {
                    //已经跟踪在旋转了，直接退了吧
                    return;
                }
                Vector3 direction = refTransform.position - soldier.gameObject.transform.transform.position;
                if (direction.magnitude > rotateStartMaxDistance || direction.magnitude < rotateStartMinDistance)
                {
                    return; //距离限制不开启
                }

                if (ScreenRaycast.isInScreen(refTransform.position))
                {
                    return; //在屏幕内的不开启
                }


                direction.y = 0;
                direction.Normalize();
                float angle = Mathf.Rad2Deg * (Mathf.Atan2(direction.x, direction.z));

                float detaAngle = Mathf.DeltaAngle(m_userDesireAngle.y, angle);
                if (Mathf.Abs(detaAngle) <= rotateStartAngle) //小于rotateStartAngle度不开启
                {
                    return;
                }

                if (enterNavMode != NavMode) //刚进入此模式，从初速度开始旋转
                {
                    m_approximageRotateSpeed = rotateStartSpeed;
                }

                m_navTragetTransform = refTransform;
            }
            else
            {
                if (NavMode == enterNavMode)
                {
                    exitNavMode(NavMode);
                }
                return;
            }
        }
        else if (enterNavMode == CameraNavMode.FloatingAngle)
        {
            if (refAngle.sqrMagnitude > 0.01f)
            {
                m_navTargetAngle = refAngle;
            }
            else
            {
                exitNavMode(enterNavMode);
            }
        }
        NavMode = enterNavMode;
    }
    public void exitNavMode(CameraNavMode navMode)
    {
        if (navMode == CameraNavMode.Follow)
        {
            m_navModeFollowCount--;
            if (m_navModeFollowCount <= 0)
            {
                m_navModeFollowCount = 0;
            }
        }

        if (navMode == CameraNavMode.Target)
        {
            m_navTragetTransform = null;
        }

        if (navMode == CameraNavMode.FloatingAngle)
        {
            m_navTragetTransform = null;
        }

        if (navMode != NavMode)
        {
            return;
        }
        else
        {
            NavMode = CameraNavMode.None;
        }
    }


    public CameraNavMode NavMode = CameraNavMode.None;
    public float NavPauseTime = 0.0f;
    public float NavPauseInterval = 2.0f;
    public float NavTravelTime = 1.0f;
    public bool NavLog = false;
    public const float NavDefRange = 10.0f;

    private GameObject NavNodeGroup;

    private List<CameraNavNode> matchnodes = new List<CameraNavNode>();
    private List<CameraNavNode> blendnodes = new List<CameraNavNode>();
    private bool bNavRun = false;
    //导航协程(对协程不甚了解或者不了解yield return 系列函数用法慎改)
    IEnumerator NavEnumerator()
    {
        bNavRun = true;

        matchnodes.Clear();
        Vector3 currentpos = m_userDesirePos;

        if (NavNodeGroup != null)
        {
            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
            {
                if (NavMode != CameraNavMode.Nav) //处于采集模式不导航，歇会退出协程
                {
                    yield return new WaitForSeconds(1.0f);
                    break;
                }

                float time = Time.time; //用户操作期间暂停自动导航NavPauseInterval秒
                if (time - NavPauseTime < NavPauseInterval) //用户操作中等待NavPauseInterval秒继续协程
                {
                    yield return new WaitForEndOfFrame();
                    continue;
                }

                //下面逐个每间隔nexttime时间去导航info点遍历一次
                Transform tchild = NavNodeGroup.transform.GetChild(index);
                CameraNavNode node = tchild.GetComponent<CameraNavNode>();
                if (node != null)
                {
                    Vector3 vec = node.pos - currentpos;
                    float mag = vec.magnitude;
                    if (mag < node.range * 0.5f)
                    {
                        matchnodes.Add(node);
                    }
                }

                float nexttime = NavTravelTime;
                if (NavNodeGroup.transform.childCount > 0)
                {
                    nexttime = NavTravelTime / NavNodeGroup.transform.childCount;
                }
                yield return new WaitForSeconds(nexttime);

            }
        }
        else //sleep 1秒
        {
            yield return new WaitForSeconds(1.0f);
        }

        blendnodes.Clear();
        for (int k = 0; k < matchnodes.Count; k++)
        {
            CameraNavNode node = matchnodes[k];
            if (node == null)
            {
                continue;
            }
            blendnodes.Add(node);
        }

        if (blendnodes.Count > 0)
        {
            Vector3 blendpos = new Vector3(0, 0, 0);
            Vector3 blendangle = new Vector3(0, 0, 0);
            Vector3 blenddir = new Vector3(0, 0, 0);
            Vector3 blendtran = new Vector3(0, 0, 0);
            float blenddistance = 0.0f;
            float blendfov = 0.0f;

            //下面先计算总权重
            float totalweight = 0.0f;
            for (int k = 0; k < blendnodes.Count; k++)
            {
                CameraNavNode node = blendnodes[k];
                Vector3 vec = node.pos - currentpos;
                float mag = vec.magnitude;
                float weight = 0.0f;
                if (node.range > 0.01f && mag < node.range * 0.5f)
                {
                    weight = 1.0f - mag / node.range * 0.5f;
                }
                totalweight += weight;
            }

            //按距离加权混合
            for (int k = 0; k < blendnodes.Count; k++)
            {
                CameraNavNode node = blendnodes[k];
                Vector3 vec = node.pos - currentpos;
                float mag = vec.magnitude;
                float weight = 0.0f;
                if (node.range > 0.01f && mag < node.range * 0.5f)
                {
                    weight = 1.0f - mag / node.range * 0.5f;
                }
                float factor = weight / totalweight;

                blendpos += node.pos * factor;
                blendangle += node.angle * factor;
                blenddir += node.dir * factor;
                blendtran += node.tran * factor;
                blenddistance += node.distance * factor;
                blendfov += node.fov * factor;
            }

            if (NavLog)
            {
                string msg = "nav:";
                for (int k = 0; k < blendnodes.Count; k++)
                {
                    CameraNavNode node = blendnodes[k];
                    if (node == null)
                    {
                        continue;
                    }
                    msg += blendnodes[k].name + ",";
                }
            }

            for (int k = 0; k < 60; k++) //通过60帧逼近目标值，因为navfactor因此逼近效果只依赖于帧数
            {
                m_userDesireAngle = CameraUtil.approximateAngleVec(m_userDesireAngle, blendangle, navFactor);
                m_userDesireDir = CameraUtil.approximateVec(m_userDesireDir, blenddir, navFactor);
                m_userDesireTran = CameraUtil.approximateVec(m_userDesireTran, blendtran, navFactor);
                m_userDesireDistance = CameraUtil.approximateFloat(m_userDesireDistance, blenddistance, navFactor);
                m_userDesireFov = CameraUtil.approximateFloat(m_userDesireFov, blendfov, navFactor);

                yield return new WaitForEndOfFrame();
            }


        }
        else  //sleep 1秒
        {
            yield return new WaitForSeconds(1.0f);
        }

        bNavRun = false;
    }



    public bool bCollectRun = false;
    //导航协程(对协程不甚了解或者不了解yield return 系列函数用法慎改)
    public IEnumerator CollectEnumerator()
    {
        bCollectRun = true;

        Vector3 currentpos = m_userDesirePos;
        CameraNavNode retnode = null;
        if (NavNodeGroup != null)
        {
            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
            {
                if (NavMode != CameraNavMode.Collect)  //处于采集模式不导航
                {
                    yield return new WaitForSeconds(1.0f);
                    break;
                }

                //下面逐个每间隔nexttime时间去导航info点遍历一次
                Transform tchild = NavNodeGroup.transform.GetChild(index);
                CameraNavNode node = tchild.GetComponent<CameraNavNode>();
                if (node != null)
                {
                    Vector3 vec = node.pos - currentpos;
                    float mag = vec.magnitude;
                    if (mag < node.range * 0.5f)
                    {
                        retnode = node;
                        break;
                    }
                }

                float nexttime = NavTravelTime;
                if (NavNodeGroup.transform.childCount > 0)
                {
                    nexttime = NavTravelTime / NavNodeGroup.transform.childCount;
                }
                yield return new WaitForSeconds(nexttime);
            }

        }

        if (NavMode == CameraNavMode.Collect)
        {
            if (retnode != null) //如果已经有数据，加权到旧数据
            {
                CameraNavNode info = retnode;
                if (!info.locked)
                {
                    float oldcount = info.count;
                    float newcount = oldcount + 1.0f;
                    //info.pos = (info.pos * oldcount + m_userDesirePos) / newcount;
                    info.angle = (info.angle * oldcount + m_userDesireAngle) / newcount;
                    info.dir = (info.dir * oldcount + m_userDesireDir) / newcount;
                    info.dir.Normalize();
                    info.tran = (info.tran * oldcount + m_userDesireTran) / newcount;
                    info.distance = (info.distance * oldcount + m_userDesireDistance) / newcount;
                    info.fov = (info.fov * oldcount + m_userDesireFov) / newcount;
                    info.count++;
                    //info.range = info.range;
                }

                if (NavLog)
                {
                    Debug.Log("collect old node" + info.name);
                }
            }
            else //否则添加新数据
            {
                GameObject go = BuildNavNode(currentpos, m_userDesireAngle, m_userDesireDir, m_userDesireTran, m_userDesireDistance, m_userDesireFov);
                CameraNavNode info = go.AddComponent<CameraNavNode>();
                info.pos = currentpos;
                info.angle = m_userDesireAngle;
                info.dir = m_userDesireDir;
                info.tran = m_userDesireTran;
                info.distance = m_userDesireDistance;
                info.fov = m_userDesireFov;
                info.count = 1;
                info.locked = false;
                info.range = NavDefRange;

                if (NavLog)
                {
                    Debug.Log("collect new node" + info.name);
                }
            }
        }


        yield return new WaitForSeconds(1.0f);

        bCollectRun = false;
    }



    private GameObject BuildNavNode(Vector3 pos, Vector3 angle, Vector3 dir, Vector3 tran, float distance, float fov)
    {
        if (NavNodeGroup == null)
        {
            NavNodeGroup = new GameObject("NavNodeGroup");
        }


        GameObject go = new GameObject();
        go.transform.parent = NavNodeGroup.transform;
        string name = "navnode_" + pos.ToString();
        go.name = name;


        GameObject dirgo = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        dirgo.name = "viewdir";
        dirgo.transform.parent = go.transform;
        dirgo.transform.localEulerAngles = new Vector3(90.0f, 30.0f, 0.0f);
        dirgo.transform.localScale = new Vector3(0.1f, 1.0f, 0.1f);
        dirgo.transform.localPosition = new Vector3(0.0f, 0, 0.0f);
        Collider dirc = dirgo.GetComponent<Collider>();
        if (dirc != null)
        {
            ResNode.DestroyRes(ref dirc);
        }

        GameObject eyego = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        eyego.name = "eye";
        eyego.transform.parent = go.transform;
        eyego.transform.localEulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
        eyego.transform.localScale = new Vector3(1, 1, 1);
        eyego.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
        Collider eyec = eyego.GetComponent<Collider>();
        if (eyec != null)
        {
            ResNode.DestroyRes(ref eyec);
        }

        GameObject lookatgo = GameObject.CreatePrimitive(PrimitiveType.Capsule);
        lookatgo.name = "lookat";
        lookatgo.transform.parent = go.transform;
        //lookatgo.transform.localEulerAngles = new Vector3(0.0f, 0.0f, 0.0f);
        lookatgo.transform.localScale = new Vector3(1.0f, 2.0f, 1.0f);
        //lookatgo.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
        Collider lookatc = lookatgo.GetComponent<Collider>();
        if (lookatc != null)
        {
            ResNode.DestroyRes(ref lookatc);
        }

        GameObject rangego = GameObject.CreatePrimitive(PrimitiveType.Quad);
        rangego.name = "range";
        rangego.transform.parent = go.transform;
        rangego.transform.localEulerAngles = new Vector3(90.0f, 0.0f, 0.0f);
        rangego.transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
        rangego.transform.localPosition = new Vector3(0.0f, -1.0f, 0.0f);
        Collider rangec = rangego.GetComponent<Collider>();
        if (rangec != null)
        {
            ResNode.DestroyRes(ref rangec);
        }
        rangego.SetActive(false);

        GameObject dirago = new GameObject();
        dirago.transform.parent = eyego.transform;
        dirago.transform.localEulerAngles = new Vector3(90.0f, 30.0f, 0.0f);
        dirago.transform.localScale = new Vector3(0.5f, 1.0f, 0.5f);
        dirago.transform.localPosition = new Vector3(0, 0, 0);

        GameObject dirbgo = new GameObject();
        dirbgo.transform.parent = eyego.transform;
        dirbgo.transform.localEulerAngles = new Vector3(90.0f, -30.0f, 0.0f);
        dirbgo.transform.localScale = new Vector3(0.5f, 1.0f, 0.5f);
        dirbgo.transform.localPosition = new Vector3(0, 0, 0);


        GameObject diraago = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        diraago.transform.parent = dirago.transform;
        diraago.transform.localPosition = new Vector3(0.5f, 0, 0);
        Collider dirac = diraago.GetComponent<Collider>();
        if (dirac != null)
        {
            ResNode.DestroyRes(ref dirac);
        }

        GameObject dirbbgo = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        dirbbgo.transform.parent = dirbgo.transform;
        dirbbgo.transform.localPosition = new Vector3(-0.5f, 0, 0);
        Collider dirbc = dirbbgo.GetComponent<Collider>();
        if (dirbc != null)
        {
            ResNode.DestroyRes(ref dirbc);
        }

        //Vector3 gridpos = new Vector3(grid.x * NodeSize, pos.y + 1.0f, grid.z * NodeSize);
        //go.transform.SetPosition( gridpos;

        go.transform.SetPosition(pos);

        return go;
    }



    public void SaveNavInfosToFile(string path)
    {
        StreamWriter sw = new StreamWriter(path, false);
        sw.WriteLine("#,posx,posy,posz,anglex,angley,anglez,dirx,diry,dirz,tranx,trany,tranz,distance,fov,count,range,locked");

        if (NavNodeGroup != null)
        {
            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
            {
                Transform tchild = NavNodeGroup.transform.GetChild(index);
                CameraNavNode info = tchild.GetComponent<CameraNavNode>();
                if (info == null)
                {
                    continue;
                }
                string sline = "node,";
                sline += info.pos.x + ",";
                sline += info.pos.y + ",";
                sline += info.pos.z + ",";
                sline += info.angle.x + ",";
                sline += info.angle.y + ",";
                sline += info.angle.z + ",";
                sline += info.dir.x + ",";
                sline += info.dir.y + ",";
                sline += info.dir.z + ",";
                sline += info.tran.x + ",";
                sline += info.tran.y + ",";
                sline += info.tran.z + ",";
                sline += info.distance + ",";
                sline += info.fov + ",";
                sline += info.count + ",";
                sline += info.range + ",";
                int locked = info.locked ? 1 : 0;
                sline += locked + ",";

                sw.WriteLine(sline);
            }
        }

        sw.Close();
    }

    public void LoadNavInfoFromFile(string path)
    {
        string text = File.ReadAllText(path);
        LoadNavInfoFromText(text, "SoldierCamera");
    }

    public void LoadNavInfoFromText(string text, string tag)
    {
        if (NavNodeGroup != null)
        {
            for (int index = 0; index < NavNodeGroup.transform.childCount; index++)
            {
                Transform tchild = NavNodeGroup.transform.GetChild(index);
                var temp = tchild.gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }


        ScpReader reader = new ScpReader(text, tag);

        for (int i = 0; i < reader.GetRecordCount(); i++)
        {
            string cmd = reader.GetString(i, 0, "");
            if (cmd != "node")
            {
                continue;
            }

            Vector3 pos;
            pos.x = reader.GetFloat(i, 1, 0.0f);
            pos.y = reader.GetFloat(i, 2, 0.0f);
            pos.z = reader.GetFloat(i, 3, 0.0f);
            Vector3 angle;
            angle.x = reader.GetFloat(i, 4, 0.0f);
            angle.y = reader.GetFloat(i, 5, 0.0f);
            angle.z = reader.GetFloat(i, 6, 0.0f);
            Vector3 dir;
            dir.x = reader.GetFloat(i, 7, 0.0f);
            dir.y = reader.GetFloat(i, 8, 0.0f);
            dir.z = reader.GetFloat(i, 9, 0.0f);
            Vector3 tran;
            tran.x = reader.GetFloat(i, 10, 0.0f);
            tran.y = reader.GetFloat(i, 11, 0.0f);
            tran.z = reader.GetFloat(i, 12, 0.0f);
            float distance;
            distance = reader.GetFloat(i, 13, 0.0f);
            float fov;
            fov = reader.GetFloat(i, 14, 0.0f);
            int count;
            count = reader.GetInt(i, 15, 0);
            float range;
            range = reader.GetFloat(i, 16, NavDefRange);
            range = Mathf.Clamp(range, 0.0f, 100.0f);
            bool locked;
            locked = reader.GetInt(i, 17, 0) != 0;

            GameObject go = BuildNavNode(pos, angle, dir, tran, distance, fov);
            CameraNavNode info = go.AddComponent<CameraNavNode>();
            info.pos = pos;
            info.angle = angle;
            info.dir = dir;
            info.tran = tran;
            info.distance = distance;
            info.fov = fov;
            info.count = 1;
            info.range = range;
            info.locked = locked;

        }

        reader.Dispose();


    }

    //自动采集镜头
    public void CollectNormally()
    {
        if (m_Camera != Initialize.mainCam || camCtrl == null)
        {
            return;
        }

        if (NavMode != CameraNavMode.Collect) //处于采集模式不导航
        {
            return;
        }
        else
        {
            if (!bCollectRun)
            {
                StartCoroutine(CollectEnumerator());
            }
        }
    }

    //自动导航镜头
    public void NavNormally()
    {
        if (m_Camera != Initialize.mainCam)
        {
            return;
        }
        if (soldier != null && soldier.gameObject)
        {
            return;
        }
        if (camCtrl == null)
            return;
        if (NavMode == CameraNavMode.Follow)
        {
            bool reached = false;
            SetUserDesireAngleY(CameraUtil.approximateAngleFloat(this, m_userDesireAngle.y, soldier.gameObject.transform.eulerAngles.y, followNavFactor, ref reached));
        }


        if (NavMode == CameraNavMode.Nav)
        {
            if (!bNavRun)
            {
                StartCoroutine(NavEnumerator()); ////这个类不是monobehaviour
            }
        }
        else
        {
            NavPauseTime = Time.time;
        }

        if (NavMode == CameraNavMode.Target)
        {
            if (soldier != null && m_navTragetTransform)
            {
                bool reached = false;
                if (ScreenRaycast.isInScreen(m_navTragetTransform.position, 0.1f))
                {
                    reached = true; //比较正中就停止了
                }
                else
                {
                    Vector3 direction = m_navTragetTransform.position - soldier.gameObject.transform.transform.position;
                    direction.y = 0;
                    direction.Normalize();
                    float angle = Mathf.Rad2Deg * (Mathf.Atan2(direction.x, direction.z));
                    SetUserDesireAngleY(CameraUtil.approximateAngleFloat(this, m_userDesireAngle.y, angle, 0.0f, ref reached, 10.0f));

                }
                if (reached)
                {
                    //转到想要的角度
                    exitNavMode(NavMode);
                }

            }
            else
            {
                //自己或者目标已被删除
                exitNavMode(NavMode);
            }
        }
        else if (NavMode == CameraNavMode.FloatingAngle)
        {
            bool reached = false;
            SetUserDesireAngleX(CameraUtil.approximateAngleFloat(this, m_userDesireAngle.x, m_navTargetAngle.x, followNavFactor * 2.0f, ref reached));
            if (reached)
            {
                exitNavMode(NavMode);
            }
        }

        if (mouseCtrl && mouseCtrl.cameraRotateButton && camCtrl.canCameraRotate())
        {
            NavPauseTime = Time.time;//如果用户操作了暂停自动导航一会         
        }
    }
    #endregion
}

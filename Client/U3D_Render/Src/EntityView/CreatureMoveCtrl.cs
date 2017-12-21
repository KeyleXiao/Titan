using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using GameLogic;
using USpeedUI;

/**
CreatureMoveCtrl
CreatureStateMachine的子部件，负责非键盘操作的位移，
包括强制冲锋，浮空，自动寻路
 */
public class CreatureMoveCtrl
{
    private CreatureStateMachine m_pFSM = null;

    // 构造
    public CreatureMoveCtrl(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        m_autoMoveNavModeFollow = false;
    }

    private void forceMoveToTarget()
    {
        if (m_bForceLockTarget && m_tForceMoveTarget)
        {
            m_ForceMovePos = m_tForceMoveTarget.position;
            //Debug.Log("m_ForceMovePos=" + m_ForceMovePos + "m_fForceMvoeSpeed=" + m_fForceMoveSpeed + "m_nForceMoveEndTick=" + m_nForceMoveEndTick);
        }

        Vector3 moveDis = m_ForceMovePos - m_pFSM.transform.position;
        m_fForceMoveSpeed = moveDis.normalized * m_fForceMoveSpeed.magnitude;

        if (bForceMoving && !m_bIgnoreSoliderController && m_pFSM.soliderController && m_pFSM.soliderController.isStopMove())
        {
            StopForceMove();
        }

        if (bForceFlighting)
        {
            forceFlight();
        }
        uint tick = GameLogicAPI.getTickCount();
        Vector3 dis = m_ForceMovePos - m_pFSM.transform.position;
        //if (!m_bDisableController) dis.y = 0; //忽略Y轴距离
        if (bForceFlighting) dis.y = 0; //有击飞的时候，Y轴由forceFlight()控制

        //if (showLog)
        //{
        //    Trace.Log("Final" + (dis + transform.position).ToString());
        //}

        float step = Mathf.Max(0.1f, m_fForceEndDistance);
        if (bForceMoving && (tick >= m_nForceMoveEndTick || dis.magnitude <= step+0.1f))
        {
            StopForceMove();
            bForceMoving = false;
            m_fForceMoveSpeed = Vector3.zero;

            if (m_pFSM.showLog)
            {
                Trace.Log("ForceMoveingEnd" + m_pFSM.transform.position);
            }

            m_pFSM.syncPos();
            return;
        }

        if (bForceMoving)
        {
            m_fForceMoveSpeed = dis.normalized * m_fForceMoveSpeed.magnitude;
            Vector3 currentMovementOffset = m_fForceMoveSpeed * Time.deltaTime;
            float maxStep = dis.magnitude - m_fForceEndDistance;
            if (currentMovementOffset.magnitude > maxStep)
            {
                currentMovementOffset = dis.normalized * maxStep;
                m_nForceMoveEndTick = tick;
            }
            if (bForceFlighting && !m_bDisableController)
            {
                currentMovementOffset.y = 0.0f;
            }

            //if (m_bDisableController)
            //{
            //    m_pFSM.transform.position += currentMovementOffset;
            //}
            //else
            //{
            //    m_pFSM.collisionFlags = m_pFSM.controller.Move(currentMovementOffset);
            //}  2016.07.19修改版本，m_bDisableController只是移动到不可拾取层，不和人物怪物发生物理碰撞，仍然和地面建筑发生碰撞
            Vector3 orgPos = m_pFSM.transform.position;
            if (m_bDisableController)
            {
                m_pFSM.transform.position = orgPos + currentMovementOffset;
            }
            else
            {
                CollisionFlags flag = m_pFSM.controller.Move(currentMovementOffset);
                if ((flag & CollisionFlags.CollidedSides) != 0) //撞东西了
                {
                    //Debug.Log("collider org=" + orgPos.ToString("f8") + " to=" + (orgPos + currentMovementOffset).ToString("f8") + " now" + m_pFSM.transform.position.ToString("f8"));

                    m_pFSM.controller.transform.position = orgPos;
                    m_pFSM.controller.Move(new Vector3(0.0f, 2.0f, 0.0f)); //向上爬两米，尝试绕过
                    CollisionFlags flag1 = m_pFSM.controller.Move(currentMovementOffset);
                    if ((flag1 & CollisionFlags.CollidedSides) != 0) //还是撞东西了
                    {
                        //2017-04-13，建伟需求，撞了也强制移到目标点，避免不同步
                        m_pFSM.controller.transform.position = m_ForceMovePos;
                        StopForceMove();
                    }

                }
            }
        }
    }


    public bool bForceFlighting = false;
    private float m_fFlightV0;
    private float m_fFlightTime;
    private float m_fFlightDownTime;
    private float m_fFlightVel;
    private uint m_uResidenceStopTick;
    private Vector3 m_flightStartPosition; //开始上升时候的位置记录，如果是连续浮空的话，会记最后一次浮空的开始位置
    private Vector3 m_flightEndPosition; //浮空的结束位置，如果是连续浮空的话，会记第一次浮空的开始位置
    private uint m_uResidenceTick;
    private float m_fFlightHeight;
    private float upAcc;
    private float downAcc;


    public void ForceFlight(cmd_effect_Flight param)
    {
        if (m_pFSM.bNotMove)
        {
            return;
        }

        if (m_pFSM.showLog)
        {
            Trace.Log(GameLogicAPI.getTickCount().ToString() + "ForceFlight" + param.nFlight + ". height" + param.fHeight + " upAcc" + param.nGoUpAcceleration.ToString() + " downAcc" + param.nDropAcceleration.ToString());
        }


        if (bForceMoving && bForceFlighting == false)
        {
            Trace.Log("在进行其他强制移动，不浮空");
            return;
        }

        if (param.nFlight==0)
        {
            //此参数为0时，停止浮空，开始下落
            uint tick = GameLogicAPI.getTickCount();
            if (bForceFlighting)
            {
                if (m_fFlightVel > 0.0f)
                {
                    m_fFlightVel = 0.0f;
                }
                if (m_uResidenceStopTick > tick)
                {
                    m_uResidenceStopTick = tick;

                }
            }
            return;
        }

        if (bForceFlighting == true)
        {
            //已经在浮空状态，第二次浮空
            if ((m_fFlightVel > 0 && param.fHeight <= m_fFlightHeight) //在上升状态，最终升空高度比fHeight要求的高
                || (m_pFSM.transform.position.y - m_flightEndPosition.y > param.fHeight)) //在下降状态，但已经比fHeight要求的高
            {
                //不会比目前状态升的更高，不需要重新上升，所以忽略
                return;
            }
            else
            {
                upAcc = param.nGoUpAcceleration;
                downAcc = param.nDropAcceleration;
                float acc = m_pFSM.currentGravity + upAcc;
                float newHeight = param.fHeight - (m_pFSM.transform.position.y - m_flightEndPosition.y);
                m_fFlightV0 = Mathf.Sqrt(2.0f * newHeight * acc);
                m_fFlightVel = m_fFlightV0;
                m_uResidenceStopTick = 0;
                m_uResidenceTick = (uint)param.nResidenceTime;
                m_fFlightHeight = param.fHeight;
                m_flightStartPosition = m_pFSM.transform.position;
                //m_flightEndPosition不变
                m_fFlightTime = 0.0f;
                isGounded = true;
            }
        }
        else
        {
            upAcc = param.nGoUpAcceleration;
            downAcc = param.nDropAcceleration;
            float acc = m_pFSM.currentGravity + upAcc;
            m_fFlightV0 = Mathf.Sqrt(2.0f * param.fHeight * acc);
            bForceFlighting = true;
            m_fFlightVel = m_fFlightV0;
            m_uResidenceStopTick = 0;
            m_uResidenceTick = (uint)param.nResidenceTime;
            m_fFlightHeight = param.fHeight;
            m_flightStartPosition = m_pFSM.transform.position;
            m_flightEndPosition = m_pFSM.transform.position;
            m_fFlightTime = 0.0f;
            isGounded = true;
        }


        m_nForceMoveStartTick = GameLogicAPI.getTickCount();
        m_nForceMoveEndTick = 0;
        m_ForceMovePos = m_pFSM.transform.position;
        m_fForceMoveSpeed = Vector3.zero;
        m_fForceStartPos = m_pFSM.transform.position;

        //冲锋完成后向逻辑层回报一次同步，请根据需求取舍
        //syncVelocity = Vector3.zero;
        //syncDetalTime = 0;
        //syncPosition = m_flightStartPosition + new Vector3(0, m_fFlightHeight, 0);
        //m_CurStateHandler.Send_Sync_Position();

    }

    bool isGounded = true;
    private void forceFlight()
    {
        uint tick = GameLogicAPI.getTickCount();
        if (m_uResidenceStopTick > 0 && m_uResidenceStopTick > tick)
        {
            return;
        }
        m_fFlightTime += Time.deltaTime;

        float newVel;
        float hight;
        if (m_fFlightVel > 0.0f)
        {
            newVel = m_fFlightV0 - (m_pFSM.currentGravity + upAcc) * m_fFlightTime;
            hight = m_fFlightV0 * m_fFlightTime - (m_pFSM.currentGravity + upAcc) * m_fFlightTime * m_fFlightTime / 2.0f;
            if (newVel <= 0.0f)
            {
                //上升结束，下降计时清0，开始浮空计时
                m_uResidenceStopTick = tick + m_uResidenceTick;
                m_fFlightDownTime = 0.0f;
                
                {
                    //如果没触地，则要一直掉落到地面
                    Vector3 pos = m_pFSM.transform.position + new Vector3(0, 0.1f, 0);
                    int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
                    RaycastHit hitInfo;
                    Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
                    Physics.Raycast(PointToGroundRay, out hitInfo, 100, LayerMaskOnGround);
                    if (Mathf.Abs(hitInfo.point.y - pos.y) > m_pFSM.controller.height + 0.6f)
                    {
                        isGounded = false;
                        m_pFSM.groundNormal = Vector3.zero;
                    }
                }

            }
            CollisionFlags collision = CollisionFlags.None;
            //if (m_fForceMoveSpeed.sqrMagnitude < 0.0001f)
            //{
            //    Vector3 newOffset = new Vector3(m_flightStartPosition.x, m_flightStartPosition.y + hight, m_flightStartPosition.z);
            //    newOffset = newOffset - m_pFSM.transform.position;
            //    collision=m_pFSM.controller.Move(newOffset); 2017-02-15 击退和击飞叠加时，m_flightEndPosition不是击退后的位置，所以不能这么移动
            //}
            //else
            {
                Vector3 newOffset = new Vector3(m_pFSM.transform.position.x, m_flightStartPosition.y + hight, m_pFSM.transform.position.z);
                newOffset = newOffset - m_pFSM.transform.position;
                collision=m_pFSM.controller.Move(newOffset);
            }
            if (0!=(collision & CollisionFlags.Above))
            {
                //touch ceiling
                m_uResidenceStopTick = tick + m_uResidenceTick;
                m_fFlightDownTime = 0.0f;
                newVel = 0.0f;
            }
        }
        else
        {
            m_fFlightDownTime += Time.deltaTime;
            newVel = 0 - (m_pFSM.currentGravity + downAcc) * m_fFlightDownTime;
            hight = m_fFlightHeight - (m_pFSM.currentGravity + downAcc) * m_fFlightDownTime * m_fFlightDownTime / 2.0f;
            CollisionFlags collision = CollisionFlags.None;
            //if (m_fForceMoveSpeed.sqrMagnitude < 0.0001f)
            //{
            //    Vector3 newOffset = new Vector3(m_flightStartPosition.x, m_flightEndPosition.y + hight, m_flightStartPosition.z);
            //    newOffset = newOffset - m_pFSM.transform.position;
            //    collision = m_pFSM.controller.Move(newOffset); 2017-02-15 击退和击飞叠加时，m_flightEndPosition不是击退后的位置，所以不能这么移动
            //}
            //else
            {
                Vector3 newOffset = new Vector3(m_pFSM.transform.position.x, m_flightEndPosition.y + hight, m_pFSM.transform.position.z);
                newOffset = newOffset - m_pFSM.transform.position;
                collision = m_pFSM.controller.Move(newOffset);
            }
            if (0 != (collision & CollisionFlags.Below))
            {
                //touch floor
                m_flightEndPosition.y = m_pFSM.transform.position.y;
            }
        }

        m_fFlightVel = newVel;
        bool isEnd = false;
        if (!isGounded)
        {
            if (m_pFSM.groundNormal.y > 0.01f)
            {
                isGounded = true;
                isEnd = true;
            }
        }
        else
        {
            if (m_pFSM.transform.position.y <= m_flightEndPosition.y)
            {
                isEnd=true;
            }
        }

        if (m_fFlightVel < 0.0f && isEnd) //已经落到底了
        {
            bForceFlighting = false;
            //if (!bForceMoving) //如果还有强制位移的话，还要继续强制位移
            //{
            //    Vector3 newOffset = m_flightEndPosition;
            //    newOffset = newOffset - m_pFSM.transform.position;
            //    m_pFSM.controller.Move(newOffset); 2017-02-15 击退和击飞叠加时，m_flightEndPosition不是击退后的位置，所以不能这么移动
            //}

            if (m_pFSM.isHero)
            {
                float fallHeight = m_flightStartPosition.y - m_pFSM.transform.position.y;
                if (fallHeight > 5.0f) //着陆时落差大于5米，则上报逻辑层
                {
                    cmd_creature_act_landspeed data = new cmd_creature_act_landspeed();

                    //data.fValue = landingSpeed;
                    data.fValue = fallHeight;

                    IntPtrHelper helper = new IntPtrHelper();
                    int len = Marshal.SizeOf(data);
                    IntPtr ptr = helper.toPtr<cmd_creature_act_landspeed>(ref data);
                    int eventID = (int)EntityLogicDef.ENTITY_CMD_LANDING_SPEED;
                    GameLogicAPI.onEntityEvent(m_pFSM.entityID, eventID, len, "", ptr, len);

                    if (m_pFSM.showLog)
                    {
                        Trace.Log("Big Landing! fallHeight=" + fallHeight.ToString());
                    }
                }
            }            

            //冲锋完成后向逻辑层回报一次同步，请根据需求取舍
            //syncVelocity = Vector3.zero;
            //syncDetalTime = 0;
            //syncPosition = transform.position;
            //m_CurStateHandler.Send_Sync_Position();
        }
    }

    public bool isAutoMoving = false;
    public Vector3 autoMoveTarget = Vector3.zero;
    private Vector3[] autoMovePathPoint;
    public Vector3[] AutoMovePathPoint
    {
        get { return autoMovePathPoint; }
    }
    private int autoMovePathPointIndex = 0;
    private Vector3 lastTurnPoint = Vector3.zero;
    private Vector3 lastAutoMovePoint = Vector3.zero; //记录上一帧的位置，用以计算是否跑过头，是否需要转向下一个路点
    private bool m_autoMoveNavModeFollow=false; //自动寻路启用了转镜头跟随
    public void AutoMoveTo(Vector3 targetPos, bool isFloating, bool rotateCamera = true)
    {
        //已经在控制走路的就不寻路了
        if (m_pFSM.inputMoveDirection.sqrMagnitude > 0.001f)
        {
            return;
        }

        if (m_pFSM.getEntityState() == EntityState.Dead)
        {
            return;
        }

        //强制位移时不自动寻路
        if (m_pFSM.animatorCtrl.bDoingAttackMove || bForceMoving || bForceFlighting)
        {
            return;
        }

        //根据目标点寻路，获得寻路结果
        cmd_map_findpath pathData = new cmd_map_findpath();
        cmd_map_findpath pathData2 = new cmd_map_findpath();
        pathData2.pathcnt = 0;
        float[] startPoint = new float[3];
        Vector3 startPos = m_pFSM.transform.position;
        if (isFloating)
        {
            startPos = ProjectToFloor(startPos);
        }
        startPoint[0] = startPos.x;
        startPoint[1] = startPos.y;
        startPoint[2] = startPos.z;
        pathData.start = startPoint;

        float[] endPoint = new float[3];
        endPoint[0] = targetPos.x;
        endPoint[1] = targetPos.y;
        endPoint[2] = targetPos.z;
        pathData.end = endPoint;

        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_map_findpath>(ref pathData);

        if (!GameLogicAPI.findPath(ptr))
        {
            return;
        }

        if (m_pFSM.showLog)
        {
            Trace.Log("AutoMove From" + m_pFSM.transform.position.ToString() + " To " + targetPos.ToString());
        }

        pathData = IntPtrHelper.toData<cmd_map_findpath>(ptr);
        if (pathData.pathcnt > 0)
        {
     
            if (pathData.path.Length < (pathData.pathcnt - 1) * 3 ||
                pathData.path.Length < (pathData.pathcnt - 1) * 3 + 1 ||
                pathData.path.Length < (pathData.pathcnt - 1) * 3 + 2)
            {
                Debug.LogError("寻路长度不合法");
                return;
            }

            Vector3 lastPoint = new Vector3(pathData.path[(pathData.pathcnt - 1) * 3], pathData.path[(pathData.pathcnt - 1) * 3 + 1], pathData.path[(pathData.pathcnt - 1) * 3 + 2]);

            int pointLen = pathData.pathcnt;
            if(isFloating)
            {
                pointLen++;
            }
            //如果到不到终点，强行寻路多一遍。。。
            else if ((targetPos - lastPoint).sqrMagnitude > 0.1f)
            {
                startPos = ProjectToFloor(lastPoint + (targetPos - lastPoint).normalized * 2.0f);
                startPoint[0] = startPos.x;
                startPoint[1] = startPos.y;
                startPoint[2] = startPos.z;
                pathData2.start = startPoint;
                pathData2.end = endPoint;

                IntPtrHelper helper2 = new IntPtrHelper();
                IntPtr ptr2 = helper.toPtr<cmd_map_findpath>(ref pathData2);
                if (GameLogicAPI.findPath(ptr2))
                {
                    pathData2 = IntPtrHelper.toData<cmd_map_findpath>(ptr);

                    if (pathData2.path.Length < (pathData2.pathcnt - 1) * 3 ||
                        pathData2.path.Length < (pathData2.pathcnt - 1) * 3 + 1 ||
                        pathData2.path.Length < (pathData2.pathcnt - 1) * 3 + 2)
                    {
                        Debug.LogError("寻路长度不合法");
                        return;
                    }

                    pointLen += pathData2.pathcnt;
                }

            }
            autoMovePathPoint = new Vector3[pointLen];
            for (int i = 0; i < pathData.pathcnt; i++)
            {
                autoMovePathPoint[i] = new Vector3(pathData.path[i * 3], pathData.path[i * 3 + 1], pathData.path[i * 3 + 2]);
                if (m_pFSM.showLog)
                {
                    Trace.Log("Path Point" + autoMovePathPoint[i].ToString());
                }
            }
            for (int i = 0; i < pathData2.pathcnt; i++)
            {
                autoMovePathPoint[i + pathData.pathcnt] = new Vector3(pathData2.path[i * 3], pathData2.path[i * 3 + 1], pathData2.path[i * 3 + 2]);
                if (m_pFSM.showLog)
                {
                    Trace.Log("Path Point" + autoMovePathPoint[i + pathData.pathcnt].ToString());
                }
            }
            if (isFloating)
                autoMovePathPoint[pointLen - 1] = targetPos;

            autoMovePathPointIndex = 0;
            lastTurnPoint = m_pFSM.transform.position;
            lastAutoMovePoint = m_pFSM.transform.position;
            isAutoMoving = true;
            autoMoveTarget = targetPos;

            Vector3 deta = autoMovePathPoint[0] - m_pFSM.transform.position;
            if (deta.sqrMagnitude < 0.25f) //太近了就不要转向了。因为在跑动中连续点寻路有时候会后退一个路点
            {
                autoMovePathPoint[0] = m_pFSM.transform.position;
            }

            //if (showLog)
            //{
            //    Trace.Log("Tick=" + GameLogicAPI.getTickCount() + " autoMove from " + transform.position + " to " + autoMovePathPoint[0]);
            //}
        }

        if (rotateCamera && !m_autoMoveNavModeFollow)
        {
            Vector3 targetDir = targetPos - m_pFSM.transform.position;
            targetDir.Normalize();
            Vector3 cameraProjDir = Vector3.Cross(Initialize.mainCam.transform.right.normalized, m_pFSM.transform.up.normalized); //摄像机方向投影到人物平面上
            //需要在确定目标后，进行一个判断，如果位置方向与镜头方向的夹角小于45度，就不会进行转动镜头的操作，而是直接在现有镜头下寻路并到点进行攻击
            if (Vector3.Dot(targetDir, cameraProjDir) < 0.707)
            {
                SoldierCamera.MainInstance<SoldierCamera>().enterNavMode(CameraNavMode.Follow, null,Vector3.zero);
                m_autoMoveNavModeFollow = true;
            }
        }

        Vector3 distance = targetPos - m_pFSM.transform.position;
        if (distance.sqrMagnitude > 225.0f) //小于15的头顶不提示
        {
            enableAutoMoveEffect(true);
        }
    }
    public void StopAutoMove(bool userStop = true)
    {
        if (!isAutoMoving)
        {
            return;
        }


        isAutoMoving = false;

        if (m_autoMoveNavModeFollow)
        {
            SoldierCamera.MainInstance<SoldierCamera>().exitNavMode(CameraNavMode.Follow);
            m_autoMoveNavModeFollow = false;
        }

        if (m_pFSM.showLog)
        {
            Trace.Log("StopAutoMove");
        }

        if (userStop)
        {
            cmd_creature_operation_type type = new cmd_creature_operation_type();
            type.nType = 0;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_creature_operation_type>(ref type);
            int len = Marshal.SizeOf(type);
            GameLogicAPI.onEntityEvent(m_pFSM.entityID, (int)EntityLogicDef.ENTITY_CMD_OPERATION_TYPE, len, "", ptr, len);
        }
        enableAutoMoveEffect(false);
    }

    private void updateAutoMove()
    {
        if (!isAutoMoving || !m_pFSM.soliderController)
        {
            return;
        }

        //强制位移时不自动寻路
        if (m_pFSM.animatorCtrl.bDoingAttackMove || bForceMoving || bForceFlighting)
        {
            StopAutoMove();
            return;
        }

        if (m_pFSM.soliderController)
        {
            if (m_pFSM.inputMoveDirection.sqrMagnitude > 0.001f)
            {
                StopAutoMove();
                return;
            }

            if (!m_pFSM.soliderController.canMove())
            {
                StopAutoMove();
                return;
            }

        }

        if ((autoMoveTarget - m_pFSM.transform.position).sqrMagnitude < 0.25f) //到达目标点
        {
            StopAutoMove();
            return;
        }


        Vector3 deta = autoMovePathPoint[autoMovePathPointIndex] - m_pFSM.transform.position;
        deta.y = 0; //目前只考虑平面
        Vector3 detaDir = deta.normalized;
        Vector3 faceDeta = m_pFSM.transform.position - lastTurnPoint;
        faceDeta.y = 0;//目前只考虑平面
        Vector3 faceDir = faceDeta.normalized;
        Vector3 step = m_pFSM.transform.position - lastAutoMovePoint;
        step.y = 0;

        if (deta.sqrMagnitude < 0.09f || ((deta.sqrMagnitude < step.sqrMagnitude) && (Vector3.Dot(detaDir, faceDir) < 0))) //离下一个目标点很近或者已经超越目标点
        {
            //if (showLog)
            //{
            //    Trace.Log("index="+autoMovePathPointIndex);
            //    Trace.Log("autoMovePathPoint=" + autoMovePathPoint[autoMovePathPointIndex]);
            //    Trace.Log("position=" + transform.position);
            //    Trace.Log("deta=" + deta);
            //    Trace.Log("faceDeta=" + faceDeta);
            //    Trace.Log("detaDir=" + detaDir);
            //    Trace.Log("faceDir=" + faceDir);
            //    Trace.Log("deta.magnitude=" + deta.magnitude);
            //    Trace.Log("faceDeta.magnitude=" + faceDeta.magnitude);
            //    Trace.Log("Vector3.Dot(detaDir, faceDir)=" + Vector3.Dot(detaDir, faceDir));

            //}


            autoMovePathPointIndex++;

            lastTurnPoint = m_pFSM.transform.position;

            if (autoMovePathPointIndex >= autoMovePathPoint.Length) //走完所有路点
            {
                StopAutoMove();
                return;
            }
            deta = autoMovePathPoint[autoMovePathPointIndex] - m_pFSM.transform.position;
            detaDir = deta.normalized;

            if (deta.sqrMagnitude < 0.25f) //太近了就不要转向了。因为在跑动中连续点寻路有时候会后退一个路点
            {
                autoMovePathPoint[0] = m_pFSM.transform.position;
            }

            //if (showLog)
            //{
            //    Trace.Log("Tick=" + GameLogicAPI.getTickCount() + " autoMove from " + transform.position + " to " + autoMovePathPoint[autoMovePathPointIndex]);
            //}
        }
        m_pFSM.autoMoveDirection = detaDir;
        //float angleDot = Vector3.Dot(autoMoveDirection, transform.forward);
        //if (angleDot < 0.99f)
        //{

        if (m_pFSM.soliderController.canRotate() && !m_pFSM.soliderController.isAttackMode())
        {
            m_pFSM.FaceTo(m_pFSM.transform.position + m_pFSM.autoMoveDirection * 10.0f);
        }

        //}

        lastAutoMovePoint = m_pFSM.transform.position;

    }


    //强制位移部分
    public bool bForceMoving = false;
    private uint m_nForceMoveStartTick = 0;
    private uint m_nForceMoveEndTick = 0;
    private Vector3 m_ForceMovePos = Vector3.zero;
    private Vector3 m_fForceMoveSpeed = Vector3.zero;
    private Vector3 m_fForceStartPos = Vector3.zero;
    private Transform m_tForceMoveTarget = null;
    private bool m_bForceLockTarget = false;
    private float m_fForceEndDistance = 0.0f; //当距离终点位置小于此数时，认为强制位移已经到终点
    private bool m_bDisableController = false; //可穿越移动的时候，关掉物理胶囊体
    private bool m_bIgnoreSoliderController = false; //true是表示是被动的强制位移，无视SoliderController的StopMove之类的定身
    public void ForceMove(cmd_force_move param)
    {
        Vector3 targetPos = new Vector3(param.fPosition_x, param.fPosition_y, param.fPosition_z);
        int moveTick = (int)param.fTime; //(int)(param.fTime * 1000);
        float fHeight = param.fHeight;
        m_tForceMoveTarget = null;
        m_bForceLockTarget = (param.nLockTarget == 1);
        m_fForceEndDistance = Mathf.Abs(param.fLocationDistance);
        if (param.uidTarget > 0)
        {
            U3D_Render.EntityView evDst = EntityFactory.getEntityViewByID(Convert.ToInt32(param.uidTarget));
            if (evDst != null && evDst.gameObject)
            {
                m_tForceMoveTarget = evDst.gameObject.transform;
            }
        }


        if (m_pFSM.bNotMove || moveTick <= 0)
        {
            return;
        }

        m_bIgnoreSoliderController = (param.nPassive!=0);  // 被动位移标识(0主动位移,不为0被动位移)

        if (m_pFSM.showLog)
        {
            Trace.Log(GameLogicAPI.getTickCount() + m_pFSM.gameObject.name + m_pFSM.transform.position + "ForceMove To" + targetPos.ToString() + "Tick=" + moveTick.ToString() + " Height=" + fHeight.ToString());
        }
        bForceMoving = true;
        if (param.nMoveType == MOVE_TYPE.MOVE_CHARGE && param.fHeight > 1.0f)
        {
            param.fHeight = 0.0f;
            fHeight = 0.0f;
            if (!m_bDisableController)
            {
                m_bDisableController = true;
                Debug.Log("m_bDisableController = true;");
                if (m_pFSM.getEntityState() != EntityState.Dead)                  
                    m_pFSM.enableCollider(false);
            }
        }
        else
        {
            m_bDisableController = false;
        }
        
        m_nForceMoveStartTick = GameLogicAPI.getTickCount();
        m_fForceStartPos = m_pFSM.transform.position;
        if (param.nLockTarget == 1 && m_tForceMoveTarget)
        {
            targetPos = m_tForceMoveTarget.position;
        }

        if (param.nLockTarget == 1 && m_tForceMoveTarget)
        {
            m_ForceMovePos = m_tForceMoveTarget.position;
            Vector3 moveDis = m_ForceMovePos - m_pFSM.transform.position;
            m_fForceMoveSpeed = moveDis.normalized * param.fSpeed;
            m_nForceMoveEndTick = m_nForceMoveStartTick + (uint)param.nLockTime;
        }
        else
        {
            m_ForceMovePos = targetPos;
            Vector3 moveDis = m_ForceMovePos - m_pFSM.transform.position;
            m_fForceMoveSpeed = moveDis / ((float)moveTick / 1000.0f);
            if (moveTick > param.fMaxTime)//(moveTick > param.fMaxTime*1000.0f)
            {
                moveTick = (int)param.fMaxTime;//(int)(param.fMaxTime * 1000.0f);
            }
            m_nForceMoveEndTick = m_nForceMoveStartTick + (uint)moveTick;
        }

        //Debug.Log(transform.name+"force targetid=" + param.uidTarget + "target=" + m_tForceMoveTarget + "nLockTarget=" + m_bForceLockTarget + "speed=" + param.fSpeed);
        //Debug.Log("m_ForceMovePos=" + m_ForceMovePos + "m_fForceMvoeSpeed=" + m_fForceMoveSpeed + "lockTime=" + param.nLockTime);


        if (fHeight > 0.01f && moveTick > 0)
        {
            //moveDis.y = 0;
            float flyTime = moveTick / 1000.0f / 2.0f;
            float g = 2.0f * fHeight / (flyTime * flyTime);
            m_fFlightV0 = Mathf.Sqrt(2.0f * fHeight * g);
            upAcc = g - m_pFSM.currentGravity;
            downAcc = g - m_pFSM.currentGravity;
            m_fFlightVel = m_fFlightV0;
            m_uResidenceStopTick = 0;
            m_uResidenceTick = 0;
            m_fFlightHeight = fHeight;
            m_flightStartPosition = m_pFSM.transform.position;

            if (bForceFlighting == false)
            {
                m_flightEndPosition = m_pFSM.transform.position;
            }
            //if (showLog)
            //{
            //    Debug.Log(transform.name + " Force Move m_flightEndPosition " + m_flightEndPosition);
            //}

            m_fFlightTime = 0.0f;
            bForceFlighting = true;

            //冲锋完成后向逻辑层回报一次同步，请根据需求取舍
            //syncVelocity = Vector3.zero;
            //syncDetalTime = 0;
            //syncPosition = new Vector3((m_fForceStartPos.x + m_ForceMovePos.x) / 2.0f, m_fForceStartPos.y + fHeight, (m_fForceStartPos.z + m_ForceMovePos.z) / 2.0f);
            //m_CurStateHandler.Send_Sync_Position();
        }
        else
        {
            m_fFlightHeight = 0.0f;
        }
    }

    public void StopForceMove()
    {
        if (m_bDisableController || bForceMoving)   //如果还是在强制位移过程，都重新设置一次
        {
            if (m_pFSM.getEntityState() != EntityState.Dead)
            {
                m_pFSM.enableCollider(true);
            }
            m_bDisableController = false;
        }
        m_ForceMovePos = m_pFSM.transform.position;
        m_nForceMoveEndTick = GameLogicAPI.getTickCount();
        if (m_pFSM.showLog)
        {
            Trace.Log("StopForceMove" + m_nForceMoveEndTick);
        }
    }

    public Vector3 getForceMovePos()
    {
        return m_ForceMovePos;
    }

    public void update()
    {
        if (bForceMoving || bForceFlighting)
        {
            forceMoveToTarget();
        }
        else if (isAutoMoving)
        {
            updateAutoMove(); //自动移动寻路
        }
    }

    public bool isCharge()
    {
        return (bForceMoving || bForceFlighting);
    }

    public bool isForceFlighting()
    {
        return bForceFlighting;
    }

    public void Transport(cmd_creature_transport pos)
    {
        Vector3 posit = Vector3.zero;
        posit.x = pos.fPosition_x;
        posit.y = pos.fPosition_y;
        posit.z = pos.fPosition_z;
        Vector3 ang = Vector3.zero; //暂时只转Y轴，另外两个轴角度怪怪的
        //ang.x = pos.fRotation_x;

        if (float.IsNaN(pos.fRotation_y) || Mathf.Abs(pos.fRotation_y) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.Log(m_pFSM.name + "Transport fRotation_y invalid" + pos.fRotation_y);
        }
        else
        {
            ang.y = pos.fRotation_y;
        }


        //ang.z = pos.fRotation_z;

        Transport(posit, pos.bUseAngle == 1, ang, pos.nReason);
    }

    public void Transport(Vector3 posit,bool bUseAngle,Vector3 ang,int reanson)
    {
        if (m_pFSM.showLog || m_pFSM.isHero)
        {
            Trace.Log(m_pFSM.name + m_pFSM.transform.position.ToString() + " Transport to" + posit.ToString());
        }

        m_pFSM.creature.SetPosition(posit);
        // 同步信息来一发
        m_pFSM.syncVelocity = Vector3.zero;
        m_pFSM.syncDetalTime = 0;
        m_pFSM.syncPosition = posit;
        m_pFSM.lastHitPoint = posit;
        m_pFSM.transform.SetPosition(posit);
        if (bUseAngle)
        {
           
            //SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle = ang;
            //if (soliderController != null)
            //{
            //    soliderController.desireAngle = ang;
            //}

            if (float.IsNaN(ang.x) || float.IsNaN(ang.y) || float.IsNaN(ang.z) || Mathf.Abs(ang.x) > SceneManager.MAX_SCENE_SIZE || Mathf.Abs(ang.y) > SceneManager.MAX_SCENE_SIZE || Mathf.Abs(ang.z) > SceneManager.MAX_SCENE_SIZE)
            {
                Debug.Log(m_pFSM.name + "MoveCtrl Transport angle invalid" + ang);
            }
            else
            {
                m_pFSM.creature.eulerAngles = ang;
                m_pFSM.syncAngle = ang;
                if (m_pFSM.soliderController != null)
                {
                    m_pFSM.soliderController.desireAngle = ang;
                }
            }

            if (m_pFSM.showLog)
            {
                Trace.Log("Angle" + ang.ToString());
            }
        }

        if ((int)TRANSPORT_REASON.TRANSPORT_REASON_BACKCITY == reanson 
            || (int)TRANSPORT_REASON.TRANSPORT_REASON_REBORN == reanson
            || (int)TRANSPORT_REASON.TRANSPORT_REASON_APPEAR == reanson)
        {
            if (m_pFSM.isHero)
            {
                if (m_pFSM.soliderController != null)
                {
                    Vector3 newAng = SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle;
                    newAng.y = m_pFSM.soliderController.desireAngle.y;
                    SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle = newAng;
                }
                else
                {
                    Vector3 newAng = SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle;
                    newAng.y = ang.y;
                    SoldierCamera.MainInstance<SoldierCamera>().userDesireAngle = newAng;
                }
            }
        }

        StopForceMove();
        bForceMoving = false;
        bForceFlighting = false;
        //ChangeState((int)EntityState.Standing, IntPtr.Zero);

    }

    //自动寻路特效
    private void enableAutoMoveEffect(bool enable)
    {
        //if (!enable)
        //{
        //    if (autoMoveEffect)
        //    {
        //        autoMoveEffect.SetActive(false);
        //    }
        //    m_showAutoMoveEffect = false;
        //    return;
        //}
        //else if (autoMoveEffect)
        //{
        //    autoMoveEffect.SetActive(true);
        //    m_showAutoMoveEffect = true;
        //    return;
        //}
        //GFxGameHud hud = SFGFxMovieManager.GetWindow<GFxGameHud>();
        //if (SFGFxMovieManager.CheckTheMovieCanUse(hud))
        //{
        //    hud.AutoMoveEffectActive = enable;
        //    m_showAutoMoveEffect = enable;
        //    return;
        //}
        //else
        //    m_showAutoMoveEffect = false;

        //autoMoveEffect is null. Create it here
        //UnityEngine.Object obj = Resources.Load("Effect/selectEffect/autoMoveEffect");
        //autoMoveEffect = ResNode.InstantiateRes(obj) as GameObject;
        //if (!autoMoveEffect) return;
        //m_showAutoMoveEffect = true;
        if (enable)
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW, null);
        }
        else
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE, null);
        }


    }

    private Vector3 ProjectToFloor(Vector3 posBeforeProject)
    {
        Vector3 posAfterProject = posBeforeProject;
        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        LayerMaskOnGround = LayerMaskOnGround | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        
        RaycastHit hitInfo;

        Ray downRay = new Ray(posAfterProject - new Vector3(0, 0.1f, 0), new Vector3(0, -1, 0));
        bool hitGround = Physics.Raycast(downRay, out hitInfo, 300, LayerMaskOnGround);
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
}
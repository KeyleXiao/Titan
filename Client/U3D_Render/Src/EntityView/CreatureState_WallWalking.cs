using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using GameLogic;

public class CreatureState_WallWalking : ICreatureState
{
    
    // 状态机指针
    CreatureStateMachine m_pFSM;    

    //人物的transform
    Transform tr;

    private int hashid_isWallWalking;
    private int hashid_isUpWall;
    private int hashid_isDownWall;

    private int m_lastSyncTick = 0;

    // 构造
    public CreatureState_WallWalking(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        hashid_isWallWalking = Animator.StringToHash("isWallWalking");
        hashid_isUpWall = Animator.StringToHash("isUpWall");
        hashid_isDownWall = Animator.StringToHash("isDownWall");
    }

    // 取得ID
    public int GetID() { return (int)EntityState.WallWalking; }

    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        m_lastSyncTick = 0;
        tr = m_pFSM.transform;
        cmd_Entity_Change_State_WallWalking param = IntPtrHelper.toData<cmd_Entity_Change_State_WallWalking>(lParam);
        m_pFSM.enableCollider(false);
        m_pFSM.transform.eulerAngles = new Vector3 (m_pFSM.transform.eulerAngles.x,param.faceDirection_y,m_pFSM.transform.eulerAngles.z);

        Vector3 faceDir = m_pFSM.transform.forward;
        faceDir.Normalize();

        float posY = m_pFSM.transform.position.y;

        Vector3 wallCornerAPos = new Vector3(param.wallCornerAPos_x, posY, param.wallCornerAPos_z);
        Vector3 wallCornerBPos = new Vector3(param.wallCornerBPos_x, posY, param.wallCornerBPos_z);
        Vector3 wallCornerCPos = new Vector3(param.wallCornerCPos_x, posY, param.wallCornerCPos_z);
        Vector3 wallCornerDPos = new Vector3(param.wallCornerDPos_x, posY, param.wallCornerDPos_z);


        //Debug.Log("wall pos=" + wallCornerAPos.ToString() + wallCornerBPos.ToString() + wallCornerCPos.ToString() + wallCornerDPos.ToString());

        //区域点输出规则：
        //飞檐走壁为一个cube，所以区域点为4个顶点的矩形
        //取最短的边开始，顺时针或逆时针方向输出
        //因此，AB CD为短边，BC为长边 
        Vector3 wallStartPos = (wallCornerAPos + wallCornerBPos)/2.0f;
        Vector3 wallEndPos = (wallCornerCPos + wallCornerDPos) / 2.0f;
        float wallWidth = (wallCornerBPos - wallCornerAPos).magnitude;
        float wallLength = (wallCornerCPos - wallCornerBPos).magnitude;
        return startWalkPos(wallStartPos, wallEndPos, wallWidth, wallLength, param.walkHeight, faceDir);
    }


    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {     
        return true;
    }

    // 离开状态
    public void OnLeave()
    {
        MoveSyncFlag sendSync = MoveSyncFlag.TO_SERVER;

        int curTick = System.Environment.TickCount;
        if (m_pFSM)
        {
            m_pFSM.syncTick = curTick;
            Vector3 sendVelocity = Vector3.zero;
            m_pFSM.syncVelocity = sendVelocity;
            m_pFSM.syncPosition = m_pFSM.transform.position;
            m_pFSM.syncAngle = m_pFSM.transform.eulerAngles;
        }
        Send_Sync_Position(sendSync);



        bWallWalking = false;
        m_wallWalkingState = WallWalkingState.none;
        if (m_pFSM)
        {
            m_pFSM.animator.SetBool(hashid_isWallWalking, false);
            m_pFSM.animator.SetBool(hashid_isUpWall, false);
            m_pFSM.animator.SetBool(hashid_isDownWall, false);
            m_pFSM.enableCollider(true);
        }
    }


    // 更新状态逻辑
    public void Update()
    {
        updateWallWalk();
    }

    public bool bWallWalking = false;
    //public float fWallWalkSpeed = 7.0f;

    private Vector3 m_wallWalkStartPos = Vector3.zero;
    private Vector3 m_wallWalkEndPos = Vector3.zero;

    private float m_wallWalkSpeed = 0.0f;
    private float m_wallClimbSpeed = 0.0f;
    private Vector3 m_wallWalkTargetPos = Vector3.zero;
    //private uint m_climbWallTick = 1000;
    private float m_climbWallHight = 2.0f;
    private Vector3 m_wallTangentDir = Vector3.zero;


    private enum WallWalkingState
    {
        none = 0,
        gotoWall,
        climbWall,
        wallWalking,
        down,
    }
    private WallWalkingState m_wallWalkingState = WallWalkingState.none;

    public bool startWalkPos(Vector3 wallStartPos, Vector3 wallEndPos, float wallWidth, float walkLength, float walkHeight, Vector3 faceDirection)
    {
        if (!calWallWalkPos(wallStartPos, wallEndPos, wallWidth, walkLength, walkHeight, faceDirection))
        {
            return false;
        }
        bWallWalking = true;
        m_wallWalkingState = WallWalkingState.none;
        m_pFSM.animator.SetBool(hashid_isWallWalking, true);
        m_pFSM.animator.SetBool(hashid_isUpWall, false);
        m_pFSM.animator.SetBool(hashid_isDownWall, false);
        nextStep();
        updateStep();
        return true;
    }


    private bool calWallWalkPos(Vector3 wallStartPos, Vector3 wallEndPos, float wallWidth, float walkLength, float walkHeight, Vector3 faceDirection)
    {
        m_climbWallHight = walkHeight;
        Vector3 startPos = Vector3.zero;
        Vector3 heroPos = m_pFSM.transform.position;
        //爬墙点不能低于地面，简单判断就是不能低于英雄当前高度
        float minY = heroPos.y;
        if (wallStartPos.y < minY)
        {
            wallStartPos.y = minY;
        }
        if (wallEndPos.y < minY)
        {
            wallEndPos.y = minY;
        }

        CollisionDetection.DisPoint2Segment(heroPos, wallStartPos, wallEndPos, ref startPos);
        Vector3 wallDir = wallEndPos - wallStartPos;
        wallDir.Normalize();
        faceDirection.Normalize();
        float dirDot = Vector3.Dot(wallDir, faceDirection);
        Vector3 endPos = wallEndPos;
        if (dirDot < 0) //墙壁方向与面向反向
        {
            endPos = wallStartPos;
        }
        Vector3 walkSeg = endPos - startPos;
        if (walkSeg.magnitude > walkLength)
        {
            endPos = walkSeg.normalized * walkLength + startPos;
        }
        else if (walkSeg.sqrMagnitude < 0.01f) //起点和终点重合了
        {
            return false;
        }

        //根据墙的厚度往外偏移起止点
        Vector3 fDir = walkSeg.normalized;
        Vector3 tDir = heroPos - startPos;
        tDir.Normalize();
        Vector3 upDir = Vector3.Cross(fDir, tDir);
        m_wallTangentDir = Vector3.Cross(upDir, fDir);
        m_wallTangentDir.Normalize();
        startPos = startPos + m_wallTangentDir * wallWidth * 0.5f;
        endPos = endPos + m_wallTangentDir * wallWidth * 0.5f;

        m_wallWalkStartPos = startPos;
        m_wallWalkEndPos = endPos;

        return true;
    }

    private void updateWallWalk()
    {
        if (m_wallWalkingState == WallWalkingState.none)
        {
            return;
        }
        else if (m_wallWalkingState == WallWalkingState.down)
        {
            updateDownWalk();
        }
        else
        {
            updateStep();
        }
    }

    private void updateStep()
    {
        if (m_wallWalkSpeed < 0.01f)
        {
            stopWallWalk();
            return;
        }

        Vector3 walkStep = m_wallWalkTargetPos - m_pFSM.transform.position;
        float stepLength = m_wallWalkSpeed * Time.deltaTime;
        if (walkStep.magnitude <= stepLength)
        {
            //m_pFSM.controller.Move(walkStep);
            m_pFSM.transform.position += walkStep;
            nextStep();            
        }
        else
        {
            walkStep.Normalize();
            walkStep = walkStep * stepLength;
            //m_pFSM.controller.Move(walkStep);
            m_pFSM.transform.position += walkStep;
        }
        

        int curTick = System.Environment.TickCount;
        if (curTick - m_lastSyncTick > 200)
        {
            m_lastSyncTick = curTick;
            MoveSyncFlag sendSync = MoveSyncFlag.TO_SERVER;
            m_pFSM.syncTick = curTick;
            Vector3 sendVelocity = walkStep.normalized * m_wallWalkSpeed;
            m_pFSM.syncVelocity = sendVelocity;
            m_pFSM.syncPosition = m_pFSM.transform.position;
            m_pFSM.syncAngle = m_pFSM.transform.eulerAngles;
            Send_Sync_Position(sendSync);

        }



    }

    private void nextStep()
    {
        if (m_wallWalkingState == WallWalkingState.none)
        {
            calGotoWallParam();
            m_wallWalkingState = WallWalkingState.gotoWall;
        }
        else if (m_wallWalkingState == WallWalkingState.gotoWall)
        {
            calClimbParam();
            m_wallWalkingState = WallWalkingState.climbWall;
            m_pFSM.animator.SetBool(hashid_isUpWall, true);
        }
        else if (m_wallWalkingState == WallWalkingState.climbWall)
        {
            m_pFSM.animator.SetBool(hashid_isUpWall, false);
            calWalkWallParam();
            m_wallWalkingState = WallWalkingState.wallWalking;
        }
        else if (m_wallWalkingState == WallWalkingState.wallWalking)
        {
            m_wallWalkingState = WallWalkingState.down;
            Vector3 angle = m_pFSM.transform.eulerAngles;
            angle.z = 0.0f;
            m_pFSM.transform.eulerAngles = angle;
            m_pFSM.animator.SetBool(hashid_isDownWall, true);
        }
        else
        {
            stopWallWalk();
        }
    }

    //以下为cal函数组，主要计算每个阶段的targetPos和WalkSpeed
    private void calGotoWallParam()
    {
        m_wallWalkTargetPos = m_wallWalkStartPos;
        m_wallWalkSpeed = m_pFSM.creaturePropety.wallWalkSpeed;
        m_pFSM.FaceTo(m_wallWalkTargetPos);

    }
    private void calClimbParam()
    {
        m_wallWalkTargetPos = m_pFSM.transform.position;
        m_wallWalkTargetPos.y += m_climbWallHight;
        //m_wallWalkSpeed = m_climbWallHight / (float)m_climbWallTick * 1000.0f;
        m_wallWalkSpeed = m_pFSM.creaturePropety.wallClimbSpeed;
        m_wallClimbSpeed = m_wallWalkSpeed;
        m_pFSM.groundNormal = Vector3.zero;
    }
    private void calWalkWallParam()
    {
        m_wallWalkTargetPos = m_wallWalkEndPos;
        m_wallWalkTargetPos.y = m_pFSM.transform.position.y;
        m_wallWalkSpeed = m_pFSM.creaturePropety.wallWalkSpeed;

        //向外倾斜45度
        m_pFSM.FaceTo(m_wallWalkTargetPos);
        Vector3 faceUp = Vector3.Cross(m_pFSM.transform.forward, m_wallTangentDir);
        Vector3 angle = m_pFSM.transform.eulerAngles;
        if (faceUp.y > 0.0f)
        {
            angle.z = -45.0f;
        }
        else
        {
            angle.z = 45.0f;
        }
        m_pFSM.transform.eulerAngles = angle;
    }

    private void updateDownWalk()
    {
        if (m_pFSM.groundNormal.y < 0.01f)
        {
            if (m_wallClimbSpeed < 0.01f)
            {
                m_wallClimbSpeed = 5.0f;
            }

            Vector3 walkStep = Vector3.zero;
            walkStep.y -= m_wallClimbSpeed * Time.deltaTime;
            m_pFSM.controller.Move(walkStep);
        }
        else
        {
            stopWallWalk();
        }
    }

    private void stopWallWalk(bool forceStop = false)
    {
        Vector3 angle = m_pFSM.transform.eulerAngles;
        angle.z = 0.0f;
        m_pFSM.transform.eulerAngles = angle;
        if (m_wallWalkingState < WallWalkingState.down && !forceStop)
        {
            m_wallWalkingState = WallWalkingState.down;
            m_pFSM.animator.SetBool(hashid_isDownWall, true);
        }
        else
        {
            m_pFSM.ChangeState((int)EntityState.Standing, IntPtr.Zero);

        }
    }

    //返回是否在地面
    public bool IsGrounded()
    {
        return false;
    }

    public bool isFloating()
    {
        return false;
    }

    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 发送一次位置同步
    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
        if (m_pFSM == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position m_pFSM == null");
            return;
        }

        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(m_pFSM.syncPosition.x) || float.IsNaN(m_pFSM.syncPosition.y) || float.IsNaN(m_pFSM.syncPosition.z) || Math.Abs(m_pFSM.syncPosition.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(m_pFSM.syncPosition.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(m_pFSM.syncPosition.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(m_pFSM.name + " standing Send_Sync_Position Position Error " + m_pFSM.syncPosition);
            return;
        }


        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)EntityState.WallWalking;
        data.fPosition_x = m_pFSM.syncPosition.x;
        data.fPosition_y = m_pFSM.syncPosition.y;
        data.fPosition_z = m_pFSM.syncPosition.z;

        // 直接用欧拉角
        data.fRotation_x = m_pFSM.syncAngle.x;
        data.fRotation_y = m_pFSM.syncAngle.y;
        data.fRotation_z = m_pFSM.syncAngle.z;

        data.fVelocity_x = m_pFSM.syncVelocity.x;
        data.fVelocity_y = m_pFSM.syncVelocity.y;
        data.fVelocity_z = m_pFSM.syncVelocity.z;
        data.nSyn2Server = (int)reportToSever;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);

        if (tr == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position tr == null");
            return;
        }

        if (tr.position == null)
        {
            Trace.LogError("CreatureState_WallWalking.cs Send_Sync_Position tr.SetPosition== null");
            return;
        }

        if (tr.rotation == null)
        {
            Trace.LogError("CreatureState_WallWalking.cs Send_Sync_Position tr.rotation == null");
            return;
        }

        if (tr.rotation.eulerAngles == null)
        {
            Trace.LogError("CreatureState_WallWalking.cs Send_Sync_Position tr.rotation.eulerAngles == null");
            return;
        }

        EntityBatchCommunicator.SendEntityPro(m_pFSM.entityID, tr.position, tr.rotation.eulerAngles);

        if (m_pFSM.isHero)
        {
            U3D_Render.EntityView ev = EntityFactory.getPlayerViewByID(m_pFSM.entityID);
            MapFinderManager.MoveObject(ref ev, tr.position);
        }        
    }

    private Vector3 m_Velocity;

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return m_Velocity;
    }
    public void setVelocity(Vector3 vel)
    {
        m_Velocity = vel;
    }

    public void OnTransport()
    {

    }

    public void OnForceMove()
    {
    }

    public void OnForceFlight()
    {
    }

    public void ChangeState(IntPtr ptrParam)
    {
    }
    public void ApplyAnimation()
    {

    }

}

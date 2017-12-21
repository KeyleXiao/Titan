using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using GameLogic;


public class CreatureState_Glide : ICreatureState
{

    // 状态机指针
    CreatureStateMachine m_pFSM;

    //人物的transform
    Transform tr;

    private Vector3 m_Velocity;         //基本的速度，未添加重力等因素，比较平滑
    private bool m_bIsLanding = false;  //是否正在着地
    private bool m_bIsJumping = false;
    private bool m_bIsGlideRuning = false;

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return m_Velocity;
    }
    public void setVelocity(Vector3 vel)
    {
        m_Velocity = vel;
    }

    private uint startUpTick = 0;
    private float oldGlideSpeed;
    private int hashid_isMoving;
    private int hashid_isLanding;
    private int hashid_Jumping;
    private int hashid_isGlide;
    private int hashid_FlyDown;

    public CreatureState_Glide(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        hashid_isMoving = Animator.StringToHash("isMoving");
        hashid_isLanding = Animator.StringToHash("isLanding");
        hashid_Jumping = Animator.StringToHash("Jumping");
        hashid_isGlide = Animator.StringToHash("isGlide");
        hashid_FlyDown = Animator.StringToHash("flyDown");
    }

    // 取得状态ID
    public int GetID() { return (int)EntityState.Glide; }

    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        tr = m_pFSM.transform;
        startUpTick = 0;
        if (m_pFSM.animator != null)
        {
            m_pFSM.animator.SetBool(hashid_isLanding, false);
        }
        m_bIsLanding = false;    //还没着陆
        m_bIsJumping = false;   //还没起跳
        m_bIsGlideRuning = true;//在地面加速

        //调整速度
        oldGlideSpeed = m_pFSM.maxGlideSpeed;
        m_pFSM.maxGlideSpeed += m_pFSM.glideSpeedGround;

        return true;
    }


    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        return false;
    }

    // 离开状态
    public void OnLeave()
    {
        m_bIsJumping = false;
        m_bIsLanding = false;
        m_bIsGlideRuning = false;
    }

    //同步用
    private int m_lastSyncTick = 0;
    private Vector3 m_lastSyncVel = Vector3.zero;
    private Vector3 m_lastSyncPos = Vector3.zero;

    float deltaTime = 0;

    // 更新状态逻辑
    public void Update()
    {
        deltaTime = Time.deltaTime;
        if (deltaTime < 0.001f)
        {
            Debug.LogError("deltaTime=" + deltaTime);
            return;
        }
        uint tick = GameLogicAPI.getTickCount();
        bool oldGrounded = isGrounded;
        isGrounded = IsGroundedTest();   //测试是否在空中
        float Vy = getVelocity().y;
        Vector3 inputVelocity = ApplyInputVelocityChange(getVelocity()); //根据键盘输入调整速度
        Vector3 velocity = Vector3.zero;
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)
        {
            velocity = inputVelocity;
        }
        else
        {
            velocity = m_pFSM.syncVelocity;
        }
        velocity.y = Vy;
        //处理在空中的情况
        if (isGrounded && !m_bIsGlideRuning)
        {
            //刚到地面，开始着陆的处理
            if (!m_bIsLanding)
            {
                //切换动作
                if (m_pFSM.animator)
                {
                    m_pFSM.animator.SetBool(hashid_isLanding, true);
                    m_pFSM.animator.SetBool(hashid_isGlide, false);
                    m_pFSM.animator.SetBool(hashid_FlyDown, false);
                }

                m_bIsLanding = true;    //正在着陆
            }
            else  //着陆后的处理
            {
                //结束本状态，切换到新状态
                velocity.y = Mathf.Min(0, velocity.y) - m_pFSM.currentGravity * deltaTime;
                setVelocity(velocity);
                m_pFSM.maxGlideSpeed = oldGlideSpeed;
                m_pFSM.isGlideState = false;
                m_pFSM.ChangeState((int)EntityState.Standing, IntPtr.Zero);
            }
        }
        else  //这里包含两个状态：起跳 -> 最高点 ->落地
        {
            if (velocity.y <= 0)
            {
                velocity.y = m_Velocity.y - m_pFSM.glideDownSpeedAcceleration * deltaTime;
            }
            else
            {
                velocity.y = m_Velocity.y - m_pFSM.currentGravity * deltaTime;
            }
            // Make sure we don't fall any faster than maxFallSpeed. This gives our character a terminal velocity.
            velocity.y = Mathf.Max(velocity.y, m_pFSM.SkinConfig == null ? -15 : -m_pFSM.SkinConfig.maxFallSpeed);
            setVelocity(velocity);
            //velocity.y = Mathf.Min(velocity.y, m_pFSM.SkinConfig == null ? 15 : m_pFSM.SkinConfig.maxFallSpeed);
        }
        if (m_bIsGlideRuning)
        {
            Vector3 jumpDir = Vector3.up;
            startUpTick = 0;
            velocity.y = 0;
            m_pFSM.animator.SetBool(hashid_isGlide, true);
            //添加y轴分量的初始速度，以后做自由落体运动
            //velocity += jumpDir * CalculateJumpVerticalSpeed(m_pFSM.glideUpHeight);// m_pFSM.glideUpHeight);
            m_bIsGlideRuning = false;
            m_bIsJumping = false;
            isGrounded = false;
            m_pFSM.groundNormal = Vector3.zero;
            setVelocity(velocity);
        }

        //同步？
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)
        {
            m_pFSM.syncVelocity = velocity;

            // 判断本地的模拟坐标和真实坐标的差别, 只有达到一定阀值才需要同步
            // 暂时在行走过程中200ms同步一次
            int curTick = System.Environment.TickCount;
            MoveSyncFlag sendSync = MoveSyncFlag.NOT_2_SERVER;
            if (curTick >= m_lastSyncTick + 5000) //5秒强制同步一次
            {
                sendSync = MoveSyncFlag.TO_SERVER;
            }
            else if ((m_lastSyncVel.sqrMagnitude > 0.0001f && m_pFSM.syncVelocity.sqrMagnitude < 0.0001f) || (m_lastSyncVel.sqrMagnitude < 0.0001f && m_pFSM.syncVelocity.sqrMagnitude > 0.0001f))//启动和站立强制同步一次
            {
                sendSync = MoveSyncFlag.TO_BROADCAST;
            }
            else //每200毫秒根据速度或者移动距离考虑是否同步一次
            {
                Vector3 velSub = m_lastSyncVel - m_pFSM.syncVelocity;
                Vector3 posSub = tr.position - m_lastSyncPos;
                if ((curTick >= m_lastSyncTick + 200) && (velSub.sqrMagnitude > 0.01f || posSub.sqrMagnitude > 1.0f))
                {
                    sendSync = MoveSyncFlag.TO_SERVER;
                }
            }

            if (sendSync != MoveSyncFlag.NOT_2_SERVER && (m_pFSM.isHero && !m_pFSM.bControlledBySever))
            {
                m_pFSM.syncVelocity.y = 0.0f; // 跳跃的同步包y方向速度都为0，由实际跳跃产生。                
                Send_Sync_Position(sendSync);
                m_lastSyncTick = curTick;
                m_lastSyncVel = m_pFSM.syncVelocity;
                m_lastSyncPos = tr.position;
            }
        }


        //移动characterController

        Vector3 currentMovementOffset = velocity * deltaTime;

        // Find out how much we need to push towards the ground to avoid loosing grouning
        // when walking down a step or over a sharp change in slope.

        //计算跨步的距离
        float pushDownOffset = Mathf.Max(m_pFSM.controller.stepOffset, (new Vector3(currentMovementOffset.x, 0, currentMovementOffset.z)).magnitude);
        if (float.IsNaN(m_pFSM.controller.stepOffset))
        {
            Debug.LogError(m_pFSM.name + " jumping controller.stepOffset error" + m_pFSM.controller.stepOffset);
            pushDownOffset = (new Vector3(currentMovementOffset.x, 0, currentMovementOffset.z)).magnitude;
        }

        //向下偏移，处理下楼梯？
        if (isGrounded)
            currentMovementOffset -= pushDownOffset * Vector3.up;

        //真正的移动
        m_pFSM.collisionFlags = m_pFSM.controller.Move(currentMovementOffset);

        //处理消息发送
        //....
    }

    //利用自由落体，通过高度反推初始速度
    float CalculateJumpVerticalSpeed(float targetJumpHeight)
    {
        // From the jump height and gravity we deduce the upwards speed 
        // for the character to reach at the apex.
        return Mathf.Sqrt(2.0f * targetJumpHeight * m_pFSM.creaturePropety.upGravity);
    }

    private Vector3 ApplyInputVelocityChange(Vector3 velocity)
    {
        if (!m_pFSM.canControl)
            m_pFSM.inputMoveDirection = Vector3.zero;

        // Find desired velocity
        Vector3 desiredVelocity;

        desiredVelocity = GetDesiredHorizontalVelocity();

        if (isGrounded)
        {
            desiredVelocity = AdjustGroundVelocityToNormal(desiredVelocity, m_pFSM.groundNormal);
        }
        //// Enforce max velocity change
        //float maxVelocityChange = m_pFSM.maxAcceleration * Time.deltaTime;
        //Vector3 velocityChangeVector = (desiredVelocity - velocity);
        //if (velocityChangeVector.sqrMagnitude > maxVelocityChange * maxVelocityChange)
        //{
        //    velocityChangeVector = velocityChangeVector.normalized * maxVelocityChange;
        //}
        //// If we're in the air and don't have control, don't apply any velocity change at all.
        //// If we're on the ground and don't have control we do apply it - it will correspond to friction.
        //if (grounded || m_pFSM.canControl)
        //    velocity += velocityChangeVector;
        velocity = desiredVelocity;


        if (isGrounded)
        {
            // When going uphill, the CharacterController will automatically move up by the needed amount.
            // Not moving it upwards manually prevent risk of lifting off from the ground.
            // When going downhill, DO move down manually, as gravity is not enough on steep hills.
            velocity.y = Mathf.Min(velocity.y, 0);
        }

        return velocity;
    }

    private Vector3 GetDesiredHorizontalVelocity()
    {
        // Find desired velocity
        Vector3 desiredLocalDirection = tr.InverseTransformDirection(m_pFSM.inputMoveDirection);
        float maxSpeed = MaxSpeedInDirection(desiredLocalDirection);
        return tr.TransformDirection(desiredLocalDirection * maxSpeed);
    }

    private Vector3 AdjustGroundVelocityToNormal(Vector3 hVelocity, Vector3 groundNormal)
    {
        Vector3 outputVec = hVelocity;
        if (groundNormal.sqrMagnitude > 0.01f)
        {
            Vector3 sideways = Vector3.Cross(Vector3.up, hVelocity);
            outputVec = Vector3.Cross(sideways, groundNormal).normalized * hVelocity.magnitude;
        }
        return outputVec;
    }

    float MaxSpeedInDirection(Vector3 desiredMovementDirection)
    {
        if (desiredMovementDirection == Vector3.zero || Mathf.Abs(m_pFSM.maxRunSidewaySpeed) < 0.0001f)
            return 0;
        else
        {
            float zAxisEllipseMultiplier = (desiredMovementDirection.z > 0 ? m_pFSM.maxForwardSpeed : m_pFSM.maxRunBackwardSpeed) / m_pFSM.maxRunSidewaySpeed;
            Vector3 temp = new Vector3(desiredMovementDirection.x, 0, desiredMovementDirection.z / zAxisEllipseMultiplier).normalized;
            float length = new Vector3(temp.x, 0, temp.z * zAxisEllipseMultiplier).magnitude * m_pFSM.maxRunSidewaySpeed;
            return length;
        }
    }

    //public void OnAnimatorIK(int layerIndex)
    //{
    //}

    //返回是否在地面
    private bool isGrounded;
    public bool IsGrounded()
    {
        return isGrounded;
    }

    private bool IsGroundedTest()
    {
        return (m_pFSM.groundNormal.y > 0.01f);
    }

    public bool isFloating()
    {
        return false;
    }
    private Vector3 syncDir;
    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)
        {
            return;
        }

        //Trace.Log("PosotionSync" + entityID.ToString()+ m_pFSM.syncPosition.ToString());
        //UpdateFunction();

        // 直接通过欧拉角来设置人物朝向
        tr.rotation = Quaternion.Euler(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        Vector3 inputV = new Vector3(sync.fVelocity_x, sync.fVelocity_y, sync.fVelocity_z);
        syncDir = inputV.normalized;
    }
    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
        // trace
        if (m_pFSM == null)
        {
            Trace.LogError("CreatureState_Jomp.cs Send_Sync_Position m_pFSM == null");
            return;
        }

        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(tr.position.x) || float.IsNaN(tr.position.y) || float.IsNaN(tr.position.z) || Math.Abs(tr.position.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(tr.position.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(tr.position.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(m_pFSM.name + " Jumping Send_Sync_Position Position Error " + tr.position);
            return;
        }


        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)EntityState.Glide;
        data.fPosition_x = tr.position.x;
        data.fPosition_y = tr.position.y;
        data.fPosition_z = tr.position.z;

        // 直接用欧拉角
        data.fRotation_x = tr.rotation.eulerAngles.x;
        data.fRotation_y = tr.rotation.eulerAngles.y;
        data.fRotation_z = tr.rotation.eulerAngles.z;

        data.fVelocity_x = m_pFSM.syncVelocity.x;
        data.fVelocity_y = m_pFSM.syncVelocity.y;
        data.fVelocity_z = m_pFSM.syncVelocity.z;
        data.nSyn2Server = (int)reportToSever;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);

        // 通知主角位置发生变化
        U3D_Render.EntityLocUpdate.SendActorLocationChange(new Vector3(tr.position.x, tr.position.y, tr.position.z));
        EntityBatchCommunicator.SendEntityPro(m_pFSM.entityID, tr.position, tr.rotation.eulerAngles);

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

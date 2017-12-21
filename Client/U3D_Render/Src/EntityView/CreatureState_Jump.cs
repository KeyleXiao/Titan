using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using GameLogic;

public class CreatureState_Jump : ICreatureState
{
    
    // 状态机指针
    CreatureStateMachine m_pFSM;    

    //人物的transform
    Transform tr;

    [HideInInspector]
    public bool grounded = true;

    public float startHeight = 0.0f;

    private SlidingAttribute sliding=new SlidingAttribute();

    private int hashid_isMoving;
    private int hashid_moveright;
    private int hashid_moveforward;
    private int hashid_velocity;

    // 构造
    public CreatureState_Jump(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        hashid_isMoving = Animator.StringToHash("isMoving");
        hashid_moveright = Animator.StringToHash("moveright");
        hashid_moveforward = Animator.StringToHash("moveforward");
        hashid_velocity = Animator.StringToHash("velocity");
    }

    // 取得ID
    public int GetID() { return (int)EntityState.Jumping; }

    // 进入状态
    private cmd_creature_longjump m_longjumpParam;
    private bool m_bLongJump;
    private float m_fLongJumpGravity;

    public void ClearState()
    {
    }

    public bool OnEnter(IntPtr lParam)
    {
        tr = m_pFSM.transform;
        startUpTick = 0;
        if (m_pFSM.animator != null)
        {
            m_pFSM.animator.SetBool("isLanding", false);
        }
        m_bIsLanding = false;
        m_bIsJumping = false;

        m_bLongJump=false;
        if (lParam != IntPtr.Zero)
        {
            //非空格键触发的特殊跳跃，由上层发放相关参数，这里只记录参数，仍然靠inputJump触发起跳
            m_pFSM.inputJump = true;
            m_bLongJump = true;
            m_longjumpParam = IntPtrHelper.toData<cmd_creature_longjump>(lParam);
        }
       else if (m_pFSM.inputJump == false)
       {
           //不是主动跳跃，是直接跌落的
           m_bIsLanding = false;
           m_bIsJumping = true;
           if (m_pFSM.animator != null)
           {
                m_pFSM.animator.SetBool("Jumping", true);//.CrossFade("Jump_Front", 0.1f);
           }
           Vector3 jumpDir = Vector3.up;
           Send_Sync_Jump(); 
           grounded = false;
           m_pFSM.groundNormal = Vector3.zero;
        }
       else
       {
           if (m_pFSM.creaturePropety.effectIDForJump>0)
           {
               m_pFSM.creaturePropety.Master.LightingEffectManager.BindLighting(m_pFSM.creaturePropety.effectIDForJump);
           }
       }

       startHeight = m_pFSM.transform.position.y;
        oldDodgeSpeed = m_pFSM.maxDodgeSpeed;

        //设置一开始的位置，时刻监测高度差
        glideLastHitPoint = m_pFSM.transform.position + new Vector3(0, 0.1f, 0);

        return true;
    }

    private uint dodgeTimes = 0;
    private const uint MAX_DODGE_TIMES = 3;
    private float oldDodgeSpeed;
    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        if (lParam != IntPtr.Zero)
        {
            //非空格键触发的特殊跳跃，由上层发放相关参数，这里只记录参数，仍然靠inputJump触发起跳
            m_pFSM.inputJump = true;
            m_bLongJump = true;
            m_longjumpParam = IntPtrHelper.toData<cmd_creature_longjump>(lParam);
            return true;
        }

        if (!m_pFSM.dodge)
        {
            return false;
        }
        m_Velocity.y = CalculateJumpVerticalSpeed(m_pFSM.creaturePropety.jumpDodgeHeight + m_pFSM.controller.height);
        Vector3 horizonalV = m_Velocity;    //取平面速度，忽略y
        horizonalV.y = 0;
        m_pFSM.maxDodgeSpeed += m_pFSM.dodgeSpeeds[dodgeTimes];
        dodgeTimes = (dodgeTimes + 1) % MAX_DODGE_TIMES;
        if (m_pFSM.inputJump)
        {
            m_pFSM.inputJump = false;
            if (m_pFSM.animator)
            {
                m_pFSM.animator.SetTrigger("Jumping");
            }
        }
        return true;
    }

    // 离开状态
    public void OnLeave()
    {
        grounded = true;
        m_bLongJump = false;
    }

    //返回是否在地面
    public bool IsGrounded()
    {
        return grounded;
    }

    public bool isFloating()
    {
        return false;
    }

    // 更新状态逻辑
    public void Update()
    {
        UpdateFunction();
        if (m_bIsLanding)//在Land状态结束本状态
        {
            {
                m_bIsLanding=false;
                m_bIsJumping = false;
                m_pFSM.inputJump = false;
                if (m_pFSM.dodge)
                    m_pFSM.maxDodgeSpeed = oldDodgeSpeed;
                m_pFSM.ChangeState((int)EntityState.Standing, IntPtr.Zero);

                if (m_pFSM.isHero)
                {
                    //落地时广播一次
                    int curTick = System.Environment.TickCount;
                    Send_Sync_Position(MoveSyncFlag.TO_SERVER);
                   //Send_Sync_Position(MoveSyncFlag.TO_BROADCAST);
                    m_lastSyncTick = curTick;
                    m_lastSyncVel = m_pFSM.syncVelocity;
                    m_lastSyncPos = tr.position;
                    
                }


            }
            return;
        } 

    }

    /*

    //=================================================================================================================o

    //================================================Aiming===========================================================o

    //=================================================================================================================o
    void AimLookIK() // Unity Pro only!
    {
        Animator a = m_pFSM.m_Animator;

        // Camera forward(Z) is aim direction
        Vector3 targetDir = Initialize.mainCam.transform.position + Initialize.mainCam.transform.forward * 22;

        a.SetLookAtPosition(targetDir);
        a.SetLookAtWeight(1, 0.5f, 0.7f, 0.0f, 0.5f);
    }
    //=================================================================================================================o

    //public void OnAnimatorIK(int layerIndex) // Unity Pro only!
    //{
    //    if (m_pFSM.isHero == false)
    //    {
    //        return;
    //    }
    //    if (layerIndex == 1)
    //    {
    //        //FeetPlacementIK(); 跳跃就不需要脚部IK了
    //    }
    //    if (layerIndex == 2)
    //    {
    //        AimLookIK();
    //    }
    //}

    */

    private Vector3 m_Velocity;         //基本的速度，未添加重力等因素，比较平滑
    private bool m_bIsLanding = false;
    private bool m_bIsJumping = false;

    //CharacterMotor移植过来的移动控制部分
    private int m_lastSyncTick = 0;
    private Vector3 m_lastSyncVel = Vector3.zero;
    private Vector3 m_lastSyncPos = Vector3.zero;

    private float deltaTime = 0.0f;
    Vector3 glideLastHitPoint = Vector3.zero;
    private void UpdateFunction()
    {
        deltaTime = Time.deltaTime;
        if (deltaTime < 0.001f)
        {
            Debug.LogError("deltaTime=" + deltaTime);
            return;
        }

        if (m_pFSM.moveCtrl.isCharge())
        {
            if (m_pFSM.animator != null)
            {
                m_pFSM.animator.SetFloat(hashid_moveforward, 0);
                m_pFSM.animator.SetFloat(hashid_moveright, 0);
                m_pFSM.animator.SetFloat(hashid_velocity, 0);
                m_pFSM.animator.SetBool(hashid_isMoving, false);
            }
            return;
        }

        Vector3 velocity = m_Velocity;

        m_pFSM.syncDetalTime += deltaTime;

        if (!m_pFSM.isHero || m_pFSM.bControlledBySever)
        {
            velocity = m_pFSM.syncVelocity;
            calSyncVel(tr.position, ref velocity);
        }
        if (float.IsNaN(velocity.x) || float.IsNaN(velocity.y) || float.IsNaN(velocity.z))
        {
            Debug.LogError(m_pFSM.name+" jumping (Line226) velocity error " + velocity);
            velocity = Vector3.zero;
        }

        Vector3 inputVelocity=ApplyInputVelocityChange(velocity); //根据键盘输入调整速度，一般在平地才用

        if (float.IsNaN(inputVelocity.x) || float.IsNaN(inputVelocity.y) || float.IsNaN(inputVelocity.z))
        {
            Debug.LogError(m_pFSM.name + "jumping inputVelocity is " + inputVelocity);
            inputVelocity = Vector3.zero;
        }

        if (m_bIsLanding && (m_pFSM.isHero && !m_pFSM.bControlledBySever))
        {
            // Update velocity based on input
            velocity = inputVelocity;
            //Trace.Log("Landing vel="+m_Velocity.ToString()+"-->"+velocity.ToString());
        }
        else if (m_pFSM.dodge)
        {
            if (m_pFSM.isHero && !m_pFSM.bControlledBySever)
            {
                velocity = inputVelocity;
            }
            else
            {
                velocity = m_pFSM.syncVelocity;
            }
        }
        else
        {
            //在空中的时候，只稍微考虑一下，为了撞墙后直接跳跃也能爬墙，而不是垂直跳
            Vector3 inputOffset = inputVelocity;
            inputOffset.y = 0.0f;
            Vector3 jumpOffset = velocity;
            jumpOffset.y = 0.0f;

            if (inputOffset.magnitude/2 > jumpOffset.magnitude)
            {
                Vector3 combineOffset = jumpOffset * 0.95f + inputOffset * 0.05f;
                velocity.x = combineOffset.x;
                velocity.z = combineOffset.z;
            }
            if (m_pFSM.isGlideState)
            {
                Vector3 pos = m_pFSM.transform.position + new Vector3(0, 0.1f, 0);
                if (Mathf.Abs(pos.y - glideLastHitPoint.y) > 0.5f)
                {
                    RaycastHit hitInfo;
                    Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
                    Physics.Raycast(PointToGroundRay, out hitInfo, 100);
                    if ((pos.y - hitInfo.point.y) > m_pFSM.controller.height + 0.6f)
                    {
                        m_pFSM.ChangeGlideState();
                    }
                    else
                    {
                        glideLastHitPoint = pos;
                    }
                }
                
            }

        }

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

            if (sendSync != MoveSyncFlag.NOT_2_SERVER &&  (m_pFSM.isHero && !m_pFSM.bControlledBySever))
            {
                m_pFSM.syncVelocity.y = 0.0f; // 跳跃的同步包y方向速度都为0，由实际跳跃产生。                
                Send_Sync_Position(sendSync);
                m_lastSyncTick = curTick;
                m_lastSyncVel = m_pFSM.syncVelocity;
                m_lastSyncPos = tr.position;
            }
        }

        // We copy the actual velocity into a temporary variable that we can manipulate.        

        // Apply gravity and jumping force
        velocity = ApplyGravityAndJumping(velocity);

        if (float.IsNaN(velocity.x) || float.IsNaN(velocity.y) || float.IsNaN(velocity.z))
        {
            Debug.LogError(m_pFSM.name+ " jumping velocity (line 304) error" + velocity);
            velocity = Vector3.zero;
        }

        if (m_bIsLanding && m_pFSM.animator != null)
        {

            Vector3 planeVel = new Vector3(velocity.x, 0, velocity.z);
            if (planeVel.sqrMagnitude > 0.0f)
            {
                m_pFSM.animator.SetBool(hashid_isMoving, true);
            }
            else
            {
                m_pFSM.animator.SetBool(hashid_isMoving, false);
            }

            //取移动速度相对于主角面向的向前和向左的标量
            float fRight = Vector3.Dot(velocity, tr.right);
            float fForward = Vector3.Dot(velocity, tr.forward);


            //移动速度考虑了服务器速度和基本速度的比率后，按服务器速度算animator参数值

            if (m_pFSM.maxForwardSpeed > 0)
            {
                fForward = fForward / m_pFSM.maxForwardSpeed;
            }
            else
            {
                fForward = fForward / m_pFSM.creaturePropety.runSpeed_Forward;
            }

            if (m_pFSM.maxRunSidewaySpeed > 0)
            {
                fRight = fRight / m_pFSM.maxRunSidewaySpeed;
            }
            else
            {
                fRight = fRight / m_pFSM.creaturePropety.runSpeed_LeftRight;
            }

            
            //fRight = fRight / m_pFSM.basicSidewaysSpeed;
            ////速度和动作不能成正比，线性的话速度小的时候容易滑步，这里利用开根做了一次非线性变换
            //if (fForward >= 0.0f)
            //{
            //    fForward = fForward / m_pFSM.basicForwardSpeed;
            //    fForward = Mathf.Sqrt(fForward);
            //}
            //else
            //{
            //    fForward = fForward / m_pFSM.basicBackwardSpeed;
            //    fForward = -Mathf.Sqrt(-fForward);
            //}

            //if (fRight >= 0.0f)
            //{
            //    fRight = Mathf.Sqrt(fRight);
            //}
            //else
            //{
            //    fRight = -Mathf.Sqrt(-fRight);
            //}



            m_pFSM.animator.SetFloat(hashid_moveright, fRight);
            m_pFSM.animator.SetFloat(hashid_moveforward, fForward);
            m_pFSM.animator.SetFloat(hashid_velocity, velocity.magnitude);

            //动画速率用maxSpeed和basicSpeed比率控制，这里暂时不控制
            //float speedScale = Mathf.Max(Mathf.Abs(fRight), Mathf.Abs(fForward));
            //if (speedScale > 1.0f)
            //{
            //    speedScale = (speedScale - 1.0f) * 0.3f + 1.0f; //张嘉华提供的跑路速度和跑路动画速度关系
            //    m_pFSM.animator.speed = speedScale;
            //}
            //else
            //{
            //    m_pFSM.animator.speed = 1.0f;
            //}
        }

        // Moving platform support
        Vector3 moveDistance = Vector3.zero;

        // Save lastPosition for velocity calculation.
        Vector3 lastPosition = tr.position;

        // We always want the movement to be framerate independent.  Multiplying by Time.deltaTime does this.

        Vector3 currentMovementOffset = velocity * deltaTime;

        // Find out how much we need to push towards the ground to avoid loosing grouning
        // when walking down a step or over a sharp change in slope.
        float pushDownOffset = Mathf.Max(m_pFSM.controller.stepOffset, (new Vector3(currentMovementOffset.x, 0, currentMovementOffset.z)).magnitude);
        if (float.IsNaN(m_pFSM.controller.stepOffset))
        {
            Debug.LogError(m_pFSM.name + " jumping controller.stepOffset error" + m_pFSM.controller.stepOffset);
            pushDownOffset = (new Vector3(currentMovementOffset.x, 0, currentMovementOffset.z)).magnitude;
        }


        if (grounded)
            currentMovementOffset -= pushDownOffset * Vector3.up;


        m_pFSM.collisionFlags = m_pFSM.controller.Move(currentMovementOffset);

        //先暂时关闭拉扯位置，保证非主角跳跃平滑
        //if (!m_pFSM.isHero) //前面都是自己算的运动轨迹，非主角考虑同步位置的问题，避免连续跳跃（例如轻功）时位置偏移过大
        //{
        //    Vector3 syncOffset = m_pFSM.syncPosition - tr.position;
        //    syncOffset.y = 0.0f;
        //    if (syncOffset.sqrMagnitude > 0.01f)
        //    {
        //        m_pFSM.controller.Move(syncOffset * 0.8f);
        //    }
        //}


        // Calculate the velocity based on the current and previous position.  
        // This means our velocity will only be the amount the character actually moved as a result of collisions.
        Vector3 oldHVelocity = new Vector3(velocity.x, 0, velocity.z);
        m_Velocity = (tr.position - lastPosition) / deltaTime;
        Vector3 newHVelocity = new Vector3(m_Velocity.x, 0, m_Velocity.z);

        // The CharacterController can be moved in unwanted directions when colliding with things.
        // We want to prevent this from influencing the recorded velocity.
        //if (oldHVelocity == Vector3.zero)
        if (oldHVelocity.sqrMagnitude < 0.0001f)
        {
            m_Velocity = new Vector3(0, m_Velocity.y, 0);
        }
        else
        {
            float projectedNewVelocity = Vector3.Dot(newHVelocity, oldHVelocity) / oldHVelocity.sqrMagnitude;
            m_Velocity = oldHVelocity * Mathf.Clamp01(projectedNewVelocity) + m_Velocity.y * Vector3.up;
        }


        if (m_Velocity.y < velocity.y - 0.001)
        {
            if (m_Velocity.y < 0)
            {
                // Something is forcing the CharacterController down faster than it should.
                // Ignore this
                m_Velocity.y = velocity.y;
            }
        }

        // We were grounded but just loosed grounding
        if (grounded && !IsGroundedTest())
        {
            grounded = false;              

            //m_pFSM.SendMessage("OnFall", SendMessageOptions.DontRequireReceiver);
            // We pushed the character down to ensure it would stay on the ground if there was any.
            // But there wasn't so now we cancel the downwards offset to make the fall smoother.
            tr.position += pushDownOffset * Vector3.up;
        }
        // We were not grounded but just landed on something
        else if (!grounded && IsGroundedTest())
        {
            grounded = true;
            if (m_pFSM.animator)
            {
                m_pFSM.animator.SetBool("isLanding", true);

                Vector3 moveDir = m_pFSM.inputMoveDirection;

                Vector3 planeVel = new Vector3(moveDir.x, 0, moveDir.z);
                if (planeVel.sqrMagnitude > 0.0f)
                {
                    m_pFSM.animator.SetBool(hashid_isMoving, true);
                }
                else
                {
                    m_pFSM.animator.SetBool(hashid_isMoving, false);
                }
            }



            m_bIsLanding = true;
            m_bIsJumping = false;
            m_bLongJump = false;
            
            //float maxFallSpeed = m_pFSM.SkinConfig == null ? 15 : m_pFSM.SkinConfig.maxFallSpeed;
            //int landingSpeed = (int)(-velocity.y);
            float fallHeight = startHeight - m_pFSM.transform.position.y;
            //if (landingSpeed > maxFallSpeed / 2.0f) //着陆大于最大速度的一半，可能需要计算伤害，上报逻辑层
            if (m_pFSM.isHero && fallHeight > 5.0f) //着陆时落差大于5米，则上报逻辑层
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

            if (m_pFSM.creaturePropety.effectIDForLand > 0)
            {
                m_pFSM.creaturePropety.Master.LightingEffectManager.BindLighting(m_pFSM.creaturePropety.effectIDForLand);
            }


            //m_pFSM.SendMessage("OnLand", SendMessageOptions.DontRequireReceiver);
        }

        if (m_bIsLanding == false)
        {
            m_pFSM.groundNormal = Vector3.zero;
        }

    }


    private uint startUpTick = 0;

    private Vector3 ApplyGravityAndJumping(Vector3 velocity)
    {
        float gravity = m_pFSM.currentGravity;
        if (m_bLongJump && getVelocity().y > 0.0f)
        {
            gravity = m_fLongJumpGravity;
        }

        if (grounded)
        {
            velocity.y = Mathf.Min(0, velocity.y) - gravity * deltaTime;
        }
        else
        {
            velocity.y = m_Velocity.y - gravity * deltaTime;
            // Make sure we don't fall any faster than maxFallSpeed. This gives our character a terminal velocity.
            velocity.y = Mathf.Max(velocity.y, m_pFSM.SkinConfig == null ? -15 : -m_pFSM.SkinConfig.maxFallSpeed);
            //velocity.y = Mathf.Min(velocity.y, m_pFSM.SkinConfig == null ? 15 : m_pFSM.SkinConfig.maxFallSpeed);
        }

        uint tick = GameLogicAPI.getTickCount();
        if (grounded) {
		    // Jump only if the jump button was pressed down in the last 0.2 seconds.
		    // We use this check instead of checking if it's pressed down right now
		    // because players will often try to jump in the exact moment when hitting the ground after a jump
		    // and if they hit the button a fraction of a second too soon and no new jump happens as a consequence,
		    // it's confusing and it feels like the game is buggy.            

            if ((m_pFSM.inputJump == true) && ((m_pFSM.animator==null) || (m_pFSM.animator.GetBool("isLanding") == false)) && (m_bIsJumping == false))
            {
                //if (m_pFSM.isHero)
                //{
                //    Send_Sync_Jump();
                //}
			    
			    m_pFSM.inputJump=false;
                m_bIsLanding = false;
                m_bIsJumping = true;
                if(m_pFSM.animator)
                {
                    m_pFSM.animator.SetTrigger("Jumping");
                }

                startUpTick = tick;
                if (!m_bLongJump && m_pFSM.SkinConfig != null && m_pFSM.SkinConfig.jumpDeltyTick>1.0f/FPS.Value*1000.0f) //跳跃的起跳延迟考虑FPS值，如果FPS过低则直接起跳，不等下一帧（避免有延时的感觉）
                {
                    startUpTick += (uint)m_pFSM.SkinConfig.jumpDeltyTick;
                }
            }

            if ((startUpTick > 0) && (tick >= startUpTick))
            {
                Vector3 jumpDir = Vector3.up;
                startUpTick = 0;
                // Apply the jumping force to the velocity. Cancel any vertical velocity first.
                velocity.y = 0;

                if (m_bLongJump)
                {
                    m_fLongJumpGravity = 2.0f * m_longjumpParam.fUpHeight / (m_longjumpParam.fUpTime*m_longjumpParam.fUpTime);
                    velocity += jumpDir * m_fLongJumpGravity * m_longjumpParam.fUpTime;
                }
                else if (m_pFSM.dodge)
                {
                    velocity += jumpDir * CalculateJumpVerticalSpeed(m_pFSM.creaturePropety.jumpDodgeHeight);
                }
                else
                {
                    velocity += jumpDir * CalculateJumpVerticalSpeed(m_pFSM.creaturePropety.jumpBaseHeight);
                }

                grounded = false;
                m_pFSM.groundNormal = Vector3.zero;
                //m_pFSM.SendMessage("OnJump", SendMessageOptions.DontRequireReceiver);
            }
        }


        return velocity;
    }

    bool TooSteep()
    {
        return (m_pFSM.groundNormal.y <= Mathf.Cos(m_pFSM.controller.slopeLimit * Mathf.Deg2Rad));
    }

    private bool IsGroundedTest()
    {
        return (m_pFSM.groundNormal.y > 0.01f);
    }

    float CalculateJumpVerticalSpeed (float targetJumpHeight)
    {
	    // From the jump height and gravity we deduce the upwards speed 
	    // for the character to reach at the apex.
        return Mathf.Sqrt(2.0f * targetJumpHeight * m_pFSM.creaturePropety.upGravity);
    }

    private void Send_Sync_Jump()
    {
        cmd_creature_act_sync data = new cmd_creature_act_sync();
        data.pos.fPosition_x = tr.position.x;
        data.pos.fPosition_y = tr.position.y;
        data.pos.fPosition_z = tr.position.z;

        //// 直接用欧拉角
        data.pos.fRotation_x = tr.rotation.eulerAngles.x;
        data.pos.fRotation_y = tr.rotation.eulerAngles.y;
        data.pos.fRotation_z = tr.rotation.eulerAngles.z;

        data.pos.fVelocity_x = m_pFSM.controller.velocity.x;
        data.pos.fVelocity_y = 0;// m_pFSM.controller.velocity.y; 跳跃的同步包y方向速度都为0，由实际跳跃产生。
        data.pos.fVelocity_z = m_pFSM.controller.velocity.z;

        data.nCtrID = (int)ActID.Jump;
        data.state.bJump = 1;

        EntityEventHelper.Instance.SendCommand<cmd_creature_act_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_CTRL, ref data);

        if (m_pFSM.showLog)
        {
            Trace.Log("Send_Sync_Jump" + m_pFSM.entityID.ToString() + "Jump!");
        }

    }


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

     private Vector3 ApplyInputVelocityChange(Vector3 velocity)
     {
         if (!m_pFSM.canControl)
             m_pFSM.inputMoveDirection = Vector3.zero;

         // Find desired velocity
         Vector3 desiredVelocity;

             desiredVelocity = GetDesiredHorizontalVelocity();


         if (grounded || m_pFSM.dodge)
             desiredVelocity = AdjustGroundVelocityToNormal(desiredVelocity, m_pFSM.groundNormal);
         else
             velocity.y = 0;

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


         if (grounded)
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

     // Project a direction onto elliptical quater segments based on forward, sideways, and backwards speed.
     // The function returns the length of the resulting vector.
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

     ///////////////////////////////////////////////////////////////////////////////////////////////////////////
     // 发送一次位置同步
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
         data.nCreatureState = (int)EntityState.Jumping;
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



     private Vector3 syncDir;
     //非主角玩家，计算同步的offset，考虑延时和平滑
     private void calSyncVel(Vector3 lastPos, ref Vector3 velocity)
     {
         //uint currentTick = GameLogicAPI.getTickCount();
         //int deltaTick = (int)(currentTick - m_pFSM.syncTick);
         //if (deltaTick <= 1)
         //{
         //    deltaTick = 1;
         //}
         //float deltaTime = ((float)deltaTick) / 1000.0f;
         Vector3 idealPos = m_pFSM.syncPosition + m_pFSM.syncVelocity * m_pFSM.syncDetalTime; //根据同步包算出来的理想位置         
         Vector3 moveVel = (idealPos - lastPos) / m_pFSM.syncDetalTime; //理想位置减去上一帧的位置，即为期望的速度

         if (moveVel.magnitude > velocity.magnitude * 2.0f) //变化太剧烈，则限定为原速度的2倍
         {
             moveVel.Normalize();
             moveVel = moveVel * velocity.magnitude*2.0f;
         }

         velocity.x = moveVel.x;
         velocity.z = moveVel.z;
     }



     // 获得基本速度
     public Vector3 getVelocity()
     {
         return m_Velocity;
     }
     public void setVelocity(Vector3 vel)
     {
         m_Velocity = vel;
     }


     /// <summary>
     /// 传送，清楚掉移动消息
     /// </summary>
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

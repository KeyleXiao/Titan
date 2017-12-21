using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using GameLogic;
using System.Runtime.InteropServices;


/// <summary>
/// 滑动
/// </summary>
public class SlidingAttribute
{
    // Does the character slide on too steep surfaces?
    public bool enabled = true;

    // How fast does the character slide on steep surfaces?
    public float slidingSpeed = 15;

    // How much can the player control the sliding direction?
    // If the value is 0.5 the player can slide sideways with half the speed of the downwards sliding speed.
    public float sidewaysControl = 1.0f;

    // How much can the player influence the sliding speed?
    // If the value is 0.5 the player can speed the sliding up to 150% or slow it down to 50%.
    public float speedControl = 0.4f;
}

public class CreatureState_Standing : ICreatureState
{

    // 状态机指针
    CreatureStateMachine m_pFSM;

    //人物的transform
    Transform tr;

    [HideInInspector]
    public bool grounded = true;
    public bool bIsMoving = false;

    //private bool useIdleFeetPlacement = true;

    [HideInInspector]
    public LayerMask groundLayers = -1;

    private SlidingAttribute sliding = new SlidingAttribute();


    private int hashid_isMoving;
    private int hashid_moveright;
    private int hashid_moveforward;
    private int hashid_velocity;

    // 构造
    public CreatureState_Standing(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
        //m_pFSM.controller = pFsm.GetComponent<CharacterController>();
        hashid_isMoving = Animator.StringToHash("isMoving");
        hashid_moveright = Animator.StringToHash("moveright");
        hashid_moveforward = Animator.StringToHash("moveforward");
        hashid_velocity = Animator.StringToHash("velocity");
    }

    // 取得ID
    public int GetID() { return (int)EntityState.Standing; }

    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        if (m_pFSM.animator)
        {
            m_pFSM.animator.SetBool("isLanding", true);
        }

        tr = m_pFSM.transform;
        m_lastFrameDeltaTime = 0.0f;
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

    /* 废弃代码，脚步IK

    //=================================================================================================================o

    //============================================Feet Placement=======================================================o

    //=================================================================================================================o
    void FeetPlacementIK() // Unity Pro only!
    {
        Transform creature = m_pFSM.creature;
        Animator a = m_pFSM.m_Animator;

        if (!useIdleFeetPlacement)
        {
            if (a.GetLayerWeight(1) != 0)
                a.SetLayerWeight(1, 0); // Set Leglayer weight to 0 
            return;
        }

        if (!grounded)
            return;

        AnimatorStateInfo st = a.GetCurrentAnimatorStateInfo(0);

        // Cancel if out of MoveTree State 
        if (!st.IsTag("MovingTree"))
            return;

        float LegLayerW = a.GetLayerWeight(2);

        // Axis movement
        if (a.GetBool("isMoving"))
        {
            if (LegLayerW > 0)
                a.SetLayerWeight(1, LegLayerW - (Time.deltaTime + 0.2f));
        }
        else // Idle
        {
            if (LegLayerW < 1)
                a.SetLayerWeight(1, LegLayerW + (Time.deltaTime + 0.1f));
        }

        if (LegLayerW > 0.1f)
        {
            RaycastHit rL;
            RaycastHit rR;
            Vector3 leftP = (creature.position + creature.up) - creature.right / 4.5f;
            Vector3 rightP = (creature.position + creature.up) + creature.right / 4.5f;

            if (!Physics.Raycast(leftP, Vector3.down, out rL, 2, groundLayers))
                return;
            if (!Physics.Raycast(rightP, Vector3.down, out rR, 2, groundLayers))
                return;

            float dif = Mathf.Abs(rL.distance - rR.distance);
            if (dif < 0.05f) // If groundheight difference is bigger than...
                return;

            a.SetIKPositionWeight(AvatarIKGoal.LeftFoot, LegLayerW);
            a.SetIKPositionWeight(AvatarIKGoal.RightFoot, LegLayerW);

            Vector3 rheight = Vector3.Lerp(Vector3.zero, new Vector3(0, dif, 0), LegLayerW);
            a.bodyPosition -= rheight;

            Vector3 feetHeight = new Vector3(0, 0.09f, 0);
            Vector3 leftFootPos = rL.point + feetHeight;
            Vector3 rightFootPos = rR.point + feetHeight;

            a.SetIKPosition(AvatarIKGoal.LeftFoot, leftFootPos);
            a.SetIKPosition(AvatarIKGoal.RightFoot, rightFootPos);
        }
    }

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

    public void OnAnimatorIK(int layerIndex) // Unity Pro only!
    {
        if (m_pFSM.isHero == false)
        {
            return;
        }

        if (layerIndex == 1)
        {
            FeetPlacementIK();
        }
        if (layerIndex == 2)
        {
            AimLookIK();
        }
    }

    */

    float approximatethre = 5.0f;       // 采用逼近公式的阈值，保证强制收敛到同步目的地 
    float apmtspeedratio = 5.0f;		// 逼近时的速度系数

    float lastupdatetime = 0.0f;
    Vector3 lastvelocity = new Vector3(0.0f, 0.0f, 0.0f);
    Vector3 lastoffset = new Vector3(0.0f, 0.0f, 0.0f);
    Vector3 currentvelocity = new Vector3(0.0f, 0.0f, 0.0f);

    MoveSyncFlag sendSync = MoveSyncFlag.NOT_2_SERVER;
    public bool physenable = false;

    int heroMoveTimes = 0;
    private float deltaTime = 0.0f;
    private float m_lastFrameDeltaTime = 100.0f;
    private void UpdateFunction()
    {
        deltaTime = Time.deltaTime;
        if (deltaTime < 0.001f)
        {
#if  UNITY_EDITOR
            Debug.LogError("deltaTime=" + deltaTime);
#else 
            Trace.Log("deltaTime=" + deltaTime);
#endif
            return;
        }

        if (m_lastFrameDeltaTime <= 0.01f)
        {
            m_lastFrameDeltaTime = deltaTime;
        }

        if (m_pFSM.moveCtrl.isCharge())
        {
            if (m_pFSM.animator != null)
            {
                //Trace.LogWarning("can't move: m_pFSM.bForceMoving=" + m_pFSM.bForceMoving.ToString() + ",m_pFSM.bForceFlighting=" + m_pFSM.bForceFlighting.ToString());
                m_pFSM.animator.SetFloat(hashid_moveforward, 0);
                m_pFSM.animator.SetFloat(hashid_moveright, 0);
                m_pFSM.animator.SetFloat(hashid_velocity, 0);
                m_pFSM.animator.SetBool(hashid_isMoving, false);
            }
            return;
        }

        sendSync = MoveSyncFlag.NOT_2_SERVER;

        //--------------------------------------------------------
        // 下面算速度
        //--------------------------------------------------------
        if (float.IsNaN(m_pFSM.creaturePropety.runSpeed_Forward))
        {
            Debug.LogError(m_pFSM.name + " Standing m_pFSM.creaturePropety.runSpeed_Forward error:" + m_pFSM.creaturePropety.runSpeed_Forward);
            return;
        }

        float finalInertiaAcceleration = m_pFSM.creaturePropety.InertiaAcceleration *
            ((m_pFSM.maxForwardSpeed / m_pFSM.creaturePropety.runSpeed_Forward - 1.0f) * m_pFSM.creaturePropety.InertiaScale + 1.0f); //惯性加速度


        currentvelocity = new Vector3(0.0f, 0.0f, 0.0f);
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever) //主角的话下面多折腾下速度
        {
            // Calculate the velocity based on the current and previous position.  
            // This means our velocity will only be the amount the character actually moved as a result of collisions.
            Vector3 oldHVelocity = new Vector3(lastvelocity.x, 0, lastvelocity.z);
            Vector3 newHVelocity = lastoffset / deltaTime;
            newHVelocity = new Vector3(newHVelocity.x, 0, newHVelocity.z);

            // The CharacterController can be moved in unwanted directions when colliding with things.
            // We want to prevent this from influencing the recorded velocity.
            //if (oldHVelocity == Vector3.zero)
            if (oldHVelocity.sqrMagnitude < 0.0001f)
            {
                newHVelocity = new Vector3(0, newHVelocity.y, 0);
            }
            else
            {
                float projectedNewVelocity = Vector3.Dot(newHVelocity, oldHVelocity) / oldHVelocity.sqrMagnitude; //方向不变 夹角为0 点积为|a||b|*1  
                newHVelocity = oldHVelocity * Mathf.Clamp01(projectedNewVelocity) + newHVelocity.y * Vector3.up;
            }



            if (newHVelocity.y < currentvelocity.y - 0.001f)
            {
                if (newHVelocity.y < 0)
                {
                    // Something is forcing the CharacterController down faster than it should.
                    // Ignore this
                    newHVelocity.y = lastvelocity.y;
                }
            }

            // Update velocity based on input
            newHVelocity = ApplyInputVelocityChange(newHVelocity);
            if (newHVelocity.sqrMagnitude < 0.0001f) //如果是停止的，就不计算惯性速度，免得技能僵直之类的效果生效不够及时
            {
                if (InputMoveDirectionDelay > 0.2f)
                {
                    currentvelocity = newHVelocity;
                }
                else
                {
                    //短暂的停止按键留有缓冲时间
                    currentvelocity = oldHVelocity;
                    InputMoveDirectionDelay += deltaTime;
                }
            }
            else
            {
                if (InputMoveDirectionDelay != 0) InputMoveDirectionDelay = 0;
                Vector3 HVelocityDelta = newHVelocity - oldHVelocity;
     
                float advance = deltaTime * finalInertiaAcceleration;
                if (advance > HVelocityDelta.magnitude) advance = HVelocityDelta.magnitude;
                currentvelocity = oldHVelocity + HVelocityDelta.normalized * advance;
                if (currentvelocity.magnitude<=0.1f)
                {
                    //按键输入不为0，速度过度时减速最低不能为0,防止上发判断为停止
                    currentvelocity = currentvelocity.normalized * 0.5f;
                }
            }



            // 判断本地的模拟坐标和真实坐标的差别, 只有达到一定阀值才需要同步
            // 暂时在行走过程中200ms同步一次
            int curTick = System.Environment.TickCount;


            //if (currentvelocity.sqrMagnitude <= 0.0001f && currentvelocity != Vector3.zero)
            //{
            //    currentvelocity = Vector3.zero;
            //}
            //if (currentvelocity == Vector3.zero)
            //{
            //    m_pFSM.RunSpeedUpUpdateInfo(0, 0);
            //}
            // 启动和站立强制同步一次
            if ((m_pFSM.syncVelocity.sqrMagnitude > 0.0001f && currentvelocity.sqrMagnitude <= 0.0001f) ||
                (m_pFSM.syncVelocity.sqrMagnitude <= 0.0001f && currentvelocity.sqrMagnitude > 0.001f))
            {
                sendSync = MoveSyncFlag.TO_BROADCAST;
            }
            // 每syncInterval毫秒根据速度或者移动距离考虑是否同步一次
            else if (curTick >= m_pFSM.syncTick + m_pFSM.syncInterval)
            {
                //速度/位置/旋转分别跟上一次同步的数据比对看是否需要同步
                Vector3 velSub = currentvelocity - m_pFSM.syncVelocity;
                Vector3 posSub = tr.position - m_pFSM.syncPosition;
                Vector3 rotSub = tr.eulerAngles - m_pFSM.syncAngle;
                if (velSub.sqrMagnitude > 0.01f || posSub.sqrMagnitude > 0.01f || rotSub.sqrMagnitude > 0.01f)
                    sendSync = MoveSyncFlag.TO_SERVER;
            }

            currentvelocity = ApplyGravity(currentvelocity); //主角的话添加重力到速度
        }
        else//不是主角直接用同步的速度
        {
            currentvelocity = m_pFSM.syncVelocity;
            Vector3 deltapos = m_pFSM.syncPosition - tr.position;
            float dis = deltapos.magnitude;
            if (m_pFSM.syncVelocity.sqrMagnitude < 0.001f && dis > 0.01f)//速度为0，但还没到同步的目的地，补偿一下误差
            {
                float alpha = Mathf.Clamp01((dis - 0.01f) / approximatethre); //对速度根据离同步位置误差距离作逼近收敛修正
                float speed = (1.0f - alpha) * 0.0f + alpha * approximatethre * apmtspeedratio;
                currentvelocity = deltapos.normalized * speed;
            }
            else
            {
                Vector3 newHVelocity = m_pFSM.syncVelocity;
                Vector3 oldHVelocity = new Vector3(lastvelocity.x, 0, lastvelocity.z);
                Vector3 HVelocityDelta = newHVelocity - oldHVelocity;
                float advance = deltaTime * finalInertiaAcceleration;
                if (advance > HVelocityDelta.magnitude) advance = HVelocityDelta.magnitude;
                currentvelocity = oldHVelocity + HVelocityDelta.normalized * advance;
            }
        }
        lastvelocity = currentvelocity;
        CheckMoveForward();//检查是否向前



        //--------------------------------------------------------
        // 下面算位置
        //--------------------------------------------------------
        // We always want the movement to be framerate independent.  Multiplying by Time.deltaTime does this.
        if (deltaTime > m_lastFrameDeltaTime * 2.0f || deltaTime < m_lastFrameDeltaTime / 2.0f) //缓冲帧率突变产生的跳跃
        {
            deltaTime = deltaTime * m_pFSM.frameAccFactor + m_lastFrameDeltaTime * (1 - m_pFSM.frameAccFactor);
        }
        m_lastFrameDeltaTime = deltaTime;

        Vector3 currentOffset = currentvelocity * deltaTime;
        m_pFSM.syncDetalTime += deltaTime;

        // Find out how much we need to push towards the ground to avoid loosing grouning
        // when walking down a step or over a sharp change in slope.
        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//主角
        {
            if (grounded)//主角贴地
            {
                float pushDownOffset = Mathf.Max(m_pFSM.controller.stepOffset, (new Vector3(currentOffset.x, 0, currentOffset.z)).magnitude);
                if (float.IsNaN(m_pFSM.controller.stepOffset))
                {
                    Debug.LogError(m_pFSM.name + "standing m_pFSM.controller.stepOffset " + m_pFSM.controller.stepOffset);
                    pushDownOffset = (new Vector3(currentOffset.x, 0, currentOffset.z)).magnitude;
                }
                currentOffset -= pushDownOffset * Vector3.up;
            }
            if (m_pFSM.ShiftEnable)
            {
                //if (InputManager.GetKey(KeyCode.LeftShift) || InputManager.GetKey(KeyCode.RightShift))//主角暂时添加加速快捷键以便测试
                {
                    currentOffset *= 5.0f;
                }
            }
        }
        else //非主角
        {
            //Vector3 idealPos = tr.position;
            //calSyncOffset(currentvelocity, ref currentOffset, ref idealPos);
            //currentvelocity =  * m_pFSM.syncVelocity.magnitude;
            Vector3 dir = (m_pFSM.syncPosition - tr.position).normalized;
            currentvelocity = dir * currentvelocity.magnitude;
            currentOffset = currentvelocity * deltaTime;
            if (currentOffset.sqrMagnitude < 0.001f)
            {
                currentvelocity = Vector3.zero;
            }
        }

        //--------------------------------------------------------
        // 下面更新animator
        //--------------------------------------------------------
        if (m_pFSM.animatorCtrl.ana != null && m_pFSM.animatorCtrl.ana.isInitialized)
        {
            Vector3 planeVel = new Vector3(currentvelocity.x, 0, currentvelocity.z);
            if (planeVel.sqrMagnitude > 0.0001f)
            {
                m_pFSM.animatorCtrl.ana.SetBool("isMoving", true);
                bIsMoving = true;
            }
            else
            {
                m_pFSM.animatorCtrl.ana.SetBool("isMoving", false);
                bIsMoving = false;
            }

            //取移动速度相对于主角面向的向前和向左的标量
            float fRight = Vector3.Dot(currentvelocity, tr.right);
            float fForward = Vector3.Dot(currentvelocity, tr.forward);
            if (currentvelocity.sqrMagnitude < 0.0036f)
            {
                //同步过来的速度为0之后，点乘还会有误差，造成轻微移动动作
                fRight = 0.0f;
                fForward = 0.0f;
            }

            if (Mathf.Abs(fRight) < 0.06f)
            {
                fRight = 0.0f;
            }
            if (Mathf.Abs(fForward) < 0.06f)
            {
                fForward = 0.0f;
            }

            //移动速度考虑了服务器速度和基本速度的比率后，按服务器速度算animator参数值

            if (m_pFSM.maxForwardSpeed > 0)
            {
                if (fForward >= 0.0f)
                {
                    fForward = fForward / m_pFSM.maxForwardSpeed;
                }
                else
                {
                    fForward = fForward / m_pFSM.maxRunBackwardSpeed;
                }
            }
            else
            {
                if (fForward > 0.0f)
                {
                    fForward = fForward / m_pFSM.creaturePropety.runSpeed_Forward;
                }
                else
                {
                    fForward = fForward / m_pFSM.creaturePropety.runSpeed_Back;
                }
            }

            if (m_pFSM.maxRunSidewaySpeed > 0)
            {
                fRight = fRight / m_pFSM.maxRunSidewaySpeed;
            }
            else
            {
                fRight = fRight / m_pFSM.creaturePropety.runSpeed_LeftRight;
            }


            //fForward = fForward / m_pFSM.basicForwardSpeed;
            //fRight = fRight / m_pFSM.basicSidewaysSpeed;


            //速度和动作不能成正比，线性的话速度小的时候容易滑步，这里利用开根做了一次非线性变换
            //if (fForward >= 0.0f)
            //{
            //    fForward = Mathf.Sqrt(fForward);
            //}
            //else
            //{
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

            if (m_pFSM.animatorCtrl.ana != null && m_pFSM.animatorCtrl.ana.isInitialized)
            {
                m_pFSM.animatorCtrl.ana.SetFloat(hashid_moveright, fRight);
                m_pFSM.animatorCtrl.ana.SetFloat(hashid_moveforward, fForward);
                m_pFSM.animatorCtrl.ana.SetFloat(hashid_velocity, currentvelocity.magnitude);
            }

            if (m_pFSM.animatorCtrl.anb != null && m_pFSM.animatorCtrl.anb.isInitialized)
            {
                m_pFSM.animatorCtrl.anb.SetFloat(hashid_moveright, fRight);
                m_pFSM.animatorCtrl.anb.SetFloat(hashid_moveforward, fForward);
            }
             //m_pFSM. updateMovingAnimSpeed();
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


        //--------------------------------------------------------
        // 下面更新transform
        //--------------------------------------------------------
        // Save lastPosition for velocity calculation.
        Vector3 oldPosition = tr.position;

        float curtime = Time.time;

        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//是主角，都用耗时的物理行走方法
        {
            m_pFSM.collisionFlags = m_pFSM.controller.Move(currentOffset);
            ++heroMoveTimes;
        }
        else//非主角
        {
            float pos_dis = (m_pFSM.syncPosition - tr.transform.position).magnitude;
            if (m_pFSM.syncVelocity.sqrMagnitude < 0.01f && m_pFSM.animatorCtrl.bDoingAttackMove)
            {
                pos_dis = 0.0f; //正在利用动作控制位移，就不按同步包移动了
            }
            if (physenable && pos_dis > approximatethre) //跟同步位置差距较大时用耗时的物理行走方法
            {
                m_pFSM.collisionFlags = m_pFSM.controller.Move(currentOffset);
            }
            else if (pos_dis > 0.01f) //否则用较小耗时的渐趋逼近公式
            {
                Vector3 deltapos = m_pFSM.syncPosition - tr.transform.position;
                Vector3 curPos;
                if (deltapos.sqrMagnitude > currentOffset.sqrMagnitude)
                {
                    curPos = tr.transform.position + currentOffset;
                }
                else
                {
                    curPos = tr.transform.position * 0.5f + m_pFSM.syncPosition * 0.5f;
                }
                float GroundedCheckDistance = 5.0f;
                curPos = BaseStateMachine.CorrectPosFromPhysic(curPos, m_pFSM.creaturePropety.CreatureHeightInMeters + 0.1f, GroundedCheckDistance);
                tr.transform.SetPosition(curPos);
            }
            else
            {
            }

        }

        if (m_pFSM.isHero && !m_pFSM.bControlledBySever)//主角不用平滑角度
        {

        }
        else//非主角才平滑角度角度
        {
            float angle_dis = (m_pFSM.syncAngle - tr.transform.eulerAngles).magnitude;
            if (angle_dis > 0.1f) //角度差距较大时采用逼近公式
            {
                Vector3 newangle;
                newangle.x = Mathf.LerpAngle(tr.eulerAngles.x, m_pFSM.syncAngle.x, 0.2f);
                newangle.y = Mathf.LerpAngle(tr.eulerAngles.y, m_pFSM.syncAngle.y, 0.2f);
                newangle.z = Mathf.LerpAngle(tr.eulerAngles.z, m_pFSM.syncAngle.z, 0.2f);
                tr.eulerAngles = newangle;
            }
        }

        lastupdatetime = curtime;

        lastoffset = currentOffset;
        //最后算完才上报
        if (sendSync != MoveSyncFlag.NOT_2_SERVER && (m_pFSM.isHero && !m_pFSM.bControlledBySever))
        {
            int curTick = System.Environment.TickCount;
            m_pFSM.syncTick = curTick;
            Vector3 sendVelocity = new Vector3(currentvelocity.x, 0.0f, currentvelocity.z);
            m_pFSM.syncVelocity = sendVelocity;
            m_pFSM.syncPosition = tr.position;
            m_pFSM.syncAngle = tr.eulerAngles;

            Send_Sync_Position(sendSync);
        }
        else if (heroMoveTimes >= 4)
        {
            Vector3 sendVelocity = new Vector3(currentvelocity.x, 0.0f, currentvelocity.z);
            updatePosWithoutSend(sendVelocity, tr.position, tr.eulerAngles);
            heroMoveTimes = 0;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 上报逻辑层位置，但是不上报给服务器
    private void updatePosWithoutSend(Vector3 velocity, Vector3 position, Vector3 angles)
    {
        //如果上报位置范围不对，则输出至LOG，退出
        if (float.IsNaN(position.x) || float.IsNaN(position.y) || float.IsNaN(position.z) || Math.Abs(position.x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(position.y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(position.z) > SceneManager.MAX_SCENE_SIZE)
        {
            Debug.LogError(m_pFSM.name + " standing updatePosWithoutSend Position Error " + position);
            return;
        }


        cmd_creature_rigidbody_sync data = new cmd_creature_rigidbody_sync();
        data.nCreatureState = (int)EntityState.Standing;
        data.fPosition_x = position.x;
        data.fPosition_y = position.y;
        data.fPosition_z = position.z;

        // 直接用欧拉角
        data.fRotation_x = angles.x;
        data.fRotation_y = angles.y;
        data.fRotation_z = angles.z;

        data.fVelocity_x = velocity.x;
        data.fVelocity_y = velocity.y;
        data.fVelocity_z = velocity.z;
        data.nSyn2Server = (int)MoveSyncFlag.NOT_2_SERVER;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);
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
        data.nCreatureState = (int)EntityState.Standing;
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
        data.nDirection = IsMoveForward ? 1 : 2;

        EntityEventHelper.Instance.SendCommand<cmd_creature_rigidbody_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SET_POS, ref data);

        // 通知主角位置发生变化
        U3D_Render.EntityLocUpdate.SendActorLocationChange(new Vector3(m_pFSM.syncPosition.x, m_pFSM.syncPosition.y, m_pFSM.syncPosition.z));

        if (tr == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position tr == null");
            return;
        }

        if (tr.position == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position tr.SetPosition(= null");
            return;
        }

        if (tr.rotation == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position tr.rotation == null");
            return;
        }

        if (tr.rotation.eulerAngles == null)
        {
            Trace.LogError("CreatureState_Standing.cs Send_Sync_Position tr.rotation.eulerAngles == null");
            return;
        }

        EntityBatchCommunicator.SendEntityPro(m_pFSM.entityID, tr.position, tr.rotation.eulerAngles);
        U3D_Render.EntityView ev = EntityFactory.getMainPlayer();
        MapFinderManager.MoveObject(ref ev, tr.position);
    }


    private Vector3 syncDir;
    //非主角玩家，计算同步的offset，考虑延时和平滑
    private void calSyncOffset(Vector3 velocity, ref Vector3 movementOffset, ref Vector3 idealPos)
    {

        idealPos = m_pFSM.syncPosition + velocity * m_pFSM.syncDetalTime; //根据同步包算出来的理想位置

        Vector3 dis = idealPos - m_pFSM.syncPosition;
        dis.y = 0.0f;
        //张嘉华要求，走了100米还没有新的更新包，就拉回上个同步位置
        if (dis.sqrMagnitude > 10000.0f)
        {
            m_pFSM.syncVelocity = Vector3.zero;
            tr.SetPosition(m_pFSM.syncPosition);
            idealPos = m_pFSM.syncPosition;
        }


        ////向理想位置按混合比例偏移，此方案最后同步的位置比较精准
        //Vector3 currentPos=lastPos+velocity*Time.deltaTime;
        //Vector3 blendPos=idealPos*syncOffsetWeight+currentPos*(1-syncOffsetWeight);
        //Vector3 blendOffset=blendPos-lastPos;

        //向理想位置按同步的速度前进，此方案看起来拉扯感比较小
        Vector3 moveVel = idealPos - tr.position;
        moveVel = moveVel.normalized * velocity.magnitude;
        Vector3 blendOffset = moveVel * deltaTime;

        Vector3 blendDir = blendOffset.normalized;

        if (Vector3.Dot(blendDir, syncDir) < -0.1f) //平滑后前进方向与同步包的方向夹角大于95度（arccos(-0.1)），会发生后退拉扯
        {
            if (blendOffset.magnitude < m_pFSM.syncDisThread)
            {
                ////后退拉扯距离小于规定阈值，则前进速度减半，不后退了
                //currentPos = lastPos + velocity / 2.0f * Time.deltaTime;
                //blendPos = currentPos;
                //blendOffset = blendPos - lastPos;
                //blendDir = blendOffset.normalized;


                //后退拉扯距离小于规定阈值，直接停止
                blendOffset = Vector3.zero;
            }
        }

        Vector2 planeVel = new Vector2(velocity.x, velocity.z);
        if (planeVel.sqrMagnitude < 0.001f && blendOffset.magnitude < m_pFSM.syncDisThread) //不移动的时候就不移动了
        {
            blendOffset = Vector3.zero;
            blendOffset.y = m_pFSM.syncPosition.y - tr.position.y;
        }

        movementOffset.x = blendOffset.x;
        movementOffset.y = blendOffset.y;
        movementOffset.z = blendOffset.z;

    }

    private Vector3 ApplyInputVelocityChange(Vector3 velocity)
    {
        if (!m_pFSM.canControl)
            m_pFSM.inputMoveDirection = Vector3.zero;
        // Find desired velocity

        Vector3 inputMoveDirection = m_pFSM.inputMoveDirection;
        if (m_pFSM.moveCtrl.isAutoMoving)
        {
            inputMoveDirection = m_pFSM.autoMoveDirection;
        }


        Vector3 desiredVelocity;
        if (IsSliding())
        {
            // The direction we're sliding in
            desiredVelocity = new Vector3(m_pFSM.groundNormal.x, 0, m_pFSM.groundNormal.z).normalized;
            // Find the input movement direction projected onto the sliding direction
            Vector3 projectedMoveDir = Vector3.Project(inputMoveDirection, desiredVelocity);
            // Add the sliding direction, the speed control, and the sideways control vectors
            desiredVelocity = desiredVelocity + projectedMoveDir * sliding.speedControl + (inputMoveDirection - projectedMoveDir) * sliding.sidewaysControl;
            // Multiply with the sliding speed
            desiredVelocity *= sliding.slidingSpeed;
        }
        else
        {
            //Vector3 desiredLocalDirection = tr.InverseTransformDirection(inputMoveDirection);
            //if (desiredLocalDirection.z < -0.01f)
            //{
            //    m_pFSM.RunSpeedUpUpdateInfo(0, 0);
            //}
            desiredVelocity = GetDesiredHorizontalVelocity(inputMoveDirection);
        }




        if (grounded)
            desiredVelocity = AdjustGroundVelocityToNormal(desiredVelocity, m_pFSM.groundNormal);
        else
            velocity.y = 0;


        // Enforce max velocity change
        float maxVelocityChange = 10.0f;//m_pFSM.maxAcceleration * Time.deltaTime;
        Vector3 velocityChangeVector = (desiredVelocity - velocity);
        if (velocityChangeVector.sqrMagnitude > maxVelocityChange * maxVelocityChange)
        {
            velocityChangeVector = velocityChangeVector.normalized * maxVelocityChange;
        }
        // If we're in the air and don't have control, don't apply any velocity change at all.
        // If we're on the ground and don't have control we do apply it - it will correspond to friction.
        if (grounded || m_pFSM.canControl)
            velocity += velocityChangeVector;
        if (grounded)
        {
            // When going uphill, the CharacterController will automatically move up by the needed amount.
            // Not moving it upwards manually prevent risk of lifting off from the ground.
            // When going downhill, DO move down manually, as gravity is not enough on steep hills.
            velocity.y = Mathf.Min(velocity.y, 0);
        }
        return velocity;
    }

    private Vector3 ApplyGravity(Vector3 velocity)
    {

        if (grounded)
        {
            velocity.y = Mathf.Min(0, velocity.y) - m_pFSM.currentGravity * deltaTime;
        }
        else
        {
            velocity.y = velocity.y - m_pFSM.currentGravity * deltaTime;

            // Make sure we don't fall any faster than maxFallSpeed. This gives our character a terminal velocity.
            velocity.y = Mathf.Max(velocity.y, m_pFSM.SkinConfig == null ? -15 : -m_pFSM.SkinConfig.maxFallSpeed);
            velocity.y = Mathf.Min(velocity.y, m_pFSM.SkinConfig == null ? 15 : m_pFSM.SkinConfig.maxFallSpeed);
        }
        return velocity;
    }

    bool IsSliding()
    {
        return (grounded && sliding.enabled && TooSteep());
    }
    bool TooSteep()
    {
        return (m_pFSM.groundNormal.y <= Mathf.Cos(m_pFSM.controller.slopeLimit * Mathf.Deg2Rad));
    }
    private Vector3 GetDesiredHorizontalVelocity(Vector3 inputMoveDirection)
    {
        // Find desired velocity
        Vector3 desiredLocalDirection = tr.InverseTransformDirection(inputMoveDirection);
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
        Vector3 sideways = Vector3.Cross(Vector3.up, hVelocity);
        return Vector3.Cross(sideways, groundNormal).normalized * hVelocity.magnitude;
    }

    private bool IsGroundedTest()
    {
        return (m_pFSM.groundNormal.y > 0.01f);
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
        //tr.rotation = Quaternion.Euler(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        //Vector3 angle = new Vector3(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        //tr.eulerAngles = angle;

        Vector3 inputV = new Vector3(sync.fVelocity_x, sync.fVelocity_y, sync.fVelocity_z);
        syncDir = inputV.normalized;

        //Trace.Log("PositionSync Pos:"+m_pFSM.syncPosition.ToString()+"Vel:"+inputV.ToString());

    }

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return currentvelocity;
    }
    public void setVelocity(Vector3 vel)
    {
        currentvelocity = vel;
    }
    public void ApplyAnimation()
    {

    }


    protected void CheckMoveForward()
    {
        if (m_pFSM.moveCtrl.isAutoMoving)
        {
            IsMoveForward = true;
            return;
        }
        Vector3 inputMoveDirection = m_pFSM.inputMoveDirection;
        Vector3 desiredLocalDirection = tr.InverseTransformDirection(inputMoveDirection);

        IsMoveForward = desiredLocalDirection.z < -0.01f ? false : true;
    }  //根据输入判断是否面向移动方向
    bool IsMoveForward
    {
        set { ISMoveForward = value; }
        get { return ISMoveForward; }
    }
    private bool ISMoveForward = false;//是否面向移动方向
    private float InputMoveDirectionDelay = 0;//方向按键停止间隔缓冲


}

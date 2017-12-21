using UnityEngine;
using System.Collections;
using System.Collections.Generic;

using SPELL;
using USpeedUI;

public class SoldierController : MonoBehaviour
{
    // Public variables shown in inspector

    //public float runSpeedFoward = 4.6f;
    //public float runSpeedBack = 4.6f;
    //public float runStrafeSpeed = 3.07f;
    //public float walkSpeedFoward = 1.22f;
    //public float walkSpeedBack = 1.22f;
    //public float walkStrafeSpeed = 1.22f;

    public GameObject radarObject;

    public float maxRotationSpeed = 540;

    //public var weaponSystem : GunManager;
    public float minCarDistance;

    public bool dead;

    public bool showLog = false;

    // Public variables hidden in inspector

    [HideInInspector]
    public bool isMoving;


    [HideInInspector]
    public bool inAir;

    [HideInInspector]
    public bool fire = false;


    [HideInInspector]
    public bool reloading;

    [HideInInspector]
    public bool grounded;

    //[HideInInspector]
    public Vector3 desireAngle = Vector3.zero;

    // Private variables

    private Transform soldierTransform;
    //private CharacterController controller;
    //private var headLookController : HeadLookController;
    //private CharacterMotor motor;
    private CreatureStateMachine m_pFSM;
    public SoldierCamera bindSoldierCamera;

    [HideInInspector]
    public Vector3 inputDir;
    public Vector3 moveDir;

    private int hashid_frontMode;

    ///// <summary>
    ///// 当前角色面向
    ///// </summary>
    //public DirectionType HeroFaceDir
    //{
    //    set
    //    {
    //        curDir = value;
    //    }

    //    get
    //    {
    //        return curDir;
    //    }
    //}

    ///// <summary>
    ///// 角色攻击时是否面向相机前方
    ///// </summary>
    //public bool IsHeroAttackFaceToForward = true;

    /////////////////////////////////////////////////////////////////
    //角色没有状态控制，根据fire属性确定角色当前是攻击状态还是Idle状态

    ////角色当前面向
    //private DirectionType curDir = DirectionType.Direction_Forward;

    /// <summary>
    /// 重置鼠标攻击时间
    /// </summary>
    public float ClearFireStateTime = 2.0f;

    //public enum DirectionType
    //{
    //    Direction_Forward = 0,
    //    Direction_LeftForward = 45,
    //    Direction_Left = 90,
    //    Direction_LeftBackward = 135,
    //    Direction_Backward = 180,
    //    Direction_RightBackward = 225,
    //    Direction_Right = 270,
    //    Direction_RightForward = 315,
    //    ///////////////////////////////
    //    FireDirection_Left = 0,
    //    FireDirection_Right = 0,
    //    FireDirection_LeftBackward = 0,
    //    FireDirection_Backward = 0,
    //    FireDirection_RightBackward = 0
    //}
    /////////////////////////////////////////////////////////////////
    public void Awake()
    {
        hashid_frontMode = Animator.StringToHash("frontMode");
    }

    public void Start()
    {

        soldierTransform = transform;

        isMoving = false;
        reloading = false;
        autoForward = false;
        skillautoForward = false;
        //controller = gameObject.GetComponent<CharacterController>();
        m_pFSM = gameObject.GetComponent<CreatureStateMachine>();
        //motor = gameObject.GetComponent<CharacterMotor>();

        stopMoveBuffList = new List<int>();
        stopRotateBuffList = new List<int>();

        desireAngle = Vector3.zero;
        if (bindSoldierCamera != null)
        {
            desireAngle.y = bindSoldierCamera.userDesireAngle.y;
        }

        unPauseMouseRotateEndTick = 0;

    }

    public void OnEnable()
    {
        if (radarObject != null)
        {
            radarObject.SetActive(true);
        }

        moveDir = Vector3.zero;
        //		headLookController = gameObject.GetComponent("HeadLookController");
        //		headLookController.enabled = true;
        isMoving = false;
        autoForward = false;
        skillautoForward = false;
        reloading = false;
    }

    public void OnDisable()
    {
        if (radarObject != null)
        {
            radarObject.SetActive(true);
        }

        moveDir = Vector3.zero;
        if (m_pFSM)
        {
            m_pFSM.inputMoveDirection = moveDir;
        }
        //headLookController.enabled = false;
        isMoving = true;
        reloading = false;
    }

    public void pauseMouseRotate(int pauseTick)
    {
        bSkillingPauseMouseRotate = true;
        unPauseMouseRotateEndTick = GameLogicAPI.getTickCount()+(uint)pauseTick;
    }

    public bool canMove() //不可移动，行走及转向参考这个
    {
        if (dead || m_bStopMove || bSkillingPauseMove || m_pFSM.bAnimPauseControl || StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
        {
            return false;
        }
        return true;
    }

    public bool canRotate()
    {
        if (dead || bSkillingPauseMoveRotate || bStopRotate || bSkillingPauseRotate || bSkillingPauseMouseRotate || m_pFSM.bAnimPauseControl)
        {
            return false;
        }
        return true;
    }

    public bool isStopMove() //被定身，冲锋和边走边打参考这个
    {
        return m_bStopMove;
    }

    public void Update()
    {
        uint tick = GameLogicAPI.getTickCount();
        if ((bSkillingPauseMoveRotate || bSkillingPauseJump) && tick >= unPauseSkillOperEndTick)
        {
            resumeSkillOperCtrl();
        }

        if (bSkillingPauseMove && tick >= unPauseMovingEndTick)
        {
            bSkillingPauseMove = false;
            bSkillingPauseMoveRotate = false;

            unPauseMovingEndTick = 0;
        }


        if (bSkillingPauseRotate && tick >= unPauseTurningEndTick)
        {
            bSkillingPauseRotate = false;
            unPauseTurningEndTick = 0;
        }

        if (bSkillingPauseMouseRotate && unPauseMouseRotateEndTick > 0 && tick >= unPauseMouseRotateEndTick)
        {
            unPauseMouseRotateEndTick = 0;
            bSkillingPauseMouseRotate = false;
        }

        if (m_resetAxis)
        {
            float ax = InputManager.GetAxis(InputAxisSet.Axis_Horizontal);
            float ay = InputManager.GetAxis(InputAxisSet.Axis_Vertical);
            //检测是否解除m_resetAxis，
            //两种可能，第一种是m_resetAxis开始时记录X轴和Y轴，X轴或Y轴相对于记录值产生了变化，相当于抬起重按移动键
            //第二种可能，就是两个轴都为0，停止移动了，解除m_resetAxis
            if ((Mathf.Abs(ax - m_resetAxisX) > 0.1f || Mathf.Abs(ay - m_resetAxisY) > 0.1f) ||
                (Mathf.Abs(ax) < 0.05f && Mathf.Abs(ay) < 0.05f))
            {
                InputManager.ResetInputAxes();
                m_resetAxis = false;
            }
            else
            {
                moveDir = Vector3.zero;
            }
            isMoving = false;
        }


        Vector3 inputDir = new Vector3(InputManager.GetAxis(InputAxisSet.Axis_Horizontal), 0, InputManager.GetAxis(InputAxisSet.Axis_Vertical));
        inputDir.Normalize();
        if (bindSoldierCamera != null && bindSoldierCamera.CurrentCamera != null && m_pFSM != null)
        {
            Transform tcam = bindSoldierCamera.CurrentCamera.transform;
            m_pFSM.rawInputMoveDirection = tcam.TransformDirection(moveDir);
        }


        GetUserInputs();
        if (!canMove())
        {
            moveDir = Vector3.zero;
        }

        //Check the soldier move direction
        //if (moveDir.sqrMagnitude > 1)
        //    moveDir = moveDir.normalized;



        ////主角暂时添加加速快捷键以便测试
        //if (InputManager.InputManager_GetKey(KeyCode.LeftAlt) || InputManager.InputManager_GetKey(KeyCode.RightAlt))
        //{
        //    moveDir *= 3.0f;
        //}

        //motor.inputMoveDirection = transform.TransformDirection(moveDir);
        //motor.inputJump = InputManager.InputManager_GetButton("Jump") && !dead;		
        //motor.movement.maxForwardSpeed = ((walk) ? walkSpeedFoward:runSpeedFoward);
        //motor.movement.maxBackwardsSpeed = ((walk) ? walkSpeedBack : runSpeedBack);
        //motor.movement.maxSidewaysSpeed = ((walk) ? walkStrafeSpeed : runStrafeSpeed);
        //inAir = !motor.grounded;

        if (m_pFSM != null && !m_pFSM.bPauseMachineUpdate && m_pFSM.GetState()!=GameLogic.EntityState.WallWalking) //stateMachine停止更新，这里也不需要再动了
        {
            if (!bSkillingPauseJump && !m_bStopMove && null != m_pFSM.moveCtrl && !m_pFSM.moveCtrl.isCharge() && null != m_pFSM.animatorCtrl && m_pFSM.animatorCtrl.bDoingAttackMove == false)
            {
                m_pFSM.inputJump = InputManager.GetKey(KeyCode.Space) && !dead;
            }
            inAir = !m_pFSM.IsGrounded();

            if (!m_pFSM.bControlledBySever)
            {
                RotateByCamera();
                UpdateSoldier(0.5f);
            }
        }

        //if (null != bindSoldierCamera)
        //{
        //    bindSoldierCamera.UpdateEx();
        //}
    }

    Vector3 m_recControllerAngle=Vector3.zero;
    public void UpdateSoldier(float alpha)
    {
        if (soldierTransform != null)
        {
            Vector3 oldangle = soldierTransform.eulerAngles;
            Vector3 newangle = desireAngle;
            Vector3 curangle = newangle;
            curangle.y = Mathf.LerpAngle(oldangle.y, newangle.y, alpha);
            curangle.z = Mathf.LerpAngle(oldangle.z, newangle.z, alpha/10.0f);
            if (canMove()) //定身状态下容易来回转
            {
                soldierTransform.eulerAngles = curangle;
            }
        }
    }

    private int m_controlmode;
    public int controlmode
    {
        get
        {
            return m_controlmode;
        }
        set
        {
            MouseCtrl mouseCtrl = transform.GetComponent<MouseCtrl>();
            if (mouseCtrl != null)
            {
                MouseCursorManager.Instance.setCtrlMode(value == 2);
            }
            m_controlmode = value;
            if(isAttackMode())
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_MAINHEROFIGHTSTATE, null);
            }      
        }
    }
    //public int solidermode = 0; //拷贝摄像机的soldermode //2016.12.2摄像机功能重构，solidermode没用过，暂时注释掉
    public bool autorotate = true;
    private ScreenRaycast sr;
    void RotateByCamera()
    {
        if (bindSoldierCamera == null || m_pFSM == null || !m_pFSM.SkinConfig || !bindSoldierCamera.CurrentCamera)
        {
            return;
        }
        Transform tcam = bindSoldierCamera.CurrentCamera.transform;

        m_pFSM.inputMoveDirection = new Vector3(0, 0, 0);
        m_pFSM.rawInputMoveDirection = tcam.TransformDirection(inputDir);

        if (dead == true) //被各种控制着不动角色
        {
            return;
        }

        if (!m_pFSM.SkinConfig.RootAReplace && m_pFSM.animatorCtrl.bDoingAttackMove)//不能移动
        {
            return;
        }

        float mode = controlmode;
        if (m_pFSM.isFloating())
        {
            mode = 3;
        }

        bool freerotate = false;
        MouseCtrl mouseCtrl = MouseCtrl.Instance;
        if (mouseCtrl != null)
        {
            freerotate = SoldierCamera.MainInstance().cameraMode == CameraMode.FPSMode;
        }

        if (m_pFSM.animatorCtrl.ana && m_pFSM.animatorCtrl.ana.isInitialized)
        {
            m_pFSM.animatorCtrl.ana.SetBool(hashid_frontMode, !(mode == 2 || (mode == 1 && freerotate)));
            //if (SoldierCamera.MainInstance<SoldierCamera>().cameraMode == SoldierCamera.CameraControlMode.FPSMode)
            //{
            //    m_pFSM.animatorCtrl.ana.SetBool(hashid_frontMode,false);
            //}
        }

        if (mode==1)
        {
            desireAngle.x = 0.0f;
            desireAngle.z = 0.0f;
            if (moveDir.sqrMagnitude > 0.001f || freerotate)
            {
                Vector3 baseright = tcam.right;
                Vector3 baseup = tcam.up;
                Vector3 baseforward = tcam.forward;

                Vector3 worlddir = (moveDir.x * baseright + moveDir.z * baseforward);
                worlddir.y = 0.0f;
                worlddir = worlddir.normalized;

                if (canRotate()) //bSkillingPauseRotate状态下不随镜头转角度
                {
                    if (freerotate)
                    {
                        desireAngle.y = Mathf.LerpAngle(desireAngle.y, tcam.eulerAngles.y, 0.5f);
                    }
                    else
                    {
                        desireAngle = Vector3.zero;
                        desireAngle.y = Mathf.Atan2(worlddir.x, worlddir.z) / Mathf.PI * 180.0f;
                    }
                }
                m_pFSM.inputMoveDirection = tcam.TransformDirection(moveDir);
               
            }
        }
        else if (mode==2)
        {
            desireAngle.x = 0.0f;
            desireAngle.z = 0.0f;
            if (canRotate())
            {
                if (sr == null) sr = GetComponent<ScreenRaycast>();
                //在做攻击动作的时候不随鼠标转向，此功能于2016年5月10日期屏蔽（陈伍宏）
                //if ((!m_pFSM.animatorCtrl.bAnimatorAttack) && ((sr != null) && (sr.targetObject != gameObject)))                
                if ((sr != null) && (sr.targetObject != gameObject))
                {
                    //mode2的时候选中自己就不转了，不然会原地旋转
                    Vector3 worlddir = sr.targetPos - gameObject.transform.position;
                    if (worlddir != Vector3.zero)
                    {
                        desireAngle = Vector3.zero;
                        desireAngle.y = Mathf.Atan2(worlddir.x, worlddir.z) / Mathf.PI * 180.0f;
                    }
                }
            }
            m_pFSM.inputMoveDirection = tcam.TransformDirection(moveDir);
        }
        else
        {
            desireAngle = tcam.eulerAngles;
            m_pFSM.inputMoveDirection = tcam.TransformDirection(moveDir);
            Vector3 oldVel = m_pFSM.getVelocity();
            Vector3 newVel = m_pFSM.inputMoveDirection * oldVel.magnitude;
            float sinAngle = -Vector3.Dot(newVel, transform.right);
            if (sinAngle > 0.8f) sinAngle = 0.8f;
            if (sinAngle < -0.8f) sinAngle = -0.8f;
            float deltaAngle = Mathf.Asin(sinAngle) / Mathf.PI * 180.0f;
            desireAngle.z = deltaAngle;
        }

    }

    public bool autoForward = false; //自动前进
    public bool skillautoForward = false; //技能导致的自动前进

    void GetUserInputs()
    {

        inputDir = new Vector3(InputManager.GetAxis(InputAxisSet.Axis_Horizontal), 0, InputManager.GetAxis(InputAxisSet.Axis_Vertical));
        inputDir.Normalize();

        if (m_resetAxis)
        {
            moveDir = Vector3.zero;
            isMoving = false;
            return;
        }

        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.AutoForward))
        {
            if (autoForward)
            {
                autoForward = false;
            }
            else if (inputDir.sqrMagnitude < 0.0001f)
            {
                autoForward = true;
            }
        }

        if (inputDir.sqrMagnitude > 0.0001f)
        {
            if (autoForward)
            {
                autoForward = false;
            }
        }

        if (autoForward || skillautoForward)
        {
            moveDir = Vector3.forward;
        }
        else
        {
            moveDir = inputDir;
        }
        //Check if the user wants the soldier to walk
        isMoving = moveDir.sqrMagnitude > 0;


        /*这里导致移动按鼠标攻击时角色不能转动,取消的话，按鼠标左键光标不会隐藏
        //按鼠标左键，相机绕角色旋转
        if (InputManager.InputManager_GetMouseButton(0) && !dead )
        {
            fire = true;
            //移动时间没按左键则，取消fire状态
            if (!IsInvoking("ClearFireStateTimer"))
                Invoke("ClearFireStateTimer", ClearFireStateTime);
            else
            {
                CancelInvoke("ClearFireStateTimer");
                Invoke("ClearFireStateTimer", ClearFireStateTime);
            }
        }
        */
    }

    ////根据按键调整角色面向
    //public void JustDirection()
    //{
    //    if (moveDir.z > 0)
    //        curDir = DirectionType.Direction_Forward;
    //    if (moveDir.x > 0)
    //        curDir = DirectionType.Direction_Left;
    //    if (moveDir.z < 0)
    //        curDir = DirectionType.FireDirection_Backward;
    //    if (moveDir.x < 0)
    //        curDir = DirectionType.Direction_Right;

    //    if (moveDir.x > 0 && moveDir.z > 0)
    //        curDir = DirectionType.Direction_LeftForward;
    //    if (moveDir.x > 0 && moveDir.z < 0)
    //        curDir = DirectionType.FireDirection_LeftBackward;
    //    if (moveDir.x < 0 && moveDir.z < 0)
    //        curDir = DirectionType.FireDirection_RightBackward;
    //    if (moveDir.x < 0 && moveDir.z > 0)
    //        curDir = DirectionType.Direction_RightForward;

    //}

    public bool bSkillingPauseMove = false; //对应技能僵直时间，cmd_creature_castSpellPos的nMoveRigidityTime
    public bool bSkillingPauseSpell = false;
    public bool bSkillingPauseMoveRotate = false; //对应SPELL_CAST_TYPE.SpellCastType_TurnCamera配置
    public bool bSkillingPauseRotate = false; //对应SPELL_CAST_TYPE.SpellCastType_PauseSkillRotate配置
    public bool bSkillingPauseJump = false; //对应SPELL_CAST_TYPE.SpellCastType_Jump配置
    public bool bSkillingPauseMouseRotate = false; //对应指令onCommand_PauseMouseMode2

    private uint unPauseSkillOperEndTick = 0;
    private uint unPauseMovingEndTick = 0;
    private uint unPauseTurningEndTick = 0;
    private uint unPauseMouseRotateEndTick = 0; //为0的时候可以一直保持暂停状态，大于0的时候到此TICK就结束

    public bool m_resetAxis = false; //重设XY轴时，人物停止走动，需要抬起WASD然后再按下才能重新走动
    public float m_resetAxisX = 0.0f;
    public float m_resetAxisY = 0.0f;

    public void resetAxisCtrl(int pauseTick)
    {
        if (showLog)
        {
            uint currentTick = GameLogicAPI.getTickCount();
            Trace.Log(currentTick.ToString() + "resetAxisCtrl tick=" + pauseTick.ToString());
        }
        uint tick = GameLogicAPI.getTickCount();
        unPauseMovingEndTick = tick + (uint)pauseTick;
        bSkillingPauseMove = true;
        m_resetAxis = true;
        autoForward = false;
        m_resetAxisX = InputManager.GetAxis(InputAxisSet.Axis_Horizontal);
        m_resetAxisY = InputManager.GetAxis(InputAxisSet.Axis_Vertical);
    }

    public void pauseCtrl(int pauseTick, int movePauseTick, int turnPauseTick, int nType)
    {
        if (showLog)
        {
            uint currentTick = GameLogicAPI.getTickCount();
            Trace.Log(currentTick.ToString() + "pauseCtrl tick=" + pauseTick.ToString() + " type=" + nType.ToString());
        }
        uint tick = GameLogicAPI.getTickCount();
        if (0 == (nType & ((int)SPELL_CAST_TYPE.SpellCastType_NoCheckRigidity)))
        {
            unPauseSkillOperEndTick = tick + (uint)pauseTick;
            //if (0 == (nType & ((int)SPELL_CAST_TYPE.SpellCastType_Removable))) 技能和移动僵直分开，这个配置取消
            //{
            //    bSkillingPauseMove = true;
            //}

            if (0 == (nType & ((int)SPELL_CAST_TYPE.SpellCastType_Jump)))
            {
                bSkillingPauseJump = true;
            }

            //if (0 != (nType & ((int)SPELL_CAST_TYPE.SpellCastType_PauseSkillRotate))) //用turnPauseTick覆盖，陈伍宏丘演航，2016/5/25
            //{
            //    bSkillingPauseRotate = true;
            //} 

            if (movePauseTick > 0)
            {
                unPauseMovingEndTick = tick + (uint)movePauseTick;
                bSkillingPauseMove = true;
                if (0 == (nType & ((int)SPELL_CAST_TYPE.SpellCastType_TurnCamera)))
                {
                    bSkillingPauseMoveRotate = true;
                }
            }

            if (turnPauseTick > 0)
            {
                unPauseTurningEndTick = tick + (uint)turnPauseTick;
                bSkillingPauseRotate = true;
            }
        }

        if (bSkillingPauseMouseRotate)
        {
            unPauseMouseRotateEndTick = tick + 500;
        }

    }

    public void stopPauseCtrl()
    {
        uint tick = GameLogicAPI.getTickCount();
        unPauseSkillOperEndTick = tick;
        unPauseMovingEndTick = tick;
        unPauseTurningEndTick = tick;
        unPauseMouseRotateEndTick = tick;
    }

    public void resumeSkillOperCtrl()
    {
        if (showLog)
        {
            uint currentTick = GameLogicAPI.getTickCount();
            Trace.Log(currentTick.ToString() + "resumeCtrl");
        }
        bSkillingPauseJump = false;
        unPauseSkillOperEndTick = 0;
    }
    void ClearFireStateTimer()
    {
        fire = false;
    }

    private bool m_bStopMove = false;
    private List<int> stopMoveBuffList;

    public void SetStopMove(bool bStopMove)
    {
        m_bStopMove = bStopMove;
    }

    public void SetStopMove(bool bStopMove, int buffID)
    {
        if (null == stopMoveBuffList)
        {
            return;
        }
        if (bStopMove)
        {
            if (!stopMoveBuffList.Contains(buffID))
            {
                stopMoveBuffList.Add(buffID);
            }
        }
        else
        {
            if (stopMoveBuffList.Contains(buffID))
            {
                stopMoveBuffList.Remove(buffID);
            }
        }

        if (stopMoveBuffList.Count > 0)
        {
            m_bStopMove = true;
        }
        else
        {
            m_bStopMove = false;
        }
    }

    public bool isAttackMode()
    {
        if (!m_pFSM)
        {
            return false;
        }
        if (controlmode == 1 || (controlmode == 2 && bSkillingPauseMouseRotate) || m_pFSM.GetState() == GameLogic.EntityState.Floating)
        {
            return false;
        }
        return true;
    }

    public bool bStopRotate = false;
    private List<int> stopRotateBuffList;
    public void SetStopRotate(bool stopRotate, int buffID)
    {
        if (stopRotate)
        {
            if (!stopRotateBuffList.Contains(buffID))
            {
                stopRotateBuffList.Add(buffID);
            }
        }
        else
        {
            if (stopRotateBuffList.Contains(buffID))
            {
                stopRotateBuffList.Remove(buffID);
            }
        }

        if (stopRotateBuffList.Count > 0)
        {
            bStopRotate = true;
        }
        else
        {
            bStopRotate = false;
        }
    }
}
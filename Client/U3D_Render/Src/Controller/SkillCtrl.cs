using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System;
using GameLogic;
using SPELL;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using System.Linq;
using ASpeedGame.Data.SlotKeyCodeConfig;
using ASpeedGame.Data.PlayerSettingLocalConfig;

public class SkillCtrl : MonoBehaviour
{
    public bool enable = true;

    int maxSlotIndexForEachType = 1000; //一类slot最多有多少个index，用于将index和type合并编码
    const int MAX_SKILL_DISTANCE = 10000;

    // 出招按键,间隔200毫秒采样，sample毫秒不按就重置
    public int longestSampleTick = 200;
    public int shortestSampleTick = 50;
    public int mouseButtonRepeatTick = 50;


    ////在此时间段内按下并抬起鼠标，算是放技能
    //public int mouseButtonSkillTick = 180;

    public bool showLog = false;
    //点击小地图时获取的场景坐标
    public Vector3 MinimapClickPosInScene = Vector3.zero;
    //点击右侧队友头像时玩家UID
    public int TeamRightClickPlayerUID;

    Dictionary<KeyCode, KeyOrder> keySetting = new Dictionary<KeyCode, KeyOrder>();
    Dictionary<string, KeyOrder> keyStringMap = new Dictionary<string, KeyOrder>();
    // 根据string存储keycode，方便取出
    Dictionary<string, KeyCode> KeyCodeStrMap = new Dictionary<string, KeyCode>();

    //槽位和键值信息，改键用
    //List<SlotDateInfo> m_SlotDataInfo = new List<SlotDateInfo>();

    KeyOrder[] keySequence = new KeyOrder[8];
    int nKeyIndex = 0;
    uint nLastKeySampleTick = 0;
    uint cur_tick = 0;
    Dictionary<int, OrderData> ordersetMap = null;
    KeyCode breakSkillOrder = FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.SystemPannel);

    /// <summary>
    /// 活动当前挂接对象上的SoldierController
    /// </summary>
    SoldierController OwnerSC = null;
    MouseCtrl mouseCtrl = null;
    CreatureProperty property = null;
    CreatureStateMachine m_pFSM = null;
    ScreenRaycast raycast = null;

    //蓄力技能专用
    public enum FocoEnergiaState
    {
        None = 0,
        Ready,
        Start,
        End,
    }

    private FocoEnergiaState m_focoEnergiaState;
    #region "初始化"
    // Use this for initialization
    private void setKey()
    {
        // 默认按键设置
        keySetting.Clear();
        keyStringMap.Clear();

        KeySet(KeyOrder.None, KeyCode.None, "None");
        KeySet(KeyOrder.A, KeyCode.A, "a");
        KeySet(KeyOrder.B, KeyCode.B, "b");
        KeySet(KeyOrder.C, KeyCode.C, "c");
        KeySet(KeyOrder.D, KeyCode.D, "d");
        KeySet(KeyOrder.E, KeyCode.E, "e");
        KeySet(KeyOrder.F, KeyCode.F, "f");
        KeySet(KeyOrder.G, KeyCode.G, "g");
        KeySet(KeyOrder.H, KeyCode.H, "h");
        KeySet(KeyOrder.I, KeyCode.I, "i");
        KeySet(KeyOrder.J, KeyCode.J, "j");
        KeySet(KeyOrder.K, KeyCode.K, "k");
        KeySet(KeyOrder.L, KeyCode.L, "l");
        KeySet(KeyOrder.M, KeyCode.M, "m");
        KeySet(KeyOrder.N, KeyCode.N, "n");
        KeySet(KeyOrder.O, KeyCode.O, "o");
        KeySet(KeyOrder.P, KeyCode.P, "p");
        KeySet(KeyOrder.Q, KeyCode.Q, "q");
        KeySet(KeyOrder.R, KeyCode.R, "r");
        KeySet(KeyOrder.S, KeyCode.S, "s");
        KeySet(KeyOrder.T, KeyCode.T, "t");
        KeySet(KeyOrder.U, KeyCode.U, "u");
        KeySet(KeyOrder.V, KeyCode.V, "v");
        KeySet(KeyOrder.W, KeyCode.W, "w");
        KeySet(KeyOrder.X, KeyCode.X, "x");
        KeySet(KeyOrder.Y, KeyCode.Y, "y");
        KeySet(KeyOrder.Z, KeyCode.Z, "z");

        KeySet(KeyOrder.ZERO, KeyCode.Alpha0, "0");
        KeySet(KeyOrder.ONE, KeyCode.Alpha1, "1");
        KeySet(KeyOrder.TWO, KeyCode.Alpha2, "2");
        KeySet(KeyOrder.THREE, KeyCode.Alpha3, "3");
        KeySet(KeyOrder.FOUR, KeyCode.Alpha4, "4");
        KeySet(KeyOrder.FIVE, KeyCode.Alpha5, "5");
        KeySet(KeyOrder.SIX, KeyCode.Alpha6, "6");
        KeySet(KeyOrder.SEVEN, KeyCode.Alpha7, "7");
        KeySet(KeyOrder.EIGHT, KeyCode.Alpha8, "8");
        KeySet(KeyOrder.NINE, KeyCode.Alpha9, "9");
        KeySet(KeyOrder.F1, KeyCode.F1, "f1");
        KeySet(KeyOrder.F2, KeyCode.F2, "f2");
        KeySet(KeyOrder.F3, KeyCode.F3, "f3");
        KeySet(KeyOrder.F4, KeyCode.F4, "f4");
        KeySet(KeyOrder.F5, KeyCode.F5, "f5");
        KeySet(KeyOrder.F6, KeyCode.F6, "f6");
        KeySet(KeyOrder.F7, KeyCode.F7, "f7");
        KeySet(KeyOrder.F8, KeyCode.F8, "f8");
        KeySet(KeyOrder.F9, KeyCode.F9, "f9");
        KeySet(KeyOrder.F10, KeyCode.F10, "f10");
        KeySet(KeyOrder.F11, KeyCode.F11, "f11");
        KeySet(KeyOrder.F12, KeyCode.F12, "f12");
        KeySet(KeyOrder.SPACE, KeyCode.Space, "space");
        KeySet(KeyOrder.ALT, KeyCode.LeftAlt, "alt");
        KeySet(KeyOrder.ALT, KeyCode.RightAlt, "alt");
        KeySet(KeyOrder.ESC, KeyCode.Escape, "esc");
        KeySet(KeyOrder.SHIFT, KeyCode.LeftShift, "shift");
        KeySet(KeyOrder.SHIFT, KeyCode.RightShift, "shift");
        KeySet(KeyOrder.CTRL, KeyCode.LeftControl, "ctrl");
        KeySet(KeyOrder.CTRL, KeyCode.RightControl, "ctrl");
        KeySet(KeyOrder.MOUSELEFT, KeyCode.Mouse0, "mouseleft");
        KeySet(KeyOrder.MOUSERIGHT, KeyCode.Mouse1, "mouseright");
        KeySet(KeyOrder.TAB, KeyCode.Tab, "tab");
        KeySet(KeyOrder.ENTER, KeyCode.KeypadEnter, "enterleft");
        KeySet(KeyOrder.RETURN, KeyCode.Return, "enterright");
        KeySet(KeyOrder.BACKQUOTE, KeyCode.BackQuote, "backquote");
    }

    private void onStartStrafe()
    {
        shortestSampleTick = 0;
        mouseButtonRepeatTick = 0;
    }

    private void onEndStrafe()
    {
        shortestSampleTick = 50;
        mouseButtonRepeatTick = 50;
    }

    // 假如正好是正在扫射的技能槽位有更新 需要停止扫射逻辑
    public void onSpellSlotChange(int nSlotID, int nSpellID)
    {
        if (m_nReportDirectlySlotID > 0 
            && (int)SLOT_TYPE.SLOT_TYPE_SKILL * maxSlotIndexForEachType + nSlotID == m_nReportDirectlySlotID /*&& nSpellID != m_currentSkillID*/)
        {
            m_nReportDirectlySlotID = -1;
            resetSkill();
            onEndStrafe();
        }
    }

    public void Start()
    {
        if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
        {
            if (!GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID))
                enable = false;
        }
        else
            enable = true;

        OwnerSC = transform.GetComponentInChildren<SoldierController>();
        if (OwnerSC == null)
            Trace.LogError("SkillCtrl 获取不到SoldierController!!!");

        if (LightingEffectFactory.Instance.selectEffects != null)
        {
            LightingEffectFactory.Instance.selectEffects.setHeroTransform(transform);
        }

        if (LightingEffectFactory.Instance.selectTips != null)
        {
            LightingEffectFactory.Instance.selectTips.setHeroTransform(transform);
        }


        mouseCtrl = transform.GetComponentInChildren<MouseCtrl>();
        if (mouseCtrl == null)
            Trace.LogError("SkillCtrl 获取不到MouseCtrl!!!");

        m_pFSM = transform.GetComponent<CreatureStateMachine>();
        if (m_pFSM == null)
        {
            Trace.LogError("SkillCtrl 获取不到CreatureStateMachine!!!");
        }

        //mouseCtrl.onTargetSelect += onTargetSelectEvent;

        m_bIsPressingMouse0 = false;
        m_nReportDirectlySlotID = -1;
        onEndStrafe();
        m_focoEnergiaState = FocoEnergiaState.None;
        m_lastStrafeUpdateTick = 0;

        resetRayCast();
        EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);

        SlotKeyCodeConfig.Instance.SetSlotData();
    }

    public void resetRayCast()
    {
        raycast = gameObject.GetComponent<ScreenRaycast>();
        if (raycast == null)
        {
            Trace.LogError("SkillCtrl 获取不到ScreenRaycast!!!");
        }

        if (property == null)
        {
            BaseStateMachine machine = transform.GetComponentInParent<BaseStateMachine>();
            if (machine != null)
            {
                CreatureProperty cp = machine.creaturePropety;
                if (cp != null)
                {
                    property = cp;
                }
            }
        }

        if (property != null && raycast != null && m_lastStrafeUpdateTick == 0)
        {
            //龙母上龙时会切换预制体，切换预制体前已经开始了扫射指令，这时不能关掉重设技能选择目标，因此改成扫射时不setDefaultSelectionParam

            bool resetSkill = true;
            if (m_currentSkillID > 0)
            {
                resetSkill = false;
            }

            raycast.defaultAimDistance = property.aimDistance;

            if (property.selectType > 0)
            {
                raycast.setDefaultSelectionParam(property.selectType, property.targetFilter, property.selectDrawType, property.selectDrawAttackRange, property.selectDrawChooseRange, property.selectDrawTargetType, LightingEffectFactory.Instance.selectEffects, resetSkill);
            }
            else
            {
                raycast.setDefaultSelectionParam(0, 0, 0, 0, 0, 0, null, resetSkill);
            }
        }

    }


    public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {
        if (e.objev == null || !e.objev.IsHero)
            return;

        string strPropertyValue = e.data.nValue.ToString();
        if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_AIM_DISTANCE && raycast != null)
        {
            raycast.defaultAimDistance = e.data.nValue;
        }
    }

    public void OnDestroy()
    {




        EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
    }

    public void OnDisable()
    {
        m_lastStrafeUpdateTick = 0;
        resetSkill();
    }

    /// <summary>
    /// 清除命令表
    /// </summary>
    /// <param name="resetType">清除类型，0.所有 1.技能</param>
    public void resetOrderset(int resetType)
    {
        setKey();
        if (ordersetMap != null)
        {
            if(resetType == 0)
            {
                ordersetMap.Clear();
            }
            else if(resetType == 1)
            {
                List<int> removeList = new List<int>();
                int skillSlotType = (int)SlotType.Skill;

                foreach(int key in ordersetMap.Keys)
                {
                    if (key >= skillSlotType * maxSlotIndexForEachType && key < (skillSlotType + 1) * maxSlotIndexForEachType)
                        removeList.Add(key);
                }

                foreach(int item in removeList)
                {
                    if (ordersetMap.ContainsKey(item))
                        ordersetMap.Remove(item);
                }

                removeList.Clear();
                removeList = null;
            }
        }
        else
        {
            ordersetMap = new Dictionary<int, OrderData>();
        }

    }


    public void enableSlot(int nSlotType, int nSlotIndex, bool enable)
    {
        if (showLog)
        {
            Trace.Log("enableSlot. type=" + nSlotType + " index=" + nSlotIndex + " enable= " + enable);
        }


        int nSlotID = nSlotType * maxSlotIndexForEachType + nSlotIndex;

        if (ordersetMap.ContainsKey(nSlotID))
        {
            ordersetMap[nSlotID].bEnabled = enable;
        }
        else
        {
            Trace.Log("can not find Slot. type=" + nSlotType + " index=" + nSlotIndex);
        }
    }

    //每次需要设置slot的时候（如切换英雄），ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT回调
    public void setSlot(int nSlotType, int nSlotIndex, int nAirCons, int nAdvanceUseType, string strKeySeq,bool isAdd)
    {
        if (showLog)
        {
            Trace.Log("setSlot. type=" + nSlotType + " index=" + nSlotIndex + " str= " + strKeySeq);
        }

        OrderData newData = null;
        //if (strKeySeq == "")
        //{
        //    removeSlot(nSlotType, nSlotIndex);
        //}

        int nSlotID = nSlotType * maxSlotIndexForEachType + nSlotIndex;


        if (ordersetMap.ContainsKey(nSlotID))
        {
            newData = ordersetMap [nSlotID];
            //newData.bEnabled = false;
            newData.nSlotID = nSlotID;
            //newData.keySequence = splitKey(strKeySeq);
            newData.nAirCons = nAirCons;
            if (isAdd)
                newData.nAdvanceUseType = nAdvanceUseType;      //技能自带的推荐usetype
        }
        else
        {
            newData = new OrderData();
            //newData.bEnabled = false;
            newData.nSlotID = nSlotID;
            //newData.keySequence = splitKey(strKeySeq);
            newData.nAirCons = nAirCons;
            if (isAdd)
                newData.nAdvanceUseType = nAdvanceUseType;      //技能自带的推荐usetype
            //if (newData.keySequence != null)
            //{
            //    newData.bEnabled = true;
            //}
            ordersetMap.Add(nSlotID, newData);
        }
    }

    public OrderData GetCurOrderData(int slotID)
    {
        if (ordersetMap.ContainsKey(slotID))
            return ordersetMap[slotID];
        else
            return null;
    }
    public void SetSlotKeyCode(int nSlotType, int nSlotIndex, int nReleaseCondition, string strKeySeq)
    {
        if (showLog)
        {
            Trace.Log("setSlotKeyCode. type=" + nSlotType + " index=" + nSlotIndex + " str= " + strKeySeq);
        }
        OrderData newData = null;
        if (strKeySeq == "")
        {
            removeSlot(nSlotType, nSlotIndex);
        }

        int nSlotID = nSlotType * maxSlotIndexForEachType + nSlotIndex;

        if (ordersetMap.ContainsKey(nSlotID))
        {
            newData = ordersetMap[nSlotID];
        }
        else
        {
            newData = new OrderData();
            newData.nAirCons = nReleaseCondition;
            ordersetMap.Add(nSlotID, newData);
        }
        newData.bEnabled = false;
        newData.nSlotID = nSlotID;
        newData.keySequence = splitKey(strKeySeq);
        if (newData.keySequence != null)
        {
            newData.bEnabled = true;
        }
    }

    public void removeSlot(int nSlotType, int nSlotIndex)
    {
        int nSlotID = nSlotType * maxSlotIndexForEachType + nSlotIndex;
        if (ordersetMap.ContainsKey(nSlotID))
        {
            ordersetMap.Remove(nSlotID);
        }
    }

    private KeyOrder[] splitKey(string strKeySeq)
    {
        KeyOrder[] order = null;

        string[] strSeq = strKeySeq.Split(';');
        if (strSeq.Length > 0)
        {
            order = new KeyOrder[strSeq.Length];
            for (int i = 0; i < strSeq.Length; i++)
            {
                string key = strSeq[i];
                if (keyStringMap.ContainsKey(key))
                {
                    order[i] = keyStringMap[key];
                }
                else
                {
                    order[i] = KeyOrder.None;
                }
            }
        }

        return order;

    }

    public void resetSkill()
    {
        if (showLog)
        {
            uint tick = GameLogicAPI.getTickCount();
            Trace.Log(tick.ToString() + "resetSkill");
        }

        if (m_lastStrafeUpdateTick > 0) //resetSkill时，发送ENTITY_CMD_CANCEL_CAST无法停止扫射，因此改成扫射时不resetSkill
        {
            return;
        }


        m_currentSlotID = -1;
        if (m_selectSkillID > 0)
        {
            cmd_creature_cancel_cast data = new cmd_creature_cancel_cast();
            data.nSpellID = m_selectSkillID;
            if (showLog)
            {
                Trace.Log("Cancel Skill");
            }
            EntityEventHelper.Instance.SendCommand<cmd_creature_cancel_cast>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_CANCEL_CAST, ref data);
        }
        m_selectSkillID = -1;
        m_currentSkillID = -1;
        m_targetObjectID = -1;
        m_selectSlotID = -1;
        m_targetPos = Vector3.zero;
        m_bCanNotUseOtherSkill = false;
        if (LightingEffectFactory.Instance.selectEffects != null)
        {
            LightingEffectFactory.Instance.selectEffects.disableEffect();
        }
        if (raycast)
        {
            raycast.SetDefaultSelection();
        }

        if (doingPKSelection)
        {
            doingPKSelection = false;
            doingPKSelection = true;
        }

    }
    #endregion

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    public void Update()
    {
        if (!enable)
        {
            return;
        }


        if (raycast == null)
        {
            resetRayCast();
            return;
        }

        cur_tick = GameLogicAPI.getTickCount();

        if (m_closeSelectTipsTick > 0)
        {
            if (cur_tick > m_closeSelectTipsTick)
            {
                m_closeSelectTipsTick = 0;
                LightingEffectFactory.Instance.selectTips.disableEffect();
            }
        }

        if (property == null)
        {
            property = gameObject.GetComponent<CreatureProperty>();
        }


        ////Update NoAttack Selection
        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.SelectNPCTarget))
        {
            //检查是否选中了NPC
            if (raycast.targetID > 0)
            {
                U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(raycast.targetID);
                if (ev != null)
                {
                    if (ev.StateMachine != null && ev.StateMachine.isNotAttack)
                    {
                        //cmd_Select_Special_Entity data = new cmd_Select_Special_Entity();
                        //data.uidOperator = (uint)m_pFSM.entityID;
                        //data.uidTarget = (uint)raycast.targetID;
                        //data.nFunctionType = 1;
                        //EntityEventHelper.Instance.SendCommand<cmd_Select_Special_Entity>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SPECIAL_SELECT, ref data);

                        cmd_entity_npc_trigger cmd;
                        cmd.uidNPC = (uint)raycast.targetID;
                        EntityEventHelper.Instance.SendCommand<cmd_entity_npc_trigger>(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_NPC_TRIGGER, ref cmd);
                        return;
                    }
                }
            }
        }

        //if (m_pFSM.GetState() == EntityState.Floating)
        //{
        //    return; //翅膀飞行不放技能
        //}

        // 按键采样
        if (KeySample() == true)
        {
            // 撮招系统
            int slotID = TestOrder(keySequence, nKeyIndex);
            if (slotID > 0)
            {
                nKeyIndex = 0;
                //调整角色面向
                //if (OwnerSC.IsHeroAttackFaceToForward)
                //{
                //    OwnerSC.HeroFaceDir = SoldierController.DirectionType.Direction_Forward;
                //}
                //Trace.Log("SEND KEY DOWN: " + GameLogicAPI.getTickCount());
                Press_Skill(slotID);
                doingPKSelection = false;
            }
            else if (slotID == -1)
            {
                //复位
                keySequence[0] = keySequence[nKeyIndex - 1];
                nKeyIndex = 1;
                nLastKeySampleTick = cur_tick;
                //return;
                doingPKSelection = false;
            }
        }

        straftUpdate(); //如果正在扫射的话，更新扫射效果

        if (keySetting.ContainsKey(breakSkillOrder))
        {
            if (InputManager.GetKeyDown(breakSkillOrder))
            {
                if (showLog)
                {
                    Trace.Log("Break Skill");
                }

                cmd_creature_cancel_cast data = new cmd_creature_cancel_cast();
                data.nSpellID = m_currentSkillID;
                EntityEventHelper.Instance.SendCommand<cmd_creature_cancel_cast>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_CANCEL_CAST, ref data);

                return;
            }
        }

        //正在蓄力的话，其他技能无法释放，直到蓄力释放或取消
        if (m_focoEnergiaState == FocoEnergiaState.Ready)
        {
            if (InputManager.GetKeyDown(KeyCode.Mouse0))
            {
                focoEnergiaStart();
            }
            else if (InputManager.GetKeyDown(KeyCode.Mouse1))
            {
                mouseCtrl.bPickingMovePoint = false;
                focoEnergiaCancel();
            }
            return;
        }


        if (OwnerSC != null && OwnerSC.bSkillingPauseSpell)
        {
            return;
        }
        bool isAdvance = GetSkillUseType(m_currentSlotID) == SpellMode.ST_Smart;
        if (isAdvance)   //按住技能键不松手不会释放，会显示描边和释放区域，松开按键释放
        {
            if (m_selectSkillID > 0 && keyPressType == KeyPressType.loose && !InputManager.GetKeyUp(KeyCode.Mouse0))
            {
                if (InputManager.GetKeyUp(KeyCode.Mouse1))
                {
                    mouseCtrl.bPickingMovePoint = false;
                    onTargetSelectEvent(false);
                }
                else
                    onTargetSelectEvent(true);
            }
            else if (InputManager.GetKeyUp(KeyCode.Mouse0) && keyPressType != KeyPressType.press2Sec) //点击鼠标立刻放
            {
                onTargetSelectEvent(true);
            }
            else
            {
                m_targetObjectID = raycast.targetID;
                m_targetPos = raycast.targetPos;
                LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
            }
        }
        //else if (m_selectSkillID > 0 && (m_nSelectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_SkillUseDelay)) != 0)
        //{
        //    if (keyPressType == KeyPressType.loose && !InputManager.GetKeyUp(KeyCode.Mouse0))    //松开立刻放
        //    {
        //        if(InputManager.GetKeyUp(KeyCode.Mouse1))
        //        {
        //            mouseCtrl.bPickingMovePoint = false;
        //            onTargetSelectEvent(false);
        //        }
        //        else
        //            onTargetSelectEvent(true);
        //    }
        //    else if (keyPressType == KeyPressType.press2Sec)//超过两秒
        //    {
        //        onTargetSelectEvent(true);
        //    }
        //    else if (InputManager.GetKeyUp(KeyCode.Mouse0) && keyPressType!= KeyPressType.press2Sec) //点击鼠标立刻放
        //    {
        //        onTargetSelectEvent(true);
        //    }
        //    else
        //    {
        //        m_targetObjectID = raycast.targetID;
        //        m_targetPos = raycast.targetPos;
        //        LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
        //    }
        //}
        else  //正常的按一次
        {
            if (m_selectSkillID > 0 || m_currentSlotID == 0) //已经触发了某技能，正在选择释放目标
            {
                if (InputManager.GetKeyUp(KeyCode.Mouse0))
                {
                    bool succeed = true;
                    if (m_bPickOnMinimap && MinimapClickPosInScene.sqrMagnitude > 0.001f)
                    {
                        m_bPickOnMinimap = false;
                        m_targetPos = MinimapClickPosInScene;
                        //二维地图没有Y值，先抬高然后向下投影
                        m_targetPos.y = 0;
                        if (m_selectDistance <= MAX_SKILL_DISTANCE)
                        {
                            Vector3 tempPos = new Vector3(this.transform.position.x, 0, this.transform.position.z);
                            float skillDis = Vector3.Distance(m_targetPos, tempPos);
                            skillDis = skillDis >= m_selectDistance ? m_selectDistance : skillDis;          //技能施法距离控制
                            m_targetPos = tempPos + Vector3.Normalize(m_targetPos - tempPos) * skillDis;
                        }
                        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
                        Ray groundRay = new Ray(m_targetPos + new Vector3(0, 50.0f, 0), new Vector3(0, -1, 0));
                        RaycastHit hitGroundInfo;
                        bool hitGround = Physics.Raycast(groundRay, out hitGroundInfo, 100.0f, LayerMaskOnGround);
                        if (hitGround)
                        {
                            m_targetPos = hitGroundInfo.point;
                        }
                    }
                    else
                    {
                        if (TeamRightClickPlayerUID > 0) //选择友方头像
                        {
                            if (raycast.onClickPlayer(TeamRightClickPlayerUID))
                            {
                                m_targetObjectID = raycast.targetID;
                                m_targetPos = raycast.targetPos;
                            }
                            else
                            {
                                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_SKILL_NULL_TARGET);
                                succeed = false;
                            }
                        }
                    }
                    if (succeed)
                    {
                        onTargetSelectEvent(true);
                    }
                }
                else if (m_unFocoEnergiaTime > 0 && m_unFocoEnergiaTime < cur_tick)
                {
                    onTargetSelectEvent(true);
                }
                else if (InputManager.GetKeyUp(KeyCode.Mouse1))
                {
                    mouseCtrl.bPickingMovePoint = false;
                    onTargetSelectEvent(false);
                }
                else
                {
                    m_targetObjectID = raycast.targetID;
                    m_targetPos = raycast.targetPos;
                     LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
                }
            }
        }

        //显示技能范围提示
        Vector3 faceTargetPos = transform.position + transform.forward;
        LightingEffectFactory.Instance.selectTips.updateEffect(faceTargetPos);

        UpdatePKSelection();
    }

    #region "搓招按键匹配"
    // 设置键位
    public void KeySet(KeyOrder k, KeyCode c, string key)
    {
        if (keySetting.ContainsKey(c))
            keySetting.Remove(c);

        keySetting.Add(c, k);

        if (keyStringMap.ContainsKey(key))
            keyStringMap.Remove(key);

        keyStringMap.Add(key, k);

        if (KeyCodeStrMap.ContainsKey(key))
        {
            KeyCodeStrMap.Remove(key);
        }
        KeyCodeStrMap.Add(key, c);

    }

    // 测试按键

    private uint un_lastMouseDownTick = 0;
    private bool m_bIsPressingMouse0 = false;
    private bool LastHitUI = false;
    public uint mouseRightSkillTick = 500; //在此时间内按下并抬起右键，算放技能
    private Vector2 LastMousePos = new Vector2(0, 0);
    //蓄力技能专用
    public enum KeyPressType
    {
        press = 0,    //一直按
        loose,          //松开
        press2Sec    //一直按超过两秒
    }

    private KeyPressType keyPressType = KeyPressType.press;      //0:一直按 1：松开 2：一直按超过两秒
    private float keyPressDownTime = 0.0f;

    public KeyOrder TestKeyDown()
    {
        KeyOrder order = KeyOrder.None;

        if (InputManager.GetMouseButtonUp(0))
        {
            if (m_mouseCancelSkilID > 0)
            {
                onTargetSelectEvent(false);
                m_mouseCancelSkilID = -1;
            }
            m_bIsPressingMouse0 = false;
            if (m_nReportDirectlySlotID > 0)
            {
                m_nReportDirectlySlotID = -1;
                resetSkill();
                onEndStrafe();
            }
        }

        //if (InputManager.GetMouseButtonDown(0) && raycast.targetID>0)
        //{
        //    U3D_Render.EntityView entityView=EntityFactory.getEntityViewByID(raycast.targetID);
        //    if ((entityView != null) && (entityView.Type == ENTITY_TYPE.TYPE_MONSTER) && ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_NPC == entityView.Flag))
        //    {
        //        cmd_entity_npc_trigger cmd;
        //        cmd.uidNPC = (uint)raycast.targetID;
        //        EntityEventHelper.Instance.SendCommand<cmd_entity_npc_trigger>(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_NPC_TRIGGER, ref cmd);
        //        return KeyOrder.None;
        //    }            
        //}


        foreach (KeyValuePair<KeyCode, KeyOrder> item in keySetting)
        {
            if (item.Key == KeyCode.Mouse0) //鼠标左键单独处理，长按鼠标左键不停攻击
            {
                if (mouseCtrl.canSkillCtrl()) //只有角色控制的时候鼠标左右键才作为放技能的快捷键
                {
                    if ((m_bIsPressingMouse0) && (InputManager.GetKey(item.Key)))
                    {
                        if (cur_tick - un_lastMouseDownTick > mouseButtonRepeatTick)
                        {
                            un_lastMouseDownTick = cur_tick;
                            order = item.Value;
                            //return order; 这里先不return,其他按键优先return
                        }
                    }
                    else if (InputManager.GetMouseButtonDown(0))
                    {
                        bool isMouseOverUI = false;
                        Vector2 CurMousePos = UISystem.Instance.GetMousePos();

                        Vector2 DeltaMousePos = CurMousePos - LastMousePos;
                        if (DeltaMousePos.sqrMagnitude > 0.01f)
                        {
                            LastMousePos = CurMousePos;
                            LastHitUI = UISystem.Instance.IsMouseOverUI();
                        }
                        isMouseOverUI = LastHitUI;
                        if (!isMouseOverUI)
                        {
                            m_bIsPressingMouse0 = true;
                            un_lastMouseDownTick = cur_tick;
                            order = item.Value;
                            //return order; 这里先不return,其他按键优先return
                        }
                        else
                        {
                            if (m_mouseCancelSkilID > 0)
                            {
                                onTargetSelectEvent(false);
                                m_mouseCancelSkilID = -1;
                            }
                            m_bIsPressingMouse0 = false;
                            if (m_nReportDirectlySlotID > 0)
                            {
                                m_nReportDirectlySlotID = -1;
                                resetSkill();
                                onEndStrafe();
                            }
                        }
                        //else
                        //{
                        //    if (showLog)
                        //    {
                        //        Trace.Log("Click on UI！" + movie.MovieName);
                        //    }
                        //}
                    }
                    else
                    {
                        if (m_mouseCancelSkilID > 0)
                        {
                            onTargetSelectEvent(false);
                            m_mouseCancelSkilID = -1;
                        }
                        m_bIsPressingMouse0 = false;
                        if (m_nReportDirectlySlotID > 0)
                        {
                            m_nReportDirectlySlotID = -1;
                            resetSkill();
                            onEndStrafe();
                        }
                    }
                }
            }
            ////右键攻击功能屏蔽，右键用于寻路(2016-06-21, required by 董哲宇)
            //else if (item.Key == KeyCode.Mouse1) //鼠标右键攻击
            //{
            //    if (mouseCtrl.canSkillCtrl() && m_selectSkillID<=0)
            //    {
            //        //只有角色控制的时候鼠标左右键才作为放技能的快捷键
            //        //有等待左键确认的技能，右键作为取消用，不攻击
            //        if ((InputManager.GetMouseButtonUp(1)) && (cur_tick - un_lastMouseRightDownTick < mouseRightSkillTick))
            //        {
            //            un_lastMouseRightDownTick = 0;
            //            order = item.Value;
            //            return order;
            //        }
            //        else if (InputManager.GetMouseButtonDown(1))
            //        {

            //            un_lastMouseRightDownTick = cur_tick;
            //        }
            //    }
            //}
            else
            {
                KeyOrder temp = item.Value;
				// 放技能时加上判断是否按下了Ctrl键,不然与激活天赋冲突
                if (InputManager.GetKeyDown(item.Key) && !InputManager.GetKey(KeyCode.LeftControl))
                {
                    keyPressDownTime = Time.time;
                    order = item.Value;
                    keyPressType = KeyPressType.press;
                    return order;
                }
                if (InputManager.GetKeyUp(item.Key))
                {
                    keyPressDownTime = 0;
                    //order = item.Value;
                    keyPressType = KeyPressType.loose;
                    //return order;
                }
                else if (keyPressDownTime > 0 && Time.time - keyPressDownTime > 2) //一直按，超过两秒
                {
                    keyPressDownTime = 0;
                    order = temp;
                    keyPressType = KeyPressType.press2Sec;
                }
            }
        }

        return order;
    }

    public bool KeySample()
    {
        KeyOrder code = TestKeyDown();
        if (code == KeyOrder.MOUSELEFT)
        {
            //左键单独处理，不列入队列
            //因为鼠标左键按住不放算是连续攻击，鼠标左键按住不放再按WW键，应该是触发“WW”翻滚，而不是输出“W鼠标W”
            //正在PK选人的时候也不记鼠标左键，PK选人的时候左键是用于确认选择对象
            if (m_currentSkillID <= 0 && m_selectSkillID <= 0 && m_currentSlotID != 0)
            {
                KeyOrder[] mouseKey = new KeyOrder[1];
                mouseKey[0] = KeyOrder.MOUSELEFT;
                int slotID = TestOrder(mouseKey, 1);
                if (slotID > 0)
                {
                    Press_Skill(slotID);
                }
            }
            else if (m_nReportDirectlySlotID > 0 && m_bIsPressingMouse0) //目前还有m_bReportDirectly在，可能需要直接上报
            {
                KeyOrder[] mouseKey = new KeyOrder[1];
                mouseKey[0] = KeyOrder.MOUSELEFT;
                int slotID = TestOrder(mouseKey, 1);
                if (slotID == m_nReportDirectlySlotID)
                {
                    m_targetObjectID = raycast.targetID;
                    m_targetPos = raycast.targetPos;
                    reportSkill();
                    //if (m_pFSM && m_pFSM.isHero) //显示层先行，转向目标方向 2017.05.02 FPS模式指向准心，不指向目标
                    //{
                    //    m_pFSM.FaceTo(m_targetPos);
                    //}
                }
            }

            return false;
        }

        //部分特殊情况下，需要过滤一些按键
        if (FilterKeyCode(code))
        {
            return false;
        }

        if (code == KeyOrder.None)
        {
            if (cur_tick - nLastKeySampleTick > longestSampleTick) nKeyIndex = 0;
            return false;
        }

        // 超过sample复位
        if (cur_tick - nLastKeySampleTick > longestSampleTick)
        {
            nKeyIndex = 1;
            keySequence[0] = code;
            nLastKeySampleTick = cur_tick;
            return true;
        }

        // 按得太快，复位
        if (cur_tick - nLastKeySampleTick < shortestSampleTick)
        {
            nKeyIndex = 1;
            keySequence[0] = code;
            nLastKeySampleTick = cur_tick;
            return true;
        }

        //while( cur_tick-nLastKeySampleTick>300 )
        //{
        //    // 超过8个按键重置
        //    if (nKeyIndex >= keySequence.Length)
        //        nKeyIndex = 0;

        //    // 中间的按键空隙填入0
        //    keySequence[nKeyIndex] = KeyOrder.None;
        //    nLastKeySampleTick += 300;
        //    nKeyIndex += 1;
        //}

        // 超过8个按键重置
        if (nKeyIndex >= keySequence.Length)
            nKeyIndex = 0;

        keySequence[nKeyIndex] = code;
        nLastKeySampleTick = cur_tick;
        nKeyIndex += 1;
        return true;

    }

    // 根据按键触发不同的技能,返回一个技能ID
    // 测试按键是否匹配
    // 返回1表示完全匹配
    // 返回0表示目前仍然匹配，还可以继续按键
    // 返回-1表示完全不匹配
    public int TestOrder(KeyOrder[] keySequence, int len)
    {

        if (ordersetMap == null)
        {
            return 0;
        }

        bool allFail = true;

        foreach (OrderData order in ordersetMap.Values)
        {
            if (order.bEnabled)
            {
                if (!airConTest(order.nAirCons))
                {
                    continue;
                }

                int testRes = order.Test(keySequence, len);

                if (testRes == 1)
                {
                    return order.nSlotID;
                }
                else if (testRes == 0)
                {
                    allFail = false;
                }
            }
        }

        if (allFail)
        {
            return -1;
        }

        return 0;
    }

    //释放条件(0：不限制，1：跳跃可释放，2：在地面，4：在空中可释放，8：轻功释放)
    private bool airConTest(int nAirCons)
    {
        if (nAirCons <= 0) return true;

        if (((nAirCons & 2) != 0) && (m_pFSM.getEntityState() == EntityState.Standing))
        {
            return true;
        }
        else if (((nAirCons & 1) != 0) && (m_pFSM.getEntityState() == EntityState.Jumping))
        {
            return true;
        }
        else if (((nAirCons & 4) != 0) && (m_pFSM.getEntityState() == EntityState.Floating))
        {
            return true;
        }
        else if (((nAirCons & 8) != 0) && (m_pFSM.dodge == true))
        {
            Trace.Log("nAirCons m_pFSM.dodge==true");
            return true;
        }


        return false;
    }
    
    //检测按键过滤
    private bool FilterKeyCode(KeyOrder order)
    {
        if(order == KeyOrder.SPACE)
        {
            if (SoldierCamera.MainInstance().cameraMode == CameraMode.MapGodControl)
                return true;
        }
        return false;
    }
    #endregion

    #region "按键与UI界面槽位匹配"
    public int GetSkillIDThroughSlot(int slotID)
    {
        int skillID = -1;
        //foreach (OrderData order in orderset)
        //{
        //    if (order.nSkillSlot == slotID)
        //    {
        //        skillID = order.nSkillID;
        //        break;
        //    }
        //}
        return skillID;
    }

    public int GetSlotIDThroughSkill(int skillID)
    {
        int slotID = -1;
        //foreach (OrderData order in orderset)
        //{
        //    if (order.nSkillID == skillID)
        //    {
        //        slotID = order.nSkillSlot;
        //        break;
        //    }
        //}
        return slotID;
    }

    public KeyCode GetKeyCodeByKeyString(ref string keyStr)
    {
        if (KeyCodeStrMap.ContainsKey(keyStr))
        {
            return KeyCodeStrMap[keyStr];
        }
        else
        {
            return KeyCode.None;
        }
    }

    public bool GetKeyStatus(string keyStr, KeyStatus status)
    {
        bool res = false;
        KeyOrder[] orders = splitKey(keyStr);
        List<KeyValuePair<KeyCode, KeyOrder>> tmpList;
        for (int i = 0; i < orders.Length; i++)
        {
            if (!keySetting.ContainsValue(orders[i]))
                continue;

            tmpList = keySetting.Where((pair) =>
            {
                if (pair.Value == orders[i])
                    return true;
                return false;
            }).ToList();

            for (int j = 0; j < tmpList.Count; j++)
            {
                if (status == KeyStatus.Down && InputManager.GetKey(tmpList[j].Key))
                    return true;
                else if (status == KeyStatus.Up && !InputManager.GetKey(tmpList[j].Key))
                    return true;
                else if (status == KeyStatus.Pressing && InputManager.GetKey(tmpList[j].Key))
                    return true;
            }
        }

        return res;
    }

    public void UseSkillThroughSlot(int slotID)
    {
        if (slotID >= 0)
        {
            Press_Skill(slotID);
        }
    }

    public void SetSkillIDThroughSlot(int slotID, int newSkillID)
    {
        //foreach (OrderData order in orderset)
        //{
        //    if (order.nSkillSlot == slotID)
        //    {
        //        order.nSkillID = newSkillID;
        //        break;
        //    }
        //}
    }

    #endregion

    #region "通用技能从按键到触发的流程处理"
    ///--------------------------------------------------------------------------------------------------------
    ///触发技能。一般是快捷键或者鼠标点快捷槽位之后，调用此函数
    ///此函数负责向逻辑层提问，得到技能相对应的属性后再接着选释放对象和触发
    ///--------------------------------------------------------------------------------------------------------
    ///
    private int m_currentSkillID = -1;
    private int m_selectSkillID = -1; //需要鼠标左键确认的技能ID另外记录，避免先按此技能键，然后上个普攻把m_currentSkillID刷新成-1了
    private int m_currentSlotID = -1; //0表示正在PK选人
    public int Press_Skill(int nSlotType, int nSlotID)
    {
        int nID = nSlotType * maxSlotIndexForEachType + nSlotID;

        if (ordersetMap.ContainsKey(nID))
        {
            if (ordersetMap[nID].bEnabled)
            {
                return Press_Skill(nID);
            }
        }
        return 0;
    }

    public bool isSelecting()
    {
        return (m_selectSkillID > 0 || m_currentSkillID > 0);
    }

    private int m_selectSlotID = -1; //正在处理的技能快捷键，用于处理第一次开始选择，再次按技能快捷键释放的技能
    public int Press_Skill(int nSlotID)
    {
        uint currentTick = GameLogicAPI.getTickCount();
        if (showLog)
        {
            Trace.Log(currentTick.ToString() + "PressSlot " + nSlotID);
        }

        //if (skillID == 101)
        //{
        //    //101目前强制设定为上下载具
        //    enterExitTank();
        //    return 0;
        //}

        //处理第一次开始选择，再次按技能快捷键释放的技能
        if (m_selectSkillID > 0 && m_bKeyConfirm && m_selectSlotID == nSlotID)
        {
            onTargetSelectEvent(true);
            return 0;
        }

        if ((m_currentSkillID > 0 || m_selectSkillID > 0) && m_bCanNotUseOtherSkill)
        {
            return 0;
        }

        m_selectSlotID = nSlotID;
        if (property != null)
        {
            cmd_creature_spellAttribute data = new cmd_creature_spellAttribute();
            //data.nID = nSlotID;
            data.nSlotType = (SLOT_TYPE)((int)nSlotID / (int)maxSlotIndexForEachType);
            data.nSlotIndex = nSlotID % maxSlotIndexForEachType;
            //Debug.Log(nSlotID);
            EntityEventHelper.Instance.SendCommand<cmd_creature_spellAttribute>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_ASKSPELLATTRIBUTE, ref data);
        }

        raycast.setSkillLock(true);

        return 0;
    }

    ///--------------------------------------------------------------------------------------------------------
    ///设置技能属性。
    ///当逻辑层答复技能属性后，调用此函数，由此函数开始调用MouseCtrl选择范围
    ///--------------------------------------------------------------------------------------------------------
    ///
    private int m_targetObjectID = -1;
    private Vector3 m_targetPos = Vector3.zero;
    private bool m_bBlockTest = false;
    private bool m_bKeyConfirm = false;
    private bool m_bCanNotUseOtherSkill = false;
    private uint m_unFocoEnergiaTime = 0; //蓄力计时，到时间后取消
    public bool m_bPickOnMinimap = false;
    private bool m_bRotateCameraToTarget = false; //将镜头转向目标
    private bool m_bIsStrafe = false; //是否扫射
    private int m_mouseCancelSkilID = 0; //非0的时候鼠标左键不连续按着的时候，发送取消技能指令
    private int m_spellFlag = 0;
    private int m_nRollSpellStartID = 0; //上报时参考的ID，因为翻滚动作得到的SkillID不等于上报时的skillID
    private int m_nSelectType = 0; //onSetSpellAttribute时的参数，存起来reportSkill要用到
    public int m_nKeyPressType = 1;        //按键施法类型
    private float m_selectDistance = 0;
    private int m_nReportDirectlySlotID = -1; //FPS特殊普攻，为true的时候按住左键不放直接reportSkill，而不经过服务器校验，这里记录槽位，避免封魔时其他功能的槽位也上报
    public void onSetSpellAttribute(cmd_spell_attribute param)
    {
        ////sc.onSetSpellAttribute(data.nAttributeType,data.nSelectType, data.fDistance, data.fAttackRange, data.fHeight);
        int success = param.nAttributeType;
        float selectDistance = param.fDistance;
        float attackRange = param.fAttackRange;
        float traceHeight = param.fHeight;

        uint currentTick = GameLogicAPI.getTickCount();
        if (showLog)
        {
            Trace.Log(currentTick.ToString() + "nDefaultAimFlag=" + param.nDefaultAimFlag + ". onSetSpellAttribute type=" + param.nSelectType.ToString() + ". selectDistance=" + selectDistance.ToString() + ". attackRange=" + attackRange.ToString());
        }

        if (param.nDefaultAimFlag != 0)
        {
            bool resetSkill = true;
            if (m_currentSkillID > 0)
            {
                resetSkill = false;
            }
            if (param.nDefaultAimFlag == 1)
            {
                raycast.defaultAimDistance = selectDistance;
                raycast.setDefaultSelectionParam(param.nSelectType, param.nTargetType, param.nDrawType, attackRange, param.fChooseRange, param.nTargetType, LightingEffectFactory.Instance.selectEffects, resetSkill);
            }
            else
            {
                raycast.defaultAimDistance = property.aimDistance;
                raycast.setDefaultSelectionParam(0, 0, 0, 0, 0, 0, null, resetSkill);
            }

            return;
        }



        if (param.nAttributeType <= 0)
        {
            if (showLog)
            {
                Trace.Log("onSetSpellAttribute nAttributeType=false");
            }

            if (m_currentSkillID <= 0)
            {
                raycast.SetDefaultSelection();
                resetSkill();
            }
            return;
        }

        if (mouseCtrl != null)
        {
            mouseCtrl.switchToState(MouseState.HeroControl);
        }

        m_nSelectType = param.nSelectType;
        m_spellFlag = param.nSpellFlag;
        m_currentSkillID = param.nSpellID;
        m_currentSlotID = (int)param.nSlotType * maxSlotIndexForEachType + param.nSlotIndex;
        m_nRollSpellStartID = param.nRollSpellStartID;
        if ((param.nCastType & (int)SPELL_CAST_TYPE.SpellCastType_Pathing) != 0) //可以寻路放技能的话，瞄准距离和技能距离取大者，这样可以瞄准后寻路过去放
        {
            if (selectDistance < raycast.defaultAimDistance)
            {
                selectDistance = raycast.defaultAimDistance;
            }
        }

        m_bBlockTest = false;
        if ((param.nCastType & (int)SPELL_CAST_TYPE.SpellCastType_CheckHinder) != 0)
        {
            m_bBlockTest = true;
        }
        MinimapClickPosInScene = Vector3.zero; //小地图取点，清零
        TeamRightClickPlayerUID = 0; //英雄取点，清零

        m_bPickOnMinimap = false;
        if ((m_nSelectType & (int)SPELL_SELECT_TYPE.SpellSelectType_MiniMapPoint) != 0)
        {
            m_bPickOnMinimap = true;
        }

        m_bRotateCameraToTarget = false;
        if ((param.nCastType & (int)SPELL_CAST_TYPE.SpellCastType_RotateCameraToTarget) != 0)
        {
            m_bRotateCameraToTarget = true;
        }

        m_bIsStrafe = false;
        if ((param.nCastFlag & (int)SPELL_CAST_FLAG.SpellCastFlag_StarfeState) != 0)
        {
            m_bIsStrafe = true;
        }

        m_mouseCancelSkilID = -1;
        if ((param.nCastFlag & (int)SPELL_CAST_FLAG.SpellCastType_MouseUpCancelSpell) != 0)
        {
            m_mouseCancelSkilID = m_currentSkillID;
        }

        raycast.SetSelection(m_nSelectType, selectDistance, traceHeight, param.nTargetType, true, m_bIsStrafe);
        m_targetObjectID = raycast.targetID;
        m_targetPos = raycast.targetPos;

        if (0 == (param.nDrawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_NONE))
        {
            int drawShape = (param.nDrawType & ((int)SPELL_DRAW_TYPE.DRAW_TYPE_RECT | (int)SPELL_DRAW_TYPE.DRAW_TYPE_SECTOR | (int)SPELL_DRAW_TYPE.DRAW_TYPE_ROUND));
            //if (drawShape > 0) 光画技能范围大圈也要进来
            {
                if (m_showingStrafeSelect)
                {
                    m_showingStrafeSelect = false;
                    LightingEffectFactory.Instance.selectEffects.disableEffect();
                }
                LightingEffectFactory.Instance.selectTips.disableEffect(); //技能提示就不要了
                LightingEffectFactory.Instance.selectEffects.enableEffect(drawShape, param.nDrawType, selectDistance, attackRange, param.fChooseRange, param.nTargetType, param.nSelectType);
                LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
            }
        }

        m_bCanNotUseOtherSkill = false;
        if (0 != (param.nCastType & (int)SPELL_CAST_TYPE.SpellCastType_NotUsedOtherSpell))
        {
            //其他技能不能使用
            m_bCanNotUseOtherSkill = true;
        }

        m_unFocoEnergiaTime = 0;
        if (param.nSpellType == (int)SPELL_TYPE.SPELL_TYPE_FOCO_ENERGIA)
        {
            //蓄力技能，上报一次
            m_targetObjectID = raycast.targetID;
            m_targetPos = raycast.targetPos;
            reportSkill();
            //蓄力时间计时，到了的话发送
            m_unFocoEnergiaTime = currentTick + (uint)param.nFocoEnergiaTime;
        }


        m_selectSkillID = -1;
        m_bKeyConfirm = false;


        m_nReportDirectlySlotID = -1;
        if (0 != (param.nSpellFlag & (int)SPELL_FLAG.SpellFlag_Strafe))
        {
            m_nReportDirectlySlotID = m_currentSlotID;
            onStartStrafe();
        }
        bool isSpeed= GetSkillUseType(m_currentSlotID) == SpellMode.ST_Speed;

        //m_bReportDirectly = true;
        //FPS普攻特殊处理，像扫射一样

        if (m_bIsStrafe || m_nReportDirectlySlotID > 0)
        {
            m_targetObjectID = raycast.targetID;
            m_targetPos = raycast.targetPos;
            reportSkill(); //只上报不重置，等到扫射结束后再重置
        }
        //不是快捷施法，且勾选了点解鼠标或者点击快捷键。才需要显示光圈
        else if (!isSpeed && (m_nSelectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_MouseClick | (int)SPELL_SELECT_TYPE.SpellSelectType_KeyConfirm)) != 0)
        {
            m_selectSkillID = m_currentSkillID;
            m_selectDistance = selectDistance;
            if (0 != (m_nSelectType & ((int)SPELL_SELECT_TYPE.SpellSelectType_KeyConfirm)))
            {
                m_bKeyConfirm = true;
            }

        }
        else
        {
            m_targetObjectID = raycast.targetID;
            m_targetPos = raycast.targetPos;
            Use_Skill();
        }
    }

    public void onReCaclacTargetInfo(cmd_spell_attribute param)
    {
        ////sc.onSetSpellAttribute(data.nAttributeType,data.nSelectType, data.fDistance, data.fAttackRange, data.fHeight);
        int success = param.nAttributeType;
        float selectDistance = param.fDistance;
        float attackRange = param.fAttackRange;
        float traceHeight = param.fHeight;

        Vector3 reslutpos = Vector3.zero;
        int stauts = 0;
        uint currentTick = GameLogicAPI.getTickCount();
        if (showLog)
        {
            Trace.Log(currentTick.ToString() + "nDefaultAimFlag=" + param.nDefaultAimFlag + ". onSetSpellAttribute type=" + param.nSelectType.ToString() + ". selectDistance=" + selectDistance.ToString() + ". attackRange=" + attackRange.ToString());
        }

        if (param.nDefaultAimFlag != 0)
        {
            reslutpos = Vector3.zero;
            stauts = 0;
        }
        else if (param.nAttributeType <= 0)
        {
            reslutpos = Vector3.zero;
            stauts = 0;
        }
        else
        {
            m_nSelectType = param.nSelectType;
            m_spellFlag = param.nSpellFlag;
            m_currentSkillID = param.nSpellID;
            m_currentSlotID = (int)param.nSlotType * maxSlotIndexForEachType + param.nSlotIndex;
            m_nRollSpellStartID = param.nRollSpellStartID;
            if ((param.nCastType & (int)SPELL_CAST_TYPE.SpellCastType_Pathing) != 0) //可以寻路放技能的话，瞄准距离和技能距离取大者，这样可以瞄准后寻路过去放
            {
                if (selectDistance < raycast.defaultAimDistance)
                {
                    selectDistance = raycast.defaultAimDistance;
                }
            }

            m_bIsStrafe = false;
            if ((param.nCastFlag & (int)SPELL_CAST_FLAG.SpellCastFlag_StarfeState) != 0)
            {
                m_bIsStrafe = true;
            }

            raycast.SetSelection(m_nSelectType, selectDistance, traceHeight, param.nTargetType, true, m_bIsStrafe);
            m_targetObjectID = raycast.targetID;
            m_targetPos = raycast.targetPos;
            reslutpos = m_targetPos;
            stauts = 1;
        }


        cmd_creature_recalc_target_info info = default(cmd_creature_recalc_target_info);
        info.self_pos_x = m_pFSM.transform.position.x;
        info.self_pos_y = m_pFSM.transform.position.y;
        info.self_pos_z = m_pFSM.transform.position.z;

        info.target_pos_x = reslutpos.x;
        info.target_pos_y = reslutpos.y;
        info.target_pos_z = reslutpos.z;
        info.nStauts = stauts;
        EntityEventHelper.Instance.SendCommand<cmd_creature_recalc_target_info>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_SPELL_RECALC_TARGET_INFO, ref info);
    }

    SpellMode GetSkillUseType(int nSlotID) //该技能是否智能施法
    {
        SpellMode useType = SpellMode.ST_Speed;

        List<PlayerSlotItem> slotList = null;
        bool hasUserConfig = PlayerSlotSettingConfig.Instance.GetLocalData(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION), out slotList);
        if (hasUserConfig) //如果用户配置了就使用用户的配置
        {
            for (int i = 0; i < slotList.Count; i++)
            {
                if (nSlotID == slotList[i].slotID)
                {
                    useType = (SpellMode)Enum.Parse(typeof(SpellMode), slotList[i].useType.ToString());
                }
            }
        }
        else   //默认施法：使用策划在技能编辑器的配置
        {
            OrderData odata = GetCurOrderData(nSlotID);
            if (odata == null)
            {
                useType = SpellMode.ST_Default;
            }
            else
                useType = (SpellMode)Enum.Parse(typeof(SpellMode), (odata.nAdvanceUseType).ToString());
        }
        return useType;
    }

    void onTargetSelectEvent(bool succeed)
    {
        if (showLog)
        {
            uint currentTick = GameLogicAPI.getTickCount();
            Trace.Log(currentTick.ToString() + "onTargetSelectEvent succeed=" + succeed.ToString() + ". targetID=" + m_targetObjectID + ". targetPos=" + m_targetPos);
        }

        if (m_currentSlotID == 0 && m_currentSkillID <= 0)
        {
            if (succeed)
            {
                if (DataCenter.LogicDataCenter.playerSystemDataManager.SelfGameState == (int)ACTOR_GAME_STATE.GAME_STATE_MATCH)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_PK_MATCHSTATE);
                }
                else
                {
                    int id = m_targetObjectID;
                    if (m_targetObjectID == 0)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET);
                    }
                    else
                    {
                        string roleName = "";
                        string pkMessage = ULocalizationService.Instance.Get("UIView", "PK", "ConfirmMessage");
                        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(id);
                        if (null != ev && null != ev.Property)
                            roleName = ev.Property.CreatureName;
                        pkMessage = string.Format(pkMessage, roleName);
                        UIDialogData toolTipData = new UIDialogData(
                            title: ULocalizationService.Instance.Get("UIView", "PK", "ShowPrompt"),
                            message: pkMessage,
                            buttons: new UIWidgets.DialogActions(){
                           {ULocalizationService.Instance.Get("UIView", "PK", "Confirm"),() => {reportPKTarget(id);return true;}},
                           {ULocalizationService.Instance.Get("UIView", "PK", "Cancel"),() => {;return true;}}
                            });
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
                    }
                }
            }

            doingPKSelection = false;
        }


        if (succeed)
        {
            m_currentSkillID = m_selectSkillID;
            if (m_bIsStrafe)
            {
                reportSkill(); //只上报不重置，等到扫射结束后再重置
            }
            else
            {
                Use_Skill();
            }
        }
        else
        {
            cmd_creature_cancel_cast data = new cmd_creature_cancel_cast();
            data.nSpellID = m_selectSkillID;

            if (showLog)
            {
                Trace.Log("Break Skill");
            }

            EntityEventHelper.Instance.SendCommand<cmd_creature_cancel_cast>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_CANCEL_CAST, ref data);
            m_selectSkillID = -1;
            resetSkill();
        }
    }


    public bool blockskill = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// 使用一次技能
    public int Use_Skill()
    {
        //if (showLog)
        //{
        //    foreach (OrderData order in ordersetMap.Values)
        //    {
        //        Trace.Log(order.keySequence[0].ToString()+ "id="+ order.nSlotID + " enable=" + order.bEnabled);
        //    }
        //}

        if (m_bBlockTest)
        {
            bool blocked = raycast.isBlocked(m_targetPos);
            if (blocked)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_SKILL_BLOCKED);
                resetSkill();
                return 0;
            }
        }

        reportSkill();

        if (m_bRotateCameraToTarget && m_targetObjectID > 0)
        {
            U3D_Render.EntityView view = EntityFactory.getEntityViewByID(m_targetObjectID);
            if (view != null && view.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE) //对英雄放技能才转
            {
                GameObject targetGO = view.gameObject;
                if (targetGO)
                {
                    SoldierCamera camera = SoldierCamera.MainInstance<SoldierCamera>();
                    if (camera)
                    {
                        camera.enterNavMode(CameraNavMode.Target, targetGO.transform, Vector3.zero);
                    }
                }
            }
        }
        m_selectSkillID = -1;
        resetSkill();
        uint currentTick = GameLogicAPI.getTickCount();
        un_lastMouseDownTick = currentTick; //释放成功重新计算鼠标左键间隔时间              

        //CreatureStateMachine machine = gameObject.GetComponent<CreatureStateMachine>();
        //if (machine != null)
        //{
        //    machine.syncPos();
        //}

        return 0;
    }

    public void UIUseSkill(int skillID, int targetID)
    {
        m_currentSkillID = skillID;
        m_targetObjectID = targetID;
        GameObject targetGo = EntityFactory.getEntityGameObjectByID(m_targetObjectID);
        if (targetGo)
        {
            m_targetPos = targetGo.transform.position;
        }

        Use_Skill();
    }

    //根据targetPos和面向算出翻滚的方向，输出不同的id，用于skillID，同时修正m_targetPos，用于攻击时转动面向
    private int calFace(ref Vector3 targetPos)
    {
        Vector3 targetDir = targetPos - transform.position;
        targetDir.y = 0.0f;
        targetDir.Normalize();
        Vector3 faceRight = Vector3.Cross(m_pFSM.transform.up, targetDir);
        faceRight.Normalize();
        Vector3 turnTo = targetDir;

        float forward = Vector3.Dot(targetDir, m_pFSM.transform.forward);
        float right = Vector3.Dot(targetDir, m_pFSM.transform.right);

        int output = 0;

        if (Mathf.Abs(forward) > Mathf.Abs(right))
        {
            right = 0.0f;
            if (forward < 0.0f)
            {
                forward = -1.0f;
                turnTo *= -1.0f;
                output = 1;
            }
            else
            {
                forward = 1.0f;
                output = 0;
            }
        }
        else
        {
            forward = 0.0f;
            if (right < 0.0f)
            {
                right = -1.0f;
                turnTo = faceRight;
                output = 2;
            }
            else
            {
                right = 1.0f;
                turnTo = -faceRight;
                output = 3;
            }
        }

        targetPos = m_pFSM.transform.position + turnTo * 6.0f;
        return output;
    }

    public uint m_nLastReportTick = 0;

    private void reportSkill()
    {

        if (m_currentSkillID <= 0)
        {
            return;
        }

        cmd_creature_act_sync data = new cmd_creature_act_sync();
        data.pos.fPosition_x = gameObject.transform.position.x;
        data.pos.fPosition_y = gameObject.transform.position.y;
        data.pos.fPosition_z = gameObject.transform.position.z;

        data.pos.fVelocity_x = 0;// fsm.rb.velocity.x;
        data.pos.fVelocity_y = 0;// fsm.rb.velocity.y;
        data.pos.fVelocity_z = 0;// fsm.rb.velocity.z;

        data.pos.fRotation_x = gameObject.transform.eulerAngles.x;//gameObject.transform.rotation.w;
        data.pos.fRotation_y = gameObject.transform.eulerAngles.y;//gameObject.transform.rotation.y;
        data.pos.fRotation_z = gameObject.transform.eulerAngles.z;// fsm.rb.rotation.z; 

        //    data.state.nPostureID = (int)fsm.curState.GetID();
        // data.state.bIsMoving = fsm.a.GetBool("isMoving") ? (byte)1 : (byte)0;      // 是否运动中
        data.state.fForward = 0;// fsm.a.GetFloat("moveforward");   // 向前状态控制
        data.state.fLeft = 0;// fsm.a.GetFloat("moveleft");         // 方向
        //    data.bJump = fsm.a.GetBool("Jump");              // 是否正在跳跃
        //    data.bFall = fsm.a.GetBool("Fall"); ;            // 是否正在下落


        if (OwnerSC != null && OwnerSC.inAir)
        {
            data.state.bJump = 1;
        }
        else
        {
            data.state.bJump = 0;
        }

        if (OwnerSC != null)
        {
            OwnerSC.autoForward = false;
        }

        if (0 != (m_spellFlag & (int)SPELL_FLAG.SpellFlag_Roll))
        {
            //根据targetPos和面向算出翻滚的方向，输出不同的id，用于skillID，同时修正m_targetPos，用于攻击时转动面向
            m_currentSkillID = m_nRollSpellStartID + calFace(ref m_targetPos);
        }

        data.nCtrID = (int)ActID.Attack; ;
        data.nCtrParam = m_currentSkillID;
        data.nCtrParam2 = m_targetObjectID;
        data.fCtrParam_x = m_targetPos.x;
        data.fCtrParam_y = m_targetPos.y;
        data.fCtrParam_z = m_targetPos.z;
        data.nSlotType = (SLOT_TYPE)(int)(m_currentSlotID / maxSlotIndexForEachType);
        data.nSlotIndex = m_currentSlotID % maxSlotIndexForEachType;
        data.operateTick = GameLogicAPI.getTickCount();     // 显示层本地操作时间

        uint currentTick = GameLogicAPI.getTickCount();
        if (showLog)
        {
            Trace.Log("report Skill " + m_currentSkillID + " interval=" + (currentTick - m_nLastReportTick));
            m_nLastReportTick = currentTick;
            Trace.Log("Pos" + transform.position + "TargetPos " + m_targetPos.ToString() + "TargetID" + m_targetObjectID);
        }
        EntityEventHelper.Instance.SendCommand<cmd_creature_act_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_CAST_SPELL, ref data);

    }
    #endregion

    #region "扫射技能处理"
    ///--------------------------------------------------------------------------------------------------------
    ///扫射技能调用
    ///扫射技能时逻辑层调用此函数，快速答复瞄准目标的相关属性，这里假设只有准心和面向类，不需要鼠标二次操作
    ///--------------------------------------------------------------------------------------------------------
    ///

    private Transform m_straftGroundTransform = null;
    private GameObject m_straftGroundObject = null;
    private bool m_showingStrafeSelect = false;
    private cmd_effect_Strafe m_strafe_param;
    private uint m_lastStrafeUpdateTick = 0;
    private uint m_lastStrafeReportTick = 0;
    private int m_strafeReportCount = 0;
    private uint m_strafeStopTick = 0;

    public void onStrafe(cmd_effect_Strafe param)
    {
        if (showLog)
        {
            Trace.Log("onStrafe type=" + param.nSelectType.ToString() + ". selectDistance=" + param.fDistance.ToString() + ". attackRange=" + param.fAttackRange.ToString());
            Trace.Log("StrafeType" + param.nStrafeType + "flyingSpeed=" + param.fFlyingSpeed + ";nTime=" + param.nTime + ";nCount=" + param.nCount);
        }
        raycast.SetSelection(param.nSelectType, param.fDistance, 0.0f, param.nTargetType, true, true);
        LightingEffectFactory.Instance.selectEffects.disableEffect();

        if (param.nDrawType > 0)
        {
            int drawShape = (param.nDrawType & ((int)SPELL_DRAW_TYPE.DRAW_TYPE_RECT | (int)SPELL_DRAW_TYPE.DRAW_TYPE_SECTOR | (int)SPELL_DRAW_TYPE.DRAW_TYPE_ROUND));
            LightingEffectFactory.Instance.selectEffects.enableEffect(drawShape, param.nDrawType, param.fDistance, param.fAttackRange, 0, 0, param.nSelectType);
            LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
            m_showingStrafeSelect = true;
        }
        else
        {
            m_showingStrafeSelect = false;
        }
        m_strafe_param = param;
        m_lastStrafeUpdateTick = GameLogicAPI.getTickCount();
        m_lastStrafeReportTick = m_lastStrafeUpdateTick - (uint)param.nTime; //进入straftUpdate马上上报一次
        m_strafeReportCount = 0;
        m_strafeStopTick = m_lastStrafeUpdateTick + (uint)(param.nTime * param.nCount);
        //StartCoroutine(StartAnswerStrafe(param));
        #region 废弃代码

        /*
        if (raycast == null)
        {
            raycast = gameObject.GetComponent<ScreenRaycast>();
            if (raycast == null)
            {
                Trace.Log("Error~raycast == null");
                return;
            }
        }
        
        //if (0!=
        //    (param.nSelectType & 
        //    ((int)SPELL_SELECT_TYPE.SpellSelectType_SelectEntity | (int)SPELL_SELECT_TYPE.SpellSelectType_SelectTile | (int)SPELL_SELECT_TYPE.SpellSelectType_SelectDirection)
        //    ))

        //{
        //    Trace.Log("Strafe does not support type" + param.nSelectType.ToString());
        //}
        //else
        {
            LightingEffectManager leManager = GetComponent<LightingEffectManager>();

            //创建扫射光效
            //if (leManager != null)
            //{                
            //    LightEffectParam AreaParam = new LightEffectParam();
            //    AreaParam.nFeedbackID = 0;
            //    AreaParam.bIsHero = true;
            //    AreaParam.nSrcEntityID = property.id;
            //    AreaParam.srcObject = gameObject;
            //    AreaParam.fDistance = param.fDistance;
            //    AreaParam.fAttackRange = param.fAttackRange;
            //    AreaParam.targetObject = null;
            //    AreaParam.targetPos = Vector3.zero;
            //    m_strafeEffect=leManager.AddLighting(54, AreaParam);
            //}

            if (m_straftGroundObject == null)
            {
                //创建地面法阵
                UnityEngine.Object obj = Resources.Load("Effect/selectEffect/skillCirclePref");
                GameObject entity = ResNode.InstantiateRes(obj) as GameObject;
                m_straftGroundTransform = entity.transform;
                m_straftGroundObject = entity;
            }
            if (m_straftGroundObject != null)
            {
                Projector projector = m_straftGroundObject.GetComponent<Projector>();
                if (projector != null)
                {
                    projector.orthographicSize = param.fAttackRange;
                }
            }

            StartCoroutine(StartAnswerStrafe(param));
        }
         */
        #endregion
    }

    private void straftUpdate()
    {
        if (m_lastStrafeUpdateTick == 0) return; //以此表示没有进行扫射

        if (m_straftGroundObject != null && m_straftGroundTransform != null)
        {
            m_straftGroundTransform.SetPosition(new Vector3(m_targetPos.x, m_targetPos.y + 1.0f, m_targetPos.z));
        }

        uint tick = GameLogicAPI.getTickCount();
        float deltaTime = ((float)(tick - m_lastStrafeUpdateTick)) / 1000.0f;
        m_lastStrafeUpdateTick = tick;

        if (m_strafe_param.nStrafeType == (int)STRAFE_TYPE.STRAFE_MOUSE_CIRCLE_ROTATE)
        {
            raycast.updateStrafeRotate(deltaTime * m_strafe_param.fFlyingSpeed / 180.0f * Mathf.PI);
        }

        m_targetObjectID = raycast.targetID;
        m_targetPos = raycast.targetPos;

        if (m_showingStrafeSelect)
        {
            LightingEffectFactory.Instance.selectEffects.updateEffect(m_targetPos);
        }

        if (tick - m_lastStrafeReportTick >= m_strafe_param.nTime)
        {
            answerStrafe(m_strafe_param.nID);
            m_strafeReportCount++;
            m_lastStrafeReportTick = tick;
            if (m_pFSM && m_pFSM.isHero)
            {
                if (m_targetObjectID != EntityFactory.MainHeroID) //选择自己的话不要转面向，不然会来回转
                {
                    m_pFSM.FaceTo(m_targetPos);
                }
            }
        }

        if (m_strafeReportCount >= m_strafe_param.nCount || tick > m_strafeStopTick)
        {
            //stop strafe
            if (m_showingStrafeSelect)
            {
                LightingEffectFactory.Instance.selectEffects.disableEffect();
                m_showingStrafeSelect = false;
            }
            m_selectSkillID = -1;
            m_lastStrafeUpdateTick = 0;
            resetSkill();
            m_strafeStopTick = 0;
        }
    }

    //IEnumerator StartAnswerStrafe(cmd_effect_Strafe param)
    //{
    //    m_currentSkillID = param.nID;
    //    for (int i = 0; i < param.nCount; i++)
    //    {
    //        answerStrafe(param, param.nTime / 1000.0f);
    //        yield return new WaitForSeconds(param.nTime / 1000.0f);
    //    }

    //    if (m_showingStrafeSelect)
    //    {
    //        selectEffects.disableEffect();
    //        m_showingStrafeSelect = false;
    //    }
    //    resetSkill();


    //    /*
    //    m_strafeEffect = null;
    //    m_straftGroundTransform = null;
    //    if (m_straftGroundObject != null)
    //    {
    //        GameObject.ResNode.DestroyRes(ref m_straftGroundObject);
    //        m_straftGroundObject = null;
    //    }
    //    if (raycast != null)
    //    {
    //        raycast.SetDefaultSelection();
    //    }
    //     */
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// 回答扫射目标点
    private int answerStrafe(int nID)
    {
        //if (param.nStrafeType == (int)STRAFE_TYPE.STRAFE_MOUSE_CIRCLE_ROTATE)
        //{
        //    raycast.updateStrafeRotate(deltaTime * param.fFlyingSpeed / 180.0f * Mathf.PI);
        //}

        cmd_creature_act_sync data = new cmd_creature_act_sync();
        data.pos.fPosition_x = gameObject.transform.position.x;
        data.pos.fPosition_y = gameObject.transform.position.y;
        data.pos.fPosition_z = gameObject.transform.position.z;

        data.pos.fVelocity_x = 0;// fsm.rb.velocity.x;
        data.pos.fVelocity_y = 0;// fsm.rb.velocity.y;
        data.pos.fVelocity_z = 0;// fsm.rb.velocity.z;

        data.pos.fRotation_x = gameObject.transform.rotation.w;
        data.pos.fRotation_y = gameObject.transform.rotation.y;
        data.pos.fRotation_z = 0;// fsm.rb.rotation.z; 

        data.state.fForward = 0;// fsm.a.GetFloat("moveforward");   // 向前状态控制
        data.state.fLeft = 0;// fsm.a.GetFloat("moveleft");         // 方向
        //data.state.nSkillID = m_currentSkillID;      // 技能ID
        data.nSlotType = (SLOT_TYPE)(int)(m_currentSlotID / maxSlotIndexForEachType);
        data.nSlotIndex = m_currentSlotID % maxSlotIndexForEachType;

        if (OwnerSC != null && OwnerSC.inAir)
        {
            data.state.bJump = 1;
        }
        else
        {
            data.state.bJump = 0;
        }

        data.nCtrID = (int)ActID.Attack;
        data.nCtrParam = nID;
        data.nCtrParam2 = m_targetObjectID;
        data.fCtrParam_x = m_targetPos.x;
        data.fCtrParam_y = m_targetPos.y;
        data.fCtrParam_z = m_targetPos.z;

        if (showLog)
        {
            uint tick = GameLogicAPI.getTickCount();
            Trace.Log(tick.ToString() + "STRAFE_DATA：TargetPos " + m_targetPos.ToString() + "TargetID" + m_targetObjectID);
        }

        EntityEventHelper.Instance.SendCommand<cmd_creature_act_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_STRAFE_DATA, ref data);

        return 0;
    }

    #endregion


    #region "蓄力技能处理"

    private void focoEnergiaStart()
    {
        m_focoEnergiaState = FocoEnergiaState.None;

        cmd_foco_energia context = new cmd_foco_energia();
        context.nID = m_currentSkillID;
        EntityEventHelper.Instance.SendCommand<cmd_foco_energia>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_FOCO_ENERGIA, ref context);

        m_selectSkillID = -1;
        resetSkill();
    }

    private void focoEnergiaCancel()
    {
        m_focoEnergiaState = FocoEnergiaState.None;
        m_selectSkillID = -1;
        resetSkill();
    }

    #endregion


    private uint m_closeSelectTipsTick = 0;
    public void showTipSelectEffect(cmd_creature_draw_spell_decal param)
    {
        if (param.nDraw <= 0)
        {
            LightingEffectFactory.Instance.selectTips.disableEffect();
            return;
        }
        m_closeSelectTipsTick = 0;
        if (param.nShowTime > 0)
        {
            m_closeSelectTipsTick = GameLogicAPI.getTickCount() + (uint)param.nShowTime;
        }

        int drawShape = (param.nDrawType & ((int)SPELL_DRAW_TYPE.DRAW_TYPE_RECT | (int)SPELL_DRAW_TYPE.DRAW_TYPE_SECTOR | (int)SPELL_DRAW_TYPE.DRAW_TYPE_ROUND));
        //if (drawShape > 0) 光画技能范围大圈也要进来
        {
            LightingEffectFactory.Instance.selectTips.enableEffect(drawShape, param.nDrawType, param.fDistance, param.fAttackRange, param.fChooseRange, 0, 0);
        }
    }

    //private void enterExitTank()
    //{

    //    if (raycast == null)
    //    {
    //        Trace.Log("Error~raycast == null");
    //        return;
    //    }

    //    if (raycast.targetID > 0 && raycast.targetType == (int)ENTITY_TYPE.TYPE_TANK)
    //    {
    //        if (showLog)
    //        {
    //            Trace.Log(property.Master.ID + "Enter Tank " + raycast.targetID);
    //        }
    //        GameLogicAPI.onEntityEvent(property.Master.ID, (int)EntityLogicDef.ENTITY_CMD_ENTER_TANK, raycast.targetID, "", IntPtr.Zero, 0);

    //    }
    //}

    #region PK选人
    //PK总开关
    private bool m_enablePKSelection = true;
    public bool enablePKSelection
    {
        set { m_enablePKSelection = value; }
        get { return m_enablePKSelection; }
    }

    //是否正在PK选人状态
    private bool m_doingPKSelection = false;
    public bool doingPKSelection
    {
        set
        {
            if (m_enablePKSelection)
            {
                if (m_doingPKSelection && !value)
                {
                    //从PK选人状态中退出
                    m_currentSlotID = -1;  //m_currentSlotID==0表示正在PK选人
                    MouseCursorManager.Instance.setPkMode(false);
                    raycast.SetDefaultSelection();
                }
                else if (!m_doingPKSelection && value)
                {
                    //由非PK选人状态进入PK选人状态
                    m_currentSlotID = 0; //0表示正在PK选人
                    raycast.SetPKTargetSelection();
                    MouseCursorManager.Instance.setPkMode(true);
                    USystemFloatTextData data = new USystemFloatTextData();
                    string szMessage = ULocalizationService.Instance.Get("UIView", "PK", "ClickPlayerMessage");
                    U3D_Render.Common.UBB.toHtml(ref szMessage);
                    data.message = szMessage;
                    data.nPos = 1;
                    data.nShowTime = 2000;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, data);
                }

                m_doingPKSelection = value;
            }
            else
            {
                if (value)
                {
                    USystemFloatTextData data = new USystemFloatTextData();
                    data.message = ULocalizationService.Instance.Get("UIView", "PK", "DoingPKMessage");
                    data.nPos = 1;
                    data.nShowTime = 5000;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, data);
                }
            }

        }
        get
        {
            return m_doingPKSelection;
        }
    }

    private void UpdatePKSelection()
    {
        if (!m_enablePKSelection) //PK总开关
        {
            return;
        }

        if (m_doingPKSelection) //正在PK的时候屏蔽PK快捷键
        {
            return;
        }
        if (m_currentSkillID >= 0) //正在使用其他技能，退出
        {
            return;
        }

        //if (InputManager.GetKeyUp(KeyCode.K))
        if (m_doingPKSelection == false && GameLogicAPI.isForbitPK() != 1 && FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.SelectPKTarget))
        {
            doingPKSelection = true;
        }
    }

    private void reportPKTarget(int targetObjectID)
    {
        cmd_creature_pk_choose_target data = new cmd_creature_pk_choose_target();
        data.uidTarget = (uint)targetObjectID;

        uint currentTick = GameLogicAPI.getTickCount();
        if (showLog)
        {
            Trace.Log(currentTick.ToString() + "reportPKTarget " + m_currentSkillID);
            Trace.Log(currentTick.ToString() + "Pos" + transform.position + "TargetPos " + m_targetPos.ToString() + "TargetID" + m_targetObjectID);
        }
        EntityEventHelper.Instance.SendCommand<cmd_creature_pk_choose_target>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_PK_CHOOSE_TARGET, ref data);
    }

    #endregion

    #region 发表情
    public void UseEmoticonSkill(int skillID)
    {
        if (m_currentSkillID >= 0) //正在使用其他技能,退出
        {
            return;
        }

        if (m_doingPKSelection) //正在PK,退出
        {
            return;
        }

        cmd_creature_act_sync data = new cmd_creature_act_sync();
        data.pos.fPosition_x = gameObject.transform.position.x;
        data.pos.fPosition_y = gameObject.transform.position.y;
        data.pos.fPosition_z = gameObject.transform.position.z;

        data.pos.fVelocity_x = 0;// fsm.rb.velocity.x;
        data.pos.fVelocity_y = 0;// fsm.rb.velocity.y;
        data.pos.fVelocity_z = 0;// fsm.rb.velocity.z;

        data.pos.fRotation_x = gameObject.transform.eulerAngles.x;//gameObject.transform.rotation.w;
        data.pos.fRotation_y = gameObject.transform.eulerAngles.y;//gameObject.transform.rotation.y;
        data.pos.fRotation_z = gameObject.transform.eulerAngles.z;// fsm.rb.rotation.z; 

        data.state.fForward = 0;// fsm.a.GetFloat("moveforward");   // 向前状态控制
        data.state.fLeft = 0;// fsm.a.GetFloat("moveleft");         // 方向

        if (OwnerSC != null && OwnerSC.inAir)
        {
            data.state.bJump = 1;
        }
        else
        {
            data.state.bJump = 0;
        }

        if (OwnerSC != null)
        {
            OwnerSC.autoForward = false;
        }

        data.nCtrID = (int)ActID.Attack;
        data.nCtrParam = skillID;
        data.nCtrParam2 = 0;
        data.fCtrParam_x = 0;
        data.fCtrParam_y = 0;
        data.fCtrParam_z = 0;
        data.nSlotType = SLOT_TYPE.SLOT_TYPE_NONE;
        data.nSlotIndex = 0;
        data.operateTick = GameLogicAPI.getTickCount();     // 显示层本地操作时间

        EntityEventHelper.Instance.SendCommand<cmd_creature_act_sync>(m_pFSM.entityID, EntityLogicDef.ENTITY_CMD_CAST_SPELL, ref data);
    }
    #endregion
}
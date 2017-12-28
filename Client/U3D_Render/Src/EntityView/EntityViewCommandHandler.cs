using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using ASpeedGame.Data.GameMobaSchemes;
using GameLogic;
using SPELL;
using U3D_Render;

// 定义实体ID
using ENTITY_ID = System.Int32;
using U3D_Render.Common;
using War;
using USpeedUI;
using DataCenter;
using USpeedUI.WarFloatFont;
using USpeedUI.SideButtonList;
using USpeedUI.PlayerFrame;
using USpeedUI.TooltipAndDialog;
using GUIDE;
using U3D_Render.CameraControl;
using Data.SideButtonConfig;

// 处理逻辑层发过来的命令
public class EntityViewCommandHandler : Singleton<EntityViewCommandHandler>
{
    // 命令处理函数
    public delegate bool FUNC_PTR_ENTITY_COMMAND_HANDLER(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen);

    // 命令映射表
    public static Dictionary<Int32, FUNC_PTR_ENTITY_COMMAND_HANDLER> m_HandlerMap = new Dictionary<Int32, FUNC_PTR_ENTITY_COMMAND_HANDLER>();

    // 消息类型映射表，用于反查消息的非托管数据大小
    public static Dictionary<Int32, Type> m_TypeMap = new Dictionary<Int32, Type>();

    // 主角状态()
    public static int mainHeroState;

    // 运行报警间隔 ms
    private static int RUN_TIMEOUT_ALERT_WARNING = 50;

    public static bool onCommand(ENTITY_ID entityID, int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen, int nTickCount)
    {
        ENTITY_ID id = (ENTITY_ID)entityID;
        Int32 cmd_id = (Int32)cmdID;
        EntityView ev = EntityFactory.getEntityViewByID(id);

        if (ev == null)
        {
            //Trace.Log(string.Format("onEntityCommand can not found entity, id=" + id.ToString() + ", cmd=" + cmdID.ToString() ));
            return false;
        }

        // 同步位置消息得特殊处理，其它实体指令不处理延迟压入commands，也得更新位置以便根据人数调度实体
        if (cmd_id == (int)EntityLogicDef.ENTITY_TOVIEW_SYNC_POS)
        {
            cmd_creature_rigidbody_sync data = IntPtrHelper.toData<cmd_creature_rigidbody_sync>(ptrParam);
            EntityFactory.entry_syncEntry(id, data);
        }
        if (cmd_id == (int)EntityLogicDef.ENTITY_TOVIEW_SYNC_TRANSPORT)
        {
            cmd_creature_transport data = IntPtrHelper.toData<cmd_creature_transport>(ptrParam);

            cmd_creature_rigidbody_sync edata = new cmd_creature_rigidbody_sync();
            edata.fPosition_x = data.fPosition_x;
            edata.fPosition_y = data.fPosition_y;
            edata.fPosition_z = data.fPosition_z;

            if (data.bUseAngle == 1)
            {
                edata.fRotation_x = data.fRotation_x;
                edata.fRotation_y = data.fRotation_y;
                edata.fRotation_z = data.fRotation_z;
            }
            edata.nActorID = entityID;
            EntityFactory.entry_syncEntry(id, edata);
        }


        if (ev.gameObject == null)
        {
            EntityViewCommand command = new EntityViewCommand();
            command.cmdID = cmdID;
            command.nParam = nParam;
            command.strParam = strParam;
            command.ptrParam = ptrParam;
            command.datalen = nPtrLen;
            if (m_TypeMap.ContainsKey(cmdID))
            {
                command.datatype = m_TypeMap[cmdID];
                if (command.datatype != null)
                {
                    command.datalen = Marshal.SizeOf(command.datatype);
                }
            }
            command.databytes = null;
            if (command.ptrParam != null && command.ptrParam != IntPtr.Zero && command.datalen > 0)
            {
                command.databytes = new byte[command.datalen];
                try
                {
                    Marshal.Copy(command.ptrParam, command.databytes, 0, command.datalen);
                }
                catch (Exception e)
                {
                    command.databytes = null;
                    Trace.LogError(e.ToString());
                }
            }
            ev.commands.Enqueue(command);
            return true;
        }
        else
        {
            // 对象无效
            if (!ev.IsValid)
            {
                Trace.LogWarning("onCommand execute failed.entityID=" + entityID.ToString() + ", cmdID=" + cmdID.ToString());
                return false;
            }

            if (!m_HandlerMap.ContainsKey(cmd_id))
            {
                Trace.Log(string.Format("EntityCommand can not found cmd, id=" + entityID.ToString() + ", cmd=" + cmd_id.ToString()));
                return false;
            }

            FUNC_PTR_ENTITY_COMMAND_HANDLER pHandle = m_HandlerMap[cmd_id];
            if (pHandle != null)
            {
                bool bResult = pHandle(ev, ev.gameObject, id, cmd_id, nParam, strParam, ptrParam, nPtrLen);

                int nRunTickcout = (int)GameLogicAPI.getTickCount() - nTickCount;
                if (nRunTickcout >= RUN_TIMEOUT_ALERT_WARNING)
                {
                    //Trace.Warning("onCommand(): entity=" + entityID.ToString() + ":cmd=" + cmdID.ToString() + ",run " + nRunTickcout.ToString() + " ms, entity_count=" + EntityFactory.Instance.EntityNum);
                }

                return bResult;
            }
            else
            {
                return false;
            }
        }
    }

    public static bool onCommandsDelay(EntityView ev)
    {
        if (ev == null || ev.commands == null)
            return false;

        while (ev.commands.Count > 0)
        {
            EntityViewCommand command = ev.commands.Dequeue();

            if (command.databytes != null && command.datalen > 0) //把缓存的参数重新解开到非托管内存
            {
                IntPtr c = Marshal.AllocHGlobal(command.datalen);
                Marshal.Copy(command.databytes, 0, c, command.datalen);
                EntityViewCommandHandler.onCommandDelay(ev, command.cmdID, command.nParam, command.strParam, c, command.datalen);
                Marshal.FreeHGlobal(c);
            }
            else
            {
                EntityViewCommandHandler.onCommandDelay(ev, command.cmdID, command.nParam, command.strParam, command.ptrParam, command.datalen);
            }
        }

        return true;
    }

    public static bool onCommandDelay(EntityView ev, int cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        ENTITY_ID id = ev.ID;
        Int32 cmd_id = (Int32)cmdID;

        if (!m_HandlerMap.ContainsKey(cmd_id))
        {
            Trace.Log(string.Format("==================onEntityCommand can not found cmdid===================" + cmd_id.ToString()));
            return false;
        }

        uint dwStartTime = GameLogicAPI.getTickCount();

        bool bResult = m_HandlerMap[cmd_id](ev, ev.gameObject, id, cmd_id, nParam, strParam, ptrParam, nPtrLen);

        uint nRunTickcout = GameLogicAPI.getTickCount() - dwStartTime;
        if (nRunTickcout >= RUN_TIMEOUT_ALERT_WARNING)
        {
            Trace.Warning("onCommandDelay(): entity=" + id.ToString() + ": cmd=" + cmdID.ToString() + ",run " + nRunTickcout.ToString() + " ms, entity_count=" + EntityFactory.Instance.EntityNum);
        }

        return bResult;
    }

    static public void setHandler(int cmdID, FUNC_PTR_ENTITY_COMMAND_HANDLER handler, Type t)
    {
        m_HandlerMap[(int)cmdID] = handler;
        m_TypeMap[(int)cmdID] = t;
    }

    static public void Start()
    {
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_NAME, onCommand_SetName, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_BLOODSHOW, onCommand_ShowBlood, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_POS, onCommand_PositionSync, typeof(cmd_creature_rigidbody_sync));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_CTRL, onCommand_CTRL_Sync, typeof(cmd_creature_act_sync));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ADD_LIGHTING, onCommand_AddLighting, typeof(cmd_creature_skillview));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_REMOVE_LIGHTING, onCommand_RemoveLighting, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_AI_ROLL, onCommand_AI_Roll, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PLAY_ANIMATION, onCommand_PlayAnimation, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_BREAK_ATTACK_ANIMATION, onCommand_BreakAttackAnimation, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SWITCH_STATE, onCommand_SwitchState, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHARGE, onCommand_Charge, typeof(cmd_force_move));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_EFFECT_FLIGHT, onCommand_Flight, typeof(cmd_effect_Flight));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_SLOT, onCommand_ChangeSlot, typeof(cmd_creature_set_slot));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SETSPELLATTRIBUTE, onCommand_SetSpellAttribute, typeof(cmd_spell_attribute));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_EFFECT_STRAFE, onCommand_EffectStrafe, typeof(cmd_effect_Strafe));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SETCASTSPELL_POS, onCommand_SetCastSpellPos, typeof(cmd_creature_castSpellPos));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SPELL_BREAK, onCommand_SpellBreak, typeof(cmd_creature_SpellBreak));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_STOP_MOVE, onCommand_StopMove, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_STOP_MOVE_HANDLE, onCommand_StopMoveHandle, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RESUME_MOVE, onCommand_ResumeMove, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NOTURNTO, onCommand_NoTurnTo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NO_SELECT, onCommand_NoSelect, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RELIVE, onCommand_Relive, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_BUFF, onCommand_SetBuff, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_TRANSPORT, onCommond_SyncForceSetPos, typeof(cmd_creature_transport));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_SPELL_OVERLAY, onCommond_SetSpellOverlay, typeof(cmd_creature_transport));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_SET_ANIMATION, onCommond_SetAnimation, typeof(cmd_entity_set_animation));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_PERSONINFO, onCommond_RicvePersonInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_SPEED, onCommond_SetMovingSpeed, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_ENTER_VIEW, onCommond_EnterView, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_LEAVE_VIEW, onCommond_LeaveView, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_SKIN, onCommond_ChangeSkin, typeof(cmd_change_entity_skin));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_BROADKILLINFO, onCommand_BroadKill, typeof(cmd_war_entity_killcountinfo));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_BDIMPENTITYINFO, onCommand_BroadImprotantEt, typeof(cmd_war_important_entiydieinfo));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_MONSTERINFO, onCommond_UpdateMonsterTimer, typeof(cmd_war_monsterinfo));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_SHOW_WIN_OR_FIAL, onCommond_ShowWarEndWinOrFail, typeof(cmd_show_win_or_fail));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_WING_STATE, onCommand_ChangeWingState, typeof(cmd_Entity_Change_Wing_State));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_DODGE_STATE, onCommand_ChangeDodgeState, typeof(cmd_Entity_Change_Dodge_State));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_LONGJUMP, onCommand_LongJump, typeof(cmd_creature_longjump));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_GLIDE, onCommand_UseGlide, typeof(cmd_creature_glide));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PERSPECTIVE_SHOW, onCommand_PerspectiveShow, typeof(cmd_creature_PerspectiveShow));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_SNIPERSCOPE, onCommand_ChangeSniperscope, typeof(cmd_Entity_Change_Sniperscope));

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MINIMAP_INFO, onCommand_MarkMinimap, typeof(cmd_war_mini_map_info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_DAMAGE, onCommand_Dmage, typeof(cmd_creature_damage));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY, onCommand_UpdateIntegerProperty, typeof(entity_update_Numproperty));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_DRAGON_SHOW_POSITION, onCommand_ShowDragonPosition, typeof(cmd_pos));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MINIMAP_ICON, onCommand_CreateStaticIcon, typeof(cmd_war_mini_map_icon));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RELIVE_TIME, onCommond_ReliveTime, typeof(cmd_war_important_entiyreliveinfo));//战场玩家复活时间
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_IMPORTANT_MONSTER_RELIVE, onCommond_BroadCastMonsterRelive, typeof(cmd_war_important_monsterliveinfo));//战场重要实体复活广播
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CLEAR_ALL_MINIMAP_INFO, onCommond_ClearAllMiniMapIcon, typeof(cmd_war_clear_all_mini_map_info));//战场重要实体复活广播
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_IMPORTANT_MONSTER_DIE_TXT, onCommond_BroadCastMonsterDieText, typeof(cmd_war_important_monsterdietxtinfo));//战场重要实体死亡文字广播
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_GOTO_GUARD, onCommond_GotoGuard, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_DEAD, onCommond_EntityDead, typeof(cmd_creature_die));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_DISABLE_SPELL, onCommand_DisableSpell, typeof(cmd_creature_disable_spell));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_FIGHT_MINIMAP_INFO, onCommand_MinimapFightInfo, typeof(cmd_war_entity_fight_minimap_info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_OPERATION, onCommand_Change_Operation, typeof(cmd_creature_changeOperation));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MAGIC_TYPE, onCommand_MagicType, typeof(cmd_creature_magicType));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MOVE_POS, onCommand_MovePos, typeof(cmd_creature_move_pos));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_STOP_MOVE_POS, onCommand_StopMovePos, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_TURN_TO_DIRECTION, onCommand_TurnToDirection, typeof(cmd_creature_turn_to_direction));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_SPELL_SLOT, onCommand_SetSpellSlot, typeof(cmd_creature_set_slot));//技能升级更新技能面板显示
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SPELL_POINT, onCommand_SpellPoint, typeof(cmd_creature_spell_point));//技能升级时更新技能升级槽
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HIDE_BODY, onCommand_HideBody, typeof(cmd_creature_hide_body));//隐藏实体，用于死亡效果等
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_WAR_PERSON_GOOD_INFO, onCommand_SetWarPersonGoodsInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_COMBO, onCommand_CombaAttac, typeof(cmd_war_persongoodsinfo));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT, onCommand_MapShortCut, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PLAYER_SKILL_STATE, onCommmand_Play_Skill_State, typeof(cmd_player_skill_state));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_FORBID_MOUSE_TURN_TO, onCommand_PauseMouseRotate, typeof(cmd_creature_forbid_mouse_turn_to));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_START_PK, onCommand_startPK, typeof(cmd_creature_pk_start_pk));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_STATE_WALL_WALKING, onCommand_wallWalking, typeof(cmd_Entity_Change_State_WallWalking));

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_ALPHA, onCommand_Set_Alpha, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_CAN_BUY_OR_SELL, onCommand_SetCanBuyOrSell, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_CANCEL_OPRATION, onCommand_SetCanCancelOpration, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SYNC_HEROVISION_LEVEL, onCommand_MainHeroVisionEntity_LevelChange, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_KILL_ICON, onCommand_ShowKill_Icon, null);           // 将连杀次数用图片实现数来
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PLAY_SOUND, onCommand_PlaySound, typeof(cmd_creature_play_sound));           // 将连杀次数用图片实现数来
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_REQUEST_PERSONINFO_COMPLETE, onCommand_OneRequestPersonInfoComplete, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHARGE_COUNT_CHANGE, onCommand_ChargeCountChange, typeof(cmd_buff_charge_count_change));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MATCH_ROOM_SHOW_SKILL_DESC, onCommand_MactchRoomSkillDesc, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHANGE_GOD_EYE_MODE, onCommand_ChangeGodEyeMode, typeof(cmd_creature_change_god_eye_mode));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_END_CREATE_RECORD, onCommand_PlayerData, null);      // 战后战绩表玩家数据更新
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SEND_UIDANDPUBUID, onCommand_SendUIDAndPBUID, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_MP_EFFECT_SPELL, onCommand_MpEffectSpell, typeof(cmd_creature_mp_effect_spell));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_PROMPT_UI, onCommand_ShowPromptUI, typeof(cmd_creature_ShowPromptUI));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_TALENT_PAGE_INFO_ALL, onCommand_Send_AllTalentPageInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_STICKY_CAMERRA, onCommand_Sticky_Camera, typeof(cmd_creature_Sticky_Camerra));
        //LightingEffectFactory.instance.LoadLightingEffectData(true);
        //AnimationEffectFactory.instance.LoadAnimEffectData();

        //注册实时更新数值属性的方法  FUNC_ENTITY_INTEGERPROPERTY_HANDLER(entity_update_Integerproperty data); 

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_DRAW_SPELL_DECAL, onCommand_DrawSpellDecal, typeof(cmd_creature_draw_spell_decal)); //技能距离提示

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_INFO, onCommand_Get_HeroInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_HERO_INFO, onCommand_Update_HeroInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PLAYER_BASE_INFO, onCommand_Get_PlayerBaseInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RETURN_HERO_DETAIL_INFO, onCommand_Return_HeroDetailInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RETURN_UPGRADE_CARD_INFO, onCommand_Return_Upgrade_CardInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_RUNE_PAGE_INFO_ALL, onCommand_Send_AllRunePageInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_WARSTATISTIC_TOTAL, onCommand_UpdateWarStatisticTotal, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_WARSTATISTIC_WAR, onCommand_UpdateWarStatisticWar, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPGRADESTARTAPROANDEXPEND, onCommand_Return_UpgradeStartProAdnExpend, typeof(cmd_entity_hero_detail_info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPGRADE_SUCESS, onCommand_Return_UpgradeSucess, typeof(cmd_Return_HeroDetail_Upgrade_Success));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RETURN_PLAYER_AWARK, onCommand_Return_PlayerAwark, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RETURN_STATICHERO_UPGRADE, onCommand_Return_StaticHeroUpgrade, typeof(cmd_entity_statichero_upgrade));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HINT_PLAYER_UPGRADE_STAR, onCommand_Hint_Player_Upgrade_Star, typeof(cmd_entity_is_show_hintinfo));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_FIRSTWIN_NOTICE, onCommand_Firstwin_Notice, typeof(cmd_entity_firstwin_notice));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_TITLE, onCommand_ShowTitleNode, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_TALENTINFO, onCommand_SetHeroTalentInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_TALENTUPDATE, onCommand_UpdateHeroTalentInfo, typeof(cmd_Hero_Talent_Info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_OBJ_RANGE_TIP, onCommand_SetObjRangeTip, typeof(cmd_entity_SetObjRangeTip));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_RUNESTOREUPDATE, onCommand_UpdateRuneStore, typeof(cmd_Rune_InStore_Item));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_OTHER_RUNE_PAGE_INFO_ALL, onCommand_Send_OtherAllRunePageInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_CHEST_INFO, onCommand_UpdateChestInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_EMOJI_INFO, onCommand_UpdateEmojiInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_OPEN_CHEST_RESULT, onCommand_OpenChestResult, typeof(cmd_entity_open_chest_result));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CHEST_RANDCARD_RESULT, onCommand_ChestRandCardResult, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_REQUEST_PK, onCommand_RequestPK, typeof(cmd_creature_pk_request_pk));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_PK_INFO, onCommand_ShowPKInfo, typeof(cmd_creature_pk_show_pk_info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_SKIN_INFO, onCommand_SetHeroSkinInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_SKIN_UPDATE, onCommand_UpdateHeroSkinInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_POINTSHOP_BUYNUM_INFO, onCommand_UpdatePointShopBuyNumInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_REFLUSH_POINTSHOP_BUYNUM_INFO, onCommand_ReflushPointShopBuyNumInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_POINTSHOP_BUY_RESULT, onCommand_PointShopBuyResult, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_POINTSHOP_UPDATE_CARDLIST, onCommand_PointShopUpdateCardlist, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ACTOR_INFO_CAHNGE, onCommand_ActorInfoChange, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_XP_RANDOMSELF_BEGINCOOL, onCommand_XPRandomSelfInCool, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_ACTIVE_TALENT_INRECONNECT, onCommand_HeroTalentActiveInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SPECIAL_ENTITY_SEARCH_STATE_CHANGE, onCommand_SpecialEntitySearchStateChange, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ENTITY_PROPERTY_DRAGONBALL, onCommand_EntityPropertyReturnDragonball, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HEROUPSTAR_UNLOCKPRIZE, onCommand_HeroUpstarUnlockPrize, typeof(cmd_entity_heroupstar_unlockprize));

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_CHOOSE_TARGET, onCommand_ShowChooseTarget, typeof(cmd_creature_ShowChooseTarget));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SPECCIAL_CONTRIBUTION, onCommand_BrodcastSpecialContribution, typeof(cmd_War_Special_Contribution));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RENAME_RESUNLE, onCommand_RenameResult, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_BENEFITCARD, onCommand_BenefitCard, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_GAMESTATE, onCommand_UpdatePlayerGameState, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_ALLGEMSTONE_UPDATE, onCommand_UpdateCrownAllGemstone, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_ALLCROWNPAGE_UPDATE, onCommand_UpdateAllCrownPage, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_GEMSTONE_UPDATE, onCommand_UpdateCrownGemstoneItem, typeof(cmd_Crown_Gemstone_Item_Info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_GEMSTONE_PRODUCE, onCommand_UpdateCrownProduct, typeof(cmd_Crown_Gemstone_Product_Info));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_GEMSTONE_OTHER_INLAY, onCommand_UpdateOtherAllCrownPage, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_CROWNG_GEMSTONE_APPRAISALRET, onCommand_CrownGemstoneAppraisalRet, typeof(cmd_Crown_Gemstone_Appraisal_Ret));

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_WAR_FILTER_SUPPORT_PERSON, onCommand_FilterSupportPlayer, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_WAR_ADD_LABEL, onCommand_AddLabel, typeof(cmd_update_support_vote_person));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_XP_SKILL_UPDATE, onCommand_UpdateHeroXPSkill, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_HERO_XP_FRAGMENT_UPDATE, onCommand_UpdateHeroXPFragment, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NPC_OPEN_SHOP, onCommand_NPCOpenShop, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NPC_RESPONSE, onCommand_NPCResponse, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NPC_CLOSE_DIALOGBOX, onCommand_NPCCloseDialogBox, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NOTIFY_DATA, onCommand_notifydata, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_LABEL_LIST, onCommand_UpdateLabelList, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_EQUIPSCHEME_LIST, onCommand_EquipSchemeList, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ACTIVE_TALENT_LIST, onCommand_ActiveTalentList, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SPELL_RECALC_TARGET_INFO, onCommand_ReClacSpellTargetInfo, typeof(cmd_spell_attribute));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_BOTTING_NOTIFY, onCommand_BottingNotify, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_WAR_INFO, onCommand_WarInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_AUTO_MOVE, onCommand_SkillAutoMove, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_WAR_END_LEGENDCUP_INFO, onCommand_WarEndLegendCupInfo, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SET_ALL_LIFEHERO, onCommand_SetAllLifeHero, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_LIFEHERO_PRIZE_TASKDATA, onCommand_UpdateLifeHeroPrizeTaskData, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_NOVICEGUIDEDATA, onCommand_UpdateNoviceGuideData, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_HISTORY_RECORD, onCommand_UpdateHistoryRecord, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_SHOW_DEBUFF_PROGRESS_BAR, onCommand_showDebuffProgressBar, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_UPDATE_LIFEHERO, onCommand_OnUpdateLifeHero, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_ACTOR_UNLOCK, onCommand_ActorUnlock, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RANK_DWON_OR_UPGRADE, onCommand_RankUpOrDown, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_NPC_BUY_GOODS_SUCCESS, onCommand_NPCBuyGoodsSuccess, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PROCESS_NAVGROUP, onCommand_processNavGroup, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_PROCESS_WEBURL, onCommand_processWebUrl, null);
        
        

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RANK_SEASON_DETAIL, onCommand_RankSeasonDetail, typeof(cmd_entity_rank_season_detail));
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RANK_SEASON_RECORD, onCommand_RankSeasonRecord, null);

        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_RECOMMEND_RECORD, onCommand_SetRecommendRecord, null);
        setHandler((int)EntityLogicDef.ENTITY_TOVIEW_GAIN_RECOMMPRIZE_SUCCESS, onCommand_GainRecommPrizeSuccess, null);

        //注册实时更新数值属性的方法  FUNC_ENTITY_INTEGERPROPERTY_HANDLER(entity_update_Integerproperty data); 
        // CreatureProperty.SetImmediateHandler(pid , handlerName);
    }

    public static bool onCommmand_Play_Skill_State(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommmand_Play_Skill_State=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiHeroSkillInfo = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, uiHeroSkillInfo);
        }
        return true;
    }

    private static string OrginalWindowText = "";
    private static string OldHeroName = "";
    public static bool onCommand_SetName(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        string name = strParam;
        if (name == null)
        {
            Trace.Log(string.Format("name==null"));
            return false;
        }

        CreatureProperty property = entity.GetComponent<CreatureProperty>();
        property.CreatureName = name;

        if (id == EntityFactory.MainHeroID)
        {
            if (!Application.isEditor)
            {

                System.Text.StringBuilder lpString = new System.Text.StringBuilder();
                if (string.IsNullOrEmpty(OrginalWindowText))
                {
                    GameLogicAPI.GetWindowText(Initialize.m_WindowHwnd, lpString, lpString.Capacity);
                    OrginalWindowText = lpString.ToString();

                }
                else
                {
                    lpString.Append(OrginalWindowText);
                }
                if (OldHeroName.Equals(strParam))
                {
                    return true;
                }
                OldHeroName = strParam;

                lpString.Append('-');
                lpString.Append('[');
                lpString.Append(strParam);
                lpString.Append(']');
                GameLogicAPI.SetWindowText(Initialize.m_WindowHwnd, lpString.ToString());
                lpString = null;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMMATEINFO_UPDATE_NAME, null);
        }

        //TopName tn = TopNameManager.FindTopName(id);
        //if (null == tn)
        //{
        //    TopNameManager.CreateTopName(entity, id, name, GFxCampManager.Instance.GetCampUColorFromType(nParam | (int)GFxCampManager.GFxCampRelated.CR_Name));
        //}
        //else
        //{
        //    TopNameManager.UpdateTopName(entity, id, name, GFxCampManager.Instance.GetCampUColorFromType(nParam | (int)GFxCampManager.GFxCampRelated.CR_Name));
        //}

        return true;
    }

    public static bool onCommand_ShowBlood(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null) return false;

        if(ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            Trace.Log("onCommand_ShowBlood:" + ev.ID);
        }

        if (GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
        {
            //没有在PK状态不创建
            if (!DataCenter.LogicDataCenter.pkDataManager.BIsPK)
                return false;

            if (USpeedUI.Blood.UBloodManager.Instance.PkTargetID != id)
                return false;

            //PK目标的阵营为敌方
            nParam = (int)USpeedUI.Blood.GFxCampTpye.CT_Enemy;
        }

        USpeedUI.Blood.UBlood ub = USpeedUI.Blood.UBloodManager.Instance.FindBlood(id);

        if (null == ub)
        {
            USpeedUI.Blood.UBloodManager.Instance.CreatBlood(ev, nParam);
        }
        else
        {
            USpeedUI.Blood.UBloodManager.Instance.UpdateBlood(ev);
        }

        return true;
    }

    //建立一个查找表，不用每次更新都GetCompoment
    //已废，移到EntityFactory
    //  static Dictionary<ENTITY_ID, BaseStateMachine> syncMap = new Dictionary<ENTITY_ID, BaseStateMachine>();
    public static bool onCommand_PositionSync(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sync = ev.StateMachine;
        if (!sync)
        {
            return false;
        }

        cmd_creature_rigidbody_sync data = IntPtrHelper.toData<cmd_creature_rigidbody_sync>(ptrParam);

        if (data.nCreatureState >= 0 && data.nCreatureState < (int)EntityState.MaxCount)
        {
            EntityState nOldState = sync.GetState();
            if (nOldState != EntityState.Dead)  // 死亡状态不能因为位置同步切到其他状态
            {
                // 转到新的状态
                if ((int)nOldState != data.nCreatureState)
                {
                    sync.ChangeState(data.nCreatureState, IntPtr.Zero);
                }
            }
        }

        //Trace.LogError(data.bParams1.ToString() + "," + data.fPosition_x + "," + data.fPosition_y + "," + data.fPosition_z);
       //Debug.Log(data.fVelocity_x + " " + data.fVelocity_y + " " + data.fVelocity_z);
        sync.PositionSync(ref data);
       // Debug.Log(data.fVelocity_x+" "+ data.fVelocity_y+ " " + data.fVelocity_z);
        Vector3 newPos = new Vector3(data.fPosition_x, data.fPosition_y, data.fPosition_z);
        EntityBatchCommunicator.SendEntityPro(id, newPos, new Vector3(data.fRotation_x, data.fRotation_y, data.fRotation_z));

        // 英雄同步位置时触发事件 或者怪物
        if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE || ev.Type == ENTITY_TYPE.TYPE_MONSTER)
        {
            MapFinderManager.MoveObject(ref ev, newPos);
        }

        return true;
    }

    public static bool onCommand_CTRL_Sync(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sync = ev.StateMachine;
        if (!sync)
        {
            return false;
        }

        cmd_creature_act_sync ctrl = IntPtrHelper.toData<cmd_creature_act_sync>(ptrParam);

        sync.CtrlSync(ref ctrl);
        return true;
    }

    public static bool onCommand_ChangeWingState(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine machine = ev.StateMachine;
        if (!machine)
        {
            return false;
        }

        machine.ChangeWingState(ptrParam);

        // 策划要求遭遇敌人反应要快 在服务器快速的搜索的话稍微耗一些。
        // 所以服务器里依然维持1秒的检测频率，快速的搜索工作放到客户端
        if (ev.IsHero)
        {
            cmd_Entity_Change_Wing_State param = IntPtrHelper.toData<cmd_Entity_Change_Wing_State>(ptrParam);
            if (param.nType == WING_STATE_TYPE.WING_STATE_START_FLY)
            {
                HeroDistanceMgr.instance.onFlyStateChange(true, param.nEncounterDistance);
            }
            else if (param.nType == WING_STATE_TYPE.WING_STATE_INVALID || param.nType == WING_STATE_TYPE.WING_STATE_FLOOR)
            {
                HeroDistanceMgr.instance.onFlyStateChange(false, param.nEncounterDistance);
            }
        }

        return true;
    }

    public static bool onCommand_ChangeDodgeState(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        CreatureStateMachine machine = null;
        if (ev.StateMachine is CreatureStateMachine)
        {
            machine = (CreatureStateMachine)ev.StateMachine;
        }

        if (!machine)
        {
            return false;
        }

        cmd_Entity_Change_Dodge_State param = IntPtrHelper.toData<cmd_Entity_Change_Dodge_State>(ptrParam);
        machine.dodgeSpeedGround = param.fSpeedUpGround;
        machine.dodgeSpeeds[0] = param.fSpeed0;
        machine.dodgeSpeeds[1] = param.fSpeed1;
        machine.dodgeSpeeds[2] = param.fSpeed2;
        machine.dodgeSkillID = param.nSpellID;
        if (param.nType != 0 && machine.creaturePropety != null)
        {
            machine.creaturePropety.enableDodge = true;
        }
        machine.dodge = (param.nType != 0);
        return true;
    }

    // 超级跳命令
    public static bool onCommand_LongJump(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        CreatureStateMachine machine = null;
        if (ev.StateMachine is CreatureStateMachine)
        {
            machine = (CreatureStateMachine)ev.StateMachine;
        }

        if (!machine)
        {
            return false;
        }

        machine.ChangeState((int)EntityState.Jumping, ptrParam);

        return true;
    }

    // 滑翔功能
    public static bool onCommand_UseGlide(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        // TODO
        if (ev == null)
        {
            return false;
        }
        //Debug.Log("start Glide!");
        BaseStateMachine machine = ev.StateMachine;
        if (!machine)
        {
            return false;
        }

        machine.UpdateGlideData(ptrParam);
        return true;
    }


    public static bool onCommand_PerspectiveShow(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        CreatureStateMachine machine = null;
        if (ev.StateMachine is CreatureStateMachine)
        {
            machine = (CreatureStateMachine)ev.StateMachine;
        }

        if (!machine || !machine.isHero) //只有主角才接收这个指令
        {
            return false;
        }

        cmd_creature_PerspectiveShow data = IntPtrHelper.toData<cmd_creature_PerspectiveShow>(ptrParam);
        machine.setXRayEffects(data);
        return true;
    }


    public static bool onCommand_ChangeSniperscope(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev.ID != EntityFactory.MainHeroID)
        {
            Trace.Error("onCommand_ChangeSniperscope ev.ID=" + ev.ID + ", EntityFactory.MainHeroID=" + EntityFactory.MainHeroID);
        }

        cmd_Entity_Change_Sniperscope data = IntPtrHelper.toData<cmd_Entity_Change_Sniperscope>(ptrParam);
        SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.SniperscopeControl, data.nType != 0);

        return true;
    }


    public static bool onCommand_startPK(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_pk_start_pk data = IntPtrHelper.toData<cmd_creature_pk_start_pk>(ptrParam);

        //Trace.Log("onCommand_startPK. bStart=" + data.bStart + ". uidTarget=" + data.uidTarget + ". nCamp=" + data.nCamp);
        if (data.bStart == 0)
        {
            LogicDataCenter.pkDataManager.EndPK((int)data.uidTarget, data.nResult, data.bForceEnd);
        }

        int pkTargetID = 0;
        if (data.uidTarget > 0 && data.bStart != 0)
        {
            pkTargetID = (int)data.uidTarget;
        }

        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null)
        {
            EntityFactory.MainHeroView.StateMachine.PK_targetID = pkTargetID;
        }

        if (data.uidTarget <= 0) return false;

        EntityView view = EntityFactory.getEntityViewByID((ENTITY_ID)data.uidTarget);
        if (view == null)
        {
            return false;
        }

        BaseStateMachine machine = view.StateMachine;
        MapFinderManager.RemoveObject(ref view);
        if (data.bStart == 0)
        {
            view.CampFlag = (SPELL.CampFlag)data.nCamp;
        }
        else
        {
            view.CampFlag = SPELL.CampFlag.CampFlag_Enemy; //开始PK时服务器发下来的是0，但是这里做敌方处理比较好
        }
        MapFinderManager.InsertObject(ref view, view.gameObject.transform.position);

        if (!machine)
        {
            return false;
        }
        machine.setPK(data.bStart != 0);

        if (data.bStart != 0)
        {
            LogicDataCenter.pkDataManager.StartPK((int)data.uidTarget, data.vLoc);
        }

        return true;
    }

    public static bool onCommand_wallWalking(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Debug.Log("onCommand_wallWalking.");
        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            sm.ChangeState((int)EntityState.WallWalking, ptrParam);
        }
        return true;
    }


    public static Queue<string> lightingEffectDebugInfo = new Queue<string>();
    static int maxDebugCount = 100;
    public static bool onCommand_AddLighting(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }
        ////光效开关
        //if (!Initialize.Instance || !OptimitzedControlPlane.Instance.EnabledLightingEffect)
        //{
        //    return true;
        //}
        cmd_creature_skillview effectContext = IntPtrHelper.toData<cmd_creature_skillview>(ptrParam);

        int nEffectID = nParam;
        EffectHelper.Play(ref ev, nEffectID, effectContext);
        if (lightingEffectDebugInfo.Count >= maxDebugCount)
        {
            lightingEffectDebugInfo.Dequeue();
        }

        string s = "EffectID:" + nEffectID + "--CreateTime:" + System.DateTime.Now.Hour + ":" + System.DateTime.Now.Minute + ":" + System.DateTime.Now.Second;
        lightingEffectDebugInfo.Enqueue(s);
        //CreatureProperty property = ev.Property;    // entity.GetComponent<CreatureProperty>();
        //int srcEntityID = id;
        //if (property != null)
        //{
        //    srcEntityID = property.Master.ID;
        //}

        //LightingEffectManager leManager = ev.LightingEffectManager; // entity.GetComponent<LightingEffectManager>();

        //if (leManager != null)
        //{
        //    if (leManager.showLog)
        //    {
        //        Trace.Log("Add Lighting" + nEffectID + ". Entity Id=" + srcEntityID);
        //        Trace.Log("TargetID=" + effectContext.nTargetEntityID + ". TargetPos=(" + effectContext.fTargetPos_x + "," + effectContext.fTargetPos_y + "," + effectContext.fTargetPos_z + ").");
        //    }
        //    LightEffectParam param = new LightEffectParam();
        //    param.nFeedbackID = effectContext.nFeedBackID;
        //    param.bIsHero = (EntityFactory.MainHeroID == srcEntityID);
        //    param.nSrcEntityID = srcEntityID;
        //    param.nTargetEntityID = effectContext.nTargetEntityID;
        //    param.srcObject = entity;
        //    param.CurveHorizontalMultiplier = effectContext.CurveHorizontalMultiplier;
        //    param.fAttackRange = effectContext.fAttackRange;
        //    param.speed = effectContext.fPASD;

        //    if (effectContext.nTargetEntityID != 0)
        //    {
        //        param.targetObject = EntityFactory.getEntityGameObjectByID(effectContext.nTargetEntityID);
        //    }

        //    param.targetPos = new Vector3(effectContext.fTargetPos_x, effectContext.fTargetPos_y, effectContext.fTargetPos_z);

        //    leManager.AddLighting(nEffectID, param);
        //}

        //ENTITY_TYPE t = (ENTITY_TYPE)property.type;
        //if (t == ENTITY_TYPE.TYPE_MONSTER)
        //{
        //    MonsterStateMachine sm = entity.GetComponent<MonsterStateMachine>();
        //    GameObject o = EntityFactory.find_entity(effectContext.nTargetEntityID);
        //    sm.FaceTo(o.transform.position);
        //}

        //下面示例代码开启物理破坏
        //if (entity == EntityFactory.MainHero)
        //{
        //    Vector3 target = new Vector3(effectContext.fTargetPos_x, effectContext.fTargetPos_y, effectContext.fTargetPos_z);
        //    float range = 250.0f;
        //    Vector3 towards = (target - entity.transform.position).normalized;
        //    AdorningManager.ApplyPhysics(target, range, towards);
        //}

        return true;
    }

    public static bool onCommand_RemoveLighting(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        int nEffectID = nParam;

        LightingEffectManager leManager = ev.LightingEffectManager;

        if (leManager != null)
        {
            leManager.RemoveLighting(nEffectID);
        }

        return true;
    }

    public static bool onCommand_AI_Roll(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null || ptrParam == null)
        {
            return false;
        }
        cmd_roll_2_target aiRollContext = IntPtrHelper.toData<cmd_roll_2_target>(ptrParam);
        if (aiRollContext.loc.Length < 3)
            return false;
        BaseStateMachine sm = ev.StateMachine;
        CreatureStateMachine machine = null;
        if (ev.StateMachine is CreatureStateMachine)
        {
            machine = (CreatureStateMachine)ev.StateMachine;
        }

        if (!machine || machine.isHero) //只有AI才接收这个指令
        {
            return false;
        }
        machine.RollToPos(new Vector3(aiRollContext.loc[0], aiRollContext.loc[1], aiRollContext.loc[2]));

        return true;
    }

    public static bool onCommand_PlayAnimation(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null || ptrParam == null)
        {
            return false;
        }

        cmd_creature_playAnimation animContext = IntPtrHelper.toData<cmd_creature_playAnimation>(ptrParam);
        string animName = animContext.szName;

        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            if (sm.showLog)
            {
                uint currentTick = GameLogicAPI.getTickCount();
                Trace.Log(currentTick.ToString() + "PlayAnim" + animName);
            }
            //设置动作目标，用作attackMove判断位移使用，但是对自己的动作不算
            if (sm.entityID != animContext.uidTarget)
            {
                sm.setAnimatorTarget((int)animContext.uidTarget);
            }
            else
            {
                sm.setAnimatorTarget(0);
            }
            if ((animName.Length > 2) && (animName[0] == 'W') && (animName[1] == '_'))
            {
                //跟策划规定， W_开头的是武器动作
                //跟策划规定， W_2_开头的是2号武器动作
                //string name = strParam.Substring(2, strParam.Length - 2);

                sm.PlayWeaponAnim(animName, (int)animContext.uidTarget, animContext.fPASD);
            }
            else if ((animName.Length >= 2) && (animName[0] == 'A') && (animName[1] == '_'))
            {
                //跟策划规定， A_开头的是有配置的复杂动作
                string name = animName.Substring(2, animName.Length - 2);
                int animID;
                int.TryParse(name, out animID);
                if (sm.showLog)
                {
                    Trace.Log("id=" + animID);
                }

                AnimationManager am = entity.GetComponent<AnimationManager>();
                if (am == null)
                {
                    am = entity.AddComponent<AnimationManager>();
                }
                if (animID > 0 && am != null)
                {
                    AnimationParam animparam = new AnimationParam();
                    animparam.pFSM = sm;
                    animparam.fSpeed = animContext.fPASD;
                    am.AddAnimation(animID, animparam);
                }
            }
            else
            {
                bool react = false;
                //跟策划规定， R_开头的是有受击动作，如果此时正在播放攻击动作则不播受击动作
                if ((animName.Length >= 2) && (animName[0] == 'R') && (animName[1] == '_'))
                {
                    react = true;
                    sm.setAnimatorTarget(0);
                }

                if (animContext.bAlreadyOver > 0)
                {
                    sm.PlayAnim(animName, react, animContext.fPASD, 0f, 0.99f);
                }
                else
                {
                    sm.PlayAnim(animName, react, animContext.fPASD);
                }
            }
        }
        return true;
    }

    public static bool onCommand_BreakAttackAnimation(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }
        cmd_creature_stopAnimation context = IntPtrHelper.toData<cmd_creature_stopAnimation>(ptrParam);

        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            sm.setAnimatorTarget(0);
            sm.BreakAttackAnim(context.szName);
        }
        return true;
    }


    public static bool onCommand_SwitchState(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Debug.Log("onCommand_SwitchState");
        mainHeroState = nParam;


        //SoldierController sc = entity.GetComponent<SoldierController>();

        //if (sc == null)
        //{
        //    return true;
        //}

        //if (sc.solidermode == 2)//控制模式2的时候实际初始默认1，进战斗才2（策划特殊需求）
        //{
        //    if (nParam == (int)ENTITY_STATE.ENTITY_STATE_FIGHT)
        //    {
        //        sc.controlmode = 2;
        //    }
        //    else
        //    {
        //        sc.controlmode = 1;
        //    }
        //}
        //else
        //{
        //    sc.controlmode = sc.solidermode;
        //}

        return true;
    }
    public static bool onCommand_Charge(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sm = ev.StateMachine;  // entity.GetComponent<BaseStateMachine>();
        if (sm != null)
        {
            cmd_force_move context = IntPtrHelper.toData<cmd_force_move>(ptrParam);
            //Vector3 targetPos = new Vector3(context.fPosition_x, context.fPosition_y, context.fPosition_z);
            //sm.ForceMove(targetPos, (int)(context.fTime * 1000),context.fHeight);            
            if (float.IsNaN(context.fPosition_x) || float.IsNaN(context.fPosition_y) || float.IsNaN(context.fPosition_z) || Math.Abs(context.fPosition_x) > SceneManager.MAX_SCENE_SIZE || Math.Abs(context.fPosition_y) > SceneManager.MAX_SCENE_SIZE || Math.Abs(context.fPosition_z) > SceneManager.MAX_SCENE_SIZE)
            {
                Vector3 targetPos = new Vector3(context.fPosition_x, context.fPosition_y, context.fPosition_z);
                Debug.LogError(sm.name + " cmd Charge Position Error " + targetPos);
            }
            else
            {
                sm.ForceMove(context);
            }

        }
        return true;
    }
    public static bool onCommand_Flight(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            cmd_effect_Flight context = IntPtrHelper.toData<cmd_effect_Flight>(ptrParam);
            sm.ForceFlight(context);
        }
        return true;
    }

    public static bool onCommand_MovePos(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            cmd_creature_move_pos context = IntPtrHelper.toData<cmd_creature_move_pos>(ptrParam);
            Vector3 targetPos = new Vector3(context.end[0], context.end[1], context.end[2]);
            sm.AutoMoveTo(targetPos);
        }
        return true;
    }

    public static bool onCommand_StopMovePos(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }
        BaseStateMachine sm = ev.StateMachine;
        if (sm != null)
        {
            sm.StopAutoMove(false);
        }
        return true;
    }


    public static bool onCommand_ChangeSlot(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeSlot=================== : {0}", nParam));
        }
        else
        {
            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                LogicDataCenter.warOBUIDataManager.ChangeSlot(ev, ptrParam);
            }
            UIMsgCmdData uiSpellEffect = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, uiSpellEffect);
        }
        return true;
    }

    //提示技能使用距离和范围
    public static bool onCommand_DrawSpellDecal(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_draw_spell_decal data = IntPtrHelper.toData<cmd_creature_draw_spell_decal>(ptrParam);
        SkillCtrl sc = entity.GetComponent<SkillCtrl>();
        if (sc != null)
        {
            sc.showTipSelectEffect(data);
        }
        return true;
    }

    public static bool onCommand_Get_HeroInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Get_HeroInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.UpdataAllHero(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_Update_HeroInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Update_HeroInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.UpdateHeroInfo(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_Get_PlayerBaseInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Get_PlayerBaseInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.UpdatePlayerInfo(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_Return_HeroDetailInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Return_HeroDetailInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.playerSystemDataManager.UpdateHeroDetailInfo(ptrParam, nPtrLen);
        }

        return true;
    }

    public static bool onCommand_Return_Upgrade_CardInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (EntityFactory.MainHeroView == null)
            return false;

        LogicDataCenter.playerSystemDataManager.updateHeroCard(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_SetSpellAttribute(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_spell_attribute data = IntPtrHelper.toData<cmd_spell_attribute>(ptrParam);

        SkillCtrl sc = ev.SkillCtrl;
        if (sc != null)
        {
            sc.onSetSpellAttribute(data);
        }
        return true;
    }

    public static bool onCommand_PauseMouseRotate(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_forbid_mouse_turn_to data = IntPtrHelper.toData<cmd_creature_forbid_mouse_turn_to>(ptrParam);
        SoldierController sc = entity.GetComponent<SoldierController>();
        if (sc != null)
        {
            sc.pauseMouseRotate(data.nTime);
        }
        return true;
    }


    public static bool onCommand_EffectStrafe(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_effect_Strafe data = IntPtrHelper.toData<cmd_effect_Strafe>(ptrParam);
        SkillCtrl sc = entity.GetComponent<SkillCtrl>();
        if (sc != null)
        {
            sc.onStrafe(data);
        }
        return true;
    }

    public static bool onCommand_SetCastSpellPos(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_castSpellPos data = IntPtrHelper.toData<cmd_creature_castSpellPos>(ptrParam);
        BaseStateMachine fsm = entity.GetComponent<BaseStateMachine>();
      
        if (0 == (data.nCastType & ((int)SPELL_CAST_TYPE.SpellCastType_NoTurn)))
        {
            if (fsm != null)
            {
                Vector3 targetPos = new Vector3(data.fPosition_x, data.fPosition_y, data.fPosition_z);
                fsm.FaceTo(targetPos);
            }
        }

        SoldierController soliderctrl = entity.GetComponent<SoldierController>();
        if (soliderctrl != null)
        {
            soliderctrl.pauseCtrl(data.nRigidityTime, data.nMoveRigidityTime, data.nTurnRigidityTime, data.nCastType);
        }

        return true;
    }

    public static bool onCommand_SpellBreak(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        SoldierController soliderctrl = entity.GetComponent<SoldierController>();
        if (soliderctrl != null)
        {
            soliderctrl.stopPauseCtrl();
        }
        return true;
    }

    public static bool onCommand_TurnToDirection(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_turn_to_direction data = IntPtrHelper.toData<cmd_creature_turn_to_direction>(ptrParam);
        BaseStateMachine fsm = entity.GetComponent<BaseStateMachine>();
        if (fsm != null)
        {
            Vector3 targetPos = new Vector3(data.ptLoc[0], data.ptLoc[1], data.ptLoc[2]);
            fsm.FaceTo(targetPos);
        }
        return true;
    }

    public static bool onCommand_StopMoveHandle(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        SoldierController soliderctrl = entity.GetComponent<SoldierController>();
        if (soliderctrl != null)
        {
            soliderctrl.resetAxisCtrl(nParam);
        }

        return true;
    }


    public static bool onCommand_StopMove(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        SoldierController sc = entity.GetComponent<SoldierController>();
        //Trace.Log("onCommand_StopMove"+nParam);
        if (sc != null)
        {
            sc.SetStopMove(true, nParam);
        }
        return true;
    }

    public static bool onCommand_ResumeMove(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        SoldierController sc = entity.GetComponent<SoldierController>();
        //Trace.Log("onCommand_ResumeMove" + nParam);
        if (sc != null)
        {
            sc.SetStopMove(false, nParam);
        }
        return true;
    }

    public static bool onCommand_NoTurnTo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        SoldierController sc = entity.GetComponent<SoldierController>();
        if (sc != null)
        {
            cmd_creature_NoTurnTo data = IntPtrHelper.toData<cmd_creature_NoTurnTo>(ptrParam);
            //Trace.Log("onCommand_StopRotate. Type=" + data.nType.ToString() + "BuffID="+data.nBuffID.ToString());
            bool stopRotate = (data.nType == 0);
            sc.SetStopRotate(stopRotate, data.nBuffID);
        }
        return true;
    }

    public static bool onCommand_NoSelect(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        BaseStateMachine machine = entity.GetComponent<BaseStateMachine>();
        if (machine != null)
        {
            cmd_creature_NoSelect data = IntPtrHelper.toData<cmd_creature_NoSelect>(ptrParam);
            bool unpickable = (data.nType == 0);
            machine.SetPickableBuff(unpickable, data.nBuffID, data.nSelectTargetFilter);
        }
        return true;
    }

    public static bool onCommand_Relive(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }
        if (ev.IsHero)
            SoldierCamera.MainInstance<SoldierCamera>().isDead = false;
        BaseStateMachine sm = ev.StateMachine;
        if (sm != null && sm.GetState() == EntityState.Dead)
        {
            sm.ChangeState((int)EntityState.Standing, IntPtr.Zero);
        }

        if (id != EntityFactory.MainHeroID)
        {
            return true;
        }

        UIMsgCmdData uiRemoveReliveTimeData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARRELIVETIME_REMOVERELIVETIME, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_REMOVERELIVETIME, uiRemoveReliveTimeData);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_CLOSE, null);

        SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.MapGodControl, false);

        return true;
    }

    public static bool onCommand_SetBuff(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeSlot=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiBuffData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, uiBuffData);
        }

        return true;
    }

    public static bool onCommond_SyncForceSetPos(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //Trace.Log(string.Format("==================onCommond_SyncForceMove==================="));
        if (entity == null || entity.transform == null || entity.transform.eulerAngles == null)
        {
            Trace.LogError("EntityViewCommandHandler.cs  onCommond_SyncForceSetPos entity == null || entity.transform == null || entity.transform.eulerAngles == null");
            return true;
        }

        cmd_creature_transport data = IntPtrHelper.toData<cmd_creature_transport>(ptrParam);

        BaseStateMachine bs = entity.GetComponent<BaseStateMachine>();
        if (bs)
        {
            bs.Transport(data);

            if (ev.IsHero)
            {
                // 通知服务器从现在开始的同步消息都是最新的了
                // （服务器下发传送后，可能被队列(客户端显示层到逻辑层队列、网络上正在传输的、服务器actor消息队列)里的旧的同步消息同步到错误的位置，所以服务器传送后加强了位置校验）
                bs.SyncPos2Server();
            }

            Vector3 pos = new Vector3(data.fPosition_x, data.fPosition_y, data.fPosition_z);
            EntityBatchCommunicator.SendEntityPro(id, pos, entity.transform.eulerAngles);
            // 立即刷新小地图位置
            EntityPosAngleInfo info;
            info.id = id;
            info.pos = pos;
            info.dir = entity.transform.eulerAngles;
            LogicDataCenter.warMinimapDataManager.UpdateObjectPos(ev, info);
        }

        // 通知主角位置发生变化
        if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.ID == id)
        {
            U3D_Render.EntityLocUpdate.SendActorLocationChange(new Vector3(data.fPosition_x, data.fPosition_y, data.fPosition_z));
        }

        return true;
    }

    public static bool onCommond_SetSpellOverlay(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_SetSpellOverlay=================== : {0}", nParam));
        }
        else
        {
            Trace.Log("onCommond_SetSpellOverlay");
            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                LogicDataCenter.warOBUIDataManager.SetSpellOverlay(ev, ptrParam);
            }
            UIMsgCmdData uiSpellOverEffect = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, uiSpellOverEffect);
        }
        return true;
    }


    // 服务器设置移动速度
    public static bool onCommond_SetMovingSpeed(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        BaseStateMachine fsm = entity.GetComponent<BaseStateMachine>();
        if (fsm != null)
        {
            //与策划商定，前进和左右的最大速度为输入速度，后退最大速度为输入速度的一半
            //nParam为输入速度放大100倍
            if (fsm.showLog)
            {
                Trace.Log("onCommond_SetMovingSpeed " + nParam.ToString());
            }
            fsm.maxRunForwardSpeed = nParam / 100.0f;
            fsm.maxRunBackwardSpeed = nParam / 200.0f;
            fsm.maxRunSidewaySpeed = nParam / 100.0f;
        }
        return true;
    }

    // 设置动画
    public static bool onCommond_SetAnimation(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine sync = ev.StateMachine;
        if (!sync)
        {
            return false;
        }

        cmd_entity_set_animation data = IntPtrHelper.toData<cmd_entity_set_animation>(ptrParam);

        bool bEnable = (data.bEnable != 0);
        sync.SetAnimatorBool(data.szAnimationBool, bEnable);

        if (bEnable)
        {
            sync.AnimatorPlay(data.szAnimationName);
        }

        return true;
    }

    // 接受人物信息
    public static bool onCommond_RicvePersonInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_RicvePersonInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.recordTableDataManager.UpdatePlayerInfo(ptrParam, nPtrLen);
            LogicDataCenter.warOBDataManager.UpdateCampPlayer(ptrParam, nPtrLen);
        }
        return true;
    }

    // 广播连杀
    public static bool onCommand_BroadKill(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeSlot=================== : {0}", nParam));
        }
        else
        {
            cmd_war_entity_killcountinfo data = IntPtrHelper.toData<cmd_war_entity_killcountinfo>(ptrParam);

            if (data.uidMaster == EntityFactory.MainHeroID)
            {
                EntityView murderEv = EntityFactory.getEntityViewByID(data.uidMurderer);
                if (murderEv != null)
                {
                    int skinID = murderEv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
                    LogicDataCenter.deathReplayDataManager.MurderSkinID = skinID;
                }
            }
            UIMsgCmdData uiBroadKillCountData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT, uiBroadKillCountData);
        }
        return true;
    }

    // 广播连击
    public static bool onCommand_CombaAttac(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CombaAttac=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiShowComboAttackData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWATTACK, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWATTACK, uiShowComboAttackData);
        }
        return true;
    }

    // 广播重要实体击杀
    public static bool onCommand_BroadImprotantEt(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeSlot=================== : {0}", nParam));
        }
        else
        {
            cmd_war_important_entiydieinfo data = IntPtrHelper.toData<cmd_war_important_entiydieinfo>(ptrParam);
            if (data.uidMaster == EntityFactory.MainHeroID)
            {
                EntityView murderEv = EntityFactory.getEntityViewByID(data.uidMurderer);
                if (murderEv != null)
                {
                    int skinID = murderEv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
                    LogicDataCenter.deathReplayDataManager.MurderSkinID = skinID;
                }
            }
            UIMsgCmdData uiBroadImportantEVData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV, uiBroadImportantEVData);
        }
        return true;
    }

    // boss刷新时间
    public static bool onCommond_UpdateMonsterTimer(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeSlot=================== : {0}", nParam));
        }
        else
        {
            cmd_war_monsterinfo data = IntPtrHelper.toData<cmd_war_monsterinfo>(ptrParam);
            LogicDataCenter.recordTableDataManager.UpdateBossTimer(data.nTableIndex, data.nTime);

            // 通知小地图倒计时
            LogicDataCenter.warMinimapDataManager.CreateCountDown(data.nIconIndex, data.nTime);
        }
        return true;
    }

    // 战场结束胜利失败界面显示
    public static bool onCommond_ShowWarEndWinOrFail(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_ShowWarEnd=================== : {0}", nParam));
        }
        else
        {
            cmd_show_win_or_fail data = IntPtrHelper.toData<cmd_show_win_or_fail>(ptrParam);

            Debug.Log("onCommond_ShowWarEndWinOrFail,WND_MSG_COMMON_WAREND");

            // 播放结束动画
            if (SoldierCamera.MainInstance<SoldierCamera>() != null && data.isPlayEndAnim > 0)
            {
                WarEndStateParam param = new WarEndStateParam(new Vector4(data.fBasePos_x, data.fBasePos_y, data.fBasePos_z), data.nWin);
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.EndWarControl, true, param);
            }

            // 发送战场结束消息
            UCommonWarEndMsgData msgData = new UCommonWarEndMsgData();
            msgData.isSelfWin = data.nWin > 0 ? true : false;
            msgData.nUseTime = data.nUseTime;
            msgData.DelayTime = GameLogicAPI.getWarEndWndDelayTime();
            msgData.isShowResultWnd = data.isShowResultWnd > 0;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, msgData);

            // 战场结束清空内容
            HeroDistanceMgr.instance.release();
            EntityTimeSearchManager.instance.release();

            // 战场结束,不许玩家操作键盘
            InputManager.Available = false;

            // 销毁战场音效
            SoundManager.DeleteAll();

            // 设置战场结束状态
            StageManager.Instance.CurSubState = EStage_SubState.ESSS_WAREND;
        }

        return true;
    }

    // 实体进入视野
    public static bool onCommond_EnterView(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        //OB局由WarOBDataManager统一管理视野，不走这里
        if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB && StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAR)
            return false;

        if (nParam != 1)
        {
            ev.StateMachine.OnEnterView();
        }

        EntityView eEntity = EntityFactory.getPlayerViewByID(id);
        if (eEntity != null && eEntity.CampFlag == CampFlag.CampFlag_Enemy)
        {
            HeroDistanceMgr.instance.setEnemyEntity(eEntity, true);

        }
        MapFinderManager.InsertObject(ref ev, ev.gameObject.transform.position);
        // 小地图显示隐藏
        if (ev.StateMachine.GetState() != EntityState.Dead)
        {
            LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(ev);
        }
        return true;
    }

    // 实体离开视野
    public static bool onCommond_LeaveView(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        //OB局由WarOBDataManager统一管理视野，不走这里
        if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB && StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAR)
            return false;

        // 是防御塔，显示层直接不处理离开视野
        //         if (ev.Type == ENTITY_TYPE.TYPE_MONSTER && (ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER || ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE))
        //         {
        //             return true;
        //         }

        Vector3 loc = ev.StateMachine.transform.position;
        if (nParam != 1)
        {
            ev.StateMachine.OnLeaveView();
        }

        EntityView eEntity = EntityFactory.getPlayerViewByID(id);
        if (eEntity != null && eEntity.CampFlag == CampFlag.CampFlag_Enemy)
        {
            HeroDistanceMgr.instance.setEnemyEntity(eEntity, false);
        }

        MapFinderManager.RemoveObject(ref ev);
        // 小地图显示隐藏
        LogicDataCenter.warMinimapDataManager.UpdateObjectVisiblity(ev);
        return true;
    }

    // 改变皮肤
    public static bool onCommond_ChangeSkin(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (!entity) return false;
        SkillCtrl sc = entity.GetComponent<SkillCtrl>();
        if (sc != null) sc.resetOrderset(1);

        return true;
    }

    public static bool onCommand_MarkMinimap(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MarkMinimap=================== : {0}", nParam));
        }
        else
        {
            cmd_war_mini_map_info data = IntPtrHelper.toData<cmd_war_mini_map_info>(ptrParam);

            // 添加小地图标记动画
            LogicDataCenter.warMinimapDataManager.MarkMinimap(data.x, data.z, data.nType);

            // 此处把信号实体放到大地图 TODO
            if (Math.Abs(data.y) < 0.01f)
            {
                data.y = 50.0f; //0的话是服务器随便给的
            }

            Vector3 pos = new Vector3(data.x, data.y, data.z);
            EntityView targetView = null;
            if (data.nEntityID > 0)
                targetView = EntityFactory.getEntityViewByID(data.nEntityID);

            if (targetView != null && targetView.IsValid)
            {
                int attachLightEffectID = LogicDataCenter.warMinimapDataManager.GetMinimapSignalEffectID((EMinimapClickType)data.nType, true);

                string targetName = targetView.Property.CreatureName;
                if (targetView.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                {
                    // 英雄
                    if (data.nType == (int)EMinimapClickType.EMCT_AttackSignal)
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_A_ENEMY_HERO, data.PlayerName + ";" + data.VocationName + ";" + targetName + ";" + data.TargetVocationName);
                    else if (data.nType == (int)EMinimapClickType.EMCT_RetreatSignal)
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_R_ENEMY_HERO, data.PlayerName + ";" + data.VocationName + ";" + targetName + ";" + data.TargetVocationName);
                }
                else if (targetView.Type == ENTITY_TYPE.TYPE_MONSTER)
                {
                    if (targetView.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER || targetView.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE)
                    {
                        // 防御塔
                        if (targetView.CampFlag == CampFlag.CampFlag_Friend)
                        {
                            if (data.nType == (int)EMinimapClickType.EMCT_RetreatSignal)
                                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_R_FRIEND_TOWER, data.PlayerName + ";" + data.VocationName + ";" + targetName);
                        }
                        else
                        {
                            if (data.nType == (int)EMinimapClickType.EMCT_AttackSignal)
                                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_A_ENEMY_TOWER, data.PlayerName + ";" + data.VocationName + ";" + targetName);
                            else if (data.nType == (int)EMinimapClickType.EMCT_RetreatSignal)
                                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_R_ENEMY_TOWER, data.PlayerName + ";" + data.VocationName + ";" + targetName);
                        }
                    }
                    else if (targetView.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_WILD_MONSTER)
                    {
                        // 野怪
                        if (data.nType == (int)EMinimapClickType.EMCT_AttackSignal)
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_A_ENEMY_MONSTER, data.PlayerName + ";" + data.VocationName + ";" + targetName);
                        else if (data.nType == (int)EMinimapClickType.EMCT_RetreatSignal)
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_R_ENEMY_MONSTER, data.PlayerName + ";" + data.VocationName + ";" + targetName);
                    }
                }

                // 添加光效
                EffectHelper.Play(ref targetView, attachLightEffectID, (int)targetView.ID, 0, pos);
            }
            else
            {
                int groundLightEffectID = LogicDataCenter.warMinimapDataManager.GetMinimapSignalEffectID((EMinimapClickType)data.nType, false);

                if (data.nType == (int)EMinimapClickType.EMCT_DangerSignal)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_DANGER, data.PlayerName + ";" + data.VocationName);
                }
                if (data.nType == (int)EMinimapClickType.EMCT_OnWaySignal)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_ONWAY, data.PlayerName + ";" + data.VocationName);
                }
                if (data.nType == (int)EMinimapClickType.EMCT_HelpMeSignal)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_HELP, data.PlayerName + ";" + data.VocationName);
                }
                if (data.nType == (int)EMinimapClickType.EMCT_LostEyeSignal)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_MAPSIGN_LOST, data.PlayerName + ";" + data.VocationName);
                }

                // 添加地面光效
                int LayerMaskOnPos = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding));
                Ray PointToGroundRay = new Ray(pos + new Vector3(0, 0.5f, 0), new Vector3(0, -1, 0));
                RaycastHit projectInfo;
                Physics.Raycast(PointToGroundRay, out projectInfo, 100, LayerMaskOnPos);
                pos = projectInfo.point;

                LightEffectParam param = new LightEffectParam();
                param.nSrcEntityID = ev.ID;
                param.sourcePos = pos;
                param.targetPos = pos;
                LightingEffectManager lem = ev.LightingEffectManager;
                if (lem != null && lem)
                    lem.AddLighting(groundLightEffectID, param);
            }
        }

        return true;
    }

    public static bool onCommond_ClearAllMiniMapIcon(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.warMinimapDataManager.ClearStaticObj();
        return true;
    }

    public static bool onCommand_Dmage(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_damage data = IntPtrHelper.toData<cmd_creature_damage>(ptrParam);

        U3D_Render.EntityView evSrc = EntityFactory.getEntityViewByID(Convert.ToInt32(data.uidOperator));
        U3D_Render.EntityView evDst = ev;

        if (null == evSrc || null == evDst)
        {
            return true;
        }

        if (evDst.IsHero && evSrc != evDst)
        {
            //主角被打，记录
            ScreenRaycast rayCast = evDst.gameObject.GetComponent<ScreenRaycast>();
            if (rayCast != null)
            {
                rayCast.onDamage(evSrc);
            }
            //主角承受伤害数据进行记录，以便统计死亡回放数据
            LogicDataCenter.deathReplayDataManager.AddHurtData(evSrc, data);
        }

        #region 伤害飘字
        //如果是伤害飘字，通知血条管理器做大血条抖动
        if ((data.nAttackType & (int)EAttackResultType.AttackResult_Damage) != 0)
            USpeedUI.Blood.UBloodManager.Instance.OnEntityHit(evSrc.ID, evDst.ID);

        //怪物打怪物没有飘字
        if (evSrc.Type == ENTITY_TYPE.TYPE_MONSTER && evDst.Type == ENTITY_TYPE.TYPE_MONSTER)
            return true;

        //int camp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
        //友方攻击别人不飘
        if (evSrc.CampFlag == CampFlag.CampFlag_Friend && evDst.CampFlag != CampFlag.CampFlag_Self)
            return true;

        //友方受到别人攻击不飘
        if (evDst.CampFlag == CampFlag.CampFlag_Friend && evSrc.CampFlag != CampFlag.CampFlag_Self)
            return true;

        //敌方打敌方不飘
        if (evSrc.CampFlag == CampFlag.CampFlag_Enemy && evDst.CampFlag == CampFlag.CampFlag_Enemy)
            return true;

        if (data.nDeltaHP == 0)
            return true;

        //不在视野内的不显示飘字
        if (ev.StateMachine)
        {
            if (!ev.StateMachine.isOnView_Logic())
                return true;
        }

        if ((0 != (data.nMask & (int)ENTITY_MASKFLAG.MASKFLAG_SHOW_EFFECT)) && (0 != (data.nSpellFlag & (int)SPELL_FLAG.SpellFlag_MonsterWhiteEffect)))
        {
            if (evDst.StateMachine != null)
            {
                //怪物闪白色
                evDst.StateMachine.setHightLight(true);
                evDst.StateMachine.ShineScale();
            }
        }

        WarFloatFontType fontType = WarFloatFontType.Invalid;

        if ((data.nSpellEffectType & (int)SPELL_EFFECT_TYPE.SpellEffectType_PhysicalAttack) != 0)
            fontType = WarFloatFontType.PhysicalAttack;

        else if ((data.nSpellEffectType & (int)SPELL_EFFECT_TYPE.SpellEffectType_MagicAttack) != 0)
            fontType = WarFloatFontType.MagicAttack;

        else if ((data.nSpellEffectType & (int)SPELL_EFFECT_TYPE.SpellEffectType_TrueDamage) != 0)
            fontType = WarFloatFontType.TrueDamage;

        if ((data.nAttackType & (int)EAttackResultType.AttackResult_Fatal) != 0)
            fontType = WarFloatFontType.Fatal;

        else if ((data.nAttackType & (int)EAttackResultType.AttackResult_Cure) != 0)
            fontType = WarFloatFontType.Cure;

        //FloatFontManager.CreateFloatFont(entity, data.nDeltaHP.ToString(), fontType);
        UWarFloatFontData fontData = new UWarFloatFontData((int)WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, entity, data.nDeltaHP.ToString(), fontType);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, fontData);

        #endregion

        #region 护盾检测
        //是近战攻击，远程攻击的护盾检测是在飞行光效里面做
        if ((data.nSpellEffectType & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_Infighting) != 0)
        {
            if (evSrc.SkinControl != null && evSrc.SkinControl.CurrentSkinInstance != null && evSrc.StateMachine != null && evDst.StateMachine != null)
            {
                EntitySkinConfig evSrcSkinConfig = evSrc.SkinControl.CurrentSkinInstance.entitySkinConfig;

                //攻击者的位置,用来在两点之间做射线检测
                Vector3 SrcPos;

                //如果有武器，那么攻击者的位置挪到武器
                if (evSrcSkinConfig.RightWeaponRender)
                {
                    SrcPos = evSrcSkinConfig.RightWeaponRender.transform.position;
                }
                else
                {
                    //如果没有，直接取状态机的位置+上高度的一半
                    SrcPos = evSrc.StateMachine.transform.position;
                    SrcPos.y += evSrc.Property.CreatureHeightInMeters * 0.5f;
                }


                //受击着的位置,用来在两点之间做射线检测
                Vector3 DstPos;

                //如果有wound点
                if (evDst.StateMachine.wound)
                {
                    DstPos = evDst.StateMachine.wound.position;
                }
                else
                {
                    //如果没有，直接取状态机的位置+上高度的一半
                    DstPos = evDst.StateMachine.transform.position;
                    DstPos.y += evDst.Property.CreatureHeightInMeters * 0.5f;
                }

                ////在两点之间做护盾检测
                //RaycastHit hit;
                //if (Physics.Linecast(SrcPos, DstPos, out hit, 1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)))
                //{
                //    GameObject go = hit.transform.gameObject;
                //    Effect.ShieldBase fb = null;
                //    fb = LightingEffectFactory.Instance.GetLightingEffectShield(go);

                //    if (fb)
                //    {
                //        fb.OnHit(hit.point);
                //    }
                //}

            }
        }
        #endregion

        return true;
    }


    // 更新数值属性处理函数
    public static bool onCommand_UpdateIntegerProperty(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateIntegerProperty=================== : {0}", nParam));
        }
        else
        {
            if (ev != null)
            {
                entity_update_Numproperty data = IntPtrHelper.toData<entity_update_Numproperty>(ptrParam);
                return ev.OnCommand_UpdateNumProp(data);
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    // 创建、销毁小地图静态打点
    public static bool onCommand_CreateStaticIcon(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            return false;
        }

        cmd_war_mini_map_icon data = IntPtrHelper.toData<cmd_war_mini_map_icon>(ptrParam);
        if (data.bIsShow != 0)
        {
            LogicDataCenter.warMinimapDataManager.AddStaticObj(data.nIconIndex);
        }
        else
        {
            LogicDataCenter.warMinimapDataManager.RemoveStaticObj(data.nIconIndex);
        }

        return true;
    }

    //龙珠显示坐标
    public static bool onCommand_ShowDragonPosition(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_pos data = IntPtrHelper.toData<cmd_pos>(ptrParam);

        EntityView objev = EntityFactory.getEntityViewByID((int)data.uUID);
        if (objev != null && objev.gameObject != null)
        {
            BaseStateMachine fsm = objev.gameObject.GetComponent<BaseStateMachine>();
            if (fsm != null)
            {
                Vector3 pos = new Vector3(data.fPosition_x, data.fPosition_y, data.fPosition_z);
                fsm.onRecivePositoinCheck(pos);
            }
        }

        return true;
    }


    //战场玩家复活时间,该接口已经废弃
    public static bool onCommond_ReliveTime(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_ReliveTime=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiReliveData = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMOM_WARHERORELIVETIME, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_WARHERORELIVETIME, uiReliveData);

        }

        return true;
    }

    //战场玩家复活时间
    public static bool onCommond_BroadCastMonsterRelive(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_BroadCastMonsterRelive=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiMonsterReliveData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE, uiMonsterReliveData);
        }

        return true;
    }

    //战场玩家复活时间
    public static bool onCommond_BroadCastMonsterDieText(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommond_ReliveTime=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiMonsterDieData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE, uiMonsterDieData);
        }

        return true;
    }

    public static bool onCommond_GotoGuard(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine bsm = ev.StateMachine;
        if (!bsm)
        {
            return false;
        }

        bsm.gotoGuard(nParam != 0);

        return true;
    }

    public static bool onCommond_EntityDead(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine bsm = ev.StateMachine;
        if (!bsm)
        {
            return false;
        }
        bsm.OnEntityDead(ptrParam);
        if (ev.IsHero)
            SoldierCamera.MainInstance<SoldierCamera>().isDead = true;
        if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            // 复活时间
            cmd_creature_die context = IntPtrHelper.toData<cmd_creature_die>(ptrParam);
            int rebornTick = context.nInterval - context.nDeadInterval;

            if (context.isNoRelive == 0)
            {
                UIWarShowReliveTimeData uiWarShowReliveTimeData = new UIWarShowReliveTimeData();
                uiWarShowReliveTimeData.msgID = (int)WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME;
                uiWarShowReliveTimeData.nReliveTime = rebornTick / 1000;
                uiWarShowReliveTimeData.uid = id;
                uiWarShowReliveTimeData.pdbid = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                uiWarShowReliveTimeData.isHero = ev.IsHero;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, uiWarShowReliveTimeData);

                // 死亡回放
                if (ev.IsHero)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOW, null);
                }
            }
        }

        return true;
    }

    public static bool onCommond_EntityDeadRelogin(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        BaseStateMachine bsm = ev.StateMachine;
        if (!bsm)
        {
            return false;
        }
        //bsm.OnEntityDead(ptrParam);  应该走一遍流程的？
        if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
        {
            if (!ev.IsHero)
                ev.StateMachine.SetVisible(false);
            else
            {
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FullScreenSaturation>();
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();
            }
        }

        return true;
    }

    public static bool onCommand_DisableSpell(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_DisableSpell=================== : {0}", nParam));
        }
        else
        {
            cmd_creature_disable_spell data = IntPtrHelper.toData<cmd_creature_disable_spell>(ptrParam);
            bool bLock = (data.nType == 0);
            SkillCtrl ctrl = entity.GetComponent<SkillCtrl>();
            if (ctrl)
            {
                for (int i = 0; i < data.nSpellCount; i++)
                {
                    ctrl.enableSlot(data.nSlotType[i], data.nSlotIndex[i], !bLock);
                }
            }
        }
        return true;
    }

    public static bool onCommand_MinimapFightInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MinimapFightInfo=================== : {0}", nParam));
        }
        else
        {
            cmd_war_entity_fight_minimap_info data = IntPtrHelper.toData<cmd_war_entity_fight_minimap_info>(ptrParam);

            LogicDataCenter.warMinimapDataManager.MinimapEntityAttachEffect((int)data.SrcUID, data.nType);
        }

        return true;
    }

    public static bool onCommand_Change_Operation(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Change_Operation=================== : {0}", nParam));
            return false;
        }

        cmd_creature_changeOperation data = IntPtrHelper.toData<cmd_creature_changeOperation>(ptrParam);

        SoldierController sc = entity.GetComponent<SoldierController>();

        if (sc == null)
        {
            return true;
        }

        //if (sc.solidermode == 2)//控制模式2的时候实际初始默认1，进战斗才2（策划特殊需求）
        //2016.12.2摄像机功能重构，solidermode没用过，暂时注释掉
        //{
        if (data.nType != 0)
        {
            sc.controlmode = 2;
        }
        else
        {
            sc.controlmode = 1;
        }
        //}
        //else
        //{
        //    sc.controlmode = sc.solidermode;
        //}

        return true;
    }

    public static bool onCommand_MagicType(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MagicType=================== : {0}", nParam));
            return false;
        }

        return true;
    }

    public static bool onCommand_SetSpellSlot(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetSpellSlot=================== : {0}", nParam));
            return false;
        }

        // OB 处理
        if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
        {
            LogicDataCenter.warOBUIDataManager.SetSpellSlot(ev, ptrParam);
        }

        UIMsgCmdData uiSkillSlotData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, uiSkillSlotData);


        if (ev.IsHero && EntityFactory.MainHero != null)
        {
            cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(ptrParam);

            // 假如正好是正在扫射的技能被替换掉了 需要停止扫射逻辑
            if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                if (sc != null) sc.onSpellSlotChange(data.nSlotIndex, data.slotview.nId);
            }
        }

        return true;
    }

    public static bool onCommand_SpellPoint(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SpellPoint=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData uiSkillUpdataData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLUPDATE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLUPDATE, uiSkillUpdataData);
        return true;
    }

    public static bool onCommand_HideBody(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_HideBody=================== : {0}", nParam));
            return false;
        }

        BaseStateMachine sm = entity.GetComponent<BaseStateMachine>();
        if (sm != null)
        {
            cmd_creature_hide_body data = IntPtrHelper.toData<cmd_creature_hide_body>(ptrParam);
            sm.SetVisible(data.nHideBody != 1);
        }
        return true;
    }

    public static bool onCommand_SetWarPersonGoodsInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_WarBuyGoods=================== : {0}", nParam));
            return false;
        }

        //UIMsgCmdData uiGoodData = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMOM_WARGOODSINFO, nParam, strParam, ptrParam, nPtrLen);
        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_WARGOODSINFO, uiGoodData);
        LogicDataCenter.warMobaGoodsDataManager.UpdateMainPlayerGoods(ptrParam, nPtrLen);

        return true;
    }


    public static bool onCommand_MapShortCut(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MapShortCut=================== : {0}", nParam));
            return false;
        }

        SkillCtrl sc = entity.GetComponent<SkillCtrl>();
        if (sc != null)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count cmdCount = helper.get<cmd_count>();
            bool isAdd = false;
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_creature_slot_maped_shortcut data = helper.get<cmd_creature_slot_maped_shortcut>();

                sc.setSlot(data.nSlotType, data.nSlotIndex, data.nAirCons, data.nAdvanceUseType, data.szKeyName, data.isAdd);
                isAdd = data.isAdd;
                ASpeedGame.Data.SlotKeyCodeConfig.SlotKeyCodeConfig.Instance.MatchSlotData(data.nSlotType, data.nSlotIndex);
            }
            if (isAdd)
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_REFRESH, null);
        }

        return true;
    }

    // 更新右上角战场玩家以及阵营战斗信息
    public static bool onCommand_Set_Alpha(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        ev.SkinControl.setAlpha((float)nParam / 100);
        return true;
    }

    public static bool onCommand_SetCanBuyOrSell(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL, new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANBUYORSELL, nParam, strParam, ptrParam, nPtrLen));
        return true;
    }
    public static bool onCommand_SetCanCancelOpration(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED, new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLECANREVOKED, nParam, strParam, ptrParam, nPtrLen));
        return true;
    }

    public static bool onCommand_MainHeroVisionEntity_LevelChange(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (ev == null)
        {
            return false;
        }

        ev.Property.SetLightEffect();
        return true;
    }

    public static bool onCommand_ShowKill_Icon(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiShowKillNumData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWKILL, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARCOMBOATTACK_SHOWKILL, uiShowKillNumData);

        return true;
    }

    public static bool onCommand_PlaySound(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        cmd_creature_play_sound data = IntPtrHelper.toData<cmd_creature_play_sound>(ptrParam);
        if (data.nStop == 1)
        {
            SoundManager.DeleteSound(data.nSoundID);
        }
        else
        {
            SoundManager.CreateSound(entity, data.nSoundID);
            SoundPlayHandler.PlaySkillRandomSound(data.nSoundID, id, entity);
        }

        return true;
    }

    public static bool onCommand_OneRequestPersonInfoComplete(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.recordTableDataManager.setIsOneRequest();
        return true;
    }


    public static bool onCommand_ChargeCountChange(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {

        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChargeCountChange=================== : {0}", nParam));
            return false;
        }


        if (UBBProperty.GetInstance() != null)
        {
            cmd_buff_charge_count_change data = IntPtrHelper.toData<cmd_buff_charge_count_change>(ptrParam);

            UBBProperty.GetInstance().SetChargeCount(data);
        }
        return true;

    }
    public static bool onCommand_MactchRoomSkillDesc(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MactchRoomSkillDesc=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiSkillDescInfo = new UIMsgCmdData((int)WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO, uiSkillDescInfo);
        }
        return true;
    }

    public static bool onCommand_ChangeGodEyeMode(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChangeGodEyeMode=================== : {0}", nParam));
        }
        else
        {
            cmd_creature_change_god_eye_mode data = IntPtrHelper.toData<cmd_creature_change_god_eye_mode>(ptrParam);

            if (data.nEnter == 0 && ev.IsHero)
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.MapGodControl, false);
            else if (data.nEnter == 1)
            {//进入先不处理
                ;
            }

        }
        return true;
    }


    public static bool onCommand_PlayerData(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_BaseFrame=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.recordTableDataManager.recvWarEndPlayerData(ptrParam, nPtrLen);
        }
        return true;
    }

    public static bool onCommand_SendUIDAndPBUID(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SendUIDAndPBUID=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiTalentData = new UIMsgCmdData((int)WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMUID, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TEAMRIGHT_INITMYTEAMUID, uiTalentData);
        }
        return true;
    }

    public static bool onCommand_MpEffectSpell(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_MpEffectSpell=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiMpEffectData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, uiMpEffectData);
        }

        return true;
    }

    public static bool onCommand_ShowPromptUI(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam || ev == null)
        {
            Trace.Log(string.Format("==================onCommand_ShowPromptUI=================== : {0}", nParam));
        }
        else
        {
            if (ev.ID == EntityFactory.MainHeroID)
            {
                UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, nParam, strParam, ptrParam, nPtrLen);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, uiData);
            }
            else
            {
                Trace.LogFormat("WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER come.");
                UIMsgCmdData uiData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, nParam, strParam, ptrParam, nPtrLen);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, uiData);
            }
        }

        return true;
    }

    public static bool onCommand_Send_AllTalentPageInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Send_AllTalentPageInfo=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiTalentData = new UIMsgCmdData((int)WndMsgID.WND_MSG_TALENTPAGE_INITTALENTDATA, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_INITTALENTDATA, uiTalentData);
        }

        return true;
    }


    public static bool onCommand_Sticky_Camera(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Sticky_Camera=================== : {0}", nParam));
        }
        else
        {
            cmd_creature_Sticky_Camerra data = IntPtrHelper.toData<cmd_creature_Sticky_Camerra>(ptrParam);
            EntityView attachev = EntityFactory.getEntityViewByID((int)data.uid);
            if (attachev != null)
            {
                SoldierCamera.MainInstance<SoldierCamera>().soldier = attachev;
            }
        }

        return true;
    }

    public static bool onCommand_Send_AllRunePageInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Send_AllRunePageInfo=================== : {0}", nParam));
        }
        else
        {
            UIMsgCmdData uiRuneData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA, nParam, strParam, ptrParam, nPtrLen);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA, uiRuneData);
        }

        return true;
    }

    public static bool onCommand_UpdateWarStatisticTotal(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warStatisticDataManager.RecvTotalStatistic(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_UpdateWarStatisticWar(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warStatisticDataManager.RecvWarStatistic(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_Return_UpgradeStartProAdnExpend(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }
        LogicDataCenter.playerSystemDataManager.UpgradeStartProAndExpend(ptrParam, nPtrLen);

        UIMsgCmdData uiHeroInfo = new UIMsgCmdData((int)WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, uiHeroInfo);
        return true;
    }

    public static bool onCommand_Return_UpgradeSucess(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }
        UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, uiResult);
        return true;
    }

    public static bool onCommand_Return_PlayerAwark(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.recordTableDataManager.recvWarEndAwake(ptrParam, nPtrLen);
        LogicDataCenter.talentPageDataManager.UpdatePersentLevel();

        return true;
    }

    public static bool onCommand_Return_StaticHeroUpgrade(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateWarStatisticTotal=================== : {0}", nParam));
            return false;
        }

        cmd_entity_statichero_upgrade data = IntPtrHelper.toData<cmd_entity_statichero_upgrade>(ptrParam);
        LogicDataCenter.recordTableDataManager.setStaticHeroUpgradeInfo(data, true);

        return true;
    }

    public static bool onCommand_Hint_Player_Upgrade_Star(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (LogicDataCenter.playerSystemDataManager.nRequestPDBID == 0)
        {
            LogicDataCenter.playerSystemDataManager.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
        }

        LogicDataCenter.playerSystemDataManager.checkAllHeroIsHaveCanUpStar(nParam);

        // 金钱属性更新,显示层改变
        UIMsgCmdData cmdData = UIMsgCmdDataHelper.Instance.create(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, nParam, "");
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, cmdData);

        return true;
    }

    public static bool onCommand_Firstwin_Notice(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Firstwin_Notice=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warStatisticDataManager.UpdateFirstwinTime(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_ShowTitleNode(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowTitleNode=================== : {0}", nParam));
            return false;
        }

        //IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        //cmd_count cmdCount = helper.get<cmd_count>();
        //for (int i = 0; i < cmdCount.nCount; ++i)
        //{
        //    cmd_entity_show_title_node node = helper.get<cmd_entity_show_title_node>();
        //}

        if (ev == null)
        {
            return false;
        }
        if (GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
        {
            UTopName ut = UTopNameManager.Instance.FindTopName(id);
            if (null == ut)
            {
                UTopNameManager.Instance.CreateTopName(ev, ptrParam, nPtrLen);
            }
            else
            {
                UTopNameManager.Instance.UpdateTopName(ev, ptrParam, nPtrLen);
            }
            if (UTopNameManager.PkTargetID == id)
            {
                UTopNameManager.Instance.SetPKMode(id, true);
                UTopNameManager.Instance.SetPKMode(EntityFactory.MainHeroID, true);
            }
        }

        return true;
    }

    public static bool onCommand_SetHeroTalentInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetHeroTalentInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.heroTalentDataManager.InitHeroTalentInfo(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_UpdateHeroTalentInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateHeroTalentInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.heroTalentDataManager.UpdateHeroTalentData(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_SetObjRangeTip(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetObjRangeTip=================== : {0}", nParam));
            return false;
        }
        cmd_entity_SetObjRangeTip data = IntPtrHelper.toData<cmd_entity_SetObjRangeTip>(ptrParam);

        MinimapSetObjRangeTip tip = new MinimapSetObjRangeTip();
        tip.bIsAdd = data.bAdd == 0 ? false : true;
        tip.nObjID = data.nObjID;
        tip.nRange = data.nRange;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETOBJECTRANGETIP, tip);

        return true;
    }

    public static bool onCommand_UpdateRuneStore(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateRuneStore=================== : {0}", nParam));
            return false;
        }

        cmd_Rune_InStore_Item data = IntPtrHelper.toData<cmd_Rune_InStore_Item>(ptrParam);
        LogicDataCenter.runePageDataManager.UpdateRuneStoreData(data);
        return true;
    }

    public static bool onCommand_Send_OtherAllRunePageInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_Send_OtherAllRunePageInfo=================== : {0}", nParam));
        }
        else
        {
            LogicDataCenter.playerSystemDataManager.UpdateAllRune(ptrParam, nPtrLen);
        }

        return true;
    }

    public static bool onCommand_UpdateChestInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateChestInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.prizeChestDataManager.OnUpdateChestInfo(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateEmojiInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateEmojiInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.emojiDataManager.OnUpdateEmojiInfo(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_OpenChestResult(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_OpenChestResult=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.prizeChestDataManager.OnOpenChestResult(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_ChestRandCardResult(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ChestRandCardResult=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.prizeChestDataManager.OnReturnRandCardInfo(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_RequestPK(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RequestPK=================== : {0}", nParam));
            return false;
        }

        cmd_creature_pk_request_pk cmdInfo = IntPtrHelper.toData<cmd_creature_pk_request_pk>(ptrParam);

        int playerLv = 0;
        if (EntityFactory.MainHeroView != null)
            playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
        SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_PKShowPrompt);
        if (cmdInfo.bShowPKPrompt > 0 && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
        {
            SideButtonListAddData msg = new SideButtonListAddData();
            msg.nItemID = (int)ESideButtonItemType.ESBT_PKShowPrompt;
            msg.nButtonIconID = config.nIconID;
            msg.strButtonText = config.strName;
            msg.strTooltip = config.strHint;
            msg.sCustomData = null;
            msg.callBack = (customData) =>
            {
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_PK_HIT_PROMPT, 0, "", IntPtr.Zero, 0);
            };
            LogicDataCenter.sideButtonDataManager.AddItem(msg);
        }
        else
        {
            SideButtonListRemoveData msg = new SideButtonListRemoveData();
            msg.nItemID = (int)ESideButtonItemType.ESBT_PKShowPrompt;
            LogicDataCenter.sideButtonDataManager.RemoveItem(msg);
        }

        return true;
    }

    public static bool onCommand_ShowPKInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowPKInfo=================== : {0}", nParam));
            return false;
        }

        //cmd_creature_pk_show_pk_info pkInfo = IntPtrHelper.toData<cmd_creature_pk_show_pk_info>(ptrParam);
        //PKInfoItem info = new PKInfoItem(pkInfo.uidOperator, pkInfo.szInfo);
        //info.uidOperator = pkInfo.uidOperator;
        //info.szInfo = pkInfo.szInfo;
        //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PKINO_ADD, info);

        LogicDataCenter.pkDataManager.onPkDidaShow(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_SetHeroSkinInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetHeroSkinInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.heroSkinDataManager.InitHeroSkinInfo(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateHeroSkinInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateHeroSkinInfo=================== : {0}", nParam));
            return false;
        }

        cmd_Hero_Skin_Info data = IntPtrHelper.toData<cmd_Hero_Skin_Info>(ptrParam);
        LogicDataCenter.heroSkinDataManager.UpdateHeroSkinInfo(data);
        return true;
    }

    public static bool onCommand_UpdatePointShopBuyNumInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdatePointShopBuyNumInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.pointShopDataManager.UpdateBuyNumInfo(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_ReflushPointShopBuyNumInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.pointShopDataManager.ReflushBuyNumInfo();

        return true;
    }

    public static bool onCommand_PointShopBuyResult(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_PointShopBuyResult=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.pointShopDataManager.OnBuyResult(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_PointShopUpdateCardlist(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_PointShopUpdateCardlist=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.pointShopDataManager.UpdateCardlist(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_ActorInfoChange(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ActorInfoChange=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.ActorInfoChangeUpdate(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_XPRandomSelfInCool(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL, null);
        return true;
    }

    public static bool onCommand_HeroTalentActiveInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        //UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_ID_HEROTALENTVIEW_INITACTIVEHEROTALENTINRECONNECT, nParam, strParam, ptrParam, nPtrLen);
        //UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_INITACTIVEHEROTALENTINRECONNECT, uiResult);
        return true;
    }

    public static bool onCommand_SpecialEntitySearchStateChange(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateHeroSkinInfo=================== : {0}", nParam));
            return false;
        }

        cmd_Special_Entity_Search_State_Change data = IntPtrHelper.toData<cmd_Special_Entity_Search_State_Change>(ptrParam);
        EntityTimeSearchManager.instance.ChangeEntityTimeSearchState((int)data.uidOperator, data.bSearch);
        return true;
    }

    public static bool onCommand_EntityPropertyReturnDragonball(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_EntityPropertyReturnDragonball=================== : {0}", nParam));
            return false;
        }

        PrefamceInfo.Instance.UpdateEntityProperty(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_HeroUpstarUnlockPrize(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_HeroUpstarUnlockPrize=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.RecvHeroUpStarUnlockPrize(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_ShowChooseTarget(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowChooseTarget=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY, uiResult);

        return true;
    }

    public static bool onCommand_BrodcastSpecialContribution(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowChooseTarget=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION, uiResult);

        return true;
    }

    public static bool onCommand_RenameResult(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, uiResult);

        return true;
    }

    // 增益卡更新
    public static bool onCommand_BenefitCard(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log("=====================onCommand_BenefitCard====failed===============================:{0}" + nParam);
            return false;
        }

        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

        cmd_benefit_card_list_head cardListHead = helper.get<cmd_benefit_card_list_head>();

        List<BenefitCardInfo> cardList = new List<BenefitCardInfo>();
        for (int i = 0; i < cardListHead.nCount; ++i)
        {
            cmd_benefit_card msg = helper.get<cmd_benefit_card>();

            BenefitCardInfo cardInfo = new BenefitCardInfo(msg.nBenefitCardType, msg.nBenefitCardSubType, msg.nBenefitValue, msg.nMultiple);

            cardList.Add(cardInfo);
        }

        LogicDataCenter.playerSystemDataManager.GetBenefitCardManager().addBenefitCard(cardListHead.nListType, ref cardList);

        return true;
    }

    public static bool onCommand_UpdateCrownAllGemstone(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.crownPageDataManager.UpdateCrownAllGemStoneData(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateAllCrownPage(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.crownPageDataManager.UpdateCrownAllPageData(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateCrownGemstoneItem(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowChooseTarget=================== : {0}", nParam));
            return false;
        }

        cmd_Crown_Gemstone_Item_Info data = IntPtrHelper.toData<cmd_Crown_Gemstone_Item_Info>(ptrParam);
        LogicDataCenter.crownPageDataManager.UpdateGemStoneItemData(data);

        return true;
    }

    public static bool onCommand_CrownGemstoneAppraisalRet(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_CrownGemstoneAppraisalRet=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData uiResult = new UIMsgCmdData((int)WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET, uiResult);

        return true;
    }

    public static bool onCommand_UpdateCrownProduct(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ShowChooseTarget=================== : {0}", nParam));
            return false;
        }

        cmd_Crown_Gemstone_Product_Info data = IntPtrHelper.toData<cmd_Crown_Gemstone_Product_Info>(ptrParam);
        LogicDataCenter.crownPageDataManager.UpdateGemStoneProductData(data);

        return true;
    }

    public static bool onCommand_UpdateOtherAllCrownPage(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.crownPageDataManager.UpdateOtherCrownAllPageData(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_FilterSupportPlayer(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_FilterSupportPlayer=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.labelDataManager.FilterSupportPlayer(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_AddLabel(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_AddLabel=================== : {0}", nParam));
            return false;
        }

        cmd_war_add_label data = IntPtrHelper.toData<cmd_war_add_label>(ptrParam);
        LogicDataCenter.labelDataManager.recvAddLabel(data);

        return true;
    }

    // 标签列表
    public static bool onCommand_UpdateLabelList(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateLabelList=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.recvLabelList(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_UpdateHeroXPSkill(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.heroXPSkillDataManager.UpdateHeroXPSkillInfo(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_UpdateHeroXPFragment(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.heroXPSkillDataManager.UpdateHeroXPFragmentInfo(ptrParam, nPtrLen);
        return true;
    }

    public static bool onCommand_NPCOpenShop(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.npcDataManager.recvNPCOpenWarShop();
        return true;
    }

    public static bool onCommand_NPCResponse(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.npcDataManager.recvNPCResponse(strParam);
        return true;
    }

    public static bool onCommand_NPCCloseDialogBox(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.npcDataManager.closeNPCDialogBox();
        return true;
    }

    public static bool onCommand_UpdatePlayerGameState(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        LogicDataCenter.playerSystemDataManager.updatePlayerGameState(ptrParam, nPtrLen);

        return true;
    }

    // 数据通知回调
    public static bool onCommand_notifydata(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_entity_notify_data cmd_data = helper.get<cmd_entity_notify_data>();

        if (cmd_data.nType == (int)NOTIFY_DATA_TYPE.NOTIFY_TYPE_STEP)
        {
            GUIDE.GuideManager.instance.SetBBValue(GUIDE.EGuideBBKey.LogicStepID, cmd_data.nValue);
        }
        else if (cmd_data.nType == (int)NOTIFY_DATA_TYPE.NOTIFY_TYPE_TIP)
        {
            GUIDE.GuideManager.instance.SetBBValue(GUIDE.EGuideBBKey.AutoGuideMessageID, cmd_data.nValue);
        }

        return true;
    }

    public static bool onCommand_EquipSchemeList(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_EquipSchemeList=================== : {0}", nParam));
            return false;
        }

        DataCenter.LogicDataCenter.equipSchemeDataMagaer.recvEquipSchemeList(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_ActiveTalentList(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ActiveTalentList=================== : {0}", nParam));
            return false;
        }

        DataCenter.LogicDataCenter.heroTalentDataManager.recvActiveTalentList(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_ReClacSpellTargetInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ReClacSpellTargetInfo=================== : {0}", nParam));
            return false;
        }

        cmd_spell_attribute data = IntPtrHelper.toData<cmd_spell_attribute>(ptrParam);

        SkillCtrl sc = ev.SkillCtrl;
        if (sc != null)
        {
            sc.onReCaclacTargetInfo(data);
        }

        return true;
    }

    public static bool onCommand_BottingNotify(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (nParam == (int)EWBottingNotifyType.EWBNT_Normal)
        {
            // 弹出挂机提示通知
            UIDialogData toolTipData = new UIDialogData(
                mutex: true,
                eDialogType: EDialogType.Botting,
                title: ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarBottingTitle"),
                message: ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarBottingDes"),
                buttons: new UIWidgets.DialogActions() {
                    {
                            ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {
                            EntityEventHelper.Instance.SendCommand(ev.ID, EntityLogicDef.ENTITY_CMD_BOTTING_CANCEL);
                            return true;
                        }
                    }
                }
                );

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
        }
        else if (nParam == (int)EWBottingNotifyType.EWBNT_Punish)
        {
            // 弹出挂机已经被惩罚提示
            UIDialogData toolTipData = new UIDialogData(
                mutex: true,
                eDialogType: EDialogType.Botting,
                title: ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarBottingTitle"),
                message: ULocalizationService.Instance.Get("UIView", "PunishTimeInfo", "WarBottingPunishDes"),
                buttons: new UIWidgets.DialogActions() {
                    {
                            ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"),() => {
                            return true;
                        }
                    }
                }
                );

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
        }

        return true;
    }

    public static bool onCommand_WarInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_onCommand_WarInfo=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warInfoDataManager.recvWarInfo(ptrParam, nParam);

        return true;
    }

    public static bool onCommand_SkillAutoMove(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SkillAutoMove=================== : {0}", nParam));
            return false;
        }
        cmd_auto_move data = IntPtrHelper.toData<cmd_auto_move>(ptrParam);
        if (ev.IsHero)
        {
            SoldierController soldierCtrl = EntityFactory.MainHero.GetComponent<SoldierController>();
            if (soldierCtrl != null)
            {
                if (data.nOpenFlag == 0)
                {
                    soldierCtrl.skillautoForward = false;
                    SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FPSMode, false);
                    ((CreatureStateMachine)ev.StateMachine).EnableAutoForward(0, false);
                }
                else
                {
                    soldierCtrl.skillautoForward = true;
                    SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.FPSMode, true);
                    ((CreatureStateMachine)ev.StateMachine).EnableAutoForward(data.fMaxSpeed, true);
                }
            }
        }
        return true;
    }

    public static bool onCommand_WarEndLegendCupInfo(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SkillAutoMove=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.warInfoDataManager.recvWarEndLegendCupInfo(ptrParam, nParam);

        return true;
    }

    public static bool onCommand_SetAllLifeHero(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetAllLifeHero=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.lifeHeroDataManager.OnRecvAllLifeHero(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateLifeHeroPrizeTaskData(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateLifeHeroPrizeTaskData=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.lifeHeroDataManager.OnUpdatePrizeTaskData(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_UpdateNoviceGuideData(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateNoviceGuideData=================== : {0}", nParam));
            return false;
        }

        GuideManager.Instance.OnUpdateNoviceGuideType(ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_UPDATENOVICEGUIDEDATA, null);

        return true;
    }

    public static bool onCommand_UpdateHistoryRecord(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_UpdateHistoryRecord=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.lifeHeroDataManager.RecvHistoryRecord(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_showDebuffProgressBar(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_showDebuffProgressBar=================== : {0}", nParam));
            return false;
        }

        cmd_show_debuff_progress_bar data = IntPtrHelper.toData<cmd_show_debuff_progress_bar>(ptrParam);
        if (null != ev && null != USpeedUI.Blood.UBloodManager.Instance)
        {
            USpeedUI.Blood.UBloodManager.Instance.ShowDebuffProgressBar(id, data);
        }

        return true;
    }

    public static bool onCommand_OnUpdateLifeHero(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_OnUpdateLifeHero=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.lifeHeroDataManager.OnUpdateLifeHero(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_ActorUnlock(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ActorUnlock=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.playerSystemDataManager.ShowActorUnlockPrize(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_RankUpOrDown(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_ActorUnlock=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.rankDataManager.RankUpOrDown(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_NPCBuyGoodsSuccess(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_NPCBuyGoodsSuccess=================== : {0}", nParam));
            return false;
        }

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_NPC_BUY_GOODS_SUCCESS, new UIMsgCmdData((int)WndMsgID.WND_MSG_NPC_BUY_GOODS_SUCCESS, nParam, strParam, ptrParam, nPtrLen));

        return true;
    }

    // 接收到服务器下发NavGroup数据
    public static bool onCommand_processNavGroup(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        int nNavCount = nParam;

        // TODO
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        for (int i = 0; i < nNavCount; ++i)
        {
            SGameNavItem item = helper.get<SGameNavItem>();
            if (item.nStrLen > 0)
            {
                string strNavName = helper.getUTF8String(item.nStrLen);
                Trace.Log("nav id=" + item.nID.ToString() + ", text=" + strNavName);
                UPromotionNavItem navData = new UPromotionNavItem(item.nID, strNavName);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_NAV, navData);
            }
        }
        return true;
    }

    // 接收WebUrl
    public static bool onCommand_processWebUrl(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        int nWebUrlID = nParam;
        string strUrl = strParam;
        Trace.Log("web id=" + nWebUrlID.ToString() + ", url=" + strUrl);
        // TODO
        UWebUrlData webUrlData = new UWebUrlData(nWebUrlID, strUrl);
        if(nWebUrlID < 1000)
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_REDIRECT_WEB, webUrlData);
        }
        else
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_PROCESS_WEBURL, webUrlData);
        }
        return true;
    }
    

    public static bool onCommand_RankSeasonDetail(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RankSeasonDetail=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_DETAIL_RECEIVE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_DETAIL_RECEIVE, cmdData);

        return true;
    }

    public static bool onCommand_RankSeasonRecord(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_RankSeasonRecord=================== : {0}", nParam));
            return false;
        }

        UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_RECORD_RECEIVE, nParam, strParam, ptrParam, nPtrLen);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_RECORD_RECEIVE, cmdData);

        return true;
    }

    public static bool onCommand_SetRecommendRecord(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_SetRecommendRecord=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.gamePromotionDataManager.SetRecommRecord(ptrParam, nPtrLen);

        return true;
    }

    public static bool onCommand_GainRecommPrizeSuccess(EntityView ev, GameObject entity, ENTITY_ID id, Int32 cmdID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
    {
        if (null == ptrParam || IntPtr.Zero == ptrParam)
        {
            Trace.Log(string.Format("==================onCommand_GainRecommPrizeSuccess=================== : {0}", nParam));
            return false;
        }

        LogicDataCenter.gamePromotionDataManager.SetObtainPrizeSuccess(nParam, strParam, ptrParam, nPtrLen);

        return true;
    }
}
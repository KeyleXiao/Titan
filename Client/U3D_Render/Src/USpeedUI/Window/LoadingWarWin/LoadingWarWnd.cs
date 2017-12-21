/*******************************************************************
** 文件名:	UMediatorLoadingWar.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	载入战场等候界面
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using ASpeedGame.Data.HeroXPskillConfig;
using Match_ManagerDef;

namespace USpeedUI.LoadingWarPart
{
    public class LoadingWarWnd : UIBaseWnd<LoadingWarWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_LOADINGWARVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "LoadingWarView/LoadingWarView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02; ;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPLAYERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPROGREESERATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie, this);


            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPLAYERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_LOADINGWARVIEW_LOADPROGREESERATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie, this);

            return;
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_ID_LOADINGWARVIEW_LOADPLAYERINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiLoadPlayerInfo = (UIMsgCmdData)msgData;
                        InitLoadingPlayerFrameInfo(uiLoadPlayerInfo.ptrParam, uiLoadPlayerInfo.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_LOADINGWARVIEW_LOADPROGREESERATE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiLoadRateInfo = (UIMsgCmdData)msgData;
                        cmd_load_scene_rate data = IntPtrHelper.toData<cmd_load_scene_rate>(uiLoadRateInfo.ptrParam);
                        m_wndView.SetLoadingRate(data);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        // 战场初始化数据
        public void InitLoadingPlayerFrameInfo(IntPtr ptrParam, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nLen);

            cmd_Camp_Count cmdCount = helper.get<cmd_Camp_Count>();

            int nMatchType = cmdCount.nMatchType;
            int nMapId = cmdCount.nMapID;
            int nSelfCamp = 0;

            // 自己皮肤列表
            List<int> self_hero_skin_list = new List<int>();
            // 友方皮肤列表
            List<int> friend_hero_skin_list = new List<int>();
            // 敌方皮肤列表
            List<int> enemy_hero_skin_list = new List<int>();
            // XP技能列表
            List<int> hero_xp_list = new List<int>();

            // 有效玩家数据列表，吃鸡地图只加载右方信息
            List<cmd_match_load_scene_playerinfo> player_info_list = new List<cmd_match_load_scene_playerinfo>();

            foreach (int nCampCount in cmdCount.vCampCount)
            {
                for (int i = 0; i < nCampCount; ++i)
                {
                    cmd_match_load_scene_playerinfo data = helper.get<cmd_match_load_scene_playerinfo>();
                    // 有效的英雄ID,有效皮肤ID
                    if (data.nHeroID <= 0 || data.nSkinID <= 0)
                        continue;

                    // 自己皮肤
                    if (data.nSelfInfo == 1)
                    {
                        self_hero_skin_list.Add(data.nSkinID);
                        nSelfCamp = data.nSelfCamp;
                    }
                    // 友方
                    else if (data.nSelfCamp == data.nCamp)
                        friend_hero_skin_list.Add(data.nSkinID);
                    // 敌方
                    else
                        enemy_hero_skin_list.Add(data.nSkinID);

                    // 有效XP
                    if (data.nXPSkillID > 0)
                        hero_xp_list.Add(data.nXPSkillID);

                    // 吃鸡模式只加右方，其他模式全加
                    if (nMatchType == (int)EMMatchType.MatchType_MultiCampMatch)
                    {
                        if (data.nSelfCamp == data.nCamp)
                        {
                            player_info_list.Add(data);
                        }
                    }
                    else
                    {
                        player_info_list.Add(data);
                    }
                }
            }

            // 设置显示数据
            m_wndView.SetData(nMatchType, nMapId, nSelfCamp, cmdCount.vCampCount, player_info_list);

            List<SkinPreLoadInfo> skins = new List<SkinPreLoadInfo>();
            List<int> self_skins = new List<int>();
            List<int> friend_skins = new List<int>();
            List<int> enemy_skins = new List<int>();

            // 预加载皮肤
            foreach (int nSkinID in self_hero_skin_list)
            {
                SkinPreLoadInfo info = new SkinPreLoadInfo();
                info.skinID = nSkinID;
                info.nMask |= (int)CachePriority.PRIORITY_VERY_HIGH;
                List<SkinPreLoadInfo> relateSkinList = SkinHelper.PreLoadPlayerSkin(info);
                skins.AddRange(relateSkinList);

                foreach (SkinPreLoadInfo peri in relateSkinList)
                {
                    self_skins.Add(peri.skinID);
                }

            }
            foreach (int nSkinID in friend_hero_skin_list)
            {
                SkinPreLoadInfo info = new SkinPreLoadInfo();
                info.skinID = nSkinID;
                info.nMask |= (int)CachePriority.PRIORITY_HIGH;
                List<SkinPreLoadInfo> relateSkinList = SkinHelper.PreLoadPlayerSkin(info);
                skins.AddRange(relateSkinList);

                foreach (SkinPreLoadInfo peri in relateSkinList)
                {
                    friend_skins.Add(peri.skinID);
                }
            }
            foreach (int nSkinID in enemy_hero_skin_list)
            {
                SkinPreLoadInfo info = new SkinPreLoadInfo();
                info.skinID = nSkinID;
                info.nMask |= (int)CachePriority.PRIORITY_MEDIUM;
                List<SkinPreLoadInfo> relateSkinList = SkinHelper.PreLoadPlayerSkin(info);
                skins.AddRange(relateSkinList);

                foreach (SkinPreLoadInfo peri in relateSkinList)
                {
                    enemy_skins.Add(peri.skinID);
                }
            }

            SkinHelper.PreLoadMonsterSkin(nMapId);

            LightingEffectFactory.Instance.AcitvieLoadingSceneQueue();

            // 预加载皮肤对应的光效
            EffectHelper.LoadPreLights(nMapId, ASpeedGame.Data.Scheme.LightType.ActorSkin, self_skins, CachePriority.PRIORITY_VERY_HIGH);
            EffectHelper.LoadPreLights(nMapId, ASpeedGame.Data.Scheme.LightType.ActorSkin, friend_skins, CachePriority.PRIORITY_HIGH);
            EffectHelper.LoadPreLights(nMapId, ASpeedGame.Data.Scheme.LightType.ActorSkin, enemy_skins, CachePriority.PRIORITY_MEDIUM);

            // 预加载XP技能对应的光效
            EffectHelper.LoadPreLights(nMapId, ASpeedGame.Data.Scheme.LightType.XP, hero_xp_list, CachePriority.PRIORITY_LOW);

            LightingEffectFactory.Instance.UnAcitvieLoadingSceneQueue();
        }

    }
}

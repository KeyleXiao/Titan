/*******************************************************************
** 文件名:	HeroTalentWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017-11-25
** 版  本:	1.0
** 描  述:	主城英雄天赋
		
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
using ASpeedGame.Data.HeroTalentConfig;
using U3D_Render.Common;
namespace USpeedUI.WarHeroTalent
{
	public class WarHeroTalentWnd : UIPopupWnd<WarHeroTalentView>
	{
		// 获取ID
		public override WndID GetID()
		{
			return WndID.WND_ID_WARHEROTALENTVIEW;
		}

		// 获取资源路径
		public override String GetPrefabPath()
		{
			return "WarHeroTalent/WarHeroTalentView";
		}

		// 获取所在层次
		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP01; ;
		}

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP | (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_All;
        }

        // 初始化
        public override bool Init()
		{
			base.Init();

			// 订阅聊天消息更新消息
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_HEROTALENTVISIBLE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_MAINHEROFIGHTSTATE, this);
			//UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_INITACTIVEHEROTALENTINRECONNECT, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_INITWARTALENT, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE, this);

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
			return true;
		}

		// 销毁
		public override void Destroy()
		{
			base.Destroy();
			// 退订聊天消息更新消息

			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_HEROTALENTVISIBLE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_MAINHEROFIGHTSTATE, this);
			//UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_INITACTIVEHEROTALENTINRECONNECT, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_INITWARTALENT, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE, this);

			EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
		}

		// 接受消息
		public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
		{
            saveCacheUIMsg(msgID, msgData);

            switch (msgID)
			{
				case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
						if (m_wndView== null)
                        {
                            LoadView();
                            SetVisible(true);
                        }

                        // 运行缓存的消息
                        runCacheUIMsg();
                    }
                    break;
                case WndMsgID.WND_ID_HEROTALENTVIEW_HEROTALENTVISIBLE:
					{
						if (m_wndView == null)
						{
							return;
						}

						HeroTalentVisibleData uiTalent = (HeroTalentVisibleData)msgData;
                        if (uiTalent.bVisible)
                        {
                            // 显示
                            SetHeroTalentViewOpen();
                        }
                        else
                        {
                            // 不显示
                            m_wndView.ShowMainTalent(false, false);
						}
					}
					break;
				case WndMsgID.WND_MSG_COMMON_WAREND:
				case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
					{
						this.UnloadView();
                        m_CacheUIMsgList.Clear();
					}
					break;
				case WndMsgID.WND_MSG_COMMON_MAINHEROFIGHTSTATE:
					{
                        // 战斗状态关闭天赋窗口
						if (m_bIsGuiding == false)
                        {
							CloseMainHeroTalentView();
                        }
					}
					break;
	
				case WndMsgID.WND_ID_HEROTALENTVIEW_INITWARTALENT:
					{
						if (m_wndView == null)
						{
							return;
						}

						if (EntityUtil.getMainHeroHeroID() <= 0)
						{
							return;
						}

                        Debug.LogWarning("WND_ID_HEROTALENTVIEW_INITWARTALENT");

                        // 初始化本英雄的天赋列表界面数据
						InitShowDataInfo(EntityUtil.getMainHeroHeroID());
					}
					break;
                case WndMsgID.WND_ID_HEROTALENTVIEW_ACTIVEDTALENTLISTUPDATE:
                    {
                        // 断线重连情况下，该消息比BATTLESTATE_ENTER消息先来
                        if (m_wndView == null)
                        {
                            LoadView();
                            SetVisible(true);
                        }

                        // 已激活的主动学习天赋列表更新
                        if (m_wndView != null)
                        {
                            // 每次成功激活一个天赋后执行
                            UIMsgCmdData uiTalent = (UIMsgCmdData)msgData;
						    int nTalentId = uiTalent.nParam;
                            if(nTalentId > 0)
                            {
                                m_wndView.onActivedTalent(nTalentId);
                            }
                            else
                            {
                                // 断线重连后收到天赋列表
                                cmd_war_active_talent_list talentList = new cmd_war_active_talent_list();
                                if(LogicDataCenter.heroTalentDataManager.getActiveTalentList((ulong)EntityUtil.getMainHeroPdbid(), out talentList))
                                {
                                    foreach(int item in talentList.nTalentList)
                                    {
                                        m_wndView.onActivedTalent(item);
                                    }
                                }
                            }

                            m_wndView.ShowMainTalent(false);
                        }
                    }
                    break;
                #region 新手引导
                case WndMsgID.WND_ID_HEROTALENTVIEW_ADDGUIDEWIDGET:
					{
						if (m_wndView != null && msgData != null)
						{
							UGuideWidgetMsgData uiTalent = (UGuideWidgetMsgData)msgData;
							m_bIsGuiding = true;
							m_wndView.AddGuideWidget(uiTalent);
						}
					}
					break;
				case WndMsgID.WND_ID_HEROTALENTVIEW_REMOVEGUIDEWIDGET:
					{
						if (m_wndView != null && msgData != null)
						{
							UGuideWidgetMsgData uiTalent = (UGuideWidgetMsgData)msgData;
							m_bIsGuiding = false;
							m_wndView.RemoveGuideWidget(uiTalent);
						}
					}
					break;
				#endregion
				default:
					break;
			}
		}

		protected bool m_bIsGuiding;

		private void CloseMainHeroTalentView()
		{
			if (m_wndView == null)
			{
				return;
			}

			m_wndView.ShowMainTalent(false);
		}

		//数值属性回调函数 用于战场
		public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
		{
			if (e.objev == null || m_wndView == null || !e.objev.IsHero)
			{
				return;
			}

			entity_view_update_Numproperty data = e.data;
			int nUpdateValue = data.nValue;
			switch (data.nPropID)
			{
				case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
					{
                        onUpdateLevel(nUpdateValue);

						UpdateTalentPropertyDesc();
					}
					break;
				case (int)ENTITY_PROPERTY.PROPERTY_VOCATION:
					{
                        if (nUpdateValue > 0)
                        {
                            if (LogicDataCenter.heroTalentDataManager.nCurHeroId == nUpdateValue)
                                return;

                            LogicDataCenter.heroTalentDataManager.nCurHeroId = nUpdateValue;

                            // 初始化新英雄的天赋数据
                            InitShowDataInfo(nUpdateValue);

                            // 清空已激活天赋
                            LogicDataCenter.heroTalentDataManager.clearSelfActivedTalentList();
                        }
					}
					break;
				case (int)ENTITY_PROPERTY.PROPERTY_PA:
				case (int)ENTITY_PROPERTY.PROPERTY_MA:
				case (int)ENTITY_PROPERTY.PROPERTY_PD:
				case (int)ENTITY_PROPERTY.PROPERTY_MD:
				case (int)ENTITY_PROPERTY.PROPERTY_SD:
				case (int)ENTITY_PROPERTY.PROPERTY_AF:
				case (int)ENTITY_PROPERTY.PROPERTY_CRC:
				case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE:
					{
						UpdateTalentPropertyDesc();
					}
					break;
				default:
					break;
			}
		}

		private void UpdateTalentPropertyDesc()
		{
			if (m_wndView == null)
			{
				return;
			}

			m_wndView.UpdateTalentItemShow();
		}

		private void onUpdateLevel(int nUpdateLevel)
		{
			if (LogicDataCenter.heroTalentDataManager.LevelArray == null)
			{
				return;
			}

			m_wndView.onUpdateLevel(nUpdateLevel);
		}

		private void SetHeroTalentViewOpen()
		{
            this.SetVisible(true);
            m_wndView.SetHeroTalentViewShowState();
        }

		private void InitShowDataInfo(int nHeroId)
		{
			if (LogicDataCenter.heroTalentDataManager.dicTalentInfoData == null || !LogicDataCenter.heroTalentDataManager.dicTalentInfoData.ContainsKey(nHeroId))
			{
				CloseMainHeroTalentView();
				return;
			}

			if (nHeroId <= 0)
				return;

            m_wndView.ClearHeroTalentData();
			HeroTalentConfigPage Instance = HeroTalentConfigPage.Instance;
			List<HeroTalentDataManager.HeroTalentInfo> listInfo = LogicDataCenter.heroTalentDataManager.dicTalentInfoData[nHeroId];
			Dictionary<int, int> dicTalentLevelRecord = new Dictionary<int, int>();
			foreach (HeroTalentDataManager.HeroTalentInfo info in listInfo)
			{
				SSchemeHeroTalentConfig config = Instance.GetHeroTalentConfig(nHeroId, info.nTalentId);
				if (config == null)
				{
					continue;
				}

				string sTalentDescInStatic = info.bActive ? config.sTalenActiveDescInStatic : config.sTalenDisableDescInStatic;
				int nLevelLimit = config.nLevelLimit;
				dicTalentLevelRecord[nLevelLimit] = 0;

				m_wndView.AddHeroTalentListData(info.nTalentId, info.bActive, config.nShowType, config.nTalentIconId, nLevelLimit, config.sTalentName, sTalentDescInStatic, config.sTalenDescInWar, config.nOnlyActive, config.nTalentTermId, config.nRecommandShow > 0);
			}

			CalculateWithLevelArray(dicTalentLevelRecord);

			m_wndView.SetHeroTalentListData();
		}

		private void CalculateWithLevelArray(Dictionary<int, int> dicTalentLevelRecord)
		{
			int nConfigLevelCount = dicTalentLevelRecord.Count;
			LogicDataCenter.heroTalentDataManager.LevelArray = new int[nConfigLevelCount];
			int nIndex = 0;
			foreach (int nLevel in dicTalentLevelRecord.Keys)
			{
				LogicDataCenter.heroTalentDataManager.LevelArray[nIndex] = nLevel;
				nIndex++;
			}

            // 对LevelArray排序
            Array.Sort(LogicDataCenter.heroTalentDataManager.LevelArray);
        }
    }
}

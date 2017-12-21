/*******************************************************************
** 文件名:	UViewWarRecordTableFighting.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	战场中战绩表控制显示界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using GameLogic;
using System.Runtime.InteropServices;
using DataCenter;
using Stage_ManagerDef;

namespace USpeedUI.WarRecordTableWarning
{
	public class UWarRecordTableFighting : UIPopupWnd<UViewWarRecordTableFighting>
	{
		private UViewWarRecordTableFighting view { get { return m_wndView as UViewWarRecordTableFighting; } }

		// 获取ID
		public override WndID GetID()
		{
			return WndID.WND_ID_WAR_RECORDTABLE_FIGHTING;
		}

		// 获取资源路径
		public override String GetPrefabPath()
		{
			return "DynamicState_Moba/WarRecordTable_Warning/WarRecordTable_Warning";
		}

		// 获取所在层次
		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_POPUP02;
		}

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        // 初始化
        public override bool Init()
		{
			base.Init();

			m_isKeyClose = true;

			InputManager.Available = true;

			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT, this);

			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND, this);

			// 订阅按键消息
			UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Up, this);
			UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Down, this);

			return true;
		}

		// 销毁
		public override void Destroy()
		{
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND, this);


			// 退订按键消息
			UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Up, this);
			UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Down, this);
			base.Destroy();
		}
		
		// 接收消息
		public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
		{
            saveCacheUIMsg(msgID, msgData);

            switch (msgID)
			{
				case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT:
					{
						if (view == null)
						{
							return;
						}
						view.initWarRecordTable();
					}
					break;

				case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME:
					{
						BossCdTime uiInfo = (BossCdTime)msgData;
						if (view == null)
						{
							return;
						}
						view.SetTeamBossReliveTime(uiInfo.nIndex, uiInfo.nTime);
					}
					break;
				case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
				case WndMsgID.WND_MSG_COMMON_WAREND:
					{
						if (view != null)
						{
							this.UnloadView();
						}

                        m_CacheUIMsgList.Clear();
                    }
					break;

				case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
					{
						if (view == null)
						{
							this.LoadView();
						}

                        // 运行缓存的消息
                        runCacheUIMsg();
                    }
					break;
				case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO:
					{
						if (view == null)
						{
							return;
						}
						view.showRecordTable();
					}
					break;
				case WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME:
					{
						if (view == null)
							return;

						UIWarShowReliveTimeData data = msgData as UIWarShowReliveTimeData;
						view.setHeroDieCountdown((ulong)data.pdbid, data.nReliveTime);
					}
					break;
				case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND:
					{
						switchTabWnd();
					}
					break;

				default:
					break;
			}
		}

		// 接受按键消息
		public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
		{
			if (m_wndView == null || StageManager.Instance.getCurrentState() != EStage_State.ESS_BATTLE)
			{
				return false;
			}

			if (index == FuntionShortCutSlotIndex.WarRecordInfoPannel)
			{
				if (status == KeyStatus.Down)
				{
					openTabWnd();
				}
				else if (status == KeyStatus.Up)
				{
					closeTabWnd();
				}
			}

			return base.OnKeyMessage(index, status);
		}

		private void switchTabWnd()
		{
			if(m_isVisible)
			{
				closeTabWnd();
			}
			else
			{
				openTabWnd();
			}
		}

		private void openTabWnd()
		{
			// 如果是第一次请求的话先请求一次战场数据
			if (LogicDataCenter.recordTableDataManager.isOneRequest)
				LogicDataCenter.recordTableDataManager.OnTabClick();
			LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible = true;
			SetVisible(true);

			notifyTabWndStatus();
		}

		private void closeTabWnd()
		{
			LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible = false;
			SetVisible(false);

			notifyTabWndStatus();
		}

		private void notifyTabWndStatus()
		{
			// 发向逻辑层是否判断战绩界面是否打开
			int nIsOpen = Convert.ToInt16(LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible);
			IntPtrHelper helper = new IntPtrHelper();
			IntPtr ptr = helper.toPtr<int>(ref nIsOpen);
			int len = Marshal.SizeOf(nIsOpen);
			GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_PALYER_TAB_ISOPEN, len, "", ptr, len);
			view.showRecordTable();
		}
    }
}

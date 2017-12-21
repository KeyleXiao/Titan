/*******************************************************************
** 文件名:	WarGameHudTopRightWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-15
** 版  本:	1.0
** 描  述:	战场右上角界面逻辑模块(接发消息)+视图模块(显示数据)	
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;

namespace USpeedUI.WarGameHudTopRight
{
    public class UWarGameHudTopRightWnd : UIBaseWnd<UWarGameHudTopRightWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_GAMEHUDTOPRIGHTVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarGameHudTopRightView/WarGameHudTopRightView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

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

        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                         this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UWarGameHudTopRightWndView : UIBaseWndView
    {
        public Text attackText;
        public Text assistText;
        public Text dieText;
        public Text netLatency;
        public Text FpsText;

        private float m_nextUpdateTime = 0.0f;
        public override bool Init(IUIWnd wnd)
        {
            attackText.text = "0";
            assistText.text = "0";
            dieText.text = "0";
            netLatency.text = "0ms";
            return base.Init(wnd);
        }

        public override void Update()
        {
            base.Update();

            if (Time.unscaledTime < m_nextUpdateTime)
            {
                return;
            }

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
            {
                return;
            }

            //更新网络延时
            int nNetLatency = GameLogicAPI.getNetLatency();
            if (nNetLatency < 1000)
                netLatency.text = "延迟：" + nNetLatency.ToString() + "毫秒";
            else
                netLatency.text = ((float)nNetLatency / 1000).ToString("F2") + "秒";

			if (nNetLatency < 100)
				netLatency.color = Color.green;
			else
				netLatency.color = Color.red;

            Color clr;
            if (nNetLatency <= 60)
            {
                ColorUtility.TryParseHtmlString("#32ff4f", out clr);
            }
            else if (nNetLatency < 100)
            {
                ColorUtility.TryParseHtmlString("#fffd32", out clr);
            }
            else
            {
                ColorUtility.TryParseHtmlString("#ff3232", out clr);
            }
            netLatency.color = clr;

            //更新fps
            if (FpsText)
            {
                int fpsValue = (int)FPS.Value;
				FpsText.text = ULocalizationService.Instance.Get("UIView", "Common", "FPS") + ":" + fpsValue;
			}

            DataCenter.WarRecordTableDataManager.WarPersonInfo personInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID((ulong)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID));
            if(personInfo == null)
            {
                return;
            }

            attackText.text = personInfo.nKillNum.ToString();
            assistText.text = personInfo.nAssists.ToString();
            dieText.text = personInfo.nDeadNum.ToString();

            m_nextUpdateTime = Time.unscaledTime + 0.5f;
        }

        public override void Destroy()
        {
            base.Destroy();
        }
    }
}

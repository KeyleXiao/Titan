
using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using U3D_Render.Common;
using UnityEngine;
using UnityEngine.UI;
using War;
using Data.PersonModel;
using U3D_Render;

namespace USpeedUI.WarGameHudTopRight
{
    enum EPreadType
    {
        WAR_NOX_SPREAD_NOT_BEGIN,
        WAR_NOX_SPREAD_COUNTDOWN,
        WAR_NOX_SPREADING,
    }

    public class BattleWarKAWnd : UIBaseWnd<BattleWarKAWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_BATTLEWAR_BATTLEWARKA;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarGameHudTopRightView/BattleWarKAView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_COLLECTING_GENIC, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_UPDATE_DEVILKING, this);

            LoadView();

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
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_COLLECTING_GENIC, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_UPDATE_DEVILKING, this);

            UnloadView();
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
                case WndMsgID.WND_MSG_BATTLEWAR_NOXSPREADCOUNTDOWN:
                    {
                        if (m_wndView)
                            m_wndView.OnNogSpreadCountdown(msgData as UIMsgCmdData);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.StartSpreadNoxSafeZone(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_BATTLEWAR_COLLECTING_GENIC:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnCollectingGenic(msgData as UBattleWarCollectGenic);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_BATTLEWAR_UPDATE_DEVILKING:
                    {
                        UIBattleWarUpdateDevilking msg = msgData as UIBattleWarUpdateDevilking;
                        LogicDataCenter.recordTableDataManager.CurDevilKing = msg.data.uidDevilKing;
                        EntityView ev = EntityFactory.getEntityViewByID(msg.data.uidDevilKing);
                        if (ev == null || ev.createinfo.byIsHero == 1)
                            break;
                        LogicDataCenter.warMinimapDataManager.AddObject(ev);
                        Trace.LogWarning("update devilking id=" + msg.data.uidDevilKing);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    [Serializable]
    public class CollectGenic
    {
        public Image headIcon;
        public Button headIconBtn;
        [HideInInspector]
        public int nHeroID;
        [HideInInspector]
        public int nSkinID;
    }
        

    public class BattleWarKAWndView : UIBaseWndView
    {
        public Text attackText;
        public Text assistText;
        public Text survivalText;
        public Text netLatency;
        public Text FpsText;
        public CollectGenic[] arrayCollectGenic = new CollectGenic[3];
        public WarNoxSpreadCDFrame warNoxSpreadCD;

        private float m_nextUpdateTime = 0.0f;
        private Sprite defaultSprite = null;


        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            attackText.text = "0";
            assistText.text = "0";
            survivalText.text = "0";
            netLatency.text = "0ms";
            warNoxSpreadCD.Init();

            for (int i = 0; i < arrayCollectGenic.Length; ++i)
            {
                int index = i;
                arrayCollectGenic[i].headIconBtn.onClick.AddListener(() => OnGenicImageClick(index));
                arrayCollectGenic[i].headIcon.gameObject.SetActive(false);
            }

            defaultSprite = arrayCollectGenic[0].headIcon.sprite;

            return true;
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

            WarRecordTableDataManager.WarPersonInfo personInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID((ulong)EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID));
            if(personInfo == null)
            {
                return;
            }

            attackText.text = personInfo.nKillNum.ToString();
            assistText.text = personInfo.nAssists.ToString();
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> recordInfoArry = LogicDataCenter.recordTableDataManager.recordInfoArry;
            int survial = 0;
            if (recordInfoArry !=  null)
            {
                survial = recordInfoArry.Count;
                foreach (var item in recordInfoArry.Values)
                {
                    survial -= (item.nDeadNum > 0 ? 1 : 0);
                }
            }
            survivalText.text = string.Format("{0}", survial);

            m_nextUpdateTime = Time.unscaledTime + 0.5f;
        }

        public void OnGenicImageClick(int index)
        {
            int nHeroID = arrayCollectGenic[index].nHeroID;
            if(nHeroID <= 0)
            {
                return;
            }

            SSchemePersonModelData heroData = new SSchemePersonModelData();
            if(PersonModelConfig.Instance.GetData_Out(nHeroID, out heroData) == false)
            {
                return;
            }

            string content = string.Format("是否进化为{0}英雄，进化后无法逆转", heroData.szHeroName);
            UBB.toHtml(ref content);
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: "进化",
                        message: content,
                        buttons: new UIWidgets.DialogActions() { { "进化", () => OnEnvolveHero(index) }, { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null } },
                        modal: true,
                        mutex: true
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool OnEnvolveHero(int index)
        {
            cmd_entity_war_change_vocation cmd = new cmd_entity_war_change_vocation();
            cmd.nGenicSlotID = index;
            EntityEventHelper.Instance.SendCommand<cmd_entity_war_change_vocation>(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_WAR_CHANGE_VOCATION, ref cmd);
            return true;
        }

        public void OnCollectingGenic(UBattleWarCollectGenic msg)
        {
            if (msg == null)
                return;

            SWar2ViewMsgGenicUpdate data = msg.genicData;
            if (data.nHeroIDList == null || data.nSkinIDList == null)
                return;

            int length = data.nHeroIDList.Length < arrayCollectGenic.Length ? data.nHeroIDList.Length : arrayCollectGenic.Length;
            for (int i = 0; i < length; ++i)
            {
                arrayCollectGenic[i].nHeroID = data.nHeroIDList[i];
                arrayCollectGenic[i].nSkinID = data.nSkinIDList[i];

                Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_BATTLEWAR_BATTLEWARKA, 1, 9, data.nHeroIDList[i]);
                if(sp == null)
                {
                    sp = defaultSprite;
                }
                arrayCollectGenic[i].headIcon.sprite = sp;
                arrayCollectGenic[i].headIcon.gameObject.SetActive(data.nHeroIDList[i] <= 0 ? false : true);
            }
        }

        public void StartSpreadNoxSafeZone(UIMsgCmdData msg)
        {
            warNoxSpreadCD.StartSpreadNoxSafeZone(msg);
        }

        public override void Destroy()
        {
            base.Destroy();
        }

        public void OnNogSpreadCountdown(UIMsgCmdData msg)
        {
            warNoxSpreadCD.NogSpreadCountdown(msg);
        }
    }

    public class WarNoxSpreadCDFrame : TimeCountDown
    {
        public Text countdownText;
        public Image progressImage;

        private int m_noxSpreadCD;
        private int m_noxSpreadCDOrigin;
        private int m_spreedBeginTick;
        private int m_spreadTime;
        private EPreadType m_spreadType = EPreadType.WAR_NOX_SPREAD_NOT_BEGIN;

        public void Init()
        {
            gameObject.SetActive(false);
            countdownText.text = "";
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            switch (m_spreadType)
            {
                case EPreadType.WAR_NOX_SPREAD_COUNTDOWN:
                    {
                        if (m_noxSpreadCD > 0)
                        {
                            countdownText.text = string.Format("毒圈扩散倒计时 {0}:{1}", m_noxSpreadCD / 60, m_noxSpreadCD % 60);
                            m_noxSpreadCD--;
                        }
                        else
                        {
                            base.CountDownCompleteCallBack();
                            countdownText.enabled = false;
                        }
                    }
                    break;
                case EPreadType.WAR_NOX_SPREADING:
                    {
                        double elapseTime = GameLogicAPI.getSyncTickCount() - m_spreedBeginTick;
                        if (m_spreadTime - elapseTime > 500)
                        {
                            progressImage.fillAmount = (float)elapseTime / (float)m_spreadTime;
                        }
                        else
                        {
                            base.CountDownCompleteCallBack();
                            progressImage.fillAmount = 1;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void NogSpreadCountdown(UIMsgCmdData msg)
        {
            if (msg == null)
                return;

            if(gameObject.activeSelf == false)
            {
                gameObject.SetActive(true);
            }

            gamelogic_war_nox_spread_cd data = IntPtrHelper.toData<gamelogic_war_nox_spread_cd>(msg.ptrParam);
            m_spreadType = EPreadType.WAR_NOX_SPREAD_COUNTDOWN;
            countdownText.enabled = true;
            countdownText.text = "";
            progressImage.fillAmount = 0;
            m_noxSpreadCD = (int)(((float)data.nSpreadTotalCDTime - (GameLogicAPI.getSyncTickCount() - data.nSpreadCDTick)) / 1000);
            m_noxSpreadCDOrigin = m_noxSpreadCD;
            this.StartCountDown(m_noxSpreadCD, 0.01f, 1.0f);
        }

        public void StartSpreadNoxSafeZone(UIMsgCmdData msg)
        {
            if (msg == null)
                return;

            gamelogic_war_safetyzone_info data = IntPtrHelper.toData<gamelogic_war_safetyzone_info>(msg.ptrParam);
            m_spreadType = EPreadType.WAR_NOX_SPREADING;
            progressImage.fillAmount = 0;
            m_spreedBeginTick = data.nSpreadBeginTick;
            m_spreadTime = data.nSpreadTime;
            this.StartCountDown(m_spreadTime / 1000, 0.01f, 0.5f);
        }
    }
}

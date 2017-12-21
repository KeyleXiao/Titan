/*******************************************************************
** 文件名:	EndOfBattleWarWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	   王全申
** 日  期:	2017/11/23
** 版  本:	1.0
** 描  述:	大逃杀战场结束时弹窗界面
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using DataCenter;
using GameLogic;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UEffect;
using War;

namespace USpeedUI.WarEndOf
{
    public class EndOfBattleWarWnd : UIBaseWnd<EndOfBattleWarWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_BATTLEWAR_END_OF_BATTLEWAR;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_MCAMP/WarEnd/EndOfBattleWar";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
        }

        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_RECORDTABLE_ACE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_RECORDTABLE_ACE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            base.Destroy();
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                        {
                            UnloadView();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_BATTLEWAR_RECORDTABLE_ACE:
                    {
                        Debug.Log("WndMsgID.WND_MSG_BATTLEWAR_RECORDTABLE_ACE");

                        if (m_wndView == null)
                            LoadView();

                        if (m_wndView != null)
                        {
                            m_wndView.OnTeamACE();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE:
                    {
                        Debug.Log("WndMsgID.WND_MSG_BATTLEWAR_LIFESTATE");
                        if (m_wndView == null)
                            LoadView();

                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdateLifeState((msgData as UBattleWarLifeState));
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class EndOfBattleWarWndView : UIBaseWndView
    {
        public Text aceTitleText;
        public Text aceRankText;
        public Text aceKDAText;
        public Button exitWarSuccessBtn;
        public Button exitWarFailBtn;

        public GameObject AceFrame;      // 团队团灭
        public GameObject DeadFrame;     // 本地玩家死亡，还剩队友
        private UEffectParamBase m_EffectParam;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            AceFrame.SetActive(false);
            DeadFrame.SetActive(false);
            AceFrame.GetComponent<Image>().color = new Color32(0x2C, 0X2C, 0X2C, 0XFF);

            return true;
        }

        public override void Destroy()
        {
            if (m_EffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);
                m_EffectParam = null;
            }

            base.Destroy();
        }

        public void OnTeamACE()
        {
            SetVisible(true);

            StartCoroutine(delayShowTeamACE());
        }
        private IEnumerator delayShowTeamACE()
        {
            // 延迟显示
            yield return new WaitForSeconds(1f);

            Debug.Log("OnTeamACE");

            if (m_EffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);
                m_EffectParam = null;
            }

            SWar2ViewMsgAce aceInfo = LogicDataCenter.recordTableDataManager.SelfCampACE;
            UGradient gradient = aceRankText.GetComponent<UGradient>();
            gradient.enabled = false;
            if (aceInfo.isWin > 0)
            {
                m_EffectParam = new UEffectPrefabParam(
                    _eType: UEffectPrefabType.UEPT_EndOfBattleWar_Win,
                    _tfAttachParent: AceFrame.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);

                exitWarSuccessBtn.gameObject.SetActive(true);
                exitWarFailBtn.gameObject.SetActive(false);
                aceTitleText.text = "“大吉大利，今晚吃鸡！”\n";
                gradient.SetGradientColorKey(new Color32(0xea, 0xb8, 0x30, 0xff), new Color32(0xff, 0xff, 0xff, 0xff));
            }
            else
            {
                m_EffectParam = new UEffectPrefabParam(
                _eType: UEffectPrefabType.UEPT_EndOfBattleWar_Failure,
                _tfAttachParent: AceFrame.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);

                exitWarSuccessBtn.gameObject.SetActive(false);
                exitWarFailBtn.gameObject.SetActive(true);
                aceTitleText.text = "“再接再厉，早日吃鸡！”";
                gradient.SetGradientColorKey(new Color32(0x7f, 0x9e, 0xb6, 0xff), new Color32(0xff, 0xff, 0xff, 0xff));
            }

            DeadFrame.SetActive(false);
            AceFrame.SetActive(true);
            gradient.enabled = true;
            WarRecordTableDataManager.WarPersonInfo personInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID((ulong)LogicDataCenter.playerSystemDataManager.SelfAcotrID);
            aceKDAText.text = string.Format("淘汰人数：{0}人, 助攻次数：{1}次", personInfo.nKillNum, personInfo.nAssists);
            aceRankText.text = string.Format("第{0}名", LogicDataCenter.recordTableDataManager.SelfCampACE.nRank);
        }

        public void OnUpdateLifeState(UBattleWarLifeState msg)
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            if (msg == null)
                return;

            if (msg.lifeState.pdbid == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
            {
                if (msg.lifeState.nLifeSate == (int)ELifeState.ELS_Dead)
                {
                    SetVisible(true);
                    DeadFrame.SetActive(true);
                }
            }
        }

        public bool ObServeWar()
        {

            return true;
        }

        public void OnExitWarFailBtnClick()
        {
            JumpBattleWarRecordView();
        }

        public void OnExitWarSuccessBtnClick()
        {
            JumpBattleWarRecordView();
        }

        private void JumpBattleWarRecordView()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_BATTLEWAR_RECORDTABLE, true);

            SoldierCamera.MainInstance<SoldierCamera>().LeaveCurMode();

            SetVisible(false);
        }

        // 游戏中途退出战场进入主城
        public void onReqExitWar()
        {
            Debug.Log("onReqExitWar");
            SoundPlayHandler.StopSceneSound();
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_EXIT_WAR);

            // 隐藏聊天框
            ChatForceConditionHide ChatData = new ChatForceConditionHide();
            ChatData.bForceHide = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, ChatData);
        }
    }
}

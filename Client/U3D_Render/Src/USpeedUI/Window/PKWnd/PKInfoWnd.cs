using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using USpeedUI.UWidgets;
using DG.Tweening;
using USpeedUI.UEffect;

namespace USpeedUI.PKWnd
{
    //计时器
    enum EMT_PKinfo_Timer : int
    {
        COUNTDOWN = 0,            //倒计时
        ELAPSEDTIME = 1,          //已用时间
    }

    public class PKInfoWnd : UIBaseWnd<UViewPKInfo>
    {

        public override WndID GetID()
        {
            return WndID.WND_ID_PKINFO;
        }

        public override string GetPrefabPath()
        {
            return "PKInfo/PKInfoView";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override bool Init()
        {
            base.Init();

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PKINFO_STARTPK,this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PKINFO_ENDPK, this);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PKINFO_STARTPK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PKINFO_ENDPK, this);
        }


        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_PKINFO_STARTPK:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.StartPK(data as PKPosInfo);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PKINFO_ENDPK:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.EndPK(data as PKResult);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class PKInfoItemView : UCustomUIBehaviour
    {
        public Text content;

        private uint uid;

        public void SetPkInfoData(uint id, string szInfo)
        {
            uid = id;
            content.text = szInfo;
        }

        public void OnOKClick()
        {
            cmd_creature_pk_handle_info data = new cmd_creature_pk_handle_info();
            data.uidOperator = uid;
            data.nAgree = 1;
            EntityEventHelper.Instance.SendCommand<cmd_creature_pk_handle_info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_PK_HANDLE_INFO, ref data);
            PKInfoItem item = new PKInfoItem(uid,null);
            //item.uidOperator = uid;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PKINO_REMOVE, item);
        }

        public void OnCancelClick()
        {
            cmd_creature_pk_handle_info data = new cmd_creature_pk_handle_info();
            data.uidOperator = uid;
            data.nAgree = 0;
            EntityEventHelper.Instance.SendCommand<cmd_creature_pk_handle_info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_PK_HANDLE_INFO, ref data);
            PKInfoItem item = new PKInfoItem(uid,null);
            //item.uidOperator = uid;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PKINO_REMOVE, item);
        }

    }

    public class UViewPKInfo : UIBaseWndView,ITimerHanlder
    {
        public RectTransform listContainer;
        public GameObject pkItem;
        public Button admit;
        public Text elapsedTimeDes;
        public Text elapsedTime;
        public Text distanceDes;
        public Text distance;

        public string SoundFilter;
        public string SoundKey_CountDown;
        public string SoundKey_Win;
        public string SoundKey_Lost;
        public string SoundKey_Tied;

        private int curCountDownTime;
        private int curElapsedTime;
        private UEffectParamBase m_EffectParam;

        private Vector3 centerPos;

        public override void Awake()
        {
            base.Awake();
        }

        public override bool Init(IUIWnd wnd)
        {
            admit.gameObject.SetActive(false);
            elapsedTimeDes.gameObject.SetActive(false);
            elapsedTime.gameObject.SetActive(false);
            distanceDes.gameObject.SetActive(false);
            distance.gameObject.SetActive(false);

            return base.Init(wnd);
        }

        public override void Destroy()
        {
            KillTimer((int)EMT_PKinfo_Timer.COUNTDOWN);
            KillTimer((int)EMT_PKinfo_Timer.ELAPSEDTIME);

            m_EffectParam = null;
            base.Destroy();
        }

        public void AdmitBtnClick()
        {
            cmd_creature_pk_cancel_pk data = new cmd_creature_pk_cancel_pk();
            data.nCancel = 1;
            EntityEventHelper.Instance.SendCommand<cmd_creature_pk_cancel_pk>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_PK_CANCEL_PK, ref data);
        }

        public void StartPK(PKPosInfo posInfo)
        {
            admit.gameObject.SetActive(true);
            curCountDownTime = 5;
            centerPos = new Vector3(posInfo.pos[0], posInfo.pos[1], posInfo.pos[2]);
            StartTimer((int)EMT_PKinfo_Timer.COUNTDOWN);
            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PKInfo_PKStart, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: true);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            m_EffectParam = param;
            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            if (Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter, SoundKey_CountDown, ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }
        }

        public void EndPK(PKResult result)
        {
            admit.gameObject.SetActive(false);
            elapsedTime.gameObject.SetActive(false);
            elapsedTimeDes.gameObject.SetActive(false);
            distanceDes.gameObject.SetActive(false);
            distance.gameObject.SetActive(false);
            KillTimer((int)EMT_PKinfo_Timer.COUNTDOWN);
            KillTimer((int)EMT_PKinfo_Timer.ELAPSEDTIME);
            curCountDownTime = 0;
            curElapsedTime = 0;
            if (m_EffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);
                m_EffectParam = null;
            }
            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            switch (result.nResult)
            {
                case 0:
                    {
                        if (result.bForceEnd)
                        {
                            // 强制结束不让播光效
                            break;
                        }
                        UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PKInfo_PKLost, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: true);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                        if(Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter, SoundKey_Lost, ref soundData))
                        {
                            SoundManager.CreateUISound(soundData.USoundID);
                        }
                    }
                    break;
                case 1:
                    {
                        UEffectParamBase param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PKInfo_PKWin, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: true);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);
                        if (Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter, SoundKey_Win, ref soundData))
                        {
                            SoundManager.CreateUISound(soundData.USoundID);
                        }
                    }
                    break;
                case 2:
                    {
                        UEffectParamBase param3 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PKInfo_PKTie, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: true);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param3);
                        if (Data.UISoundConfig.UISoundConfig.Instance.GetData(SoundFilter, SoundKey_Tied, ref soundData))
                        {
                            SoundManager.CreateUISound(soundData.USoundID);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void CountTime()
        {
            if (m_EffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_EffectParam);
                m_EffectParam = null;
            }
            elapsedTime.gameObject.SetActive(true);
            elapsedTimeDes.gameObject.SetActive(true);
            distanceDes.gameObject.SetActive(true);
            distance.gameObject.SetActive(true);
            curElapsedTime = 0;
            elapsedTime.text = "05 : 00";
            StartTimer((int)EMT_PKinfo_Timer.ELAPSEDTIME);
        }

        public void StartTimer(int nTimerID)
        {
            TimerManager.SetTimer(this, nTimerID, 1.0f);
        }

        public void KillTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, nTimerID);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EMT_PKinfo_Timer.COUNTDOWN:
                    {
                        curCountDownTime--;

                        if (curCountDownTime < 0)
                        {
                            CountTime();
                            KillTimer((int)EMT_PKinfo_Timer.COUNTDOWN);
                        }
                    }
                    break;
                case (int)EMT_PKinfo_Timer.ELAPSEDTIME:
                    {
                        curElapsedTime++;

                        int nMin, nSec;
                        string strMin, strSec;
                        nMin = 5 - (curElapsedTime / 60 + 1);
                        nSec = 60 - curElapsedTime % 60;
                        strMin = nMin.ToString();
                        strSec = nSec.ToString();
                        if (nMin / 10 < 1)
                            strMin = "0" + strMin;
                        if (nSec / 10 < 1)
                            strSec = "0" + strSec;

                        // 更新战斗时间
                        elapsedTime.text = strMin + " : " + strSec;
                    }
                    break;
                default:
                    break;
            }
        }

        public override void Update()
        {
            base.Update();

            if (EntityFactory.MainHero && distance.gameObject.activeSelf)
            {
                float dis = Vector3.Distance(EntityFactory.MainHero.transform.position, centerPos);
                distance.text = (int)dis + "m";
            }
        }
    }
}

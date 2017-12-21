using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;
using DG.Tweening;
using USpeedUI.UWidgets;
using Common.FSMSample;
using DataCenter;

namespace USpeedUI.FPSBullet
{
    public class FPSBulletWnd : UIBaseWnd<FPSBulletWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_FPSBULLET;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/FPSBullet/FPSBulletView";
        }

        public override bool Init()
        {
            //订阅改变准星位置事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS, this);
            //订阅改变准星是否显示事件
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FPSBULLET_VISIBLE, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            //退订改变准星位置事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FPSBULLET_VISIBLE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                //case WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE:
                //    {
                //        UFrontSightVisibleData visibleData = data as UFrontSightVisibleData;

                //        this.SetVisible(visibleData.bShow);
                //    }
                //    break;
                case WndMsgID.WND_MSG_WAR_FPSBULLET_VISIBLE:
                    {
                        UFPSBulletVisibleData visibleData = data as UFPSBulletVisibleData;

                        this.SetVisible(visibleData.bShow);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS:
                    {
                        UFrontSightPosData posData = data as UFrontSightPosData;
                        SetFontSizePos(posData.Pos);
                    }
                    break;
                case WndMsgID.WND_MSG_FPSBULLTE_UPDATECOUNT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.UpdateBulletCountDisplay(LogicDataCenter.fpsBulletDataManager);
                        }
                            
                    }
                    break;
                default:
                    break;
            }
        }

        private void SetFontSizePos(Vector2 _vNewPos)
        {
            if (m_wndView != null)
                m_wndView.SetFrontSightPos(_vNewPos);
        }        
    }

    public class FPSBulletWndView : UIBaseWndView
    {
        public enum EBulletDisplayType
        {
            /// <summary>
            /// 右下角文字方式显示
            /// </summary>
            Type_0,
            /// <summary>
            /// 中间纯图片方式显示
            /// </summary>
            Type_1,
        }
        [SerializeField]
        protected EBulletDisplayType m_eCurrentDisplayType = EBulletDisplayType.Type_0;
        [SerializeField]
        public UCustomUIBehaviour[] DisplayTypeArray = null;

        //测试用
        //public UFPSBullet testParam = new UFPSBullet();

        public override bool Init(IUIWnd wnd)
        {
            bool result = base.Init(wnd);

            if (DisplayTypeArray == null)
            {
                Trace.LogError("注意：FPSBulletWndView::DisplayTypeArray 是空的。");
            }
            else
            {
                ChangeBulletDisplayType(m_eCurrentDisplayType, null);
            }

            return result;
        }

        public void SetFrontSightPos(Vector2 pos)
        {
            if (IsCurrentDisplayTypeValid() && m_eCurrentDisplayType == EBulletDisplayType.Type_1)
            {
                DisplayTypeArray[(int)m_eCurrentDisplayType].GetRectTransform.anchoredPosition = pos;
            }
        }

        public void UpdateBulletCountDisplay(FPSBulletDataManager _fpsBulletParam)
        {
            if (IsCurrentDisplayTypeValid() && (DisplayTypeArray[(int)m_eCurrentDisplayType] is IFSM_Base))
                (DisplayTypeArray[(int)m_eCurrentDisplayType] as IFSM_Base).Execute(_fpsBulletParam);
        }

        public void ChangeBulletDisplayType(EBulletDisplayType _eNewType, FPSBulletDataManager _fpsBulletParam)
        {
            if (DisplayTypeArray != null && (int)m_eCurrentDisplayType < DisplayTypeArray.Length && (DisplayTypeArray[(int)m_eCurrentDisplayType] is IFSM_Base))
            {
                (DisplayTypeArray[(int)m_eCurrentDisplayType] as IFSM_Base).Exit(_fpsBulletParam);
                m_eCurrentDisplayType = _eNewType;
                (DisplayTypeArray[(int)m_eCurrentDisplayType] as IFSM_Base).Enter(_fpsBulletParam);

                UpdateBulletCountDisplay(_fpsBulletParam);
            }
        }

        public override void Update()
        {
            base.Update();

            //测试用
            //if (InputManager.GetKeyUp(KeyCode.Y))
            //{
            //    //testParam.CurBulletCount--;
            //    ChangeBulletDisplayType(m_eCurrentDisplayType, null);
            //}

            //if(InputManager.GetKeyUp(KeyCode.U))
            //{
            //    testParam.CurBulletCount = testParam.MaxBulletCount;
            //    UpdateBulletCountDisplay(testParam);
            //}

            //if(InputManager.GetKeyUp(KeyCode.I))
            //{
            //    ChangeBulletDisplayType(EBulletDisplayType.Type_0, testParam);
            //}

            //if (InputManager.GetKeyUp(KeyCode.O))
            //{
            //    ChangeBulletDisplayType(EBulletDisplayType.Type_1, testParam);
            //}

        }

        protected bool IsCurrentDisplayTypeValid()
        {
            return DisplayTypeArray != null && (int)m_eCurrentDisplayType < DisplayTypeArray.Length;
        }
    }

    /// <summary>
    /// 右下角文字方式显示
    /// </summary>
    [Serializable]
    public class UBulletDisplayType_0 : UCustomUIBehaviour, IFSM_Base
    {
        public Text BulletCountTxt = null;
        protected string m_strDisplay = "{0}/{1}";
        public int Enter(IFSMParam _data)
        {
            gameObject.SetActive(true);
            if (BulletCountTxt == null)
            {
                BulletCountTxt = GetRectTransform.GetComponentInChildren<Text>();
            }

            return RunningStatus.FINISHED;
        }

        public int Execute(IFSMParam _data)
        {
            FPSBulletDataManager data = _data as FPSBulletDataManager;

            if (data != null && BulletCountTxt != null)
            {
                BulletCountTxt.text = string.Format(m_strDisplay, data.CurBulletCount, data.MaxBulletCount);
            }

            return RunningStatus.FINISHED;
        }

        public int Exit(IFSMParam _data)
        {
            gameObject.SetActive(false);

            return RunningStatus.FINISHED;
        }

        public uint GetStateID()
        {
            return (uint)FPSBulletWndView.EBulletDisplayType.Type_0;
        }
    }

    /// <summary>
    /// 中间纯图片方式显示
    /// </summary>
    [Serializable]
    public class UBulletDisplayType_1 : UCustomUIBehaviour, IFSM_Base
    {
        public Image BulletCountDisplay = null;
        public float MaxImageFillCount = 0.33f;

        public int Enter(IFSMParam _data)
        {
            gameObject.SetActive(true);
            if (BulletCountDisplay == null)
            {
                BulletCountDisplay = GetRectTransform.GetComponentInChildren<Image>();
            }
            return RunningStatus.FINISHED;
        }

        public int Execute(IFSMParam _data)
        {
            FPSBulletDataManager data = _data as FPSBulletDataManager;
            if (data != null && BulletCountDisplay != null)
            {
                BulletCountDisplay.fillAmount = data.BulletCountPercent * MaxImageFillCount;
            }
            return RunningStatus.FINISHED;
        }

        public int Exit(IFSMParam _data)
        {
            gameObject.SetActive(false);
            return RunningStatus.FINISHED;
        }

        public uint GetStateID()
        {
            return (uint)FPSBulletWndView.EBulletDisplayType.Type_1;
        }
    }
}

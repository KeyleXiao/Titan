using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;
using DG.Tweening;

namespace USpeedUI.WarFrontSight
{
    public class WarFrontSightWnd : UIBaseWnd<WarFrontSightWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_FRONTSIGHT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarFrontSight/WarFrontSightView";
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {
            //订阅改变准星位置事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS, this);
            //订阅改变准星颜色事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_COLOR, this);
            //订阅改变准星是否显示事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE, this);
            //订阅狙击镜显示事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, this);
            //订阅准星瞄准敌人事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            //退订改变准星位置事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS, this);
            //退订改变准星颜色事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_COLOR, this);
            //退订改变准星是否显示事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE, this);
            //退订狙击镜显示事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY, this);
            //退订准星瞄准敌人事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {

            if(m_wndView == null)
            {
                this.LoadView();
                this.SetVisible(false);
            }
            switch (msgID)
            {

                case WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS:
                    {
                            UFrontSightPosData posData = data as UFrontSightPosData;
                            SetFontSizePos(posData.Pos);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FRONTSIGHT_COLOR:
                    {

                            UFrontSightColorData colorData = data as UFrontSightColorData;
                            m_wndView.SetFrontSightColor(colorData.nColor);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE:
                    {

                            UFrontSightVisibleData visibleData = data as UFrontSightVisibleData;
                            this.SetVisible(visibleData.bShow);

                    }
                    break;
                case WndMsgID.WND_MSG_SNIPERSCOPE_TOGGLEDISPLAY:
                    {
                        UIMsgCmdData cmdData = data as UIMsgCmdData;
                        if (cmdData != null)
                        {
                            SetVisible(Convert.ToBoolean(cmdData.nParam));
                        }                     
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FRONTSIGHT_TOGGLELOCKTARGET:
                    {
                        if(m_wndView != null && IsVisible())
                        {
                            UIMsgCmdData tmpData = data as UIMsgCmdData;
                            if(tmpData != null)
                            {
                                if (tmpData.nParam > 0)
                                    m_wndView.BeginSightAnim();
                                else
                                    m_wndView.EndSightAnim();
                            }
                            
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool LoadView()
        {
            bool result = base.LoadView();
            SetFontSizePos(DataCenter.WarFrontSightDataManager.Instance.FrontSightPos);
            DataCenter.WarFrontSightDataManager.Instance.CalculateFrontSightScreenPos();
            return result;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (!_bVisible)
            {
                m_wndView.EndSightAnim();
            }
        }

        private void SetFontSizePos(Vector2 _vNewPos)
        {
            m_wndView.SetFrontSightPos(_vNewPos);

            //将坐标转换为屏幕坐标
            //Vector2 ScreenPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), m_wndView.GetTransform().position);
            //DataCenter.WarFrontSightDataManager.Instance.FrontSightScreenPos = ScreenPos;
        }


    }

    public class WarFrontSightWndView : UIBaseWndView
    {
        public RectTransform frontSightViewRt;

        public Image crossHairTick1;
        public Image crossHairTick2;
        public Image crossHairTick3;
        public Image crossHairTick4;
        public Image FrontSightCircle;

        public Vector3 DefalutScale = Vector3.one;
        public Vector3 TargetScale = new Vector3(0.5f, 0.5f, 1);
        public float ScaleDuration = 0.2f;
        public int loopTime = 4;

        public override bool Init(IUIWnd wnd)
        {
            EndSightAnim();
            return base.Init(wnd);
        }

        public void SetFrontSightPos(Vector2 pos)
        {
            frontSightViewRt.anchoredPosition = pos;

            
        }

        public void SetFrontSightColor(Color cor)
        {
            crossHairTick1.color = cor;
            crossHairTick2.color = cor;
            crossHairTick3.color = cor;
            crossHairTick4.color = cor;
            FrontSightCircle.color = cor;
        }

        public bool bIsOpen = false;
        public bool bTest = false;
        public override void Update()
        {
            if (bTest)
            {
                if (bIsOpen)
                {
                    BeginSightAnim();
                    bIsOpen = false;
                }
                else
                {
                    EndSightAnim();
                    bIsOpen = true;
                }
                bTest = false;
            }

        }
        public void BeginSightAnim()
        {
            GetTransform().DOScale(TargetScale, ScaleDuration).SetLoops(loopTime, LoopType.Yoyo).SetAutoKill(true).OnComplete(()=> { EndSightAnim(); });
        }
        public void EndSightAnim()
        {
            GetTransform().localScale = DefalutScale;
        }
    }
}

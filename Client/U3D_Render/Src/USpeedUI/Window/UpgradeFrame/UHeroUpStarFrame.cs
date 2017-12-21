/*******************************************************************
** 文件名:	UHeroUpStarFrame.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	升星界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;

namespace USpeedUI.UpgradeFrame
{
    public class UHeroUpStarFrame : UIBaseWnd<UViewHeroUpStarFrame>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_UPGRADESTARHINT;
        }

        public override String GetPrefabPath()
        {
            return "HeroUpStarFrame/HeroUpStarFrame";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 订阅点击升星按钮事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
            // 退订点击升星按钮事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CHICKUPGRADEBUTTON_RETURN:
                    {
                        SetVisible(true);
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiHeroInfo = (UIMsgCmdData)msgData;
                        cmd_entity_hero_diff_expend cmdData = IntPtrHelper.toData<cmd_entity_hero_diff_expend>(uiHeroInfo.ptrParam);
                        m_wndView.setHeroUpStarInfo(cmdData);
                    }
                    break;
                case WndMsgID.WND_MSG_UPGRADESTARSUCESS_RESULT:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiHeroInfo = (UIMsgCmdData)msgData;
                        cmd_Return_HeroDetail_Upgrade_Success cmdData = IntPtrHelper.toData<cmd_Return_HeroDetail_Upgrade_Success>(uiHeroInfo.ptrParam);
                        m_wndView.showUpStarSucess(cmdData);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

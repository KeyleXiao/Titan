/*******************************************************************
** 文件名:	UViewHeroUpStarFrame.cs 
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
using UnityEngine.UI;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI.UEffect;
using UnityEngine;
using USpeedUI.PointShop;
using DataCenter;

namespace USpeedUI
{
    public class UViewHeroUpStarFrame : UIBaseWndView
    {
        public Text[] Hp;
        public Text[] PhysicAttack;
        public Text[] PhysicDef;
        public Text[] Mp;
        public Text[] MagicAttack;
        public Text[] MagicDef;
        public Image heroIcon;
        public Text heroName;
        public RectTransform heroStarFrame;
        public Text heroLvLimitFront;
        public Text heroLvLimitBack;
        public Text expendGold;
        public Text expendCard;
        public Text title;
        public Text btnDesc;

        private int nHeroID = 0;
        private UEffectParamBase m_effectParam;
        private HeroStarItem heroStarItem;

        public override bool Init(IUIWnd wnd)
        {
            m_effectParam = new UEffectPrefabParam(UEffectPrefabType.UEPT_HeroUpStar_Success, transform, null, null, true);
            title.text = ULocalizationService.Instance.Get("UIView", "HeroUpStarFrame", "Title");
            btnDesc.text = ULocalizationService.Instance.Get("UIView", "HeroUpStarFrame", "BtnDesc");

            if (heroStarItem == null)
            {
                heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(heroStarFrame);
            }
            
            return base.Init(wnd);
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            if (!visible)
            {
                if (m_effectParam != null)
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
            }
        }

        // 将升星前后的英雄属性赋值，并显示出来
        public void setHeroUpStarInfo( cmd_entity_hero_diff_expend cmdInfo )
        {
            WndID wndID = GetUIWnd().GetID();
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, wndID, 1, 3, cmdInfo.nHeroID);
            if (heroIcon.sprite == null)
            {
                heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, wndID, 1, 3, 10);
            }
            int nHeroStar = cmdInfo.nStarLv;

            heroStarItem.SetData(cmdInfo.nHeroID, nHeroStar, false, true);

            heroName.text = cmdInfo.chHeroName;
            if ( cmdInfo.nHaveGold < cmdInfo.nExpedGold )
            {
                expendGold.text = "金币 " + cmdInfo.nExpedGold.ToString() + "（现有：<color=red>" + cmdInfo.nHaveGold.ToString() + "</color>）";
            }
            else
            {
                expendGold.text = "金币 " + cmdInfo.nExpedGold.ToString() + "（现有：" + cmdInfo.nHaveGold.ToString() + "）";
            }

            if (cmdInfo.nHaveCardCount < cmdInfo.nExpendCard)
            {
                expendCard.text = "荣誉勋章 " + cmdInfo.nExpendCard.ToString() + "（现有：<color=red>" + cmdInfo.nHaveCardCount.ToString() + "</color>）";
            }
            else
            {
                expendCard.text = "荣誉勋章 " + cmdInfo.nExpendCard.ToString() + "（现有：" + cmdInfo.nHaveCardCount.ToString() + "）";
            }
            
            heroLvLimitFront.text = cmdInfo.nLimitLv.ToString();
            heroLvLimitBack.text = cmdInfo.nLimitLvAdd == 0 ? "" : ("+" + cmdInfo.nLimitLvAdd.ToString());

            for (int i = 0; i < UDefines.PROPERTY_COUNT; ++i)
            {
                if ( i >= 2)
                {
                    Hp[i].text = (cmdInfo.fHP[i] == 0 ? "" : ("+" + cmdInfo.fHP[i].ToString("F2")));
                    PhysicAttack[i].text = (cmdInfo.fPA[i] == 0 ? "" : ("+" + cmdInfo.fPA[i].ToString("F2")));
                    PhysicDef[i].text = (cmdInfo.fPD[i] == 0 ? "" : ("+" + cmdInfo.fPD[i].ToString("F2")));
                    Mp[i].text = (cmdInfo.fMagic[i] == 0 ? "" : ("+" + cmdInfo.fMagic[i].ToString("F2")));
                    MagicAttack[i].text = (cmdInfo.fMA[i] == 0 ? "" : ("+" + cmdInfo.fMA[i].ToString("F2")));
                    MagicDef[i].text = (cmdInfo.fMD[i] == 0 ? "" : ("+" + cmdInfo.fMD[i].ToString("F2")));
                }
                else
                {
                    Hp[i].text = cmdInfo.fHP[i].ToString();
                    PhysicAttack[i].text = cmdInfo.fPA[i].ToString();
                    PhysicDef[i].text = cmdInfo.fPD[i].ToString();
                    Mp[i].text = cmdInfo.fMagic[i].ToString();
                    MagicAttack[i].text = cmdInfo.fMA[i].ToString();
                    MagicDef[i].text = cmdInfo.fMD[i].ToString();
                }
            }
            nHeroID = cmdInfo.nHeroID;
        }

        // 当点击确定按钮时向逻辑层请求升星
        public void onClickUpStarButton()
        {
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<int>(ref nHeroID);
            int nLen = Marshal.SizeOf(nHeroID);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_UPGRADE_STAR, nLen, "", ptr, nLen);
        }

        // 当点击关闭按钮时直接关闭此界面
        public void onClickCloseButton()
        {
            if ( IsVisible() )
            {
                SetVisible(false);
            }
        }

        // 当点击参与战场按钮时
        public void onClickLoginMobaWar()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, null);
            // 隐藏其他同级窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERFRAME, false);
            UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);
            UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);
        }

        // 当点击捐赠卡牌时直接跳转到工会系统
        public void onGiveCard()
        {
            if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN) == 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_DISMISS_STATE);
            }
            else
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CLAN_REQUESTCARD, null);
                SetVisible(false);
            }
        }

        public void showUpStarSucess(cmd_Return_HeroDetail_Upgrade_Success cmdInfo)
        {
            if (!IsVisible())
                return;

            bool bSucess = (cmdInfo.nSucess == 1 ? true : false);
            if (bSucess)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
            }
        }
    }
}

/*******************************************************************
** 文件名:	GFxWarBottomCenterHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：WarBottomCenterHud.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using GameLogic;
using System.Runtime.InteropServices;
using ASpeedGame.Data.GameMobaSchemes;

partial class GFxWarBottomCenterHud
{
    #region CS->AS
    /*
    *   英雄属性相关
    */
    private void SetHeroLevel(entity_view_update_Numproperty data, U3D_Render.EntityView objev)
    {
        if(objev.IsHero)
        {
            if (!Value.IsValueValid(vLevelTF))
                return;

            vLevelTF.SetText(data.nValue.ToString());
        }
    }
    private void SetHeroExp(entity_view_update_Numproperty data, U3D_Render.EntityView objev)
    {
        if(objev.IsHero)
        {
            if (!Value.IsValueValid(vExpBar))
                return;

            //ExpBar.SetMember("minimum", 0);
            //ExpBar.SetMember("maximum", data.nOtherData);
            //ExpBar.SetMember("value", data.nValue);
            if (vExpBar.Invoke("SetStatusValue", data.nValue, data.nOtherData, 0) == null)
            {
                Trace.LogError("call ExpBar.SetStatusValue failed");
                return;
            }

        }
    }
    private void SetHeroStamina()
    {
        if (!Value.IsValueValid(vStaminaBar))
            return;

        //StaminaBar.SetMember("minimum", 0);
        //StaminaBar.SetMember("maximum", _maxStamina);
        //StaminaBar.SetMember("value", _currentStamina);
        if(vStaminaBar.Invoke("SetStatusValue", _currentStamina, _maxStamina,0) == null)
        {
            Trace.LogError("call StaminaBar.SetStatusValue failed");
            return;
        }
        
    }
    private void SetHeroHP()
    {
        if (!Value.IsValueValid(vHpBar) || !Value.IsValueValid(vShieldBar) )
            return;

        if (_nCurrentShield > 0)
        {
           // Trace.LogWarning("Shield Value ：" + _nCurrentShield);
            vShieldBar.SetMember("visible", true);
            if (vShieldBar.Invoke("SetStatusValue", _currentHp, _maxHp, 0) == null)
            {
                Trace.LogError("call vShieldBar.SetStatusValue failed");
                return;
            }

            int nTmpHp = _currentHp - _nCurrentShield;

            if (vHpBar.Invoke("SetStatusValue", nTmpHp, _maxHp, 0) == null)
            {
                Trace.LogError("call HpBar.SetStatusValue failed");
                return;
            }
        }
        else
        {
            vShieldBar.SetMember("visible", false);
            if (vHpBar.Invoke("SetStatusValue", _currentHp, _maxHp, 0) == null)
            {
                Trace.LogError("call HpBar.SetStatusValue failed");
                return;
            }
        }

        if (!Value.IsValueValid(vHPTF))
            return;

        string strHp = _currentHp + "/" + _maxHp;
        vHPTF.SetText(strHp);
    
    }
    private void SetHeroMP()
    {
        if (!Value.IsValueValid(vMpBar))
            return;

        if (vMpBar.Invoke("SetStatusValue", _currentMp, _maxMp, 0) == null)
        {
            Trace.LogError("call MpBar.SetStatusValue failed");
            return;
        }
        if (!Value.IsValueValid(vMPTF))
            return;

        string strMp = _currentMp + "/" + _maxMp;
        vMPTF.SetText(strMp);
    }
    private void SetGoodsStoreMoney(entity_view_update_Numproperty data, U3D_Render.EntityView objev)
    {
        if(objev.IsHero)
        {
            if (!Value.IsValueValid(vGoodsStoreBtn))
                return;

            string strMoney = data.nValue.ToString();;
            vGoodsStoreBtn.SetMember("label", strMoney);

        }
    }
    private void SetHeroVocationProtrait(entity_view_update_Numproperty data, U3D_Render.EntityView objev)
    {
        if(objev.IsHero)
        {
            if (!Value.IsValueValid(vHeroProtraitBtn))
                return;
            int nVocationID = EntityFactory.getEntityViewByID(EntityFactory.MainHeroID).Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            Trace.LogWarning("SetHeroVocationProtrait:" + nVocationID);
            vHeroProtraitBtn.SetMember("aHeroID", nVocationID);
        }
    }
    /**
    *   英雄技能相关
    */
    private void SetSkillSlotType(int nSlotID,SLOT_TYPE nSlotType)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;


        int _nType = Convert.ToInt32(nSlotType);
        if (vSkillBtnsMgr.Invoke("SetSkillSlotType", nSlotID, _nType) == null)
        {
            Trace.LogError("call SetSkillSlotType failed");
            return;
        }
    }
    private void SetSkillSlotID(int nSlotID)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;


        if (vSkillBtnsMgr.Invoke("SetSkillSlotID", nSlotID) == null)
        {
            Trace.LogError("call SetSkillSlotID failed");
            return;
        }
    }
    private void SetSkillIcon(int nSlotID,int nIconID)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillIcon",nSlotID,nIconID) == null)
        {
            Trace.LogError("call SetSkillIcon failed");
            return;
        }
        
    }
    private void SetSkillTooltip(int nSlotID, cmd_creature_set_slot data)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;


        string _strDes1 = data.slotview.szSlotDesc;

        if (vSkillBtnsMgr.Invoke("SetSkillBtnTooltip", nSlotID, _strDes1) == null)
        {
            Trace.LogError("call SetSkillBtnTooltip failed");
            return;
        }

    }
    private void SetSkillBtnUpgrade(int nUpgradeSlots,int nSpellPoint)
    {
        if (!Value.IsValueValid(vBuffsMgr) || !Value.IsValueValid(vSkillUpgradePanel))
            return;


        //Trace.LogWarning("C# SetSkillBtnUpgrade:" + _nUpgradeSlots);
        if (vSkillUpgradePanel.Invoke("SetSkillBtnUpgrade", nUpgradeSlots,nSpellPoint) == null)
        {
            Trace.LogError("call SetSkillBtnUpgrade failed");
            return;
        }
        //调整buff位置
        double dSkillUpgradePanelYPos = vSkillUpgradePanel.GetMember("y");
        double dBuffMgrH = vBuffsMgr.GetMember("height");
        dSkillUpgradePanelYPos -= dBuffMgrH;
        vBuffsMgr.SetMember("y", dSkillUpgradePanelYPos);
    }
    private void SetSkillLevel(int nSlotID,int nLevel)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillLevel", nSlotID, nLevel) == null)
        {
            Trace.LogError("call SetSkillLevel failed");
            return;
        }

    }
    private void SetSkillBtnEffect(int nSlotID,int nEffectType,int nBeginTime,int nEndTime)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillBtnEffect",nSlotID, nEffectType, nBeginTime, nEndTime) == null)
        {
            Trace.LogError("call SetSkillBtnEffect failed");
            return;
        }
        
    }
    private void SetSkillBtnEffectClear(int nSlotID)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillBtnEffectClear", nSlotID) == null)
        {
            Trace.LogError("call SetSkillBtnEffectClear failed");
            return;
        }

    }


    private void SetSkillFlagEffectClear(int nSlotID)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillFlagEffectClear",nSlotID) == null)
        {
           Trace.LogError("call SetSkillFlagEffectClear failed");
           return;
        }

    }
    private void SetSkillFlagEffect(int nSlotID, int nFlagCount,int nFreezeTime,int nFlagLeftTime,int nFlagTotalTime)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillFlagEffect", nSlotID, nFlagCount, nFreezeTime, nFlagLeftTime, nFlagTotalTime) == null)
        {
            Trace.LogError("call SetSkillFlagEffect failed");
            return;
        }
    }
    private void SetSkillFlagCount(int nSlotID,int nFlagCount)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        if (vSkillBtnsMgr.Invoke("SetSkillFlagCount", nSlotID, nFlagCount) == null)
        {
            Trace.LogError("call SetSkillFlagCount failed");
            return;
        }

    }
    private bool SetSkillBtnLockState(int nSlotID,bool bLockState)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return false;

        if (vSkillBtnsMgr.Invoke("SetSkillBtnLockState", nSlotID, bLockState) == null)
        {
            Trace.LogError("call SetSkillBtnLockState failed");
            return false;
        }

        return true;
    }
    private void SetSkillShortcutKey(int nSlotID,string strShortcutKey)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return ;
        if (vSkillBtnsMgr.Invoke("BtnSkillShortcutKey", nSlotID, strShortcutKey) == null)
        {
            Trace.LogError("call BtnSkillShortcutKey failed.");
            return ;
        }

    }
    /*
    *   施法条、下方飘字、Buff
    */
    private void AddBuff(uint nServerIndex, int nBuffID, uint nBuffEndTime, string strBuffName, string strBuffDes,int BuffLevel,bool BuffIsShow)
    {
        if (!Value.IsValueValid(vBuffsMgr))
            return;

        int nServerIndexInt = (int)nServerIndex;
        int EndTime = (int)nBuffEndTime;
        //public function AddBuff(serverIndex:int,buffIconID:int,buffEndTime:uint,buffName:String,buffDes:String,buffLevel:int)
        if (vBuffsMgr.Invoke("AddBuff", nServerIndexInt, nBuffID, EndTime, strBuffName, strBuffDes, BuffLevel, BuffIsShow) == null)
        {
            Trace.LogError("call AddBuff Failed");
            return;
        }
       
    }
    private void RemoveBuff(uint nServerIndex)
    {
        if (!Value.IsValueValid(vBuffsMgr))
            return;

        int nServerIndexInt = (int)nServerIndex;
        if (vBuffsMgr.Invoke("RemoveBuff", nServerIndexInt) == null)
        {
            Trace.LogError("call RemoveBuff Failed");
            return;
        }

    }

    private void AddGoodsButtonToSlot(cmd_war_persongoodsinfo data, Value asGoodsValue)
    {
        if (!Value.IsValueValid(vPurchasedSlotMgr))
            return;
                
        bool bCanUse = data.nCanUse == 1 ? true : false;
        //AddGoodsButtonToSlot(_nSlotID:int,_goods:Object,_nCanUseTimes:int,_nDisplayMaxUseTime:int,_bCanUse:Boolean,_nFreezeID:int,_nBuffID:int,_nSkillID:int):void
        if (vPurchasedSlotMgr.Invoke("AddGoodsButtonToSlot", data.nSlotIndex, asGoodsValue,data.nCanUseTimes,0, bCanUse,data.nFreezeID,data.nBuffID,data.nSkillID) == null)
        {
            Trace.LogError("call AddGoodsButtonToSlot failed");
            return;
        }
        
    }
    private void RemoveGoodsButtonFromSlot(int nSlotID)
    {
        if (!Value.IsValueValid(vPurchasedSlotMgr))
            return;

        if (vPurchasedSlotMgr.Invoke("RemoveGoodsButtonFromSlot", nSlotID) == null)
        {
            Trace.LogError("call RemoveGoodsButtonFromSlot faild");
            return;
        }

    }
    private void SetGoodsSlotShortcutKey(int nSlotID,string sttShortcut)
    {
        if (!Value.IsValueValid(vPurchasedSlotMgr))
            return;

        if (vPurchasedSlotMgr.Invoke("SetGoodsSlotShortcutKey", nSlotID, sttShortcut) == null)
        {
            Trace.LogError("call SetGoodsSlotShortcutKey failed");
            return;
        }

    }
    private void PlayPurchasedGoodsUseEffect(int nSlotID,int nBeginTime,int nEndTime)
    {
        if (!Value.IsValueValid(vPurchasedSlotMgr))
            return;
        if (vPurchasedSlotMgr.Invoke("PlayPurchasedGoodsUseEffect", nSlotID, nBeginTime, nEndTime) == null)
        {
            Trace.LogError("call PlayPurchasedGoodsUseEffect failed");
            return;
        }

    }
    /// <summary>
    /// 设置英雄属性面板各属性值
    /// </summary>
    /// <param name="_nATK">物理攻击</param>
    /// <param name="_nDEF">物理防御</param>
    /// <param name="_nMA">法术攻击</param>
    /// <param name="_nMDEF">法术防御</param>
    /// <param name="_nAF">普攻系数</param>
    /// <param name="_nSPD">移动速度</param>
    private void SetAllPropertyValue(string strATK, string strMA, string strDEF, string strMDEF, string strAF, string strCOOLREDUCE, string strCRC, string strSPD)
    {
        //SetAllPropertyValue(_nATK:int,_nMA:int,_nDEF:int,_nMDEF:int,_nSPD:int,_nASPD:int)
        if (!Value.IsValueValid(vPropertyPanel))
            return;

        if (vPropertyPanel.Invoke("SetAllPropertyValue", strATK, strMA, strDEF, strMDEF, strAF, strCOOLREDUCE, strCRC, strSPD) == null)
        {
            Trace.LogError("call SetAllPropertyValue failed");
            return;
        }
    }

    // 设置是否可选
    public void setIsEnable( bool bIsEnable )
    {
        if ( !Value.IsValueValid(MyRoot))
        {
            return;
        }
        if (MyRoot.Invoke("setIsEnable", bIsEnable) == null)
        {
            Trace.LogError("call setIsEnale failed");
            return;
        }
    }

    //设置是否需要置灰
    public void SetSkillBtnIsDark(int nSlotID,bool bIsDark)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        ReCalSlotIndex(ref nSlotID);

        if (vSkillBtnsMgr.Invoke("SetSkillBtnIsDark", nSlotID, bIsDark) == null)
        {
            Trace.LogError("call SetSkillBtnIsDark failed.");
            return;
        }
    }

    public void SetSpellSlot2AS(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
    {
        if (!Value.IsValueValid(vSkillBtnsMgr))
            return;

        //_nSlotID: int, _nSlotType:int, _nSlotIconID:int, _strSlotTipsDesc:String,_nSlotLevel: int, _bSpellUseFlag:Boolean
        if (vSkillBtnsMgr.Invoke("SetSpellSlot", nSlotID, nSlotType, nSlotIconID, strSlotTipsDesc, nSlotLevel, bSpellUseFlag) == null)
        {
            Trace.LogError("call SetSpellSlot failed");
            return;
        }
    } 
    #endregion
}
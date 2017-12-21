/*******************************************************************
** 文件名:	GFxWarBottomCenterHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/8/21
** 版  本:	1.0
** 描  述:	战场Moba类型主界面 中下方的界面
** 应  用:  
 *          对应Flash Layout文件：WarBottomCenterHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
***************************** 修改记录 *****************************
** 修改人: 昔文博
** 日  期: 2016/05/18
** 描  述: 新增客户端自己修改槽位键值文本的方法
 *             SetSlotShortcutString()
*******************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using GameLogic;
using System.Runtime.InteropServices;
using ASpeedGame.Data.GameMobaSchemes;
using System.Collections.Generic;
using War;

partial class GFxWarBottomCenterHud : SFMovieEx
{
    /*经验条*/
    protected Value vExpBar;
    /*体力值*/
    protected Value vStaminaBar;
    /*生命条*/
    protected Value vHpBar;
    /*护盾条*/
    protected Value vShieldBar;
    /*魔法条*/
    protected Value vMpBar;
    /*等级文本*/
    protected Value vLevelTF;
    /*HP文本*/
    protected Value vHPTF;
    /*MP文本*/
    protected Value vMPTF;
    /*物品购买界面按钮*/
    protected Value vGoodsStoreBtn;
    /*施法条*/
    protected Value vSkillProgressBar;
    protected Value vSkillProgressBar2;
    /*buff 管理器*/
    protected Value vBuffsMgr;
    /*技能 管理器*/
    protected Value vSkillBtnsMgr;
    /*已购买物品管理器*/
    protected Value vPurchasedSlotMgr;
    /*技能升级面板*/
    protected Value vSkillUpgradePanel;
    /*英雄属性面板*/
    protected Value vPropertyPanel;
    /*英雄头像按钮*/
    protected Value vHeroProtraitBtn;
    /**
        部分临时暂存数据
    */
    /*特殊能力值*/
    private int _currentStamina, _maxStamina = 0;
    /*血量*/
    private int _currentHp, _maxHp = 0;
    /*魔法值*/
    private int _currentMp, _maxMp = 0;
    /*要显示的属性值*/
    private Dictionary<ENTITY_PROPERTY,float> _propertyValue = new Dictionary<ENTITY_PROPERTY, float>();
    /*护盾值*/
    private int _nCurrentShield = 0;
    /// <summary>
    /// 英雄技能控制脚本
    /// </summary>
    private SkillCtrl MyHeroSkillCtrl = null;
    /// <summary>
    /// 英雄对象
    /// </summary>
    private GameObject MyHeroGO = null;
    /// <summary>
    /// 存一下当前可用的升级点，用于检测升级快捷键
    /// </summary>
    private int currentSpellUpgradePoint = 0;

    private const int nMainSkillSlotIdBegin = 0;

    private const int nMainSkillSlotIdEnd = 3;

    private const int nSummerSkillSlotIdBegin = 13;

    private const int nSummerSkillSlotIdEnd = 14;

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        bCanSyncTickCount = true;
        _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_PA, 0); _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_MA, 0);
        _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_PD, 0); _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_MD, 0);
        _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_AF, 0); _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE, 0);
        _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_CRC, 0); _propertyValue.Add(ENTITY_PROPERTY.PROPERTY_SD, 0);
    }

    override public void Update()
    {
        CheckSpellUpdateByShortcut();


    }
    public override void OnDestroy()
    {
        base.OnDestroy();
        _propertyValue = null;
    }

    public override void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {
        if (e.objev == null || !this.IsRegisterMovieSuccess)
        {
            return;
        }

        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_SKIN:
                SetHeroVocationProtrait(e.data, e.objev);
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_EXP:
                SetHeroExp(e.data, e.objev);
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                SetHeroLevel(e.data, e.objev);
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_STAMINA:
                {
                    if (e.objev.IsHero)
                    {
                        _currentStamina = e.data.nValue;
                        SetHeroStamina();
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_STAMINA:
                {
                    if (e.objev.IsHero)
                    {
                        _maxStamina = e.data.nValue;
                        SetHeroStamina();
                    }
                }
                break;

            case (int)ENTITY_PROPERTY.PROPERTY_MP:
            case (int)ENTITY_PROPERTY.PROPERTY_ENERGY:
                {
                    if (e.objev.IsHero)
                    {
                        //if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_MP)
                        {
                            _currentMp = e.data.nValue;
                            SetHeroMP();
                        }
                        //else
                        //{
                        //    Trace.Log("PROPERTY_ENERGY:目前没实现特殊能力值的显示效果");
                        //}
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_ENERGY:
                {
                    if (e.objev.IsHero)
                    {
                        //if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_MP)
                        {
                            _maxMp = e.data.nValue;
                            SetHeroMP();
                        }
                        //else
                        //{
                        //    Trace.Log("PROPERTY_MAX_ENERGY:目前没实现特殊能力值的显示效果");
                        //}
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT://金钱
                {
                    SetGoodsStoreMoney(e.data, e.objev);
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_HP:
                {
                    if (e.objev.IsHero)
                    {
                        _currentHp = e.data.nValue;
                        SetHeroHP();
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                {
                    if (e.objev.IsHero)
                    {
                        _maxHp = e.data.nValue;
                        SetHeroHP();
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_SHIELD:
                {
                    if (e.objev.IsHero)
                    {
                        //_nCurrentShield = e.data.nValue;
                        SetHeroHP();
                    }
                }
                break;
            case (int)ENTITY_PROPERTY.PROPERTY_PA:
            case (int)ENTITY_PROPERTY.PROPERTY_MA:
            case (int)ENTITY_PROPERTY.PROPERTY_PD:
            case (int)ENTITY_PROPERTY.PROPERTY_MD:
            case (int)ENTITY_PROPERTY.PROPERTY_SD:
            case (int)ENTITY_PROPERTY.PROPERTY_AF:
            case (int)ENTITY_PROPERTY.PROPERTY_CRC:
            case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE:
                {
                    if (e.objev.IsHero)
                    {
                        ENTITY_PROPERTY index = (ENTITY_PROPERTY)e.data.nPropID;
                        float nValue = e.data.nValue;
                        if (!Mathf.Approximately(_propertyValue[index], nValue))
                        {
                            _propertyValue[index] = nValue;

                            if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_AF)
                            {
                                _propertyValue[index] /= 10000;
                            }
                            else if(e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_CRC || e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE)
                            {
                                _propertyValue[index] /= 100;
                            }

                            SetAllPropertyValue(_propertyValue[ENTITY_PROPERTY.PROPERTY_PA].ToString(), _propertyValue[ENTITY_PROPERTY.PROPERTY_MA].ToString(),
                                                _propertyValue[ENTITY_PROPERTY.PROPERTY_PD].ToString(), _propertyValue[ENTITY_PROPERTY.PROPERTY_MD].ToString(),
                                                _propertyValue[ENTITY_PROPERTY.PROPERTY_AF].ToString("f2"), _propertyValue[ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE].ToString(),
                                                _propertyValue[ENTITY_PROPERTY.PROPERTY_CRC].ToString(), _propertyValue[ENTITY_PROPERTY.PROPERTY_SD].ToString());
                        }
                    }
                }
                break;
        }
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        Value MainFrame;
        GetMemberWithCheck(MyRoot, "MainFrame", out MainFrame);

        GetMemberWithCheck(MainFrame, "ExpBar", out vExpBar);
        GetMemberWithCheck(MainFrame, "StaminaBar", out vStaminaBar);
        GetMemberWithCheck(MainFrame, "HpBar", out vHpBar);
        GetMemberWithCheck(MainFrame, "MpBar", out vMpBar);
        GetMemberWithCheck(MainFrame, "LevelTF", out vLevelTF);
        GetMemberWithCheck(MainFrame, "HPTF", out vHPTF);
        GetMemberWithCheck(MainFrame, "MPTF", out vMPTF);
        GetMemberWithCheck(MainFrame, "BuffsMgr", out vBuffsMgr);
        GetMemberWithCheck(MainFrame, "GoodsStoreBtn", out vGoodsStoreBtn);
        GetMemberWithCheck(MainFrame, "mcSkillProgressBar", out vSkillProgressBar);
        GetMemberWithCheck(MainFrame, "mcSkillProgressBar2", out vSkillProgressBar2);
        GetMemberWithCheck(MainFrame, "SkillBtnsMgr", out vSkillBtnsMgr);
        GetMemberWithCheck(MainFrame, "PurchasedSlotMgr", out vPurchasedSlotMgr);
        GetMemberWithCheck(MainFrame, "SkillUpgradePanel", out vSkillUpgradePanel);
        GetMemberWithCheck(MainFrame, "PropertyPanel", out vPropertyPanel);
        GetMemberWithCheck(MainFrame, "HeroProtraitBtn", out vHeroProtraitBtn);
        GetMemberWithCheck(MainFrame, "ShieldBar", out vShieldBar);

    }
    /// <summary>
    /// 获取当前英雄的技能控制脚本
    /// </summary>
    /// <returns></returns>
    private SkillCtrl GetMyHeroCtrl()
    {
        if (MyHeroSkillCtrl == null)
        {
            MyHeroGO = EntityFactory.getEntityGameObjectByID(EntityFactory.MainHeroID);
            if (MyHeroGO == null)
            {
                Trace.LogError("get Hero GameObject failed");
                return null;
            }
            MyHeroSkillCtrl = MyHeroGO.GetComponent<SkillCtrl>();
        }
        return MyHeroSkillCtrl;
    }
    private void ReCalSlotIndex(ref int _nSlotIndex)
    {
        if (_nSlotIndex > 3)
            _nSlotIndex -= 9;
    }

    private bool CheckSlotIndexRange(int _nSlotIndex)
    {
        if (((_nSlotIndex >= nMainSkillSlotIdBegin) && (_nSlotIndex <= nMainSkillSlotIdEnd)) || ((_nSlotIndex == nSummerSkillSlotIdBegin)))// && (_data.nSlotIndex <= nSummerSkillSlotIdEnd)))
            return true;
        return false;
    }
    /// <summary>
    /// 检测是否有通过快捷键升级技能
    /// </summary>
    private void CheckSpellUpdateByShortcut()
    {
        //没技能点就不检测
        if (currentSpellUpgradePoint <= 0)
            return;
        for(int slotID = 0;slotID<3;slotID++)
        {
            if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0+ slotID))
            {
                onSkillUpgradeButtonClick(slotID);
            }
        }
    }
    #region AS->CS
    /**
    *   英雄技能相关
    */
    public void onSkillUpgradeButtonClick(int _nClickSlotID)
    {
        cmd_creature_upgrade_spell data = new cmd_creature_upgrade_spell();
        data.nSlotIndex = _nClickSlotID;

        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_creature_upgrade_spell>(ref data);
        int len = Marshal.SizeOf(data);

        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_UPGRADE_SPELL, len, "", ptr, len);
    }
    /*
    *   施法条、下方飘字  、打开商店  
    */
    public void ProgressBarComplete()
    {

    }

    public void OpenGoodsStoreFrameClick()
    {
        GFxWarGoodsStore goodsStore = SFGFxMovieManager.GetWindow<GFxWarGoodsStore>();
        if (SFGFxMovieManager.CheckTheMovieCanUse(goodsStore))
        {
            if (!goodsStore.IsGoodsStoreOpened)
            {
                PopupMovieManager.Instance.PushMovie(goodsStore);
            }
            else
            {
                PopupMovieManager.Instance.PopMovie(goodsStore);
            }
        }
    }

    public void onPurchasedSlotDrop(int _nOrigDragSlotID, int _nTargetDropSlotID)
    {
        cmd_war_personexchangeslot data = new cmd_war_personexchangeslot();
        data.nSrcSlotIndex = _nOrigDragSlotID;
        data.nTargetSlotIndex = _nTargetDropSlotID;

        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_war_personexchangeslot>(ref data);
        int len = Marshal.SizeOf(data);

        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_EXCHANGE_SLOT, len, "", ptr, len);

    }
    public void onMobaGoodsItemClick(int _nPurchasedGoodsSlotID)
    {
        GetMyHeroCtrl().Press_Skill((int)SLOT_TYPE.SLOT_TYPE_GOODS, _nPurchasedGoodsSlotID);
    }
    public void onSkillBtnClick(int _nSkillSlotID)
    {
        if (_nSkillSlotID >= 5 && _nSkillSlotID <= 6)
            _nSkillSlotID += 9;
        GetMyHeroCtrl().Press_Skill((int)SLOT_TYPE.SLOT_TYPE_SKILL, _nSkillSlotID);
    }
    /// <summary>
    /// 鼠标在技能/物品插槽上移动/移除事件
    //param1：插槽ID
    //param2：插槽类型 1：物品、2：技能//这里与CS：：SLOT_TYPE对应
    //param3：鼠标类型 0：离开、1：进入
    //param4：额外的数据类型
    /// </summary>
    public void onAllSlotMouseEvent(int _nSlotId,int _nSlotType,int _nMouseEventType)
    {
        cmd_entity_mouse_enter_slot data = new cmd_entity_mouse_enter_slot();

        data.nEnterSlot = _nMouseEventType;
        data.nSlotType = _nSlotType;
        data.nSlotIndex = _nSlotId;

        IntPtrHelper helper = new IntPtrHelper();
        IntPtr ptr = helper.toPtr<cmd_entity_mouse_enter_slot>(ref data);
        int len = Marshal.SizeOf(data);

        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_MOUSE_ENTER_SLOT, len, "", ptr, len);
        
    }
    #endregion

    #region 外部接口
    /**
    *   英雄技能相关
    */
    public void SetSpellSlot(cmd_creature_set_slot _data)
    {
        //召唤师技能(5 - 6)目前还没有，不接受消息
        if (CheckSlotIndexRange(_data.nSlotIndex))
        {
          
            ReCalSlotIndex(ref _data.nSlotIndex);

            //主动技能(0-2)
            //天赋技能(3)
            //召唤师技能(AS中序号是4)(策划配置是13)
            //SetSkillSlotID(_data.nSlotIndex);
            //SetSkillSlotType(_data.nSlotIndex, );
            //SetSkillIcon(_data.nSlotIndex, _data.slotview.nIconID);
            //SetSkillTooltip(_data.nSlotIndex, _data);
            //SetSkillLevel(_data.nSlotIndex, );
            int _nType = Convert.ToInt32(_data.nSlotType);
            bool bUseSlotFlag = Convert.ToBoolean(_data.nOverlaySpell);

      
            SetSpellSlot2AS(_data.nSlotIndex, _nType, _data.slotview.nIconID, _data.slotview.szSlotDesc, _data.nSlotLevel, bUseSlotFlag);
            GetMyHeroCtrl().SetSkillIDThroughSlot(_data.nSlotIndex, _data.slotview.nId);

            UBBProperty ubbProperty = UBBProperty.GetInstance();
            ubbProperty.SetShortCutSlotSkillId(_data.nSlotIndex, _data.slotview.nId);

        }
    }
    public void SetSpellSlotUpgrade(cmd_creature_spell_point _data)
    {
        int upgradeSlots = 0;
        int tmpID = 0;

        for (int i = 0; i < 4; i++)
        {
            if (_data.nAddSpellPoint[i] > 0)
            {
                switch (i)
                {
                    case 0:
                        tmpID = 1;
                        break;
                    case 1:
                        tmpID = 2;
                        break;
                    case 2:
                        tmpID = 4;
                        break;
                    case 3:
                        tmpID = 8;
                        break;
                }
                upgradeSlots |= tmpID;
            }
        }
        currentSpellUpgradePoint = _data.nSpellPoint;
        SetSkillBtnUpgrade(upgradeSlots, _data.nSpellPoint);
    }
    public void SetSpellSlotEffect(cmd_creature_set_slot _data)
    {
        //主动技能(0-2)
        //天赋技能(3)
        //召唤师技能(AS中序号是4)(策划配置是13)
        if (CheckSlotIndexRange(_data.nSlotIndex))
        {
            ReCalSlotIndex(ref _data.nSlotIndex);

            int effectType = (int)(_data.slotview.nViewType | SoltViewType.ViewType_RightCircle);
            if (_data.slotview.nViewType != SoltViewType.ViewType_NONE)
            {
                int endTime = _data.slotview.nMaxData;
                int beginTime = _data.slotview.nMaxData - _data.slotview.nCurData < 0 ? 0 : _data.slotview.nMaxData - _data.slotview.nCurData;

                if (_data.slotview.nCurData == 0)
                    SetSkillBtnEffectClear(_data.nSlotIndex);
                else
                    SetSkillBtnEffect(_data.nSlotIndex, effectType, beginTime, endTime);
            }
        }
    }
    public void SetSpellSlotFlagEffect(cmd_set_spell_overlay _data)
    {
        //如果这里有报错，先看看逻辑层这个nID是否是槽位ID，以前是下发技能ID的
        
        ReCalSlotIndex(ref _data.nSlotIndex);

        //if (_data.nFlag == 0)
        //{
        //    SetSkillFlagEffectClear(_data.nSlotIndex);
        //}
        //else if (_data.nFlag == 1)
        //{
        if (_data.nType != 0)
            SetSkillFlagEffect(_data.nSlotIndex, _data.nFlag, _data.nFreezeLeftTime, _data.nLeftTime, _data.nTotalTime);
        else
            SetSkillFlagCount(_data.nSlotIndex, _data.nFlag);
        //}

    }
    public bool SetSpellSlotLockState(int _nSlotID, bool _bLockState)
    {

        if (_nSlotID < 0)
        {
            Trace.LogWarning("技能ID：" + _nSlotID + "对应的插槽没找到，不能切换锁定状态");
            return false;
        }
        if (!SetSkillBtnLockState(_nSlotID, _bLockState))
            return false;

        //技能控件设置该技能不能释放
        //GetMyHeroCtrl().SetSkillEnable(_nSkillID, !_bLockState);
        return true;
    }
    /*
    *   施法条、下方飘字   、buff 
    */
    public void PlayProgressBar(Double _dProgressTime,int _nBarIndex = 1)
    {
        Value tmpSkillProgressBar;

        if (_nBarIndex == 2)
            tmpSkillProgressBar = vSkillProgressBar2;
        else
            tmpSkillProgressBar = vSkillProgressBar;

        if (Value.IsValueValid(tmpSkillProgressBar))
        {
            if (tmpSkillProgressBar.Invoke("PlayProgressBar", _dProgressTime) == null)
            {
                Trace.LogError("call PlayProgressBar failed");
                return;
            }
        }
    }
    public void ClearProgressBar(int _nBarIndex = 1)
    {
        Value tmpSkillProgressBar;

        if (_nBarIndex == 2)
            tmpSkillProgressBar = vSkillProgressBar2;
        else
            tmpSkillProgressBar = vSkillProgressBar;

        if (Value.IsValueValid(tmpSkillProgressBar))
        {
            if (tmpSkillProgressBar.Invoke("ClearProgressBar") == null)
            {
                Trace.LogError("call ClearProgressBar failed");
                return;
            }
        }
    }


    public void SetHeroBuff(cmd_creature_setBuff _data)
    {
        if (_data.nType == 1)
        {
            bool bShow = false;
            if (_data.nIsShow != 0)
            {
                bShow = true;
            }
            AddBuff(_data.dwIndex, _data.nIconID, _data.dwTime, _data.szName, _data.szTips, _data.nLevel, bShow);
        }
        else if (_data.nType == 2)
        {
            RemoveBuff(_data.dwIndex);
        }
    }
    public void SetPersonBuyGoodsInfo(IntPtr ptrParam, int nPtrLen)
    {
        //IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

        //cmd_war_persongoodsinfo_count obj = helper.get<cmd_war_persongoodsinfo_count>();
        //for (int i = 0; i < obj.nCount; ++i)
        //{
        //    cmd_war_persongoodsinfo data = helper.get<cmd_war_persongoodsinfo>();
        //    if (data.nGoodID != 0)
        //    {

        //        Value ASGoodsValue;
        //        //GameMobaSchemesManager.Instance.GetASMobaGoods(data.nGoodID, out ASGoodsValue, this);

        //        if (!Value.IsValueValid(ASGoodsValue))
        //            continue;

        //        AddGoodsButtonToSlot(data, ASGoodsValue);
        //    }
        //    else
        //    {
        //        RemoveGoodsButtonFromSlot(data.nSlotIndex);
        //    }
        //}

    }
    public void SetPurchasedGoodsUseEffect(cmd_creature_set_slot _data)
    {
        if (_data.nSlotIndex < 0)
            return;
        int endTime = _data.slotview.nMaxData;
        int beginTime = _data.slotview.nMaxData - _data.slotview.nCurData < 0 ? 0 : _data.slotview.nMaxData - _data.slotview.nCurData;
        

        PlayPurchasedGoodsUseEffect(_data.nSlotIndex, beginTime, endTime);
    }

    public void SetPurchasedGoodsCanDragDrop(cmd_creature_set_slot _data)
    {
        if (_data.nSlotIndex < 0)
            return;

        if (!Value.IsValueValid(vPurchasedSlotMgr))
            return;

        bool bCanDragDrop = Convert.ToBoolean(_data.slotview.bCanDrag);
        if (vPurchasedSlotMgr.Invoke("SlotCanDragDrop", _data.nSlotIndex, bCanDragDrop) == null)
        {
            Trace.LogError("call SlotCanDragDrop failed");
            return;
        }
    }

    /// <summary>
    /// 设置英雄魔法类型
    /// </summary>
    /// <param name="_data"></param>
    public void SetHeroMagicType(cmd_creature_magicType _data)
    {

    }
    public void SetSlotShortcut(IntPtr recordInfo, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(recordInfo, nPtrLen);
      
        cmd_count obj = helper.get<cmd_count>();
        for (int i = 0; i < obj.nCount; i++)
        {
            cmd_Slot_Shortcut data = helper.get<cmd_Slot_Shortcut>();

            if (data.nType == (int)SLOT_TYPE.SLOT_TYPE_GOODS)
            {
                SetGoodsSlotShortcutKey(data.nSlotId, data.szShortcut);
            }
            else if (data.nType == (int)SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                if (CheckSlotIndexRange(data.nSlotId))//(((data.nSlotId >= 0) && (data.nSlotId <= 4)) || ((data.nSlotId >= 13) && (data.nSlotId <= 14)))
                {
                    ReCalSlotIndex(ref data.nSlotId);
                    SetSkillShortcutKey(data.nSlotId, data.szShortcut);
                }
            }

        }
    }


    //提供一个客户端自己更改槽位键值的方法
    public void SetSlotShortcutString(int nType,int nSlotId,string keyCode)
    {
        if (nType == (int)SLOT_TYPE.SLOT_TYPE_GOODS)
        {
            SetGoodsSlotShortcutKey(nSlotId, keyCode);
        }
        else if (nType == (int)SLOT_TYPE.SLOT_TYPE_SKILL)
        {
            if (CheckSlotIndexRange(nSlotId))//(((nSlotId >= 0) && (nSlotId <= 4)) || ((nSlotId >= 13) && (nSlotId <= 14)))
            {
                ReCalSlotIndex(ref nSlotId);
                SetSkillShortcutKey(nSlotId, keyCode);

                UBBProperty ubbProperty = UBBProperty.GetInstance();
                ubbProperty.SetShortCutSlotDesc(nSlotId, keyCode);
            }
        }
    }

    public void SetMpEffectSpell(cmd_creature_mp_effect_spell _data)
    {
        if(_data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
        {
            SetSkillBtnIsDark(_data.nSlotIndex,Convert.ToBoolean(_data.nShowType));
        }
    }
    public float GetBottomHudYPos
    {
        get
        {
            double yPos = 0;
            if (MyRoot.IsValid())
            {
                Value MyRootRect = MyRoot.Invoke("RectRelativeToStage", MyRoot);
                if (MyRootRect != null)
                    yPos = (double)MyRootRect.GetMember("y");
            }
            return (float)yPos;
        }
    }
    #endregion
}

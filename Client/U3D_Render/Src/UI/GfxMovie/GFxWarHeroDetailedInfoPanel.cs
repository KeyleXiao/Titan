///*******************************************************************
//** 文件名:	GFxWarHeroDetailedInfoPanel.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	林建聪
//** 日  期:	2015/9/16
//** 版  本:	1.0
//** 描  述:	游戏状态 GFx界面显示 英雄详细信息面板
//** 应  用:  
// *          对应Flash Layout文件：WarHeroDetailedInfoPanel.swf/gfx
//**************************** 修改记录 ******************************
//** 修改人: 陈言蒙
//** 日  期: 2015/09/28
//** 描  述: 拆分C#代码中与AS通讯的逻辑
//********************************************************************/
//using UnityEngine;
//using System;
//using System.Collections;
//using Scaleform;
//using ASpeedGame.Data.GameMobaSchemes;
//using System.Collections.Generic;

//partial class GFxWarHeroDetailedInfoPanel : SFMovieEx
//{
//    protected Value vInformationFrame;
//    protected Value vInformationPanel;

//    /// <summary>
//    /// 当前显示详细面板的实体ID
//    /// </summary>
//    private int CurrentShowEntityID;
//    /// <summary>
//    /// 缓存主英雄的信息，对应选择英雄可能需要主动询问逻辑层要属性数据
//    /// </summary>
//    private Dictionary<int, string> MainHeroPropertyDic = new Dictionary<int, string>();
//    /*临时主角血量*/
//    private int _curHeroHp, _maxHeroHp = 0;
//    /*临时的主角法力值*/
//    private int _curHeroMp, _maxHeroMp = 0;

//    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
//    {
//        base.Init(_parent, _sfMgr, _cp);

//    }

//    override public void Update()
//    {

//    }

//    public override void OnDestroy()
//    {
//        if(MainHeroPropertyDic!=null)
//        {
//            MainHeroPropertyDic.Clear();
//            MainHeroPropertyDic = null;
//        }
//        //PopupMovieManager.Instance.UnBindKeyCodeForMovie(this);

//        base.OnDestroy();
//    }

//    public override void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
//    {
//        if (!e.objev.IsHero || e.objev == null || !this.IsRegisterMovieSuccess)
//            return;

//        int nIndex = 0;//在配置表里的属性序号
//        string strPropertyValue = e.data.nValue.ToString();
//        switch(e.data.nPropID)
//        {
//            case (int)ENTITY_PROPERTY.PROPERTY_HP://血量
//                {
//                    _curHeroHp = e.data.nValue;
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_HP;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP://最大血量
//                {
//                    _maxHeroHp = e.data.nValue;
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_HP;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_HP_REPLY://血量回复
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_HpReply;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PA://物理攻击力
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PA;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PDP://物理防御穿透
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PDP;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PCT_PDRAINS://百分比物理吸血
//                {
//                    strPropertyValue = (e.data.nValue / 100).ToString();
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTPDrains;
//                }                   
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_AF://普攻系数
//                {
//                    strPropertyValue = ((e.data.nValue) / 10000.00).ToString("f2");
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_AF;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_CRC://暴击几率
//                {
//                    strPropertyValue = (e.data.nValue / 100).ToString();
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_CRC;
//                }
//                break ;
//            //case (int)ENTITY_PROPERTY.attackRange://目前无法获得攻击范围
//            //    break;
//            case (int)ENTITY_PROPERTY.PROPERTY_SD://移动速度
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_SD;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MP://法力
//                {
//                    _curHeroMp = e.data.nValue;
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MP;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP://最大法力值
//                {
//                    _maxHeroMp = e.data.nValue;
//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MP;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MP_REPLY://法力回复
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MpReply;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MA://法术强度
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MA;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MDP://法术防御穿透
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MDP;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PCT_MDRAINS://百分比法术吸血
//                {
//                    strPropertyValue = (e.data.nValue / 100).ToString();

//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTMDrains;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE://百分比冷却缩减
//                {
//                    strPropertyValue = (e.data.nValue / 100).ToString();

//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTCoolReduce;
//                }
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_PD://物理防御
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PD;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_MD://法术防御
//                nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MD;
//                break;
//            case (int)ENTITY_PROPERTY.PROPERTY_TENACITY://韧性
//                {
//                    strPropertyValue = (e.data.nValue / 100).ToString();

//                    nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Tenacity;
//                }
//                break;
//        }

//        SetMainHeroPropertyValue(nIndex, strPropertyValue);
//        if(bIsVisible)
//        {
//            SetPropertyValueToPropertyLabel(MainHeroPropertyDic);
//        }

//        //SetPropertyLabelValue(nIndex, strPropertyValue);
//    }

//    public override void EnterMovie()
//    {
//        base.EnterMovie();
//        //传属性值到SF显示,目前默认传主英雄属性
//        SetPropertyValueToPropertyLabel(MainHeroPropertyDic);


//        ShowInfoPanel(true);
//    }

//    public override void LeaveMovie()
//    {
//        base.LeaveMovie();
//        ShowInfoPanel(false);
//    }

//    public override void OnRegisterDocument(Value _root)
//    {
//        base.OnRegisterDocument(_root);
//        GetMemberWithCheck(MyRoot, "HeroDetailedInfoPanel", out vInformationPanel);

//        GetMemberWithCheck(vInformationPanel, "InformationFrame", out vInformationFrame);

//        //绑定按键
//        //PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.PropertyPannel), this);

//        InitPropertyInfo();
//    }

//    private void ShowInfoPanel(bool _bShow)
//    {
//        if (!Value.IsValueValid(vInformationPanel))
//            return;

//        if (vInformationPanel.Invoke("ShowInfoPanel", _bShow) == null)
//        {
//            Trace.LogError("call ShowInfoPanel failed");
//            return;
//        }

//    }
    
//    private void InitPropertyInfo()
//    {
//        int i = 0;
//        int max = (int)EMobaDetailedPropertyInfoID.EMDPI_Max;
//        string PropertyName, PropertyDes;
//        for(i= 0;i< max;i++)
//        {
//            PropertyName = GameMobaSchemesManager.Instance.GetDetailedPropertyName(i);
//            PropertyDes = GameMobaSchemesManager.Instance.GetDetailedPropertyDes(i);

//            SetPropertyLabelValue(i, "0");
//            SetPropertyLabelName(i, PropertyName);
//            SetPropertyLabelDes(i, PropertyDes);//读的时候已经转过UBB
//            //初始化主英雄属性表
//            MainHeroPropertyDic.Add(i, "0");
//        }
//    }

//    /// <summary>
//    /// 将当前属性存入主角临时属性表里，顺便处理要特殊显示的属性
//    /// </summary>
//    /// <param name="_nIndex">临时属性表里属性ID</param>
//    /// <param name="_strPropertyValue">当前属性值</param>
//    private void SetMainHeroPropertyValue(int _nIndex, string _strPropertyValue)
//    {
//        if (!MainHeroPropertyDic.ContainsKey(_nIndex))
//            return;

//        //特殊属性特殊处理...
//        if (_nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_HP)
//        {
//            _strPropertyValue = _curHeroHp + "/" + _maxHeroHp;
//        }
//        else if (_nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_MP)
//        {
//            _strPropertyValue = _curHeroMp + "/" + _maxHeroMp;
//        }
//        else if(_nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_PCTPDrains || 
//            _nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_CRC ||
//            _nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_PCTMDrains || 
//            _nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_PCTCoolReduce ||
//            _nIndex == (int)EMobaDetailedPropertyInfoID.EMDPI_Tenacity)
//        {
//            _strPropertyValue = _strPropertyValue + "%";
//        }
//        MainHeroPropertyDic[_nIndex] = _strPropertyValue;
//    }

//    /// <summary>
//    /// 当显示详细信息面板时才把属性传到SF那边
//    /// </summary>
//    /// <param name="_dic">当前要显示的属性表</param>
//    private void SetPropertyValueToPropertyLabel(Dictionary<int, string> _dic)
//    {
//        foreach(KeyValuePair<int,string> dicPair in _dic)
//        {
//            SetPropertyLabelValue(dicPair.Key, dicPair.Value);
//        }
//    }

//    #region AS -> CS
//    public void onInfoPanelHide()
//    {
//        PopupMovieManager.Instance.PopMovie(this);
//    }
//#endregion

//}
/*******************************************************************
** 文件名:	GFxTooltipHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/28
** 版  本:	1.0
** 描  述:	
 *  添加tips步骤:
 *  AS：
 *      1、tooltipHud.fla里添加新tips的布局，也可以代码添加，个人喜欢
 *      2、tooltipMgr.as 添加tips类型
 *      3、tooltipHud.as 添加新tips布局对象
 *      4、重新发布tooltipHud.fla
 *  CS:
 *      1、注册AS那边新添加的tips布局对象(OnRegisterTooltipHud())
 *      2、新建解析该新tips类型的方法(ParseDefaultTooltip())
 *      3、在ParseTooltipInformaton()里面添加新的解析类型
 *      
** 应  用:  对应Flash Layout文件：TooltipHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using U3D_Render.Common;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using GameLogic;



partial class GFxTooltipHud : SFMovieEx
{

    private static GFxTooltipHud _instance = null;
    private Dictionary<string, Vector2> _dicAspectRatioOffset = new Dictionary<string, Vector2>();
    private Vector2 _curAspRatioOffset;
    private int OldScreenWidth, OldScreenHeight;
    //viewport 宽高比
    private float AspectRatio_4_3, AspectRatio_16_9, AspectRatio_16_10;
    //边界偏移
    private int EdgeOffset = 10;
    //UBBProperty单实例
    private UBBProperty UBBPropertyInstance = UBBProperty.GetInstance();
    /// <summary>
    /// 当前Tooltip类型
    /// </summary>
    private string _curTooltipType;

    #region Tooltip AS 实例
    private Value DefaultTooltip = null;
    private Value WarSkillTooltip = null;
    private Value WarGoodsStoreTooltip = null;
    private Value WarPurchasedGoodsTooltip = null;
    private Value WarBuffTooltip = null;
    private Value Stage9Grid = null;//格子组
    private Value GameSummonerTooltip = null;
    private Value WarRecordTableTitleTip = null;
    private Value WarPlayerMatchTitleTip = null;// 天梯称号
    private Value RoomHeroDescTip = null;
    private Value CardNameTip = null;
    #endregion

    #region 在GFxTooltipHud.cs里和TooltipMgr.as里面的类型对应
    public const string TooltipType_Default = "TTT_Defalut";
    public const string TooltipType_WarSkill = "TTT_WarSkill";
    public const string TooltipType_WarGoodsStore = "TTT_WarGoodsStore";
    public const string TooltipType_WarPurchasedGoods = "TTT_WarPurchasedGoods";
    public const string TooltipType_WarBuff = "TTT_WarBuff";
    public const string TooltipType_WarTableEquip = "TooltipType_WarTableEquip";
    public const string TooltipType_WaitingRoomSummonerSkill = "TooltipType_GameSummonerSkill";
    public const string TllotipType_RecordTableTitle = "TllotipType_RecordTableTitle";
    public const string TooltipType_RecordTableMatchTitle = "TooltipType_RecordTableMatchTitle";
    public const string TooltipType_RoomHeroDesc = "TooltipType_RoomHeroDesc";
    public const string TooltipType_CardNameDesc = "TooltipType_CardNameDesc";
    #endregion


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        _instance = this;
        base.Init(_parent, _sfMgr, _cp);
        InitAspectRatio();
        if (UBBPropertyInstance == null)
        {
            Trace.LogError("获取UBBProperty单实例失败");
            return;
        }
        UBBPropertyInstance.PropertyUBB();
    }


    public override void OnDestroy()
    {
        _instance = null;
        base.OnDestroy();
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "DefaultTooltip", out DefaultTooltip);
        GetMemberWithCheck(MyRoot, "WarSkillTooltip", out WarSkillTooltip);
        GetMemberWithCheck(MyRoot, "WarGoodsStoreTooltip", out WarGoodsStoreTooltip);
        GetMemberWithCheck(MyRoot, "WarPurchasedGoodsTooltip", out WarPurchasedGoodsTooltip);
        GetMemberWithCheck(MyRoot, "WarBuffTooltip", out WarBuffTooltip);
        GetMemberWithCheck(MyRoot, "Stage9Grid", out Stage9Grid);
        GetMemberWithCheck(MyRoot, "WaitRoomSummonerTip", out GameSummonerTooltip);
        GetMemberWithCheck(MyRoot, "TitleTips", out WarRecordTableTitleTip);
        GetMemberWithCheck(MyRoot, "MatchTitleTips", out WarPlayerMatchTitleTip);
        GetMemberWithCheck(MyRoot, "RoomHeroDescTip", out RoomHeroDescTip);
        GetMemberWithCheck(MyRoot, "CardNameTips", out CardNameTip);
        
        
        this.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Base);
    }
    public static GFxTooltipHud Instance
    {
        get
        {
            return _instance;
        }
    }

    override public void Update()
    {

    }
    override public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
    {


        int id = e.data.nEntityID;
        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE:
                {



                }
                break;
        }
    }

    public override void Advance(float deltaTime)
    {
        base.Advance(deltaTime);
        UpdateAspectRatio();
    }

    /// <summary>
    /// 初始化各宽高比的偏移值
    /// </summary>
    private void InitAspectRatio()
    {
        AspectRatio_4_3 = GetPoint(((float)4 / (float)3), 3);
        AspectRatio_16_9 = GetPoint(((float)16 / (float)9), 3);
        AspectRatio_16_10 = GetPoint(((float)16 / (float)10), 3);
        //按比例调整偏移,目前发现好像不需要加偏移
        //且与editor无关
        //#if UNITY_EDITOR
        //        _dicAspectRatioOffset.Add("4:3", new Vector2(15, 115+24));
        //        _dicAspectRatioOffset.Add("16:9", new Vector2(15, 0+24));
        //        _dicAspectRatioOffset.Add("16:10", new Vector2(15, 35+24));
        //#else
        _dicAspectRatioOffset.Add("4:3", new Vector2(0, 5));
        _dicAspectRatioOffset.Add("16:9", new Vector2(0, 5));
        _dicAspectRatioOffset.Add("16:10", new Vector2(0, 5));
        //#endif
    }

    public void UpdateAspectRatio()
    {
        if (MyRoot == null)
        {
            return;
        }
        if (OldScreenHeight != Screen.height || OldScreenWidth != Screen.width)
        {
            //if (MyRoot.Invoke("updateAspectRatio") == null)
            //{
            //    Trace.LogError("call updateAspectRatio error");
            //    return;
            //}
            //Value ar = new Value();
            //if (!MyRoot.GetMember("_aspectRatio", ref ar))
            //{
            //    Trace.LogError("get _aspectRatio error");
            //    return;
            //}

            string arKey = "16:9";
            float ar = GetPoint(Initialize.mainCam.aspect, 3);
            if (ar == AspectRatio_4_3)
            {
                arKey = "4:3";
            }
            else if (ar == AspectRatio_16_9)
            {
                arKey = "16:9";
            }
            else if (ar == AspectRatio_16_10)
            {
                arKey = "16:10";
            }

            // Trace.LogError(arKey);

            if (_dicAspectRatioOffset.ContainsKey(arKey))
            {
                _curAspRatioOffset = this._dicAspectRatioOffset[arKey];
                // Trace.LogError("current _curAspRatioOffset =" + _curAspRatioOffset);
            }
            else
            {
                Trace.LogError("没有" + ar + "比例的偏移值，偏移设为0");
                _curAspRatioOffset = Vector2.zero;
            }
            OldScreenHeight = Screen.height;
            OldScreenWidth = Screen.width;
            //if (MyRoot.Invoke("calStage9Grid",OldScreenWidth,OldScreenHeight) == null)
            //{
            //    Trace.LogError("call calStage9Grid failed");
            //    return;
            //}
        }
    }

    private float GetPoint(float num, int bit)
    {
        return Mathf.Round(num * Mathf.Pow(10, bit)) / Mathf.Pow(10, bit);
    }

    private Vector2 GetMouseStagePostion()
    {
        Vector2 stagePos = Vector2.zero;
        Vector2 screenPos = SFGFxMovieManager.Instance.GetMousePos();

        stagePos = ScreenPointToStagePoint(screenPos);

        return stagePos;
    }

    /// <summary>
    /// 获取当前显示的tooltip类型
    /// </summary>
    public string CurrentTooltipType
    {
        get
        {
            return this._curTooltipType;
        }
    }

    private void CalTooltipPostion(ref double _x, ref double _y, double _w, double _h, Value _tooltip)
    {
        int GridIndex = GetTargetIn9Grid(_x, _y);
        Value GridRect = Stage9Grid.GetElement((uint)GridIndex);
        double TmpX = _x, TmpY = _y;
        double TipsH = _tooltip.GetMember("height");
        double TipsW = _tooltip.GetMember("width");

        double RX, ScreenWOffset;
        Vector3 RVecStage, RVecScreen;

        TmpY -= TipsH;//默认在Target上方
        switch (GridIndex)
        {
            case 0:
            case 1:
                if (TmpY < EdgeOffset)
                {
                    TmpY = _y + _h;//stage
                }
                break;
            case 2:
                RX = TmpX + TipsW;//stage coord
                RVecStage = new Vector3((float)RX, 0, 0);
                RVecScreen = StagePointToScreenPoint(RVecStage);
                ScreenWOffset = Screen.width - EdgeOffset;
                if (RVecScreen.x > ScreenWOffset)
                {
                    RVecScreen.x -= (float)ScreenWOffset;
                    RVecStage = ScreenPointToStagePoint(RVecScreen);
                    TmpX -= RVecStage.x;
                    //TmpY = _y;
                }
                //tips右边点
                if (TmpY < EdgeOffset)
                {
                    TmpY = _y + _h;
                }

                break;
            case 3:
            case 4:
                break;
            case 5:
                RX = TmpX + TipsW;//stage coord
                RVecStage = new Vector3((float)RX, 0, 0);
                RVecScreen = StagePointToScreenPoint(RVecStage);
                ScreenWOffset = Screen.width - EdgeOffset;
                if (RVecScreen.x > ScreenWOffset)
                {
                    RVecScreen.x -= (float)ScreenWOffset;
                    RVecStage = ScreenPointToStagePoint(RVecScreen);
                    TmpX -= RVecStage.x;
                    //TmpY = _y;
                }
                //tips右边点
                if (TmpY < EdgeOffset)
                {
                    TmpY = _y + _h;
                }
                break;
            case 6:
            case 7:
                break;
            case 8:

                RX = TmpX + TipsW;//stage coord
                RVecStage = new Vector3((float)RX, 0, 0);
                RVecScreen = StagePointToScreenPoint(RVecStage);
                ScreenWOffset = Screen.width - EdgeOffset;
                if (RVecScreen.x > ScreenWOffset)
                {
                    RVecScreen.x -= (float)ScreenWOffset;
                    RVecStage = ScreenPointToStagePoint(RVecScreen);
                    TmpX -= RVecStage.x;
                    //TmpY = _y;
                }
                break;
        }
        _x = TmpX;
        _y = TmpY;
    }


    #region AS -> C#


    #endregion

    #region 解析发来的Tooltip信息

    public void ParseTooltipInformaton(string _tooltipType, Value _tooltipElememts)
    {
        if (!_tooltipElememts.IsValid())
            return;

        if (_curTooltipType != "")
        {
            HideTooltipEx(_curTooltipType);
        }

        switch (_tooltipType)
        {
            case TooltipType_Default:
                ParseDefaultTooltip(_tooltipElememts);
                break;
            case TooltipType_WarSkill:
                ParseWarSkillTooltip(_tooltipElememts);
                break;
            case TooltipType_WarGoodsStore:
                ParseWarGoodsStoreTooltip(_tooltipElememts);
                break;
            case TooltipType_WarPurchasedGoods:
                ParseWarPurchasedGoodsTooltip(_tooltipElememts);
                break;
            case TooltipType_WarBuff:
                ParseWarBuffTooltip(_tooltipElememts);
                break;
            case TooltipType_WarTableEquip:
                ParseWarTableGoodsTooltip(_tooltipElememts);
                break;
            case TooltipType_WaitingRoomSummonerSkill:
                {
                    ParseWaitingRoomSummonerSkillTooltip(_tooltipElememts);
                }
                break;
            case TllotipType_RecordTableTitle:
                {
                    ParseWarTableTitleTooltip(_tooltipElememts);
                }
                break;
            case TooltipType_RecordTableMatchTitle:
                {
                    ParseWarTableMatchTitleTooltip(_tooltipElememts);
                }
                break;
            case TooltipType_RoomHeroDesc:
                {
                    ParseRoomHeroDescTooltip(_tooltipElememts);
                }
                break;
            case TooltipType_CardNameDesc:
                {
                    ParseCardDescTooltip(_tooltipElememts);
                }
                break;
            default:
                Trace.LogError("不存在Tooltip类型为" + _tooltipType + "的Tooltip，解析失败!!");
                return;
        }
        _curTooltipType = _tooltipType;
    }

    private bool ParseDefaultTooltip(Value _tooltipElememts)
    {
        bool res = true;

        double _x, _y = 0;
        double _w, _h = 0;

        string _defaultText;
        /*
         *  元素0：目标的x坐标
         *  元素1：目标的y坐标
         *  元素2：目标的宽
         *  元素3：目标的高
         *  元素4：目标DefaultText内容
         */
        if (_tooltipElememts.GetArraySize() > 0)
        {
            _x = (double)_tooltipElememts.GetElement(0);
            _y = (double)_tooltipElememts.GetElement(1);
            _w = (double)_tooltipElememts.GetElement(2);
            _h = (double)_tooltipElememts.GetElement(3);

            if (_tooltipElememts.GetElement(4) == null)
            {
                return false;
            }
            _defaultText = (string)_tooltipElememts.GetElement(4);
            //解析UBB
            UBB.toHtml(ref _defaultText);

            if (!Value.IsValueValid(DefaultTooltip))
            {
                Trace.LogError("解析Tooltip类型时，发现DefaultTooltip无实例对象");
                return false;
            }

            if (DefaultTooltip.Invoke("SetText", _defaultText) == null)
            {
                Trace.LogError("解析Tooltip类型时，执行DefaultTooltip 的SetText操作失败");
                return false;
            }
            //设置完内容后重新计算
            //_y -=  DefaultTooltip.GetMember("height");
            ////按比例调整偏移,目前发现好像不需要加偏移
            ////_x -= _curAspRatioOffset.x;
            //_y -= _curAspRatioOffset.y;
            CalTooltipPostion(ref _x, ref _y, _w, _h, DefaultTooltip);


            //让AS显示
            ShowTooltipEx(DefaultTooltip, _x, _y);
        }
        return res;
    }

    private bool ParseWarSkillTooltip(Value _tooltipElememts)
    {
        bool res = true;

        double _x, _y = 0;
        string _skillDes1;

        /*目前解析为测试用
         *  元素0：目标的x坐标
         *  元素1：目标的y坐标
         *  元素2：目标的宽（这里不需要）
         *  元素3：目标的高（这里不需要）
         *  元素4：目标DefaultText内容（这里不需要）
         *  
         *  元素5：目标技能第一项描述
         *  元素6：目标技能ID
         */
        if (_tooltipElememts.GetArraySize() > 0)
        {
            _x = (double)_tooltipElememts.GetElement(0);
            _y = (double)_tooltipElememts.GetElement(1);
            //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
            if (_tooltipElememts.GetElement(5) == null)
            {
                return false;
            }

            _skillDes1 = ((string)_tooltipElememts.GetElement(5)).TrimStart('$');

            //获得各个属性加成系数
            UBBPropertyInstance.getRatio(_skillDes1);

            //解析UBB
            UBB.toHtml(ref _skillDes1);

            //传数据到AS
            if (!Value.IsValueValid(WarSkillTooltip))
            {
                Trace.LogError("解析Tooltip类型时，发现WarSkillTooltip无实例对象");
                return false;
            }
            // function SetText(_strDes1:String):void
            if (WarSkillTooltip.Invoke("SetText", _skillDes1) == null)
            {
                Trace.LogError("解析Tooltip类型时，执行WarSkillTooltip 的SetText操作失败");
                return false;
            }

            double tipsHeight = (double)WarSkillTooltip.GetMember("height");
            double tipsWidth = (double)WarSkillTooltip.GetMember("width");

            GFxWarBottomCenterHud bottomCenterHud = SFGFxMovieManager.GetWindow<GFxWarBottomCenterHud>();

            if (SFGFxMovieManager.CheckTheMovieCanUse(bottomCenterHud))
            {
                double VisibleRectWidth = (double)MyRoot.GetMember("VisibleRectWidth");
                double VisibleRectX = (double)MyRoot.GetMember("VisibleRectX");
                _x = VisibleRectWidth / 2 - (float)tipsWidth / 2;
                _y = bottomCenterHud.GetBottomHudYPos - (float)tipsHeight;

            }
            else
            {
                Vector3 ScreenPos = Vector3.zero;
                Vector3 StagePos = Vector3.zero;

                ScreenPos.x = Screen.width / 4;
                ScreenPos.y = Screen.height * 2 / 5;
                StagePos = this.ScreenPointToStagePoint(ScreenPos);
                _x = StagePos.x;
                _y = StagePos.y;
            }


            //让AS显示
            ShowTooltipEx(WarSkillTooltip, _x, _y);

        }

        return res;
    }

    private bool ParseWarGoodsStoreTooltip(Value _tooltipElememts)
    {
        bool res = true;
        double _x, _y, _width, _height, _tooltipHeight = 0;
        int _nGoodsIconID, _nGoodsCost;
        string _strGoodsName, _strGoodsDes;

        /*目前解析为测试用
         *  元素0：目标的x坐标
         *  元素1：目标的y坐标
         *  元素2：目标的宽
         *  元素3：目标的高（这里不需要）
         *  元素4：目标DefaultText内容（这里不需要）
         *  
         *  元素5：目标物品图标ID
         *  元素6：目标物品名字
         *  元素7：目标物品价格
         *	元素8：目标物品描述
         */
        if (_tooltipElememts.GetArraySize() > 0)
        {
            _x = (double)_tooltipElememts.GetElement(0);
            _y = (double)_tooltipElememts.GetElement(1);
            _width = (double)_tooltipElememts.GetElement(2);
            _height = (double)_tooltipElememts.GetElement(3);
            //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
            if (_tooltipElememts.GetElement(5) == null)
            {
                return false;
            }

            _nGoodsIconID = (int)_tooltipElememts.GetElement(5);
            _strGoodsName = (string)_tooltipElememts.GetElement(6);
            _nGoodsCost = (int)_tooltipElememts.GetElement(7);
            _strGoodsDes = (string)_tooltipElememts.GetElement(8);

            UBB.toHtml(ref _strGoodsDes);

            //传数据到AS
            if (!Value.IsValueValid(WarGoodsStoreTooltip))
            {
                Trace.LogError("解析Tooltip类型时，发现WarGoodsStoreTooltip无实例对象");
                return false;
            }

            //SetText(_nGoodsIconID:int,_nGoodsCost:int,_strGoodsName:String,_strGoodsDes:String)
            if (WarGoodsStoreTooltip.Invoke("SetText", _nGoodsIconID, _nGoodsCost, _strGoodsName, _strGoodsDes) == null)
            {
                Trace.LogError("解析Tooltip类型时，执行WarSkillTooltip 的SetText操作失败");
                return false;
            }

            //Vector2 mouseStagePos = GetMouseStagePostion();
            //_x = mouseStagePos.x + 20;
            //_y = mouseStagePos.y - 20;
            _tooltipHeight = (double)WarGoodsStoreTooltip.GetMember("height");
            //_x = _x + _width + 5;
            //_y = _y - _tooltipHeight - 5;

            CalTooltipPostion(ref _x, ref _y, _width, _height, WarGoodsStoreTooltip);
            _x = _x + _width / 2;
            //让AS显示
            ShowTooltipEx(WarGoodsStoreTooltip, _x, _y);
        }

        return res;
    }

    private bool ParseWarPurchasedGoodsTooltip(Value _tooltipElememts)
    {
        bool res = true;
        double _x, _y = 0;
        int _nGoodsCost;
        string _strGoodsName, _strGoodsDes;


        _x = (double)_tooltipElememts.GetElement(0);
        _y = (double)_tooltipElememts.GetElement(1);
        //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
        if (_tooltipElememts.GetElement(5) == null)
        {
            return false;
        }
        _strGoodsName = (string)_tooltipElememts.GetElement(5);
        _nGoodsCost = (int)_tooltipElememts.GetElement(6);
        _strGoodsDes = (string)_tooltipElememts.GetElement(7);


        //解析UBB
        UBB.toHtml(ref _strGoodsDes);

        //传数据到AS
        if (!Value.IsValueValid(WarPurchasedGoodsTooltip))
        {
            Trace.LogError("解析Tooltip类型时，发现WarPurchasedGoodsTooltip无实例对象");
            return false;
        }
        // 		public function SetText(_nGoodsCost:int,_strGoodsName:String,_strGoodsDes:String)
        if (WarPurchasedGoodsTooltip.Invoke("SetText", _nGoodsCost, _strGoodsName, _strGoodsDes) == null)
        {
            Trace.LogError("解析Tooltip类型时，执行WarSkillTooltip 的SetText操作失败");
            return false;
        }

        double tipsHeight = (double)WarPurchasedGoodsTooltip.GetMember("height");
        double tipsWidth = (double)WarPurchasedGoodsTooltip.GetMember("width");

        GFxWarBottomCenterHud bottomCenterHud = SFGFxMovieManager.GetWindow<GFxWarBottomCenterHud>();

        if (SFGFxMovieManager.CheckTheMovieCanUse(bottomCenterHud))
        {
            double MovieDefWidth = (double)MyRoot.GetMember("VisibleRectWidth");
            _x = MovieDefWidth / 2;
            _y = bottomCenterHud.GetBottomHudYPos - (float)tipsHeight;

        }
        else
        {
            Vector3 ScreenPos = Vector3.zero;
            Vector3 StagePos = Vector3.zero;
            ScreenPos.x = Screen.width / 4;
            ScreenPos.y = Screen.height * 2 / 5;

            StagePos = this.ScreenPointToStagePoint(ScreenPos);
            _x = StagePos.x;
            _y = StagePos.y;
        }


        //让AS显示
        ShowTooltipEx(WarPurchasedGoodsTooltip, _x, _y);


        return res;
    }

    public bool ParseWarTableGoodsTooltip(Value _tooltipElememts)
    {
        double _x, _y = 0;
        int _nGoodsCost;
        string _strGoodsName, _strGoodsDes;
        if (_tooltipElememts.GetElement(5) == null)
        {
            return false;
        }

        _strGoodsName = (string)_tooltipElememts.GetElement(5);
        _nGoodsCost = (int)_tooltipElememts.GetElement(6);
        _strGoodsDes = (string)_tooltipElememts.GetElement(7);
        //解析UBB
        UBB.toHtml(ref _strGoodsDes);
        //传数据到AS
        if (!Value.IsValueValid(WarPurchasedGoodsTooltip))
        {
            Trace.LogError("解析Tooltip类型时，发现WarPurchasedGoodsTooltip无实例对象");
            return false;
        }
        // 		public function SetText(_nGoodsCost:int,_strGoodsName:String,_strGoodsDes:String)
        if (WarPurchasedGoodsTooltip.Invoke("SetText", _nGoodsCost, _strGoodsName, _strGoodsDes) == null)
        {
            Trace.LogError("解析Tooltip类型时，执行WarSkillTooltip 的SetText操作失败");
            return false;
        }
        double tipsHeight = (double)WarPurchasedGoodsTooltip.GetMember("height");
        double tipsWidth = (double)WarPurchasedGoodsTooltip.GetMember("width");



        return true;
    }

    private bool ParseWarBuffTooltip(Value _tooltipElememts)
    {
        double _x, _y = 0;
        string _strBuffName, _strBuffDes;


        _x = (double)_tooltipElememts.GetElement(0);
        _y = (double)_tooltipElememts.GetElement(1);
        //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
        if (_tooltipElememts.GetElement(5) == null)
        {
            return false;
        }

        _strBuffName = (string)_tooltipElememts.GetElement(5);
        _strBuffDes = (string)_tooltipElememts.GetElement(6);

        //获得各个属性加成系数
        UBBPropertyInstance.getRatio(_strBuffDes);

        //解析UBB
        UBB.toHtml(ref _strBuffDes);
        UBB.toHtml(ref _strBuffName);



        //传数据到AS
        if (!Value.IsValueValid(WarBuffTooltip))
        {
            Trace.LogError("解析Tooltip类型时，发现WarBuffTooltip无实例对象");
            return false;
        }
        // 		public function SetText(_strBuffName:String,_strBuffDes:String)
        if (WarBuffTooltip.Invoke("SetText", _strBuffName, _strBuffDes) == null)
        {
            Trace.LogError("解析Tooltip类型时，执行 WarBuffTooltip 的SetText操作失败");
            return false;
        }

        double tipsHeight = (double)WarBuffTooltip.GetMember("height");
        double tipsWidth = (double)WarBuffTooltip.GetMember("width");

        GFxWarBottomCenterHud bottomCenterHud = SFGFxMovieManager.GetWindow<GFxWarBottomCenterHud>();

        if (SFGFxMovieManager.CheckTheMovieCanUse(bottomCenterHud))
        {
            double MovieDefWidth = (double)MyRoot.GetMember("VisibleRectWidth");
            _x = MovieDefWidth / 2 - tipsWidth;
            _y = bottomCenterHud.GetBottomHudYPos - (float)tipsHeight;

        }
        else
        {
            Vector3 ScreenPos = Vector3.zero;
            Vector3 StagePos = Vector3.zero;

            ScreenPos.x = Screen.width / 4;
            ScreenPos.y = Screen.height * 2 / 5;
            StagePos = this.ScreenPointToStagePoint(ScreenPos);
            _x = StagePos.x;
            _y = StagePos.y;
        }


        //让AS显示
        ShowTooltipEx(WarBuffTooltip, _x, _y);

        return true;
    }

    private bool ParseWaitingRoomSummonerSkillTooltip(Value _tooltipElememts)
    {
        bool res = true;

        double _x, _y, _width, _height = 0;
        string skillDesc;
        /*目前解析为测试用
         *  元素5：目标的召唤师技能名字描述
         *  元素6：目标的召唤师技能描述
         */
        if (_tooltipElememts.GetArraySize() > 0)
        {
            _x = (double)_tooltipElememts.GetElement(0);
            _y = (double)_tooltipElememts.GetElement(1);
            _width = (double)_tooltipElememts.GetElement(2);
            _height = (double)_tooltipElememts.GetElement(3);
            //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
            if (_tooltipElememts.GetElement(5) == null)
            {
                return false;
            }

            skillDesc = ((string)_tooltipElememts.GetElement(5)).TrimStart('$');


            //解析UBB
            UBB.toHtml(ref skillDesc);

            //传数据到AS
            if (!Value.IsValueValid(GameSummonerTooltip))
            {
                Trace.LogError("解析Tooltip类型时，发现GameSummonerTooltip无实例对象");
                return false;
            }
            // function SetText(_strDes1:String):void
            if (GameSummonerTooltip.Invoke("SetText", skillDesc) == null)
            {
                Trace.LogError("解析Tooltip类型时，执行GameSummonerTooltip 的SetText操作失败");
                return false;
            }
            _x += _width * 3;
            _y += _height * 1.5;
            //让AS显示
            ShowTooltipEx(GameSummonerTooltip, _x, _y);

        }

        return res;
    }


    private bool ParseWarTableTitleTooltip(Value _tooltipElememts)
    {

        bool res = true;
        double _x, _y, _width, _height = 0;
        string stTitleName, stTitleDesc, stPicName;


        _x = (double)_tooltipElememts.GetElement(0);
        _y = (double)_tooltipElememts.GetElement(1);
        _width = (double)_tooltipElememts.GetElement(2);
        _height = (double)_tooltipElememts.GetElement(3);
        if (_tooltipElememts.GetElement(5) == null)
        {
            return false;
        }
        stTitleName = (string)_tooltipElememts.GetElement(5);
        stTitleDesc = (string)_tooltipElememts.GetElement(6);
        stPicName = (string)_tooltipElememts.GetElement(7);

        //解析UBB
        UBB.toHtml(ref stTitleName);
        UBB.toHtml(ref stTitleDesc);

        //传数据到AS
        if (!Value.IsValueValid(WarRecordTableTitleTip))
        {
            Trace.LogError("解析Tooltip类型时，发现WarRecordTableTitleTip无实例对象");
            return false;
        }
        if (WarRecordTableTitleTip.Invoke("SetText", stTitleName, stTitleDesc, stPicName) == null)
        {
            Trace.LogError("解析Tooltip类型时，执行WarRecordTableTitleTip 的SetText操作失败");
            return false;
        }

        _x += 80;
        _y += 20;

        ShowTooltipEx(WarRecordTableTitleTip, _x, _y);
        return res;
    }

     private bool  ParseRoomHeroDescTooltip(Value _tooltipElememts)
     {
         bool res = true;

         double _x, _y, _width, _height = 0;
         string skillDesc="";
         /*目前解析为测试用
          *  元素5：目标的召唤师技能名字描述
          *  元素6：目标的召唤师技能描述
          */
         if (_tooltipElememts.GetArraySize() > 0)
         {
             _x = (double)_tooltipElememts.GetElement(0);
             _y = (double)_tooltipElememts.GetElement(1);
             _width = (double)_tooltipElememts.GetElement(2);
             _height = (double)_tooltipElememts.GetElement(3);
             //Trace.LogWarning("_tooltipElememts.GetArraySize()=" + _tooltipElememts.GetArraySize());
             if (_tooltipElememts.GetElement(4) == null)
             {
                 return false;
             }

             skillDesc = ((string)_tooltipElememts.GetElement(4)).TrimStart('$');


             //解析UBB
             UBB.toHtml(ref skillDesc);

             //传数据到AS
             if (!Value.IsValueValid(RoomHeroDescTip))
             {
                 Trace.LogError("解析Tooltip类型时，发现ParseRoomHeroDescTooltip无实例对象");
                 return false;
             }
             // function SetText(_strDes1:String):void
             if (RoomHeroDescTip.Invoke("SetText", skillDesc) == null)
             {
                 Trace.LogError("解析Tooltip类型时，执行ParseRoomHeroDescTooltip 的SetText操作失败");
                 return false;
             }
             _x += _width * 1;
             _y += _height * 0.5;
             //让AS显示
             ShowTooltipEx(RoomHeroDescTip, _x, _y);

         }

         return res;
     }

     private bool ParseCardDescTooltip(Value _tooltipElememts)
     {
         bool res = true;

         double _x, _y, _width, _height = 0;
         string CardDesc;

         if (_tooltipElememts.GetArraySize() > 0)
         {
             _x = (double)_tooltipElememts.GetElement(0);
             _y = (double)_tooltipElememts.GetElement(1);
             _width = (double)_tooltipElememts.GetElement(2);
             _height = (double)_tooltipElememts.GetElement(3);

             if (_tooltipElememts.GetElement(5) == null)
             {
                 return false;
             }

             CardDesc = ((string)_tooltipElememts.GetElement(5)).TrimStart('$');


             //解析UBB
             UBB.toHtml(ref CardDesc);

             //传数据到AS
             if (!Value.IsValueValid(CardNameTip))
             {
                 Trace.LogError("解析Tooltip类型时，发现GameCardNameTooltip无实例对象");
                 return false;
             }
             // function SetText(_strDes1:String):void
             if (CardNameTip.Invoke("SetText", CardDesc) == null)
             {
                 Trace.LogError("解析Tooltip类型时，执行GameCardNameTooltip 的SetText操作失败");
                 return false;
             }
 
             _x += _width;
             _y += _height;

             ShowTooltipEx(CardNameTip, _x, _y);

         }

         return res;
     }

    private bool ParseWarTableMatchTitleTooltip(Value _tooltipElememts)
    {
        bool res = true;
        double _x, _y, _width, _height = 0;
        string stTitleName;
        int nTitleScore;

        _x = (double)_tooltipElememts.GetElement(0);
        _y = (double)_tooltipElememts.GetElement(1);
        _width = (double)_tooltipElememts.GetElement(2);
        _height = (double)_tooltipElememts.GetElement(3);

        if (_tooltipElememts.GetElement(5) == null)
        {
            return false;
        }
        stTitleName = (string)_tooltipElememts.GetElement(5);
        nTitleScore = (int)_tooltipElememts.GetElement(6);

        //解析UBB
        UBB.toHtml(ref stTitleName);

        //传数据到AS
        if (!Value.IsValueValid(WarPlayerMatchTitleTip))
        {
            Trace.LogError("解析Tooltip类型时，发现WarPlayerMatchTitleTip无实例对象");
            return false;
        }
        if (WarPlayerMatchTitleTip.Invoke("SetText", stTitleName, nTitleScore) == null)
        {
            Trace.LogError("解析Tooltip类型时，执行WarPlayerMatchTitleTip 的SetText操作失败");
            return false;
        }

        _x += 80;
        _y += 20;

        ShowTooltipEx(WarPlayerMatchTitleTip, _x, _y);


        return res;
    }


    private float RescaleValueWithOffsetEx(float value, float scale, float offset)
    {
        // remove the offset
        value -= offset * 0.5f;

        // normalize value
        value /= offset;

        // scale up to the targetScale
        value *= scale;

        // restore the offset
        value += offset * 0.5f;

        return value;
    }

    #endregion

}
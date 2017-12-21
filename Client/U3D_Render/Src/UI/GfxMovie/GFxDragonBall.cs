///*******************************************************************
//** 文件名:	GFxDragonBall.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	林飞
//** 日  期:	2015/2/27
//** 版  本:	1.0
//** 描  述:	游戏状态 GFx界面显示 html信息显示
//** 应  用:  
// *          对应Flash Layout文件：GFxDragonBall.swf/gfx
//**************************** 修改记录 ******************************
//** 修改人: 林建聪
//** 日  期: 2015.3.17
//** 描  述: 
// *         1、更改父类为SFMovieEx
// *         2、在获取AS对象时添加检测
//********************************************************************/
//using UnityEngine;
//using System;
//using System.Collections;
//using Scaleform;
//using GameLogic;
//using System.Collections.Generic;
//using System.Runtime.InteropServices;


//class GFxDragonBall : SFMovieEx
//{
//    /////////////////////////////////////////////////////////////////基本属性
//    public int entityID = 0;
//    private bool open = false;
//    /////////////////////////////////////////////////////////////////
//    private string m_strRoleStartName = "taitan1001";
//    private string m_strRoleEndName   = "taitan2000";

//    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
//    {
//        base.Init(_parent, _sfMgr, _cp);

        
//        //this.bIsFloat = true;
//    }

//    // 是否禁止使用龙珠功能
//    private bool IsForbitBall()
//    {
//        try
//        {
//            // 硬编码定死策划数值账号段
//            U3D_Render.EntityView ev = EntityFactory.MainHeroView;
//            if (ev != null && ev.Property != null)
//            {
//                string strName = Config.AccountName;
//                // 外部及策划数值测试账号，禁止使用七龙珠
//                if (strName.ToLower().StartsWith("g") || string.Compare(strName, m_strRoleStartName, true) >= 0 && string.Compare(strName, m_strRoleEndName, true) <= 0)
//                {
//                    return true;
//                }
//            }
//        }
//        catch
//        {
//        }
//        return false;
//    }
    
//    override public void Update()
//    {
//        if (InputManager.GetKeyDown(KeyCode.F10))
//        {
//            if (IsForbitBall())
//            {
//                return;
//            }

//            open = !open;
//            //SFGFxMovieManager.Instance.UseUIMouseCtrl();
//            if (open)
//                EnterMovie();
//            else
//                LeaveMovie();
//            //MyRoot.Invoke("ShowOrHideDragonBallButton");
//        }

//        if(FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.KillSelf))//if (InputManager.GetKeyDown(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.KillSelf)))
//        {
//            string m_param = "SupperStone_Suicide()";
//            OnLinkClick(m_param);
//        }

//    }

//    public override void EnterMovie()
//    {
//        this.SetDepth((int)(SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Loading));
//        this.SetFocus(true);
//      //  InputManager.CanControlHero = false;
//        base.EnterMovie();
//        MyRoot.Invoke("ShowOrHideDragonBallButton");
//    }
//    public override void LeaveMovie()
//    {
//        this.SetFocus(false);
//    //    InputManager.CanControlHero = true;
//        base.LeaveMovie();
//        MyRoot.Invoke("ShowOrHideDragonBallButton");
//    }
//    public override void OnDestroy()
//    {
//        PopupMovieManager.Instance.UnBindKeyCodeForMovie(this);
//        LeaveMovie();
//        base.OnDestroy();
//    }

//    public override void OnRegisterDocument(Value _root)
//    {
//        base.OnRegisterDocument(_root);
//        //PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.DragonBall), this);

//    }

//    public void OnLinkClick(string strParam)
//    {        
//        IntPtrHelper helper = new IntPtrHelper();
//        IntPtr strParam_ptr = helper.Str2Ptr(strParam);
//        int eventID = (int)EntityLogicDef.ENTITY_CMD_DRAGONBALL;
//        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, eventID, 0, strParam, strParam_ptr, GameLogicAPI.getStringLen(strParam_ptr));
//    }
//    //能够获取到参数了 那么明天的事就好解决了
//    public void SupperStone_SetSpeed()
//    {
//        Value BG = MyRoot.GetMember("BG");
//        if (BG.IsValid())
//        {
//            Value textValue = MyRoot.GetMember("FirstTextContext");
//            if (textValue.IsValid())
//            {
//                Trace.Log(textValue);
//                 Value txt=new Value();
//                 textValue.GetText(ref txt);
//                string tt ="获取的文本信息"+txt;
//                Trace.Log(tt);
//            }
//            else
//                Trace.Log("textValue null");
//        }
//    }

//    public void SupperStone_Showposition(Vector3 u_Position, Vector3 s_Position)
//    {
//        string cpos_xyz, spos_xyz;
//        cpos_xyz = "X:" +  u_Position.x.ToString()+ ", Y:" +  u_Position.y.ToString() + ", Z:" + u_Position.z.ToString();
//        spos_xyz = "X:" +  s_Position.x.ToString()+ ", Y:" +  s_Position.y.ToString() + ", Z:" + s_Position.z.ToString();
//        MyRoot.Invoke("ShowPosition", cpos_xyz, spos_xyz);
//    }

//    public void SupperStone_SetBindMoney()
//    {
			
//    }
//    public void SupperStone_Retrieve()
//    {
			
//    }
//    public void SupperStone_CallCustomMonster()
//    {
			
//    }
//    public void SupperStone_SetPower()
//    {
			
//    }
//    public void SupperStone_SetExperience()
//    {
			
//    }

//    public void SupperStone_SetMoney()
//    {
			
//    }

//    public void SupperStone_GetItem()
//    {
			
//    }

//    public void SupperStone_SetLevel()
//    {
			
//    }

//    public void SupperStone_SetTick()
//    {
			
//    }

//    public void SupperStone_CreateKin()
//    {
//        //创建家族
       
//    }

//    public void SupperStone_JoinKin()
//    {
//        //申请加入家族

//    }

//    public void SupperStone_QuitKin()
//    {

//    }

//    public void SupperStone_DismissKin()
//    {
  
//    }

//    public void SupperStone_CreateClan()
//    {
//        //创建帮会
//    }
//    public void SupperStone_JoinClan()
//    {
//        //加入帮会
//    }
//    public void SupperStone_QuitClan()
//    {
//        //退出帮会
//    }
//    public void SupperStone_DismissClan()
//    {
//        //解散帮会
//    }
//}

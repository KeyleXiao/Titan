/*******************************************************************
** 文件名:	GFxLoginMenu.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2014/11/11
** 版  本:	1.0
** 描  述:	GFx 登录界面逻辑类
				其余的登录界面逻辑类都用GFx开头区分
** 应  用:  	对应Flash Layout文件：LoginWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2014/12.11
** 描  述: 合并Bootcamp项目，增加一个登录后显示游戏界面
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;
using System.Xml;
using ASpeedGame.Data.GameSettingsXml;

public class GFxLoginMenu : SFMovieEx
{
    /// <summary>
    /// 界面选择的英雄类型
    /// </summary>
    private int HeroType = 1;

    protected Value vLoginFrame;


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

        this.SetFocus(true);
        //  SetViewport(0, 0, 500, 600);
    }

    override public void Update()
    {
        if (!IsRegisterMovieSuccess)
            return;


    }
    public override void OnDestroy()
    {
        //使用登录背景特效时开启
        //DisAttachBackgroundMovie();
        base.OnDestroy();


    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);

        SetMovieVisibleOnCreate();

        Value LoginPanel;
        GetMemberWithCheck(MyRoot, "loginPanel",out LoginPanel);

        GetMemberWithCheck(LoginPanel, "LoginFrame", out vLoginFrame);


        // 将配置文件中获取的用户名集合以数组的形式传递给AS
        Value userNameArray = this.CreateArray();
        string[] userNameArrayStr =
            GameSettingsXmlManager.Instance.GameSettingsModel.loginedUserName.AValue.Split(';');
        //用户名数组下表,用于传递给AS
        uint userNameCount = 0;
        uint userNameArrayStrVolumn = Convert.ToUInt32(userNameArrayStr.Length);
        userNameArray.SetArraySize(userNameArrayStrVolumn);

        foreach (string singleUserName in userNameArrayStr)
        {
            // 保证传递给AS后，下拉菜单中选项以最近登陆使用的顺序排列用户名
            userNameArray.SetElement(userNameArrayStrVolumn - userNameCount - 1, singleUserName);
            userNameCount += 1;
        }

        if (vLoginFrame.Invoke("SetDropDownUserName", userNameArray, userNameCount + 1) == null)
        {
            Trace.LogError("call SetDropDownUserName failed");
        }

    }

    public void LoginBtnClick(string UserName, string Password,string AdDressIP)
    {
        // 现在是IP地址
        gameview_login data = new gameview_login();
        if (GameSettingsXmlManager.Instance.GameSettingsModel.serverIPAddress.AValue != "")
        {
            data.svrip = GameSettingsXmlManager.Instance.GameSettingsModel.serverIPAddress.AValue;
        }
        else
            data.svrip = AdDressIP;

        data.account = UserName;
        data.password = Password;

        // 首先判断该登陆用户名是否存在于配置文件中，如果不存在则进行添加
        System.Text.StringBuilder loginedUserName_str = new System.Text.StringBuilder();
        if (!GameSettingsXmlManager.Instance.GameSettingsModel.loginedUserName.AValue.Contains(UserName))
        {
            int userNameCount = 0;
            int userNameExistCount = 0;
            string[] userNameArrayStr;

            if (GameSettingsXmlManager.Instance.GameSettingsModel.loginedUserName.AValue.Length != 0)
            {
                
                userNameArrayStr = 
                    GameSettingsXmlManager.Instance.GameSettingsModel.loginedUserName.AValue.Split(';');
                userNameExistCount = userNameArrayStr.Length;

                foreach (string singleUserName in userNameArrayStr)
                {
                    userNameCount += 1;
                    // 保证传递给AS后，下拉菜单中选项以最近登陆使用的顺序排列用户名
                    if (userNameCount == 1 && userNameExistCount > 10)
                    {
                        // 跳过最久最近(LRU算法)未使用的那个账号
                        continue;
                    }
                    else if (userNameCount == 11) 
                    {
                        // 限定记住用户名的个数为10个
                        break;
                    }
                    else if (singleUserName != "")
                    {
                        loginedUserName_str.Append(singleUserName);
                        loginedUserName_str.Append(';');
                    }
                }
            }

            loginedUserName_str.Append(UserName);
            //loginedUserName_str.Append(';');
            // 保存用户名信息
            GameSettingsXmlManager.Instance.GameSettingsModel.loginedUserName.ValidateSetting(
                loginedUserName_str.ToString());
            // 保存用户名信息到XML配置文件中去
            GameSettingsXmlManager.Instance.SaveGameSettings();
        }

        ViewEventHelper.Instance.SendCommand<gameview_login>(GameLogicDef.GVIEWCMD_USER_LOGIN, ref data);
    }

    public void HeroSelect(double _HeroType)
    {
        HeroType = (int)_HeroType;
    }

    /// <summary>
    /// destroy 时移除背景动画
    /// </summary>
    private void DisAttachBackgroundMovie()
    {
        if (MyRoot != null  && MyRoot.IsValid())
        {
            if (MyRoot.Invoke("DisAttachBackgroundMovie") == null)
            {
                Trace.LogError("call DisAttachBackgroundMovie failed");
                return;
            }
        }
    }
}

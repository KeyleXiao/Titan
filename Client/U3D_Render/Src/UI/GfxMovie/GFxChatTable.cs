using UnityEngine;
using System;
using System.Collections.Generic;
using Scaleform;
using System.Collections;
using GameLogic;
using U3D_Render;
using System.Runtime.InteropServices;

partial class GFxChatTable : SFMovieEx
{

    //记录当前战绩表的显示状态，不用每次都取AS的值来显示
    private bool IsWarRecordTableVisible = false;
    private Value _vMainChatTable;

    public override void EnterMovie()
    {
        showMovie();
    }

    public override void LeaveMovie()
    {
        showMovie();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "MainChatTable", out _vMainChatTable);
        PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.EnterLeft), this);
        PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.EnterRight), this);
        SetMovieVisibleOnCreate();
        this.SetMovieVisible(true);
    }

    public void showMovie()
    {
        if (IsWarRecordTableVisible == false)
        {
            this.SetFocus(true);
            base.EnterMovie();
            this.SetMovieVisible(true);

            if (!Value.IsValueValid(_vMainChatTable))
            {
                return;
            }
            if (_vMainChatTable.Invoke("CheckVisibleToOpen") == null)
            {
                Trace.LogError("call CheckVisibleToOpen failed");
                return;
            }
           
            IsWarRecordTableVisible = true;
        }
        else
        {
            base.LeaveMovie();
//             this.SetMovieVisible(false);
//             InputManager.enabled = true;


            if (!Value.IsValueValid(_vMainChatTable))
            {
                return;
            }
            if (_vMainChatTable.Invoke("onEnterEnter") == null)
            {
                Trace.LogError("call onEnterEnter failed");
                return;
            }
            if (_vMainChatTable.Invoke("CheckTimerBegin") == null)
            {
                Trace.LogError("call CheckTimerBegin failed");
                return;
            }
            
            this.SetFocus(false);
            IsWarRecordTableVisible = false;
        }
        
    }

    #region AS->C#
    // 将聊天信息发送给逻辑层
    public void OnGetChatInfoAndSend( String stInfo )
    {
        GameUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_TEAMMATE, stInfo);
    }

    #endregion

}

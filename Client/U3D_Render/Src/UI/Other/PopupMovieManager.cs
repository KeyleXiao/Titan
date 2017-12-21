/*******************************************************************
** 文件名:	PopupMovieManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/9/2
** 版  本:	1.0
** 描  述:	游戏弹出类界面打开/关闭管理
** 应  用:  目前主要有两个方法，一个是通过按键绑定界面(BindKeyCodeForMovie/UnBindKeyCodeForMovie)，一个是对界面进行压栈出栈(PushMovie/PopMovie)
            要注意那些关闭界面的途径，其他途径关闭界面调用下popmovie
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PopupMovieManager : Singleton<PopupMovieManager>
{
    /// <summary>
    /// 按键与界面绑定,一个按键只与一个movie绑定（应该很正常吧？）界面移除的时候，绑定也会移除
    /// </summary>
    private Hashtable BindKeyCodeForMovieTable = new Hashtable();
    /// <summary>
    /// Popup movie list模拟栈操作
    /// </summary>
    private List<SFMovieEx> PopupMovieStack = new List<SFMovieEx>();


    /// <summary>
    /// 按键与界面绑定，一个按键只与一个movie绑定（应该很正常吧？）界面移除的时候，绑定也会移除
    /// </summary>
    /// <param name="_targetKeyCode">要绑定的目标按键</param>
    /// <param name="_targetMovie">按键绑定的movie</param>
    public void BindKeyCodeForMovie(KeyCode _targetKeyCode, SFMovieEx _targetMovie)
    {
        if (BindKeyCodeForMovieTable.ContainsKey(_targetKeyCode))
        {
            Trace.LogWarning("KeyCode:" + _targetKeyCode + "is bind a movie,use other");
            return;
        }

        BindKeyCodeForMovieTable.Add(_targetKeyCode, _targetMovie);

    }

    /// <summary>
    /// 解除按键与movie的绑定信息
    /// </summary>
    /// <param name="_targetMovie"></param>
    public void UnBindKeyCodeForMovie(SFMovieEx _targetMovie)
    {
        if (!BindKeyCodeForMovieTable.ContainsValue(_targetMovie))
        {
            return;
        }

        KeyCode code = KeyCode.None;
        foreach (DictionaryEntry keypair in BindKeyCodeForMovieTable)
        {
            if ((keypair.Value as SFMovieEx) == _targetMovie)
            {
                BindKeyCodeForMovieTable.Remove(keypair.Key);
                break;
            }
        }
    }

    public void Updata()
    {
        KeyCode nEscapeKeyCode = FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.SystemPannel);
        //不要ESC界面的时候这里也记得关掉
		if ((!BindKeyCodeForMovieTable.ContainsKey(nEscapeKeyCode)) && PopupMovieStack.Count > 0)
        {
            PopMovie();
        }


        foreach (DictionaryEntry keypair in BindKeyCodeForMovieTable)
        {
            KeyCode key = (KeyCode)keypair.Key;
            SFMovieEx movie = (SFMovieEx)keypair.Value;

            if (InputManager.GetKeyUp(key))
            {
                if (key == nEscapeKeyCode && PopupMovieStack.Count > 0)
                {
                    PopMovie();
                    continue;
                }

                if (!IsMovieAllowed(movie))
                {
                    PushMovie(movie);
                }
                else
                {
                    PopMovie(movie);
                }
            }
        }
    }

    public void Destroy()
    {
        BindKeyCodeForMovieTable.Clear();
        PopupMovieStack.Clear();
    }

    public void PushMovie(SFMovieEx _targetMovie)
    {
        if (_targetMovie == null)
            return;

        if (!IsMovieAllowed(_targetMovie))
        {
            _targetMovie.EnterMovie();
            PopupMovieStack.Add(_targetMovie);
        }
        int nNewDepth = (int)(SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer) + PopupMovieStack.Count + 1;
        if (_targetMovie.Depth != nNewDepth)
            _targetMovie.SetDepth(nNewDepth);

    }

    /// <summary>
    /// 移除movie
    /// </summary>
    /// <param name="_targetMovie">指定要移除的mv，如果为null则移除栈顶的mv</param>
    /// <returns>返回当前要移出栈的mv</returns>
    public SFMovieEx PopMovie(SFMovieEx _targetMovie = null)
    {
        if (PopupMovieStack.Count < 0)
            return null;

        SFMovieEx popMV = null;

        if (_targetMovie == null)
        {
            popMV = PopupMovieStack[PopupMovieStack.Count - 1];
        }
        else
        {
            if (!PopupMovieStack.Contains(_targetMovie))
                return null;

            int nTargetIndex = PopupMovieStack.IndexOf(_targetMovie);

            popMV = PopupMovieStack[nTargetIndex];

        }
        if (popMV == null)
        {
            return null;
        }
        popMV.LeaveMovie();
        int nNewDepth = (int)(SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Default);
        if (popMV.Depth != nNewDepth)
            popMV.SetDepth(nNewDepth);

        PopupMovieStack.Remove(popMV);

        return popMV;
    }

    /// <summary>
    /// 获取当前栈顶的mv，但从栈内移除
    /// </summary>
    /// <returns>当前栈顶的mv</returns>
    public SFMovieEx GetTopMovie()
    {
        if (PopupMovieStack.Count <= 0)
            return null;

        return PopupMovieStack[PopupMovieStack.Count - 1];
    }

    public void InsertMovie(SFMovieEx _insertSource,SFMovieEx _insertTarget)
    {
        if(IsMovieAllowed(_insertTarget))
        {
            int sourceIndex = PopupMovieStack.IndexOf(_insertSource);
            int targetIndex = PopupMovieStack.IndexOf(_insertTarget);
            int tmpDepth = _insertSource.Depth;

            _insertSource.SetDepth(_insertTarget.Depth);
            _insertTarget.SetDepth(tmpDepth);

            PopupMovieStack[sourceIndex] = _insertTarget;
            PopupMovieStack[targetIndex] = _insertSource;
        }
        else
        {
            PushMovie(_insertTarget);
        }
    }
    /// <summary>
    /// 判断PopupMovieStack里是否已经有_targetMovie
    /// </summary>
    /// <param name="_targetMovie">要检测的movie</param>
    /// <returns></returns>
    private bool IsMovieAllowed(SFMovieEx _targetMovie)
    {
        if (PopupMovieStack.Contains(_targetMovie))
        {
            return true;
        }

        return false;
    }
}
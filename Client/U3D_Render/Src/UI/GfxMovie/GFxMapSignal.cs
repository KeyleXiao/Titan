using System;
using System.Collections.Generic;
using Scaleform;
using U3D_Render.Common;

public class GFxMapSignal : SFMovieEx
{
    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
    }

    #region MapSignal
    public void ShowMapSignal(bool bShow)
    {
        if (MyRoot == null || !MyRoot.IsValid())
            return;

        if (bShow)
        {
            PopupMovieManager.Instance.PushMovie(this);
            MyRoot.Invoke("SetMapSignalVisible", 1);
        }
        else
        {
            MyRoot.Invoke("SetMapSignalVisible", 0);
            PopupMovieManager.Instance.PopMovie(this);
        }
    }

    public void SetMapSignalType(int signalType)
    {
        if (MyRoot == null || !MyRoot.IsValid())
            return;

        MyRoot.Invoke("SetMapSignalType", signalType);
    }

    public void UpdateMouseLine(float rotation)
    {
        if (MyRoot == null || !MyRoot.IsValid())
            return;

        MyRoot.Invoke("UpdateMouseLine", rotation);
    }

    #endregion
}
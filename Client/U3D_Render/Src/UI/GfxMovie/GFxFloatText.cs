using System;
using System.Collections.Generic;
using Scaleform;
using U3D_Render.Common;

public class GFxFloatText : SFMovieEx
{
    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);
    }

    public override bool AcceptMouseEvents(int icase)
    {
        base.AcceptMouseEvents(icase);
        return false;
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_Base);
    }

    #region FloatText
    /// 外部调用UI上系统飘字信息
    public void onBroadFloatTextSystem(string stTextInfo = "", int nPosType = 0, int nTime = 0)
    {
        if (MyRoot != null && MyRoot.IsValid())
        {
            string stInfo = stTextInfo;
            UBB.toHtml(ref stInfo);
            if (null == MyRoot.Invoke("onBroadFloatTextSystem", stInfo, nPosType, nTime))
            {
                Trace.LogError("onBroadFloatTextSystem invoke == NULL stInfo = " + stInfo + ", nPosType = " + nPosType + "nTime = " + nTime);
            }
        }
    }

    #endregion
}
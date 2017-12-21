/*******************************************************************
** 文件名:	DynamicMovie.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/26
** 版  本:	1.0
** 描  述:	动态Movie类
** 应  用:  	这只是一个容器类，没有任何的逻辑，所有动态创建的东西全部扔到这个swf文件中去。以后可能会有更好的方案

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.3.17
** 描  述: 
 *         1、更改父类为SFMovieEx
 *         2、在获取AS对象时添加检测
********************************************************************/

using UnityEngine;
using System.Collections;
using Scaleform;

public class GfxDynamicMovie : SFMovieEx {

    public static Value Instance
    {
        get
        {
            if (!Value.IsValueValid(m_Instance))
                return null;
            return m_Instance;
        }
    }

    public static long ActiveMovieID = 0;

    private static Value m_Instance = null;


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);
    }

    public override void OnDestroy()
    {
        GfxLableManager.DestroyALL();
        m_Instance = null;
        base.OnDestroy();
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);

        m_Instance = _root;

        ActiveMovieID = GetID();
    }

}

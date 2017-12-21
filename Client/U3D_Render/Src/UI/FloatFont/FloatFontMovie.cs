/*******************************************************************
** 文件名:	FloatFontMovie.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/28
** 版  本:	1.0
** 描  述:	这个movie里创建的是飘字
** 应  用:  	movie里全部是飘字，没有其他

**************************** 修改记录 *******************************/
using UnityEngine;
using System.Collections;
using Scaleform;

public class FloatFontMovie : SFMovieEx
{
    //public const int FloatFontRenderTextureWidth = 1024;
    //public const int FloatFontRenderTextureHeight = 1024;

    //public const float FloatFontMovieWidth = 1024;
    //public const float FloatFontMovieHeight = 1024;

    //public static RenderTexture FloatFontRenderTexture
    //{
    //    get
    //    {
    //        if (m_RenderTexture == null)
    //        {
    //            m_RenderTexture = new RenderTexture(FloatFontRenderTextureWidth, FloatFontRenderTextureHeight, 0);
    //            m_RenderTexture.filterMode = FilterMode.Point;
    //            m_RenderTexture.Create();
    //        }
    //        return m_RenderTexture;
    //    }
    //}

    //public static Vector3 RenderTexureScale = new Vector3(FloatFontMovieHeight / FloatFontRenderTextureWidth, FloatFontMovieHeight / FloatFontRenderTextureHeight, 1.0f);

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

    //private static RenderTexture m_RenderTexture = null;


    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);

        SFGFxMovieManager.Instance.AddMovieToIgnoreHitTestList(this);
    }

    override public void OnDestroy()
    {
        m_Instance = null;
        base.OnDestroy();
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        m_Instance = _root;
       
        ActiveMovieID = GetID();
    }

    ///// <summary>
    ///// 变换纹理坐标
    ///// </summary>
    ///// <param name="pixelOffset">像素坐标</param>
    ///// <returns></returns>
    //public static Vector2 TransformTextureOffsetFromPixelPos(Vector2 pixelOffset)
    //{
    //    Vector2 off = pixelOffset;
    //    off.x = off.x / FloatFontRenderTextureWidth;
    //    off.y = off.y / FloatFontRenderTextureHeight;

    //    off.x = off.x / RenderTexureScale.x;
    //    off.y = off.y / RenderTexureScale.y;

    //    return off;
    //}

    ///// <summary>
    ///// 变换公告板
    ///// </summary>
    ///// <param name="b"></param>
    //public static void TransformBillBorad(BillBoard b)
    //{
    //    if (null == b)
    //    {
    //        return;
    //    }
    //    Vector3 scal = Vector3.one;
    //    scal.y *= -1;
    //    scal.x = scal.x * RenderTexureScale.x;
    //    scal.y = scal.y * RenderTexureScale.y;

    //    b.StandradScale = scal;
    //    b.transform.localScale = scal;
    //}

}


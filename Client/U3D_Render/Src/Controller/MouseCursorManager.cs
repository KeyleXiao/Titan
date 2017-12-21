/*******************************************************************
** 文件名:	MouseCursorManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/1/14
** 版  本:	1.0
** 描  述:	鼠标光标管理类
** 应  用:  	
            需要增加光标类型的话
            1、EMouseCursorType里增加光标类型
            2、loadMouseCursorTexture()里给新类型绑定资源
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  8/16/2016
** 描  述:  增加PK选人状态光标类型
********************************************************************/
using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// 光标类型
/// </summary>
public enum EMouseCursorType
{
    MCT_None = 0,               // 无设定，按unity自身鼠标
    MCT_Normal = 1,             // 普通鼠标形状
    MCT_OnSnap = 2,             // 选中敌人时的形状
    MCT_NormalMode2 = 3,        // mode2下普通鼠标形状
    MCT_OnSnapMode2 = 4,        // mode2下选中敌人时的形状
    MCT_MouseAttack = 5,        // 标记攻击按钮时
    MCT_MouseSign = 6,          // 标记信号按钮式
    MCT_PkMode = 7,             // PK模式
}

public struct FCursorInfo
{
    public Texture2D CursorTexture;
    public Vector2 TexHotspot;

    public FCursorInfo(Texture2D _texCursor,Vector2 _vec2Hopspot)
    {
        CursorTexture = _texCursor;
        TexHotspot = _vec2Hopspot;
    }
}
public class MouseCursorManager : Singleton<MouseCursorManager>
{
    private EMouseCursorType m_currentMouseCursorType = EMouseCursorType.MCT_None;
    public EMouseCursorType MouseCursorType
    {
        get
        {
            return m_currentMouseCursorType;
        }
        set
        {
            if(m_currentMouseCursorType != value)
            {
                if(dic_CursorInfo.ContainsKey(value))
                {
                    Cursor.SetCursor(dic_CursorInfo[value].CursorTexture, dic_CursorInfo[value].TexHotspot, CursorMode.Auto);
                    m_currentMouseCursorType = value;
                }
            }
        }
    }

    public bool MouseCursorVisible
    {
        get { return Cursor.visible; }
        set
        {
            Cursor.visible = value;
        }
    }


    private Dictionary<EMouseCursorType, FCursorInfo> dic_CursorInfo = null;
    //是否为记录mode2
    private bool m_isCtrlMode2;
    //是否为记录mode2
    private bool m_isTraceSnap;
    //是否为PK选人状态
    private bool m_isPkSelectTarget;

    public void InitMouseCursorManager()
    {
        dic_CursorInfo = new Dictionary<EMouseCursorType, FCursorInfo>();
        loadMouseCursorTexture();
        MouseCursorType = EMouseCursorType.MCT_Normal;
    }
    public void DestroyMouseCursroManager()
    {
        foreach(var tmpCursorInfo in dic_CursorInfo)
        {
            AssetSystem.UnloadAsset(tmpCursorInfo.Value.CursorTexture);
            //tmpCursorInfo.Value.CursorTexture = null;
        }
        dic_CursorInfo.Clear();
        dic_CursorInfo = null;
    }

    /// <summary>
    /// 加载光标纹理资源
    /// </summary>
    private void loadMouseCursorTexture()
    {
        if (dic_CursorInfo == null)
            return;

//        AddCursorInfo(EMouseCursorType.MCT_None, "Effect/selectEffect/mouse_none", false);
        AddCursorInfo(EMouseCursorType.MCT_Normal, "CommonArtist/Effect/selectEffect/mouse_up", false);
        AddCursorInfo(EMouseCursorType.MCT_OnSnap, "CommonArtist/Effect/selectEffect/mouse_over", false);
        AddCursorInfo(EMouseCursorType.MCT_NormalMode2, "CommonArtist/Effect/selectEffect/Choice_up", true);
        AddCursorInfo(EMouseCursorType.MCT_OnSnapMode2, "CommonArtist/Effect/selectEffect/Choice_over", true);
        AddCursorInfo(EMouseCursorType.MCT_PkMode, "CommonArtist/Effect/selectEffect/pk_over", false);
    }

    /// <summary>
    /// 将光标信息添加到数组
    /// </summary>
    /// <param name="_cursorType">光标类型</param>
    /// <param name="_strTexCursorPath">光标资源纹理路径</param>
    /// <param name="_bCenterHotspot">热点是否在纹理中心</param>
    /// <param name="_vec2Hotspot">热点坐标</param>
    private void AddCursorInfo(EMouseCursorType _cursorType,string _strTexCursorPath,bool _bCenterHotspot = true,Vector2 _vec2Hotspot = new Vector2())
    {
        /*鼠标资源是一直存在的，所以不用释放，等游戏结束直接释放即可*/
        ResNode curRes = AssetBundleManager.GetAssets(AssetType.Asset_Cursour, _strTexCursorPath);
        if(null == curRes)
        {
            Trace.LogError("设置光标注册点错误，光标纹理资源获取失败.加载资源：" + _strTexCursorPath + " 失败！");
            return;
        }
        Texture2D CursorTexture = curRes.ConvertRes<Texture2D>();
        Vector2 tmpVec2Hotspot = Vector2.zero;
        if (CursorTexture == null && _cursorType != EMouseCursorType.MCT_None)
        {
            Trace.LogError("设置光标注册点错误，光标纹理资源获取失败.加载资源："+ _strTexCursorPath+" 失败！");
            return;
        }

        tmpVec2Hotspot = _vec2Hotspot;

        if (_bCenterHotspot)
            tmpVec2Hotspot = new Vector2(CursorTexture.width / 2, CursorTexture.height / 2);
        
        FCursorInfo newCursorInfo = new FCursorInfo(CursorTexture, tmpVec2Hotspot);
        dic_CursorInfo.Add(_cursorType, newCursorInfo);
    }

    private void updateMouseType()
    {
        if (m_isCtrlMode2)
        {
            if (m_isTraceSnap)
            {
                MouseCursorType = EMouseCursorType.MCT_OnSnapMode2;
            }
            else
            {
                MouseCursorType = EMouseCursorType.MCT_NormalMode2;
            }
        }
        else if(m_isPkSelectTarget)
        {
            MouseCursorType = EMouseCursorType.MCT_PkMode;
        }
        else 
        {
            if (m_isTraceSnap)
            {
                    MouseCursorType = EMouseCursorType.MCT_OnSnap;
            }
            else
            {
                    MouseCursorType = EMouseCursorType.MCT_Normal;
            }
        }
    }

    public void setCtrlMode(bool isMode2)
    {
        m_isCtrlMode2 = isMode2;
        updateMouseType();
    }

    public void setTraceSnap(bool isSnap)
    {
        m_isTraceSnap = isSnap;
        updateMouseType();
    }

    public void setPkMode(bool isSelect)
    {
        m_isPkSelectTarget = isSelect;
        updateMouseType();
    }
}

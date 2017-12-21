using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Scaleform;

/// <summary>
/// Lable管理器，负责Lable的创建删除以及更新
/// </summary>
public class GfxLableManager
{
    #region API

    /// <summary>
    /// 创建一个Lable，返回一个GfxLable实例，失败返回null
    /// </summary>
    /// <param name="swf_id">哪个SWF</param>
    /// <param name="text">内容</param>
    /// <param name="col">颜色</param>
    public static GfxLable CreateLable(string text, Color col)
    {
        if (SFGFxMovieManager.Instance == null)
        {
            Trace.LogError("SFGFxMovieManager没有初始化，请不要在Awake或Start中创建！");
            return null;
        }

        if (!bInit)
        {
            Trace.LogError("GfxLableManager必须初始化！");
            return null;
        }
        //有可能DynamicMovie还没有创建完成.
        if (GfxDynamicMovie.Instance == null)
        {
            //Trace.LogError("DynamicMovie尚未创建完成，请等待创建完成后再调用。");
            return null;
        }
        Value ins = new Value();
        if (!GfxDynamicMovie.Instance.AttachMovie(ref ins, LableAS3RefernceName, LableAS3RefernceName + index, -1))
        {
            Trace.LogError("Flash -- Lable创建失败！");
            ins = null;
            return null;
        }
        GfxLable gf = new GfxLable(ins, text, col, index);
        m_LableMap.Add(index, gf);
        index++;
        return gf;
    }


    /// <summary>
    /// 初始化，必须先调用哦
    /// </summary>
    public static void Init()
    {
        if (!bInit)
        {
            bInit = true;
        }
    }


    /// <summary>
    /// 摧毁所有的组件
    /// </summary>
    public static void DestroyALL()
    {
        if (Movie.GetSingleton<GfxDynamicMovie>() == null)
        {
            return;
        }
        if (null != m_LableMap)
        {
            foreach (KeyValuePair<int, GfxLable> var in m_LableMap)
            {
                if (GfxDynamicMovie.Instance != null)
                {
                    GfxDynamicMovie.Instance.RemoveMovieAS3(var.Value.Instance);
                }
                var.Value.Clear();
            }
            m_LableMap.Clear();
        }
        index = 0;
    }


    /// <summary>
    /// 摧毁指定的Lable
    /// </summary>
    /// <param name="instance">需要清除的Lable</param>
    public static void Destroy(GfxLable instance)
    {
        if (Movie.GetSingleton<GfxDynamicMovie>() == null)
        {
            return;
        }
        if (null == instance)
        {
            return;
        }
        int id = instance.ID;
        if (m_LableMap.ContainsKey(id))
        {
            if (GfxDynamicMovie.Instance != null)
            {
                GfxDynamicMovie.Instance.RemoveMovieAS3(instance.Instance);
            }
            instance.Clear();
            m_LableMap[id] = null;
            m_LableMap.Remove(id);
            instance = null;
        }

    }


    #endregion

    #region 静态属性
    private static bool bInit = false;

    private const string LableAS3RefernceName = "Com.Scaleform.Common.DefaultLable";
    private static Dictionary<int, GfxLable> m_LableMap = new Dictionary<int, GfxLable>();
   
    private static int index = 0;
    #endregion

}

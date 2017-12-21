/*******************************************************************
** 文件名:	AssetBundleManager_Shader.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，Shader部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Runtime.InteropServices;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    /*
     * Shader与其他资源不一样，shader全部加载进来，再游戏推出的时候再全部卸载
     * 这个功能在编辑器中并不会生效
     */

    static Dictionary<string, Shader> m_ShaderAssetTable = new Dictionary<string, Shader>();

    static List<AssetBundle> m_ShaderAssetBundleList = new List<AssetBundle>();

    static List<string> m_NormalShaderAssetBundlePath = new List<string>();
    static List<string> m_FirstLoadAssetBundlePath = new List<string>();
    static string FirstLoadPatchKey = "unitybuiltinshader";

    static string entityAlphaShaderName = "Custom/ModelFadeIn";
    static string entityOutlineShaderName = "Custom/OutLine";

    public static Shader GetShader(string shaderName)
    {
        Shader sh = null;
        if(!isOnRunTimeMode)
        {
            sh = Shader.Find(shaderName);
            return sh;
        }
        else
        {
            if (!m_ShaderAssetTable.TryGetValue(shaderName, out sh))
            {
                sh = Shader.Find(shaderName);
            }
            return sh;
        }
    }

    private static void LoadShaders()
    {
        ImageEffects.ImageEffectManager.API_RegisterLoadShaderDelegate(GetShader);

        if (!isOnRunTimeMode)
        {
            LoadEntityShder();
            return;
        }

        foreach (string str in m_FirstLoadAssetBundlePath)
        {
            AssetBundle p = LoadAssetBundle(FullAssetPackagePathRoot + str, null);
            if (p)
            {
                Shader[] allShaders = p.LoadAllAssets<Shader>();
                foreach (Shader shader in allShaders)
                {
                    Shader temp = null;
                    string shaderName = shader.name;
                    if (m_ShaderAssetTable.TryGetValue(shaderName, out temp))
                    {
                        Debug.LogWarning("资源包中有重复的shader:" + shaderName + ",资源包:" + str);
                    }
                    else
                    {
                        m_ShaderAssetTable.Add(shaderName, shader);
                    }
                }
                m_ShaderAssetBundleList.Add(p);
            }
            else
            {
                Debug.LogWarning("加载shader文件失败: " + str);
            }
        }
        m_FirstLoadAssetBundlePath.Clear();
        foreach (string str in m_NormalShaderAssetBundlePath)
        {
            AssetBundle p = LoadAssetBundle(FullAssetPackagePathRoot + str, null);
            if (p)
            {
                Shader[] allShaders = p.LoadAllAssets<Shader>();
                foreach(Shader shader in allShaders)
                {
                    Shader temp = null;
                    string shaderName = shader.name;
                    if(m_ShaderAssetTable.TryGetValue(shaderName,out temp))
                    {
                        Debug.LogWarning("资源包中有重复的shader:" + shaderName + ",资源包:" + str);
                    }
                    else
                    {
                        m_ShaderAssetTable.Add(shaderName,shader);
                    }
                }
                m_ShaderAssetBundleList.Add(p);
            }
            else
            {
                Debug.LogWarning("加载shader文件失败: " + str);
            }
        }
        m_NormalShaderAssetBundlePath.Clear();

        LoadEntityShder();
        Shader.WarmupAllShaders();
    }

    /// <summary>
    /// 删除资源
    /// </summary>
    /// <param name="res"></param>
    private static void UnLoadShaders()
    {
        //编辑器下，是无效的
        if (!isOnRunTimeMode)
        {
            return;
        }

        foreach (AssetBundle nod in m_ShaderAssetBundleList)
        {
            if (nod)
            {
                nod.Unload(true);
            }
        }
        m_ShaderAssetTable.Clear();
        m_ShaderAssetBundleList.Clear();
    }


    private static void LoadEntityShder()
    {
       m_entityAlphaShader = GetShader("Custom/ModelFadeIn");
       m_entityOutlineShader = GetShader("Custom/OutLine");
    }

    /// <summary>
    /// 实体透明Shader
    /// </summary>
    public static Shader EntityAlphaShader
    {
        get
        {
            return m_entityAlphaShader;
        }
    }

    /// <summary>
    /// 实体描边Shader
    /// </summary>
    public static Shader EntityOutlineShader
    {
        get
        {
            return m_entityOutlineShader;
        }
    }

    private static Shader m_entityAlphaShader = null;
    private static Shader m_entityOutlineShader = null;
}

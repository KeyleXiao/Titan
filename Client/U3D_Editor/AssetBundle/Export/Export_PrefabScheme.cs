/*******************************************************************
** 文件名:	EditorLog.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using System;
using LZ4ps;
using System.Text;
using System.Linq;

public partial class SchemeExport
{

    /// <summary>
    /// 导出预制体配置
    /// </summary>
    /// <param name="configTable">为空，将导出默认的</param>
    /// <param name="depedenceConfigTable">为空，将导出默认的</param>
    /// <param name="depedenceResTable">为空，将导出默认的</param>
    public static void Export_PrefabScheme(Dictionary<string, Dictionary<string, UnionResConfigData>> configTable = null,
                                         Dictionary<string, PrefabDependenicesResConfigData> depedenceConfigTable = null,
                                          Dictionary<string, ResConfigData> depedenceResTable = null)
    {
        Export_PrefabDepedenceConfig(depedenceResTable);
        Export_PrefabSelfConfig(configTable, depedenceConfigTable);
    }

    /// <summary>
    /// 导出预制体的资源表
    /// </summary>
    /// <param name="depedenceResTable">为空，将导出默认的</param>
    private static void Export_PrefabDepedenceConfig(Dictionary<string,ResConfigData> depedenceResTable = null)
    {
        string dic = System.IO.Path.GetDirectoryName(AssetBundleManager.GetDependenicesResConfigPath());
        if (!System.IO.Directory.Exists(dic))
        {
            System.IO.Directory.CreateDirectory(dic);
        }
        if (depedenceResTable == null)
        {
            depedenceResTable = AssetBundleManager.DependenciesResConfigTable;
        }

        StreamWriter writer = new StreamWriter(AssetBundleManager.GetDependenicesResConfigPath(), false, new System.Text.UTF8Encoding(true));


        foreach (ResConfigData cof in depedenceResTable.Values)
        {
            string s = string.Empty;
            s += cof.AssetBundleName;
            s += ",";

            s += cof.AssetName;
            s += ",";

            s += cof.AssetGUID;
            s += ",";

            s += cof.AssetSize_X.ToString("F3");
            s += ",";

            s += cof.AssetSize_Y.ToString("F3");
            writer.WriteLine(s);
        }

        writer.Flush();
        writer.Close();
    }

    private static void Export_PrefabSelfConfig(Dictionary<string, Dictionary<string, UnionResConfigData>> configTable = null,
                                         Dictionary<string, PrefabDependenicesResConfigData> depedenceResult = null)
    {
        string dic = System.IO.Path.GetDirectoryName(AssetBundleManager.GetPrefabConfigPath());
        if (!System.IO.Directory.Exists(dic))
        {
            System.IO.Directory.CreateDirectory(dic);
        }
        if (null == configTable || depedenceResult == null)
        {
            configTable = AssetBundleManager.PrefabConfigTable;
            depedenceResult = AssetBundleManager.PrefabDependenciesConfigTable;
        }
        StreamWriter writer = new StreamWriter(AssetBundleManager.GetPrefabConfigPath(), false, new System.Text.UTF8Encoding(true));

        foreach (KeyValuePair<string, Dictionary<string, UnionResConfigData>> par in configTable)
        {
            foreach (KeyValuePair<string, UnionResConfigData> valpar in par.Value)
            {
                UnionResConfigData config = valpar.Value;

                //首先，取出预制的依赖配置
                PrefabDependenicesResConfigData dependenicesData = null;
                if (!depedenceResult.TryGetValue(config.AssetGUID, out dependenicesData))
                {
                    //Debug.LogWarning("预制体:" + config.AssetName + ",没有资源依赖~");
                }

                //先写入预制体信息,预制体信息格式为:包名，资源名，GUID,合并后的文件索引,其实位置，长度,贴图数量，FBX数量，AnimationClip数量，AnimatorController数量
                string s = string.Empty;
                s += config.AssetBundleName;
                s += ",";

                s += config.AssetName;
                s += ",";

                s += config.AssetGUID;
                s += ",";

                s += config.unionIndx;
                s += ",";

                s += config.startIndex;
                s += ",";

                s += config.dataLength;
                s += ",";

                //这个预制体没有依赖的资源
                if (null == dependenicesData)
                {
                    //贴图
                    s += 0;
                    s += ",";

                    //fbx
                    s += 0;
                    s += ",";

                    //clip
                    s += 0;
                    s += ",";

                    //controller
                    s += 0;
                    s += ",";

                    //shader
                    s += 0;
                    s += ",";

                    //material
                    s += 0;

                    //然后就可以写入到文件了.
                    writer.WriteLine(s);
                }
                else
                {
                    s += dependenicesData.TextureResConfigDatas.Count;
                    s += ",";

                    s += dependenicesData.FBXResConfigDatas.Count;
                    s += ",";

                    s += dependenicesData.AnimatonClipResConfigDatas.Count;
                    s += ",";

                    s += dependenicesData.AnimatorControllerResConfigDatas.Count;
                    s += ",";

                    s += dependenicesData.ShaderResConfigDatas.Count;
                    s += ",";

                    s += dependenicesData.MaterialResConfigDatas.Count;

                    //然后就可以写入到文件了.
                    writer.WriteLine(s);

                    /*写完依赖的数目之后，就可以另起一行，开始写入具体的数据*/

                    //先写入贴图
                    foreach (string cof in dependenicesData.TextureResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }

                    //再写FBX

                    foreach (string cof in dependenicesData.FBXResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }

                    //再写clip

                    foreach (string cof in dependenicesData.AnimatonClipResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }


                    //再写controller

                    foreach (string cof in dependenicesData.AnimatorControllerResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }

                    //再写shader
                    foreach (string cof in dependenicesData.ShaderResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }


                    //再写材质球
                    foreach (string cof in dependenicesData.MaterialResConfigDatas)
                    {
                        writer.WriteLine(cof);
                    }
                }

            }
        }

        writer.Flush();
        writer.Close();
    }
}

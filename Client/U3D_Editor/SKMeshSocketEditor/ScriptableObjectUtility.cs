/*******************************************************************
** 文件名:	ScriptableObjectUtility.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/2/4
** 版  本:	1.0
** 描  述:	 * 创建一个自定义类型的assert(相当于自定义类型的配置文件)
             * 读取方法:
             * 1.在需要引用数据的脚本中定义一个成员变量，然后在编辑器中选取绑定
             * 2.通过AssetBundle.Load动态加载(可以从Web端下载或本地读取)
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using UnityEditor;
using System.IO;

public static class ScriptableObjectUtility
{
    public static T CreateAsset<T>(string fileName) where T : ScriptableObject
    {
        T asset = ScriptableObject.CreateInstance<T>();

        string path = AssetDatabase.GetAssetPath(Selection.activeObject);
        if (path == "")
        {
            path = "Assets";
        }
        else if (Path.GetExtension(path) != "")
        {
            path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(Selection.activeObject)), "");
        }

        string assetPathAndName = AssetDatabase.GenerateUniqueAssetPath(path + "/" + fileName + ".asset");

        AssetDatabase.CreateAsset(asset, assetPathAndName);

        AssetDatabase.SaveAssets();
        return asset;
    }

    public static void DelectAsset(string fileName)
    {
        string assetPathAndName = ("Assets/" + fileName + ".asset");
        AssetDatabase.DeleteAsset(assetPathAndName);
    }

}

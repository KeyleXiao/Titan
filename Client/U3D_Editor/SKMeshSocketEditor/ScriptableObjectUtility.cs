/*******************************************************************
** �ļ���:	ScriptableObjectUtility.cs
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2014 - Speed
** ������:	�ֽ���
** ��  ��:	2015/2/4
** ��  ��:	1.0
** ��  ��:	 * ����һ���Զ������͵�assert(�൱���Զ������͵������ļ�)
             * ��ȡ����:
             * 1.����Ҫ�������ݵĽű��ж���һ����Ա������Ȼ���ڱ༭����ѡȡ��
             * 2.ͨ��AssetBundle.Load��̬����(���Դ�Web�����ػ򱾵ض�ȡ)
** Ӧ  ��:  
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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

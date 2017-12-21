/*******************************************************************
** 文件名:	UAtlasMaker.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	林建聪
** 日  期:	2016/3/28
** 版  本:	1.0
** 描  述:	将一个文件夹内的图片序列化到一个预制体里的编辑器工具，
** 应  用:  通常用于一些需要动态切换的图片资源
 *          
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using UnityEditor;
using System.IO;


class UAtlasMaker
{
    [MenuItem("USpeedUI/UGUIMakeAtlas")]
    static void MakeAtlas()
    {
        Debug.Log("序列化动态图片...");
        //新建的预制体存放根路径
        string targetDir = Application.dataPath + "/Resources/UI/DynamicPrefabs";
        if (!Directory.Exists(targetDir))
        {
            Directory.CreateDirectory(targetDir);
        }
        //需要制作预制体的资源根路径
        DirectoryInfo fromDirInfo = new DirectoryInfo(Application.dataPath + "/Artist/UI/Dynamic");
        if(!fromDirInfo.Exists)
        {
            Debug.Log("没找到动态图片图素文件夹  /Artist/UI/Dynamic  ... \n序列化动态图片结束....");
            return;
        }

        //遍历该路径下所有子文件夹，并以子文件夹命名预制体
        foreach (DirectoryInfo dirInfo in fromDirInfo.GetDirectories())
        {
            Debug.Log("开始给子文件夹:" + dirInfo.Name+ "制作预制体");
            MakeOnePrefab(dirInfo, fromDirInfo, targetDir);
        }
        Debug.Log("序列化动态图片结束....");

    }
    static void MakeOnePrefab(DirectoryInfo dirInfo, DirectoryInfo fromDirInfo, string targetDir)
    {
        string fieldName = dirInfo.Name;
        FileInfo[] allPngFiles = null;

        // 遍历子文件夹找到png文件//
        foreach (DirectoryInfo childDir in dirInfo.GetDirectories())
        {
            allPngFiles = childDir.GetFiles("*.png", SearchOption.TopDirectoryOnly);
            if (allPngFiles.Length == 0)
            {
                MakeOnePrefab(childDir, fromDirInfo, targetDir);
                continue;
            }
            else
            {
                MakePrefab(allPngFiles, dirInfo, fromDirInfo, targetDir);
            }
        }

    }

    static void MakePrefab(FileInfo[] allPngFiles, DirectoryInfo dirInfo, DirectoryInfo fromDirInfo, string targetDir)
    {
        //用资源前缀做预制体名字 e.g.\\***_ID.png
        int beginIndex = allPngFiles[0].FullName.LastIndexOf("\\") + 1;
        int lenght = allPngFiles[0].FullName.LastIndexOf('_') - beginIndex;
        string fieldName = (allPngFiles[0].FullName.Substring(beginIndex, lenght));


        if (allPngFiles.Length <= 0)
        {
            string shortPath = fromDirInfo.FullName.Substring(fromDirInfo.FullName.IndexOf("Assets"));
            Debug.LogWarning(string.Format("{0}该文件夹下没有png文件？", shortPath));
        }
        else
        {
            GameObject go = new GameObject(fieldName);
            UAtlasMap am = go.AddComponent<UAtlasMap>();

            // 如果“Resources/UI/DynamicImagePrefabs/**”下没有和“Artist/UI/ResDynamic/**对应的文件夹”则创建一个//
            string FolderName = dirInfo.FullName.Substring(fromDirInfo.FullName.Length + 1, dirInfo.FullName.Length - fromDirInfo.FullName.Length - 1);
            //判断是否还有子文件夹名
            int i = FolderName.IndexOf('\\');
            if(i>=0)
                FolderName = FolderName.Substring(0, i);

            string prefabParentFieldPath = string.Format("{0}/{1}", targetDir, FolderName);

            if (!Directory.Exists(prefabParentFieldPath))
            {
                Directory.CreateDirectory(prefabParentFieldPath);
            }

            // 将Sprite存入AtlasMap脚本中//
            foreach (FileInfo pngFile in allPngFiles)
            {
                beginIndex = pngFile.FullName.LastIndexOf('_') + 1;
                lenght = pngFile.FullName.LastIndexOf('.') - beginIndex;

                int nKey = System.Convert.ToInt32((pngFile.FullName.Substring(beginIndex, lenght)));

                string assetPath = pngFile.FullName.Substring(pngFile.FullName.IndexOf("Assets"));
                Sprite sprite = AssetDatabase.LoadAssetAtPath<Sprite>(assetPath);
                if (sprite == null)
                {
                    Debug.LogWarning(string.Format("确认下当前TextureType 是否是Sprite： \"{0}\"", assetPath));
                    continue;
                }
                am.AddSprite(nKey, sprite);
            }

            // 在对应文件夹上生成预设//
            string prefabAllPath = string.Format("{0}/{1}.prefab", prefabParentFieldPath, fieldName);
            string prefabPath = prefabAllPath.Substring(prefabAllPath.IndexOf("Assets"));
            PrefabUtility.CreatePrefab(prefabPath, go);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();

            // 销毁go//
            GameObject.DestroyImmediate(go);
        }


    }
}

/*******************************************************************
** 文件名:	UAtlasMap.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	林建聪
** 日  期:	2016/3/28
** 版  本:	1.0
** 描  述:	将一个文件夹内的图片序列化到一个预制体里的脚本
** 应  用:  用于存储制作好的动态图预设上的sprite信息
 *          
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public class UAtlasMap : MonoBehaviour
{
    
    [HideInInspector]
    [SerializeField]
    protected List<Sprite> spriteValuesList = new List<Sprite>();
    [HideInInspector]
    [SerializeField]
    protected List<int> spriteKeysList = new List<int>();

    public Sprite GetSpriteByKeyID(int nKey)
    {
        int nValueIndex = spriteKeysList.FindIndex(o => o == nKey);
        if (nValueIndex == -1)
            return null;
        else
            return spriteValuesList[nValueIndex];
    }

    public void AddSprite(int nKey,Sprite spValue)
    {
        spriteKeysList.Add(nKey);
        spriteValuesList.Add(spValue);
    }
}


/*
* type指的是该UI所在界面对应在Scripts或ResourcesHW下的文件夹名字，比如创角选角的CSHero
* name是所需sprite的名字
* extendedStr是路径扩展字符，比如MainUI路径下各个动态图集预设是在其各个子目录下的，所以需要额外的扩展路径MainUI
*/
/*
public static Sprite GetSpriteByTypeAndName(string type, string spriteName, string extendedStr = null)
{
    string goPath = "";
    if (string.IsNullOrEmpty(extendedStr))
    {
        goPath = string.Format("UI/{0}/{0}DynSprite", type);
    }
    else
    {
        goPath = string.Format("UI/{1}/{0}/{0}DynSprite", type, extendedStr);
    }
    GameObject go = Resources.Load(goPath);
    if (go == null)
    {
        Debug.LogError(string.Format("There is no prefab of {0}. Be sure that type is right and you had make atlas of type", type));
        return null;
    }
    AtlasMap am = go.GetComponent();
    if (am == null)
    {
        Debug.LogError(string.Format("There is no AtlasMap on prefab of {0}.Be sure MakeAtlasMaker work", type));
        return null;
    }
    return am.GetSpriteByName(spriteName);
}
*/
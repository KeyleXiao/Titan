/*******************************************************************
** 文件名:	UArtistFontEditor.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-5-17
** 版  本:	1.0
** 描  述:	美术字体.fnt文件解析类
		
********************************************************************/
using UnityEngine;
using System.Collections;
using UnityEditor;
using System.IO;
using System.Collections.Generic;
using System.Text.RegularExpressions;

public class UArtistFontEditor : Editor
{
    [MenuItem("Assets/CreateBMFont")]
    static void CreateFont()
    {
        Object obj = Selection.activeObject;
        string fntPath = AssetDatabase.GetAssetPath(obj);
        if (fntPath.IndexOf(".fnt") == -1)
        {
            Debug.LogWarning("这不是字体文件。");
            return;
        }
        string customFontPath = fntPath.Replace(".fnt", ".fontsettings");
        if (!File.Exists(customFontPath))
        {
            //没有fontsettings文件则，创建一个
            Font CustomFont = new Font();
            {
                AssetDatabase.CreateAsset(CustomFont, customFontPath);
                AssetDatabase.SaveAssets();
            }
        }

        StreamReader reader = new StreamReader(new FileStream(fntPath, FileMode.Open));

        List<CharacterInfo> charList = new List<CharacterInfo>();

        Regex reg = new Regex(@"char id=(?<id>\d+)\s+x=(?<x>\d+)\s+y=(?<y>\d+)\s+width=(?<width>\d+)\s+height=(?<height>\d+)\s+xoffset=(?<xoffset>\d+)\s+yoffset=(?<yoffset>\d+)\s+xadvance=(?<xadvance>\d+)\s+");
        string line = reader.ReadLine();
        string strSpriteName = string.Empty;
        int lineHeight = 0;
        int texWidth = 1;
        int texHeight = 1;

        while (line != null)
        {
            if (line.IndexOf("char id=") != -1)
            {
                Match match = reg.Match(line);
                MathCharLine(match, charList, texWidth, texHeight);
            }
            else if (line.IndexOf("scaleW=") != -1)
            {
                Regex reg2 = new Regex(@"common lineHeight=(?<lineHeight>\d+)\s+.*scaleW=(?<scaleW>\d+)\s+scaleH=(?<scaleH>\d+)");
                Match match = reg2.Match(line);
                MathCommonLine(match, out lineHeight, out texWidth, out texHeight);
            }
            else if (line.IndexOf("page id=") != -1)
            {
                //page id=0 file="hit_0.png"(?<="")
                Regex reg3 = new Regex(@"page id=(?<id>\d+)\s+file=\""(?<file>.+?)\""");
                Match match = reg3.Match(line);
                MathPageLine(match, out strSpriteName);
            }
            line = reader.ReadLine();
        }

        Font customFont = AssetDatabase.LoadAssetAtPath<Font>(customFontPath);
        customFont.characterInfo = charList.ToArray();
        EditorUtility.SetDirty(customFont);
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();

        RefreshFontMaterial(customFont, fntPath, strSpriteName);

        Debug.Log(customFont);
    }

    protected static void MathCharLine(Match match, List<CharacterInfo> charList, int texWidth, int texHeight)
    {
        if (match != Match.Empty)
        {
            var id = System.Convert.ToInt32(match.Groups["id"].Value);
            var x = System.Convert.ToInt32(match.Groups["x"].Value);
            var y = System.Convert.ToInt32(match.Groups["y"].Value);
            var width = System.Convert.ToInt32(match.Groups["width"].Value);
            var height = System.Convert.ToInt32(match.Groups["height"].Value);
            var xoffset = System.Convert.ToInt32(match.Groups["xoffset"].Value);
            var yoffset = System.Convert.ToInt32(match.Groups["yoffset"].Value);
            var xadvance = System.Convert.ToInt32(match.Groups["xadvance"].Value);

            CharacterInfo info = new CharacterInfo();
            info.index = id;
            float uvx = 1f * x / texWidth;
            float uvy = 1 - (1f * y / texHeight);
            float uvw = 1f * width / texWidth;
            float uvh = -1f * height / texHeight;

            info.uvBottomLeft = new Vector2(uvx, uvy);
            info.uvBottomRight = new Vector2(uvx + uvw, uvy);
            info.uvTopLeft = new Vector2(uvx, uvy + uvh);
            info.uvTopRight = new Vector2(uvx + uvw, uvy + uvh);

            info.minX = xoffset;
            info.minY = yoffset + height / 2;   // 这样调出来的效果是ok的，原理未知  
            info.glyphWidth = width;
            info.glyphHeight = -height; // 同上，不知道为什么要用负的，可能跟unity纹理uv有关  
            info.advance = xadvance;

            charList.Add(info);
        }

    }

    protected static void MathCommonLine(Match match, out int lineHeight, out int texWidth, out int texHeight)
    {
        if (match != Match.Empty)
        {
            lineHeight = System.Convert.ToInt32(match.Groups["lineHeight"].Value);
            texWidth = System.Convert.ToInt32(match.Groups["scaleW"].Value);
            texHeight = System.Convert.ToInt32(match.Groups["scaleH"].Value);
        }
        else
        {
            lineHeight = 0;
            texWidth = texHeight = 1;
        }
    }

    protected static void MathPageLine(Match match, out string spriteName)
    {
        if (match != Match.Empty)
        {
            spriteName = match.Groups["file"].Value;
            spriteName = spriteName.Replace(".png", "");
        }
        else
        {
            spriteName = string.Empty;
        }
    }

    protected static string GetTextureFileName(string strSpriteName)
    {
        string path = UnityEditor.AssetDatabase.GetAssetPath(UnityEditor.Selection.activeInstanceID);
        path = Path.GetDirectoryName(path) + "/";

        return path + strSpriteName + ".png";

    }

    protected static void RefreshFontMaterial(Font customFont, string fntPath, string strSpriteName)
    {
        string matFileName = fntPath.Replace(".fnt", ".mat");
        Material mat = null;
        {
            Shader shader = AssetBundleManager.GetShader("UI/Unlit/Text");
            mat = new Material(shader);
            Texture tex = AssetDatabase.LoadAssetAtPath(GetTextureFileName(strSpriteName), typeof(Texture)) as Texture;
            mat.SetTexture("_MainTex", tex);
            AssetDatabase.CreateAsset(mat, matFileName);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();

        }
        customFont.material = mat;
    }
}

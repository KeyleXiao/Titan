using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine.UI;
using System.Collections.Generic;

namespace RichTextPart
{
    public class URichTextHandler_Texture : URichTextHandler_Base
    {
        private static URichTextHandler_Texture _handler = null;

        public URichTextSprite TextSprite
        {
            get
            {
                return this.richText.GetTextSprite(); 
            }
        }
        public static void Init()
        {
            if (_handler == null)
            {
                _handler = new URichTextHandler_Texture();
                URichTextUtil.AddHandler(_handler);
            }
        }

        URichTextHandler_Texture()
        {

        }
        #region handler接口
        //前期的字符串替换
        public override string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group)
        {
            text = URichTextUtil.ReplacePlaceHolder("<quad size="+ group[2].Value + " width=" + group[3].Value + " />", text, startIndex);
            return text;
        }

        //字符串网格控制
        public override bool HandleTextMesh(URichTextParseInfo info)
        {
            if (!Application.isPlaying)
            {
                Debug.LogWarning("请在运行模式下观察富文本转图片结果");
                return false;
            }
                
            if (info.vertStartIndex < 0)
                return false;

            List<UIVertex> uivertList = new List<UIVertex>();
            vHelper.GetUIVertexStream(uivertList);
            // 移除角落产生的字符纹理
            if (info.vertStartIndex * 6 + 6 > uivertList.Count)
                return false;

            int spriteIndex = 0;

            Data.AnimSpriteConfig.SSchemeAnimFrameData frameData = Data.AnimSpriteConfig.AnimSpriteConfig.Instance.GetData(info.values[1].Value);
            if (frameData == null)
                return false;
            Rect newSpriteRect = frameData.GetFrameRect(spriteIndex);//sInfo.rect;
            URichTextSpriteMeshInfo meshInfo = new URichTextSpriteMeshInfo();

            if (frameData.nAnimFrame > 0)
                meshInfo.animKey = frameData.strAnimName;
            else
                meshInfo.animKey = "";

            meshInfo.animLen = frameData.nAnimFrame == 0?1: frameData.nAnimFrame;
            meshInfo.spriteIndex = spriteIndex;
            meshInfo.place = info.startIndex;
            //顶点:
            //textpos：图片的左上角
            //之前的富文本被隐藏部分也会占用顶点！！
            //每个索引一个顶点...所以要乘以6
            Vector3 textpos = uivertList[info.vertStartIndex * 6].position;     
            int size = int.Parse(info.values[2].Value);
            int width = int.Parse(info.values[3].Value);
            //设置图片的位置
            meshInfo.vertices[0] = new Vector3(0, 0, 0) + textpos;
            meshInfo.vertices[1] = new Vector3(size * width, 0, 0) + textpos;
            meshInfo.vertices[2] = new Vector3(size * width, -size, 0) + textpos;
            meshInfo.vertices[3] = new Vector3(0, -size, 0) + textpos;
            //uv
            Sprite frameSprite = USpriteManager.Instance.GetAnimSprite(frameData.strAnimName);
            if (frameSprite == null)
                return false;
            Vector2 newTexSize = new Vector2(frameSprite.texture.width, frameSprite.texture.height);

            meshInfo.uvs[3] = new Vector2(newSpriteRect.x / newTexSize.x, 1 - (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
                            //new Vector2(newSpriteRect.x / newTexSize.x, newSpriteRect.y / newTexSize.y);
            meshInfo.uvs[1] = new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, 1 - newSpriteRect.y / newTexSize.y);
                            //new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
            meshInfo.uvs[2] = new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, 1 - (newSpriteRect.y + newSpriteRect.height) / newTexSize.y); 
                            //new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, newSpriteRect.y / newTexSize.y);
            meshInfo.uvs[0] = new Vector2(newSpriteRect.x / newTexSize.x, 1 - newSpriteRect.y / newTexSize.y); 
                            //new Vector2(newSpriteRect.x / newTexSize.x, (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);


            meshInfo.colors[0] = Color.white;
            meshInfo.colors[1] = Color.white;
            meshInfo.colors[2] = Color.white;
            meshInfo.colors[3] = Color.white;

            //索引先不处理

            TextSprite.UpdateSpriteMeshInfo(meshInfo);
            
            return true;
        }

        public override bool IsSingleRegex() { return true; }
        public override int GetRegexType() { return URichTextUtil.REGEX_TYPE_SYSTEMB; }

        public override string[] GetRegexInfo()
        {
            return new string[] { @"<img name=(.+?) size=(\d*\.?\d+%?) width=(\d*\.?\d+%?) />" };
        }
        #endregion
    }
}
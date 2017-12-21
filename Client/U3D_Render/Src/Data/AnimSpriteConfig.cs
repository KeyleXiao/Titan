using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;
using UnityEngine;

namespace Data.AnimSpriteConfig
{
    public enum EAnimSpriteConfig
    {
        EASC_AssetName,
        EASC_AnimName,
        EASC_AnimFrame,
        EASC_AnimStartPos,
        EASC_AnimFrameSize,
    }
    public class SSchemeAnimFrameData
    {
        public string strAssetName;

        public string strAnimName;
        public int nAnimFrame;
        public Vector2 vec2AnimStartPos;
        public List<Vector2> vec2AnimFrameSize;

        public Rect GetFrameRect(int _nFrame)
        {

            Vector2 tmpFrameSize = _nFrame >= vec2AnimFrameSize.Count ? vec2AnimFrameSize[0] : vec2AnimFrameSize[_nFrame];

            return new Rect(vec2AnimStartPos.x + _nFrame * tmpFrameSize.x, vec2AnimStartPos.y, tmpFrameSize.x, tmpFrameSize.y);
        }
    }
    class AnimSpriteConfig : Singleton<AnimSpriteConfig>, IModelConfig
    {
        private Dictionary<string, SSchemeAnimFrameData> m_dicAnimSpriteConfig;

        public void Load(string strPath)
        {
            m_dicAnimSpriteConfig = new Dictionary<string, SSchemeAnimFrameData>();

            ScpReader reader = new ScpReader(strPath, true);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeAnimFrameData sConfig = new SSchemeAnimFrameData();
                sConfig.strAssetName = reader.GetString(i, (int)EAnimSpriteConfig.EASC_AssetName, string.Empty);
                sConfig.strAnimName = reader.GetString(i, (int)EAnimSpriteConfig.EASC_AnimName, string.Empty);
                sConfig.nAnimFrame = reader.GetInt(i, (int)EAnimSpriteConfig.EASC_AnimFrame, 1);

                string tmpAnimStartPos = reader.GetString(i, (int)EAnimSpriteConfig.EASC_AnimStartPos, "0;0");
                sConfig.vec2AnimStartPos = new Vector2(float.Parse(tmpAnimStartPos.Split(';')[0]), float.Parse(tmpAnimStartPos.Split(';')[1]));


                string tmpAnimFrameSize = reader.GetString(i, (int)EAnimSpriteConfig.EASC_AnimFrameSize, "32;32");
                sConfig.vec2AnimFrameSize = new List<Vector2>();
                if (tmpAnimFrameSize.Contains("|"))
                {
                    string[] sizeArray = tmpAnimFrameSize.Split('|');
                    if(sizeArray.Length != sConfig.nAnimFrame)
                    {
                        Trace.LogWarning("动画帧尺寸不一致，且尺寸数量与帧数不符，所有帧都用第一帧尺寸使用");
                        string tmpSize = sizeArray[0];
                        sConfig.vec2AnimFrameSize.Add(new Vector2(float.Parse(tmpSize.Split(';')[0]), float.Parse(tmpSize.Split(';')[1])));
                    }
                    else
                    {
                        foreach(var strSize in sizeArray)
                        {
                            sConfig.vec2AnimFrameSize.Add(new Vector2(float.Parse(strSize.Split(';')[0]), float.Parse(strSize.Split(';')[1])));
                        }
                    }
                }
                else
                {
                    sConfig.vec2AnimFrameSize.Add(new Vector2(float.Parse(tmpAnimFrameSize.Split(';')[0]), float.Parse(tmpAnimFrameSize.Split(';')[1])));
                }
                if(m_dicAnimSpriteConfig.ContainsKey(sConfig.strAnimName))
                {
                    Trace.LogWarning("有相同的动画名称:"+ sConfig.strAnimName + "在表第"+i+"行.");
                }
                else
                    m_dicAnimSpriteConfig.Add(sConfig.strAnimName, sConfig);
            }
            reader.Dispose();
            reader = null;
        }

        public void UnLoad()
        {
            if(m_dicAnimSpriteConfig != null)
            {
                m_dicAnimSpriteConfig.Clear();
                m_dicAnimSpriteConfig = null;
            }
        }

        public SSchemeAnimFrameData GetData(string _strAnimName)
        {
            if (!m_dicAnimSpriteConfig.ContainsKey(_strAnimName))
                return null;
            return m_dicAnimSpriteConfig[_strAnimName];
        }

    }
}

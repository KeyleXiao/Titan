/*******************************************************************
** 文件名:	SkinInstance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤实例，外部通过这个来使用这个皮肤
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 这个是皮肤实例，外部通过这个来使用这个皮肤
/// </summary>
public partial class SkinInstance                       
{
    private Dictionary<Renderer, Material[]> m_OrginalMat = new Dictionary<Renderer, Material[]>();
    private Dictionary<Renderer, Material[]> m_AlphaMat = new Dictionary<Renderer, Material[]>();
    bool bInitAlpha = false;

    public void SetAlpha(float alpha)
    {
        if (!bInitAlpha)
        {
            return;
        }
        int cout = AllRenders.Count;
        for (int i = 0; i < cout; i++)
        {
            Renderer r = AllRenders[i];
            if (m_OrginalMat.ContainsKey(r))
            {
                Material[] mats = m_AlphaMat[r];

                ////长度不相等的话，就可能有增加材质球等等其他的效果,不做半透明了。
                //if(mats.Length != r.sharedMaterials.Length)
                //{
                //    return;
                //}

                int l = mats.Length;
                for (int j = 0; j < l; j++)
                {
                    mats[j].SetFloat("_Alpha", alpha);
                }
                //长度不相等的话，就可能有增加材质球等等其他的效果
                if (mats.Length != r.sharedMaterials.Length)
                {
                    Material[] m = r.sharedMaterials;
                    for (int j = 0; j < l; j++)
                    {
                        m[j] = mats[j];
                    }
                    r.sharedMaterials = m;
                }
                else
                {
                    r.sharedMaterials = mats;
                }

            }
        }


    }

    public void ResetAlpha()
    {
        if (!bInitAlpha)
        {
            return;
        }
        int cout = AllRenders.Count;
        for (int i = 0; i < cout; i++)
        {
            Renderer r = AllRenders[i];
            if (m_OrginalMat.ContainsKey(r))
            {
                Material[] mats = m_OrginalMat[r];
                int l = mats.Length;

                ////长度不相等的话，就可能有增加材质球等等其他的效果，不做半透明了
                //if (mats.Length != r.sharedMaterials.Length)
                //{
                //    return;
                //}

                //长度不相等的话，就可能有增加材质球等等其他的效果
                if (mats.Length != r.sharedMaterials.Length)
                {
                    Material[] m = r.sharedMaterials;
                    for (int j = 0; j < l; j++)
                    {
                        m[j] = mats[j];
                    }
                    r.sharedMaterials = m;
                }
                else
                {
                    r.sharedMaterials = mats;
                }

            }
        }
    }

    private void ClearAlpha()
    {
        m_OrginalMat.Clear();
        m_AlphaMat.Clear();
        m_OrginalMat = null;
        m_AlphaMat = null;
    }

    private void InitAlpha(Shader alphaShader)
    {
        if (bInitAlpha || !alphaShader ||
            AllRenders.Count <= 0)
        {
            return;
        }
        bInitAlpha = true;
        int cout = AllRenders.Count;
        for (int i = 0; i < cout; i++)
        {
            Renderer r = AllRenders[i];
            if (r)
            {
                if (r.sharedMaterial)
                {
                    foreach (string str in r.sharedMaterial.shaderKeywords)
                    {
                        r.sharedMaterial.EnableKeyword(str);
                    }
                
                }
                m_OrginalMat.Add(r, r.sharedMaterials);
                Material[] mats = r.materials;
                int l = mats.Length;
                for (int j = 0; j < l; j++)
                {
                    mats[j].shader = alphaShader;
                }
                m_AlphaMat.Add(r, mats);
                r.sharedMaterials = m_OrginalMat[r];

                ////分开运行时和编辑器，因为编辑器下就改sharedmaterial会导致材质修改，
                ////进一步导致svn修改
                //if(AssetBundleManager.isOnRunTimeMode)
                //{
                //    //让人物渲染队列滞后些，以便ssao后处理先对场景实施，
                //    foreach (Material mat in r.sharedMaterials)
                //    {
                //        if (mat)
                //        {
                //            if (SkinType == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                //            {
                //                mat.renderQueue = 3000;
                //            }
                //            else
                //            {
                //                mat.renderQueue = -1;
                //            }
                //        }
                //    }
                //}
                //else
                //{
                //    //让人物渲染队列滞后些，以便ssao后处理先对场景实施，
                //    foreach (Material mat in r.materials)
                //    {
                //        if (mat)
                //        {
                //            if (SkinType == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                //            {
                //                mat.renderQueue = 3000;
                //            }
                //            else
                //            {
                //                mat.renderQueue = -1;
                //            }
                //        }
                //    }
                //}
            }
            
        }

    }

}

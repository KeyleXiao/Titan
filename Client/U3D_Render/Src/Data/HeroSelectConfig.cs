/*******************************************************************
** 文件名:	HeroSelectConfig.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/3/19
** 版  本:	1.0
** 描  述:	英雄选择界面配置信息
** 应  用:  
 *          用于英雄选择界面时，显示那些英雄
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class HeroSelectConfig : ScriptableObject
{
    /// <summary>
    /// 英雄
    /// </summary>
    public List<GameObject> Heros = new List<GameObject>();

    /// <summary>
    /// 英雄对应的ID
    /// </summary>
    public List<int> HeroIDs = new List<int>();

    /// <summary>
    /// 英雄角度
    /// </summary>
    public List<Vector3> HerosRot = new List<Vector3>();

    /// <summary>
    /// 动画列表
    /// </summary>
    public List<string> AnimationName = new List<string>();

    /// <summary>
    /// 动画长度
    /// </summary>
    public List<string> AnimationLength = new List<string>();

    /// <summary>
    /// 选择英雄的位置
    /// </summary>
    public Vector3 SelectedPos = new Vector3(0, -1.0f, 3.3f);

    public const string HerosSplitKey = "$";

    public Dictionary<int, GameObject> HeroSet
    {
        get
        {
            return m_HerosSet;
        }
    }

    Dictionary<int, GameObject> m_HerosSet = new Dictionary<int, GameObject>();
    Dictionary<int, Vector3> m_HerosRotSet = new Dictionary<int, Vector3>();

    bool bInit = false;

    public void Init()
    {
        m_HerosSet.Clear();
        m_HerosRotSet.Clear();
        for (int i = 0; i < Heros.Count; i++)
        {
            m_HerosSet.Add(HeroIDs[i], Heros[i]);
            m_HerosRotSet.Add(HeroIDs[i], HerosRot[i]);
            
        }
        bInit = true;
    }

    public void UpdateData()
    {
        m_HerosSet.Clear();
        m_HerosRotSet.Clear();
        for (int i = 0; i < Heros.Count; i++)
        {
            m_HerosSet.Add(HeroIDs[i], Heros[i]);
            m_HerosRotSet.Add(HeroIDs[i], HerosRot[i]);

        }
    }
    public Vector3 GetHeroRot(int id)
    {
        if (!bInit)
        {
            Init();
        }

        if (m_HerosRotSet.ContainsKey(id))
        {
            return m_HerosRotSet[id];
        }
        else
        {
            Trace.LogWarning("找不到ID：" + id + "，请检查配置是否与英雄选择界面的ID相对应");
            return Vector3.zero;
        }
    }

    /// <summary>
    /// 取得英雄预制
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public GameObject GetHeroPrefabId(int id)
    {
        if (!bInit)
        {
            Init();
        }

        if (m_HerosSet.ContainsKey(id))
        {
            return m_HerosSet[id];
        }
        else
        {
            Trace.LogWarning("找不到ID：" + id + "，请检查配置是否与英雄选择界面的ID相对应");
            return null;
        }
    }

    /// <summary>
    /// 取得所有英雄预制
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public GameObject[] GetAllHeroPrefab()
    {
        if (!bInit)
        {
            Init();
        }
       
        GameObject[] tempArray = new GameObject[m_HerosSet.Values.Count];
        m_HerosSet.Values.CopyTo(tempArray,0);
        return tempArray;
    }

    /// <summary>
    /// 取得所有英雄预制的ID
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public int[] GetAllHeroID()
    {
        if (!bInit)
        {
            Init();
        }

        int[] tempArray = new int[m_HerosSet.Keys.Count];
        m_HerosSet.Keys.CopyTo(tempArray, 0);
        return tempArray;
    }


    public  void DecodeData(Dictionary<int ,List<string>> AniNameTable ,Dictionary<int,List<string>> AniLengthTable)
    {
        if (null == AniNameTable ||
            null == AniLengthTable
            )
        {
            return ;
        }

        List<int> heroids = HeroIDs;
        List<string> AniName = AnimationName;
        List<string> AniLength = AnimationLength;

        int heroIndex = 0;

        AniNameTable.Add(heroids[heroIndex], new List<string>());
        AniLengthTable.Add(heroids[heroIndex], new List<string>());

        int currHeroID = heroids[heroIndex];
        for (int i = 0; i < AniName.Count; i++)
        {
            if (AniName[i].Equals(HerosSplitKey))
            {
                if (0 == heroIndex)
                {
                    heroIndex++;
                }
                else
                {
                    
                    currHeroID = heroids[heroIndex];
                    AniNameTable.Add(currHeroID, new List<string>());
                    AniLengthTable.Add(currHeroID, new List<string>());
                    heroIndex++;
                }

            }
            else
            {
                AniNameTable[currHeroID].Add(AniName[i]);
                AniLengthTable[currHeroID].Add(AniLength[i]);
            }
        }
    }
}

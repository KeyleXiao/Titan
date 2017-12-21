using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using UnityEditor;
using UnityEngine;

/// <summary>
/// Glow材质编辑器
/// </summary>
public class GlowMatInspector : MaterialEditor
{
    private int activeGlowModeIdx = 0;
    private int activeGlowMultiplierIdx = 0;

    private List<string> GlowModeList = new List<string>();
    private List<string> activeGlowMultiplierList = new List<string>();

    /// <summary>
    /// Key为显示的名字，Value为shader里的KeyWorld
    /// </summary>
    private Dictionary<string, string> GlowModeKeyWorldMap = new Dictionary<string, string>();

    /// <summary>
    /// Key为显示的名字，Value为shader里的KeyWorld
    /// </summary>
    private Dictionary<string, string> GlowGlowMultiplierKeyWorldMap = new Dictionary<string, string>();


    Material m_Material;
    private void Init()
    {
        GlowModeList.Add("Base Texture");
        GlowModeKeyWorldMap.Add("Base Texture", "GLOW_MAINTEX");

        GlowModeList.Add("Main Color");
        GlowModeKeyWorldMap.Add("Main Color", "GLOW_MAINCOLOR");

        GlowModeList.Add("Glow Texture");
        GlowModeKeyWorldMap.Add("Glow Texture", "GLOW_GLOWTEX");

        GlowModeList.Add("Glow Color");
        GlowModeKeyWorldMap.Add("Glow Color", "GLOW_GLOWCOLOR");

        GlowModeList.Add("Vertex Color");
        GlowModeKeyWorldMap.Add("Vertex Color", "GLOW_VERTEXCOLOR");


        activeGlowMultiplierList.Add("None");
        GlowGlowMultiplierKeyWorldMap.Add("None", "NO_MULTIPLY");

        activeGlowMultiplierList.Add("Glow Color");
        GlowGlowMultiplierKeyWorldMap.Add("Glow Color", "MULTIPLY_GLOWCOLOR");

        activeGlowMultiplierList.Add("Vertex Color");
        GlowGlowMultiplierKeyWorldMap.Add("Vertex Color", "MULTIPLY_VERT");

        activeGlowMultiplierList.Add("Vertex Alpha");
        GlowGlowMultiplierKeyWorldMap.Add("Vertex Alpha", "MULTIPLY_VERT_ALPHA");

        activeGlowMultiplierList.Add("Base Texture (A)");
        GlowGlowMultiplierKeyWorldMap.Add("Base Texture (A)", "MULTIPLY_MAINTEX_ALPHA");
        if(m_Material.shader.name.EndsWith("_Mask"))
        {
            m_Material.EnableKeyword("USE_TEXTURE_MASK");
        }
        else
        {
            m_Material.DisableKeyword("USE_TEXTURE_MASK");
        }
    }

    public override void OnEnable()
    {
        base.OnEnable();
        m_Material = target as Material;
        Init();
        

    }

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        if (m_Material.shader.name.EndsWith("_Mask"))
        {
            m_Material.EnableKeyword("USE_TEXTURE_MASK");
        }
        else
        {
            m_Material.DisableKeyword("USE_TEXTURE_MASK");
        }
        string[] keyWorlds = m_Material.shaderKeywords;

        foreach (string s in keyWorlds)
        {
            for (int i = 0; i < GlowModeList.Count; i++)
            {
                if (GlowModeKeyWorldMap[GlowModeList[i]].Equals(s))
                {
                    activeGlowModeIdx = i;
                    break;
                }
            }

            for (int i = 0; i < activeGlowMultiplierList.Count; i++)
            {
                if (GlowGlowMultiplierKeyWorldMap[activeGlowMultiplierList[i]].Equals(s))
                {
                    activeGlowMultiplierIdx = i;
                    break;
                }
            }
        }


        activeGlowModeIdx = EditorGUILayout.Popup("Glow Source", activeGlowModeIdx, GlowModeList.ToArray());
        activeGlowMultiplierIdx = EditorGUILayout.Popup("Glow Multiplier", activeGlowMultiplierIdx, activeGlowMultiplierList.ToArray());

        foreach (string s in GlowModeList)
        {
            if (s.Equals(GlowModeList[activeGlowModeIdx]))
            {
                m_Material.EnableKeyword(GlowModeKeyWorldMap[s]);
            }
            else
            {
                m_Material.DisableKeyword(GlowModeKeyWorldMap[s]);
            }
        }

        foreach (string s in activeGlowMultiplierList)
        {
            if (s.Equals(activeGlowMultiplierList[activeGlowMultiplierIdx]))
            {
                m_Material.EnableKeyword(GlowGlowMultiplierKeyWorldMap[s]);
            }
            else
            {
                m_Material.DisableKeyword(GlowGlowMultiplierKeyWorldMap[s]);
            }
        }
        //m_Material.DisableKeyword("GLOW_DISSLOVE");
        if (GUI.changed)
        {
            EditorUtility.SetDirty(m_Material);
        }

    }
}


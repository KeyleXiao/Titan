using System;
using UnityEngine;
using UnityEditor;

/// <summary>
/// Á÷¹â²ÄÖÊUI
/// </summary>
public class FlowLight_Self_Illumin_Bumped_SpecularShaderGUI : ShaderGUI
{
	public override void OnGUI (MaterialEditor materialEditor, MaterialProperty[] props)
	{
        base.OnGUI(materialEditor, props);

        materialEditor.LightmapEmissionProperty(0);
        foreach (Material material in materialEditor.targets)
        {
            material.globalIlluminationFlags &= ~MaterialGlobalIlluminationFlags.EmissiveIsBlack;
        }

	}

}
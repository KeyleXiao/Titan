#if (UNITY_4_0 || UNITY_4_0_1 || UNITY_4_1 || UNITY_4_2 || UNITY_4_3 || UNITY_4_5 || UNITY_4_6)
#define UNITY_4_X
#else
#define UNITY_5_X
#endif

using UnityEngine;
using UnityEditor;
using ImageEffects;
[CustomEditor(typeof(SSAOPro))]
public class SSAOProEditor : Editor
{
	static GUIContent[] c_aoModes = { new GUIContent("1.1 (Legacy)"), new GUIContent("1.2 (Recommended)") };

#if UNITY_4_X
	SerializedProperty p_useHighPrecisionDepthMap;
#endif

	SerializedProperty p_samples;
	SerializedProperty p_downsampling;
	SerializedProperty p_radius;
	SerializedProperty p_intensity;

    SerializedProperty p_ambientOnly;
    SerializedProperty p_forceForwardCompatibility;

	SerializedProperty p_debugAO;

    public void OnEnable()
	{

#if UNITY_4_X
		//p_useHighPrecisionDepthMap = serializedObject.FindProperty("UseHighPrecisionDepthMap");
#endif

        p_samples = serializedObject.FindProperty("sampleCount");
        p_downsampling = serializedObject.FindProperty("Downsampling");
		p_radius = serializedObject.FindProperty("Radius");
        p_intensity = serializedObject.FindProperty("Intensity");

        p_forceForwardCompatibility = serializedObject.FindProperty("ForceForwardCompatibility");
        p_ambientOnly = serializedObject.FindProperty("AmbientOnly");

		p_debugAO = serializedObject.FindProperty("DebugAO");
	}

    public void OnDisable()
    {
    }

	public override void OnInspectorGUI()
	{
		serializedObject.Update();

		EditorGUILayout.PropertyField(p_samples, new GUIContent("Sample Count", "Number of ambient occlusion samples (higher is slower)"));
		EditorGUILayout.PropertyField(p_downsampling, new GUIContent("Downsampling", "The resolution at which calculations should be performed (for example, a downsampling value of 2 will work at half the screen resolution)"));
		EditorGUILayout.PropertyField(p_intensity, new GUIContent("Intensity", "Occlusion multiplier (degree of darkness added by ambient occlusion)"));
		EditorGUILayout.PropertyField(p_radius, new GUIContent("Radius", "Sampling radius (in world unit)"));

        EditorGUILayout.PropertyField(p_ambientOnly, new GUIContent("Ambient Only", "Enables the ambient-only mode in that the effect only affects ambient lighting. This mode is only available with the Deferred rendering path and HDR rendering."));
        EditorGUILayout.PropertyField(p_forceForwardCompatibility, new GUIContent("Force Forward Rendering Compatibility", "Forces compatibility with Forward rendered objects when working with the Deferred rendering path."));
        

		p_debugAO.boolValue = GUILayout.Toggle(p_debugAO.boolValue, "Show AO", EditorStyles.miniButton);

		serializedObject.ApplyModifiedProperties();
	}
}

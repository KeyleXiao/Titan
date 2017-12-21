using UnityEngine;
using System.Collections;

public class AssetBundleDebug : MonoBehaviour
{

	// Use this for initialization
	void Awake () 
    {
        if(!OptimitzedControlPlane.Instance)
        {
            OptimitzedControlPlane.Instance = GetComponent<OptimitzedControlPlane>();
        }

        AssetBundleManager.Init();

	}
    //public void OnGUI()
    //{
    //    GUILayout.BeginVertical();
    //    string[] names = QualitySettings.names;
    //    for (var i = 0; i < names.Length; i++)
    //    {
    //        if (GUILayout.Button(names[i]))
    //        {
    //            QualitySettings.SetQualityLevel(i, true);
    //        }
    //    }
    //    GUILayout.EndVertical();

    //}


    public void OnApplicationQuit()
    {

        //清除资源
        AssetBundleManager.Clear();
    }
}

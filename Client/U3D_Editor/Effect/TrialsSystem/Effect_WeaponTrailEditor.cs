using UnityEngine;
using System.Collections;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_WeaponTrail))]
    public class Effect_WeaponTrailEditor : Editor
    {
        Effect_WeaponTrail weap = null;
        public void OnEnable()
        {
            weap = target as Effect_WeaponTrail;
        }
        public override void OnInspectorGUI()
        {
            GUILayout.Space(10);
            DrawDefaultInspector();
            //if (GUILayout.Button("Play"))
            //{
            //    weap.Play();
            //}
            //if (GUILayout.Button("Stop"))
            //{
            //    weap.Stop();
            //}
        }
    }

}



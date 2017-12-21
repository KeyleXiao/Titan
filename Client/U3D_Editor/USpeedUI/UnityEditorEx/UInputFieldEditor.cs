using System;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;

namespace UnityEditor.UI
{
    [CustomEditor(typeof(InputField), true)]
    [CanEditMultipleObjects]
    public class UInputFieldEditor : InputFieldEditor
    {

        protected override void OnEnable()
        {
            base.OnEnable();
            UInputFieldIMEToggle imeToggle = (target as InputField).gameObject.GetComponent<UInputFieldIMEToggle>();
            if (imeToggle == null)
                imeToggle = (target as InputField).gameObject.AddComponent<UInputFieldIMEToggle>();
        }
    }
}

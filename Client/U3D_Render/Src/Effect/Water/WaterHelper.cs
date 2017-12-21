using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Rendering;

public class WaterHelper : MonoBehaviour
{
    public Water hostWater = null;
    public Camera hostCamera = null;
    float OldDistance = 0;
    int oldPixelLightCount = 0;
    public void OnPreCull()
    {
        if (hostCamera.enabled && hostWater)
        {
            OldDistance = QualitySettings.shadowDistance;
            QualitySettings.shadowDistance = 0;

            oldPixelLightCount = QualitySettings.pixelLightCount;
            QualitySettings.pixelLightCount = 0;
        }
    }

    public void OnPostRender()
    {
        if (hostCamera.enabled && hostWater)
        {
            QualitySettings.shadowDistance = OldDistance;
            QualitySettings.pixelLightCount = oldPixelLightCount;
        }

    }
    public void OnDestroy()
    {
        hostWater = null;
        hostCamera = null;
    }
}

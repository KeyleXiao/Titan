using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[System.Serializable]
public class GemBaseConfig : ScriptableObject
{
    public Mesh[] GemMeshes;


    //球形
    public Material SphereLv1;
    public Material SphereLv2;
    public Material SphereLv3;
    public Material SphereLv4;
    public Material SphereLv5;

    //椭球形
    public Material OvalLv1;
    public Material OvalLv2;
    public Material OvalLv3;
    public Material OvalLv4;
    public Material OvalLv5;

    //粗糙的方体钻石
    public Material LowSquareDiamon;
    //粗糙的球体钻石
    public Material LowSphereDiamon;

    //精细的方体钻石
    public Material HighSquareDiamonLv4;
    public Material HighSquareDiamonLv5;

    //精细的球体钻石
    public Material HighSphereDiamonLv4;
    public Material HighSphereDiamonLv5;

    public Cubemap evnCubeMap;

    public Texture2D[] AdditiveTexs;
    public Texture2D[] AdditiveNormalTexs;

    public Texture2D[] BaseTexs;

    public Color[] Colors;
    public Color[] DecalColors;

    public GameObject[] PrefabInstances;
    public GameObject[] PrefabInstances_Particle;
    [SerializeField]
    public string materialsPath = "Assets/";
    //public string ...

    public Vector2 cellspace = new Vector2(3,3);
    public Vector2 tilespace = new Vector2(25, 25);

    public bool isColDependParticle = false;    //颜色是否依赖特效

    public GemBaseConfig()
    {
        GemMeshes = new Mesh[6];
        AdditiveTexs = new Texture2D[5];
        AdditiveNormalTexs = new Texture2D[5];
        Colors = new Color[5];
        DecalColors = new Color[5];
        BaseTexs = new Texture2D[25];
        PrefabInstances = new GameObject[5];
        PrefabInstances_Particle = new GameObject[17];
        materialsPath = "Assets/";
    }

    public List<Material> GetTempMaterialList()
    {
        List<Material> matList = new List<Material>();
        matList.Add(SphereLv1);
        matList.Add(SphereLv2);
        matList.Add(SphereLv3);
        matList.Add(SphereLv4);
        matList.Add(SphereLv5);

        matList.Add(OvalLv1);
        matList.Add(OvalLv2);
        matList.Add(OvalLv3);
        matList.Add(OvalLv4);
        matList.Add(OvalLv5);

        matList.Add(LowSquareDiamon);
        matList.Add(LowSphereDiamon);

        matList.Add(HighSquareDiamonLv4);
        matList.Add(HighSquareDiamonLv5);

        matList.Add(HighSphereDiamonLv4);
        matList.Add(HighSphereDiamonLv5);
        return matList;
    }

    public int CheckConfig()
    {
        if (SphereLv1 == null)
            return -1;
        if (SphereLv2 == null)
            return -2;
        if (SphereLv3 == null)
            return -3;
        if (SphereLv4 == null)
            return -4;
        if (SphereLv5 == null)
            return -5;
        if (OvalLv1 == null)
            return -6;
        if (OvalLv2 == null)
            return -7;
        if (OvalLv3 == null)
            return -8;
        if (OvalLv4 == null)
            return -9;
        if (OvalLv5 == null)
            return -10;
        if (LowSquareDiamon == null)
            return -11;
        if (LowSphereDiamon == null)
            return -12;
        if (HighSquareDiamonLv4 == null)
            return -13;
        if (HighSquareDiamonLv5 == null)
            return -14;
        if (HighSphereDiamonLv4 == null)
            return -15;
        if (HighSphereDiamonLv5 == null)
            return -16;
        if (evnCubeMap == null)
            return -17;
        for (int i = 0; i < 5; i++)
        {
            if (AdditiveTexs[i] == null)
                return -18;
            if (AdditiveNormalTexs[i] == null)
                return -19;
            if (Colors[i] == null)
                return -20;
            if (DecalColors[i] == null)
                return -21;
            if(!isColDependParticle)
                if(PrefabInstances[i]==null)
                    return -22;
        }
        if (isColDependParticle)
            for (int i = 0; i < 17; i++)
                if (PrefabInstances_Particle[i] == null)
                     return -23;
        for (int i = 0; i < 6; i++)
            if (GemMeshes[i] == null)
                return -24;
        for (int i = 0; i < 25; i++)
            if (BaseTexs[i] == null)
                return -25;
        return 1;
    }

    public Texture2D ChooseTheAdditiveTex(GemDecalType decalTpye)
    {
        return AdditiveTexs[(int)decalTpye];
    }

    public Texture2D ChooseTheAdditiveNormalTex(GemDecalType decalTpye)
    {
        return AdditiveNormalTexs[(int)decalTpye];
    }

    public Material ChooseTheMat(int level, GemMeshType meshType)
    {
        Material result = null;
        switch (meshType)
        {
            case GemMeshType.Sphere:
                if (level == 1) result = new Material(SphereLv1);
                if (level == 2) result = new Material(SphereLv2);
                if (level == 3) result = new Material(SphereLv3);
                if (level == 4) result = new Material(SphereLv4);
                if (level == 5) result = new Material(SphereLv5);
                break;
            case GemMeshType.Oval:
                if (level == 1) result = new Material(OvalLv1);
                if (level == 2) result = new Material(OvalLv2);
                if (level == 3) result = new Material(OvalLv3);
                if (level == 4) result = new Material(OvalLv4);
                if (level == 5) result = new Material(OvalLv5);
                break;
            case GemMeshType.LowSquareDiamon:
                if (level == 3) result = new Material(LowSquareDiamon);
                break;
            case GemMeshType.LowSphereDiamon:
                if (level == 3) result = new Material(LowSphereDiamon);
                break;
            case GemMeshType.HighSphereDiamon:
                if (level == 4) result = new Material(HighSphereDiamonLv4);
                if (level == 5) result = new Material(HighSphereDiamonLv5);
                break;
            case GemMeshType.HighSquareDiamon:
                if (level == 4) result = new Material(HighSquareDiamonLv4);
                if (level == 5) result = new Material(HighSquareDiamonLv5);
                break;
            default:
                result = new Material(SphereLv1);
                break;
        }
        return result;
    }
}
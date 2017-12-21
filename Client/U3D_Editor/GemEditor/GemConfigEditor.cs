using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
using System.IO;

[CustomEditor(typeof(GemBaseConfig))]
public class GemConfigCreator : Editor
{
    private GemBaseConfig cfg;
    [MenuItem("GemSystem/Create Config")]
    static void CreateMaterial()
    {
        // Create a simple material asset

        GemBaseConfig config = ScriptableObject.CreateInstance<GemBaseConfig>();
        Debug.Log(config);
        AssetDatabase.CreateAsset(config, "Assets/GemBaseConfig.asset");

        // Print the path of the created asset
        AssetDatabase.Refresh();
    }

    private string[] additiveMsgs = new string[]
    {
        "A",
        "B",
        "C",
        "D",
        "E",
    };

    private string[] meshMsgs = new string[]
    {
        "Sphere",
        "Oval",
        "LowSquareDiamon",
        "LowSphereDiamon",
        "HighSquareDiamon",
        "HighSphereDiamon",
    };

    private List<string> generatedMats = new List<string>();

    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();
        cfg = (GemBaseConfig)target;

        for (int i = 0; i < 6; i++)
        {
            cfg.GemMeshes[i] = AssetField<Mesh>(meshMsgs[i], cfg.GemMeshes[i]);
        }

        cfg.SphereLv1 = AssetField<Material>("球形宝石Lv1", cfg.SphereLv1);
        cfg.SphereLv2 = AssetField<Material>("球形宝石Lv2", cfg.SphereLv2);
        cfg.SphereLv3 = AssetField<Material>("球形宝石Lv3", cfg.SphereLv3);
        cfg.SphereLv4 = AssetField<Material>("球形宝石Lv4", cfg.SphereLv4);
        cfg.SphereLv5 = AssetField<Material>("球形宝石Lv5", cfg.SphereLv5);
        cfg.OvalLv1 = AssetField<Material>("椭球形宝石Lv1", cfg.OvalLv1);
        cfg.OvalLv2 = AssetField<Material>("椭球形宝石Lv2", cfg.OvalLv2);
        cfg.OvalLv3 = AssetField<Material>("椭球形宝石Lv3", cfg.OvalLv3);
        cfg.OvalLv4 = AssetField<Material>("椭球形宝石Lv4", cfg.OvalLv4);
        cfg.OvalLv5 = AssetField<Material>("椭球形宝石Lv5", cfg.OvalLv5);
        cfg.LowSquareDiamon = AssetField<Material>("粗糙的方体钻石Lv3", cfg.LowSquareDiamon);
        cfg.LowSphereDiamon = AssetField<Material>("粗糙的球体钻石Lv3", cfg.LowSphereDiamon);
        cfg.HighSquareDiamonLv4 = AssetField<Material>("精细的方体钻石Lv4", cfg.HighSquareDiamonLv4);
        cfg.HighSquareDiamonLv5 = AssetField<Material>("精细的方体钻石Lv5", cfg.HighSquareDiamonLv5);
        cfg.HighSphereDiamonLv4 = AssetField<Material>("精细的球体钻石Lv4", cfg.HighSphereDiamonLv4);
        cfg.HighSphereDiamonLv5 = AssetField<Material>("精细的球体钻石Lv5", cfg.HighSphereDiamonLv5);
        EditorGUILayout.LabelField("环境贴图路径");
        cfg.evnCubeMap = AssetField<Cubemap>("",cfg.evnCubeMap);
        EditorGUILayout.LabelField("符文纹理设置：");
        for (int i = 0; i < 5; i++)
        {
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.BeginVertical();
            EditorGUILayout.LabelField("符文[" + additiveMsgs[i] + "]纹理");
            cfg.AdditiveTexs[i] = EditorGUILayout.ObjectField(cfg.AdditiveTexs[i], typeof(Texture2D), true) as Texture2D;
            EditorGUILayout.EndVertical();
            EditorGUILayout.BeginVertical();
            EditorGUILayout.LabelField("符文[" + additiveMsgs[i] + "]法线贴图");
            cfg.AdditiveNormalTexs[i] = EditorGUILayout.ObjectField(cfg.AdditiveNormalTexs[i], typeof(Texture2D),true) as Texture2D;
            EditorGUILayout.EndVertical();
            EditorGUILayout.EndHorizontal();
        }

        for (int i = 0; i < 5; i++)
        {
            EditorGUILayout.BeginHorizontal(GUILayout.Width(50));
            EditorGUILayout.BeginVertical();
            EditorGUILayout.LabelField("主颜色:");
            cfg.Colors[i] = EditorGUILayout.ColorField( cfg.Colors[i]);
            EditorGUILayout.EndVertical();
            EditorGUILayout.BeginVertical();
            EditorGUILayout.LabelField("符文颜色:");
            cfg.DecalColors[i] = EditorGUILayout.ColorField(cfg.DecalColors[i]);
            EditorGUILayout.EndVertical();
            EditorGUILayout.EndHorizontal();
        }

        //主纹理，依赖等级与颜色
        EditorGUILayout.LabelField("主纹理贴图:");
        for (int lv = 0; lv < 5; lv++)
        {
            EditorGUILayout.LabelField("等级" + (lv+1) + ":");
            EditorGUILayout.BeginHorizontal();// (GUILayout.Width(50));
            for (int i = 0; i < 5; i++)
            {
                cfg.BaseTexs[lv * 5 + i] = EditorGUILayout.ObjectField(cfg.BaseTexs[lv * 5 + i], typeof(Texture2D), true) as Texture2D;
            }
            EditorGUILayout.EndHorizontal();
        }

        EditorGUILayout.LabelField("展示设置：");
        cfg.tilespace = EditorGUILayout.Vector2Field("每个Tile的间隔", cfg.tilespace);
        cfg.cellspace = EditorGUILayout.Vector2Field("每个Cell的间隔", cfg.cellspace);

        cfg.isColDependParticle = EditorGUILayout.Toggle("颜色是否依赖特效：",cfg.isColDependParticle);
        if (!cfg.isColDependParticle)
        { 
            for (int i = 0; i < 5; i++)
                cfg.PrefabInstances[i] = AssetField<GameObject>("等级【" + (i + 1) + "】预置体:", cfg.PrefabInstances[i]);
        }
        else
        {
            cfg.PrefabInstances_Particle[0] = AssetField<GameObject>("等级1预置体:", cfg.PrefabInstances_Particle[0]);
            cfg.PrefabInstances_Particle[1] = AssetField<GameObject>("等级2预置体:", cfg.PrefabInstances_Particle[1]);
            for (int l = 0; l < 3;l++ )
                for (int i = 0; i < 5; i++)
                    cfg.PrefabInstances_Particle[l * 5 + i + 2] = AssetField<GameObject>("等级" + (l + 1 + 2) + "颜色【" + (i + 1) + "】预置体:", cfg.PrefabInstances_Particle[l * 5 + i + 2]);
        }
        cfg.materialsPath = EditorGUILayout.TextField("材质球生成路径：", cfg.materialsPath);
        EditorUtility.SetDirty(cfg);
        //AssetDatabase.SaveAssets();
        //Debug.Log(Application.dataPath + cfg.materialsPath.Remove(0, 6));
        if (Directory.Exists(Application.dataPath + cfg.materialsPath.Remove(0,6)) == false)
        {
            EditorGUILayout.HelpBox("材质球路径不存在！", MessageType.Error);
            return;
        }
        int r = cfg.CheckConfig();
        if (cfg.CheckConfig() > 0)
        {
            if (GUILayout.Button("一键生成"))
            {
                BuildGems();
            }
        }
        else
        {
            EditorGUILayout.HelpBox("宝石系统的配置没配好,请先处理!!: r= " + r, MessageType.Error);
            return;
        }
        if(GUILayout.Button("清除材质球"))
        {
            Debug.Log("generatedMats : " + generatedMats.Count);
            for (int i = 0; i < generatedMats.Count; i++)
                AssetDatabase.DeleteAsset(generatedMats[i]);
            AssetDatabase.Refresh();
        }

    }
    private GameObject GemsManager;
    private int gemMatID = 0;
    private void BuildGems()
    {
        generatedMats.Clear();
        gemMatID = 0;
        GemsManager = new GameObject("GemManager");
        for (int lv = 1; lv <= 5; lv++)    //5个等级
        {
            switch (lv)
            {
                case 1:
                case 2:
                    for (int colorID = 0; colorID < cfg.Colors.Length; colorID++)
                    {
                        //椭圆球
                        BuildLowLvGemObj(lv, GemMeshType.Oval, colorID);
                        //正圆球
                        BuildLowLvGemObj(lv, GemMeshType.Sphere, colorID);
                    }
                    break;
                case 3:
                    for (int colorID = 0; colorID < cfg.Colors.Length; colorID++)
                    {
                        for (int decalID = 0; decalID < (int)GemDecalType.TOTAL; decalID++)
                        {
                            //椭圆球
                            BuildHighLvGemObj(lv, GemMeshType.Oval, (GemDecalType)decalID, colorID);
                            //正圆球
                            BuildHighLvGemObj(lv, GemMeshType.Sphere, (GemDecalType)decalID, colorID);
                        }
                        //粗糙的方体钻石
                        BuildHighLvGemObj(lv, GemMeshType.LowSquareDiamon, GemDecalType.NULL, colorID);
                        //粗糙的球体钻石
                        BuildHighLvGemObj(lv, GemMeshType.LowSphereDiamon, GemDecalType.NULL, colorID);
                    }

                    break;
                case 4:
                case 5:
                    for (int colorID = 0; colorID < cfg.Colors.Length; colorID++)
                    {
                        for (int decalID = 0; decalID < (int)GemDecalType.TOTAL; decalID++)
                        {
                            //椭圆球
                            BuildHighLvGemObj(lv, GemMeshType.Oval, (GemDecalType)decalID, colorID);
                            //正圆球
                            BuildHighLvGemObj(lv, GemMeshType.Sphere, (GemDecalType)decalID, colorID);
                        }
                        //精致的方体钻石
                        BuildHighLvGemObj(lv, GemMeshType.HighSquareDiamon, GemDecalType.NULL, colorID);
                        //精致的球体钻石
                        BuildHighLvGemObj(lv, GemMeshType.HighSphereDiamon, GemDecalType.NULL, colorID);
                    }
                    break;
            }
        }
    }

    private GameObject BuildLowLvGemObj(int level, GemMeshType meshType, int colID)
    {
        GameObject gem;
        if (cfg.isColDependParticle)
        {
            gem = GameObject.Instantiate(cfg.PrefabInstances_Particle[level - 1]);
        }
        else
        {
            gem = GameObject.Instantiate(cfg.PrefabInstances[level - 1]);
        }
        gem.name = "Gem_Lv" + level+"_" + meshType.ToString() + "_" + colID+ "_NoDecal";
        GemStone stone = gem.transform.GetChild(0).FindChild("GemObj").gameObject.AddComponent<GemStone>();
        stone.meshType = meshType;
        stone.GetComponent<MeshFilter>().mesh = cfg.GemMeshes[(int)meshType];
        stone.level = level;
        if (stone.Material == null)
        {
            //Material tempM = DrawMaterialList(_stone.material, cfg.GetTempMaterialList());
            Material tempM = cfg.ChooseTheMat(stone.level, stone.meshType);
            AssetDatabase.CreateAsset(tempM, cfg.materialsPath + "/Mat_Lv" + level + "_" + meshType.ToString() + "_" + colID + "_" + "DecalNULL" + ".mat");
            stone.Material = tempM;
            stone.Material = AssetField<Material>("Material", stone.Material);
            generatedMats.Add(cfg.materialsPath + "/Mat_Lv" + level + "_" + meshType.ToString() + "_" + colID + "_" + "DecalNULL" + ".mat");
        }
        if(meshType==GemMeshType.Oval ||meshType==GemMeshType.Sphere){
            stone.GemColor = cfg.Colors[colID];
            stone.DecalColor = new Color(0, 0, 0, 0);
            stone.MainTex = cfg.BaseTexs[(level - 1) * 5 + colID];
        }
        gem.transform.SetParent(GemsManager.transform);
        //计算位置：
        gem.transform.localPosition = GetTheGemPos(level - 1, meshType, GemDecalType.NULL, colID);
        stone.materialID = gemMatID;
        gemMatID++;
        return gem;
    }
    
    private GameObject BuildHighLvGemObj(int level, GemMeshType meshType, GemDecalType decalType, int colID)
    {
        GameObject gem;
        if (cfg.isColDependParticle)
        {
            gem = GameObject.Instantiate(cfg.PrefabInstances_Particle[(level - 3)*5+colID + 2]);
        }
        else
        {
            gem = GameObject.Instantiate(cfg.PrefabInstances[level - 1]);
        }
        string DecalType = decalType== GemDecalType .NULL? "_NoDecal" : "_Decal" + (int)decalType;
        gem.name = "Gem_Lv" + level + "_" + meshType.ToString() + "_" + colID + DecalType;
        GemStone stone = gem.transform.GetChild(0).FindChild("GemObj").gameObject.AddComponent<GemStone>();
        stone.meshType = meshType;
        stone.GetComponent<MeshFilter>().mesh = cfg.GemMeshes[(int)meshType];
        stone.level = level;
        if (stone.Material == null)
        {
            //Material tempM = DrawMaterialList(_stone.material, cfg.GetTempMaterialList());
            Material tempM = cfg.ChooseTheMat(stone.level, stone.meshType);
            AssetDatabase.CreateAsset(tempM, cfg.materialsPath + "/Mat_Lv" + level + "_" + meshType.ToString() + "_" + colID + DecalType + ".mat");
            stone.Material = tempM;
            stone.Material = AssetField<Material>("Material", stone.Material);
            generatedMats.Add(cfg.materialsPath + "/Mat_Lv" + level + "_" + meshType.ToString() + "_" + colID + DecalType + ".mat");
        }
        if (stone.meshType == GemMeshType.Oval || stone.meshType == GemMeshType.Sphere)
        {
            stone.GemColor = cfg.Colors[colID];
            stone.decalType = decalType;
            if (decalType != GemDecalType.NULL)
            {
                stone.DecalColor = cfg.DecalColors[colID];
                stone.DecalTex = cfg.AdditiveTexs[(int)decalType];
                stone.BumpMap = cfg.AdditiveNormalTexs[(int)decalType];
            }
            else
            {
                stone.DecalColor = new Color(0, 0, 0, 0);
            }
            stone.MainTex = cfg.BaseTexs[(level - 1) * 5 + colID];
        }
        gem.transform.SetParent(GemsManager.transform);

        //计算位置：
        gem.transform.localPosition = GetTheGemPos(level - 1, meshType, decalType, colID);
        gemMatID++;
        return gem;
    }

    private Vector3 GetTheGemPos(int level, GemMeshType meshType, GemDecalType decalType, int colID)
    {

        //分成5（等级）*6（形状）个Tile
        Vector3 startPos = new Vector3(
            (int)meshType * cfg.tilespace.x,
            -level * cfg.tilespace.y, 0);
        //每个Tile分成5（符文种类颜色）*5（颜色）个Cell
        float yoffset = 0;
        if (decalType == GemDecalType.NULL)
            yoffset = 2 * cfg.cellspace.y;
        else
            yoffset = (int)decalType * cfg.cellspace.y;
        Vector3 offset = new Vector3(
            colID * cfg.cellspace.x, -yoffset, 0);

        return startPos + offset;
    }


    //private GameObject BuildLvXGemObj(int level, GemMeshType meshType, GemDecalType decalType, Color mainCol, Color decalColor)
    //{
    //    GameObject gem = new GameObject();
    //    GemStone stone = gem.AddComponent<GemStone>();
    //    stone.meshType = GemMeshType.Oval;
    //    stone.level = 1;
    //    return gem;
    //}

    private static T AssetField<T>(string label, T obj) where T : Object
    {
        return (T)EditorGUILayout.ObjectField(label, (T)obj, typeof(T), false,GUILayout.Width(400));
    }

}

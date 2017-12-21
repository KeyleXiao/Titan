using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System.Reflection;

[CustomEditor(typeof(GemStone))]
public class GemStoneEditor : Editor {
    private GemStone _stone;
    void OnEnable() {

    }

    public override void OnInspectorGUI() {
        return;     //有bug..
        //_stone = (GemStone)target;
        //_stone.gemsConfig = AssetField<GemBaseConfig>("Config: ", _stone.gemsConfig);
        //if (_stone.gemsConfig == null)
        //    return;
        //int r = _stone.gemsConfig.CheckConfig();
        //if(r<0)
        //{
        //    EditorGUILayout.HelpBox("宝石系统的配置没配好,请先处理!!: r= " + r, MessageType.Error);
        //    return;
        //}
        //bool isResetMaterial = false;
        //_stone.level = EditorGUILayout.Popup("宝石等级", _stone.level, new string[] { "1", "2", "3", "4", "5" });
        //_stone.meshType = (GemMeshType)EditorGUILayout.EnumPopup("宝石形状", _stone.meshType);
        //if (GUILayout.Button("重置材质球"))
        //    isResetMaterial = true;

        //if (_stone.Material == null || isResetMaterial) {
        //    //Material tempM = DrawMaterialList(_stone.material, _stone.gemsConfig.GetTempMaterialList());
        //    Material tempM = _stone.gemsConfig.ChooseTheMat(_stone.level,_stone.meshType);
        //    AssetDatabase.CreateAsset(tempM, "Assets/" + _stone.gameObject.name + "_mat_" + _stone.materialID + ".mat");
        //    _stone.Material = tempM;
        //    _stone.Material = AssetField<Material>("Material", _stone.Material);
        //}
        //if (_stone.Material == null)
        //    return;
        //EditorGUILayout.Separator();

        ////只有球体和椭球体才需要控制
        //if (_stone.meshType == GemMeshType.Oval || _stone.meshType == GemMeshType.Sphere)
        //{
        //    //主纹理
        //    //_stone.MainTex = _stone.gemsConfig.DrawMaterialList(_stone.material, _stone.gemsConfig.GetTempMaterialList());
        //    _stone.MainTex = AssetField<Texture2D>("主纹理：", _stone.MainTex);

        //    //Cubemap
        //    //_stone.material = _stone.gemsConfig.DrawMaterialList(_stone.material, _stone.gemsConfig.GetTempMaterialList());
        //    _stone.evnCubeMap = AssetField<Cubemap>("环境贴图", _stone.evnCubeMap);
        //    EditorGUILayout.Separator();

        //    //颜色控制：高光颜色，反射颜色，宝石颜色
        //    _stone.GemColor = EditorGUILayout.ColorField("宝石颜色：", _stone.GemColor);
        //    _stone.SpecColor = EditorGUILayout.ColorField("高光颜色：", _stone.SpecColor);
        //    _stone.ReflectColour = EditorGUILayout.ColorField("反射颜色：", _stone.ReflectColour);

        //    EditorGUILayout.Separator();


        //    //等级大于2才有符文纹理
        //    if (_stone.level > 2)
        //    {
        //        _stone.decalType = (GemDecalType)EditorGUILayout.EnumPopup("符文类型", _stone.decalType);

        //        //符文纹理：
        //        _stone.DecalTex = _stone.gemsConfig.ChooseTheAdditiveTex(_stone.decalType);
        //        _stone.DecalTex = AssetField<Texture2D>("符文贴图：", _stone.DecalTex);
        //        _stone.DecalColor = EditorGUILayout.ColorField("符文颜色：", _stone.DecalColor);
        //    }
        //    //等级大于3才有法线
        //    if (_stone.level > 3)
        //    {
        //        _stone.BumpMap = _stone.gemsConfig.ChooseTheAdditiveNormalTex(_stone.decalType);
        //        _stone.BumpMap = AssetField<Texture2D>("法线贴图：", _stone.BumpMap);
        //    }
        //}
        //EditorUtility.SetDirty(_stone);
    }

    private static Material DrawMaterialList(Material material, List<Material> list)
    {
        string[] names = new string[list.Count];
        for (int i = 0; i < list.Count; i++)
        {
            names[i] = list[i].name;
        }

        int selected = list.IndexOf(material);
        selected = EditorGUILayout.Popup("Material", selected, names);
        if (selected != -1) return list[selected];
        return null;
    }

    private static T AssetField<T>(string label, T obj) where T : Object
    {
        return (T)EditorGUILayout.ObjectField(label, (T)obj, typeof(T), false);
    }
}
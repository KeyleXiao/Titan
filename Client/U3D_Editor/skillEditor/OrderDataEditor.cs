using UnityEngine;
using UnityEditor;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.IO;

[CustomEditor(typeof(OrderDataSet))]
public class OrderDataEditor_Editor : CustomInspector<OrderDataSet>
{
    int bLoadedSkill;

    // 自定义创建SkillData对象的菜单
    [MenuItem("Assets/Create/OrderDataSet")]
    public static void Create()
    {
        CreateAsset();
        //EffectViewFactory.Instance.Start();
    }

    public override void OnMemberGUI(MemberInfo info)
    {
        //if (info.Name == "nSkillID")
        //{
        //    EffectViewData skill = EffectViewFactory.Instance.GetEffectViewData(dataInstance.nSkillID);
        //    if (skill == null && bLoadedSkill != dataInstance.nSkillID)
        //    {
        //        EffectViewFactory.Instance.Start();
        //        bLoadedSkill = dataInstance.nSkillID;
        //    }

        //    skill = (EffectViewData)EditorGUILayout.ObjectField("Skill:", skill, typeof(EffectViewData), false);
        //    if (skill!=null)
        //        dataInstance.nSkillID = skill.n_id;
        //    return;
        //}

        // 调用父目录的
        DefaultMemberEdit(info);//, ref dataInstance);
    }


    public override void asset2csv()
    {
        //string assetPath = "OrderSet";
        //string path = AssetDatabase.GetAssetPath(Selection.activeObject);
        //path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(Selection.activeObject)), "");
        //string csvFileName = path + "ordersetData.csv";
        //OrdersetDataManager.Instance.asset2csv(assetPath, csvFileName);
    }

    public override void manager2asset()
    {
        //string path = AssetDatabase.GetAssetPath(Selection.activeObject);
        //path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(Selection.activeObject)), "");
        //string csvFileName = path + "ordersetData.csv";
        //OrdersetDataManager.Instance.clear();
        //ScpReader.ReadCSVFile(csvFileName, false, OrdersetDataManager.Instance);
        //DataTable.SaveAction fun = saveInstance;
        //OrdersetDataManager.Instance.save2asset("OrderSet", fun);
    }

    public override void loadFromCsv()
    {
        //string path = AssetDatabase.GetAssetPath(Selection.activeObject);
        //path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(Selection.activeObject)), "");
        //string csvFileName = path + "ordersetData.csv";
        //ScpReader.ReadCSVFile(csvFileName, false, OrdersetDataManager.Instance);
    }
}

//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using System;

///// <summary>
///// 模型管理器，负责所有模型的加载
///// </summary>
//public sealed class ModelManager
//{

//    /// <summary>
//    /// 加载模型
//    /// </summary>
//    /// <param name="SkinID">模型ID</param>
//    /// <param name="strPrefabPath">模型路径</param>
//    /// <param name="ModelType">模型类型(是怪物，还是人物..)</param>
//    /// <returns>是否成功</returns>
//    public static bool LoadModel(short SkinID, string strPrefabPath,Byte ModelType)
//    {
//       // Trace.LogError("HeroID=" + HeroID);
//       // Trace.LogError("strPrefabPath=" + strPrefabPath);
//      //  Trace.LogError("ModelType=" + ModelType);

//        UnityEngine.Object obj = Resources.Load(strPrefabPath);
//        if (obj != null)
//        {
//            Dictionary<int, UnityEngine.Object> dic = null;
//            if(m_ModelMap.ContainsKey(ModelType))
//            {
//                dic = m_ModelMap[ModelType];
//            }       
//            else
//            {
//                dic = new Dictionary<int, UnityEngine.Object>();
//                m_ModelMap.Add(ModelType,dic);
//            }
//            if (dic.ContainsKey(SkinID))
//            {
//                return true;
//            }
//            dic.Add(SkinID,obj);
//            obj = null;
//            return true;
//        }
//        else
//        {
//            Trace.LogWarning("加载模型失败,ID:" + SkinID +"，模型类型：" + ModelType + ",路径:" + strPrefabPath);
//            return false;
//        }

//    }


//    /// <summary>
//    /// 取得模型，失败返回null
//    /// </summary>
//    /// <param name="ModelType">模型类型(是怪物，还是人物..)</param>
//    /// <param name="SkinID">皮肤ID</param>
//    /// <returns></returns>
//    public static UnityEngine.Object GetModel(Byte ModelType, int SkinID)
//    {
//        if (m_ModelMap.ContainsKey(ModelType))
//        {
//            Dictionary<int, UnityEngine.Object> dic = m_ModelMap[ModelType];
//            if (dic.ContainsKey(SkinID))
//                return dic[SkinID];
//            return null;
//        }
//        return null;
//    }


//    private static Dictionary<Byte, Dictionary<int, UnityEngine.Object>> m_ModelMap = new Dictionary<Byte, Dictionary<int, UnityEngine.Object>>();
//}

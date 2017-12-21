/*******************************************************************
** 文件名:	SKMeshSocketListInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/2/4
** 版  本:	1.0
** 描  述:	带骨骼模型使用插槽是需要的信息
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

/// <summary>
///  编辑器保存数据有用
///  单个socket的属性
/// </summary>
[System.Serializable]
public class SkeletalMeshSocket
{

    /// <summary>
    /// socket object 要挂接到这个插口的对象都作为他的子对象
    /// 注意：子对象的prefab.transform一定要初始化，不作调整，只调整socket object的transform
    /// 
    /// 有个问题就是改了SocketObjectName 但SkeletalMeshSocket.name不会被修改；但也无关紧要，获取的时候根据SocketObjectName获取 
    /// </summary>
    public GameObject SocketGameObject;
    /// <summary>
    /// 插口名字，根据改名字在socket list中查询
    /// </summary>
    public string SocketObjectName;
    //public Mesh SlotMesh;
    //public Material SlotMeshMat;
    //public GameObject PreviewObj;
    //public SkinnedMeshRenderer PreViewSkinnedMesh;
   // public bool bUsePreviewObject;
   // public Vector3 position  ;
    /// <summary>
    /// socket object 创建前要先指定骨骼
    /// 暂无太大用处，因为socket object已经作为选定骨骼的子对象
    /// </summary>
    public Transform SocketAttachBone;
    /// <summary>
    /// 移除socket时在编辑状态下把SocketGameObject 移除
    /// </summary>
    public void Destroy()
    {
        GameObject.DestroyImmediate(SocketGameObject);
    }


}

//编辑器保存数据有用
/// <summary>
/// 对应的是.asset 文件
/// </summary>
[System.Serializable]
public class SKMeshSocketListInfo : ScriptableObject
{
    /// <summary>
    /// asset文件连接的角色预制
    /// </summary>
    public GameObject characterPrefab;
    /// <summary>
    /// 预制体里的SocketList
    /// </summary>
    public SkeletalMeshSocket[] SocketList;
}

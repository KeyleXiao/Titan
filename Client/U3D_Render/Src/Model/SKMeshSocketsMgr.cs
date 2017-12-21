/*******************************************************************
** 文件名:	SKMeshSocketsMgr.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/2/4
** 版  本:	1.0
** 描  述:	管理带骨骼模型上所有插槽
** 应  用:  每个需要插槽的Gameobject需要先把本脚本添加到GameObject里面
            才能使用Socket Object Editor进行SkeletalMeshSocket添加
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;

public class SKMeshSocketsMgr : MonoBehaviour {
    /// <summary>
    /// 存储当前SocketMgr拥有者的所有sockets
    /// </summary>
    public SkeletalMeshSocket[] SocketList;
    //public string SlotGroupName;

    /// <summary>
    /// 根据传入的骨骼创建socket
    /// </summary>
    /// <param name="_SocketName">socket object 名字</param>
    /// <param name="bone">socket object 绑定的骨骼</param>
    public void CreateSocketGameObject(string _SocketName,Transform bone)
    {
        //获取Socket在list里面的对象
        SkeletalMeshSocket SocketObj ;
        //加载空的SkeletalMeshSocket 暂时改为用new方式创建一个空的GameObject
        //GameObject SocketPrefab;
        //插槽名字
        string SocketName = "Socket_";


        //SocketPrefab = Resources.Load("Prefab/SlotGroupPre") as GameObject;

        if (bone != null)// && SocketPrefab != null)
        {
            SocketObj = GetSocketByName(_SocketName);

            if (SocketObj == null)
                return;

            SocketObj.SocketGameObject = new GameObject();// ResNode.InstantiateRes(SocketPrefab) as GameObject;
            SocketObj.SocketGameObject.name = SocketName + _SocketName;

            SocketObj.SocketGameObject.transform.parent = bone;
            SocketObj.SocketGameObject.transform.SetPosition( bone.position);
            SocketObj.SocketGameObject.transform.rotation = bone.rotation;
        }

       // GameObject.CreatePrimitive(PrimitiveType.Cube);

    }

    /// <summary>
    /// 根据名字获得对应的socket object
    /// </summary>
    /// <param name="_SocketName">要返回的socket object 名字</param>
    /// <returns>返回_SocketName对应的socket object,没有则返回null</returns>
    public SkeletalMeshSocket GetSocketByName(string _SocketName)
    {

        if (SocketList != null && SocketList.Length > 0)
        {
            for (int index = 0; index < SocketList.Length; index++)
            {
                if (SocketList[index].SocketObjectName == _SocketName)
                    return SocketList[index];
            }
        }
        Debug.LogError("没找到名字为：" + _SocketName + "的插槽");
            return null;
    }

    /// <summary>
    /// 修改SkeletalMeshSocket 名字
    /// </summary>
    /// <param name="_SocketObject">需要修改名字的Socket Object</param>
    /// <param name="_NewSocketName">Socket Object新名字</param>
    /// <returns></returns>
    public bool ChangeSocketObjectName(SkeletalMeshSocket _SocketObject,string _NewSocketName)
    {
        if (_SocketObject == null)
            return false;
        if(_NewSocketName != null)
            _SocketObject.SocketObjectName = _NewSocketName;
    
        if(_SocketObject.SocketGameObject != null)
            _SocketObject.SocketGameObject.name = "Socket_" + _NewSocketName;
        return true;
    }

    /// <summary>
    /// 将对象附加到指定的插口
    /// </summary>
    /// <param name="_AttachSocketName">附加的目标插口名字</param>
    /// <param name="AttackObject">附加的对象</param>
    /// <returns>true:附加成功</returns>
    public bool AttackToSocket(string _AttachSocketName,Transform AttackObject)
    {
        bool res = false;
        SkeletalMeshSocket so = GetSocketByName(_AttachSocketName);

        if (so != null)
        {
            AttackObject.parent = so.SocketGameObject.transform;
            AttackObject.localPosition = Vector3.zero;
            AttackObject.localRotation = Quaternion.identity;
            res = true;
        }

        return res;
    }
}

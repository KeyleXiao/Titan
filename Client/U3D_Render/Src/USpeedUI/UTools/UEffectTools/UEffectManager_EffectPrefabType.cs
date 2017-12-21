/*******************************************************************
** 文件名:	UEffectManager_EffectPrefabType.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-6-28
** 版  本:	1.0
** 描  述:	预制体效果(UEffectManager_EffectPrefabType)

            一、需要新增预制体效果：
                1、UEffectPrefabType 添加新类型
                2、从美术那获取新类型对应的预制体文件，并填到m_dicEffectDataTable里面
                3、在新预制体里加上脚本UEffectNode
            二、使用
                1、创建效果参数对象并填入你需要的参数 var param = new UEffectPrefabParam();
                2、执行UEffectManager.CreateEffect(UEffectType.UET_EffectPrefab,ref param);
                3、手动移除执行 UEffect.DestroyEffect(UEffectType.UET_EffectPrefab,ref param);
                4、不主动移除的话，一般会随View的删除而删除。如果创建参数设置AutoDestroy=true时，效果播放结束会自动移除，但前提是该效果非循环效果;

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2017.7.13
** 描  述: 增加异步加载预制体类型UI特效方法
   使用：将创建特效的类型改用 UEffectPrefabAsyncParam 来实例化参数。
         有需要再特效实例化后进行额外操作的可以在参数的 onResLoadFinished 回调中处理。
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UEffect
{
    public partial class UEffectManager : IResNodeHandle, IAsyncResLoadHandler
    {
        /// <summary>
        /// 效果资源数据容器,基本动态添加的效果都写在这里
        /// 如果那些确定直接挂到View上的效果可以不写，不添加UEffectNode脚本
        /// </summary>
        protected Dictionary<UEffectPrefabType, UEffectDataParam> m_dicEffectDataTable = GetEffectDataTable();

        #region 一些用到的数据
        /// <summary>
        /// 效果数据信息
        /// </summary>
        [Serializable]
        protected class UEffectDataParam
        {
            /// <summary>
            /// 效果资源路径
            /// </summary>
            public string EffectPath;

            public UEffectDataParam(string _strEffectPath = "")
            {
                EffectPath = _strEffectPath;
            }
        }


        /// <summary>
        /// 效果数据加载信息
        /// </summary>
        protected class UEffectDataLoadParam
        {
            /// <summary>
            /// 记录预制体
            /// </summary>
            public ResNode RefferencePrefabResNode;
            public List<GameObject> RefferenceList;


            public UEffectDataLoadParam(ResNode _refPrefabResNode = null)
            {
                RefferencePrefabResNode = _refPrefabResNode;
                RefferenceList = new List<GameObject>();
            }
            public void Destroy()
            {
                if (RefferencePrefabResNode.isAsync)//这部分没测试过
                    AssetBundleManager.ReleaseResNodeAsync(UEffectManager.Instance, RefferencePrefabResNode, UEffectManager.Instance, true);
                else
                    AssetBundleManager.ReleaseResNode(UEffectManager.Instance, RefferencePrefabResNode, true);
                    //AssetBundleManager.DeleteAssets(ref RefferencePrefabResNode, true);
            }
        }
        #endregion


        #region 增加预制体类型时一般不需要碰的东西
        /// <summary>记录预制体实例引用</summary>
        protected Dictionary<UEffectPrefabType, UEffectDataLoadParam> m_dicEffectPrefabTypeUsed = new Dictionary<UEffectPrefabType, UEffectDataLoadParam>();
        /// <summary>记录定时器ID对象</summary>
        protected Dictionary<int, GameObject> m_dicTimerIDObject = new Dictionary<int, GameObject>();
        //所有效果引用数量最大值
        public const int MaxEffectPrefabReffCount = 50;
        //效果类型数量最大值
        public const int MaxEffectPrefabTypeCount = 10;

        //下面这个值注意修改
        //一种类型引用数量最大值(一种UI效果引用超过十个应该不会吧？还真有会超过十个的。)
        public const int MaxPrefabTypeRefCount = 50;

        protected bool CreatePrefabEffect(UEffectPrefabParam _effectParam)
        {
            UEffectDataLoadParam data = null;
            //已经加载过的直接实例化
            if (m_dicEffectPrefabTypeUsed.TryGetValue(_effectParam.AttachEffectType, out data))
            {
                _effectParam.EffectRes = data.RefferencePrefabResNode;
            }
            else
            {
                if (_effectParam is UEffectPrefabAsyncParam)
                {
                    AsyncLoadEffectPrefab(_effectParam);
                    return true;
                }
                else
                {
                    if (LoadEffectPrefab(_effectParam) == null)
                    {
                        return false;
                    }
                }
            }


            InstantiateEffectPrefab(_effectParam);

            InitEffectPrefabRefParam(_effectParam);


            return true;
        }
        protected bool DestroyPrefabEffect(UEffectPrefabParam _effectParam)
        {
            //删实例
            if (_effectParam.EffectGameObject != null)
            {
                UEffectNode tmpNode = _effectParam.EffectGameObject.GetComponent<UEffectNode>();
                if (tmpNode != null)
                {
                    tmpNode.To<UEffectNodeData_Prefab>().StopEffect();
                    UnRegisterEffectPrefabNode(tmpNode, _effectParam.AttachEffectType);
                }
                _effectParam.EffectRes.DestroyInstanceRes(ref _effectParam.EffectGameObject);
            }


            //删资源
            UEffectDataLoadParam loadParam = null;
            if (m_dicEffectPrefabTypeUsed.TryGetValue(_effectParam.AttachEffectType, out loadParam) && _effectParam.IsDestroyRes)
            {
                if (_effectParam is UEffectPrefabAsyncParam)
                {
                    AssetBundleManager.ReleaseResNodeAsync(this, loadParam.RefferencePrefabResNode, this, true);
                }
                else
                {
                    AssetBundleManager.ReleaseResNode(this, loadParam.RefferencePrefabResNode, true);
                }
                m_dicEffectPrefabTypeUsed.Remove(_effectParam.AttachEffectType);
            }


            return true;
        }
        protected void InitEffectPrefabRefParam(UEffectPrefabParam _effectParam)
        {
            if (_effectParam.EffectRes == null)
            {
                Trace.LogWarning("实例化UI效果类型:" + _effectParam.AttachEffectType + " 失败,资源没加载");
                return;
            }
            //异步没加载完
            if (!_effectParam.EffectRes.isLoadFinish)
                return;

            //实例化失败，已报错
            if (_effectParam.EffectGameObject == null)
                return;

            GameObject _goEffect = _effectParam.EffectGameObject;
            Transform EffectTrans = _goEffect.transform;
            EffectTrans.SetParent(_effectParam.AttachParent, false);

            //初始化坐标
            if (_effectParam.AttachPosition != null)
            {
                EffectTrans.localPosition = (Vector3)_effectParam.AttachPosition;
            }
            //初始化角度
            if (_effectParam.AttachRotation != null)
            {
                EffectTrans.GetComponentsInChildren<ParticleSystem>().ToList()
                    .ForEach(x =>
                    {
                        x.startRotation = (float)_effectParam.AttachRotation * Mathf.Deg2Rad;
                    });
            }
            //初始化延迟
            if (_effectParam.AttachDelay != null)
            {

                int timerID = (int)EEffectTimer.EET_Delay * MaxTimerIDCount + _goEffect.GetComponent<UEffectNode>().To<UEffectNodeData_Prefab>().EffectPrefabTypeID;
                TimerManager.SetTimer(this, timerID, (float)_effectParam.AttachDelay, 1);
                _goEffect.SetActive(false);
                m_dicTimerIDObject.Add(timerID, _goEffect);
            }
            ////////粒子相关
            UEffectNodeData_Prefab prefab = _goEffect.GetComponent<UEffectNode>().To<UEffectNodeData_Prefab>();
            prefab.InitParticles();

            //是否需要播放完后自动移除
            if (_effectParam.AutoDestroy)
            {
                //检测是否有循环播放的粒子系统，有的话不能自动移除
                if (prefab.IsLoopEffect())
                {
                    Trace.LogWarning("UI效果：" + EffectTrans.name + "不能用于自动移除，因为用到循环粒子系统");
                }
                else
                {
                    prefab = _goEffect.GetComponent<UEffectNode>().To<UEffectNodeData_Prefab>();
                    prefab.SetAutoDestroy();
                }
            }

            //if(_effectParam.AttachEffectType == UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame4)
            //{
            //    Debug.Log("begin  InitEffectPrefabRefParam");
            //    UParticleMask mask = _goEffect.GetComponent<UParticleMask>();
            //    if (mask == null)
            //        mask = _goEffect.AddComponent<UParticleMask>();
            //    RectTransform rectTrans = (EffectTrans as RectTransform);
            //    rectTrans.sizeDelta = new Vector2(152f, 190f);
            //    Debug.Log(rectTrans.gameObject.name + "..InitEffectPrefabRefParam.." + rectTrans.position);
            //    Debug.Log("end    InitEffectPrefabRefParam");
            //}

            //最后播放粒子
            prefab.ReplayEffect();

            //异步资源回调
            if (_effectParam is UEffectPrefabAsyncParam)
            {
                if ((_effectParam as UEffectPrefabAsyncParam).onResLoadFinished != null)
                    (_effectParam as UEffectPrefabAsyncParam).onResLoadFinished.Invoke(_effectParam as UEffectPrefabAsyncParam);
            }
        }
        //同步加载
        protected ResNode LoadEffectPrefab(UEffectPrefabParam _effectParam)
        {
            if (!m_dicEffectDataTable.ContainsKey(_effectParam.AttachEffectType))
            {
                Trace.LogWarning("UI效果资源数据容器 没有类型：" + _effectParam.AttachEffectType + " 的数据");
                return null;
            }
            UEffectDataLoadParam data = null;

            {
                ResNodeHandleParamBase param = new ResNodeHandleParam_Path(AssetType.Asset_Prefab, UPath.UArtistEffectPath + m_dicEffectDataTable[_effectParam.AttachEffectType].EffectPath);

                var resNode = AssetBundleManager.GetResNode(this, param);
                if (resNode == null)
                {
                    Trace.LogError("无法取资源：" + UPath.UArtistEffectPath + m_dicEffectDataTable[_effectParam.AttachEffectType].EffectPath);
                    return null;
                }
                //AssetBundleManager.SaveMemory(resNode);
                //当前使用的效果类型超出了限定值，先移除下无效的记录
                if (m_dicEffectPrefabTypeUsed.Count >= MaxEffectPrefabTypeCount)
                {
                    ClearUnusedEffectPrefabType();
                }
                data = new UEffectDataLoadParam(resNode);
                m_dicEffectPrefabTypeUsed.Add(_effectParam.AttachEffectType, data);
            }

            _effectParam.EffectRes = data.RefferencePrefabResNode;
            return data.RefferencePrefabResNode;
        }
        //异步加载
        protected ResNode AsyncLoadEffectPrefab(UEffectPrefabParam _effectParam)
        {
            UEffectPrefabType _eType = _effectParam.AttachEffectType;

            if (!m_dicEffectDataTable.ContainsKey(_eType))
            {
                Trace.LogWarning("UI效果资源数据容器 没有类型：" + _eType + " 的数据");
                return null;
            }
            UEffectDataLoadParam data = null;

            {
                //var resNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UArtistEffectPath + m_dicEffectDataTable[_eType].EffectPath);

                ResNodeHandleParamBase param = new ResNodeHandleParam_Path(AssetType.Asset_Prefab, UPath.UArtistEffectPath + m_dicEffectDataTable[_eType].EffectPath);

                ResNode resNode = AssetBundleManager.GetResNodeAsync(this, param, this, _effectParam);


                if (resNode == null)
                {
                    Trace.LogError("无法取资源：" + UPath.UArtistEffectPath + m_dicEffectDataTable[_eType].EffectPath);
                    return null;
                }
                //AssetBundleManager.SaveMemory(resNode);
                //当前使用的效果类型超出了限定值，先移除下无效的记录
                if (m_dicEffectPrefabTypeUsed.Count >= MaxEffectPrefabTypeCount)
                {
                    ClearUnusedEffectPrefabType();
                }
                data = new UEffectDataLoadParam(resNode);
                m_dicEffectPrefabTypeUsed.Add(_eType, data);
            }

            _effectParam.EffectRes = data.RefferencePrefabResNode;
            return data.RefferencePrefabResNode;
        }

        //实例化
        protected void InstantiateEffectPrefab(UEffectPrefabParam _effectParam)
        {
            if (_effectParam.EffectRes == null)
            {
                Trace.LogWarning("实例化UI效果类型:" + _effectParam.AttachEffectType + " 失败,资源没加载");
                return;
            }
            //异步没加载完
            if (!_effectParam.EffectRes.isLoadFinish)
                return;

            GameObject newEffect = _effectParam.EffectRes.InstanceMainRes();

            if(newEffect == null)
            {
                Trace.LogWarning("UI特效资源："+ _effectParam.AttachEffectType+" 实例化失败？");
                return;
            }

            UEffectNode uen = newEffect.GetComponent<UEffectNode>();
            if (!uen)
            {
                uen = newEffect.AddComponent<UEffectNode>();
                uen.EffectType = UEffectType.UET_EffectPrefab;
            }

            uen.NodeParam = _effectParam;
            _effectParam.EffectGameObject = newEffect;
            RegisterEffectPrefabNode(uen, _effectParam.AttachEffectType);
        }

        //Manager内部调Node的注册方法
        private void RegisterEffectPrefabNode(UEffectNode _nodeRegister, UEffectPrefabType _eType)
        {
            var _node = _nodeRegister.To<UEffectNodeData_Prefab>();
            if (_node != null)
            {
                _node.EffectType = _eType;
                _node.EffectPrefabTypeID = CalculateEffectPrefabRefID(_eType);

                m_dicEffectPrefabTypeUsed[_eType].RefferenceList.Add(_nodeRegister.gameObject);

                CheckMaxEffectCount(_eType);
            }
        }

        public void UnRegisterEffectPrefabNode(UEffectNode _nodeUnRegister, UEffectPrefabType _eType)
        {
            //有效果实例移除，减一下引用计数
            if (m_dicEffectPrefabTypeUsed.ContainsKey(_eType) && m_dicEffectPrefabTypeUsed[_eType].RefferenceList.Contains(_nodeUnRegister.gameObject))
            {
                m_dicEffectPrefabTypeUsed[_eType].RefferenceList.Remove(_nodeUnRegister.gameObject);
            }
            //如果有延时效果，在计时还没开始就要结束的话也移除下
            int nTimerID = (int)EEffectTimer.EET_Delay * MaxTimerIDCount + _nodeUnRegister.To<UEffectNodeData_Prefab>().EffectPrefabTypeID;
            if (m_dicTimerIDObject.ContainsKey(nTimerID))
            {
                TimerManager.KillTimer(this, nTimerID);
                m_dicTimerIDObject.Remove(nTimerID);
            }
        }

        protected bool ClearUnusedEffectPrefabType()
        {
            bool res = false;

            List<UEffectPrefabType> unusedTypeList = m_dicEffectPrefabTypeUsed.Where(x => x.Value.RefferenceList.Count <= 0).Select(x => x.Key).ToList();

            if (unusedTypeList.Count > 0)
            {
                unusedTypeList.ForEach(x =>
                {
                    m_dicEffectPrefabTypeUsed[x].Destroy();
                    m_dicEffectPrefabTypeUsed.Remove(x);
                });
                res = true;
            }
            else
            {
                Trace.LogWarning("UI 效果资源类型使用量超过限定值(" + MaxEffectPrefabTypeCount + ")且没发现有可释放的，请人工检查下");
            }

            return res;
        }

        protected int CalculateEffectPrefabRefID(UEffectPrefabType _eType)
        {
            return (int)_eType * MaxPrefabTypeRefCount + (int)m_dicEffectPrefabTypeUsed[_eType].RefferenceList.Count;
        }

        // 检查所有效果数量有没有超出值
        protected void CheckMaxEffectCount(UEffectPrefabType _eType)
        {
            int count = 0;
            m_dicEffectPrefabTypeUsed.Select(x => x.Value).ToList().ForEach(x => { count += x.RefferenceList.Count; });
            if (count > MaxEffectPrefabReffCount)
            {
                StringBuilder tmpSB = new StringBuilder();
                StringBuilder tmpSB1 = new StringBuilder();

                foreach(KeyValuePair<UEffectPrefabType,UEffectDataLoadParam> kp in m_dicEffectPrefabTypeUsed)
                {
                    tmpSB.Append(string.Format("<tr><td>{0}</td><td>{1}</td></tr>", kp.Key, kp.Value.RefferenceList.Count));
                }

                tmpSB1.Append("<details>");
                tmpSB1.Append("<summary>所有效果类型预制体数量超过限定值:{0}，当前值：{1},看是否需要优化下</summary>");
                tmpSB1.Append("<table border='1'><tbody align='center'>");
                tmpSB1.Append("<tr><th> UI特效类型 </th><th> 实例数量 </th></tr>{2}");
                tmpSB1.Append("</tbody></table>");
                tmpSB1.Append("</details>");             

                Trace.LogWarning(string.Format(tmpSB1.ToString(), MaxEffectPrefabReffCount, count, tmpSB.ToString()));
            }
                

            if (m_dicEffectPrefabTypeUsed[_eType].RefferenceList.Count >= MaxPrefabTypeRefCount)
            {
                Trace.LogWarning("效果类型：" + _eType + " 的引用数量超过了" + MaxPrefabTypeRefCount + "是不是哪里有问题？");
            }
        }

        /// <summary>
        /// 异步加载回调
        /// </summary>
        /// <param name="cof"></param>
        /// <param name="res"></param>
        /// <param name="UserDataObj"></param>
        public void ResLoadCallBack(ResConfigData cof, ResNode res, object UserDataObj)
        {
            UEffectPrefabAsyncParam _effectParam = UserDataObj as UEffectPrefabAsyncParam;

            if (_effectParam != null && _effectParam.EffectRes != null)
            {
                InstantiateEffectPrefab(_effectParam);

                InitEffectPrefabRefParam(_effectParam);
            }
        }
        #endregion

    }
}

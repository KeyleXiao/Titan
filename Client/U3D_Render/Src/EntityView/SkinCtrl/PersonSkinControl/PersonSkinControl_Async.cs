/*******************************************************************
** 文件名:	PersonSkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	人物实体皮肤控制
********************************************************************/
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render.SkinCtrl
{


    /// <summary>
    /// 人物皮肤控制
    /// </summary>
    public partial class PersonSkinControl : EntitySkinControl
    {
        #region << Field Area >>
        Dictionary<int, List<ChangeSkinData>> m_ChangeSkinData = new Dictionary<int, List<ChangeSkinData>>();
        List<int> LoadCompleteList = new List<int>();

        
        #endregion


        #region << Method Area >>


        private void CollectAsyncData()
        {
            SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);

            if(!isCull || !SkinManager.enabledSkinCull)
            {
                SkinManager.DeleteSkinAsync(currentSkinId, OnSkinLoadFinish, NotReleaseFromCache);
            }

            RemoveCallBack(currentSkinId);
            //没有加载完的，卸载掉
            if (m_ChangeSkinData.Count > 0)
            {
                foreach (int id in m_ChangeSkinData.Keys)
                {
                    SkinManager.DeleteSkinAsync(id, OnSkinLoadFinish, NotReleaseFromCache);
                }
            }

            m_ChangeSkinData.Clear();
            m_currentLoadedSkinInstance = null;
            ClearAllocSkinControlAsyncLoadingObj();
        }
        //private void TestOnSkinChanged(CreatureProperty property, SkinInstance sk, int nOtherValue)
        //{
        //    BaseStateMachine fsm = property.Master.StateMachine;
        //    fsm.gameObject.name = sk.SrcSkin.ResSkinObj.AssetName + "(entityID:" + fsm.entityID + ")";
        //    if (fsm)
        //    {
        //        fsm.OnSkinChanged(property.Master, sk, nOtherValue);
        //    }


        //}

        private void UpdateLoadingSkin()
        {
            if (m_ChangeSkinData.Count <= 0 || (isCull && SkinManager.enabledSkinCull))
            {
                return;
            }

            LoadCompleteList.Clear();
            foreach (int id in m_ChangeSkinData.Keys)
            {
                SkinControlAsyncLoadingObj asyncojb = AllocSkinControlAsyncLoadingObj(id);
                if (!asyncojb.loadFinish)
                {
                    continue;
                }

                if (asyncojb.nSkinID != id)
                {
                    continue;
                }
                List<ChangeSkinData> dataList = null;
                if (!m_ChangeSkinData.TryGetValue(id, out dataList))
                {
                    Debug.LogError("PersonSkinControl重大错误--UpdateLoadingSkin");
                    continue;
                }
                LoadCompleteList.Add(id);
            }

            foreach (int id in LoadCompleteList)
            {
                List<ChangeSkinData> dataList = null;
                m_ChangeSkinData.TryGetValue(id, out dataList);

                SkinControlAsyncLoadingObj list = AllocSkinControlAsyncLoadingObj(id);

                m_ChangeSkinData.Remove(id);

                for (int i = 0; i < dataList.Count; i++)
                {
                    InvokeSkinChangedEvent(m_currentLoadedSkinInstance,dataList[i].nPart);
                }
                list.Clear();
            }
        }

        private void OnSkinLoadFinish(Skin skin)
        {
            if (skin == null)
            {
                return;
            }

            //回调的，跟当前ID不匹配，卸载这个皮肤
            if(skin.nSkinID != currentSkinId)
            {
                if (m_currentLoadedSkinInstance != null)
                {
                    if(m_currentLoadedSkinInstance.isValid)
                    {
                        //看看当前的skininstance是不是与skin的ID一样
                        if(m_currentLoadedSkinInstance.nSkinID == skin.nSkinID)
                        {
                            SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
                            m_currentLoadedSkinInstance = null;
                        }
                    }
                }
                RemoveCallBack(skin.nSkinID);
                SkinManager.DeleteSkinAsync(skin.nSkinID, OnSkinLoadFinish, NotReleaseFromCache);
                return;
            }

            //如果已经被裁剪了，卸载这个皮肤
            if (isCull && SkinManager.enabledSkinCull)
            {
                if (m_currentLoadedSkinInstance != null)
                {
                    if (m_currentLoadedSkinInstance.isValid)
                    {
                        //看看当前的skininstance是不是与skin的ID一样
                        if (m_currentLoadedSkinInstance.nSkinID == skin.nSkinID)
                        {
                            SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
                            m_currentLoadedSkinInstance = null;
                        }
                    }
                }
                SkinManager.DeleteSkinAsync(skin.nSkinID, OnSkinLoadFinish, NotReleaseFromCache);
                return;
            }


            if(m_currentLoadedSkinInstance != null)
            {
                if (m_currentLoadedSkinInstance.isValid)
                {
                    SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
                    RemoveCallBack(m_currentLoadedSkinInstance.nSkinID);
                    SkinManager.DeleteSkinAsync(m_currentLoadedSkinInstance.nSkinID, OnSkinLoadFinish, NotReleaseFromCache);

                    m_currentLoadedSkinInstance = null;
                }

            }

            SkinControlAsyncLoadingObj obj = AllocSkinControlAsyncLoadingObj(skin.nSkinID);
            if (m_ChangeSkinData.ContainsKey(skin.nSkinID))
            {
                obj.loadFinish = true;
            }
            SkinInstance si = skin.Instantiate_Main();
            if (si == null)
            {
                m_currentLoadedSkinInstance = null;
                ClearPersonSkinDataFromMachine(m_machine);
                return ;
            }
            GameUtil.SetLayer(m_machine.creature.gameObject.layer, si.SkinObj, true);

            m_currentLoadedSkinInstance = si;
            PersonSkinConfig psp = si.entitySkinConfig as PersonSkinConfig;
            si.SkinObj.transform.parent = transform;
            si.SkinObj.transform.localPosition = Vector3.zero;
            si.SkinObj.transform.localRotation = Quaternion.identity;
            si.SkinObj.transform.name = skin.ResSkinObj.AssetName + "-skin" + skin.nSkinID + "_C"; ;

            if (psp.AttackAnimatorLayer <= 0)
            {
                si.SkinObj_A.transform.parent = transform;
                si.SkinObj_B.transform.parent = transform;
            }

            if (si.SkinObj_A)
            {
                si.SkinObj_A.transform.parent = transform;
                si.SkinObj_A.transform.localPosition = new Vector3(-1.0f, 0.0f, 0.0f);
                si.SkinObj_A.transform.localRotation = Quaternion.identity;
                si.SkinObj_A.name = skin.ResSkinObj.AssetName + "-skin" + skin.nSkinID + "_A";
            }

            if (si.SkinObj_B)
            {
                si.SkinObj_B.transform.parent = transform;
                si.SkinObj_B.transform.localPosition = new Vector3(-1.0f, 0.0f, 0.0f);
                si.SkinObj_B.transform.localRotation = Quaternion.identity;
                si.SkinObj_B.name = skin.ResSkinObj.AssetName + "-skin" + skin.nSkinID + "_A";
            }

            foreach (RenderVisibleHelper skre in si.VisibleHelperList)
            {
                skre.hostSkin = null;
            }

            foreach (RenderVisibleHelper skre in si.VisibleHelperList)
            {
                skre.hostSkin = this;
            }

            ApplyStateData(m_currStateData);
            ApplyPersonSkinDataToMachine(m_machine,si);
        }

        private void PushToCallBack(int nSkinID, int nPart)
        {
            SkinControlAsyncLoadingObj obj = AllocSkinControlAsyncLoadingObj(nSkinID);
            List<ChangeSkinData> dataList = null;
            if (!m_ChangeSkinData.TryGetValue(nSkinID,out dataList))
            {
                dataList = new List<ChangeSkinData>();
                m_ChangeSkinData.Add(nSkinID, dataList);

                obj.Clear();

                obj.nSkinID = nSkinID;
                obj.loadFinish = false;
            }
            else
            {
                dataList = m_ChangeSkinData[nSkinID];
            }
            ChangeSkinData data = new ChangeSkinData();
            data.nPart = nPart;
            dataList.Add(data);
        }

        private void RemoveCallBack(int nSkinID)
        {
            if (m_ChangeSkinData.ContainsKey(nSkinID))
            {
                AllocSkinControlAsyncLoadingObj(nSkinID).Clear();
                m_ChangeSkinData.Remove(nSkinID);
            }
        }

        #endregion

    }
}

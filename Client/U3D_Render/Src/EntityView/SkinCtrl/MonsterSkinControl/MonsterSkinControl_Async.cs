/*******************************************************************
** 文件名:	MonsterSkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	怪物实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render.SkinCtrl
{
    /// <summary>
    /// 怪物皮肤控制
    /// </summary>
    public partial class MonsterSkinControl : EntitySkinControl
    {
        #region << Field Area >>

        Dictionary<int, List<ChangeSkinData>> m_ChangeSkinData = new Dictionary<int, List<ChangeSkinData>>();
        List<int> LoadCompleteList = new List<int>();
        #endregion


        #region << Method Area >>


        private void UpdateLoadingSkin()
        {
            if (m_ChangeSkinData.Count <= 0)
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
                    Debug.LogError("MonsterSkinControl重大错误--UpdateLoadingSkin");
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
                    InvokeSkinChangedEvent(m_currentLoadedSkinInstance, dataList[i].nPart);
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
            if (skin.nSkinID != currentSkinId)
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
                RemoveCallBack(skin.nSkinID);
                SkinManager.DeleteSkinAsync(skin.nSkinID, OnSkinLoadFinish);
                return;
            }

            if (m_currentLoadedSkinInstance != null)
            {
                if (m_currentLoadedSkinInstance.isValid)
                {
                    SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
                    RemoveCallBack(m_currentLoadedSkinInstance.nSkinID);
                    SkinManager.DeleteSkinAsync(m_currentLoadedSkinInstance.nSkinID, OnSkinLoadFinish);

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
                ClearMonsterSkinDataFromMachine(m_machine);
                return ;
            }
            GameObject go = si.SkinObj;
            GameUtil.SetLayer(m_machine.creature.gameObject.layer, go, true);
            m_currentLoadedSkinInstance = si;
            go.transform.parent = m_machine.creature;
            go.transform.localPosition = Vector3.zero;
            go.transform.localRotation = Quaternion.identity;
            go.name = skin.ResSkinObj.AssetName + "-skin" + skin.nSkinID;

            go = null;

            ApplyStateData(m_currStateData);
            ApplyMonsterSkinDataToMachine(m_machine, si);
        }


        private void PushToCallBack(int nSkinID, int nPart)
        {
            SkinControlAsyncLoadingObj obj = AllocSkinControlAsyncLoadingObj(nSkinID);
            List<ChangeSkinData> dataList = null;
            if (!m_ChangeSkinData.TryGetValue(nSkinID, out dataList))
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

        private void CollectAsyncData()
        {
            SkinManager.CacheSkinInstance(m_currentLoadedSkinInstance);
            SkinManager.DeleteSkinAsync(currentSkinId, OnSkinLoadFinish);

            RemoveCallBack(currentSkinId);
            //没有加载完的，卸载掉
            if (m_ChangeSkinData.Count > 0)
            {
                foreach (int id in m_ChangeSkinData.Keys)
                {
                    SkinManager.DeleteSkinAsync(id, OnSkinLoadFinish);
                }
            }
            m_ChangeSkinData.Clear();
            m_currentLoadedSkinInstance = null;
            ClearAllocSkinControlAsyncLoadingObj();
        }
        #endregion
    }
}

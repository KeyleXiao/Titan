using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.PersonModelTransFormInfo
{
    // 与PersonModelTransFormInfo.csv表里面的列对应
    public enum PERSONMODELVIEWID : int
    {
        PERSONMODELVIEWID_GAMEWAITINGROOM = 1,                      // 界面id之选人大厅     
        PERSONMODELVIEWID_SHOP,
        PERSONMODELVIEWID_WARSUPPORT,
    }

    /// <summary>
    /// 选人提示类
    /// </summary>
    public class HeroModelTransFormInfo
    {
        public int nHeroId;
        public Vector3 Position ;                  // 比赛类型
        public Vector3 Rotation;                   // 比赛类型
        public Vector3 Scale;                      // 比赛类型
    }

    public class ViewHeroModelInfo
    {
        public int nViewId;
        public Dictionary<int, HeroModelTransFormInfo> dicAllViewHeroModel = new Dictionary<int, HeroModelTransFormInfo>();
    }

    class PersonModelTransFormInfo
    {
        public Hashtable allPersonModelTransFormInfoTable;
        private static PersonModelTransFormInfo singleton = new PersonModelTransFormInfo();
        private Dictionary<int, ViewHeroModelInfo> dicTypeTeamSelectCount = new Dictionary<int, ViewHeroModelInfo>();
 
        public static PersonModelTransFormInfo Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new PersonModelTransFormInfo();
            }
        }

        public void Load(string stPath)
        {
            InitPersonModelTransFormInfoCsv(stPath);
        }

        public void UnLoad()
        {
            if (allPersonModelTransFormInfoTable != null)
            {
                allPersonModelTransFormInfoTable.Clear();
                allPersonModelTransFormInfoTable = null;
            }
            singleton = null;
        }

        // 初始化csv表
        protected void InitPersonModelTransFormInfoCsv(string stPath)
        {
            // string stSchemeAllText = File.ReadAllText(stPath);        
            ScpReader reader = new ScpReader(stPath, true, 2);
            allPersonModelTransFormInfoTable = new Hashtable();
            int nIndex = 0; 
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                nIndex = 0;
               
                HeroModelTransFormInfo singleHeroInfo = new HeroModelTransFormInfo();

                singleHeroInfo.nHeroId = reader.GetInt(i, (int)nIndex++, 0);
                int nViewId = reader.GetInt(i, (int)nIndex++, 0);               
                singleHeroInfo.Position.x = reader.GetFloat(i, (int)nIndex++, 0);
                singleHeroInfo.Position.y = reader.GetFloat(i, (int)nIndex++, 0);
                singleHeroInfo.Position.z = reader.GetFloat(i, (int)nIndex++, 0);
                singleHeroInfo.Rotation.x = reader.GetInt(i, (int)nIndex++, 0);
                singleHeroInfo.Rotation.y = reader.GetInt(i, (int)nIndex++, 0);
                singleHeroInfo.Rotation.z = reader.GetInt(i, (int)nIndex++, 0);
                singleHeroInfo.Scale.x = reader.GetFloat(i, (int)nIndex++, 0);
                singleHeroInfo.Scale.y = reader.GetFloat(i, (int)nIndex++, 0);
                singleHeroInfo.Scale.z = reader.GetFloat(i, (int)nIndex++, 0);
               
                if (!allPersonModelTransFormInfoTable.ContainsKey(nViewId))
                {
                    ViewHeroModelInfo ViewHeroInfo = new ViewHeroModelInfo();
                    ViewHeroInfo.dicAllViewHeroModel[singleHeroInfo.nHeroId] = singleHeroInfo;
                    ViewHeroInfo.nViewId = nViewId;
                    allPersonModelTransFormInfoTable[ViewHeroInfo.nViewId] = ViewHeroInfo;
                } 
                else
                {
                    ViewHeroModelInfo ViewHeroInfo = allPersonModelTransFormInfoTable[nViewId] as ViewHeroModelInfo;
                    ViewHeroInfo.dicAllViewHeroModel[singleHeroInfo.nHeroId] = singleHeroInfo;
                }
            }
        }

        public Vector3 GetHeroModelRotation(PERSONMODELVIEWID viewId, int nHeroId)
        {
            HeroModelTransFormInfo heroInfo = GetHeroModelInfo(viewId, nHeroId);
            if(heroInfo == null)
            {
                return new Vector3(0, 180, 0);
            }
            else
            {
                return heroInfo.Rotation;
            }
        }

        public Vector3 GetHeroModelScale(PERSONMODELVIEWID viewId, int nHeroId)
        {
            HeroModelTransFormInfo heroInfo = GetHeroModelInfo(viewId, nHeroId);
            if (heroInfo == null)
            {
                return new Vector3(600, 600, 600);
            }
            else
            {
                return heroInfo.Scale;
            }
        }

        public Vector3 GetHeroModelPosition(PERSONMODELVIEWID viewId, int nHeroId)
        {
            HeroModelTransFormInfo heroInfo = GetHeroModelInfo(viewId, nHeroId);
            if (heroInfo == null)
            {
                return new Vector3(50, -600, -400);
            }
            else
            {
                return heroInfo.Position;
            }
        }

        public HeroModelTransFormInfo GetHeroModelInfo(PERSONMODELVIEWID viewId, int nHeroId)
        {
            if (allPersonModelTransFormInfoTable == null || !allPersonModelTransFormInfoTable.ContainsKey((int)viewId))
            {
                return null;
            }

            ViewHeroModelInfo viewInfo = allPersonModelTransFormInfoTable[(int)viewId] as ViewHeroModelInfo;
            if (viewInfo == null || !viewInfo.dicAllViewHeroModel.ContainsKey(nHeroId))
            {
                return null;
            }

            HeroModelTransFormInfo heroInfo = viewInfo.dicAllViewHeroModel[nHeroId];

            return heroInfo;
        }
    }
}

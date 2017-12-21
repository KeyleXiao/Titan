using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using SPELL;
using GameLogic;

public class EntityTimeSearchManager : Singleton<EntityTimeSearchManager>
{
    public enum EntitySearchType
    {
        EST_WATCHOUT, 
    }
 
    public Dictionary<int, IEntityTimeSearch> dicEntitySearchManager = new Dictionary<int, IEntityTimeSearch>();

    public EntityTimeSearchManager()
    {
       
    }
    ~EntityTimeSearchManager()
    {
        
    }

    public void AddEntityTimeSearch(U3D_Render.EntityView objev, EntitySearchType eType = EntitySearchType.EST_WATCHOUT)
    {
        int nEntityID = objev.ID;
        if (!dicEntitySearchManager.ContainsKey(nEntityID))
        {
            switch (eType)
            {
                case EntitySearchType.EST_WATCHOUT:
                    {
                        EntityTimeSearchWatchOutTower entityTimeSearch = new EntityTimeSearchWatchOutTower();
                        dicEntitySearchManager[nEntityID] =entityTimeSearch;
                    }
                    break;
                default:
                    break;
            }
        }

        dicEntitySearchManager[nEntityID].AddEntityTimeSearch(objev);
    }

    public void ChangeEntityTimeSearchState(int nEntityID,bool bSearch)
    {
        if (!dicEntitySearchManager.ContainsKey(nEntityID))
        {
            return;
        }


        dicEntitySearchManager[nEntityID].ChangeEntityTimeSearchState(bSearch);
    }

    public void release()
    {
        foreach (IEntityTimeSearch item in dicEntitySearchManager.Values)
        {
            item.release(); 
        }

        dicEntitySearchManager.Clear();
    }
}
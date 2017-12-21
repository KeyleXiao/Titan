using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using SPELL;
using GameLogic;

public class IEntityTimeSearch : ITimerHanlder
{
    protected U3D_Render.EntityView entityViewInfo;

    public enum EN_TimeId
    {
        EN_ENTITYSEARCH = 0,        // 实体范围检测实体
    }

    public IEntityTimeSearch()
    {
        
    }

    ~IEntityTimeSearch()
    {
       
    }

    public virtual void AddEntityTimeSearch(U3D_Render.EntityView objev)
    {
     
    }

    public virtual void OnTimer(int nTimerID)
    {

    }

    public virtual void release()
    {
       
    }

    public virtual void ChangeEntityTimeSearchState(bool bSearch)
    {
        
    }
}
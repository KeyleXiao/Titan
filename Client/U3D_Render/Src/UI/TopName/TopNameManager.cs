using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using U3D_Render;
using Scaleform;
public partial class TopNameManager 
{

    #region API

    /// <summary>
    /// 创建一个头顶名称
    /// </summary>
    /// <param name="entry">实体</param>
    /// <param name="entryId">实体ID</param>
    /// <param name="text">内容</param>
    /// <param name="col">颜色</param>
    /// <returns></returns>
    public static TopName CreateTopName(GameObject entry, int entryId, string text, Color col)
    {
        if (SFGFxMovieManager.Instance == null)
        {
            Trace.LogError("SFGFxMovieManager没有初始化，请不要在Awake或Start中创建！");
            return null;
        }

        if (!bInit)
        {
            Trace.LogError("GfxLableManager必须初始化！");
            return null;
        }
        //有可能TopNameMovie还没有创建完成.
        if (TopNameMovie.Instance == null)
        {
            //Trace.LogError("TopNameMovie尚未创建完成，请等待创建完成后再调用。");
            return null;
        }
        if (m_TopNameMap.ContainsKey(entryId))
        {
            Trace.LogWarning(entryId + "已经有头顶名，返回已有的实例");

            return m_TopNameMap[entryId];
        }

        U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entryId);
        if (!objev.IsValid)
        {
            Trace.LogWarning("实体视图无效！id=" + entryId);
            return null;
        }
        ENTITY_TYPE entityType = objev.Type;

        MovieNode trs = new MovieNode();


        TopName t = GetTopName(ref trs);
        if(t == null)
        {
            return t;
        }
        t.Init(entry, entryId, text, col);
        t.CreateBillBorad();
        t.ChangeMovieNode(trs);
        m_TopNameMap.Add(entryId, t);
        index++;
        ChangeMask(EntityFactory.getEntityViewByID(entryId), true);

        //if (!go)
        //{
        //    go = GameObject.CreatePrimitive(PrimitiveType.Plane);

        //    go.renderer.material.SetTexture("_MainTex", TopNameMovie.TopNameRenderTexture);
        //} 

        return t;
    }
    //static GameObject go;
    public static TopName FindTopName(int entryID)
    {
        if (m_TopNameMap.ContainsKey(entryID))
        {
            return m_TopNameMap[entryID];
        }
        return null;
    }

    public static void UpdateTopName(GameObject entry, int entryId, string text, Color col)
    {
        if (m_TopNameMap.ContainsKey(entryId))
        {
            TopName t = m_TopNameMap[entryId];
            t.SetText(text);
            t.SetColor(col);
            t.Entry = entry;
        }

    }

    public static void Init()
    {
        if(bInit)
        {
            return;
        }
        bInit = true;
        InitCache();
    }

    public static void Clear()
    {
        if (!bInit)
        {
            return;
        }
        bInit = false;
        DestroyALL();
    }
    /// <summary>
    /// 改变mask
    /// </summary>
    /// <param name="id"></param>
    /// <param name="bUseInitMask"></param>
    public static void ChangeMask(EntityView ev, bool bUseInitMask = false)
    {
        if (ev != null)
        {
            if (ev.Type == ENTITY_TYPE.TYPE_MONSTER)
            {
                int mas = 0;
                if (bUseInitMask)
                {
                    mas = ev.InitMaskFlag;
                }
                else
                {
                    mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
                    if (!bFirstVisibleMask)
                    {
                        bFirstVisibleMask = true;
                        return;
                    }
                }
                if (mas != 0)
                {
                    SetVisible(ev.ID, (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_NAME) != (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_NAME);
                }
                else
                {
                    SetVisible(ev.ID, true);
                }

            }
        }
    }

    public static void Update()
    {
        if (!bInit)
            return;

        InvalidList.Clear();

        U3D_Render.EntityView view;
        foreach (TopName var in m_TopNameMap.Values)
        {
            TopName tn = var;
            view = EntityFactory.getEntityViewByID(tn.ID);

            if (view == null)
            {
                InvalidList.Enqueue(tn);
                continue;
            }

            if (!tn.Entry)//实体被删除
            {
                InvalidList.Enqueue(tn);
                continue;
            }

            if (!tn.Entry.activeSelf || !view.StateMachine.GetVisible())//实体没有被激活或者不可见
            {
                if (tn.GetEnable())
                {
                    tn.SetEnable(false);
                }
            }
            else//正常的实体
            {
                if (!tn.GetEnable())
                {
                    tn.SetEnable(true);
                }
                tn.OnUpdate();
            }
        }

        while (InvalidList.Count > 0)
        {
            //移除无效的TopName节点
            TopName b = InvalidList.Dequeue();
            if(OptimitzedControlPlane.Instance.EnityCreateOptimize)
            {
                CacheTopName(b);
            }
            else
            {
                TopNameManager.Destroy(b);
            }
            
        }

        InvalidList.Clear();
    }

    /// <summary>
    /// 摧毁所有的组件
    /// </summary>
    public static void DestroyALL()
    {
        if (Movie.GetSingleton<TopNameMovie>() == null)
        {
            return;
        }
        ClearCache();
        if (null != m_TopNameMap)
        {
            foreach (KeyValuePair<int, TopName> var in m_TopNameMap)
            {
                if (TopNameMovie.Instance != null)
                {
                    TopNameMovie.Instance.RemoveMovieAS3(var.Value.Instance);
                }
                var.Value.Clear();
            }
            m_TopNameMap.Clear();

            CurrentColIndex = 0;
            CurrentRowIndex = 0;
            UnUsedMovieNodeList.Clear();
        }
    }

    /// <summary>
    /// 摧毁指定的Lable
    /// </summary>
    /// <param name="instance">需要清除的Lable</param>
    public static void Destroy(TopName instance)
    {
        if (Movie.GetSingleton<TopNameMovie>() == null)
        {
            return;
        }
        if (null == instance)
        {
            return;
        }
        int id = instance.ID;

        if (instance.movieNode.isValid)
        {
            UnUsedMovieNodeList.Enqueue(instance.movieNode);
        }

        if (TopNameMovie.Instance != null)
        {
            TopNameMovie.Instance.RemoveMovieAS3(instance.Instance);
            instance.Clear();
        }
        if (m_TopNameMap.ContainsKey(id))
        {
            m_TopNameMap[id] = null;
            m_TopNameMap.Remove(id);
           
        }
        instance = null;
    }

    public static void SetVisible(int id, bool b)
    {
        if (m_TopNameMap.ContainsKey(id))
        {
            TopName tn = m_TopNameMap[id];
            tn.SetVisible(b);
        }
    }

    /// <summary>
    /// 实体死亡
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnEntityDead(int entityID,ENTITY_TYPE t)
    {
        if (Movie.GetSingleton<TopNameMovie>() == null)
        {
            return;
        }
        if (m_TopNameMap.ContainsKey(entityID))
        {
            //目前仅仅针对怪物
            if (t == ENTITY_TYPE.TYPE_MONSTER)
            {
                if (OptimitzedControlPlane.Instance.EnityCreateOptimize)
                {
                    CacheTopName(m_TopNameMap[entityID]);
                }
                else
                {
                    Destroy(m_TopNameMap[entityID]);
                }
                
            }

        }
    }

    /// <summary>
    /// 实体复活
    /// </summary>
    /// <param name="entityID"></param>
    public static void OnEntityRelive(int entityID, ENTITY_TYPE t)
    {

    }
    #endregion



    #region Movie管理
    /// <summary>
    /// 将swf的每个头顶名单位的宽
    /// </summary>
    public const int MovieCell_Width = 120;

    /// <summary>
    /// 将swf的每个头顶名单位的高
    /// </summary>
    public const int MovieCell_Height = 20;

    /// <summary>
    /// 最大行
    /// </summary>
    public const int MaxRowCount = (int)TopNameMovie.TopNameMovieWidth / MovieCell_Width;

    /// <summary>
    /// 最大列
    /// </summary>
    public const int MaxColCount = (int)TopNameMovie.TopNameMovieHeight / MovieCell_Height;

    public static Vector2 TopNameSize = new Vector2(120.0f, 20.0f);

    private static int CurrentRowIndex = 0;
    private static int CurrentColIndex = 0;

    /// <summary>
    /// 为头顶名分配一个位置，分配失败，则表示Movie已经满了。
    /// </summary>
    /// <param name="btr"></param>
    /// <returns></returns>
    private static bool AllocTopNamePos(ref MovieNode btr)
    {
        btr.isValid = false;
        if (UnUsedMovieNodeList.Count > 0)
        {
            MovieNode bt = UnUsedMovieNodeList.Dequeue();
            btr.RowIndex = bt.RowIndex;
            btr.ColIndex = bt.ColIndex;

            btr.start.x = MovieCell_Width * bt.RowIndex;
            btr.start.y = MovieCell_Height * bt.ColIndex;

            btr.end.x = btr.start.x + TopNameSize.x;
            btr.end.y = btr.start.y + TopNameSize.y;
            btr.vSize = TopNameSize;

            btr.isValid = true;
        }
        else
        {
            btr.start.x = MovieCell_Width * CurrentRowIndex;
            btr.start.y = MovieCell_Height * CurrentColIndex;

            btr.end.x = btr.start.x + TopNameSize.x;
            btr.end.y = btr.start.y + TopNameSize.y;
            btr.vSize = TopNameSize;


            btr.RowIndex = CurrentRowIndex;
            btr.ColIndex = CurrentColIndex;

            CurrentRowIndex++;
            if (CurrentRowIndex >= MaxRowCount)
            {
                CurrentColIndex++;
                if (CurrentColIndex >= MaxColCount)
                {
                    return false;
                }
                CurrentRowIndex = 0;
            }

            btr.isValid = true;
        }
        return true;
    }

    #endregion


    #region 静态属性

    private static bool bInit = false;

    /// <summary>
    /// 第一次走跟visible相关的mask，就是创建时候的mask，屏蔽掉
    /// </summary>
    private static bool bFirstVisibleMask = false;

    private static Dictionary<int, TopName> m_TopNameMap = new Dictionary<int, TopName>();

    private static Queue<TopName> InvalidList = new Queue<TopName>();

    private static Queue<MovieNode> UnUsedMovieNodeList = new Queue<MovieNode>();

    private static int index = 0;
    private const string LableAS3RefernceName = "Com.Scaleform.Common.DefaultLable";
    #endregion
}

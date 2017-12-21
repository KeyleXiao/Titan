using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SPELL;

/*-------------------------------------
 * 技能选择光圈管理
 * 目前有3种光圈
 * 1、圆形
 * 初始化计算：位置类型（英雄位置，技能目标点位置）；光圈大小（技能距离，技能攻击范围）
 * 每帧更新：圆心位置
 * 2、矩形
 * 初始化计算：矩形大小（技能距离表示长度，技能范围表示宽度）
 * 每帧更新：更新矩形角度，自身点指向目标点
 * 3、扇形
 * 初始化计算：位置类型（英雄位置，技能目标点位置）；扇形大小（技能距离表示半径，技能范围表示角度）
 * 每帧更新：更新扇形角度，自身点指向目标点

--------------------------------------*/

public class SelectEffect
{
    public string effectName;    
    protected Transform m_transform;
    public bool isMaskBuilding;
    public bool isAnotherPlane; //是否在水平方向上做多一个plane
    #region 初始化和析构

    private Transform m_selectEffectRoot;
    public void loadEffect(string prefabPath,Transform parent)
    {
        m_selectEffectRoot = parent;
        this.prefabPath = prefabPath;
        this.active = false;
    }

    public void destroy()
    {
        if (m_PrefabRes != null)
        {
            AssetBundleManager.DeleteAssets(ref m_PrefabRes,true);
        }
        m_transform = null;
    }

    #endregion

    #region 各类属性

    private bool m_active;
    // define the active attribute of effect gameObject
    public bool active
    {
        get { return m_active; }
        set 
        {
            m_active = value;
            if (m_transform)
            {
                OnDisable();
                m_transform.gameObject.SetActive(value);
            }
        }
    }

    public virtual void OnDisable() { return; }

    private ResNode m_PrefabRes = null;
    private string m_prefabPath;
    //the path in Resource folder of Unity assets. Setting prefabPath will reload the effect gameObject
    public string prefabPath
    {
        get { return m_prefabPath; }
        set
        {
            if (m_prefabPath != value)
            {
                if (m_transform)
                {
                    AssetBundleManager.DeleteAssets(ref m_PrefabRes,true);

                    m_prefabPath = "";
                }
                if(null == m_PrefabRes)
                {
                    m_PrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,value);
                }

                if(null != m_PrefabRes)
                {
                    GameObject entity = m_PrefabRes.InstanceMainRes();
                    if (entity)
                    {
                        m_transform = entity.transform;
                        m_prefabPath = value;
                        if (m_selectEffectRoot)
                        {
                            m_transform.parent = m_selectEffectRoot;
                        }
                    }
                }

            }            
        }
    }

    private Vector3 m_position;
    public Vector3 position
    {
        get { return m_position; }
        set
        {
            m_position = value;
            if (m_transform)
            {
                //因为贴图是Projector，所以Y轴上提一段距离
                Vector3 projectPosition = value;
                projectPosition.y += 1.0f;
                m_transform.SetPosition(projectPosition);
            }
        }
    }

    private int m_drawType;
    public int nDrawType
    {
        get { return m_drawType; }
        set { m_drawType = value; }
    }
    
    #endregion
        
    public virtual void buildArea(float selectDistance, float attackRange) { }
    public virtual void updateArea(Vector3 sourcePosition,Vector3 targetPos) { }
    public virtual bool isInArea(Vector3 targetPos) { return false; }
}

#region 各类真实形状贴花
public class SelectEffect_Rect:SelectEffect
{
    private float m_rectSelectDis = 0.0f;
    private float m_rectSelectWidth = 0.0f;

    public override void buildArea(float selectDistance, float attackRange)
    {
        float distance=selectDistance;
        float width = attackRange * 2;//*2:策划要求
        if (!m_transform) return;
        Projector projector = m_transform.GetComponent<Projector>();
        if (projector == null)
        {
            Trace.Log("projector is null");
            return;
        }
        projector.orthographicSize = distance / 2.0f ;
        projector.aspectRatio = 1.0f / distance * width ;   
        if (isMaskBuilding)
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers & 0 << layerMask;
        }
        else
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers | 1 << layerMask;
        }
        m_rectSelectDis = distance;
        m_rectSelectWidth = width;
    }

    private Vector3 m_targetDir;
    private Vector3 m_rectEndPos; //矩形中心线终点，用于计算是否在区域内
    private Vector3 m_rectStartPos;//矩形中心线起点，用于计算是否在区域内
    public override void updateArea(Vector3 sorcePosition,Vector3 targetPosition)
    {
        if (!m_transform) return;
        m_targetDir = targetPosition - sorcePosition;
        if (m_targetDir.sqrMagnitude > 0.001f)
        {
            m_targetDir.y = 0;
            m_targetDir.Normalize();
            m_transform.rotation = Quaternion.LookRotation(new Vector3(0, -1, 0), m_targetDir);            
        }

        Vector3 center = sorcePosition + (m_targetDir * m_rectSelectDis / 2.0f);
        m_rectEndPos = sorcePosition + (m_targetDir * m_rectSelectDis);
        m_rectStartPos = sorcePosition;
        this.position = center;
        //this.lookAt = targetPosition;
    }

    public override bool isInArea(Vector3 targetPos)
    {
        if (!m_transform) return false;
        Vector3 startPosition = m_rectStartPos;
        Vector3 endPosition = m_rectEndPos;        
        startPosition.y=0.0f;
        endPosition.y = 0.0f;
        targetPos.y = 0.0f;
        if (Vector3.Distance(startPosition, targetPos) > m_rectSelectDis + m_rectSelectWidth) return false; //粗略距离判断
        float scale = 0.0f;
        float dis = DisPoint2Segment(targetPos, startPosition, m_rectEndPos, ref scale);
        if (scale < 0.0f || scale > 1.0f) return false; //垂足不在0-1范围，所以在矩形区域外
        return dis < m_rectSelectWidth / 2.0f;
    }

    private float DisPoint2Segment(Vector3 pointPos, Vector3 segStartPos, Vector3 segEndPos, ref float scale)
    {
        //用投影法解点到线段之间的距离
        //参考http://blog.sina.com.cn/s/blog_5d5c80840101bnhw.html，将二维扩展为三维

        Vector3 AB = segEndPos - segStartPos;
        Vector3 AC = pointPos - segStartPos;

        float dot = Vector3.Dot(AB, AC);
        if (dot < 0)
        {
            //AC在AB的反方向，最短距离为AC
            scale = -1.0f;
            return AC.magnitude;
        }

        float d2 = AB.sqrMagnitude;
        if (dot > d2)
        {
            //投影比AB长，最短距离为BC
            scale = 2.0f;
            return Vector3.Distance(pointPos, segEndPos);
        }

        float r = dot / d2;
        Vector3 projectPos = segStartPos + AB * r;
        return Vector3.Distance(pointPos, projectPos);
    }
}

public class SelectEffect_Fan:SelectEffect
{
    public float m_fanAngle;

    private const float SPELL_SECTOR_START_LEN = 1.5f;
    private void SetFanAngle(float angle, float radius)
    {
        m_fanAngle = angle;
        if (m_transform)
        {
            Projector projector = m_transform.GetComponent<Projector>();
            if (projector == null)
            {
                Trace.Log("projector is null");
                return;
            }
            Material mat = projector.material;
            if (mat == null)
            {
                Trace.Log("Projector Material is null");
            }
            mat.SetFloat("_CosAngle", Mathf.Cos(angle));
            mat.SetFloat("_NearRadius", SPELL_SECTOR_START_LEN / radius);
        }
    }

    private float m_cosFanAngle = 0.0f; //扇形角度的angle值，用来判断是否在区域内
    private Vector3 m_sourcePos;
    private float m_selectDistance = 0.0f;
    public override void buildArea(float selectDistance, float attackRange)
    {
        float angle=attackRange;
        m_cosFanAngle = Mathf.Cos(Mathf.Deg2Rad * angle);
        float rad=selectDistance;
        if (!m_transform) return;
        SetFanAngle(Mathf.Deg2Rad*angle,rad);
        Projector projector = m_transform.gameObject.GetComponent<Projector>();
        if (projector) projector.orthographicSize = rad;
        if (isMaskBuilding)
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers & 0 << layerMask;
        }
        else
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers | 1 << layerMask;
        }
        m_selectDistance = rad;
    }

    private Vector3 m_selectDir; //扇形中心线方向，用于判断是否在区域内
    public override void updateArea(Vector3 sourcePosition, Vector3 targetPosition)
    {
        Vector3 curDir = new Vector3(0, 0, 1);
        Vector3 targetDir = targetPosition - sourcePosition;
        if (targetDir.sqrMagnitude > 0.001f)
        {
            targetDir.y = 0;
            targetDir.Normalize();
            m_transform.rotation = Quaternion.LookRotation(new Vector3(0, -1, 0), targetDir);
        }
        position = sourcePosition;
        m_sourcePos = sourcePosition;
        m_selectDir = targetDir;
        m_selectDir.y = 0.0f;
        m_selectDir.Normalize();
    }

    public override bool isInArea(Vector3 targetPos)
    {
        Vector3 startPosition = m_sourcePos;
        if (!m_transform) return false;
        startPosition.y = 0.0f;
        targetPos.y = 0.0f;
        if (Vector3.Distance(startPosition, targetPos) > m_selectDistance) return false; //粗略距离判断
        Vector3 targetDir = targetPos - m_sourcePos;
        targetDir.y = 0.0f;
        targetDir.Normalize();
        float cosAngle = Vector3.Dot(targetDir, m_selectDir);
        if (cosAngle < m_cosFanAngle) return false;
        return true;
    }
}

public class SelectEffect_Circle:SelectEffect
{
    private float m_rad;
    private Vector3 m_centerPos;
    public override void buildArea(float selectDistance, float attackRange)
    {
        float rad = attackRange;

        if (0 != (nDrawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_SPELL_DISTANCE))
        {
            rad=selectDistance;
        }
        
        if (!m_transform) return;
        Projector projector = m_transform.gameObject.GetComponent<Projector>();
        if (projector) projector.orthographicSize = rad;
        if (isMaskBuilding)
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers & 0 << layerMask;
        }
        else
        {
            int layerMask = LayerMask.NameToLayer("Building");
            projector.ignoreLayers = projector.ignoreLayers | 1 << layerMask;
        }
        if (m_transform.childCount >= 2) {
            m_transform.GetChild(1).gameObject.SetActive(false);         //默认第二个
        }
        m_rad = rad;
    }
    public override void updateArea(Vector3 sourcePosition,Vector3 targetPosition)
    {
        if (0 != (nDrawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_SELF))
        {
            position = sourcePosition;
            m_centerPos = sourcePosition;
        }
        else
        {
            if(isAnotherPlane)
            {
                UpdateOtherPlane(position, targetPosition);
            }
            position = targetPosition;
            m_centerPos = targetPosition;
        }
    }

    private GameObject CircleOtherPlane = null;

    private void UpdateOtherPlane(Vector3 oriPos,Vector3 newPos)
    {

        if (m_transform!=null && m_transform.childCount >= 2)
        {
            if(CircleOtherPlane==null)
            {
                CircleOtherPlane = GameObject.CreatePrimitive(PrimitiveType.Quad);
                CircleOtherPlane.transform.localScale = Vector3.one * m_rad;
                Object.Destroy(CircleOtherPlane.GetComponent<MeshCollider>());
                CircleOtherPlane.GetComponent<MeshRenderer>().material = m_transform.GetChild(1).GetComponent<MeshRenderer>().material;
                //new Material(AssetBundleManager.GetShader("Unlit/Transparent"));
                CircleOtherPlane.transform.eulerAngles = Vector3.right * 90;
            }
            else
            {
                CircleOtherPlane.SetActive(true);
            }
            CircleOtherPlane.transform.position = newPos + Vector3.up * 0.1f;
        }
    }
    public override bool isInArea(Vector3 targetPos)
    {
        Vector3 startPosition = m_centerPos;
        if (!m_transform) return false;
        startPosition.y = 0.0f;
        targetPos.y = 0.0f;
        if (Vector3.Distance(startPosition, targetPos) > m_rad) return false; //距离判断
        return true;
    }

    public override void OnDisable()
    {
        if (CircleOtherPlane != null)
        {
            CircleOtherPlane.SetActive(false);
        }
    }

    private Vector3 ProjectToFloor(Vector3 OriginalPos)
    {
        Vector3 posAfterProject = Vector3.zero;
        //int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault)) | (1 << LayerMask.NameToLayer(Config.LayerBuilding));
        int LayerMaskOnGround = (1 << LayerMask.NameToLayer(Config.LayerDefault));
        RaycastHit hitInfo;

        Ray downRay = new Ray(OriginalPos + new Vector3(0, 1.0f, 0), new Vector3(0, -1, 0));
        bool hitGround = Physics.Raycast(downRay, out hitInfo, 1000, LayerMaskOnGround);
        if (hitGround && hitInfo.point.y < OriginalPos.y)
        {
            posAfterProject = hitInfo.point;
        }
        return posAfterProject;
    }
}

#endregion


// 技能选择光圈管理
public class SelectEffectManager
{
    private Dictionary<int, SelectEffect> m_selectEffectMap;

    private Transform m_heroTransform;
    private Transform m_selectEffectRoot;
    private int m_selectLayerFilter; //过滤掉光效、透明等层，供选取用
    public void setHeroTransform(Transform heroTransform)
    {
        m_heroTransform = heroTransform;
    }

    public void initSelectEffectManager(Transform heroTransform,string rootName)
    {
        if (!heroTransform) return;

        if (m_selectEffectMap==null) m_selectEffectMap = new Dictionary<int, SelectEffect>();
        if (m_outlineList == null) m_outlineList = new HashSet<int>();

        if (!m_selectEffectRoot)
        {
            GameObject SelectEffectRoot = new GameObject(rootName);
            m_selectEffectRoot = SelectEffectRoot.transform;
            UnityEngine.GameObject.DontDestroyOnLoad(SelectEffectRoot);
        }

        m_heroTransform = heroTransform;
        m_selectEffectMap.Clear();
        SelectEffect_Fan fanEffect = new SelectEffect_Fan();
        fanEffect.loadEffect("CommonPrefab/Effect/selectEffect/skillFanPref", m_selectEffectRoot);
        fanEffect.effectName="fan";
        m_selectEffectMap[(int)SPELL_DRAW_TYPE.DRAW_TYPE_SECTOR] = fanEffect;
        SelectEffect_Circle circleEffect = new SelectEffect_Circle();
        circleEffect.loadEffect("CommonPrefab/Effect/selectEffect/skillCirclePref", m_selectEffectRoot);
        circleEffect.effectName="circle";
        m_selectEffectMap[(int)SPELL_DRAW_TYPE.DRAW_TYPE_ROUND] = circleEffect;
        SelectEffect_Rect rectEffect = new SelectEffect_Rect();
        rectEffect.loadEffect("CommonPrefab/Effect/selectEffect/skillLinePref", m_selectEffectRoot);
        rectEffect.effectName="rect";
        m_selectEffectMap[(int)SPELL_DRAW_TYPE.DRAW_TYPE_RECT] = rectEffect;
        SelectEffect_Circle distanceEffect = new SelectEffect_Circle();
        distanceEffect.loadEffect("CommonPrefab/Effect/selectEffect/distanceCirclePref", m_selectEffectRoot);
        distanceEffect.effectName = "distanceCircle";
        m_selectEffectMap[(int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_DISTANCE] = distanceEffect;


        m_selectLayerFilter = -1;
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Friend)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Enemy)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerTransparentFX)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)));
        m_selectLayerFilter = m_selectLayerFilter & (~(1 << LayerMask.NameToLayer(Config.LayerIngoreRayCast)));
    }

    public void clearEffects()
    {
        foreach (KeyValuePair<int, SelectEffect> keypair in m_selectEffectMap)
        {
            SelectEffect effect = keypair.Value;
            effect.destroy();
        }
        if (m_selectEffectRoot)
        {
            var temp = m_selectEffectRoot.gameObject;
            ResNode.DestroyRes(ref temp);
        }

        m_selectEffectMap.Clear();
    }

    private SelectEffect m_currentEfffect;
    private SelectEffect m_currentDistanceEfffect;

    private int m_drawType;
    private float m_selectDistance;
    private float m_attackRange;
    private int m_nTargetFilter;
    public void enableEffect(int effectType,int drawType,float selectDistance,float attackRange,float chooseRange,int nTargetFilter,int selectType)
    {
        if (m_currentEfffect!=null)
        {
            disableEffect();
        }

        if (0 != (drawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_CHOOSE_RANGE))
        {
            attackRange = chooseRange;
        }
        bool isAnotherPlane = false;
        //水平光圈提示
        if (drawType!=0 && 0 != (drawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_HORIZONTAL_APERTURE))
        {
            isAnotherPlane = true;
        }
        bool isMaskBuilding = false;
        //取点时检测建筑层
        if (selectType!=0 && (selectType & (int)SPELL_SELECT_TYPE.SpellSelectType_HeroProjectToFloor) != 0)
        {
            isMaskBuilding = true;
        }

        if (m_selectEffectMap == null)
        {
            return;
        }

        m_drawType = drawType;
        m_selectDistance = selectDistance;
        m_attackRange = attackRange;
        m_nTargetFilter = nTargetFilter;
        if (m_selectEffectMap.ContainsKey(effectType))
        {
            m_currentEfffect = m_selectEffectMap[effectType];
            m_currentEfffect.isAnotherPlane = isAnotherPlane;
            m_currentEfffect.isMaskBuilding = isMaskBuilding;
            m_currentEfffect.active = true;
            m_currentEfffect.nDrawType = drawType;
            m_currentEfffect.buildArea(selectDistance, attackRange);
        }

        if ((m_selectEffectMap.ContainsKey((int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_DISTANCE)) && (0 != (m_drawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_DISTANCE)))
        {
            m_currentDistanceEfffect = m_selectEffectMap[(int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_DISTANCE];
            m_currentDistanceEfffect.active = true;
            m_currentDistanceEfffect.isAnotherPlane = isAnotherPlane;
            m_currentDistanceEfffect.isMaskBuilding = isMaskBuilding;
            m_currentDistanceEfffect.nDrawType = drawType;
            m_currentDistanceEfffect.buildArea(selectDistance, selectDistance);
        }
    }

    public void disableEffect()
    {
        if (m_currentEfffect != null)
        {
            m_currentEfffect.active = false;
            m_currentEfffect = null;
        }
        if (m_currentDistanceEfffect != null)
        {
            m_currentDistanceEfffect.active = false;
            m_currentDistanceEfffect = null;
        }
        clearOutline();
    }

    public void updateEffect(Vector3 targetPos)
    {
        if (m_currentEfffect != null)
        {
            m_currentEfffect.updateArea(m_heroTransform.position, targetPos);

            if (0 != (m_drawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_OUTLINE))
            {
                updateOutline(m_currentEfffect);
            }
        }


        if (m_currentDistanceEfffect != null)
        {
            if (0 != (m_drawType & (int)SPELL_DRAW_TYPE.DRAW_TYPE_DRAW_DISTANCE))
            {
                m_currentDistanceEfffect.updateArea(m_heroTransform.position, m_heroTransform.position);
            }
        }       
        
    }

    private HashSet<int> m_outlineList;
    private void updateOutline(SelectEffect effect)
    {
        if (effect == null) return;

        ICollection<U3D_Render.EntityView> all_entities = EntityFactory.GetEntityList();
        foreach (U3D_Render.EntityView ev in all_entities)
        {
            if (ev == null) continue;

            int entity_id = ev.ID;


            if (!ev.gameObject) continue;
            int layer = ev.gameObject.layer;
            if (0 == ((1 << layer) & m_selectLayerFilter)) continue;


            bool select = (ScreenRaycast.checkTargetFilter(ev, m_nTargetFilter));
            select = select && (effect.isInArea(ev.gameObject.transform.position));
            if (select)
            {
                if (!m_outlineList.Contains(entity_id))
                {
                    ev.StateMachine.outLineCount++;
                    m_outlineList.Add(entity_id);
                }
            }
            else
            {
                if (m_outlineList.Contains(entity_id))
                {
                    ev.StateMachine.outLineCount--;
                    m_outlineList.Remove(entity_id);
                }
            }

        }

    }

    private void clearOutline()
    {
        foreach (int entity_id in m_outlineList)
        {
            U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entity_id);
            if (ev != null)
            {
                ev.StateMachine.outLineCount--;
            }
        }
        m_outlineList.Clear();
    }
}

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using ImageEffects;
using U3D_Render;

public class WarFogManager : MonoBehaviour
{
    public static WarFogManager Instance = null;

    private Dictionary<GameObject, bool> fogobjs = new Dictionary<GameObject, bool>();
    public bool b3DWarFog = true;
    public float RangeStart = 0.35f;
    public float RangeEnd = 0.55f;
    //能看到哪些层的对象
    public LayerMask MyCampLayer = 0;

    public void Awake()
    {
        Instance = this;

        MyCampLayer = MyCampLayer.value | (1 << LayerMask.NameToLayer(Config.LayerHero));
        MyCampLayer = MyCampLayer.value | (1 << LayerMask.NameToLayer(Config.LayerPlayerFriend));
        MyCampLayer = MyCampLayer.value | (1 << LayerMask.NameToLayer(Config.LayerMonsterFriend));

    }

    public void OnDestroy()
    {
        Instance = null;
    }

    public void Add(GameObject go)
    {
        if (go == null) return;
        fogobjs[go] = true;
    }

    public void Remove(GameObject go)
    {
        fogobjs.Remove(go);
    }

    public void Clear()
    {
        fogobjs.Clear();
    }

    private WarFog warfog = null;
    public void Begin()
    {
        if (warfog == null)
        {
            warfog = ImageEffectManager.API_Add<WarFog>(SceneResInfoManager.CurrentActiveInstance.SceneCamera);
        }
        ImageEffectManager.API_EnableImageEffect<WarFog>();
        ImageEffectManager.API_DisableImageEffect<GlobalFog>();
        ImageEffectManager.API_DisableImageEffect<RadialBlur>();
        Refresh();
    }

    void Refresh()
    {
        Clear();
        ICollection<EntityView> entities = EntityFactory.GetEntityList();
        foreach (EntityView ev in entities)
        {
            // if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
            if ((ev.gameObject != null) && ((MyCampLayer.value & (1 << ev.gameObject.layer)) > 0))
            {
                WarFogManager.Instance.Add(ev.gameObject);
            }
        }
    }

    public void End()
    {
        ImageEffectManager.API_EnableImageEffect<GlobalFog>();
        ImageEffectManager.API_DisableImageEffect<WarFog>();

        Clear();
    }

    public void Update()
    {
        if (warfog != null)
        {
            warfog.Clear();
            foreach (GameObject go in fogobjs.Keys)
            {
                if (go != null)
                {
                    if (b3DWarFog)
                    {
                        warfog.Add(go.transform.position, RangeStart, RangeEnd, true);
                    }
                    else
                    {
                        Vector3 screenpos = Initialize.mainCam.WorldToScreenPoint(go.transform.position);
                        warfog.Add(screenpos, RangeStart, RangeEnd, false);
                    }
                }
            }
        }
    }
}

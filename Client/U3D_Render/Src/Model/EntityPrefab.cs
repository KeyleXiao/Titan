using UnityEngine;
using System.Collections;

/// <summary>
/// 实体预制体，主要是用来区分是那个类型的prefab
/// </summary>
public class EntityPrefab : MonoBehaviour
{

    public ENTITY_TYPE EntityType = ENTITY_TYPE.TYPE_PLAYER_ROLE;
}

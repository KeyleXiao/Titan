using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;

// 光效工厂 
// 全局单一类，整个游戏只有一个实例
// 负责载入光效配置，创建光效

public enum LightingEffectConfigType
{
    Invalid = 0,
    Bind,
    Fly,
    AniReplace,
    Area,
    //MatReplace,
    Camera,
    Shadow,
    Sound,
    Speed,
    Ward,
}

public partial class LightingEffectFactory : MonoBehaviour
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static LightingEffectFactory Instance = null;


    /// <summary>
    /// 检测层
    /// </summary>
    private LayerMask collisionLayers = -1;
    public int LayerMaskOnCollision;  //选择实体的时候用这个Mask
    public int LayerMaskOnGround; //选择除玩家外的时候用这个Mask，穿过玩家碰地
    public int LayerMaskOnPlayerOrMonster;  //选择玩家或者怪物的时候用这个Mask
    public int LayerMaskOnPlayer;  //选择玩家的时候用这个Mask
    public int LayerMaskOnMonster;  //选择怪物的时候用这个Mask


    public void Awake()
    {
        LayerMaskOnCollision = collisionLayers.value & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerTransparentFX)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)));
        LayerMaskOnCollision = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerIngoreRayCast)));


        LayerMaskOnGround = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        LayerMaskOnGround = LayerMaskOnGround & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        LayerMaskOnGround = LayerMaskOnGround & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        LayerMaskOnGround = LayerMaskOnGround & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));


        LayerMaskOnPlayerOrMonster = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerDefault)));
        LayerMaskOnPlayerOrMonster = LayerMaskOnPlayerOrMonster & (~(1 << LayerMask.NameToLayer(Config.LayerBuilding)));

        LayerMaskOnPlayer = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerDefault)));
        LayerMaskOnPlayer = LayerMaskOnPlayer & (~(1 << LayerMask.NameToLayer(Config.LayerBuilding)));
        LayerMaskOnPlayer = LayerMaskOnPlayer & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        LayerMaskOnPlayer = LayerMaskOnPlayer & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        LayerMaskOnPlayer = LayerMaskOnPlayer & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));

        LayerMaskOnMonster = LayerMaskOnCollision & (~(1 << LayerMask.NameToLayer(Config.LayerDefault)));
        LayerMaskOnMonster = LayerMaskOnMonster & (~(1 << LayerMask.NameToLayer(Config.LayerBuilding)));
        LayerMaskOnMonster = LayerMaskOnMonster & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        LayerMaskOnMonster = LayerMaskOnMonster & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        LayerMaskOnMonster = LayerMaskOnMonster & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));

        Instance = this;
        InitCache();
	}

    public void OnDestroy()
    {
        ClearCache();
    }

    public void Update()
    {
        UpdateAsync();
    }

    

    public bool IsSingleEffect (int nID)
    {
        if (nID <= 0)
        {
            return false;
        }
        LightingEffect ef =  ASpeedGame.Data.Scheme.SchemeLightingEffect.Instance.GetLightingEffect(nID);
        if (!ef)
        {
            return false;
        }

        return ef.bSingle;
    }


    private void LoadByEditorImp(int effectID)
    {
        ClearSingleCache(effectID);
        PreWarmEffect(effectID, 1);
    }

    public static void LoadByEditor(int id)
    {
        if (!Application.isPlaying)
        {
            return;
        }
        if (Instance != null)
        {
            Instance.LoadByEditorImp(id);
        }
    }
  
}

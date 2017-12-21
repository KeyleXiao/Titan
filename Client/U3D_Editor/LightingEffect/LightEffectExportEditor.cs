using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEditor;


public class LightingEffectExportEditor : EditorWindow
{
    public const string schemePath = "/Scheme/LightingEffect";
    public const string outPutPath = "/Data/Scp/";
    public const string schemeFileName = "LightingEffect.csv";
    public const string schemeExtension = ".asset";

    public static string configPath = "";

    private static StreamWriter streamWriter;
    private static Dictionary<LightingEffectConfigType, List<LightingEffect>> lightEffectConfigMap = new Dictionary<LightingEffectConfigType, List<LightingEffect>>();


    [MenuItem("Tools/LightingEffect Export")]
    public  static void Init()
    {
        LightingEffectExportEditor editor = EditorWindow.GetWindow<LightingEffectExportEditor>(false, "光效配置导出", true);
        editor.Show();
        editor.autoRepaintOnSceneChange = true;
    }


    public void OnEnable()
    {
        configPath = Application.dataPath + schemePath;
    }


    public void OnGUI()
    {
        if (Application.isPlaying)
        {
            EditorGUILayout.LabelField("不能在播放模式下进行导出");
            return;
        }

        GUILayout.Space(10.0f);  
        GUILayout.Label("光效配置根目录：");

        GUILayout.Space(5.0f);
        GUILayout.BeginHorizontal();
        configPath = EditorGUILayout.TextField(configPath);
        GUILayout.EndHorizontal();

        GUILayout.Space(15.0f);
        if (GUILayout.Button("导出配置"))
        {
            ExportLightEffect(false);
        }
    }


    public static void ExportLightEffect(bool autoBulid = true)
    {
        Clear();

        if (string.IsNullOrEmpty(configPath))
            configPath = Application.dataPath + schemePath;

        if (!Directory.Exists(configPath))
        {
            if (autoBulid)
            {
                EditorLog.LogError("光效配置根目录不存在！");
            }
            else
            {
                EditorUtility.DisplayDialog("配置导出", "光效配置根目录不存在，请检查!", "OK");
            }

            return;
        }

        string[] schemeArray = Directory.GetFiles(configPath, "*" + schemeExtension, SearchOption.AllDirectories);
        foreach (string strSchemePath in schemeArray)
        {
            if(string.IsNullOrEmpty(strSchemePath) || !strSchemePath.EndsWith(schemeExtension))
                continue;

            string strTempPath = strSchemePath.Replace(@"\", "/");
            strTempPath = strTempPath.Substring(strTempPath.IndexOf("Assets"));

            UnityEngine.Object schemeObj = AssetDatabase.LoadAssetAtPath(@strTempPath, typeof(UnityEngine.Object));
            if(schemeObj != null)
            {
                LightingEffect scheme = schemeObj as LightingEffect;
                if(scheme != null)
                {
                    LightingEffectConfigType type = scheme.GetEffectType();
                    if(!lightEffectConfigMap.ContainsKey(type))
                        lightEffectConfigMap.Add(type, new List<LightingEffect>());

                    lightEffectConfigMap[type].Add(scheme);
                }
            }
        }

        lightEffectConfigMap = lightEffectConfigMap.OrderBy(o => o.Key).ToDictionary(o => o.Key, p => p.Value);

        InitStreamWriter();

        foreach (KeyValuePair<LightingEffectConfigType, List<LightingEffect>> kvp in lightEffectConfigMap)
        {
            WriteSchemeHead(kvp.Key);

            foreach (LightingEffect item in kvp.Value)
            {
                ExportScheme(kvp.Key, item);
            }

            FlushStreamWriter();
        }

        FreeStreamWriter();
        Clear();

        if(!autoBulid)
            EditorUtility.DisplayDialog("配置导出", "光效配置导出完成!", "OK");
    }


    private static void Clear()
    {
        foreach (List<LightingEffect> schemeList in lightEffectConfigMap.Values)
        {
            schemeList.Clear();
        }

        lightEffectConfigMap.Clear();
    }


    private static void ExportScheme(LightingEffectConfigType type, LightingEffect scheme)
    {
        switch(type)
        {
            case LightingEffectConfigType.Bind:
                {
                    ExportScheme_Bind(scheme);
                }
                break;
            case LightingEffectConfigType.Fly:
                {
                    ExportScheme_Fly(scheme);
                }
                break;
            case LightingEffectConfigType.AniReplace:
                {
                    ExportScheme_AniReplace(scheme);
                }
                break;
            case LightingEffectConfigType.Area:
                {
                    ExportScheme_Area(scheme);
                }
                break;
            case LightingEffectConfigType.Camera:
                {
                    ExportScheme_Camera(scheme);
                }
                break;
            case LightingEffectConfigType.Shadow:
                {
                    ExportScheme_Shadow(scheme);
                }
                break;
            case LightingEffectConfigType.Sound:
                {
                    ExportScheme_Sound(scheme);
                }
                break;
            case LightingEffectConfigType.Speed:
                {
                    ExportScheme_Speed(scheme);
                }
                break;
            case LightingEffectConfigType.Ward:
                {
                    ExportScheme_Ward(scheme);
                }
                break;
            case LightingEffectConfigType.Invalid:
            default:
                break;
        }
    }


    private static void ExportScheme_Bind(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Bind bindScheme = scheme as LightingEffect_Bind;
        if (bindScheme == null)
            return;
               
        //bind
        string createOutScreen = (bindScheme.bCreateOutsideScreen ? 1 : 0).ToString();
        string scale = bindScheme.scale.ToString();
        string startTick = bindScheme.effectStartTick.ToString();
        string liveTick = bindScheme.effectLiveTick.ToString();
        string dispearTick = bindScheme.effectDispearTick.ToString();
        string posBindBone = bindScheme.posBindBone;
        string posOffset = bindScheme.posOffset.x + ";" + bindScheme.posOffset.y + ";" + bindScheme.posOffset.z;
        string rotBinBone = bindScheme.rotationBindBone;
        string rotOffset = bindScheme.rotationOffset.x + ";" + bindScheme.rotationOffset.y + ";" + bindScheme.rotationOffset.z;
        string faceToCam = (bindScheme.faceToCamera ? 1 : 0).ToString();
        string offsetToSurface = (bindScheme.offsetToSurface ? 1 : 0).ToString();
        string bindType = ((int)bindScheme.bindType).ToString();
        string faceAgnistTarget = (bindScheme.faceAganistTarget ? 1 : 0).ToString();
        string projOnFloor = (bindScheme.bProjectOnFloor ? 1 : 0).ToString();
        string faceToWalkingDir = (bindScheme.faceToWalkingDirection ? 1 : 0).ToString();
        string notRotate = (bindScheme.bNotRotate ? 1 : 0).ToString(); 
        string bindTargetToSrc = (bindScheme.bindTargetToSource ? 1 : 0).ToString();
        string bindTargetBone = bindScheme.bindTargetBone;
        string triggerHitDetect = (bindScheme.triggerHitDetect ? 1 : 0).ToString();
        string sound = bindScheme.SoundID.ToString();
        string posBindwound = bindScheme.posBindWound;


        ExportSchemeInfo(bindScheme, createOutScreen, scale, startTick, liveTick, dispearTick, posBindBone, posOffset, rotBinBone, rotOffset, faceToCam, offsetToSurface, 
                         bindType, faceAgnistTarget, projOnFloor, faceToWalkingDir, notRotate, bindTargetToSrc, bindTargetBone, triggerHitDetect, sound, posBindwound);  
    }


    private static void ExportScheme_Fly(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Fly flyScheme = scheme as LightingEffect_Fly;
        if (flyScheme == null)
            return;

        //fly
        string scale = flyScheme.scale.ToString();
        string startTick = flyScheme.effectStartTick.ToString();
        string liveTick = flyScheme.effectLiveTick.ToString();
        string residenceTick = flyScheme.residenceTick.ToString();
        string dispearTick = flyScheme.effectDispearTick.ToString();
        string maxDistance = flyScheme.maxDistance.ToString();
        string posBindBone = flyScheme.posBindBone;
        string rotationBindBone = flyScheme.rotationBindBone;
        string posOffset = flyScheme.posOffset.x + ";" + flyScheme.posOffset.y + ";" + flyScheme.posOffset.z;
        string startPosOffset = flyScheme.startPosOffset.x + ";" + flyScheme.startPosOffset.y + ";" + flyScheme.startPosOffset.z;
        string targetTrsBone = flyScheme.targetTransformBone;
        string targetOffset = flyScheme.targetOffset.x + ";" + flyScheme.targetOffset.y + ";" + flyScheme.targetOffset.z;
        string flyType = ((int)flyScheme.flyType).ToString();
        string StartType = ((int)flyScheme.startType).ToString();
        string faceTarget = (flyScheme.faceTarget ? 1 : 0).ToString();
        string lowestHeight = flyScheme.lowestHeight.ToString();
        string highestHeight = flyScheme.highestHeight.ToString();
        string flySpeed = flyScheme.flySpeed.ToString();
        string absFlyTime = flyScheme.AbsFlyTime.ToString();
        string destroyWhenReached = (flyScheme.destroyWhenReached ? 1 : 0).ToString();
        string destroyWhenTargetDead = (flyScheme.destroyWhenTargetDead ? 1 : 0).ToString();
        string useCollision = (flyScheme.useCollision ? 1 : 0).ToString();
        string destroyWhenCollision = (flyScheme.destroyWhenCollision ? 1 : 0).ToString();
        string reviseFly = (flyScheme.reviseFly ? 1 : 0).ToString();
        string effectReturn = (flyScheme.effectReturn ? 1 : 0).ToString();
        string closeReturn = (flyScheme.closeReturn ? 1 : 0).ToString();
        string returnSpeed = flyScheme.returnSpeed.ToString();
        string faceSrcToTarget = (flyScheme.faceSourceToTarget ? 1 : 0).ToString();
        string effIDHitPlayer = flyScheme.effectIDWhenHitPlayer;
        string effIDHitMonster = flyScheme.effectIDWhenHitMonster;
        string effIDHitGroud = flyScheme.effectIDWhenHitGround;
        string effIDReach = flyScheme.effectIDWhenReached;
        string triggerHitDetect = (flyScheme.triggerHitDetect ? 1 : 0).ToString();
        string playAnimWhenClose = flyScheme.playAnimWhenClose;
        string upcastMotion = (flyScheme.upcastMotion ? 1 : 0).ToString();
        string upcastHeight = flyScheme.upcastHeight.ToString();
        string minUpcastAngle = flyScheme.minUpcastAngle.ToString();
        string maxUpcastAngle = flyScheme.maxUpcastAngle.ToString();
        string upcastTopPrec = flyScheme.upcastTopPrec.ToString();
        string sound = flyScheme.SoundID.ToString();
        string delayDestroyTick = flyScheme.delayDestroyTick.ToString();
        string delayHideTick = flyScheme.delayHideTick.ToString();
        string attackToObj = (flyScheme.attackToObject ? 1 : 0).ToString();
        string affectShield = (flyScheme.AffectedShield ? 1 : 0).ToString();

        ExportSchemeInfo(flyScheme, scale, startTick, liveTick, residenceTick, dispearTick, maxDistance, posBindBone, rotationBindBone, posOffset, startPosOffset, targetTrsBone,
                            targetOffset, flyType,StartType, faceTarget, lowestHeight, highestHeight, flySpeed, absFlyTime, destroyWhenReached, destroyWhenTargetDead, useCollision,
                            destroyWhenCollision, reviseFly, effectReturn, closeReturn, returnSpeed, faceSrcToTarget, effIDHitPlayer, effIDHitMonster, effIDHitGroud, effIDReach,
                            triggerHitDetect, playAnimWhenClose, upcastMotion, upcastHeight, minUpcastAngle, maxUpcastAngle, upcastTopPrec, sound, delayDestroyTick, delayHideTick,
                            attackToObj, affectShield);  
    }


    private static void ExportScheme_AniReplace(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_AnimationReplace aniReplaceScheme = scheme as LightingEffect_AnimationReplace;
        if (aniReplaceScheme == null)
            return;

        //animationReplace
        string startTick = aniReplaceScheme.nStartTick.ToString();
        string endTick = aniReplaceScheme.nEndTick.ToString();

        ExportSchemeInfo(aniReplaceScheme, startTick, endTick);
    }


    private static void ExportScheme_Area(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Area areaScheme = scheme as LightingEffect_Area;
        if (areaScheme == null)
            return;

        //Area
        string startTick = areaScheme.effectStartTick.ToString();
        string intervalTick = areaScheme.effectIntervalTick.ToString();
        string subEffectID = areaScheme.subEffectID.ToString();
        string posBindBone = areaScheme.posBindBone;
        string customOffset = areaScheme.customOffset.x + ";" + areaScheme.customOffset.y + ";" + areaScheme.customOffset.z;
        string projOnGround = (areaScheme.projectOnGround ? 1 : 0).ToString();
        string extentToMaxDis = (areaScheme.extentToMaxDistance ? 1 : 0).ToString();
        string effectCount = areaScheme.effectCount.ToString();
        string effectTotalTick = areaScheme.effectTotalTick.ToString();

        ExportSchemeInfo(areaScheme, startTick, intervalTick, subEffectID, posBindBone, customOffset, projOnGround, extentToMaxDis, effectCount, effectTotalTick);
    }


    private static void ExportScheme_Camera(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Camera camScheme = scheme as LightingEffect_Camera;
        if (camScheme == null)
            return;

        //cam
        string animOrgPos = ((int)camScheme.animationOrgPos).ToString();
        string animOrgProjFloor = (camScheme.animationOrgProjectFloor ? 1 : 0).ToString();
        string animMainCam = (camScheme.animationMainCamera ? 1 : 0).ToString();
        string attackType = ((int)camScheme.attackType).ToString();
        string effectRad = camScheme.effectRad.ToString();
        string effectLiveTick = camScheme.effectLiveTick.ToString();
        string delayTick = camScheme.delayTick.ToString();
        string effectFinishTick = camScheme.effectFinishTick.ToString();
        string recoverTrs = (camScheme.recoverTransform ? 1 : 0).ToString();

        ExportSchemeInfo(camScheme, animOrgPos, animOrgProjFloor, animMainCam, attackType, effectRad, effectLiveTick, delayTick, effectFinishTick, recoverTrs);
    }


    private static void ExportScheme_Shadow(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Shadow shadowScheme = scheme as LightingEffect_Shadow;
        if (shadowScheme == null)
            return;

        //shadow
        string startTick = shadowScheme.nStartTick.ToString();
        string endTick = shadowScheme.nEndTick.ToString();
        string startAlpha = shadowScheme.fStartAlpha.ToString();
        string shadowMaintainTick = shadowScheme.nShadowMaintainTick.ToString();
        string shadowFadeoutTick = shadowScheme.nShadowFadeoutTick.ToString();
        string shadowIntervTick = shadowScheme.shadowIntervalTicks.ToString();
        string shadowFrameTick = "";
        for(int i = 0; i < shadowScheme.shadowFrameTicks.Length; i++)
        {
            shadowFrameTick += shadowScheme.shadowFrameTicks[i];
            if (i != shadowScheme.shadowFrameTicks.Length - 1)
                shadowFrameTick += ";";
        }
        string alphaAttName = shadowScheme.alphaAttributeName;
        string disableWeapon = (shadowScheme.DisableWeapon ? 1 : 0).ToString();
        string alphaAttType = ((int)shadowScheme.alphaAttributeType).ToString();
        string minDis = shadowScheme.minDistance.ToString();

        ExportSchemeInfo(shadowScheme, startTick, endTick, startAlpha, shadowMaintainTick, shadowFadeoutTick, shadowIntervTick, shadowFrameTick,
                         alphaAttName, disableWeapon, alphaAttType, minDis);
    }


    private static void ExportScheme_Sound(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Sound soundScheme = scheme as LightingEffect_Sound;
        if (soundScheme == null)
            return;

        //sound
        string sound = soundScheme.soundID.ToString();

        ExportSchemeInfo(soundScheme, sound);
    }


    private static void ExportScheme_Speed(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Speed speedScheme = scheme as LightingEffect_Speed;
        if (speedScheme == null)
            return;

        //speed
        string startTick = speedScheme.nStartTick.ToString();
        string endTick = speedScheme.nEndTick.ToString();
        string globalSpeed = speedScheme.globalSpeed.ToString();
        string animSpeed = speedScheme.animatorSpeed.ToString();
        string effectRad = speedScheme.effectRad.ToString();

        ExportSchemeInfo(speedScheme, startTick, endTick, globalSpeed, animSpeed, effectRad);
    }


    private static void ExportScheme_Ward(LightingEffect scheme)
    {
        if (scheme == null)
            return;

        LightingEffect_Ward wardScheme = scheme as LightingEffect_Ward;
        if (wardScheme == null)
            return;

        //ward
        string startTick = wardScheme.nStartTick.ToString();
        string endTick = wardScheme.nEndTick.ToString();
        string blockAllCreature = (wardScheme.blockAllCreature ? 1 : 0).ToString();

        ExportSchemeInfo(wardScheme, startTick, endTick, blockAllCreature);
    }


    private static void ExportSchemeInfo(LightingEffect baseScheme, params string[] info)
    {
        StringBuilder sb = new StringBuilder();

        string effectCfgType = ((int)baseScheme.GetEffectType()).ToString();
        sb.Append(effectCfgType + ',');
        string nID = baseScheme.nID.ToString();
        sb.Append(nID + ',');
        string assetGuid = baseScheme.AssetGUID;
        sb.Append(assetGuid + ',');
        string desc = baseScheme.strDesc;
        sb.Append(desc + ',');
        string global = (baseScheme.bGlobal ? 1 : 0).ToString();
        sb.Append(global + ',');
        string single = (baseScheme.bSingle ? 1 : 0).ToString();
        sb.Append(single + ',');
        string recovereffect = (baseScheme.bRecoverSingleEffect ? 1 : 0).ToString();
        sb.Append(recovereffect + ',');
        string useSpeedScale = (baseScheme.bUseSpeedScale ? 1 : 0).ToString();
        sb.Append(useSpeedScale + ',');
        string ignoreEffOpt = (baseScheme.bIgnoreSingleEffectOptimtized ? 1 : 0).ToString();
        sb.Append(ignoreEffOpt + ',');
        string enableDisOpt = (baseScheme.EnableDistanceOptimtized ? 1 : 0).ToString();
        sb.Append(enableDisOpt + ',');
        string closeWhenDead = (baseScheme.CloseWhenDead ? 1 : 0).ToString();
        sb.Append(closeWhenDead + ',');
        string effectType = ((int)baseScheme.effectType).ToString();
        sb.Append(effectType + ',');

        foreach (string s in info)
        {
            sb.Append(s);
            sb.Append(',');
        }

        if(sb != null)
            WriteScheme(sb.ToString());
    }


    private static void WriteSchemeHead(LightingEffectConfigType cfgType)
    {
        if (streamWriter == null)
            return;

        StringBuilder nameBuilder = new StringBuilder();
        nameBuilder.Append("光效配置类型(1.Bind 2.Fly 3.AnimReplace 4.Area 5.Camera 6.Shadow 7.Sound 8.Speed 9.Ward)(int),");
        nameBuilder.Append("光效ID(int),");
        nameBuilder.Append("资源GUID(string),");
        nameBuilder.Append("光效描述(string),");
        nameBuilder.Append("是否全局光效(bool),");
        nameBuilder.Append("一个人物是否只能挂一个(bool),");
        nameBuilder.Append("是否覆盖旧参数(bool),");
        nameBuilder.Append("是否需要速度缩放(bool),");
        nameBuilder.Append("忽略单一光效优化(bool),");
        nameBuilder.Append("启用距离优化(bool),");
        nameBuilder.Append("施法者死亡关闭光效(bool),");
        nameBuilder.Append("光效类型(0.受击 1.普通)(int),");

        switch(cfgType)
        {
            case LightingEffectConfigType.Bind:
                {
                    nameBuilder.Append("屏幕外仍创建(bool),");
                    nameBuilder.Append("光效统一缩放比例(float),");
                    nameBuilder.Append("光效开始播放时间(毫秒)(int),");
                    nameBuilder.Append("光效存在时间(毫秒)(int),");
                    nameBuilder.Append("光效消失时间(毫秒)(int),");
                    nameBuilder.Append("光效位置绑定骨骼名字(string),");
                    nameBuilder.Append("光效相对骨骼的偏移(vector3),");
                    nameBuilder.Append("光效旋转绑定骨骼名字(string),");
                    nameBuilder.Append("光效相对绑定骨骼的旋转(vector3),");
                    nameBuilder.Append("光效面向镜头(bool),");
                    nameBuilder.Append("光效向镜头偏移到物体外部(bool),");
                    nameBuilder.Append("光效绑定类型(0.StayAlone 1.BindWithBone 2.StayInTargetPos)(int),");
                    nameBuilder.Append("光效面向(bool),");
                    nameBuilder.Append("光效贴地(bool),");
                    nameBuilder.Append("光效根据移动方向旋转(bool),");
                    nameBuilder.Append("光效不随人物旋转(bool),");
                    nameBuilder.Append("将target绑定到source上(bool),");
                    nameBuilder.Append("将target绑定到source上时target的骨骼名(string),");
                    nameBuilder.Append("使用碰撞盒检测光效是否到达(bool),");
                    nameBuilder.Append("音效ID(int)");
                }
                break;
            case LightingEffectConfigType.Fly:
                {
                    nameBuilder.Append("光效统一缩放比例(float),");
                    nameBuilder.Append("光效开始播放时间(毫秒)(int),");
                    nameBuilder.Append("光效存在时间(毫秒)(int),");
                    nameBuilder.Append("光效开始时原地滞留时间(毫秒)(int),");
                    nameBuilder.Append("光效消失时间(毫秒)(int),");
                    nameBuilder.Append("源点和目标点最大距离(float),");
                    nameBuilder.Append("光效位置绑定骨骼名字(string),");
                    nameBuilder.Append("光效旋转绑定骨骼名字(string),");
                    nameBuilder.Append("光效相对骨骼的偏移(vector3),");
                    nameBuilder.Append("光效开始时的偏移(vector3),");
                    nameBuilder.Append("飞向光效目标对象骨骼(string),");
                    nameBuilder.Append("目标点的偏移(vector3),");
                    nameBuilder.Append("光效飞行类型(0.TargetObject 1.TargetPosition 2.TargetPositionToTargetObject)(int),");
                    nameBuilder.Append("光效是否始终面向目标(bool),");
                    nameBuilder.Append("光效相对地面最低高度(float),");
                    nameBuilder.Append("光效相对地面最高高度(float),");
                    nameBuilder.Append("光效飞行速度(float),");
                    nameBuilder.Append("光效飞行时间(秒)(float),");
                    nameBuilder.Append("光效到达目的地时是否消失(bool),");
                    nameBuilder.Append("对手死亡时关闭光效(bool),");
                    nameBuilder.Append("光效是否进行碰撞检测(bool),");
                    nameBuilder.Append("光效产生碰撞时是否消失(bool),");
                    nameBuilder.Append("反路径飞行(bool),");
                    nameBuilder.Append("到达目的后返回(bool),");
                    nameBuilder.Append("光效关闭时返回(bool),");
                    nameBuilder.Append("光效返回时的飞行速度(float),");
                    nameBuilder.Append("始终有srcObject指向targetObject(bool),");
                    nameBuilder.Append("击中人时的后续光效(string),");
                    nameBuilder.Append("击中怪物时的后续光效(string),");
                    nameBuilder.Append("击中地面时的后续光效(string),");
                    nameBuilder.Append("飞到最远处的后续光效(string),");
                    nameBuilder.Append("使用碰撞盒检测光效是否到达(bool),");
                    nameBuilder.Append("结束时后续动作(string),");
                    nameBuilder.Append("是否做上抛运动(bool),");
                    nameBuilder.Append("上抛运动的高度(float),");
                    nameBuilder.Append("上抛最小偏转角(float),");
                    nameBuilder.Append("上抛最大偏转角(float),");
                    nameBuilder.Append("上抛运动最高点在路径上的百分比(float),");
                    nameBuilder.Append("音效ID(int),");
                    nameBuilder.Append("光效到达目标位置后延迟消失的时间(毫秒)(int),");
                    nameBuilder.Append("光效延迟消失的时候隐藏美术指定部件时间(毫秒)(int),");
                    nameBuilder.Append("光效延迟消失时吸附在目标上(bool),");
                    nameBuilder.Append("光效是否会影响护盾(bool)");
                }
                break;
            case LightingEffectConfigType.AniReplace:
                {
                    nameBuilder.Append("开始时刻(毫秒)(int),");
                    nameBuilder.Append("结束时刻(毫秒)(int)");
                }
                break;
            case LightingEffectConfigType.Area:
                {
                    nameBuilder.Append("光效开始播放时间(毫秒)(int),");
                    nameBuilder.Append("诞生光效间隔时间(毫秒)(int),");
                    nameBuilder.Append("诞生光效ID(int),");
                    nameBuilder.Append("光效位置绑定骨骼名字(string),");
                    nameBuilder.Append("光效指定相对落地点的位移(vector3),");
                    nameBuilder.Append("光效范围是投影到地面的(bool),");
                    nameBuilder.Append("光效距离一直延长到最大距离(bool),");
                    nameBuilder.Append("光效数量(int),");
                    nameBuilder.Append("光效最长生命时间(毫秒)(int),");
                }
                break;
            case LightingEffectConfigType.Camera:
                {
                    nameBuilder.Append("动画镜头的原点取的对象(0.Hero 1.FollowHero 2.TargetPos 3.SourceObject)(int),");
                    nameBuilder.Append("动画镜头的原点需要贴地(bool),");
                    nameBuilder.Append("动画镜头强制取主摄像机(bool),");
                    nameBuilder.Append("震屏光效需要附加到哪些人身上(0.Source 1.Target 2.SourceAndTarget 3.All)(int),");
                    nameBuilder.Append("震屏光效影响范围(float),");
                    nameBuilder.Append("光效存在时间(毫秒)(int),");
                    nameBuilder.Append("光效延迟开始时间(毫秒)(int),");
                    nameBuilder.Append("光效中断时摄像机恢复用的秒数(毫秒)(int),");
                    nameBuilder.Append("结束时是否恢复一开始的摄像机(bool),");
                }
                break;
            case LightingEffectConfigType.Shadow:
                {
                    nameBuilder.Append("开始时刻(毫秒)(int),");
                    nameBuilder.Append("结束时刻(毫秒)(int),");
                    nameBuilder.Append("每个残影开始的透明度(float),");
                    nameBuilder.Append("每个残影存在的时间(毫秒)(int),");
                    nameBuilder.Append("每个残影淡出的时间(毫秒)(int),");
                    nameBuilder.Append("插入残影的时间间隔(毫秒)(int),");
                    nameBuilder.Append("需要插入残影的关键帧(string),");
                    nameBuilder.Append("替换为的材质的透明参数名字(string),");
                    nameBuilder.Append("武器无残影(bool),");
                    nameBuilder.Append("替换为的材质的透明参数类型(0.Color 1.Float 2.Int)(int),");
                    nameBuilder.Append("残影与残影/人物间最短间隔距离(float),");
                }
                break;
            case LightingEffectConfigType.Sound:
                {
                    nameBuilder.Append("光效声音ID(int),");
                }
                break;
            case LightingEffectConfigType.Speed:
                {
                    nameBuilder.Append("开始时刻(毫秒)(int),");
                    nameBuilder.Append("结束时刻(毫秒)(int),");
                    nameBuilder.Append("全局时间速度(float),");
                    nameBuilder.Append("个人动作速度(float),");
                    nameBuilder.Append("效果有效范围(float),");
                }
                break;
            case LightingEffectConfigType.Ward:
                {
                    nameBuilder.Append("开始时刻(毫秒)(int),");
                    nameBuilder.Append("结束时刻(毫秒)(int),");
                    nameBuilder.Append("无视阵营阻挡一切(bool),");
                }
                break;
            case LightingEffectConfigType.Invalid:
            default:
                break;
        }
        streamWriter.WriteLine(nameBuilder.ToString());

        FlushStreamWriter();
    }


    private static void WriteScheme(string szSchemeInfo)
    {
        if (streamWriter != null)
        {
            streamWriter.WriteLine(szSchemeInfo);
        }
    }


    private static void InitStreamWriter()
    {
        FreeStreamWriter();

        string strOutputPath = Application.dataPath + outPutPath + schemeFileName;
        int index = strOutputPath.IndexOf("Assets/");
        if (index > -1)
        {
            strOutputPath = strOutputPath.Remove(index, 7);
        }

        streamWriter = new StreamWriter(strOutputPath, false, new System.Text.UTF8Encoding(true));
    }


    private static void FreeStreamWriter()
    {
        if (null != streamWriter)
        {
            streamWriter.Close();
            streamWriter = null;
        }
    }


    private static void FlushStreamWriter()
    {
        if (null != streamWriter)
        {
            streamWriter.Flush();
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace ASpeedGame.Data.Scheme
{
    /// <summary>
    /// 光效配置
    /// </summary>
    public class SchemeLightingEffect
    {
        private enum Column_Common : int
        {
            COL_CFGTYPE = 0,
            COL_ID,
            COL_GUID,
            COL_DESC,
            COL_GLOBAL,
            COL_SINGLE,
            COL_RECOVEREFFECT,
            COL_SPEEDSCALE,
            COL_IGNOREEFFECTOPTIMIZED,
            COL_ENABLEDISTANCEOPTIMIZED,
            COL_CLOSEWHENDEAD,
            COL_EFFECTTYPE,
            COL_MAX,
        }

        private enum Column_Bind : int
        {
            COL_CREATEOUTSCREEN = (int)Column_Common.COL_MAX,
            COL_SCALE,
            COL_STARTTICK,
            COL_LIVETICK,
            COL_DISPEARTICK,
            COL_POSBINDBONE,
            COL_POSOFFSET,
            COL_ROTBINDBONE,
            COL_ROTOFFSET,
            COL_FACETOCAM,
            COL_OFFSETTOSURFACE,
            COL_BINDTYPE,
            COL_FACEAGNISTTARGET,
            COL_PROJONFLOOR,
            COL_FACETOWALKINGDIR,
            COL_NOTROTATE,
            COL_BINDTARGETTOSRC,
            COL_BINDTARGETBONE,
            COL_TRIGGERHITDETECT,
            COL_SOUND,
            COL_POSBINDWOUND,
        }

        private enum Column_Fly : int
        {
            COL_SCALE = (int)Column_Common.COL_MAX,
            COL_STARTTICK,
            COL_LIVETICK,
            COL_RESIDENCETICK, 
            COL_DISPEARTICK,
            COL_MAXDISTANCE,
            COL_POSBINDBONE,
            COL_ROTATIONBINDBONE,
            COL_POSOFFSET,
            COL_STARTPOSOFFSET,
            COL_TARGETTRSBONE,
            COL_TARGETOFFSET,
            COL_FLYTYPE,
            COL_STARTTYPE,
            COL_FACETARGET,
            COL_LOWESTHEIGHT,
            COL_HIGHESTHEIGHT,
            COL_FLYSPEED,
            COL_ABSFLYTIME,
            COL_DESTROYWHENREACHED,
            COL_DESTROYWHENTARGETDEAD,
            COL_USECOLLISION,
            COL_DESTROYWHENCOLLISION,
            COL_REVISEFLY,
            COL_EFFECTRETURN,
            COL_CLOSERETURN,
            COL_RETURNSPEED,
            COL_FACESRCTOTARGET,
            COL_EFFECTIDHITPLAYER,
            COL_EFFECTIDHITMONSTER,
            COL_EFFECTIDHITGROUND,
            COL_EFFECTIDREACH,
            COL_TRIGGERHITDETECT,
            COL_PLAYANIMWHENCLOSE,
            COL_UPCASTMOTION,
            COL_UPCASTHEIGHT,
            COL_MINUPCASTANGLE,
            COL_MAXUPCASTANGLE,
            COL_UPCASTTOPPREC,
            COL_SOUND,
            COL_DELAYDESTROYTICK,
            COL_DELAYHIDETICK,
            COL_ATTACKTOOBJECT,
            COL_AFFECTSHIELD,
        }

        private enum Column_AnimReplace : int
        {
            COL_STARTTICK = (int)Column_Common.COL_MAX,
            COL_ENDTICK,
        }

        private enum Column_Area : int
        {
            COL_STARTTICK = (int)Column_Common.COL_MAX,
            COL_INTERVTICK,
            COL_SUBEFFECTID,
            COL_POSBINDBONE,
            COL_CUSTOMOFFSET,
            COL_PROJONGROUND,
            COL_EXTENTTOMAXDIS,
            COL_EFFECTCOUNT,
            COL_EFFECTTOTALTICK,
        }

        private enum Column_Camera : int 
        {
            COL_ANIMORGPOS = (int)Column_Common.COL_MAX,
            COL_ANIMORGPROJFLOOR,
            COL_ANIMMAINCAM,
            COL_ATTACKTYPE,
            COL_EFFECTRAD,
            COL_EFFECTLIVETICK,
            COL_DELAYTICK,
            COL_EFFECTFINISHTICK,
            RECOVERTRANSFORM,
        }

        private enum Column_Shadow : int
        {
            COL_STARTTICK = (int)Column_Common.COL_MAX,
            COL_ENDTICK,
            COL_STARTALPHA,
            COL_MAINTAINTICK,
            COL_FADEOUTTICK,
            COL_INTERVTICK,
            COL_FRAMETICK,
            COL_ALPHAATTNAME,
            COL_DISABLEWEAPON,
            COL_ALPHAATTTYPE,
            COL_MINDISTANCE,
        }

        private enum Column_Sound : int
        {
            COL_SOUND = (int)Column_Common.COL_MAX,
        }

        private enum Column_Speed : int
        {
            COL_STARTTICK = (int)Column_Common.COL_MAX,
            COL_ENDTICK,
            COL_GLOBALSPEED,
            COL_ANIMSPEED,
            COL_EFFECTRAD,
        }

        private enum Column_Ward : int
        {
            COL_STARTTICK = (int)Column_Common.COL_MAX,
            COL_ENDTICK,
            COL_BLOCKALLCREATURE,
        }

        public static readonly string schemeFileName = "LightingEffect.csv";

        public Dictionary<int, LightingEffect> DicLightEffectNodeList { get { return m_dicLightEffectNodeList; } }
        private Dictionary<int, LightingEffect> m_dicLightEffectNodeList = null;
        static string editorModeAssetPath = "Assets/Scheme/LightingEffect";

        private static SchemeLightingEffect m_instance = new SchemeLightingEffect();
        public static SchemeLightingEffect Instance
        {
            get { return m_instance; }
        }


        SchemeLightingEffect()
        {
            m_dicLightEffectNodeList = new Dictionary<int, LightingEffect>();
        }

        public LightingEffect GetLightingEffect(int id)
        {
            LightingEffect ef = null;
            m_dicLightEffectNodeList.TryGetValue(id, out ef);
            return ef;
        }

        public void UnLoad()
        {
            m_dicLightEffectNodeList.Clear();
            m_dicLightEffectNodeList = null;
        }


        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true);
            if (reader == null)
            {
                Trace.LogError("read scheme fail! file = " + strFilePath);
                return;
            }

            m_dicLightEffectNodeList.Clear();

            if(AssetBundleManager.isOnRunTimeMode)
            {
                int nRecorderCount = reader.GetRecordCount();
                for (int row = 0; row < nRecorderCount; row++)
                {
                    int nType = reader.GetInt(row, (int)Column_Common.COL_CFGTYPE, 0);

                    AddSchemeNode(nType, reader, row);
                }
            }
            else
            {
                UnityEngine.Object[] assets = AssetBundleManager.LoadAllAssetOnEditorFromPathInstance(editorModeAssetPath, typeof(LightingEffect));
                foreach (UnityEngine.Object asset in assets)
                {
                    LightingEffect ef = asset as LightingEffect;
                    AddLightEffect(ef);
                }
            }

        }


        void AddSchemeNode(int nType, ScpReader reader, int row)
        {
            switch (nType)
            {
                case (int)LightingEffectConfigType.Bind:
                    {
                        LightingEffect_Bind bindEffect = ScriptableObject.CreateInstance<LightingEffect_Bind>();
                        FillBaseData(bindEffect, reader, row);

                        bindEffect.bCreateOutsideScreen = reader.GetInt(row, (int)Column_Bind.COL_CREATEOUTSCREEN, 0) > 0 ? true : false;
                        bindEffect.scale = reader.GetFloat(row, (int)Column_Bind.COL_SCALE, 0);
                        bindEffect.effectStartTick = reader.GetInt(row, (int)Column_Bind.COL_STARTTICK, 0);
                        bindEffect.effectLiveTick = reader.GetInt(row, (int)Column_Bind.COL_LIVETICK, 0);
                        bindEffect.effectDispearTick = reader.GetInt(row, (int)Column_Bind.COL_DISPEARTICK, 0);
                        bindEffect.posBindBone = reader.GetString(row, (int)Column_Bind.COL_POSBINDBONE, "");

                        string[] posOffsetStrArry = reader.GetString(row, (int)Column_Bind.COL_POSOFFSET, "").Split(';');
                        bindEffect.posOffset = StrArry2Vector3(posOffsetStrArry);

                        bindEffect.rotationBindBone = reader.GetString(row, (int)Column_Bind.COL_ROTBINDBONE, "");

                        string[] rotationOffsetStrArry = reader.GetString(row, (int)Column_Bind.COL_ROTOFFSET, "").Split(';'); 
                        bindEffect.rotationOffset = StrArry2Vector3(rotationOffsetStrArry);

                        bindEffect.faceToCamera = reader.GetInt(row, (int)Column_Bind.COL_FACETOCAM, 0) > 0 ? true : false;
                        bindEffect.offsetToSurface = reader.GetInt(row, (int)Column_Bind.COL_OFFSETTOSURFACE, 0) > 0 ? true : false;

                        int bindType = reader.GetInt(row, (int)Column_Bind.COL_BINDTYPE, 0);
                        if (Enum.IsDefined(typeof(BindType), bindType))
                        {
                            bindEffect.bindType = (BindType)bindType;
                        }

                        bindEffect.faceAganistTarget = reader.GetInt(row, (int)Column_Bind.COL_FACEAGNISTTARGET, 0) > 0 ? true : false;
                        bindEffect.bProjectOnFloor = reader.GetInt(row, (int)Column_Bind.COL_PROJONFLOOR, 0) > 0 ? true : false;
                        bindEffect.faceToWalkingDirection = reader.GetInt(row, (int)Column_Bind.COL_FACETOWALKINGDIR, 0) > 0 ? true : false;
                        bindEffect.bNotRotate = reader.GetInt(row, (int)Column_Bind.COL_NOTROTATE, 0) > 0 ? true : false;
                        bindEffect.bindTargetToSource = reader.GetInt(row, (int)Column_Bind.COL_BINDTARGETTOSRC, 0) > 0 ? true : false;
                        bindEffect.bindTargetBone = reader.GetString(row, (int)Column_Bind.COL_BINDTARGETBONE, "");
                        bindEffect.triggerHitDetect = reader.GetInt(row, (int)Column_Bind.COL_TRIGGERHITDETECT, 0) > 0 ? true : false;
                        bindEffect.SoundID = reader.GetInt(row, (int)Column_Bind.COL_SOUND, 0);
                        bindEffect.posBindWound = reader.GetString(row, (int)Column_Bind.COL_POSBINDWOUND, "");

                        AddLightEffect(bindEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Fly:
                    {
                        LightingEffect_Fly flyEffect = ScriptableObject.CreateInstance<LightingEffect_Fly>();
                        FillBaseData(flyEffect, reader, row);

                        flyEffect.scale = reader.GetFloat(row, (int)Column_Fly.COL_SCALE, 0);
                        flyEffect.effectStartTick = reader.GetInt(row, (int)Column_Fly.COL_STARTTICK, 0);
                        flyEffect.effectLiveTick = reader.GetInt(row, (int)Column_Fly.COL_LIVETICK, 0);
                        flyEffect.residenceTick = reader.GetInt(row, (int)Column_Fly.COL_RESIDENCETICK, 0);
                        flyEffect.effectDispearTick = reader.GetInt(row, (int)Column_Fly.COL_DISPEARTICK, 0);
                        flyEffect.maxDistance = reader.GetFloat(row, (int)Column_Fly.COL_MAXDISTANCE, 0);
                        flyEffect.posBindBone = reader.GetString(row, (int)Column_Fly.COL_POSBINDBONE, "");
                        flyEffect.rotationBindBone = reader.GetString(row, (int)Column_Fly.COL_ROTATIONBINDBONE, "");

                        string[] posOffsetStrArry = reader.GetString(row, (int)Column_Fly.COL_POSOFFSET, "").Split(';');
                        flyEffect.posOffset = StrArry2Vector3(posOffsetStrArry);

                        string[] startPosOffsetStrArry = reader.GetString(row, (int)Column_Fly.COL_STARTPOSOFFSET, "").Split(';');
                        flyEffect.startPosOffset = StrArry2Vector3(startPosOffsetStrArry);

                        flyEffect.targetTransformBone = reader.GetString(row, (int)Column_Fly.COL_TARGETTRSBONE, "");

                        string[] targetOffsetStrArry = reader.GetString(row, (int)Column_Fly.COL_TARGETOFFSET, "").Split(';');
                        flyEffect.targetOffset = StrArry2Vector3(targetOffsetStrArry);

                        int flyType = reader.GetInt(row, (int)Column_Fly.COL_FLYTYPE, 0);
                        if (Enum.IsDefined(typeof(FlyType), flyType))
                        {
                            flyEffect.flyType = (FlyType)flyType;
                        }

                        int StartType = reader.GetInt(row, (int)Column_Fly.COL_STARTTYPE, 0);
                        if (Enum.IsDefined(typeof(StartPosType), StartType))
                        {
                            flyEffect.startType = (StartPosType)StartType;
                        }
                        
                        flyEffect.faceTarget = reader.GetInt(row, (int)Column_Fly.COL_FACETARGET, 0) > 0 ? true : false;
                        flyEffect.lowestHeight = reader.GetFloat(row, (int)Column_Fly.COL_LOWESTHEIGHT, 0);
                        flyEffect.highestHeight = reader.GetFloat(row, (int)Column_Fly.COL_HIGHESTHEIGHT, 0);
                        flyEffect.flySpeed = reader.GetFloat(row, (int)Column_Fly.COL_FLYSPEED, 0);
                        flyEffect.AbsFlyTime = reader.GetFloat(row, (int)Column_Fly.COL_ABSFLYTIME, 0);
                        flyEffect.destroyWhenReached = reader.GetInt(row, (int)Column_Fly.COL_DESTROYWHENREACHED, 0) > 0 ? true : false;
                        flyEffect.destroyWhenTargetDead = reader.GetInt(row, (int)Column_Fly.COL_DESTROYWHENTARGETDEAD, 0) > 0 ? true : false;
                        flyEffect.useCollision = reader.GetInt(row, (int)Column_Fly.COL_USECOLLISION, 0) > 0 ? true : false;
                        flyEffect.destroyWhenCollision = reader.GetInt(row, (int)Column_Fly.COL_DESTROYWHENCOLLISION, 0) > 0 ? true : false;
                        flyEffect.reviseFly = reader.GetInt(row, (int)Column_Fly.COL_REVISEFLY, 0) > 0 ? true : false;
                        flyEffect.effectReturn = reader.GetInt(row, (int)Column_Fly.COL_EFFECTRETURN, 0) > 0 ? true : false;
                        flyEffect.closeReturn = reader.GetInt(row, (int)Column_Fly.COL_CLOSERETURN, 0) > 0 ? true : false;
                        flyEffect.returnSpeed = reader.GetFloat(row, (int)Column_Fly.COL_RETURNSPEED, 0);
                        flyEffect.faceSourceToTarget = reader.GetFloat(row, (int)Column_Fly.COL_FACESRCTOTARGET, 0) > 0 ? true : false;
                        flyEffect.effectIDWhenHitPlayer = reader.GetString(row, (int)Column_Fly.COL_EFFECTIDHITPLAYER, "");
                        flyEffect.effectIDWhenHitMonster = reader.GetString(row, (int)Column_Fly.COL_EFFECTIDHITMONSTER, "");
                        flyEffect.effectIDWhenHitGround = reader.GetString(row, (int)Column_Fly.COL_EFFECTIDHITGROUND, "");
                        flyEffect.effectIDWhenReached = reader.GetString(row, (int)Column_Fly.COL_EFFECTIDREACH, "");
                        flyEffect.triggerHitDetect = reader.GetInt(row, (int)Column_Fly.COL_TRIGGERHITDETECT, 0) > 0 ? true : false;
                        flyEffect.playAnimWhenClose = reader.GetString(row, (int)Column_Fly.COL_PLAYANIMWHENCLOSE, "");
                        flyEffect.upcastMotion = reader.GetFloat(row, (int)Column_Fly.COL_UPCASTMOTION, 0) > 0 ? true : false;
                        flyEffect.upcastHeight = reader.GetFloat(row, (int)Column_Fly.COL_UPCASTHEIGHT, 0);
                        flyEffect.minUpcastAngle = reader.GetFloat(row, (int)Column_Fly.COL_MINUPCASTANGLE, 0);
                        flyEffect.maxUpcastAngle = reader.GetFloat(row, (int)Column_Fly.COL_MAXUPCASTANGLE, 0);
                        flyEffect.upcastTopPrec = reader.GetFloat(row, (int)Column_Fly.COL_UPCASTTOPPREC, 0);
                        flyEffect.SoundID = reader.GetInt(row, (int)Column_Fly.COL_SOUND, 0);
                        flyEffect.delayDestroyTick = reader.GetInt(row, (int)Column_Fly.COL_DELAYDESTROYTICK, 0);
                        flyEffect.delayHideTick = reader.GetInt(row, (int)Column_Fly.COL_DELAYHIDETICK, 0);
                        flyEffect.attackToObject = reader.GetInt(row, (int)Column_Fly.COL_ATTACKTOOBJECT, 0) > 0 ? true : false;
                        flyEffect.AffectedShield = reader.GetInt(row, (int)Column_Fly.COL_AFFECTSHIELD, 0) > 0 ? true : false;

                        AddLightEffect(flyEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.AniReplace:
                    {
                        LightingEffect_AnimationReplace aniReplaceEffect = ScriptableObject.CreateInstance<LightingEffect_AnimationReplace>();
                        FillBaseData(aniReplaceEffect, reader, row);

                        aniReplaceEffect.nStartTick = reader.GetInt(row, (int)Column_AnimReplace.COL_STARTTICK, 0);
                        aniReplaceEffect.nEndTick = reader.GetInt(row, (int)Column_AnimReplace.COL_ENDTICK, 0);

                        AddLightEffect(aniReplaceEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Area:
                    {
                        LightingEffect_Area areaEffect = ScriptableObject.CreateInstance<LightingEffect_Area>();
                        FillBaseData(areaEffect, reader, row);

                        areaEffect.effectStartTick = reader.GetInt(row, (int)Column_Area.COL_STARTTICK, 0);
                        areaEffect.effectIntervalTick = reader.GetInt(row, (int)Column_Area.COL_INTERVTICK, 0);
                        areaEffect.subEffectID = reader.GetInt(row, (int)Column_Area.COL_SUBEFFECTID, 0);
                        areaEffect.posBindBone = reader.GetString(row, (int)Column_Area.COL_POSBINDBONE, "");

                        string[] customOffStrArry = reader.GetString(row, (int)Column_Area.COL_CUSTOMOFFSET, "").Split(';');
                        areaEffect.customOffset = StrArry2Vector3(customOffStrArry);

                        areaEffect.projectOnGround = reader.GetInt(row, (int)Column_Area.COL_PROJONGROUND, 0) > 0 ? true : false;
                        areaEffect.extentToMaxDistance = reader.GetInt(row, (int)Column_Area.COL_EXTENTTOMAXDIS, 0) > 0 ? true : false;
                        areaEffect.effectCount = reader.GetInt(row, (int)Column_Area.COL_EFFECTCOUNT, 0);
                        areaEffect.effectTotalTick = reader.GetInt(row, (int)Column_Area.COL_EFFECTTOTALTICK, 0);

                        AddLightEffect(areaEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Camera:
                    {
                        LightingEffect_Camera camEffect = ScriptableObject.CreateInstance<LightingEffect_Camera>();
                        FillBaseData(camEffect, reader, row);

                        int animOrgPos = reader.GetInt(row, (int)Column_Camera.COL_ANIMORGPOS, 0);
                        if(Enum.IsDefined(typeof(Effect.CameraAnimOrgType), animOrgPos))
                        {
                            camEffect.animationOrgPos = (Effect.CameraAnimOrgType)animOrgPos;
                        }

                        camEffect.animationOrgProjectFloor = reader.GetInt(row, (int)Column_Camera.COL_ANIMORGPROJFLOOR, 0) > 0 ? true : false;
                        camEffect.animationMainCamera = reader.GetInt(row, (int)Column_Camera.COL_ANIMMAINCAM, 0) > 0 ? true : false;

                        int attackType = reader.GetInt(row, (int)Column_Camera.COL_ATTACKTYPE, 0);
                        if (Enum.IsDefined(typeof(Effect.EffectAttackType), attackType))
                        {
                            camEffect.attackType = (Effect.EffectAttackType)attackType;
                        }

                        camEffect.effectRad = reader.GetFloat(row, (int)Column_Camera.COL_EFFECTRAD, 0f);
                        camEffect.effectLiveTick = reader.GetInt(row, (int)Column_Camera.COL_EFFECTLIVETICK, 0);
                        camEffect.delayTick = reader.GetInt(row, (int)Column_Camera.COL_DELAYTICK, 0);
                        camEffect.effectFinishTick = reader.GetInt(row, (int)Column_Camera.COL_EFFECTFINISHTICK, 0);
                        camEffect.recoverTransform = reader.GetInt(row, (int)Column_Camera.RECOVERTRANSFORM, 0) > 0 ? true : false;

                        AddLightEffect(camEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Shadow:
                    {
                        LightingEffect_Shadow shadowEffect = ScriptableObject.CreateInstance<LightingEffect_Shadow>();
                        FillBaseData(shadowEffect, reader, row);

                        shadowEffect.nStartTick = reader.GetInt(row, (int)Column_Shadow.COL_STARTTICK, 0);
                        shadowEffect.nEndTick = reader.GetInt(row, (int)Column_Shadow.COL_ENDTICK, 0);
                        shadowEffect.fStartAlpha = reader.GetFloat(row, (int)Column_Shadow.COL_STARTALPHA, 0);
                        shadowEffect.nShadowMaintainTick = reader.GetInt(row, (int)Column_Shadow.COL_MAINTAINTICK, 0);
                        shadowEffect.nShadowFadeoutTick = reader.GetInt(row, (int)Column_Shadow.COL_FADEOUTTICK, 0);
                        shadowEffect.shadowIntervalTicks = reader.GetInt(row, (int)Column_Shadow.COL_INTERVTICK, 0);

                        string[] frameStrArry = reader.GetString(row, (int)Column_Shadow.COL_FRAMETICK, "").Split(';');
                        int[] frameIntArry = new int[frameStrArry.Length];
                        for (int i = 0; i < frameStrArry.Length; i++)
                        {
                            int.TryParse(frameStrArry[i], out frameIntArry[i]);
                        }
                        shadowEffect.shadowFrameTicks = frameIntArry;

                        shadowEffect.alphaAttributeName = reader.GetString(row, (int)Column_Shadow.COL_ALPHAATTNAME, "");

                        shadowEffect.DisableWeapon = reader.GetInt(row, (int)Column_Shadow.COL_DISABLEWEAPON, 0) > 0 ? true : false;

                        int attrType = reader.GetInt(row, (int)Column_Shadow.COL_ALPHAATTTYPE, 0);
                        if(Enum.IsDefined(typeof(LightingEffect_Shadow.AlphaAttributeType), attrType))
                        {
                            shadowEffect.alphaAttributeType = (LightingEffect_Shadow.AlphaAttributeType)attrType;
                        }

                        shadowEffect.minDistance = reader.GetFloat(row, (int)Column_Shadow.COL_MINDISTANCE, 0);

                        AddLightEffect(shadowEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Sound:
                    {
                        LightingEffect_Sound soundEffect = ScriptableObject.CreateInstance<LightingEffect_Sound>();
                        FillBaseData(soundEffect, reader, row);

                        soundEffect.soundID = reader.GetInt(row, (int)Column_Sound.COL_SOUND, 0);

                        AddLightEffect(soundEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Speed:
                    {
                        LightingEffect_Speed speedEffect = ScriptableObject.CreateInstance<LightingEffect_Speed>();
                        FillBaseData(speedEffect, reader, row);

                        speedEffect.nStartTick = reader.GetInt(row, (int)Column_Speed.COL_STARTTICK, 0);
                        speedEffect.nEndTick = reader.GetInt(row, (int)Column_Speed.COL_ENDTICK, 0);
                        speedEffect.globalSpeed = reader.GetFloat(row, (int)Column_Speed.COL_GLOBALSPEED, 0);
                        speedEffect.animatorSpeed = reader.GetFloat(row, (int)Column_Speed.COL_ANIMSPEED, 0);
                        speedEffect.effectRad = reader.GetFloat(row, (int)Column_Speed.COL_EFFECTRAD, 0);

                        AddLightEffect(speedEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Ward:
                    {
                        LightingEffect_Ward wardEffect = ScriptableObject.CreateInstance<LightingEffect_Ward>();
                        FillBaseData(wardEffect, reader, row);

                        wardEffect.nStartTick = reader.GetInt(row, (int)Column_Ward.COL_STARTTICK, 0);
                        wardEffect.nEndTick = reader.GetInt(row, (int)Column_Ward.COL_ENDTICK, 0);
                        wardEffect.blockAllCreature = reader.GetInt(row, (int)Column_Ward.COL_BLOCKALLCREATURE, 0) > 0 ? true : false;

                        AddLightEffect(wardEffect);
                    }
                    break;
                case (int)LightingEffectConfigType.Invalid:
                default:
                    break;
            }
        }


        void AddLightEffect(LightingEffect lightingEffect)
        {
            if (m_dicLightEffectNodeList == null || lightingEffect == null)
                return;

            if (m_dicLightEffectNodeList.ContainsKey(lightingEffect.nID))
            {
                Trace.LogWarning("配置表ID重复！ID:" + lightingEffect.nID + " File = " + schemeFileName);
                return;
            }

            m_dicLightEffectNodeList.Add(lightingEffect.nID, lightingEffect);
        }


        void FillBaseData(LightingEffect baseEffect, ScpReader reader, int row)
        {
            baseEffect.nID = reader.GetInt(row, (int)Column_Common.COL_ID, 0);
            baseEffect.AssetGUID = reader.GetString(row, (int)Column_Common.COL_GUID, "");
            baseEffect.strDesc = reader.GetString(row, (int)Column_Common.COL_DESC, "");
            baseEffect.bGlobal = reader.GetInt(row, (int)Column_Common.COL_GLOBAL, 0) > 0 ? true : false;
            baseEffect.bSingle = reader.GetInt(row, (int)Column_Common.COL_SINGLE, 0) > 0 ? true : false;
            baseEffect.bRecoverSingleEffect = reader.GetInt(row, (int)Column_Common.COL_RECOVEREFFECT, 0) > 0 ? true : false;
            baseEffect.bUseSpeedScale = reader.GetInt(row, (int)Column_Common.COL_SPEEDSCALE, 0) > 0 ? true : false;
            baseEffect.bIgnoreSingleEffectOptimtized = reader.GetInt(row, (int)Column_Common.COL_IGNOREEFFECTOPTIMIZED, 0) > 0 ? true : false;
            baseEffect.EnableDistanceOptimtized = reader.GetInt(row, (int)Column_Common.COL_ENABLEDISTANCEOPTIMIZED, 0) > 0 ? true : false;
            baseEffect.CloseWhenDead = reader.GetInt(row, (int)Column_Common.COL_CLOSEWHENDEAD, 0) > 0 ? true : false;
            
            int effectType = reader.GetInt(row, (int)Column_Common.COL_EFFECTTYPE, 0);
            if (Enum.IsDefined(typeof(LightingEffect.LightingEffectType), effectType))
            {
                baseEffect.effectType = (LightingEffect.LightingEffectType)effectType;
            }
        }

        private Vector3 StrArry2Vector3(string[] strArry)
        {
            if (strArry == null)
                return Vector3.zero;

            float[] intArry = new float[3] { 0, 0, 0 };

            for (int i = 0; i < 3; i++)
            {
                if (strArry.Length > i)
                {
                    float.TryParse(strArry[i], out intArry[i]);
                }
            }

            return new Vector3(intArry[0], intArry[1], intArry[2]);
        }
    }
}

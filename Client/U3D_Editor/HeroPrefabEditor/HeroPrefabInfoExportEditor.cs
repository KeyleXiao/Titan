using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace U3D_Editor.HeroPrefabEditor
{
    class HeroPrefabInfoExportEditor: EditorWindow
    {
    public const string schemePath = "/Prefabs/Skin/Hero";
        public const string outPutPath = "/Data/Scp/";
        public const string schemeFileName = "HeroPrefabInfoEX.csv";
        public const string schemeExtension = ".prefab";

        public static string configPath = "";
        public static string OutConfigPath = "";

        private static StreamWriter streamWriter;
        private static Dictionary<LightingEffectConfigType, List<LightingEffect>> lightEffectConfigMap = new Dictionary<LightingEffectConfigType, List<LightingEffect>>();

        struct HeroInfo
        {
            public string prefabPath;
            public string prefabName;
            public string modelPath;
            public Animator Animator;
            public CharacterController CharacterController;
            public CreatureProperty CreatureProperty;
            public PersonSkinConfig PersonSkinConfig;
        }
        private static List<HeroInfo> HeroInfoConfigList = new List<HeroInfo>();

        [MenuItem("Tools/HeroInfo Export")]
        public static void Init()
        {
            HeroPrefabInfoExportEditor editor = EditorWindow.GetWindow<HeroPrefabInfoExportEditor>(false, "英雄配置导出", true);
            editor.Show();
            editor.autoRepaintOnSceneChange = true;
        }

        public void OnEnable()
        {
            configPath = Application.dataPath + schemePath;
            OutConfigPath = Application.dataPath + outPutPath + schemeFileName;
        }
        public void OnGUI()
        {
            if (Application.isPlaying)
            {
                EditorGUILayout.LabelField("不能在播放模式下进行导出");
                return;
            }

            GUILayout.Space(10.0f);
            GUILayout.Label("Unity英雄配置根目录：");

            GUILayout.Space(5.0f);
            GUILayout.BeginHorizontal();
            configPath = EditorGUILayout.TextField(configPath);
            GUILayout.EndHorizontal();

            GUILayout.Space(10.0f);
            GUILayout.Label("CSV英雄配置根目录：");

            GUILayout.Space(5.0f);
            GUILayout.BeginHorizontal();
            OutConfigPath = EditorGUILayout.TextField(OutConfigPath);
            GUILayout.EndHorizontal();



            GUILayout.Space(15.0f);
            if (GUILayout.Button("导出配置"))
            {
                ExportHeroPrefab();
            }
            GUILayout.Space(15.0f);
            if (GUILayout.Button("导入配置"))
            {
               // Import();
            }
        }
        public static void ExportHeroPrefab()
        {
            if (string.IsNullOrEmpty(configPath))
                configPath = Application.dataPath + schemePath;
            if (!Directory.Exists(configPath))
            {
                EditorUtility.DisplayDialog("配置导出", "光效配置根目录不存在，请检查!", "OK");
                return;
            }
            string[] schemeArray = Directory.GetFiles(configPath, "*" + schemeExtension, SearchOption.AllDirectories);
            foreach (string strSchemePath in schemeArray)
            {
                if (string.IsNullOrEmpty(strSchemePath) || !strSchemePath.EndsWith(schemeExtension))
                    continue;
                string strTempPath = strSchemePath.Replace(@"\", "/");
                strTempPath = strTempPath.Substring(strTempPath.IndexOf("Assets"));
                UnityEngine.Object HeroPrefab = AssetDatabase.LoadAssetAtPath(@strTempPath, typeof(UnityEngine.Object));
                if (HeroPrefab != null)
                {
                    //Debug.Log(strTempPath);
                    GameObject scheme = HeroPrefab as GameObject;
                    if (scheme != null)
                    {
                        HeroInfo heroinfo;
                        heroinfo.prefabPath =/* configPath;*/ strTempPath;
                        heroinfo.prefabName = scheme.name;
                        heroinfo.Animator = scheme.GetComponent<Animator>();
                        var modelPath = AssetDatabase.GetAssetPath(heroinfo.Animator.avatar);
                        heroinfo.modelPath = modelPath;
                        heroinfo.CharacterController = scheme.GetComponentInChildren<CharacterController>();
                        heroinfo.CreatureProperty = scheme.GetComponentInChildren<CreatureProperty>();
                        heroinfo.PersonSkinConfig = scheme.GetComponentInChildren<PersonSkinConfig>();
                        HeroInfoConfigList.Add(heroinfo);
                    }
                }
            }
            InitStreamWriter();
            WriteHeroInfoHead();
            for (int i = 0; i < HeroInfoConfigList.Count; i++)
            {
                StringBuilder sb = new StringBuilder();
                HeroPrefabInfoAppend(ref sb, HeroInfoConfigList[i]);

                streamWriter.WriteLine(sb.ToString());
                FlushStreamWriter();
            }
            FreeStreamWriter();
            Clear();
            EditorUtility.DisplayDialog("配置导出", "光效配置导出完成!", "OK");
        }
        private static void WriteHeroInfoHead()
        {
            if (streamWriter == null)
                return;
            StringBuilder TypeBuilder = new StringBuilder();
            StringBuilder nameBuilder = new StringBuilder();

            FollowAppend(ref nameBuilder, ref TypeBuilder, "预制体路径");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "预制体文件名");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "模型路径");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Controller路径", "Animator");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Slope Limit", "Character Controller");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Step Offset");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Skin Width");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Min Move Distance");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Center X");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Center Y");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Center Z");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Radius");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Height");

            FollowAppend(ref nameBuilder, ref TypeBuilder, "Floating Min Height", "Creature Property");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Aim Distance");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Lock Aim Distance");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Attacked Aim Distance");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Attack Move Range");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Max Attack Move Speed Scale");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Common Attack Skill ID");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Animator Cross Speed");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "FPS Mode(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Top Info Hide In Distance");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Blood Y Offset");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Top Name Y Offset");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Cross Hair Offset(vector2)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Cross Hair Visible(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Mouse Aim(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Check Position(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Creature Height In Meters");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Screen Cast Object");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Creature Bounding Rad X");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Creature Bounding Rad Y");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Creature Bounding Rad Z");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Inertia Acceleration");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Inertia Scale");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "右手武器", "Person Skin Config");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "左手武器");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "身体");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "背部");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "翅膀挂点");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "优化距离");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "缓存数目");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "靠近主角时半透明显示(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Main");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Root_M");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "BackA_M");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "Root_MCopy");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "weaponObject");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "weaponObject2");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "adjustSpine(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "RootAReplace(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "上下半身分离的攻击层位置");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "rootAngleWithAim X");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "rootAngleWithAim Y");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "rootAngleWithAim Z");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "progressWeight(bool)");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "弯腰瞄准程度");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "弯腰瞄准参考骨骼");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "ScreenCastObject");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "jumpDeltyTick");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "maxFallSpeed");
            FollowAppend(ref nameBuilder, ref TypeBuilder, "MorphHip(bool)");

            streamWriter.WriteLine(TypeBuilder.ToString());
            streamWriter.WriteLine(nameBuilder.ToString());

            FlushStreamWriter();
        }
        static void FollowAppend(ref StringBuilder s, ref StringBuilder s2, string str1, string str2 = "")
        {
            s.Append(str1 + ",");
            s2.Append(str2 + ",");
        }
        static void HeroPrefabInfoAppend(ref StringBuilder sb, HeroInfo Info)
        {
            sb.Append(Info.prefabPath + ",");
            sb.Append(Info.prefabName + ",");
            sb.Append(Info.modelPath + ",");
            sb.Append(AssetDatabase.GetAssetPath(Info.Animator.runtimeAnimatorController) + ",");

            ///CharacterController
            if (Info.CharacterController != null)
            {
                sb.Append(Info.CharacterController.slopeLimit + ",");
                sb.Append(Info.CharacterController.stepOffset + ",");
                sb.Append(Info.CharacterController.skinWidth + ",");
                //sb.Append(Info.CharacterController.minMoveDistance + ",");
                sb.Append("0" + ",");
                sb.Append(Info.CharacterController.center.x + ",");
                sb.Append(Info.CharacterController.center.y + ",");
                sb.Append(Info.CharacterController.center.z + ",");
                sb.Append(Info.CharacterController.radius + ",");
                sb.Append(Info.CharacterController.height + ",");
            }
            else
            {
                sb.Append("null,,,,,,,,,");
            }
            ///Creature Property
            if (Info.CreatureProperty != null)
            {
                sb.Append(Info.CreatureProperty.floatingMinHeight + ",");
                sb.Append(Info.CreatureProperty.aimDistance + ",");
                sb.Append(Info.CreatureProperty.lockAimDistance + ",");
                sb.Append(Info.CreatureProperty.attackedAimDistance + ",");
                sb.Append(Info.CreatureProperty.attackMoveRange + ",");
                sb.Append(Info.CreatureProperty.maxAttackMoveSpeedScale + ",");
                sb.Append(Info.CreatureProperty.commonAttackSkillID + ",");
                sb.Append(Info.CreatureProperty.animatorCrossSpeed + ",");
                string FpsMode = (Info.CreatureProperty.FPSMode ? 1 : 0).ToString();
                sb.Append(FpsMode + ",");
                sb.Append(Info.CreatureProperty.TopInfoHideInDistance + ",");
                sb.Append(Info.CreatureProperty.BloodYOffset + ",");
                sb.Append(Info.CreatureProperty.TopNameYOffset + ",");
                sb.Append(Info.CreatureProperty.CrossHairOffset.x + ":" + Info.CreatureProperty.CrossHairOffset.y + ",");
                string CrossHairVisible = (Info.CreatureProperty.CrossHairVisible ? 1 : 0).ToString();
                sb.Append(CrossHairVisible + ",");
                string MouseAim = (Info.CreatureProperty.MouseAim ? 1 : 0).ToString();
                sb.Append(MouseAim + ",");
                string checkPosition = (Info.CreatureProperty.checkPosition ? 1 : 0).ToString();
                sb.Append(checkPosition + ",");
                sb.Append(Info.CreatureProperty.CreatureHeightInMeters + ",");
                if (Info.CreatureProperty.ScreenCastObject != null)
                {
                    var modelPath = AssetDatabase.GetAssetPath(Info.CreatureProperty.ScreenCastObject);
                    sb.Append(modelPath + ",");  //--
                }
                else
                {
                    sb.Append(Info.CreatureProperty.ScreenCastObject + ",");  //--
                }
                sb.Append(Info.CreatureProperty.CreatureBoundingRad.x + ",");
                sb.Append(Info.CreatureProperty.CreatureBoundingRad.y + ",");
                sb.Append(Info.CreatureProperty.CreatureBoundingRad.z + ",");
                sb.Append(Info.CreatureProperty.InertiaAcceleration + ",");
                sb.Append(Info.CreatureProperty.InertiaScale + ",");
            }
            else
            {
                sb.Append("null,,,,,,,,,,,,,,,,,,,,,,,");
            }
            ///PersonSkinConfig
            if (Info.PersonSkinConfig.RightWeaponRender != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.RightWeaponRender.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.RightWeaponRender + ",");

            if (Info.PersonSkinConfig.LeftWeaponRender != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.LeftWeaponRender.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.LeftWeaponRender + ",");

            if (Info.PersonSkinConfig.BodyRender != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.BodyRender.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.BodyRender + ",");

            if (Info.PersonSkinConfig.BackBodyRender != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.BackBodyRender.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.BackBodyRender + ",");

            if (Info.PersonSkinConfig.chibangguadian != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.chibangguadian.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.chibangguadian + ",");

            sb.Append(Info.PersonSkinConfig.OptimitzedDistance + ",");
            sb.Append(Info.PersonSkinConfig.CacheCout + ",");
            string OptimizedNearMainHero = (Info.PersonSkinConfig.OptimizedNearMainHero ? 1 : 0).ToString();
            sb.Append(OptimizedNearMainHero + ",");

            if (Info.PersonSkinConfig.Main != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.Main.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.Main + ",");

            if (Info.PersonSkinConfig.Root_M != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.Root_M.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.Root_M + ",");

            if (Info.PersonSkinConfig.BackA_M != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.BackA_M.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.BackA_M + ",");

            if (Info.PersonSkinConfig.Root_MCopy != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.Root_MCopy.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.Root_MCopy + ",");

            if (Info.PersonSkinConfig.weaponObject != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.weaponObject.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.weaponObject + ",");

            if (Info.PersonSkinConfig.weaponObject2 != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.weaponObject2.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.weaponObject2 + ",");

            string adjustSpine = (Info.PersonSkinConfig.adjustSpine ? 1 : 0).ToString();
            sb.Append(adjustSpine + ",");
            string RootAReplace = (Info.PersonSkinConfig.RootAReplace ? 1 : 0).ToString();
            sb.Append(RootAReplace + ",");
            sb.Append(Info.PersonSkinConfig.AttackAnimatorLayer + ",");
            sb.Append(Info.PersonSkinConfig.rootAngleWithAim.x + ",");
            sb.Append(Info.PersonSkinConfig.rootAngleWithAim.y + ",");
            sb.Append(Info.PersonSkinConfig.rootAngleWithAim.z + ",");
            string progressWeight = (Info.PersonSkinConfig.progressWeight ? 1 : 0).ToString();
            sb.Append(progressWeight + ",");
            sb.Append(Info.PersonSkinConfig.lookAngleScale + ",");

            if (Info.PersonSkinConfig.lookRefTransform != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.lookRefTransform.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.lookRefTransform + ",");

            if (Info.PersonSkinConfig.ScreenCastObject != null)
            {
                StringBuilder sbval = new StringBuilder();
                SetAllParentsTransform(ref sbval, Info.PersonSkinConfig.ScreenCastObject.transform);
                sb.Append(sbval + ",");
            }
            else
                sb.Append(Info.PersonSkinConfig.ScreenCastObject + ",");

            sb.Append(Info.PersonSkinConfig.jumpDeltyTick + ",");
            sb.Append(Info.PersonSkinConfig.maxFallSpeed + ",");
            string MorphHip = (Info.PersonSkinConfig.MorphHip ? 1 : 0).ToString();
            sb.Append(MorphHip + ",");
        }
        static void SetAllParentsTransform(ref StringBuilder sb, Transform tra)
        {
            sb.Insert(0, "/" + tra.name);
            if (tra.parent != null)
            {
                SetAllParentsTransform(ref sb, tra.parent);
            }
        }
        private static void Clear()
        {
            HeroInfoConfigList.Clear();
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
        public static void Import()
        {
            int index = Application.dataPath.LastIndexOf("/");
            string stPath = Application.dataPath.Substring(0, index) + "/Data/Scp/" + schemeFileName;

            // string stPath = OutConfigPath;
            //if (!File.Exists(stPath))
            //    return;
            ScpReader reader = new ScpReader(stPath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read scheme fail! file = " + stPath);
                return;
            }
            int nRecorderCount = reader.GetRecordCount();

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                if (i > 1) break;
                //
                HeroInfo heroInfo;
                heroInfo.prefabPath = reader.GetString(i, (int)HeroPrefabCsvCouml.PrefabPath, "");
                heroInfo.prefabName = reader.GetString(i, 1, "");
                heroInfo.modelPath = reader.GetString(i, 2, "");
                GameObject HeroPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(heroInfo.prefabPath);
                if (HeroPrefab == null)
                {
                    Debug.Log("配置导入 , 光效配置导入出错: " + heroInfo.prefabPath + " 预制体不存在");
                    continue;
                }
                heroInfo.Animator = HeroPrefab.GetComponent<Animator>();
                heroInfo.CharacterController = HeroPrefab.GetComponentInChildren<CharacterController>();
                heroInfo.CreatureProperty = HeroPrefab.GetComponentInChildren<CreatureProperty>();
                heroInfo.PersonSkinConfig = HeroPrefab.GetComponentInChildren<PersonSkinConfig>();

                //if (heroInfo.Animator==null)
                //{
                //    string AnimatorPath = reader.GetString(i, 3, "");
                //    RuntimeAnimatorController HeroAnimator = AssetDatabase.LoadAssetAtPath<RuntimeAnimatorController>(AnimatorPath);
                //    if (HeroAnimator!=null)
                //    {
                //        heroInfo.Animator = HeroPrefab.AddComponent<Animator>();
                //        heroInfo.Animator.runtimeAnimatorController = HeroAnimator;
                //    }
                //}
                if (heroInfo.Animator != null)
                {
                    string AnimatorPathOld = AssetDatabase.GetAssetPath(heroInfo.Animator.runtimeAnimatorController);
                    string AnimatorPathNew = reader.GetString(i, 3, "");
                    if (AnimatorPathOld != AnimatorPathNew)
                    {
                        RuntimeAnimatorController HeroAnimator = AssetDatabase.LoadAssetAtPath<RuntimeAnimatorController>(AnimatorPathNew);
                        heroInfo.Animator.runtimeAnimatorController = HeroAnimator;
                    }
                }
                if (heroInfo.CharacterController != null)
                {
                    string str = reader.GetString(i, 4, "");
                    if (str != "null")
                    {
                        float SlopeLimit = reader.GetFloat(i, 4, 0);
                        float StepOffset = reader.GetFloat(i, 5, 0);
                        float SkinWidth = reader.GetFloat(i, 6, 0);
                        float MinMoveDistance = 0;
                        float CenterX = reader.GetFloat(i, 8, 0);
                        float CenterY = reader.GetFloat(i, 9, 0);
                        float CenterZ = reader.GetFloat(i, 10, 0);
                        Vector3 Center = new Vector3(CenterX, CenterY, CenterZ);
                        float Radius = reader.GetFloat(i, 11, 0);
                        float Height = reader.GetFloat(i, 12, 0);

                        heroInfo.CharacterController.slopeLimit = SlopeLimit;
                        heroInfo.CharacterController.stepOffset = StepOffset;
                        heroInfo.CharacterController.skinWidth = SkinWidth;
                        heroInfo.CharacterController.center = Center;
                        heroInfo.CharacterController.radius = Radius;
                        heroInfo.CharacterController.height = Height;
                    }
                }
                if (heroInfo.CreatureProperty != null)
                {
                    string str = reader.GetString(i, 13, "");
                    if (str != "null")
                    {
                        float FloatingMinHeight = reader.GetFloat(i, 13, 0);
                        float AimDistance = reader.GetFloat(i, 14, 0);
                        float LockAimDistance = reader.GetFloat(i, 15, 0);
                        float AttackedAimDistance = reader.GetFloat(i, 16, 0);
                        float AttackMoveRange = reader.GetFloat(i, 17, 0);
                        float MaxAttackMoveSpeedScale = reader.GetFloat(i, 18, 0);
                        int CommonAttackSkillID = reader.GetInt(i, 19, 0);
                        float AnimatorCrossSpeed = reader.GetFloat(i, 20, 0);
                        bool FPSMode = reader.GetInt(i, 21, 0) == 1 ? true : false;
                        float TopInfoHideInDistance = reader.GetFloat(i, 22, 0);
                        float BloodYOffset = reader.GetFloat(i, 23, 0);
                        float TopNameYOffset = reader.GetFloat(i, 24, 0);
                        string[] CrossHairOffsetStr = reader.GetString(i, 25, "").Split(';');
                        Vector2 CrossHairOffset = new Vector2(float.Parse(CrossHairOffsetStr[0]), float.Parse(CrossHairOffsetStr[1]));
                        bool CrossHairVisible = reader.GetInt(i, 26, 0) == 1 ? true : false;
                        bool MouseAim = reader.GetInt(i, 27, 0) == 1 ? true : false;
                        bool CheckPosition = reader.GetInt(i, 28, 0) == 1 ? true : false;
                        float CreatureHeightInMeters = reader.GetFloat(i, 29, 0);
                        GameObject ScreenCastObject = null/*reader.GetFloat(i, 30, 0)*/;
                        Vector3 CreatureBoundingRad = new Vector3(reader.GetFloat(i, 31, 0), reader.GetFloat(i, 32, 0), reader.GetFloat(i, 33, 0));
                        float InertiaAcceleration = reader.GetFloat(i, 34, 0);
                        float InertiaScale = reader.GetFloat(i, 35, 0);

                        heroInfo.CreatureProperty.floatingMinHeight = FloatingMinHeight;
                        heroInfo.CreatureProperty.aimDistance = AimDistance;
                        heroInfo.CreatureProperty.lockAimDistance = LockAimDistance;
                        heroInfo.CreatureProperty.attackedAimDistance = AttackedAimDistance;
                        heroInfo.CreatureProperty.attackMoveRange = AttackMoveRange;
                        heroInfo.CreatureProperty.maxAttackMoveSpeedScale = MaxAttackMoveSpeedScale;
                        heroInfo.CreatureProperty.commonAttackSkillID = CommonAttackSkillID;
                        heroInfo.CreatureProperty.animatorCrossSpeed = AnimatorCrossSpeed;
                        heroInfo.CreatureProperty.FPSMode = FPSMode;
                        heroInfo.CreatureProperty.TopInfoHideInDistance = TopInfoHideInDistance;
                        heroInfo.CreatureProperty.BloodYOffset = BloodYOffset;
                        heroInfo.CreatureProperty.TopNameYOffset = TopNameYOffset;
                        heroInfo.CreatureProperty.CrossHairOffset = CrossHairOffset;
                        heroInfo.CreatureProperty.CrossHairVisible = CrossHairVisible;
                        heroInfo.CreatureProperty.MouseAim = MouseAim;
                        heroInfo.CreatureProperty.checkPosition = CheckPosition;
                        heroInfo.CreatureProperty.CreatureHeightInMeters = CreatureHeightInMeters;
                        heroInfo.CreatureProperty.ScreenCastObject = ScreenCastObject;
                        heroInfo.CreatureProperty.CreatureBoundingRad = CreatureBoundingRad;
                        heroInfo.CreatureProperty.InertiaAcceleration = InertiaAcceleration;
                        heroInfo.CreatureProperty.InertiaScale = InertiaScale;

                    }
                }
                if (heroInfo.PersonSkinConfig != null)
                {
                    Renderer rightweapon = GetComponentFromPath<Renderer>(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.RightWeapon, ""));
                    Renderer leftweapon = GetComponentFromPath<Renderer>(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.LeftWeapon, ""));
                    Renderer body = GetComponentFromPath<Renderer>(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.Body, ""));
                    Renderer backBody = GetComponentFromPath<Renderer>(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.BackBody, ""));
                    Transform chibangguadian = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.chibangguadian, ""));
                    float OptimitzedDistance = reader.GetFloat(i, (int)HeroPrefabCsvCouml.OptimitzedDistance, 0);
                    int CacheCout = reader.GetInt(i, (int)HeroPrefabCsvCouml.CacheCout, 0);
                    bool OptimizedNearMainHero = reader.GetInt(i, (int)HeroPrefabCsvCouml.OptimizedNearMainHero, 0) == 1 ? true : false;

                    Transform Main = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.Main, ""));
                    Transform Root_M = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.Root_M, ""));
                    Transform BackA_M = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.BackA_M, ""));
                    Transform Root_MCopy = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.Root_MCopy, ""));

                    Transform weaponObject = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.weaponObject, ""));
                    Transform weaponObject2 = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.weaponObject2, ""));

                    bool adjustSpine = reader.GetInt(i, (int)HeroPrefabCsvCouml.adjustSpine, 0) == 1 ? true : false;
                    bool RootAReplace = reader.GetInt(i, (int)HeroPrefabCsvCouml.RootAReplace, 0) == 1 ? true : false;
                    int AttackAnimatorLayer = reader.GetInt(i, (int)HeroPrefabCsvCouml.AttackAnimatorLayer, 0);
                    Vector3 rootAngleWithAim = new Vector3(
                        reader.GetInt(i, (int)HeroPrefabCsvCouml.rootAngleWithAimX, 0),
                        reader.GetInt(i, (int)HeroPrefabCsvCouml.rootAngleWithAimY, 0),
                        reader.GetInt(i, (int)HeroPrefabCsvCouml.rootAngleWithAimZ, 0)
                        );
                    bool progressWeight = reader.GetInt(i, (int)HeroPrefabCsvCouml.progressWeight, 0) == 1 ? true : false;
                    float lookAngleScale = reader.GetFloat(i, (int)HeroPrefabCsvCouml.lookAngleScale, 0);
                    Transform lookRefTransform = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.lookRefTransform, ""));
                    Transform ScreenCastObjectTr = GetTransformFromPath(HeroPrefab.transform, reader.GetString(i, (int)HeroPrefabCsvCouml.ScreenCastObject2, ""));
                    GameObject ScreenCastObject = ScreenCastObjectTr == null ? null : ScreenCastObjectTr.gameObject;
                    int jumpDeltyTick = reader.GetInt(i, (int)HeroPrefabCsvCouml.jumpDeltyTick, 0);
                    float maxFallSpeed = reader.GetFloat(i, (int)HeroPrefabCsvCouml.maxFallSpeed, 0);
                    bool MorphHip = reader.GetInt(i, (int)HeroPrefabCsvCouml.MorphHip, 0) == 1 ? true : false;


                    heroInfo.PersonSkinConfig.RightWeaponRender = rightweapon;
                    heroInfo.PersonSkinConfig.LeftWeaponRender = leftweapon;
                    heroInfo.PersonSkinConfig.BodyRender = body;
                    heroInfo.PersonSkinConfig.BackBodyRender = backBody;
                    heroInfo.PersonSkinConfig.chibangguadian = chibangguadian;
                    heroInfo.PersonSkinConfig.OptimitzedDistance = OptimitzedDistance;
                    heroInfo.PersonSkinConfig.CacheCout = CacheCout;
                    heroInfo.PersonSkinConfig.OptimizedNearMainHero = OptimizedNearMainHero;

                    heroInfo.PersonSkinConfig.Main = Main;
                    heroInfo.PersonSkinConfig.Root_M = Root_M;
                    heroInfo.PersonSkinConfig.BackA_M = BackA_M;
                    heroInfo.PersonSkinConfig.Root_MCopy = Root_MCopy;
                    heroInfo.PersonSkinConfig.weaponObject = weaponObject;
                    heroInfo.PersonSkinConfig.weaponObject2 = weaponObject2;
                    heroInfo.PersonSkinConfig.adjustSpine = adjustSpine;
                    heroInfo.PersonSkinConfig.RootAReplace = RootAReplace;
                    heroInfo.PersonSkinConfig.AttackAnimatorLayer = AttackAnimatorLayer;
                    heroInfo.PersonSkinConfig.rootAngleWithAim = rootAngleWithAim;
                    heroInfo.PersonSkinConfig.progressWeight = progressWeight;
                    heroInfo.PersonSkinConfig.lookAngleScale = lookAngleScale;
                    heroInfo.PersonSkinConfig.lookRefTransform = lookRefTransform;
                    heroInfo.PersonSkinConfig.ScreenCastObject = ScreenCastObject;
                    heroInfo.PersonSkinConfig.jumpDeltyTick = jumpDeltyTick;
                    heroInfo.PersonSkinConfig.maxFallSpeed = maxFallSpeed;
                    heroInfo.PersonSkinConfig.MorphHip = MorphHip;
                }
            }
        }

        static T GetComponentFromPath<T>(Transform tr, string path)
        {
            if (path == "null") return default(T);
            Transform Target = GetTransformFromPath(tr, path);
            if (Target == null) return default(T);

            T t = Target.GetComponent<T>();
            return t;
        }
        static Transform GetTransformFromPath(Transform tr, string path)
        {
            if (path == "null") return null;
            Transform Target = null;
            Transform Father = tr;
            string[] paths = path.Split('/');
            for (int i = 0; i < paths.Length; i++)
            {
                if (paths[i] == tr.name || string.IsNullOrEmpty(paths[i])) continue;
                Target = Father.FindChild(paths[i]);
                if (Target == null)
                {
                    Debug.Log(" Transform 路径 跟踪错误:" + path + "");
                    break;
                }
                Father = Target;
            }
            return Target;
        }
    }

    enum HeroPrefabCsvCouml : int
    {
        PrefabPath,
        PrefabName,
        ModelPath,
        ControllerPath,

        SlopeLimit,
        StepOffset,
        SkinWidth,
        MinMoveDistance,
        CenterX,
        CenterY,
        CenterZ,
        Radius,
        Height,

        FloatingMinHeight,
        AimDistance,
        LockAimDistance,
        AttackedAimDistance,
        AttackMoveRange,
        MaxAttackMoveSpeedScale,
        CommonAttackSkillID,
        AnimatorCrossSpeed,
        FPSMode,
        TopInfoHideInDistance,
        BloodYOffset,
        TopNameYOffset,
        CrossHairOffset,
        CrossHairVisible,
        MouseAim,
        CheckPosition,
        CreatureHeightInMeters,
        ScreenCastObject,
        CreatureBoundingRadX,
        CreatureBoundingRadY,
        CreatureBoundingRadZ,
        InertiaAcceleration,
        InertiaScale,

        RightWeapon,
        LeftWeapon,
        Body,
        BackBody,
        chibangguadian,
        OptimitzedDistance,
        CacheCout,
        OptimizedNearMainHero,
        Main,
        Root_M,
        BackA_M,
        Root_MCopy,
        weaponObject,
        weaponObject2,
        adjustSpine,
        RootAReplace,
        AttackAnimatorLayer,
        rootAngleWithAimX,
        rootAngleWithAimY,
        rootAngleWithAimZ,
        progressWeight,
        lookAngleScale,
        lookRefTransform,
        ScreenCastObject2,
        jumpDeltyTick,
        maxFallSpeed,
        MorphHip,
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render;
using SPELL;
using War;
using GameLogic;
using U3D_Render.Utility;
using UnityEngine.EventSystems;
using DataCenter;
using USpeedUI.UEffect;
using DG.Tweening;
using U3D_Render.CameraControl;
using UIWidgets;

namespace USpeedUI.WarMinimapWnd
{
    public partial class WarMinimapWndView : UIBaseWndView, IEventHandler, IBeginDragHandler, IDragHandler, IEndDragHandler, IPointerDownHandler
    {
        public Material ZoneMaterial = null;
        public ULineRenderer SafeZoneDirTips; //方向指示
        public RectTransform SafeZoneTransform;
        public RectTransform VirusZoneTransform;
        private Transform SafeZoneRenderTransform;
        private Transform VirusZoneRenderTransform;
        private MeshRenderer SafeZoneMeshRender;
        private MeshRenderer VirusZoneMeshRender;
       
        private Material SafeZoneMaterial = null;
        private Material VirusZoneMaterial = null;
        private Color SafeZoneColor = Color.white;
        private Color VirusZoneColor = Color.blue;

        private const float standradRadius = 0.5f;
        private const int standradSize = 512;
        private const float standradCicleWidth = 0.001f;
        private float totalMapWidth = 1;
        private int ParamPropertyID = 0;
        private int ColorStrengthPropertyID = 0;
        private int ColorPropertyID = 0;

        private bool isZoneValid
        {
            get
            {
                return bInit && isOnChijiScene;
            }
        }
        private bool bInit = false;
        private bool isOnChijiScene = false;
        private bool isStarting = false;
        private bool isHaveData = false;

        private Vector2 worldPointToZonePoint(Vector3 worPoint)
        {
            return SceneToMinimap(worPoint) / totalMapWidth;
        }

        private float worldLengthToZoneLength(float worldLength)
        {
            return SceneLengthToMinimap(worldLength) / totalMapWidth;
        }

        /// <summary>
        /// 开始扩散毒圈
        /// </summary>
        /// <param name="safeCenter"></param>
        /// <param name="safeRadius"></param>
        /// <param name="NoxSafeCenter"></param>
        /// <param name="NoxSafeRadius"></param>
        /// <param name="SpreadTimeInSecond"></param>
        public void StartSpreadNoxSafeZone()
        {
            if (!isZoneValid)
            {
                return;
            }
            isHaveData = true;
            EnableZoneUI();

            ApplyMaterialParams(SafeZoneMaterial, worldPointToZonePoint(SafeZone.dstCenter), worldLengthToZoneLength(SafeZone.dstRadius), SafeZoneColor);
            ApplyMaterialParams(VirusZoneMaterial, worldPointToZonePoint(SafeZone.srcCenter), worldLengthToZoneLength(SafeZone.srcRadius), VirusZoneColor);
            isStarting = true;
        }



        /// <summary>
        /// 设置毒圈范围
        /// </summary>
        /// <param name="Center"></param>
        /// <param name="Radius"></param>
        public void SetNoxSafeZone()
        {
            if (!isZoneValid)
            {
                return;
            }
            isHaveData = true;
            EnableZoneUI();

            ApplyMaterialParams(SafeZoneMaterial, worldPointToZonePoint(SafeZone.dstCenter), worldLengthToZoneLength(SafeZone.dstRadius), SafeZoneColor);
            ApplyMaterialParams(VirusZoneMaterial, worldPointToZonePoint(SafeZone.srcCenter), worldLengthToZoneLength(SafeZone.srcRadius), VirusZoneColor);
            isStarting = false;
        }

        private void UpdateZone()
        {
            if (!isZoneValid)
            {
                return;
            }

            if (!isOnChijiScene)
            {
                return;
            }

            if (!isStarting)
            {
                return;
            }

            ApplyMaterialParams(VirusZoneMaterial, worldPointToZonePoint(SafeZone.currPos), worldLengthToZoneLength(SafeZone.currRadius), VirusZoneColor);
            isStarting = SafeZone.isSpreadStarted;
        }

        private void ApplyMaterialParams(Material mat, Vector2 pos,float radius,Color color)
        {
            if (!isZoneValid)
            {
                return;
            }
            if(!mat)
            {
                return;
            }
            Vector4 reslut = Vector4.zero;
            float sizeScale = standradSize / totalMapWidth;
            float radiusScale = standradRadius / radius;
            
            reslut.x = pos.x;
            reslut.y = pos.y;
            reslut.z = radius;
            reslut.w = standradCicleWidth;

            mat.SetVector(ParamPropertyID, reslut);
            mat.SetFloat(ColorStrengthPropertyID, 2.5f);
            mat.SetColor(ColorPropertyID, color);
            return ;
        }

        private void CreateMaterial()
        {
            if(!isZoneValid)
            {
                return;
            }

            if(!SafeZoneMaterial)
            {
                SafeZoneMaterial = new Material(ZoneMaterial);
                SafeZoneMaterial.name = "SafeZoneMaterial-Instance";
                SafeZoneMeshRender.sharedMaterial = SafeZoneMaterial;
                
            }

            if (!VirusZoneMaterial)
            {
                VirusZoneMaterial = new Material(ZoneMaterial);
                VirusZoneMaterial.name = "VirusZoneMaterial-Instance";
                VirusZoneMeshRender.sharedMaterial = VirusZoneMaterial;
            }
        }

        private void EnableZoneUI()
        {
            if (!isZoneValid)
            {
                return;
            }
            CreateMaterial();
            if (SafeZoneRenderTransform)
            {
                SafeZoneRenderTransform.localScale = new Vector3(m_fMinimapWidth, m_fMinimapHeight,1);
                SafeZoneMeshRender.enabled = true;
            }
            if (VirusZoneRenderTransform)
            {
                VirusZoneRenderTransform.localScale = new Vector3(m_fMinimapWidth, m_fMinimapHeight, 1);
                VirusZoneMeshRender.enabled = true;
            }
            EnableDirTipsUI();

        }

        private void DisableZoneUI()
        {
            if (!bInit)
            {
                return;
            }
            if (SafeZoneMeshRender)
            {
                SafeZoneMeshRender.enabled = false;
            }
            if (VirusZoneMeshRender)
            {
                VirusZoneMeshRender.enabled = false;
            }
            DisableDirTipsUI();
            ResNode.DestroyRes(ref SafeZoneMaterial);
            ResNode.DestroyRes(ref VirusZoneMaterial);
        }

        private void EnableDirTipsUI()
        {
            if (!isZoneValid)
            {
                return;
            }

            //if (null == SafeZoneDirTips.Points || SafeZoneDirTips.Points.Length <= 0)
            //{
            //    SafeZoneDirTips.Points = new Vector2[] { Vector3.zero, Vector3.one };
            //}

            //if (SafeZoneDirTips)
            //{
            //    SafeZoneDirTips.gameObject.SetActive(true);
            //}
        }

        private void DisableDirTipsUI()
        {
            if (!bInit)
            {
                return;
            }

            //if (SafeZoneDirTips)
            //{
            //    SafeZoneDirTips.gameObject.SetActive(false);
            //}
        }

        private void UpdateDirTipsUI()
        {
            if(!isZoneValid)
            {
                return;
            }
            if(!isHaveData)
            {
                return;
            }
            //if (U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView))
            //{
            //    return;
            //}

            //Vector3 CamerPos = EntityFactory.MainHeroView.transform.position;

            //if(!SafeZone.IsPointInSafeZone(CamerPos))
            //{
            //    EnableDirTipsUI();
            //    SafeZoneDirTips.Points[0] = SceneToMinimap(CamerPos);
            //    SafeZoneDirTips.Points[1] = SceneToMinimap(SafeZone.dstCenter);
            //    SafeZoneDirTips.SetAllDirty();
                
            //}
            //else
            //{
            //    DisableDirTipsUI();
            //}
        }

        private void OnClose_Zone()
        {
            if (!bInit)
            {
                return;
            }
            isOnChijiScene = false;
            DisableZoneUI();
        }

        private void OnOpen_Zone()
        {
            if (!bInit)
            {
                return;
            }
            isOnChijiScene = GameLogicAPI.getCurGameMode() == Game_ManagedDef.EGameMode.EGM_MCAMP;
            isHaveData = false;
            if (m_fMinimapWidth != m_fMinimapHeight)
            {
                Debug.LogWarning("小地图宽高不一致，毒圈显示将会不正常");
            }
            totalMapWidth = Mathf.Max(m_fMinimapWidth, m_fMinimapHeight);
            DisableZoneUI();
         
        }

        private void Destroy_Zone()
        {
            ResNode.DestroyRes(ref SafeZoneMaterial);
            ResNode.DestroyRes(ref VirusZoneMaterial);
            bInit = false;
        }

        private void Init_Zone()
        {
            if (!SafeZoneTransform || !VirusZoneTransform /*|| !SafeZoneDirTips*/)
            {
                Debug.LogError("SafeZoneTransform 或者 VirusZoneTransform 或者 SafeZoneDirTips 为空");
                return;
            }

            SafeZoneRenderTransform = SafeZoneTransform.Find("Renderer");
            VirusZoneRenderTransform = VirusZoneTransform.Find("Renderer");

            if (!SafeZoneRenderTransform || !VirusZoneRenderTransform)
            {
                Debug.LogError("SafeZoneTransform 或者 VirusZoneTransform 找不到Renderer节点");
                return;
            }

            SafeZoneMeshRender = SafeZoneRenderTransform.GetComponent<MeshRenderer>();
            VirusZoneMeshRender = VirusZoneRenderTransform.GetComponent<MeshRenderer>();
           // SafeZoneDirTips.gameObject.SetActive(false);
            if (!SafeZoneMeshRender || !VirusZoneMeshRender)
            {
                Debug.LogError("SafeZoneRenderTransform 或者 VirusZoneRenderTransform 找不到MeshRenderer组件");
                return;
            }

            if (!ZoneMaterial)
            {
                Debug.LogError("找不到画圈材质");
                return;
            }

            ParamPropertyID = Shader.PropertyToID("_Params");
            ColorStrengthPropertyID = Shader.PropertyToID("_ColorStrength");
            ColorPropertyID = Shader.PropertyToID("_Color");

            //SafeZoneDirTips.LineThickness = 2;
            bInit = true;
        }

    }
}

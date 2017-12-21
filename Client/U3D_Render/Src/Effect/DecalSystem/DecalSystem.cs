using UnityEngine;
using System.Collections.Generic;
using System.Collections;

namespace Effect
{

    public struct DecalBuildInfo
    {
        public Vector3      decalPos;
        public Vector3      decalEulerAngle;
        public Vector3      decalScale;
        public float           decalFadeInTime;
        public float           decalFadeTime;
        public float           decalFadeOutTime;
        public Material     decalMat;
        public Sprite         decalSprite;
    }

    public class DecalShowCube: MonoBehaviour
    {
        public EffectNode node;
    }

    public class DecalSystem
    {
        private static DecalSystem _instance;
        public static DecalSystem Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new DecalSystem();
                return _instance;
            }
        }

        public LayerMask affectedLayers = -1;
        private Dictionary<string, Material> decalMatPool = new Dictionary<string, Material>();
        private Coroutine delayCoroutine = null;
        private float delayTime =0;
        public void DelayBuildDecal(EffectNode node,DecalBuildInfo decalInfo, float delayTime, string decalAltasName = "DecalDefault")
        {
            this.delayTime = delayTime;
            if (delayTime>0)
            {
                delayCoroutine = GameUtil.StartCoroutineHelper(node, DelayFunc(decalInfo, decalAltasName));
            }
            else
            {
                BuildDecal(decalInfo, decalAltasName);
            }
        }

        private IEnumerator DelayFunc(DecalBuildInfo decalInfo, string decalAltasName)
        {
            yield return new WaitForSeconds(delayTime);
            BuildDecal(decalInfo, decalAltasName);
        }

        public bool CheckHitDecal(Vector3 startPos, Vector3 decalRayDir,ref Vector3 decalPos)
        {
            RaycastHit hitInfo;
            int LayerMaskOnBuilding = (1 << LayerMask.NameToLayer(Config.LayerBuilding));
            Ray ray = new Ray(startPos, Vector3.Normalize(decalRayDir));
            if (Physics.Raycast(ray, out hitInfo, 100, LayerMaskOnBuilding))
            {
                Debug.Log(hitInfo.transform.name);
                decalPos = hitInfo.point;
                return true;
            }
            return false;
        }

        public void BuildDecal(DecalBuildInfo decalInfo, string decalAltasName= "DecalDefault")
        {
            Material decalMat = null;
            if (decalInfo.decalMat == null)    //没有专门指定材质球
            {
                if (decalAltasName != "" && decalMatPool.ContainsKey(decalAltasName))
                    decalMat = decalMatPool[decalAltasName];
                else
                {
                    Shader decalShader = AssetBundleManager.GetShader("Decal/Default");
                    decalMat = new Material(decalShader);
                    decalMatPool.Add(decalAltasName, decalMat);
                }
            }
            else
            {
                decalMat = decalInfo.decalMat;
            }
            decalMat.mainTexture = decalInfo.decalSprite.texture;
            Decal decal = CreateDecalObj(decalInfo.decalPos, decalInfo.decalEulerAngle, decalInfo.decalScale);
            decal.SetMaterial(decalMat);
            decal.DecalSprite = decalInfo.decalSprite;

            //淡入淡出
            decal.decalFadeInTime = decalInfo.decalFadeInTime;
            decal.decalFadeTime = decalInfo.decalFadeTime;
            decal.decalFadeOutTime = decalInfo.decalFadeOutTime;

            BuildDecalMesh(decal);
        }

        private Decal CreateDecalObj(Vector3 position,Vector3 eulerAngle, Vector3 scale)
        {
            GameObject decalObj = new GameObject();
            decalObj.layer = LayerMask.NameToLayer(Config.LayerIngoreRayCast);
            Decal decal = decalObj.AddComponent<Decal>();
            decal.m_Filter = decalObj.GetComponent<MeshFilter>();
            decal.m_Render = decalObj.GetComponent<MeshRenderer>();
            decal.transform.position = position;
            decal.transform.eulerAngles = eulerAngle;
            decal.transform.localScale = scale;
            decal.affectedLayers = 1<<LayerMask.NameToLayer(Config.LayerBuilding);
            decal.affectedLayers = decal.affectedLayers & (~(1 << decalObj.layer));
            return decal;
        }

        private void BuildDecalMesh(Decal decal)
        {
            GameObject[] affectedObjects = GetAffectedObjects(decal.GetBounds(), decal.affectedLayers);
            foreach (GameObject go in affectedObjects)
            {
                DecalBuilder.BuildDecalForObject(decal, go);
            }
            DecalBuilder.Push(decal.pushDistance);

            Mesh mesh = DecalBuilder.CreateMesh();
            if (mesh != null)
            {
                mesh.name = "DecalMesh";
                decal.m_Filter.mesh = mesh;
            }
        }

        private GameObject[] GetAffectedObjects(Bounds bounds, LayerMask affectedLayers)
        {
            MeshCollider[] colliders = (MeshCollider[])GameObject.FindObjectsOfType<MeshCollider>();
            List<GameObject> objects = new List<GameObject>();
            foreach (MeshCollider r in colliders)
            {
                if (!r.enabled) continue;
                if (!IsLayerContains(affectedLayers, r.gameObject.layer)) continue;
                if (r.GetComponent<Decal>() != null) continue;

                if (bounds.Intersects(r.bounds))
                {
                    objects.Add(r.gameObject);
                }
            }
            return objects.ToArray();
        }

        private static bool IsLayerContains(LayerMask mask, int layer)
        {
            return (mask.value & 1 << layer) != 0;
        }

    }
}

using UnityEngine;
using System.Collections.Generic;
using System.Collections;

namespace Effect
{
    [RequireComponent(typeof(MeshFilter))]
    [RequireComponent(typeof(MeshRenderer))]
    public class Decal : MonoBehaviour
    {
        public Material DecalMaterial;
        public Sprite DecalSprite;

        public float maxAngle = 90.0f;
        public float pushDistance = 0.009f;
        public LayerMask affectedLayers = -1;

        /// <summary>
        /// 只影响这些物体,如果这个不为空，其他的会被忽略
        /// </summary>
        public List<GameObject> OnlyAffectThisGameObject = new List<GameObject>();
        public float decalFadeInTime = 0f;
        public float decalFadeTime = 0.5f;
        public float decalFadeOutTime = 2f;
        float StartTime = 0;
        void Start()
        {
            StartTime = Time.time;
            int indexCount = this.GetComponent<MeshFilter>().mesh.vertexCount;
            for (int i = 0; i < indexCount; i++)
                colorList.Add(Color.clear);
            StartCoroutine(AutoDestroy());

        }
        private List<Color> colorList = new List<Color>();
        void Update()
        {
            float deltaTime = Time.time - StartTime;
            float c_alpha = 0;
            if (deltaTime < decalFadeInTime)        //淡入阶段
            {
                c_alpha =  (Time.time - StartTime) / decalFadeInTime;
            }
            else if(deltaTime >= decalFadeInTime && deltaTime < decalFadeInTime + decalFadeTime)    //持续阶段
            {
                //不变
                c_alpha = 1;
            }
            else if (deltaTime >= decalFadeInTime + decalFadeTime && deltaTime < decalFadeInTime + decalFadeTime + decalFadeOutTime)    //淡出阶段
            {
                c_alpha = 1 - (Time.time - StartTime - decalFadeInTime - decalFadeTime) / decalFadeOutTime;
            }
            for (int i = 0; i < colorList.Count; i++)
                colorList[i] = new Color(c_alpha,0,0);
            this.GetComponent<MeshFilter>().mesh.SetColors(colorList);
        }

        IEnumerator AutoDestroy()
        {
            yield return new WaitForSeconds(decalFadeInTime + decalFadeTime + decalFadeOutTime);
            GameObject.Destroy(this.gameObject);
        }

        /// <summary>
        /// 是否有效
        /// </summary>
        public bool IsVilad
        {
            get
            {
                return bInit;
            }
        }

        private static bool IsLayerContains(LayerMask mask, int layer)
        {
            return (mask.value & 1 << layer) != 0;
        }

        //void OnDrawGizmosSelected()
        //{
        //    Gizmos.matrix = transform.localToWorldMatrix;
        //    Gizmos.DrawWireCube( Vector3.zero, Vector3.one );
        //}

        public MeshFilter m_Filter;
        public MeshRenderer m_Render;
        private bool bInit = false;

        public void SetMaterial(Material mat)
        {
            if (m_Render != null)
                m_Render.sharedMaterial = mat;
        }



        public Bounds GetBounds()
        {
            Vector3 size = transform.lossyScale;
            Vector3 min = -size / 2f;
            Vector3 max = size / 2f;

            Vector3[] vts = new Vector3[] {
			new Vector3(min.x, min.y, min.z),
			new Vector3(max.x, min.y, min.z),
			new Vector3(min.x, max.y, min.z),
			new Vector3(max.x, max.y, min.z),

			new Vector3(min.x, min.y, max.z),
			new Vector3(max.x, min.y, max.z),
			new Vector3(min.x, max.y, max.z),
			new Vector3(max.x, max.y, max.z),
		};

            for (int i = 0; i < 8; i++)
            {
                vts[i] = transform.TransformDirection(vts[i]);
            }

            min = max = vts[0];
            foreach (Vector3 v in vts)
            {
                min = Vector3.Min(min, v);
                max = Vector3.Max(max, v);
            }

            return new Bounds(transform.position, max - min);
        }

    }
}

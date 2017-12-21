//using UnityEngine;
//using System.Collections;
//using System;
//namespace Effect
//{
//    public class ForceShield : ShieldBase
//    {
//        public event EventHandler OnFieldHit;

//        public GameObject ShieldObj = null;

//        private Material m_FiledMat;
//        private MeshFilter m_FieldMesh;

//        // Number of controllable interpolators (impact points)
//        private int interpolators = 24;

//        // Used to access shader variables with int id instead of string name
//        private int[] shaderPosID, shaderPowID;

//        // Data containing xyz coordinate of impact and alpha in w for each interpolator    
//        private Vector4[] shaderPos;

//        // Current active interpolator
//        int curProp = 0;
//        // Timer used to advance trough interpolators
//        float curTime = 0;

//        ///// <summary>
//        ///// 护盾物体，这个物体上必须有MeshRender和MeshFilter，不支持SkinnedMeshRender
//        ///// </summary>
//        //public GameObject Field;


//        /// <summary>
//        /// 护盾扩散速度
//        /// </summary>
//        public float DecaySpeed = 2.0f;

//        /// <summary>
//        /// 护盾产生间隔
//        /// </summary>
//        public float ReactSpeed = 0.1f;

//        // Non-uniform scale correction
//        public bool FixNonUniformScale;

//        private EventArgs tempEventArgs = new EventArgs();

//        // INITIALIZATION
//        public override void Start()
//        {
//            // Cache required components
//            base.Start();

//            m_FiledMat = ShieldObj.GetComponent<Renderer>().material;
//            m_FieldMesh = ShieldObj.GetComponent<MeshFilter>();
            

//            // Generate unique IDs for optimised performance
//            // since script has to access them each frame
//            shaderPosID = new int[interpolators];
//            shaderPowID = new int[interpolators];

//            for (int i = 0; i < interpolators; i++)
//            {
//                shaderPosID[i] = Shader.PropertyToID("_Pos_" + i.ToString());
//                shaderPowID[i] = Shader.PropertyToID("_Pow_" + i.ToString());
//            }

//            // Initialize data array
//            shaderPos = new Vector4[interpolators];

//        }


//        public void OnTriggerEnter(Collider other)
//        {
//            OnHit(other.bounds.center);
//        }

//        /// <summary>
//        /// 被击中
//        /// </summary>
//        /// <param name="hitPoint">位置</param>
//        /// <param name="bPrecisionPoint">是否是精确的点，如果为false，会调用Collider的点重新计算</param>
//        public override void OnHit(Vector3 hitPoint, bool bPrecisionPoint = true)
//        {

//            // Check reaction interval
//            if (curTime >= ReactSpeed)
//            {
//                if (!bPrecisionPoint)
//                {
//                    hitPoint = m_Collider.ClosestPointOnBounds(hitPoint);
//                }
//                // Hit point coordinates are transformed into local space
//                Vector4 newHitPoint = m_FieldMesh.transform.InverseTransformPoint(hitPoint);

//                // Clamp alpha
//                //newHitPoint.w = Mathf.Clamp(hitAlpha, 0.0f, 1.0f); 
//                newHitPoint.w = Mathf.Clamp(1.0f, 0.0f, 1.0f);

//                // Store new hit point data using current counter
//                shaderPos[curProp] = newHitPoint;

//                // Fix non-uniform scale
//                if (FixNonUniformScale)
//                {
//                    if (!Mathf.Approximately(transform.lossyScale.x, transform.lossyScale.y) || !Mathf.Approximately(transform.lossyScale.y, transform.lossyScale.z) || !Mathf.Approximately(transform.lossyScale.y, transform.lossyScale.z))
//                    {
//                        shaderPos[curProp].x *= transform.lossyScale.x;
//                        shaderPos[curProp].y *= transform.lossyScale.y;
//                        shaderPos[curProp].z *= transform.lossyScale.z;
//                    }
//                }


//                // Send hitPower into a shader
//                //m_FiledMat.SetFloat(shaderPowID[curProp], hitPower);
//                m_FiledMat.SetFloat(shaderPowID[curProp], 0);
//                // Reset timer and advance counter
//                curTime = 0.0f;
//                curProp++;
//                if (curProp == interpolators) curProp = 0;
//            }

//            if (null != OnFieldHit)
//            {
//                OnFieldHit(this, tempEventArgs);
//            }
//        }

//        public override void Play()
//        {
            
//        }

//        public override void Stop()
//        {
            
//        }

//        // Called each frame to pass values into a shader
//        void FadeMask()
//        {
//            for (int i = 0; i < interpolators; i++)
//            {
//                if (shaderPos[i].w > 0f)
//                {
//                    // Lerp alpha value for current interpolator
//                    shaderPos[i].w = Mathf.Lerp(shaderPos[i].w, -0.0001f, Time.deltaTime * DecaySpeed);
//                    shaderPos[i].w = Mathf.Clamp(shaderPos[i].w, 0f, 1f);
//                    // Assign new value to a shader variable
//                    m_FiledMat.SetVector(shaderPosID[i], shaderPos[i]);
//                }
//            }
//        }

//        // UPDATE
//        public void Update()
//        {
//            // Advance response timer
//            curTime += Time.deltaTime;
//            // Update shader each frame
//            FadeMask();
//        }
//    }

//}

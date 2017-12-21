using UnityEngine;
using System.Collections;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using System;
using System.Collections.Generic;

namespace USpeedUI.UEffect
{
    public class UParticleMask : UIBehaviour
    {
        private RectTransform m_nRecttransform;
        protected RectTransform rectTransform
        {
            get
            {
                if (m_nRecttransform == null)
                    m_nRecttransform = transform as RectTransform;
                return m_nRecttransform;
            }
        }

        private ParticleSystem[] particlesSystems;
        public List<Shader> UseMaskShader;
        // Use this for initialization
        protected override void Start()
        {
            particlesSystems = transform.GetComponentsInChildren<ParticleSystem>();
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            m_nCount = 0;
            particlesSystems = null;
        }

        protected override void OnDisable()
        {
            base.OnDisable();
        }

        protected void LateUpdate()
        {
            if (m_nCount < 5)
            {
                m_nCount++;
               if(m_nCount == 4)
                Change();
            }

        }
                
        /// <summary>
        /// lateupdate里限制执行次数,粒子初始化先后顺序不确定
        /// </summary>
        private int m_nCount;
        public void Change()
        {
            float minX, minY, maxX, maxY;

            Vector3[] corners = new Vector3[4];
            RectTransform rectTransform = transform as RectTransform;
            rectTransform.GetWorldCorners(corners);
            Renderer render = null;


            minX = (int)((corners[0].x) * 100) / 100f;
            minY = (int)((corners[0].y) * 100) / 100f; 
            maxX = (int)((corners[2].x) * 100) / 100f; 
            maxY = (int)((corners[2].y) * 100) / 100f; 

            //if (particlesSystems == null || particlesSystems.Length <= 0)
            {
                particlesSystems = transform.GetComponentsInChildren<ParticleSystem>();
                foreach (ParticleSystem particleSystem in particlesSystems)
                {
                    render = particleSystem.GetComponent<Renderer>();

                    if(UseMaskShader != null && UseMaskShader.Count > 0)
                    {
                        if (!UseMaskShader.Contains(render.material.shader))
                            continue;
                    }

                    render.material = new Material(render.material);
                    render.material.name = render.material.shader.name + "_ins";
                }

            }


            foreach (ParticleSystem particleSystem in particlesSystems)
            {
                render = particleSystem.GetComponent<Renderer>();
                render.material.SetFloat("_MinX", minX);
                render.material.SetFloat("_MinY", minY);
                render.material.SetFloat("_MaxX", maxX);
                render.material.SetFloat("_MaxY", maxY);
            }
        }
    }
    
}
